/**
 * @file utility-diagnostic.hpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Provides some compiler-independent macros to temporarily turn off specific warnings
 * @version 0.1
 * @date 2018-10-24
 *
 * @copyright Copyright (c) 2018
 */

#ifndef EAP_UTILITY_DIAGNOSTIC_HPP
#define EAP_UTILITY_DIAGNOSTIC_HPP

/**
 * @brief Mark that a variable is intentionally unused. For example, it may only be used inside an
 * assert, so there would be a unused variable warning in release builds
 */
#define MARK_UNUSED(x) ((void)(x))

/**
 * @brief Push the current diagnostic state so that it can be restored later after altering it
 */
#if (defined(__INTEL_COMPILER) || defined(_MSC_VER))
// check for Intel compiler first because it ALSO defines __GNUC__
#define DIAGNOSTIC_PUSH _Pragma("warning(push)")
#elif defined(__clang__)
#define DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#elif defined(__xlc__)
#define DIAGNOSTIC_PUSH /* XL does not have a diagnostic push */
#elif defined(__GNUC__)
#define DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#else
#error "Unrecognized compiler"
#endif

/**
 * @brief Restore a previously pushed diagnostic state
 */
#if (defined(__INTEL_COMPILER) || defined(_MSC_VER))
// check for Intel compiler first because it ALSO defines __GNUC__
#define DIAGNOSTIC_POP _Pragma("warning(pop)")
#elif defined(__clang__)
#define DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif defined(__xlc__)
#define DIAGNOSTIC_POP _Pragma("report(pop)")
#elif defined(__GNUC__)
#define DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#else
#error "Unrecognized compiler"
#endif

/**
 * @brief Disable warning related to comparing an unsigned integer to 0, i.e. x < 0 where x is
 * unsigned
 */
#if (defined(__INTEL_COMPILER) || defined(_MSC_VER))
// check for Intel compiler first because it ALSO defines __GNUC__
#define DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO _Pragma("warning(disable : 186)")
#elif defined(__clang__)
#define DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO                                   \
    _Pragma("clang diagnostic ignored \"-Wtype-limits\"")
#elif defined(__xlc__)
#define DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO                                   \
    /* TODO _Pragma("report(disable, \"CCN\")") */
#elif defined(__GNUC__)
#define DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO                                   \
    _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
#else
#error "Unrecognized compiler"
#endif

/**
 * @brief Disable warning related to unused functions
 */
#if (defined(__INTEL_COMPILER) || defined(_MSC_VER))
// check for Intel compiler first because it ALSO defines __GNUC__
#define DIAGNOSTIC_DISABLE_WARNING_UNUSED_FUNCTION /* TODO _Pragma("warning(disable : )") */
#elif defined(__clang__)
#define DIAGNOSTIC_DISABLE_WARNING_UNUSED_FUNCTION                                                 \
    _Pragma("clang diagnostic ignored \"-Wunused-function\"")
#elif defined(__xlc__)
#define DIAGNOSTIC_DISABLE_WARNING_UNUSED_FUNCTION /* TODO _Pragma("report(disable, \"CCN\")") */
#elif defined(__GNUC__)
#define DIAGNOSTIC_DISABLE_WARNING_UNUSED_FUNCTION                                                 \
    _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#else
#error "Unrecognized compiler"
#endif

#endif // EAP_UTILITY_DIAGNOSTIC_HPP
