/**
 * @file comm-token.hpp
 *
 * @brief comm-token.hpp is an interface for building and using the EAP Token communication pattern.
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef COMM_TOKEN_HPP
#define COMM_TOKEN_HPP

// STL Includes
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <vector>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <mpi/mpi.hpp>

// Internal Includes
#include <error-macros.hpp>
#include <error.hpp>
#include <utility-addressing.hpp>
#include <utility-fortran_index.hpp>
#include <utility-kokkos.hpp>
#include <utility-memory.hpp>

// Local Includes
#include "comm-internal-typestr.hpp"
#include "comm-patterns.hpp"
#include "comm-reserved_tags.hpp"
#include "comm-timer.hpp"

namespace eap {
namespace comm {

namespace internal {
void BuildGlobalBase(mpi::Comm comm,
                     uint32_t num_local_cell,
                     std::vector<std::uint32_t> &numcells,
                     std::vector<std::uint64_t> &bases);

struct PeCellAddress {
    mpi::rank_t rank;
    local_index_t local_address;
};
PeCellAddress PeAndLocalAddress(std::vector<global_index_t> const &rank_bases,
                                global_index_t global_address);

struct CopyFromTo {
    std::vector<std::size_t> copy_from;
    std::vector<std::size_t> copy_to;
    std::vector<std::size_t> zero;
};
CopyFromTo BuildCopyInfo(mpi::rank_t mype,
                         std::span<FortranLocalIndex const> home_addresses,
                         std::span<utility::NonNegativeInteger<mpi::rank_t> const> away_pe,
                         std::span<OptionalFortranLocalIndex const> away_address);

struct Segment {
    mpi::rank_t rank;
    std::size_t begin;
    std::size_t length;
};

template <typename View>
constexpr auto is_hostspace_view_v = Kokkos::is_view<View>::value
    &&Kokkos::SpaceAccessibility<eap::HostMemorySpace, typename View::memory_space>::assignable;

template <typename InputView, typename OutputView, typename ValueType>
constexpr auto are_views_compatible_v = is_hostspace_view_v<OutputView>
    &&is_hostspace_view_v<InputView> &&std::is_same<typename OutputView::non_const_data_type,
                                                    typename OutputView::data_type>::value;

std::size_t RecvScratchArraySize(std::uint32_t desired_max_gs_recv_size,
                                 std::uint32_t unit_size,
                                 std::uint32_t row_size,
                                 std::vector<Segment> const &segments);

class RecvRequestCompletionStateMachine {
  public:
    RecvRequestCompletionStateMachine(std::vector<mpi::UniqueRequest> &recv_requests,
                                      bool require_default_order);

    bool get_next_requests(std::vector<int> &completed);

  private:
    std::vector<mpi::UniqueRequest> &recv_requests_;
    int num_completed_ = 0;

    bool require_default_order_;

    std::vector<mpi::rank_t> requests_completed_;
};
} // namespace internal

class TokenBuilder;

/// The operation to perform on data exchanged via Token.
enum class TokenOperation {
    /// Performs a simple copy of remote data into the local buffer.
    /// For Get operations, zeroes cells for negative remote addresses.
    Copy,
    /// Adds the received data into local buffer.
    Add,
    /// Subtracts the received data from its mapped value in the local buffer.
    Sub,
    /// Sets the target cell to the minimum of the received value and the existing value.
    Min,
    /// Sets the target cell to the maximum of the received value and the existing value.
    Max,
};

/**
 * @brief
 *  Token is an object used for collective communication of neighbor data. Tokens are built
 *  using the TokenBuilder interface. They are typically used to exchange ghost cell data.
 *
 *  See TokenBuilder documentation to understand how to construct a Token.
 */
class Token {
    friend class TokenBuilder;

    enum class DoWhich { Gather, Scatter };

  public:
    size_t GetHomeNum() const { return home_segments_.size(); }

    size_t GetHomeSize() const { return home_index_.size(); }

    void FillHomeArrays(std::span<mpi::rank_t> ranks,
                        std::span<eap::utility::FortranIndex<local_index_t>> los,
                        std::span<local_index_t> lengths,
                        std::span<eap::utility::FortranIndex<local_index_t>> indices) const;

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving the
     * requested remote addresses.
     *
     * This overload accepts 1D Kokkos views. Each View's data_type must be the same MPI-compatible
     * type.
     *
     * @tparam InputView
     *  1D Kokkos View type for input.
     * @tparam OutputView
     *  1D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param output
     *  This rank's received neighbor data. It must be large enough to receive data according to the
     *  supplied home_addresses array.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void Get(TokenOperation dowhat, InputView const &input, OutputView &output) {
        using namespace eap::utility::kokkos;

        EAP_COMM_TIME_FUNCTION("eap::comm::Token::Get<" +
                               std::string(internal::type_to_str<ValueType>::name()) + ">");

        static_assert(Kokkos::is_view<InputView>::value && Kokkos::is_view<OutputView>::value,
                      "Token::Get input and output must be Kokkos views.");

        static_assert(
            InputView::rank == 1 && OutputView::rank == 1,
            "Token::Get is only compatible with rank 1 Views. Use Token::PutV for rank 2 Views");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "InputView and OutputView do not match");

        EE_DIAG_PRE

        EE_ASSERT(input.extent(0) >= minimum_scatter_size_,
                  "This token expects rank " << comm_.rank() << " to have " << minimum_scatter_size_
                                             << " home cells. The provided buffer only contains "
                                             << input.extent(0) << " home cells.");

        EE_ASSERT(output.extent(0) >= minimum_gather_size_,
                  "This token expects rank "
                      << comm_.rank() << " to receive " << minimum_gather_size_
                      << " away cells. The provided buffer only contains " << output.extent(0)
                      << " away cells.");

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type[1],
                     typename InputView::array_layout,
                     eap::HostMemorySpace>
            input_host = Convert1DTo2D(input);
        Kokkos::View<typename OutputView::non_const_data_type[1],
                     typename OutputView::array_layout,
                     eap::HostMemorySpace>
            output_host = Convert1DTo2D(output);

        GatherScatter<decltype(input_host), decltype(output_host), ValueType>(
            DoWhich::Gather, dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving the
     * requested remote addresses.
     *
     * This overload accepts raw, contiguous arrays.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param input_length
     *  The length of the input buffer.
     * @param output
     *  This rank's received neighbor data. It must be large enough to receive data according to the
     *  supplied home_addresses array.
     * @param output_length
     *  The length of the output buffer.
     */
    template <typename T>
    void Get(TokenOperation dowhat,
             T const *input,
             std::size_t input_length,
             T *output,
             std::size_t output_length) {
        Kokkos::View<T const *, Kokkos::LayoutRight, eap::HostMemorySpace> input_kokkos =
            Kokkos::View<T const *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(input,
                                                                                   input_length);

        Kokkos::View<T *, Kokkos::LayoutRight, eap::HostMemorySpace> output_kokkos =
            Kokkos::View<T *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(output, output_length);

        Get(dowhat, input_kokkos, output_kokkos);
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving the
     * requested remote addresses.
     *
     * This overload accepts the data as vector references.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param output
     *  This rank's received neighbor data. It must be large enough to receive data according to the
     *  supplied home_addresses array.
     */
    template <typename T>
    void Get(TokenOperation dowhat, std::vector<T> const &input, std::vector<T> &output) {
        Get(dowhat, input.data(), input.size(), output.data(), output.size());
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving the
     * requested remote addresses.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @return std::vector<T>
     *  This rank's received neighbor data. It will be just large enough to receive data according
     *  to the supplied home_addresses array. i.e. it will be large enough to receive the largest
     *  index in home_addresses.
     */
    template <typename T>
    std::vector<T> Get(TokenOperation dowhat, std::vector<T> const &input) {
        std::vector<T> output(minimum_gather_size_);
        Get(dowhat, input, output);
        return output;
    }

    /**
     * @brief Collective operation. Exchanges data row-wise according to token neighbor data,
     * receiving the requested remote addresses.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param output
     *  This rank's received neighbor data. It must be large enough to receive data according to the
     *  supplied home_addresses array.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void GetV(TokenOperation dowhat, InputView const &input, OutputView &output) {
        EAP_COMM_TIME_FUNCTION(
            "eap::comm::Token::GetV<" +
            std::string(internal::type_to_str<typename InputView::non_const_value_type>::name()) +
            ">");

        static_assert(InputView::rank == 2 && OutputView::rank == 2,
                      "Only 2D views are compatible with GetV");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "The input and output views must be compatible.");

        EE_DIAG_PRE

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type,
                     typename InputView::array_layout,
                     eap::HostMemorySpace>
            input_host = input;
        Kokkos::View<typename OutputView::non_const_data_type,
                     typename OutputView::array_layout,
                     eap::HostMemorySpace>
            output_host = output;

        GatherScatter(DoWhich::Gather, dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data row-wise according to token neighbor data,
     * receiving the requested remote addresses.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @return
     *  This rank's received neighbor data. It will be just large enough to receive data according
     *  to the supplied home_addresses array. i.e. it will be large enough to receive the largest
     *  index in home_addresses.
     */
    template <typename InputView,
              typename OutputView =
                  Kokkos::View<typename InputView::non_const_data_type, eap::HostMemorySpace>>
    OutputView GetV(TokenOperation dowhat, InputView const &input) {
        static_assert(internal::is_hostspace_view_v<InputView>,
                      "The Kokkos View must be HostSpace accessible.");

        OutputView output("get_v_output", minimum_gather_size_, input.extent(1));
        GetV(dowhat, input, output);
        return output;
    }

    /**
     * @brief Collective operation. Exchanges data column-wise according to token neighbor data,
     * receiving the requested remote addresses.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param output
     *  This rank's received neighbor data. It must be large enough to receive data according to the
     *  supplied home_addresses array.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void GetVInv(TokenOperation dowhat, InputView const &input, OutputView &output) {
        using eap::utility::kokkos::Transpose;
        using eap::utility::kokkos::transpose_layout;

        EAP_COMM_TIME_FUNCTION(
            "eap::comm::Token::GetVInv<" +
            std::string(internal::type_to_str<typename InputView::non_const_value_type>::name()) +
            ">");

        static_assert(InputView::rank == 2 && OutputView::rank == 2,
                      "Only 2D views are compatible with get_inv");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "The input and output views must be compatible.");

        EE_DIAG_PRE

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type,
                     transpose_layout<typename InputView::array_layout>,
                     eap::HostMemorySpace>
            input_host = Transpose(input);
        Kokkos::View<typename OutputView::non_const_data_type,
                     transpose_layout<typename OutputView::array_layout>,
                     eap::HostMemorySpace>
            output_host = Transpose(output);

        GetV(dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data column-wise according to token neighbor data,
     * receiving the requested remote addresses.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos View type for output. Defaults to a Kokkos::View with an equivalent DataType to
     *  the DataType of InputView and available in HostSpace with default layout.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @return OutputView
     *  This rank's received neighbor data. It will be just large enough to receive data according
     *  to the supplied home_addresses array. i.e. it will be large enough to receive the largest
     *  index in home_addresses.
     */
    template <typename InputView,
              typename OutputView =
                  Kokkos::View<typename InputView::non_const_data_type, eap::HostMemorySpace>>
    OutputView GetVInv(TokenOperation dowhat, InputView const &input) {
        static_assert(internal::is_hostspace_view_v<InputView>,
                      "The Kokkos View must be HostSpace accessible.");

        OutputView output("get_v_inv_output", minimum_gather_size_, input.extent(1));
        GetVInv(dowhat, input, output);
        return output;
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving data
     * in the home format.
     *
     * This overload accepts 1D Kokkos views. Each View's data_type must be the same MPI-compatible
     * type.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos View type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @param output
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void Put(TokenOperation dowhat, InputView const &input, OutputView &output) {
        using namespace eap::utility::kokkos;

        EAP_COMM_TIME_FUNCTION(
            "eap::comm::Token::Put<" +
            std::string(internal::type_to_str<typename InputView::non_const_value_type>::name()) +
            ">");

        static_assert(Kokkos::is_view<InputView>::value && Kokkos::is_view<OutputView>::value,
                      "Token::Put input and output must be Kokkos views.");

        static_assert(
            InputView::rank == 1 && OutputView::rank == 1,
            "Token::Put is only compatible with rank 1 Views. Use Token::PutV for rank 2 Views");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "InputView and OutputView do not match");

        EE_DIAG_PRE

        EE_ASSERT(input.extent(0) >= minimum_gather_size_,
                  "This token expects rank " << comm_.rank() << " to have " << minimum_gather_size_
                                             << " away cells. The provided buffer only contains "
                                             << input.extent(0) << " away cells.");

        EE_ASSERT(output.extent(0) >= minimum_scatter_size_,
                  "This token expects rank "
                      << comm_.rank() << " to receive " << minimum_scatter_size_
                      << " home cells. The provided buffer only contains " << output.extent(0)
                      << " home cells.");

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type[1],
                     typename InputView::array_layout,
                     eap::HostMemorySpace>
            input_host = Convert1DTo2D(input);
        Kokkos::View<typename OutputView::non_const_data_type[1],
                     typename OutputView::array_layout,
                     eap::HostMemorySpace>
            output_host = Convert1DTo2D(output);

        GatherScatter(DoWhich::Scatter, dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving data
     * in the home format.
     *
     * This overload accepts raw, contiguous arrays.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @param input_length
     *  The length of the input buffer.
     * @param output
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     * @param output_length
     *  The length of the output buffer.
     */
    template <typename T>
    void Put(TokenOperation dowhat,
             T const *input,
             std::size_t input_length,
             T *output,
             std::size_t output_length) {
        Kokkos::View<T const *, Kokkos::LayoutRight, eap::HostMemorySpace> input_kokkos =
            Kokkos::View<T const *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(input,
                                                                                   input_length);

        Kokkos::View<T *, Kokkos::LayoutRight, eap::HostMemorySpace> output_kokkos =
            Kokkos::View<T *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(output, output_length);

        Put(dowhat, input_kokkos, output_kokkos);
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving data
     * in the home format.
     *
     * This overload accepts the data as vector references.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @param output
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     */
    template <typename T>
    void Put(TokenOperation dowhat, std::vector<T> const &input, std::vector<T> &output) {
        Put(dowhat, input.data(), input.size(), output.data(), output.size());
    }

    /**
     * @brief Collective operation. Exchanges data according to token neighbor data, receiving data
     * in the home format.
     *
     * This overload accepts the data as vector references.
     *
     * @tparam T
     *  An MPI-compatible type.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @return std::vector<T>
     *  This rank's token data. It will be just large enough to contain all local cells.
     */
    template <typename T>
    std::vector<T> Put(TokenOperation dowhat, std::vector<T> const &input) {
        std::vector<T> output(minimum_scatter_size_);
        Put(dowhat, input, output);
        return output;
    }

    /**
     * @brief Collective operation. Exchanges data row-wise according to token neighbor data,
     * receiving data in the home format.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @param output
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void PutV(TokenOperation dowhat, InputView const &input, OutputView &output) {
        EAP_COMM_TIME_FUNCTION(
            "eap::comm::Token::PutV<" +
            std::string(internal::type_to_str<typename InputView::non_const_value_type>::name()) +
            ">");

        static_assert(InputView::rank == 2 && OutputView::rank == 2,
                      "Only 2D views are compatible with PutV");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "The input and output views must be compatible.");

        EE_DIAG_PRE

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type,
                     typename InputView::array_layout,
                     eap::HostMemorySpace>
            input_host = input;
        Kokkos::View<typename OutputView::non_const_data_type,
                     typename OutputView::array_layout,
                     eap::HostMemorySpace>
            output_host = output;

        GatherScatter(DoWhich::Scatter, dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data row-wise according to token neighbor data,
     * receiving data in the home format.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @return OutputView
     *  This rank's token data. It will be just large enough to contain all local cells.
     */
    template <typename InputView,
              typename OutputView =
                  Kokkos::View<typename InputView::non_const_data_type, eap::HostMemorySpace>>
    OutputView PutV(TokenOperation dowhat, InputView const &input) {
        static_assert(internal::is_hostspace_view_v<InputView>,
                      "The Kokkos View must be HostSpace accessible.");

        OutputView output("put_v_output", minimum_gather_size_, input.extent(1));
        PutV(dowhat, input, output);
        return output;
    }

    /**
     * @brief Collective operation. Exchanges data column-wise according to token neighbor data,
     * receiving data in the home format.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @param output
     *  This rank's token data. It must provide a minimum of the num_cells of data.
     */
    template <typename InputView,
              typename OutputView,
              typename ValueType = typename InputView::non_const_value_type>
    void PutVInv(TokenOperation dowhat, InputView const &input, OutputView &output) {
        using eap::utility::kokkos::Transpose;
        using eap::utility::kokkos::transpose_layout;

        EAP_COMM_TIME_FUNCTION(
            "eap::comm::Token::PutVInv<" +
            std::string(internal::type_to_str<typename InputView::non_const_value_type>::name()) +
            ">");

        static_assert(InputView::rank == 2 && OutputView::rank == 2,
                      "Only 2D views are compatible with PutV");

        static_assert(internal::are_views_compatible_v<InputView, OutputView, ValueType>,
                      "The input and output views must be compatible.");

        EE_DIAG_PRE

        // Explicitly cast Kokkos::Views to the exact requirements of Token::GatherScatter - this
        // reduces extraneously instantiations of Token::GatherScatter.
        Kokkos::View<typename InputView::const_data_type,
                     transpose_layout<typename InputView::array_layout>,
                     eap::HostMemorySpace>
            input_host = Transpose(input);
        Kokkos::View<typename OutputView::non_const_data_type,
                     transpose_layout<typename OutputView::array_layout>,
                     eap::HostMemorySpace>
            output_host = Transpose(output);

        PutV(dowhat, input_host, output_host);

        EE_DIAG_POST_MSG("dowhat = " << (int)dowhat)
    }

    /**
     * @brief Collective operation. Exchanges data column-wise according to token neighbor data,
     * receiving data in the home format.
     *
     * @tparam InputView
     *  2D Kokkos View type for input.
     * @tparam OutputView
     *  2D Kokkos view type for output.
     * @param dowhat
     *  The requested operation to perform on the received data.
     * @param input
     *  This rank's scatter data. It will be just large enough to supply data according
     *  to the supplied home_addresses array. i.e. it will be large enough to supply the largest
     *  index in home_addresses.
     * @return OutputView
     *  This rank's token data. It will be just large enough to contain all local cells.
     */
    template <typename InputView,
              typename OutputView =
                  Kokkos::View<typename InputView::non_const_data_type, eap::HostMemorySpace>>
    OutputView PutVInv(TokenOperation dowhat, InputView const &input) {
        static_assert(internal::is_hostspace_view_v<InputView>,
                      "The Kokkos View must be HostSpace accessible.");

        OutputView output("put_v_inv_output", minimum_gather_size_, input.extent(1));
        PutVInv(dowhat, input, output);
        return output;
    }

  private:
    mpi::Comm comm_;
    std::size_t minimum_gather_size_ = 0;
    std::size_t minimum_scatter_size_ = 0;
    std::vector<std::size_t> zero_;
    std::vector<std::size_t> copy_from_info_;
    std::vector<std::size_t> copy_to_info_;
    std::vector<internal::Segment> home_segments_;
    std::vector<local_index_t> home_index_;
    std::vector<internal::Segment> away_segments_;
    std::vector<local_index_t> away_index_;

    bool has_target_max_gs_receive_size_ = false;
    std::uint32_t target_max_gs_receive_size_ = 0;

    bool require_rank_order_completion_ = false;

    Token(mpi::Comm comm,
          std::size_t minimum_gather_size,
          std::size_t minimum_scatter_size,
          std::vector<std::size_t> &&zero,
          std::vector<std::size_t> &&copy_from_info,
          std::vector<std::size_t> &&copy_to_info,
          std::vector<internal::Segment> &&home_segments,
          std::vector<local_index_t> &&home_index,
          std::vector<internal::Segment> &&away_segments,
          std::vector<local_index_t> &&away_index,
          bool has_target_max_gs_receive_size,
          std::uint32_t target_max_gs_receive_size,
          bool require_rank_order_completion);

    std::vector<std::size_t> const &GetCopyFrom(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return copy_from_info_;
        } else {
            return copy_to_info_;
        }
    }

    std::vector<std::size_t> const &GetCopyTo(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return copy_to_info_;
        } else {
            return copy_from_info_;
        }
    }

    std::vector<internal::Segment> const &GetRecvSegments(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return home_segments_;
        } else {
            return away_segments_;
        }
    }

    std::vector<local_index_t> const &GetRecvIndex(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return home_index_;
        } else {
            return away_index_;
        }
    }

    std::vector<internal::Segment> const &GetSendSegments(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return away_segments_;
        } else {
            return home_segments_;
        }
    }

    std::vector<local_index_t> const &GetSendIndex(DoWhich dowhich) const {
        if (dowhich == DoWhich::Gather) {
            return away_index_;
        } else {
            return home_index_;
        }
    }

    template <typename T>
    std::pair<size_t, std::unique_ptr<T[]>>
    GetRecvScratch(std::uint32_t row_size, std::vector<internal::Segment> const &segments) {
        EE_PRELUDE

        size_t recv_scratch_size = 0;
        if (has_target_max_gs_receive_size_) {
            recv_scratch_size = internal::RecvScratchArraySize(
                target_max_gs_receive_size_, sizeof(T), row_size, segments);
        } else {
            for (auto &segment : segments) {
                recv_scratch_size += segment.length * row_size;
            }
        }

        return std::make_pair(
            recv_scratch_size,
            EE_CHECK(std::unique_ptr<T[]>(new T[recv_scratch_size]),
                     "Not enough memory to allocate a receive scratch array of size "
                         << recv_scratch_size));
    }

    std::vector<internal::Segment>::const_iterator
    GetScratchArrayDimensions(std::size_t recv_scratch_size,
                              std::size_t row_size,
                              std::vector<internal::Segment>::const_iterator begin,
                              std::vector<internal::Segment>::const_iterator end) {
        size_t low = 0;
        for (auto it = begin; it != end; it++) {
            low += it->length;

            if (low * row_size >= recv_scratch_size) {
                return it + 1;
            }
        }

        return end;
    }

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void GatherScatter(DoWhich dowhich,
                       TokenOperation dowhat,
                       InputView const &input,
                       OutputView &output) {
        using namespace comm::internal;

        using Kokkos::ALL;
        using mpi::UniqueRequest;
        using std::size_t;
        using std::vector;

        using OutputType = typename OutputView::non_const_value_type;

        EE_DIAG_PRE

        EE_ASSERT_EQ(input.extent(1),
                     output.extent(1),
                     "Input (dims = (" << input.extent(0) << "," << input.extent(1)
                                       << ")) and output (dims = (" << output.extent(0) << ","
                                       << output.extent(1)
                                       << ")) must have the same number of columns");

        EE_ASSERT(dowhich == DoWhich::Gather || dowhich == DoWhich::Scatter,
                  "The value of dowhich (" << (int)dowhich << ") is invalid.");

        auto const row_size = input.extent(1);

        auto const &copy_from = GetCopyFrom(dowhich);
        auto const &copy_to = GetCopyTo(dowhich);
        auto const &recv_segments = GetRecvSegments(dowhich);
        auto const &recv_index = GetRecvIndex(dowhich);
        auto const &send_segments = GetSendSegments(dowhich);
        auto const &send_index = GetSendIndex(dowhich);

        size_t send_scratch_size = 0;
        for (auto &segment : send_segments) {
            send_scratch_size += segment.length * row_size;
        }

        auto recv_scratch_tup = GetRecvScratch<ValueType>(row_size, recv_segments);
        auto const recv_scratch_size = recv_scratch_tup.first;
        auto recv_scratch = std::move(recv_scratch_tup.second);

        auto send_scratch =
            EE_CHECK(std::unique_ptr<ValueType[]>(new ValueType[send_scratch_size]),
                     "Could not allocate send_scratch array of size " << send_scratch_size);

        {
            auto current = 0;
            for (auto &segment : send_segments) {
                for (size_t i = segment.begin; i < segment.begin + segment.length; i++) {
                    for (size_t j = 0; j < row_size; j++) {
                        send_scratch[current++] = input(send_index[i], j);
                    }
                }
            }
        }

        vector<UniqueRequest> recv_requests;

        auto recv_batch_begin = recv_segments.begin();

        auto recv_batch_end = GetScratchArrayDimensions(
            recv_scratch_size, row_size, recv_segments.begin(), recv_segments.end());

        auto queue_receive_requests = [&]() {
            for (auto segment = recv_batch_begin; segment != recv_batch_end; segment++) {
                recv_requests.push_back(comm_.immediate_recv(
                    &recv_scratch[(segment->begin - recv_batch_begin->begin) * row_size],
                    segment->length * row_size,
                    segment->rank,
                    TOKEN_GS_TAG));
            }
        };

        EE_CHECK(queue_receive_requests(), "Failed to issue new receive requests");

        vector<UniqueRequest> send_requests;
        EE_CHECK(send_requests.reserve(send_segments.size()),
                 "Could not reserve space in 'send_requests' of size " << send_segments.size());

        // Send requests to higher ranks first, then to lower ranks in order to distribute traffic
        // better.
        for (auto &segment : send_segments) {
            assert(segment.rank != comm_.rank());
            if (segment.rank > comm_.rank()) {
                send_requests.push_back(
                    comm_.immediate_send(&send_scratch[segment.begin * row_size],
                                         segment.length * row_size,
                                         segment.rank,
                                         TOKEN_GS_TAG));
            }
        }

        for (auto &segment : send_segments) {
            assert(segment.rank != comm_.rank());
            if (segment.rank < comm_.rank()) {
                send_requests.push_back(
                    comm_.immediate_send(&send_scratch[segment.begin * row_size],
                                         segment.length * row_size,
                                         segment.rank,
                                         TOKEN_GS_TAG));
            }
        }

        assert(send_segments.size() == send_requests.size());

        switch (dowhat) {
        case TokenOperation::Copy:
            if (dowhich == DoWhich::Gather) {
                for (auto const &idx : zero_) {
                    for (size_t j = 0; j < row_size; j++) {
                        output(idx, j) = 0;
                    }
                }
            }

            for (size_t i = 0; i < copy_from.size(); i++) {
                for (size_t j = 0; j < row_size; j++) {
                    output(copy_to[i], j) = input(copy_from[i], j);
                }
            }
            break;
        case TokenOperation::Add:
            for (size_t i = 0; i < copy_from.size(); i++) {
                for (size_t j = 0; j < row_size; j++) {
                    output(copy_to[i], j) += input(copy_from[i], j);
                }
            }
            break;
        case TokenOperation::Sub:
            for (size_t i = 0; i < copy_from.size(); i++) {
                for (size_t j = 0; j < row_size; j++) {
                    output(copy_to[i], j) -= input(copy_from[i], j);
                }
            }
            break;
        case TokenOperation::Max:
            for (size_t i = 0; i < copy_from.size(); i++) {
                for (size_t j = 0; j < row_size; j++) {
                    output(copy_to[i], j) = std::max(
                        output(copy_to[i], j), static_cast<OutputType>(input(copy_from[i], j)));
                }
            }
            break;
        case TokenOperation::Min:
            for (size_t i = 0; i < copy_from.size(); i++) {
                for (size_t j = 0; j < row_size; j++) {
                    output(copy_to[i], j) = std::min(
                        output(copy_to[i], j), static_cast<OutputType>(input(copy_from[i], j)));
                }
            }
            break;
        }

        vector<int> completed;

        while (recv_batch_begin != recv_segments.end()) {
            // Acts as a type of iterator for completed receive requests. Masks over the difference
            // between requiring rank-ordered completion vs. allowing any-order completion.
            RecvRequestCompletionStateMachine request_completion(recv_requests,
                                                                 require_rank_order_completion_);

            while (request_completion.get_next_requests(completed)) {
                for (auto idx : completed) {
                    auto const &segment = recv_batch_begin[idx];

                    auto recv_scratch_begin =
                        &recv_scratch[(segment.begin - recv_batch_begin->begin) * row_size];

                    switch (dowhat) {
                    case TokenOperation::Copy:
                        for (size_t i = 0; i < segment.length; i++) {
                            auto const scratch = &recv_scratch_begin[i * row_size];

                            for (size_t j = 0; j < row_size; j++) {
                                output(recv_index[i + segment.begin], j) = scratch[j];
                            }
                        }
                        break;
                    case TokenOperation::Add:
                        for (size_t i = 0; i < segment.length; i++) {
                            auto const scratch = &recv_scratch_begin[i * row_size];

                            for (size_t j = 0; j < row_size; j++) {
                                output(recv_index[i + segment.begin], j) += scratch[j];
                            }
                        }
                        break;
                    case TokenOperation::Sub:
                        for (size_t i = 0; i < segment.length; i++) {
                            auto const scratch = &recv_scratch_begin[i * row_size];

                            for (size_t j = 0; j < row_size; j++) {
                                output(recv_index[i + segment.begin], j) -= scratch[j];
                            }
                        }
                        break;
                    case TokenOperation::Max:
                        for (size_t i = 0; i < segment.length; i++) {
                            auto const scratch = &recv_scratch_begin[i * row_size];

                            for (size_t j = 0; j < row_size; j++) {
                                output(recv_index[i + segment.begin], j) =
                                    std::max(output(recv_index[i + segment.begin], j),
                                             static_cast<OutputType>(scratch[j]));
                            }
                        }
                        break;
                    case TokenOperation::Min:
                        for (size_t i = 0; i < segment.length; i++) {
                            auto const scratch = &recv_scratch_begin[i * row_size];

                            for (size_t j = 0; j < row_size; j++) {
                                output(recv_index[i + segment.begin], j) =
                                    std::min(output(recv_index[i + segment.begin], j),
                                             static_cast<OutputType>(scratch[j]));
                            }
                        }
                        break;
                    }
                }
            }

            recv_batch_begin = recv_batch_end;
            recv_requests.clear();

            recv_batch_end = GetScratchArrayDimensions(
                recv_scratch_size, row_size, recv_batch_begin, recv_segments.end());

            EE_CHECK(queue_receive_requests(), "Failed to issue new receive requests");
        }

        mpi::wait_all(send_requests);

        EE_DIAG_POST_MSG("dowhich = " << (int)dowhich << ", dowhat = " << (int)dowhat)
    }
}; // namespace comm

/**
 * @brief
 *  TokenBuilder is used to construct Tokens. Using a TokenBuilder allows you to re-use some inputs
 *  to Token construction multiple times, allowing you to more efficiently instantiate similar
 *  Tokens multiple times.
 *
 */
class TokenBuilder {
  public:
    /**
     * @brief Creates a TokenBuilder as a copy of another
     *
     * @param other
     *  Original TokenBuilder
     */
    TokenBuilder(TokenBuilder const &other) = default;

    /**
     * @brief Changes this TokenBuilder to match other
     *
     * @param other
     *  TokenBuilder to copy from
     * @return TokenBuilder&
     *  This TokenBuilder
     */
    TokenBuilder &operator=(TokenBuilder const &other) = default;

    /**
     * @brief Moves other TokenBuilder to a new location
     *
     * @param other
     *  TokenBuilder to move from
     */
    TokenBuilder(TokenBuilder &&other) = default;

    /**
     * @brief Moves other TokenBuilder to a new location
     *
     * @param other
     *  TokenBuilder to move from
     * @return TokenBuilder&
     *  This TokenBuilder
     */
    TokenBuilder &operator=(TokenBuilder &&other) = default;

    /**
     * @brief Not collective. Instantiates a TokenBuilder using the supplied Comm object.
     * TokenBuilder does not take ownership of the communicator, so the caller is responsible for
     * keeping the comm object alive for the life of the Token.
     *
     * @param comm
     *  The comm object to use. Typically MPI_COMM_WORLD.
     * @return TokenBuilder
     *  A new TokenBuilder using Comm.
     */
    static TokenBuilder FromComm(mpi::Comm comm);

    /**
     * @brief Required (or SetCellBases). Collective operation. Sets the number of cells on the
     *  local rank and exchanges this information with other ranks in the comm object.
     *
     * @param num_local_cell
     *  The number of cells on the local rank in the global address space.
     */
    void SetNumCells(uint32_t num_local_cell);

    /**
     * @brief Required (or SetNumCells). Collective operation. Set the base address for each rank.
     *
     * @param bases
     *  Base address for each rank. Must be the same array on every rank. Must be the same size as
     *  `comm_.size()`.
     */
    void SetCellBases(std::span<uint64_t const> bases);

    /**
     * @brief Not collective. Set the RmaAllToAll communication pattern instantiation to use for
     * AllToAll in Token construction. The caller is responsible for ensuring that RmaAllToAll
     * lives long enough - it is invalid to use the TokenBuilder after the RmaAllToAll instance is
     * destroyed.
     *
     * If the RmaAllToAll pattern is used, it must be used on all ranks.
     *
     * @param rma
     *  The RMA AllToAll object to use.
     */
    void UseRmaAllToAll(RmaAllToAll<std::int32_t> *rma);

    /**
     * @brief Not collective. Disables the use of the RmaAllToAll communication pattern.
     *
     * If the RmaAllToAll pattern is disable, it must be disabled on all ranks.
     */
    void DisableRmaAllToAll() { rma_ = nullptr; }

    /**
     * @brief Not collective.
     *
     * When true, requires that receive requests be consumed in rank-order. E.g. receives from rank
     * 0 will be be consumed prior to receives from rank 1.
     *
     * When false, allows receive requests to be consumed in any-order. This allows for more overlap
     * of computation and communication. This can result in run-to-run numeric differences when
     * Token receives multiple remote elements from different ranks into the same local element for
     * Copy for all data types, and for Add and Sub for datatypes where addition and subtraction are
     * not commutative (e.g. floating point numbers).
     *
     * Defaults to false, allowing any order.
     *
     * @param require_rank_order_completion
     *  True requires rank-order receive completion, false allows any-order receive completion
     */
    void RequireRankOrderRequestCompletion(bool require_rank_order_completion);

    /**
     * @brief Optional. Collective operation. If the local rank's target neighbors are known up
     *  front, this will allow a more efficient build command, changing an MPI_Alltoall into a more
     *  hand-tuned message exchange that limits the number of communications between ranks.
     *
     *  This optimization is only worth taking if the TokenBuilder is called multiple times.
     *
     *  Implicitly calls ClearToAndFromPes on any existing neighbor information.
     *
     * @param to_pes
     *  A list of size MPI_Comm_size of boolean values indicating if values should be sent to the
     *  rank.
     */
    void SetToPes(std::vector<int> &&to_pes);

    /**
     * @brief Optional. Not collective. If the local rank's target neighbors are known up front,
     *  this will allow a more efficient build command, changing an MPI_Alltoall into a more
     *  hand-tuned message exchange that limits the number of communications between ranks.
     *
     *  This optimization is only worth taking if the TokenBuilder is called multiple times, or both
     *  the to_pes and from_pes can be calculated locally.
     *
     *  Implicitly calls ClearToAndFromPes on any existing neighbor information.
     *
     * @param to_pes
     *  A list of size MPI_Comm_size of boolean values indicating if values should be sent to the
     *  rank.
     * @param from_pes
     *  A list of size MPI_Comm_size of boolean values indicating if values should be received from
     *  the rank.
     */
    void SetToAndFromPes(std::vector<int> &&to_pes, std::vector<int> &&from_pes);

    /**
     * @brief Not collective. Clears any neighbor data set by SetToPes or SetToAndFromPes.
     *
     */
    void ClearToAndFromPes() {
        to_pes_.clear();
        from_pes_.clear();
    }

    /**
     * @brief Not collective. Sets the max size (in bytes) to use for a receive buffer. Defaults to
     *  having no maximum size.
     *
     *  Max size will be ignored if any one target requires requires more than max_gs_receive_size
     *  bytes.
     *
     * @param max_gs_receive_size
     *  Max buffer size in bytes.
     */
    void SetMaxGsReceiveSize(size_t max_gs_receive_size) {
        has_target_max_gs_receive_size_ = true;
        target_max_gs_receive_size_ = max_gs_receive_size;
    }

    /**
     * @brief Not collective. Clears the max size option.
     *
     */
    void ClearMaxGsReceiveSize() { has_target_max_gs_receive_size_ = false; }

    /**
     * @brief Not collective. Does not modify the TokenBuilder object. Uses the value from
     *  SetNumCells to allow the consumer to convert global addresses to local address
     *  information.
     *
     *  The size of away_globals, pes, and addresses should be identical.
     *
     * @param away_globals
     *  List of remote addresses to convert.
     * @param pes
     *  (Out) An array of remote ranks, where pes[i] is the rank for away_globals[i].
     * @param addresses
     *  (Out) An array of local addresses, where addresses[i] is the local address for
     *  away_globals[i].
     */
    void PesAndAddresses(std::span<OptionalFortranGlobalIndex const> away_globals,
                         std::span<utility::NonNegativeInteger<mpi::rank_t>> pes,
                         std::span<OptionalFortranLocalIndex> addresses) const;

    /**
     * @brief Not collective. Does not modify the TokenBuilder object. Uses the value from
     *  SetNumCells to allow the consumer to convert global addresses to local address
     *  information.
     *
     * @param away_globals
     *  List of global addresses to convert.
     * @param pes
     *  (Out) An array of remote ranks, where pes[i] is the rank for away_globals[i]. pes will be
     *  resized to away_globals_length.
     * @param addresses
     *  (Out) An array of local addresses, where addresses[i] is the local address for
     *  away_globals[i]. addresses will be resized to away_globals_length.
     */
    void PesAndAddresses(std::span<OptionalFortranGlobalIndex const> away_globals,
                         std::vector<utility::NonNegativeInteger<mpi::rank_t>> &pes,
                         std::vector<OptionalFortranLocalIndex> &addresses) const;

    /**
     * @brief Not collective. Does not modify the TokenBuilder object. Returns an array of flags of
     *  size MPI_Comm_size that marks the ranks that are being targeted in away_globals.
     *
     * @param away_globals
     *  List of global addresses to flag the ranks of.
     * @param pe_flags
     *  (Out) An array of flags, where, if an address is on rank r, pe_flags[r] == 1. If no address
     *  is on rank r, pe_flags[r] == 0.
     */
    void FlagPes(std::span<OptionalFortranGlobalIndex const> away_globals,
                 std::span<int> pe_flags) const;

    /**
     * @brief Not collective. Does not modify the TokenBuilder object. Returns an array of flags of
     *  size MPI_Comm_size that marks the ranks that are being targeted in away_globals.
     *
     * @param away_globals
     *  List of global addresses to flag the ranks of.
     * @param pe_flags
     *  (Out) An array of flags, where, if an address is on rank r, pe_flags[r] == 1. If no address
     *  is on rank r, pe_flags[r] == 0.
     */
    void FlagPes(std::vector<OptionalFortranGlobalIndex> const &away_globals,
                 std::vector<int> &pe_flags) const {
        FlagPes(std::span<OptionalFortranGlobalIndex const>(away_globals),
                std::span<int>(pe_flags));
    }

    /**
     * @brief Collective. Builds a new token that maps global addresses in away_global to a local
     *  buffer using the index data in home_addresses.
     *
     *  The new Token has a lifetime independent of this TokenBuilder, meaning the TokenBuilder
     *  can be destroyed after creating all of the needed Tokens. The new Token will be
     *  invalid if the MPI_Comm set in TokenBuilder::FromComm is freed.
     *
     *  home_addresses and away_global must be the same size.
     *
     * @param home_addresses
     *  List of indices that map the remote addresses to a the receive buffer in a Token::Get* call.
     * @param away_global
     *  List of global addresses that the Token requires.
     * @return Token
     *  A new Token using the properties in the TokenBuilder.
     */
    Token BuildGlobal(std::vector<FortranLocalIndex> const &home_addresses,
                      std::vector<OptionalFortranGlobalIndex> const &away_global) {
        return BuildGlobal(std::span<FortranLocalIndex const>(home_addresses),
                           std::span<OptionalFortranGlobalIndex const>(away_global));
    }

    /**
     * @brief Collective. Builds a new token that maps global addresses in away_global to a local
     *  buffer using the index data in home_addresses.
     *
     *  The new Token has a lifetime independent of this TokenBuilder, meaning the TokenBuilder
     *  can be destroyed after creating all of the needed Tokens. The new Token will be
     *  invalid if the MPI_Comm set in TokenBuilder::FromComm is freed.
     *
     *  home_addresses and away_global must be the same size.
     *
     * @param home_addresses
     *  List of indices that map the remote addresses to a the receive buffer in a Token::Get* call.
     * @param away_global
     *  List of global addresses that the Token requires.
     * @return Token
     *  A new Token using the properties in the TokenBuilder.
     */
    Token BuildGlobal(std::span<FortranLocalIndex const> home_addresses,
                      std::span<OptionalFortranGlobalIndex const> away_global);

    /**
     * @brief Collective. Builds a new token that maps remote-local addresses in away_pe and
     *  away_addresses to a local buffer using the index data in home_addresses.
     *
     *  The new Token has a lifetime independent of this TokenBuilder, meaning the TokenBuilder
     *  can be destroyed after creating all of the needed Tokens. The new Token will be
     *  invalid if the MPI_Comm set in TokenBuilder::FromComm is freed.
     *
     *  home_addresses, away_pe, and away_address must be the same size.
     *
     * @param home_addresses
     *  List of indices that map the remote addresses to a the receive buffer in a Token::Get* call.
     * @param away_pe
     *  List of remote ranks that the Token requires. away_pe[i] is the rank where away_address[i]
     *  is located.
     * @param away_address
     *  List of remote-local addresses that the Token requires. See away_pe.
     * @return Token
     *  A new Token using the properties in the TokenBuilder.
     */
    Token BuildLocal(std::span<FortranLocalIndex const> home_addresses,
                     std::span<utility::NonNegativeInteger<mpi::rank_t> const> away_pe,
                     std::span<OptionalFortranLocalIndex const> away_address);

  private:
    mpi::Comm comm_;
    // Just used to avoid allocating each time SetNumCells is called
    std::vector<uint32_t> num_cells_;
    // Used to build tokens
    std::vector<uint64_t> bases_;

    // optional descriptors of which PEs to communicate count data to
    std::vector<int> to_pes_;
    std::vector<int> from_pes_;

    // Option for using max_gs_receive_size
    bool has_target_max_gs_receive_size_ = false;
    std::uint32_t target_max_gs_receive_size_ = 0;

    // Option for requiring rank_order_completion
    bool require_rank_order_completion_ = false;

    RmaAllToAll<std::int32_t> *rma_ = nullptr;

    TokenBuilder(mpi::Comm comm) : comm_(comm) {}
};

#define TOKEN_INSTANTIATE_TOKEN_GS_UNIT(type)                                                      \
    template void Token::GatherScatter<                                                            \
        Kokkos::View<type const * [1], Kokkos::LayoutRight, eap::HostMemorySpace>,                 \
        Kokkos::View<type * [1], Kokkos::LayoutRight, eap::HostMemorySpace>>(                      \
        DoWhich,                                                                                   \
        TokenOperation,                                                                            \
        Kokkos::View<type const * [1], Kokkos::LayoutRight, eap::HostMemorySpace> const &,         \
        Kokkos::View<type * [1], Kokkos::LayoutRight, eap::HostMemorySpace> &)

extern TOKEN_INSTANTIATE_TOKEN_GS_UNIT(double);
extern TOKEN_INSTANTIATE_TOKEN_GS_UNIT(float);

#define TOKEN_INSTANTIATE_TOKEN_GS_V(type, layout)                                                 \
    template void Token::GatherScatter<Kokkos::View<type const **, layout, eap::HostMemorySpace>,  \
                                       Kokkos::View<type **, layout, eap::HostMemorySpace>>(       \
        DoWhich,                                                                                   \
        TokenOperation,                                                                            \
        Kokkos::View<type const **, layout, eap::HostMemorySpace> const &,                         \
        Kokkos::View<type **, layout, eap::HostMemorySpace> &)

extern TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutLeft);
extern TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutRight);
extern TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutStride);

extern TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutLeft);
extern TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutRight);
extern TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutStride);
} // namespace comm
} // namespace eap

#endif // COMM_TOKEN_HPP