/**
 * @file comm-ffi-token.cpp
 *
 * @brief Implementation of FFI to eap::comm::Token
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <comm-ffi-token.h>

// STL Includes
#include <iostream>
#include <memory>

// Internal Includes
#include <comm-token.hpp>
#include <utility-ffi-interop-addressing.hpp>
#include <utility-kokkos.hpp>

// Local Includes
#include <comm-ffi-internal-patterns.hpp>
#include <comm-ffi-interop.hpp>

using namespace eap::comm;
using namespace eap::comm::ffi;

using eap::local_index_t;
using eap::utility::FortranIndex;
using eap::utility::ffi::FortranLocalAddressSpan;
using eap::utility::ffi::LocalAddressSpan;
using eap::utility::ffi::NonNegativeIntSpan;
using eap::utility::ffi::OptionalFortranGlobalAddressSpan;
using eap::utility::ffi::OptionalFortranLocalAddressSpan;
using eap::utility::kokkos::IsLayoutLeft;
using eap::utility::kokkos::IsLayoutRight;
using eap::utility::kokkos::Transpose;
using eap::utility::kokkos::ViewFromNdarray;
using nonstd::span;
using std::shared_ptr;

namespace {
TokenOperation ConvertOp(comm_token_operation_t op) {
    switch (op) {
    case comm_token_operation_copy:
        return TokenOperation::Copy;
    case comm_token_operation_add:
        return TokenOperation::Add;
    case comm_token_operation_sub:
        return TokenOperation::Sub;
    case comm_token_operation_max:
        return TokenOperation::Max;
    case comm_token_operation_min:
        return TokenOperation::Min;
    default:
        std::cerr << "Invalid token operation: " << op << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// Calls kernel with the precise type of the Kokkos::View - gets a better GatherScatter
/// implementation when we have LayoutRight.
template <typename DataType, typename Fn>
void ConcretizeView(abi_ndarray_t const &nd_view, Fn kernel) {
    auto view = ViewFromNdarray<DataType>(nd_view);

#ifdef TOKEN_OPTIMIZE_FOR_LAYOUT_RIGHT
    Kokkos::LayoutRight right;
    if (IsLayoutRight(view, right)) {
        return kernel(Kokkos::View<DataType, Kokkos::LayoutRight, eap::HostMemorySpace>(
            Kokkos::
                View<DataType, Kokkos::LayoutRight, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>{
                    view.data(), right}));
    }
#endif

#ifdef TOKEN_OPTIMIZE_FOR_LAYOUT_LEFT
    Kokkos::LayoutLeft left;
    if (IsLayoutLeft(view, left)) {
        return kernel(Kokkos::View<DataType, Kokkos::LayoutLeft, eap::HostMemorySpace>(
            Kokkos::
                View<DataType, Kokkos::LayoutLeft, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>{
                    view.data(), left}));
    }
#endif

    return kernel(view);
}

template <typename DataType, typename Fn>
void ConcretizeViews(abi_ndarray_t const &nd_input, abi_ndarray_t const &nd_output, Fn functor) {
    using traits = Kokkos::ViewTraits<DataType>;

    ConcretizeView<typename traits::const_data_type>(nd_input, [&](auto input) {
        ConcretizeView<typename traits::non_const_data_type>(
            nd_output, [&](auto output) { functor(input, output); });
    });
}
} // namespace

EXTERN_C void comm_token_builder_create(MPI_Comm comm, comm_token_builder_t **builder) {
    EAP_EXTERN_PRE

    // This creates an mpi::Comm that does not close the communicator when dropped.
    auto comm_ref = mpi::Comm::from_handle(comm);

    auto comm_token_builder = new TokenBuilder(TokenBuilder::FromComm(comm_ref));

    *builder = reinterpret_cast<comm_token_builder_t *>(comm_token_builder);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_create_f(MPI_Fint comm, comm_token_builder_t **builder) {
    EAP_EXTERN_PRE

    // This creates an mpi::Comm that does not close the communicator when dropped.
    auto comm_ref = mpi::Comm::from_handle(MPI_Comm_f2c(comm));

    auto comm_token_builder = new TokenBuilder(TokenBuilder::FromComm(comm_ref));

    *builder = reinterpret_cast<comm_token_builder_t *>(comm_token_builder);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_free(comm_token_builder_t *builder) {
    EAP_EXTERN_PRE

    delete TokenBuilderFromFFI(builder);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_clone(comm_token_builder_t const *original_builder,
                                       comm_token_builder_t **new_builder) {
    EAP_EXTERN_PRE

    *new_builder = reinterpret_cast<comm_token_builder_t *>(
        new TokenBuilder(*TokenBuilderFromFFI(original_builder)));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_set_num_cells(comm_token_builder_t *builder,
                                               eap_local_t num_local_cell) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->SetNumCells(num_local_cell);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_use_rma_all_to_all(comm_token_builder_t *builder,
                                                    comm_rma_all_to_all_t *rma_all_to_all) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->UseRmaAllToAll(
        RmaAllToAllFromHandle(rma_all_to_all)->unwrap_int());

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_disable_rma_all_to_all(comm_token_builder_t *builder) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->DisableRmaAllToAll();

    EAP_EXTERN_POST
}

EXTERN_C void
comm_token_builder_require_rank_order_request_completion(comm_token_builder_t *builder,
                                                         bool require_rank_order_completion) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->RequireRankOrderRequestCompletion(require_rank_order_completion);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_set_to_pes(comm_token_builder_t *builder,
                                            int const *to_pes,
                                            size_t to_pes_length) {
    EAP_EXTERN_PRE

    std::vector<int> to_pes_vec;
    std::copy(to_pes, to_pes + to_pes_length, std::back_inserter(to_pes_vec));
    TokenBuilderFromFFI(builder)->SetToPes(std::move(to_pes_vec));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_set_to_and_from_pes(comm_token_builder_t *builder,
                                                     int const *to_pes,
                                                     size_t to_pes_length,
                                                     int const *from_pes,
                                                     size_t from_pes_length) {
    EAP_EXTERN_PRE

    std::vector<int> to_pes_vec, from_pes_vec;
    std::copy(to_pes, to_pes + to_pes_length, std::back_inserter(to_pes_vec));
    std::copy(from_pes, from_pes + from_pes_length, std::back_inserter(from_pes_vec));
    TokenBuilderFromFFI(builder)->SetToAndFromPes(std::move(to_pes_vec), std::move(from_pes_vec));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_clear_to_and_from_pes(comm_token_builder_t *builder) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->ClearToAndFromPes();

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_set_max_gs_receive_size(comm_token_builder_t *builder,
                                                         size_t max_gs_receive_size) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->SetMaxGsReceiveSize(max_gs_receive_size);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_clear_max_gs_receive_size(comm_token_builder_t *builder) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->ClearMaxGsReceiveSize();

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_pes_and_addresses_f(comm_token_builder_t const *builder,
                                                     eap_global_t const *away_globals,
                                                     size_t away_globals_length,
                                                     int *pes,
                                                     size_t pes_length,
                                                     eap_local_t *addresses,
                                                     size_t addresses_length) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->PesAndAddresses(
        OptionalFortranGlobalAddressSpan(away_globals, away_globals_length),
        NonNegativeIntSpan(pes, pes_length),
        OptionalFortranLocalAddressSpan(addresses, addresses_length));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_flag_pes_f(comm_token_builder_t const *builder,
                                            eap_global_t const *away_globals,
                                            size_t away_globals_length,
                                            int *pe_flags,
                                            size_t pes_length) {
    EAP_EXTERN_PRE

    TokenBuilderFromFFI(builder)->FlagPes(
        OptionalFortranGlobalAddressSpan(away_globals, away_globals_length),
        nonstd::span<int>(pe_flags, pes_length));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_build_global_f(comm_token_builder_t *builder,
                                                eap_local_t const *home_addresses,
                                                size_t home_addresses_length,
                                                eap_global_t const *away_global,
                                                size_t away_global_length,
                                                comm_token_t **token) {
    EAP_EXTERN_PRE

    auto comm_token = new shared_ptr<Token>(new Token(TokenBuilderFromFFI(builder)->BuildGlobal(
        FortranLocalAddressSpan(home_addresses, home_addresses_length),
        OptionalFortranGlobalAddressSpan(away_global, away_global_length))));

    *token = TokenToFFI(comm_token);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_builder_build_local_f(comm_token_builder_t *builder,
                                               eap_local_t const *home_addresses,
                                               size_t home_addresses_length,
                                               int const *away_pe,
                                               size_t away_pe_length,
                                               eap_local_t const *away_address,
                                               size_t away_address_length,
                                               comm_token_t **token) {
    EAP_EXTERN_PRE

    auto comm_token = new shared_ptr<Token>(new Token(TokenBuilderFromFFI(builder)->BuildLocal(
        FortranLocalAddressSpan(home_addresses, home_addresses_length),
        NonNegativeIntSpan(away_pe, away_pe_length),
        OptionalFortranLocalAddressSpan(away_address, away_address_length))));

    *token = TokenToFFI(comm_token);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_clone(comm_token_t *token, comm_token_t **new_token) {
    EAP_EXTERN_PRE

    *new_token = TokenToFFI(new shared_ptr<Token>(*TokenFromFFI(token)));

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_free(comm_token_t *token) {
    EAP_EXTERN_PRE

    delete TokenFromFFI(token);

    EAP_EXTERN_POST
}

EXTERN_C void comm_token_get_home_num(comm_token_t const *token, size_t *home_num) {
    EAP_EXTERN_PRE
    *home_num = (*TokenFromFFI(token))->GetHomeNum();
    EAP_EXTERN_POST
}

EXTERN_C void comm_token_get_home_size(comm_token_t const *token, size_t *home_size) {
    EAP_EXTERN_PRE
    *home_size = (*TokenFromFFI(token))->GetHomeSize();
    EAP_EXTERN_POST
}

EXTERN_C void comm_token_fill_home_arrays_f(comm_token_t const *token,
                                            size_t home_num,
                                            int *ranks,
                                            eap_local_t *los,
                                            eap_local_t *lengths,
                                            size_t home_size,
                                            eap_local_t *indices) {
    EAP_EXTERN_PRE

        (*TokenFromFFI(token))
            ->FillHomeArrays(span<int>(ranks, home_num),
                             FortranLocalAddressSpan(los, home_num),
                             LocalAddressSpan(lengths, home_num),
                             FortranLocalAddressSpan(indices, home_size));

    EAP_EXTERN_POST
}

template <typename T>
void comm_token_get_t(Token &token,
                      TokenOperation dowhat,
                      abi_ndarray_t const &input,
                      abi_ndarray_t &output) {
    if (input.rank == 1) {
        ConcretizeViews<T *>(input, output, [&token, dowhat](auto input, auto output) {
            token.Get(dowhat, input, output);
        });
    } else {
        assert(input.rank == 2);
        ConcretizeViews<T **>(input, output, [&token, dowhat](auto input, auto output) {
            token.GetV(dowhat, input, output);
        });
    }
}

EXTERN_C void comm_token_get(comm_token_t *token,
                             comm_token_operation_t dowhat,
                             abi_ndarray_t const *input,
                             abi_ndarray_t *output,
                             eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    EE_ASSERT_NOT_NULL(token);
    EE_ASSERT_NOT_NULL(input);
    EE_ASSERT_NOT_NULL(output);

    EE_ASSERT(input->rank == 1 || input->rank == 2,
              "Only 1D and 2D arrays are supported. `input` had rank " << input->rank);
    EE_ASSERT_EQ(input->rank, output->rank);

    switch (datatype) {
    case eap_comm_datatype_bool:
        comm_token_get_t<char>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int32:
        comm_token_get_t<int32_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int64:
        comm_token_get_t<int64_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_float:
        comm_token_get_t<float>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_double:
        comm_token_get_t<double>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    default:
        EE_RAISE("Invalid token datatype " << datatype);
    }

    EAP_EXTERN_POST
}

template <typename T>
void comm_token_get_inv_t(Token &token,
                          TokenOperation dowhat,
                          abi_ndarray_t const &input,
                          abi_ndarray_t &output) {
    assert(input.rank == 2);
    ConcretizeViews<T **>(input, output, [&token, dowhat](auto input, auto output) {
        token.GetVInv(dowhat, input, output);
    });
}

EXTERN_C void comm_token_get_inv(comm_token_t *token,
                                 comm_token_operation_t dowhat,
                                 abi_ndarray_t const *input,
                                 abi_ndarray_t *output,
                                 eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    EE_ASSERT_NOT_NULL(token);
    EE_ASSERT_NOT_NULL(input);
    EE_ASSERT_NOT_NULL(output);

    EE_ASSERT_EQ(2, input->rank, "Only 2D arrays are supported");
    EE_ASSERT_EQ(2, output->rank, "Only 2D arrays are supported");

    switch (datatype) {
    case eap_comm_datatype_bool:
        comm_token_get_inv_t<char>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int32:
        comm_token_get_inv_t<int32_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int64:
        comm_token_get_inv_t<int64_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_float:
        comm_token_get_inv_t<float>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_double:
        comm_token_get_inv_t<double>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    default:
        EE_RAISE("Invalid token datatype " << datatype);
    }

    EAP_EXTERN_POST
}

template <typename T>
void comm_token_put_t(Token &token,
                      TokenOperation dowhat,
                      abi_ndarray_t const &input,
                      abi_ndarray_t &output) {
    if (input.rank == 1) {
        ConcretizeViews<T *>(input, output, [&token, dowhat](auto input, auto output) {
            token.Put(dowhat, input, output);
        });
    } else {
        assert(input.rank == 2);

        ConcretizeViews<T **>(input, output, [&token, dowhat](auto input, auto output) {
            token.PutV(dowhat, input, output);
        });
    }
}

EXTERN_C void comm_token_put(comm_token_t *token,
                             comm_token_operation_t dowhat,
                             abi_ndarray_t const *input,
                             abi_ndarray_t *output,
                             eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    EE_ASSERT_NOT_NULL(token);
    EE_ASSERT_NOT_NULL(input);
    EE_ASSERT_NOT_NULL(output);

    EE_ASSERT(input->rank == 1 || input->rank == 2,
              "Only 1D and 2D arrays are supported. `input` had rank " << input->rank);
    EE_ASSERT_EQ(input->rank, output->rank);

    switch (datatype) {
    case eap_comm_datatype_bool:
        comm_token_put_t<char>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int32:
        comm_token_put_t<int32_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int64:
        comm_token_put_t<int64_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_float:
        comm_token_put_t<float>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_double:
        comm_token_put_t<double>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    default:
        EE_RAISE("Invalid token datatype " << datatype);
    }

    EAP_EXTERN_POST
}

template <typename T>
void comm_token_put_inv_t(Token &token,
                          TokenOperation dowhat,
                          abi_ndarray_t const &input,
                          abi_ndarray_t &output) {
    assert(input.rank == 2);
    ConcretizeViews<T **>(input, output, [&token, dowhat](auto input, auto output) {
        token.PutVInv(dowhat, input, output);
    });
}

EXTERN_C void comm_token_put_inv(comm_token_t *token,
                                 comm_token_operation_t dowhat,
                                 abi_ndarray_t const *input,
                                 abi_ndarray_t *output,
                                 eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    EE_ASSERT_NOT_NULL(token);
    EE_ASSERT_NOT_NULL(input);
    EE_ASSERT_NOT_NULL(output);

    EE_ASSERT_EQ(2, input->rank, "Only 2D arrays are supported");
    EE_ASSERT_EQ(2, output->rank, "Only 2D arrays are supported");

    switch (datatype) {
    case eap_comm_datatype_bool:
        comm_token_put_inv_t<char>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int32:
        comm_token_put_inv_t<int32_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_int64:
        comm_token_put_inv_t<int64_t>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_float:
        comm_token_put_inv_t<float>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    case eap_comm_datatype_double:
        comm_token_put_inv_t<double>(**TokenFromFFI(token), ConvertOp(dowhat), *input, *output);
        break;
    default:
        EE_RAISE("Invalid token datatype " << datatype);
    }

    EAP_EXTERN_POST
}