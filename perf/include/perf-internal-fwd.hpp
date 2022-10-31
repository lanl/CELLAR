/**
 * @brief Forward declares types when necessary. This header should never be included directly.
 *
 * @file perf-internal-fwd.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_INTERNAL_FWD_HPP
#define EAP_PERF_INTERNAL_FWD_HPP

namespace eap {
namespace perf {

template <typename Clock>
class TimerRegistry;

template <typename Clock>
class TimedSection;

} // namespace perf
} // namespace eap

#endif // EAP_PERF_INTERNAL_FWD_HPP