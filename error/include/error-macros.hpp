/**
 * @file error-macros.hpp
 *
 * @brief Defines macros for originating and re-propagating errors.
 * @date 2018-10-03
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_ERROR_MACROS_HPP_
#define EAP_ERROR_MACROS_HPP_

// STL Includes
#include <cstdint>
#include <cstdlib>
#include <sstream>

// Third Party Includes
#include <nonstd/string_view.hpp>

// Package includes
#include "error-internal.hpp"
#include "error-records.hpp"

// There's an outstanding bug in VSCode causing constexpr to intersect poorly with cosntant string
// values. This makes VSCode not red-squiggle when using the EAP Error macros, which uses __FUNC__
// and __PRETTYFUNC__ with constexpr.
//
// See: https://github.com/Microsoft/vscode-cpptools/issues/2939
#ifdef __INTELLISENSE__
#define EAP_ERROR_CONSTEXPR
#else
#define EAP_ERROR_CONSTEXPR constexpr
#endif

#if defined(__clang__)
// clang supports constexpr __PRETTY_FUNCTION__/__func__
#define EAP_ERROR_FUNC_NAME_CONSTEXPR EAP_ERROR_CONSTEXPR
#elif (defined(__GNUG__) && (__GNUC__ >= 8))
// __PRETTY_FUNCTION__/__func__ only useable from constexpr in GCC as of GCC 8
// See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66639
#define EAP_ERROR_FUNC_NAME_CONSTEXPR EAP_ERROR_CONSTEXPR
#else
#define EAP_ERROR_FUNC_NAME_CONSTEXPR
#endif

#define EAP_FILE_RELATIVE                                                                          \
    (::eap::error::internal::relative_path(                                                        \
        ::eap::error::internal::SourceRoot,                                                        \
        nonstd::string_view(__FILE__, ::eap::error::internal::static_string_length(__FILE__))))

#if defined(__GNUG__)
#define EAP_FUNC __PRETTY_FUNCTION__
#else
#define EAP_FUNC __func__
#endif

#define EAP_FUNC_SV                                                                                \
    (nonstd::string_view(EAP_FUNC, ::eap::error::internal::static_string_length(EAP_FUNC)))

#if defined(__GNUG__)
#define EAP_PRETTY_FUNC (::eap::error::internal::trim_pretty_function(EAP_FUNC_SV))
#else
#define EAP_PRETTY_FUNC EAP_FUNC_SV
#endif

// Our use of the Intel compiler over-aggressively inlines std::iostream functions, exploding
// compile times in some cases. We tell Intel to not inline the stream code - don't care for other
// compilers at this time (no compile time issues).
#if defined(__INTEL_COMPILER)
#define EAP_ERROR_INTERNAL_SHOULD_NOINLINE _Pragma("noinline")
#else
#define EAP_ERROR_INTERNAL_SHOULD_NOINLINE
#endif

/**
 * @brief A constant expression that describes the file+func+line tuple of the line of code that
 * EAP_LOCATION is evaluated on.
 */
#define EAP_LOCATION (::eap::error::Location{EAP_FILE_RELATIVE, EAP_PRETTY_FUNC, __LINE__})

/**
 * @brief Indirects between overloaded macros with one or two arguments
 *
 * @details
 * See: https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
 */
#define EAP_IMPL_OVERLOAD_MACRO1_2(_1, _2, NAME, ...) NAME

/**
 * @brief Indirects between overloaded macros with two or three arguments
 *
 * @details
 * See: https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
 */
#define EAP_IMPL_OVERLOAD_MACRO2_3(_1, _2, _3, NAME, ...) NAME

/**
 * @brief Evaluates iostream `msg` as a `std::string`
 */
#define EAP_ERROR_EVALUATE_STRING_STREAM(msg)                                                      \
    (([&]() __attribute__((always_inline)) {                                                       \
        EAP_ERROR_INTERNAL_SHOULD_NOINLINE{                                                        \
            std::ostringstream ss;                                                                 \
            ss << msg;                                                                             \
            return ss.str();                                                                       \
    }                                                                                              \
    })())

/**
 * @brief See: EE_ASSERT_NOT_NULL
 */
#define EAP_ERROR_ASSERT_NOT_NULL(parameter)                                                       \
    do {                                                                                           \
        if ((parameter) == nullptr) {                                                              \
            EAP_ERROR_RAISE_MSG("'" << #parameter << "' must not be nullptr");                     \
        }                                                                                          \
    } while (false)

/**
 * @brief See: EE_ASSERT_NOT_NULL
 */
#define EAP_ERROR_ASSERT_NOT_NULL_MSG(parameter, msg)                                              \
    do {                                                                                           \
        if ((parameter) == nullptr) {                                                              \
            EAP_ERROR_RAISE_MSG("'" << #parameter << "' must not be nullptr: " << msg);            \
        }                                                                                          \
    } while (false)

/**
 * @brief See: EE_ASSERT_EQ
 */
#define EAP_ERROR_ASSERT_EQ(a, b)                                                                  \
    do {                                                                                           \
        auto const a_eval = (a);                                                                   \
        auto const b_eval = (b);                                                                   \
        if (!(a_eval == b_eval)) {                                                                 \
            EAP_ERROR_RAISE_MSG("Expected `" << #a << "` (" << a_eval << ") == `" << #b << "` ("   \
                                             << b_eval << ")");                                    \
        }                                                                                          \
    } while (false)

/**
 * @brief See: EE_ASSERT_EQ
 */
#define EAP_ERROR_ASSERT_EQ_MSG(a, b, msg)                                                         \
    do {                                                                                           \
        auto const a_eval = (a);                                                                   \
        auto const b_eval = (b);                                                                   \
        if (!(a_eval == b_eval)) {                                                                 \
            EAP_ERROR_RAISE_MSG("Expected `" << #a << "` (" << a_eval << ") == `" << #b << "` ("   \
                                             << b_eval << "): " << msg);                           \
        }                                                                                          \
    } while (false)

/// @brief See: EE_ASSERT_NE
#define EAP_ERROR_ASSERT_NE(a, b)                                                                  \
    do {                                                                                           \
        auto const a_eval = (a);                                                                   \
        auto const b_eval = (b);                                                                   \
        if (!(a_eval != b_eval)) {                                                                 \
            EAP_ERROR_RAISE_MSG("Expected `" << #a << "` (" << a_eval << ") != `" << #b << "` ("   \
                                             << b_eval << ")");                                    \
        }                                                                                          \
    } while (false)

/// @brief See: EE_ASSERT_NE
#define EAP_ERROR_ASSERT_NE_MSG(a, b, msg)                                                         \
    do {                                                                                           \
        auto const a_eval = (a);                                                                   \
        auto const b_eval = (b);                                                                   \
        if (!(a_eval != b_eval)) {                                                                 \
            EAP_ERROR_RAISE_MSG("Expected `" << #a << "` (" << a_eval << ") != `" << #b << "` ("   \
                                             << b_eval << "): " << msg);                           \
        }                                                                                          \
    } while (false)

/// @brief See: EE_ASSERT
#define EAP_ERROR_ASSERT(expr)                                                                     \
    do {                                                                                           \
        auto const eap_error_internal_eval = (expr);                                               \
        if (!eap_error_internal_eval) {                                                            \
            EAP_ERROR_RAISE_MSG("Expected `" << #expr << "`");                                     \
        }                                                                                          \
    } while (false)

/// @brief See: EE_ASSERT
#define EAP_ERROR_ASSERT_MSG(expr, msg)                                                            \
    do {                                                                                           \
        auto const eap_error_internal_eval = (expr);                                               \
        if (!eap_error_internal_eval) {                                                            \
            EAP_ERROR_RAISE_MSG("Expected `" << #expr << "`: " << msg);                            \
        }                                                                                          \
    } while (false)

/// See: EE_RAISE
#define EAP_ERROR_RAISE_MSG(msg)                                                                   \
    do {                                                                                           \
        static EAP_ERROR_FUNC_NAME_CONSTEXPR ::eap::error::Location const                          \
            eap_error_internal_raise_location{                                                     \
                eap_error_internal_prelude_file,                                                   \
                ::eap::error::internal::trim_pretty_function(eap_error_internal_prelude_func),     \
                __LINE__};                                                                         \
        EAP_ERROR_INTERNAL_SHOULD_NOINLINE                                                         \
        throw ::eap::error::RaisedError(eap_error_internal_raise_location,                         \
                                        EAP_ERROR_EVALUATE_STRING_STREAM(msg));                    \
    } while (false)

/// See: EE_CHECK
#define EAP_ERROR_CHECK_MSG(expression, msg)                                                       \
    /*                                                                                             \
        We force always_inline here - the function defined by EAP_ERROR_CHECK_MSG is called in     \
        exactly one location, so it's always better to inline it                                   \
    */                                                                                             \
    (([&]() -> decltype(auto) {                                                                    \
        /* Initialize location here to use captured "file" and "function" variables. */            \
        static EAP_ERROR_FUNC_NAME_CONSTEXPR ::eap::error::Location const                          \
            eap_error_internal_propagate_location{                                                 \
                eap_error_internal_prelude_file,                                                   \
                ::eap::error::internal::trim_pretty_function(eap_error_internal_prelude_func),     \
                __LINE__};                                                                         \
        try {                                                                                      \
            return (expression);                                                                   \
        } catch (...) {                                                                            \
            EAP_ERROR_INTERNAL_SHOULD_NOINLINE                                                     \
            ::eap::error::internal::PropagateException(eap_error_internal_propagate_location,      \
                                                       ::nonstd::string_view{#expression},         \
                                                       EAP_ERROR_EVALUATE_STRING_STREAM(msg));     \
        }                                                                                          \
    })())

/// See: EE_PRELUDE
#define EAP_ERROR_PRELUDE                                                                          \
    static EAP_ERROR_CONSTEXPR nonstd::string_view const eap_error_internal_prelude_file =         \
        EAP_FILE_RELATIVE;                                                                         \
    /*                                                                                             \
        We don't assign the truncated string here because we don't want it to truncate until we    \
        actually use it to avoid wasted cycles when there is no error. This can't always be        \
        constexpr because some platforms don't support constexpr __PRETTY_FUNCTION__/__func__      \
     */                                                                                            \
    static EAP_ERROR_FUNC_NAME_CONSTEXPR nonstd::string_view const                                 \
        eap_error_internal_prelude_func = EAP_FUNC_SV;                                             \
    static EAP_ERROR_CONSTEXPR auto const eap_error_internal_prelude_line = __LINE__;              \
    /* It's possible that in some contexts these values aren't used, so mark as unused */          \
    ((void)(eap_error_internal_prelude_file));                                                     \
    ((void)(eap_error_internal_prelude_func));                                                     \
    ((void)(eap_error_internal_prelude_line));

/// See: EE_DIAG_PRE
#define EAP_ERROR_DIAG_PRE                                                                         \
    EAP_ERROR_PRELUDE                                                                              \
    try {

/// See: EE_DIAG_PRE
#define EAP_ERROR_DIAG_POST                                                                        \
    }                                                                                              \
    catch (...) {                                                                                  \
        static EAP_ERROR_FUNC_NAME_CONSTEXPR ::eap::error::Location const                          \
            eap_error_internal_post_location{                                                      \
                eap_error_internal_prelude_file,                                                   \
                ::eap::error::internal::trim_pretty_function(eap_error_internal_prelude_func),     \
                eap_error_internal_prelude_line};                                                  \
        ::eap::error::internal::PropagateException(                                                \
            eap_error_internal_post_location, ::nonstd::nullopt, "Routine failed");                \
        std::exit(EXIT_FAILURE); /* HACK: Intel does not recognize the [[noreturn]] attribute */   \
    }

/// See: EE_DIAG_PRE
#define EAP_ERROR_DIAG_POST_MSG(msg)                                                               \
    }                                                                                              \
    catch (...) {                                                                                  \
        static EAP_ERROR_FUNC_NAME_CONSTEXPR ::eap::error::Location const                          \
            eap_error_internal_post_location{                                                      \
                eap_error_internal_prelude_file,                                                   \
                ::eap::error::internal::trim_pretty_function(eap_error_internal_prelude_func),     \
                eap_error_internal_prelude_line};                                                  \
        EAP_ERROR_INTERNAL_SHOULD_NOINLINE                                                         \
        ::eap::error::internal::PropagateException(eap_error_internal_post_location,               \
                                                   ::nonstd::nullopt,                              \
                                                   EAP_ERROR_EVALUATE_STRING_STREAM(msg));         \
        std::exit(EXIT_FAILURE); /* HACK: Intel does not recognize the [[noreturn]] attribute */   \
    }

/**
 * @brief Used to abort at `extern "C"` boundaries rather than propagating an exception into non-C++
 * code. Requires `EAP_EXTERN_POST` at the end of the routine. Implies `EAP_ERROR_PRELUDE`.
 */
#define EAP_EXTERN_PRE                                                                             \
    EAP_ERROR_PRELUDE                                                                              \
    try {

/**
 * @brief See `EAP_EXTERN_PRE`.
 */
#define EAP_EXTERN_POST                                                                            \
    }                                                                                              \
    catch (...) {                                                                                  \
        static EAP_ERROR_FUNC_NAME_CONSTEXPR ::eap::error::Location const                          \
            eap_error_internal_abort_location{eap_error_internal_prelude_file,                     \
                                              EAP_PRETTY_FUNC,                                     \
                                              eap_error_internal_prelude_line};                    \
        ::eap::error::internal::AbortWithException(eap_error_internal_abort_location);             \
        std::exit(EXIT_FAILURE); /* HACK: Intel does not recognize the [[noreturn]] attribute */   \
    }

// Alias macros

/**
 * @brief Writes var to an output stream with the variable name
 *
 */
#define EE_DBG(var)                                                                                \
    "`" #var "`"                                                                                   \
        << " = " << (var)

/**
 * @brief Asserts that `parameter` evaluates to a value that is not `nullptr`.
 *
 * @param parameter A pointer value
 * @param msg (Optional) a stream statement that is appended to the error message.
 */
#define EE_ASSERT_NOT_NULL(...)                                                                    \
    EAP_IMPL_OVERLOAD_MACRO1_2(                                                                    \
        __VA_ARGS__, EAP_ERROR_ASSERT_NOT_NULL_MSG, EAP_ERROR_ASSERT_NOT_NULL)                     \
    (__VA_ARGS__)

/**
 * @brief Asserts that `a` evaluates to a value that compares `==` to `b`.
 *
 * @param a expression
 * @param b expression
 * @param msg (Optional) a stream statement that is appended to the error message.
 */
#define EE_ASSERT_EQ(...)                                                                          \
    EAP_IMPL_OVERLOAD_MACRO2_3(__VA_ARGS__, EAP_ERROR_ASSERT_EQ_MSG, EAP_ERROR_ASSERT_EQ)          \
    (__VA_ARGS__)

/**
 * @brief Asserts that `a` evaluates to a value that compares `!=` to `b`.
 *
 * @param a expression
 * @param b expression
 * @param msg (Optional) a stream statement that is appended to the error message.
 */
#define EE_ASSERT_NE(...)                                                                          \
    EAP_IMPL_OVERLOAD_MACRO2_3(__VA_ARGS__, EAP_ERROR_ASSERT_NE_MSG, EAP_ERROR_ASSERT_NE)          \
    (__VA_ARGS__)

/**
 * @brief Asserts that `expr` evaluates to `true`.
 *
 * @param expr boolean expression
 * @param msg (Optional) a stream statement that is appended to the error message.
 */
#define EE_ASSERT(...)                                                                             \
    EAP_IMPL_OVERLOAD_MACRO1_2(__VA_ARGS__, EAP_ERROR_ASSERT_MSG, EAP_ERROR_ASSERT)(__VA_ARGS__)

/**
 * @brief Originates an error
 *
 * @details
 * This macro makes it easy to raise a new error with file, line, and function information. It is
 * augmented by information provided in `msg`.
 *
 * @param msg a stream statement that is appended to the error message.
 */
#define EE_RAISE EAP_ERROR_RAISE_MSG

/**
 * @brief Executes `expression`, adding additional context when an error occurs. On success,
 * evaluates to the result of `expression`. Requires an EAP Error prelude such as EAP_ERROR_PRELUDE.
 *
 * @param expression An expression which can fail.
 * @param msg (Optional) a stream statement that is appended to the error message.
 *
 * @details
 * For example, to create a `std::vector` and to check it for allocation issues:
 *
 * \code{.cpp}
 * auto my_big_allocation =
 *     EE_CHECK(std::vector<double>(big_size),
 *              "Failed to allocate a large vector of size " << big_size);
 * \endcode
 *
 * This calls the `std::vector` constructor in a checked context, and then assigns that allocated
 * vector to `my_big_allocation`. The stream statement is only evaluated on failure.
 *
 * There's no way to assert that the EAP_LOCATION is evaluated at compile time in this context
 * since this macro needs to evaluate to an expression. Therefore we can't use a constexpr variable
 * declaration prior to the immediately-invoked lambda function. However, if we were to do the
 * constexpr assignment to EAP_LOCATION inside of the lambda, EAP_PRETTY_FUNC would evaluate to the
 * wrong function. This is why `EAP_ERROR_PRELUDE` is needed.
 */
#define EE_CHECK EAP_ERROR_CHECK_MSG

/**
 * @brief Captures file and function information. Required to use `EE_CHECK`.
 *
 * @callgraph
 */
#define EE_PRELUDE EAP_ERROR_PRELUDE

/**
 * @brief Captures additional information on a failure as it propagates up call stack. Must be used
 * with `EE_DIAG_POST` or `EE_DIAG_POST_MSG`. Implies `EE_PRELUDE`.
 *
 * @details
 * Example:
 * \code{.cpp}
 * void MyFunction(int argument) {
 *     EE_DIAG_PRE
 *
 *     // Some code here that can fail.
 *
 *     EE_DIAG_POST_MSG("argument = " << argument)
 * }
 * \endcode
 */
#define EE_DIAG_PRE EAP_ERROR_DIAG_PRE

/// See: EE_DIAG_PRE
#define EE_DIAG_POST EAP_ERROR_DIAG_POST

/// See: EE_DIAG_PRE
#define EE_DIAG_POST_MSG EAP_ERROR_DIAG_POST_MSG

#endif // EAP_ERROR_MACROS_HPP_