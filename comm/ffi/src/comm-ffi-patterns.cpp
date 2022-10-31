/**
 * @file comm-ffi-patterns.cpp
 *
 * @brief Implementation of FFI to eap::comm patterns
 * @date 2018-06-18
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Includes
#include <comm-ffi-internal-patterns.hpp>
#include <comm-ffi-patterns.h>

// STL Includes
#include <type_traits>

// Internal Includes
#include <error-macros.hpp>
#include <utility-ffi-addressing.h>
#include <utility-ffi-interop-addressing.hpp>
#include <utility-kokkos.hpp>
#include <utility-linear_view.hpp>

// Local Includes
#include <comm-ffi-concretize.hpp>
#include <comm-ffi-interop.hpp>

using namespace eap::comm::ffi;

using eap::FortranLocalIndex;
using eap::local_index_t;
using eap::comm::ffi::RmaAllToAllGeneric;
using eap::utility::ffi::FortranLocalAddressSpan;
using eap::utility::ffi::LocalAddressSpan;
using nonstd::span;

template <typename Dt>
using right_unmanged_t =
    Kokkos::View<Dt, Kokkos::LayoutRight, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>;

template <typename Dt>
using right_t = Kokkos::View<Dt, Kokkos::LayoutRight, eap::HostMemorySpace>;

template <typename Dt>
using a2a_view = Kokkos::View<Dt *, Kokkos::LayoutStride, eap::HostMemorySpace>;

template <typename DataType>
using StrideUnmanagedView =
    Kokkos::View<DataType, Kokkos::LayoutStride, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>;

RmaAllToAllGeneric::RmaAllToAllGeneric(MPI_Comm comm, eap_comm_datatype_t datatype, int count)
    : datatype_(datatype) {

    this->comm = mpi::Comm::from_handle(comm);

    switch (this->datatype()) {
    case eap_comm_datatype_int32:
        rma_all_to_all = new eap::comm::RmaAllToAll<int>(this->comm, count);
        break;
    default:
        std::cerr << this->comm.rank() << ": eap_comm_datatype_t " << this->datatype()
                  << " is not supported by comm_rma_all_to_all." << std::endl;
        this->comm.abort(EXIT_FAILURE);
    }
}

void RmaAllToAllGeneric::invoke(abi_ndarray_t const *input, abi_ndarray_t *output) {
    use_concrete([input, output](auto *rma) {
        using eap::utility::kokkos::IsLayoutRight;
        using eap::utility::kokkos::ViewFromNdarray;

        using vt = typename std::decay_t<decltype(*rma)>::value_type;

        a2a_view<vt const> input_view = ViewFromNdarray<vt const *>(*input);
        a2a_view<vt> output_view = ViewFromNdarray<vt *>(*output);

        Kokkos::LayoutRight input_layout_right, output_layout_right;
        if (IsLayoutRight(input_view, input_layout_right) &&
            IsLayoutRight(output_view, output_layout_right)) {

            right_t<vt const *> input_right =
                right_unmanged_t<vt const *>(input_view.data(), input_layout_right);
            right_t<vt *> output_right =
                right_unmanged_t<vt *>(output_view.data(), output_layout_right);

            rma->AllToAll(input_right, output_right);
        } else {
            rma->AllToAll(input_view, output_view);
        }
    });
}

RmaAllToAllGeneric::~RmaAllToAllGeneric() {
    use_concrete([](auto *rma) { delete rma; });

    rma_all_to_all = nullptr;
}

EXTERN_C void comm_rma_all_to_all_create(MPI_Comm comm,
                                         eap_comm_datatype_t datatype,
                                         int count,
                                         comm_rma_all_to_all_t **rma_all_to_all) {
    EAP_EXTERN_PRE

    *rma_all_to_all =
        reinterpret_cast<comm_rma_all_to_all_t *>(new RmaAllToAllGeneric(comm, datatype, count));

    EAP_EXTERN_POST
}

EXTERN_C void comm_rma_all_to_all_create_f(MPI_Fint comm,
                                           eap_comm_datatype_t datatype,
                                           int count,
                                           comm_rma_all_to_all_t **rma_all_to_all) {
    comm_rma_all_to_all_create(MPI_Comm_f2c(comm), datatype, count, rma_all_to_all);
}

EXTERN_C void comm_rma_all_to_all_free(comm_rma_all_to_all_t *rma_all_to_all) {
    EAP_EXTERN_PRE

    delete RmaAllToAllFromHandle(rma_all_to_all);

    EAP_EXTERN_POST
}

EXTERN_C void comm_rma_all_to_all_invoke(comm_rma_all_to_all_t *rma_all_to_all,
                                         abi_ndarray_t const *send,
                                         abi_ndarray_t *recv) {
    EAP_EXTERN_PRE

    RmaAllToAllFromHandle(rma_all_to_all)->invoke(send, recv);

    EAP_EXTERN_POST
}

namespace {
template <typename T>
void eap_comm_move_t(mpi::Comm &comm,
                     span<FortranLocalIndex const> send_start,
                     span<local_index_t const> send_length,
                     StrideUnmanagedView<T const *> send_data,
                     span<FortranLocalIndex const> recv_start,
                     span<local_index_t const> recv_length,
                     StrideUnmanagedView<T *> recv_data) {
    using eap::utility::MakeLinearView;

    // Move requires the data to be laid out in an unit-stride array.
    // `MakeLinearView` automatically shadows the send_data and recv_data with a
    // unit-stride array, ommitting the re-allocation if it's already
    // unit-stride.
    auto send_data_linear = MakeLinearView(send_data);
    auto recv_data_linear = MakeLinearView(recv_data);

    eap::comm::Move(comm,
                    send_start,
                    send_length,
                    send_data_linear.Span(),
                    recv_start,
                    recv_length,
                    recv_data_linear.Span());
}

void eap_comm_move_portable(MPI_Comm comm,
                            eap_comm_datatype_t datatype,
                            eap_local_t const *send_start,
                            eap_local_t const *send_length,
                            abi_ndarray_t const *send_data,
                            eap_local_t const *recv_start,
                            eap_local_t const *recv_length,
                            abi_ndarray_t const *recv_data) {
    using eap::local_index_t;
    using eap::comm::ffi::ConcretizeViews;
    using eap::comm::ffi::ToKokkosView;

    auto mpi_comm = mpi::Comm::from_handle(comm);

    auto const send_start_span = FortranLocalAddressSpan(send_start, mpi_comm.size());
    auto const send_length_span = LocalAddressSpan(send_length, mpi_comm.size());
    auto const recv_start_span = FortranLocalAddressSpan(recv_start, mpi_comm.size());
    auto const recv_length_span = LocalAddressSpan(recv_length, mpi_comm.size());

    // Calls the lambda function with Kokkos:Views of an explicit type, supporting all types in
    // eap_comm_datatype_t
    ConcretizeViews(
        datatype,
        [&](auto send_data_view, auto recv_data_view) {
            eap_comm_move_t(mpi_comm,
                            send_start_span,
                            send_length_span,
                            send_data_view,
                            recv_start_span,
                            recv_length_span,
                            recv_data_view);
        },
        // Indicates that send_data must be a 1D array, and the data is immutable
        ToKokkosView<void const *>(*send_data),
        // Indicates that recv_data must be a 1D array, and the data is mutable
        ToKokkosView<void *>(*recv_data));
}
} // namespace

EXTERN_C void eap_comm_move_f(MPI_Fint comm,
                              eap_comm_datatype_t datatype,
                              eap_local_t const *send_start,
                              eap_local_t const *send_length,
                              abi_ndarray_t const *send_data,
                              eap_local_t const *recv_start,
                              eap_local_t const *recv_length,
                              abi_ndarray_t const *recv_data) {
    EAP_EXTERN_PRE

    eap_comm_move_portable(MPI_Comm_f2c(comm),
                           datatype,
                           send_start,
                           send_length,
                           send_data,
                           recv_start,
                           recv_length,
                           recv_data);

    EAP_EXTERN_POST
}