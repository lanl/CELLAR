/**
 * @brief Defines some attributes that may differ on different compilers, systems, etc.
 *  This header should never be included directly.
 *
 * @file sys.hpp
 *
 * @date 2018-09-06
 */

#ifndef EAP_PERF_INTERNAL_SYS_HPP
#define EAP_PERF_INTERNAL_SYS_HPP

// In C++17, this is portable as [[nodiscard]]
#if __cplusplus >= 201703L
#define EAP_PERF_MUST_USE [[nodiscard]]
#else
#define EAP_PERF_MUST_USE
#endif

#endif // EAP_PERF_INTERNAL_SYS_HPP
