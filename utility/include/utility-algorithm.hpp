/**
 * @file utility-algorithm.hpp
 *
 * @brief Implementation of common algorithms
 * @date 2019-05-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 *
 */

#ifndef EAP_UTILITY_ALGORITHM_HPP_
#define EAP_UTILITY_ALGORITHM_HPP_

// STL Includes
#include <cstdint>
#include <type_traits>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <mpi/comm.hpp>

// Internal Includes
#include <error-macros.hpp>

// Local Includes
#include <utility-memory.hpp>

namespace eap {
namespace utility {
template <typename InputView,
          typename OutputView,
          typename ExecutionSpace = typename InputView::execution_space>
std::enable_if_t<Kokkos::is_view<InputView>::value && Kokkos::is_view<OutputView>::value>
ExclusiveSumScan(InputView num_cells, OutputView bases) {
    using namespace Kokkos;

    using OutputType = typename OutputView::non_const_value_type;

    EE_DIAG_PRE

    EE_ASSERT_EQ(num_cells.extent(0), bases.extent(0), "bases and num_cells must be the same size");

    parallel_scan(
        "eap::utility::ExclusiveSumScan::bases",
        // Must run on host - using host memory
        RangePolicy<ExecutionSpace>(0, num_cells.extent(0)),
        KOKKOS_LAMBDA(int pe, OutputType &update, bool is_final) {
            if (is_final) {
                bases(pe) = update;
            }

            // accumulates all preceding numcells into the base value
            update += num_cells(pe);
        });

    EE_DIAG_POST
}

template <typename InputView,
          typename OutputView,
          typename ExecutionSpace = typename InputView::execution_space>
std::enable_if_t<Kokkos::is_view<InputView>::value && Kokkos::is_view<OutputView>::value>
InclusiveSumScan(InputView num_cells, OutputView bases) {
    using namespace Kokkos;

    using OutputType = typename OutputView::non_const_value_type;

    EE_DIAG_PRE

    EE_ASSERT_EQ(num_cells.extent(0), bases.extent(0), "bases and num_cells must be the same size");

    parallel_scan(
        "eap::utility::InclusiveSumScan::bases",
        // Must run on host - using host memory
        RangePolicy<ExecutionSpace>(0, num_cells.extent(0)),
        KOKKOS_LAMBDA(int pe, OutputType &update, bool is_final) {
            // accumulates all cells up-to-and-including the current value into the base
            // value
            update += num_cells(pe);

            if (is_final) {
                bases(pe) = update;
            }
        });

    EE_DIAG_POST
}

template <typename InputView,
          typename OutputView,
          typename ExecutionSpace = typename InputView::execution_space>
std::enable_if_t<Kokkos::is_view<InputView>::value && Kokkos::is_view<OutputView>::value>
InclusiveExclusiveSumScan(InputView num_cells, OutputView bases) {
    using namespace Kokkos;

    EE_DIAG_PRE

    EE_ASSERT_EQ(num_cells.extent(0) + 1, bases.extent(0), "bases must be 1 larger than num_cells");

    // Set the first element to zero, then use InclusiveScan
    deep_copy(subview(bases, 0), 0);

    InclusiveSumScan<InputView, OutputView, ExecutionSpace>(
        num_cells, subview(bases, std::make_pair((size_t)1, bases.extent(0))));

    EE_DIAG_POST
}

void InclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output);

void InclusiveExclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output);

void ExclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output);

template <typename Scalar>
struct LocalGlobalMaxResult {
    Scalar local;
    Scalar global;
};

template <typename View, typename ExecutionSpace = typename View::execution_space>
auto LocalGlobalMax(View input, mpi::Comm comm = mpi::Comm::world())
    -> LocalGlobalMaxResult<typename View::non_const_value_type> {
    static_assert(View::traits::rank == 1, "GlobalMax is only implemented for 1D arrays");

    using value_type = typename View::non_const_value_type;

    value_type max;
    Kokkos::parallel_reduce(
        "eap::utility::GlobalMax",
        Kokkos::RangePolicy<ExecutionSpace>(0, input.extent(0)),
        KOKKOS_LAMBDA(size_t const i, value_type &max_value) {
            max_value = max_value > input(i) ? max_value : input(i);
        },
        Kokkos::Max<value_type>(max));

    return LocalGlobalMaxResult<value_type>{max, comm.all_reduce(mpi::max(), max)};
}
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_ALGORITHM_HPP_