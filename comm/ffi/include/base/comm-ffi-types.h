/**
 * @file comm-ffi-types.h
 *
 * @brief Defines shared FFI types from comm-ffi
 * @date 2019-01-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_FFI_TYPES_H_
#define EAP_COMM_FFI_TYPES_H_

typedef enum _eap_comm_datatype_t {
    eap_comm_datatype_bool,
    eap_comm_datatype_int32,
    eap_comm_datatype_int64,
    eap_comm_datatype_float,
    eap_comm_datatype_double,
} eap_comm_datatype_t;

typedef enum _comm_token_operation_t {
    comm_token_operation_copy,
    comm_token_operation_add,
    comm_token_operation_sub,
    comm_token_operation_max,
    comm_token_operation_min,
} comm_token_operation_t;

typedef struct comm_token_t comm_token_t;
typedef struct comm_token_builder_t comm_token_builder_t;
typedef struct comm_rma_all_to_all_t comm_rma_all_to_all_t;

#endif // EAP_COMM_FFI_TYPES_H_