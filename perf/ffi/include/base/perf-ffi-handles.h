/**
 * @brief Exposes handle types for eap-perf-ffi.
 *
 * @file perf-ffi-handles.h
 *
 * @date 2018-09-28
 */

#ifndef EAP_PERF_FFI_BASE_HANDLES_H_
#define EAP_PERF_FFI_BASE_HANDLES_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Opaque handle type for eap::perf::TimerRegistry.
 */
typedef struct eap_perf_timer_registry_t eap_perf_timer_registry_t;

/**
 * @brief Handle to a timer in an eap_perf_timer_registry_t
 */
typedef ptrdiff_t eap_perf_timer_handle_t;

/**
 * @brief Value for un-specific timer handle.
 *
 * @details
 *  -1 is used since 0 is a valid timer handle.
 */
#define EAP_PERF_TIMER_HANDLE_NULL (-1)

#endif // EAP_PERF_FFI_BASE_HANDLES_H_