/**
 * @brief Provides interfaces for going back-and-forth between eap-perf-ffi and eap::perf.
 *
 * @file perf-ffi-interop.hpp
 *
 * @date 2018-09-26
 */

#ifndef EAP_PERF_FFI_INTEROP_HPP_
#define EAP_PERF_FFI_INTEROP_HPP_

#include <chrono>

#include <perf-ffi-handles.h>
#include <perf-registry.hpp>

namespace eap {
namespace perf {
namespace ffi {
using TimerRegistryFFI = eap::perf::TimerRegistry<>;
using FFIDuration = std::chrono::duration<int64_t, std::nano>;

inline eap_perf_timer_registry_t *TimerRegistryToFFI(TimerRegistryFFI *registry) {
    return reinterpret_cast<eap_perf_timer_registry_t *>(registry);
}

inline TimerRegistryFFI *TimerRegistryFromFFI(eap_perf_timer_registry_t *registry) {
    return reinterpret_cast<TimerRegistryFFI *>(registry);
}

inline TimerRegistryFFI const *TimerRegistryFromFFI(eap_perf_timer_registry_t const *registry) {
    return reinterpret_cast<TimerRegistryFFI const *>(registry);
}

inline eap_perf_timer_handle_t TimerHandleToFFI(eap::perf::TimerHandle const &handle) {
    return reinterpret_cast<eap_perf_timer_handle_t const &>(handle);
}

inline eap::perf::TimerHandle TimerHandleFromFFI(eap_perf_timer_handle_t const &handle) {
    return reinterpret_cast<eap::perf::TimerHandle const &>(handle);
}

template <typename Duration>
int64_t DurationToFFI(std::optional<Duration> const &duration) {
    if (duration) {
        return std::chrono::duration_cast<FFIDuration>(duration.value()).count();
    } else {
        return -1;
    }
}
} // namespace ffi
} // namespace perf
} // namespace eap

#endif // EAP_PERF_FFI_INTEROP_HPP_