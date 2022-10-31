/**
 * @brief C Interface to the C++ eap::perf::TimerRegistry.
 *
 * @file perf-ffi-registry.h
 *
 * @date 2018-09-10
 */

#ifndef EAP_PERF_FFI_REGISTRY_H_
#define EAP_PERF_FFI_REGISTRY_H_

#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <abi-extern.h>

#include <perf-ffi-handles.h>

/**
 * @brief Aggregate that receives all statistics associated with a timer.
 */
typedef struct eap_perf_timer_statistics_t {
    /**
     * @brief Number of times timer was run.
     */
    size_t timer_count;

    /**
     * @brief The total run-time of the timer over all runs in ns.
     */
    int64_t sum_time;

    /**
     * @brief The average time of the runs in ns. -1 if the timer hasn't been run at least once.
     */
    int64_t avg_time;

    /**
     * @brief The longest time the timer ran in ns. -1 if the timer hasn't been run at least once.
     */
    int64_t max_time;

    /**
     * @brief The shortest time the timer ran in ns. -1 if the timer hasn't been run at least once.
     */
    int64_t min_time;
} eap_perf_timer_statistics_t;

/**
 * @brief Error type for select eap_perf_timer_registry routines.
 */
typedef enum eap_perf_timer_registry_err_t {
    /**
     * @brief The eap_perf_timer_registry call succeeded as expected.
     */
    eap_perf_timer_registry_success = 0,

    /**
     * @brief The buffer passed to an eap_perf_timer_registry call wasn't large enough.
     */
    eap_perf_timer_registry_buffer_too_small,
} eap_perf_timer_registry_err_t;

EXTERN_C_BEGIN

/**
 * @brief Creates a new eap::perf::TimerRegistry.
 *
 * @details
 *  Can be freed via eap_perf_timer_registry_free.
 *
 *  If the registry lives the life-time of the entire application, it does not need to be freed.
 *
 * @param registry
 *  Out parameter for registry.
 */
void eap_perf_timer_registry_create(eap_perf_timer_registry_t **registry);

/**
 * @brief Frees an eap::perf::TimerRegistry
 *
 * @param registry
 */
void eap_perf_timer_registry_free(eap_perf_timer_registry_t *registry);

/**
 * @brief Creates a new timer in the timer registry with the name "timer_name" if it doesn't already
 *  exist.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_name
 *  The name of the timer. Must end with a NULL.
 * @param timer_handle
 *  Out parameter receiving a handle to the timer.
 */
void eap_perf_timer_registry_insert_or_lookup_timer(eap_perf_timer_registry_t *registry,
                                                    char const *timer_name,
                                                    eap_perf_timer_handle_t *timer_handle);

/**
 * @brief Pushes a timer to the timer stack, starting the timer if it's the first timer in the
 *  stack.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_handle
 *  Handle to a timer in the timer registry.
 */
void eap_perf_timer_registry_push_timer(eap_perf_timer_registry_t *registry,
                                        eap_perf_timer_handle_t timer_handle);

/**
 * @brief Pops a timer from the timer stack, stopping the timer if it's the last timer in the stack.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_handle
 *  Optional (NULL allowed). Out parameter containing the popped timer.
 */
void eap_perf_timer_registry_pop_timer(eap_perf_timer_registry_t *registry,
                                       eap_perf_timer_handle_t *timer_handle);

/**
 * @brief Gets the length of the name of the timer, not including the null pointer.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_handle
 *  Timer to get name length of.
 * @param timer_name_length
 *  Returns the length of the timer name.
 */
void eap_perf_timer_registry_get_timer_name_length(eap_perf_timer_registry_t const *registry,
                                                   eap_perf_timer_handle_t timer_handle,
                                                   size_t *timer_name_length);

/**
 * @brief Gets the name of the timer associated with timer_handle.
 *
 * @details
 *  If timer_name_length is not large enough to hold the name of the timer, returns
 *  eap_perf_timer_registry_buffer_too_small. If the size of the timer name is not previously known,
 *  call eap_perf_timer_registry_get_timer_name_length first.
 *
 *  timer_name_length should include the null character.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_handle
 *  Timer to get the name of.
 * @param timer_name
 *  Buffer that receives the name of the timer. Will be null-terminated on success.
 * @param timer_name_length
 *  The length of the timer_name buffer, including space for the null-terminator.
 * @return eap_perf_timer_registry_err_t
 *  eap_perf_timer_registry_success on success.
 */
eap_perf_timer_registry_err_t
eap_perf_timer_registry_get_timer_name(eap_perf_timer_registry_t const *registry,
                                       eap_perf_timer_handle_t timer_handle,
                                       char *timer_name,
                                       size_t timer_name_length);

/**
 * @brief Gets the statistics associated with the timer.
 *
 * @param registry
 *  A previously created timer registry.
 * @param timer_handle
 *  Timer to get the statistics of
 * @param timer_stats
 *  Out parameter containing the statistics.
 */
void eap_perf_timer_registry_get_statistics(eap_perf_timer_registry_t const *registry,
                                            eap_perf_timer_handle_t timer_handle,
                                            eap_perf_timer_statistics_t *timer_stats);

/**
 * @brief Gets the number of timers in the registry.
 *
 * @param registry
 *  A previously created timer registry.
 * @param num_timers
 *  Out parameter returning the number of timers.
 */
void eap_perf_timer_registry_get_num_timers(eap_perf_timer_registry_t const *registry,
                                            size_t *num_timers);

/**
 * @brief Begins iteration over the timers in registry
 *
 * @param registry
 *  The timer registry to iterate
 * @param start
 *  The first timer in the registry
 */
void eap_perf_timer_registry_create_timer_iterator(eap_perf_timer_registry_t const *registry,
                                                   eap_perf_timer_handle_t *start);

/**
 * @brief Advances to the next timer in the registry.
 *
 * @param registry
 *  The timer registry to iterate
 * @param timer_handle
 *  Inout parameter. timer_handle will receive the next timer in the registry.
 * @return true
 *  There are more timers available in the registry
 * @return false
 *  Iteration has completed.
 */
bool eap_perf_timer_registry_next_timer(eap_perf_timer_registry_t const *registry,
                                        eap_perf_timer_handle_t *timer_handle);

EXTERN_C_END

#endif // EAP_PERF_FFI_REGISTRY_H_