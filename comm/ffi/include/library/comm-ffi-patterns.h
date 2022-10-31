/**
 * @file comm-ffi-patterns.h
 *
 * @brief C interface to eap-comm communication patterns
 * @date 2019-01-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef COMM_FFI_PATTERNS_H_
#define COMM_FFI_PATTERNS_H_

// Third Party Includes
#include <mpi_stub_out.h>

// Internal Includes
#include <abi-extern.h>
#include <abi-ndarray.h>
#include <utility-ffi-addressing.h>

// Local Includes
#include "comm-ffi-types.h"

EXTERN_C_BEGIN

void comm_rma_all_to_all_create(MPI_Comm comm,
                                eap_comm_datatype_t datatype,
                                int count,
                                comm_rma_all_to_all_t **rma_all_to_all);

void comm_rma_all_to_all_free(comm_rma_all_to_all_t *rma_all_to_all);

void comm_rma_all_to_all_invoke(comm_rma_all_to_all_t *rma_all_to_all,
                                abi_ndarray_t const *send,
                                abi_ndarray_t *recv);

void eap_comm_move_f(MPI_Fint comm,
                     eap_comm_datatype_t datatype,
                     eap_local_t const *send_start,
                     eap_local_t const *send_length,
                     abi_ndarray_t const *send_data,
                     eap_local_t const *recv_start,
                     eap_local_t const *recv_length,
                     abi_ndarray_t const *recv_data);

EXTERN_C_END

#endif // COMM_FFI_PATTERNS_H_