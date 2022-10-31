/**
 * @brief The umbrella header for the eap-perf library. This header includes all other headers in
 * the eap-perf library. All other header files in eap-perf can be safely included
 * directly, which may be desirabale to improve compile times.
 *
 * @file perf.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_PERF_HPP
#define EAP_PERF_PERF_HPP

#include "perf-internal-fwd.hpp"

#include "perf-clock.hpp"
#include "perf-error.hpp"
#include "perf-registry.hpp"
#include "perf-timer.hpp"

/**
 * @dir perf
 *
 * @brief eap-perf root include directory
 */

/**
 * @dir internal
 *
 * @brief DO NOT USE: Implementation details for eap-perf headers
 */

/**
 * @brief Eulererian Applications Project
 *
 * @details
 *  The eap (Eulerian Applications Project) namespace contains routines and types used in the
 *  production EAP codebase.
 */
namespace eap {
/**
 * @brief EAP Performance Profiling Interfaces
 *
 * @details
 *  The perf namespace contains routines and types for performing performance
 *  profiling of a production codebase. It is targeted towards production-time
 *  performance-testing, not for one-off isolated micro-benchmarking.
 *
 */
namespace perf {}
} // namespace eap

#endif // EAP_PERF_PERF_HPP