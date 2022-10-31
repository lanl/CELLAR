#include <comm-timer.hpp>

eap::perf::TimerRegistry<> *eap::comm::GetTimerRegistry() {
    static eap::perf::TimerRegistry<> *global_timer_registry = new eap::perf::TimerRegistry<>();
    return global_timer_registry;
}