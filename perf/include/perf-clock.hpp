/**
 * @brief Defines types for dealing with C++ clock types
 *
 * @file perf-clock.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_CLOCK_HPP
#define EAP_PERF_CLOCK_HPP

#include <chrono>

namespace eap {
namespace perf {
/** @brief The default C++ Clock to use in eap-timer */
using DefaultClock = std::chrono::steady_clock;
} // namespace perf
} // namespace eap

#endif // EAP_PERF_CLOCK_HPP