#include <comm-ffi-timer.h>

#include <comm-timer.hpp>
#include <error-macros.hpp>
#include <perf-ffi-interop.hpp>

EXTERN_C
eap_perf_timer_registry_t *eap_comm_timer_registry() {
    EAP_EXTERN_PRE

    return eap::perf::ffi::TimerRegistryToFFI(eap::comm::GetTimerRegistry());

    EAP_EXTERN_POST
}