/**
 * @file comm-ffi-timer.h
 *
 * @brief Exposes the eap::comm timer to FFI users
 * @date 2019-01-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_FFI_TIMER_H_
#define EAP_COMM_FFI_TIMER_H_

#include <abi-extern.h>
#include <perf-ffi-handles.h>

EXTERN_C_BEGIN

/**
 * @brief Gets a handle to the TimerRegistry used by eap::comm.
 *
 * @return eap_perf_timer_registry_t
 *  FFI handle to the TimerRegistry
 */
eap_perf_timer_registry_t *eap_comm_timer_registry();

EXTERN_C_END

#endif // EAP_COMM_FFI_TIMER_H_