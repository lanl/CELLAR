#include <perf-ffi-registry.h>

#include <algorithm>
#include <chrono>
#include <optional>
#include <perf-ffi-interop.hpp>
#include <perf-registry.hpp>

using eap::perf::ffi::DurationToFFI;
using eap::perf::ffi::TimerHandleFromFFI;
using eap::perf::ffi::TimerHandleToFFI;
using eap::perf::ffi::TimerRegistryFromFFI;
using eap::perf::ffi::TimerRegistryToFFI;
using std::chrono::duration_cast;

static_assert(sizeof(eap_perf_timer_handle_t) == sizeof(eap::perf::TimerHandle),
              "The FFI code is no longer valid if eap::perf::TimerHandle stops being the same size "
              "as eap_perf_timer_handle_t");

EXTERN_C
void eap_perf_timer_registry_create(eap_perf_timer_registry_t **registry) {
    *registry = TimerRegistryToFFI(new eap::perf::ffi::TimerRegistryFFI());
}

EXTERN_C void eap_perf_timer_registry_free(eap_perf_timer_registry_t *registry) {
    delete TimerRegistryFromFFI(registry);
}

EXTERN_C
void eap_perf_timer_registry_insert_or_lookup_timer_f(eap_perf_timer_registry_t *registry,
                                                      char const *timer_name,
                                                      size_t timer_name_length,
                                                      eap_perf_timer_handle_t *timer_handle) {
    *timer_handle = TimerHandleToFFI(TimerRegistryFromFFI(registry)->InsertOrLookupTimer(
        std::string_view(timer_name, timer_name_length)));
}

EXTERN_C
void eap_perf_timer_registry_insert_or_lookup_timer(eap_perf_timer_registry_t *registry,
                                                    char const *timer_name,
                                                    eap_perf_timer_handle_t *timer_handle) {
    *timer_handle = TimerHandleToFFI(
        TimerRegistryFromFFI(registry)->InsertOrLookupTimer(std::string_view(timer_name)));
}

EXTERN_C
void eap_perf_timer_registry_push_timer(eap_perf_timer_registry_t *registry,
                                        eap_perf_timer_handle_t timer_handle) {
    TimerRegistryFromFFI(registry)->PushTimer(TimerHandleFromFFI(timer_handle));
}

EXTERN_C
void eap_perf_timer_registry_pop_timer(eap_perf_timer_registry_t *registry,
                                       eap_perf_timer_handle_t *timer_handle) {
    auto const popped = TimerHandleToFFI(TimerRegistryFromFFI(registry)->PopTimer());

    if (timer_handle) {
        *timer_handle = popped;
    }
}

EXTERN_C
void eap_perf_timer_registry_get_timer_name_length(eap_perf_timer_registry_t const *registry,
                                                   eap_perf_timer_handle_t timer_handle,
                                                   size_t *timer_name_length) {
    *timer_name_length =
        TimerRegistryFromFFI(registry)->GetTimerName(TimerHandleFromFFI(timer_handle)).size();
}

EXTERN_C
eap_perf_timer_registry_err_t
eap_perf_timer_registry_get_timer_name(eap_perf_timer_registry_t const *registry,
                                       eap_perf_timer_handle_t timer_handle,
                                       char *timer_name,
                                       size_t timer_name_length) {
    auto const name =
        TimerRegistryFromFFI(registry)->GetTimerName(TimerHandleFromFFI(timer_handle));

    // The timer_name buffer must be large enough to receive "name" and a null-terminator
    if (timer_name_length < name.size() + 1) {
        return eap_perf_timer_registry_buffer_too_small;
    }

    // copy name into the timer_name buffer and add a null terminator
    std::copy(name.begin(), name.end(), timer_name);
    timer_name[name.size()] = 0;

    return eap_perf_timer_registry_success;
}

EXTERN_C
void eap_perf_timer_registry_get_statistics(eap_perf_timer_registry_t const *registry,
                                            eap_perf_timer_handle_t timer_handle,
                                            eap_perf_timer_statistics_t *timer_stats) {
    auto const &timer = TimerRegistryFromFFI(registry)->GetTimer(TimerHandleFromFFI(timer_handle));

    timer_stats->timer_count = timer.TimerCount();
    timer_stats->sum_time = duration_cast<eap::perf::ffi::FFIDuration>(timer.SumTime()).count();

    timer_stats->avg_time = DurationToFFI(timer.AverageTime());
    timer_stats->max_time = DurationToFFI(timer.MaxTime());
    timer_stats->min_time = DurationToFFI(timer.MinTime());
}

EXTERN_C
void eap_perf_timer_registry_get_num_timers(eap_perf_timer_registry_t const *registry,
                                            size_t *num_timers) {
    *num_timers = TimerRegistryFromFFI(registry)->Count();
}

EXTERN_C
void eap_perf_timer_registry_create_timer_iterator(eap_perf_timer_registry_t const *,
                                                   eap_perf_timer_handle_t *timer_handle) {
    // handle starts at -1, i.e. just before the first timer. It advances to valid index on the
    // first call to next.
    //
    // This is to make it easy to use the return value of eap_perf_timer_registry_next_timer in
    // a loop conditional.
    *timer_handle = EAP_PERF_TIMER_HANDLE_NULL;
}

EXTERN_C
bool eap_perf_timer_registry_next_timer(eap_perf_timer_registry_t const *registry,
                                        eap_perf_timer_handle_t *timer_handle) {
    auto const &timer_registry = *TimerRegistryFromFFI(registry);
    if (*timer_handle >= static_cast<ptrdiff_t>(timer_registry.Count())) {
        return false;
    }

    *timer_handle += 1;
    return *timer_handle < (eap_perf_timer_handle_t)timer_registry.Count();
}