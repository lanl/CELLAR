/**
 * @file comm-ffi-token.h
 *
 * @brief Defines ABI for Token
 * @date 2019-01-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef COMM_FFI_COMM_H
#define COMM_FFI_COMM_H

// STL Includes
#include <stdbool.h>
#include <stdint.h>

// Third Party Includes
#include <mpi_stub_out.h>

// Internal Includes
#include <abi-extern.h>
#include <abi-ndarray.h>
#include <perf-ffi-handles.h>
#include <utility-ffi-addressing.h>

// Local Includes
#include <comm-ffi-patterns.h>
#include <comm-ffi-types.h>

EXTERN_C_BEGIN

void comm_token_builder_create(MPI_Comm comm, comm_token_builder_t **builder);

void comm_token_builder_free(comm_token_builder_t *builder);

void comm_token_builder_clone(comm_token_builder_t const *original_builder,
                              comm_token_builder_t **new_builder);

void comm_token_builder_set_num_cells(comm_token_builder_t *builder, eap_local_t num_local_cell);

void comm_token_builder_use_rma_all_to_all(comm_token_builder_t *builder,
                                           comm_rma_all_to_all_t *rma_all_to_all);

void comm_token_builder_disable_rma_all_to_all(comm_token_builder_t *builder);

void comm_token_builder_require_rank_order_request_completion(comm_token_builder_t *builder,
                                                              bool require_rank_order_completion);

void comm_token_builder_set_to_pes(comm_token_builder_t *builder,
                                   int const *to_pes,
                                   size_t to_pes_length);

void comm_token_builder_set_to_and_from_pes(comm_token_builder_t *builder,
                                            int const *to_pes,
                                            size_t to_pes_length,
                                            int const *from_pes,
                                            size_t from_pes_length);

void comm_token_builder_clear_to_and_from_pes(comm_token_builder_t *builder);

void comm_token_builder_set_max_gs_receive_size(comm_token_builder_t *builder,
                                                size_t max_gs_receive_size);

void comm_token_builder_clear_max_gs_receive_size(comm_token_builder_t *builder);

void comm_token_builder_pes_and_addresses_f(comm_token_builder_t const *builder,
                                            eap_global_t const *away_globals,
                                            size_t away_globals_length,
                                            int *pes,
                                            size_t pes_length,
                                            eap_local_t *addresses,
                                            size_t addresses_length);

void comm_token_builder_flag_pes_f(comm_token_builder_t const *builder,
                                   eap_global_t const *away_globals,
                                   size_t away_globals_length,
                                   int *flag_pes,
                                   size_t pes_length);

void comm_token_builder_build_global_f(comm_token_builder_t *builder,
                                       eap_local_t const *home_addresses,
                                       size_t home_addresses_length,
                                       eap_global_t const *away_global,
                                       size_t away_global_length,
                                       comm_token_t **token);

void comm_token_builder_build_local_f(comm_token_builder_t *builder,
                                      eap_local_t const *home_addresses,
                                      size_t home_addresses_length,
                                      int const *away_pe,
                                      size_t away_pe_length,
                                      eap_local_t const *away_address,
                                      size_t away_address_length,
                                      comm_token_t **token);

void comm_token_clone(comm_token_t *token, comm_token_t **new_token);

void comm_token_free(comm_token_t *token);

void comm_token_get_home_num(comm_token_t const *token, size_t *home_num);
void comm_token_get_home_size(comm_token_t const *token, size_t *home_size);

void comm_token_fill_home_arrays_f(comm_token_t const *token,
                                   size_t home_num,
                                   int *ranks,
                                   eap_local_t *los,
                                   eap_local_t *lengths,
                                   size_t home_size,
                                   eap_local_t *indices);

void comm_token_get(comm_token_t *token,
                    comm_token_operation_t dowhat,
                    abi_ndarray_t const *input,
                    abi_ndarray_t *output,
                    eap_comm_datatype_t datatype);

void comm_token_put(comm_token_t *token,
                    comm_token_operation_t dowhat,
                    abi_ndarray_t const *input,
                    abi_ndarray_t *output,
                    eap_comm_datatype_t datatype);

void comm_token_get_inv(comm_token_t *token,
                        comm_token_operation_t dowhat,
                        abi_ndarray_t const *input,
                        abi_ndarray_t *output,
                        eap_comm_datatype_t datatype);

void comm_token_put_inv(comm_token_t *token,
                        comm_token_operation_t dowhat,
                        abi_ndarray_t const *input,
                        abi_ndarray_t *output,
                        eap_comm_datatype_t datatype);

EXTERN_C_END

#endif // COMM_FFI_COMM_H