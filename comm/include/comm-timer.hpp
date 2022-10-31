#ifndef EAP_COMM_TIMER_HPP_
#define EAP_COMM_TIMER_HPP_

#include <perf-registry.hpp>

namespace eap {
namespace comm {
/**
 * @brief
 *  Returns the TimerRegistry used by eap::comm.
 *
 * @return eap::perf::TimerRegistry<> *
 *  Pointer to the eap::comm TimerRegistry. Never nullptr.
 */
eap::perf::TimerRegistry<> *GetTimerRegistry();
} // namespace comm
} // namespace eap

#define EAP_COMM_TIME_FUNCTION(TimeNameExpression)                                                 \
    static eap::perf::TimerHandle const eap_comm_timer_handle =                                    \
        eap::comm::GetTimerRegistry()->InsertOrLookupTimer((TimeNameExpression));                  \
                                                                                                   \
    eap::perf::TimedSection<> const eap_comm_time_function =                                       \
        eap::comm::GetTimerRegistry()->TimeSection(eap_comm_timer_handle);

#endif // EAP_COMM_TIMER_HPP_