/**
 * @file error-internal.hpp
 *
 * @brief Compile-time routines for manipulating __FILE__ and __PRETTY_FUNCTION__
 * @date 2019-02-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_ERROR_INTERNAL_HPP_
#define EAP_ERROR_INTERNAL_HPP_

// STL Includes
#include <cstdint>

// Third Party Includes
#include <string_view>

/**
 * @brief CMake uses full file paths, so this should be set when building using CMake to make the
 * output file names relative to the source directory.
 */
#ifndef EAP_FILE_ROOT
#define EAP_FILE_ROOT ""
#endif

namespace eap {
namespace error {
namespace internal {
template <size_t N>
constexpr size_t static_string_length(char const (&)[N]) {
    return N - 1;
}

constexpr std::string_view SourceRoot =
    std::string_view{EAP_FILE_ROOT, static_string_length(EAP_FILE_ROOT)};

constexpr std::string_view relative_path(std::string_view const &root,
                                            std::string_view const &path) {
    if (root.length() > path.length()) {
        return path;
    }

    if (root.empty()) return path;

    size_t pre = 0;
    for (pre = 0; pre < root.length(); pre++) {
        if (root[pre] != path[pre]) {
            return path;
        }
    }

    if (root[root.length() - 1] != '/' && path[pre] != '/') return path;

    while (path[pre] == '/')
        pre++;

    auto new_path = path;

    new_path.remove_prefix(pre);
    return new_path;
}

constexpr size_t close_brackets(std::string_view const &func) {
    auto open = func[0];

    char close = 0;
    if (open == '<') {
        close = '>';
    } else if (open == '[') {
        close = ']';
    } else if (open == '(') {
        close = ')';
    } else {
        throw "Not a bracket!";
    }

    size_t i = 1;
    while (i < func.length()) {
        if (func[i] == close) return i;

        switch (func[i]) {
        case '<':
        case '[':
        case '(':
            i += close_brackets(func.substr(i));
            break;
        }

        i++;
    }

    // go to the end of the function if the bracket is never closed.
    return i;
}

constexpr size_t parse_type(std::string_view const &func) {
    size_t i = 0;

    while (i < func.length()) {
        switch (func[i]) {
        case ' ':
            return i;
        case '<':
        case '[':
        case '(':
            i += close_brackets(func.substr(i));
            break;
        }

        i++;
    }

    return 0;
}

/**
 * @brief Trims __PRETTY_FUNCTION__ to namespace qualified function name.
 *
 * @details
 * By default __PRETTY_FUNCTION__ gives a very verbose output. For example, this function
 * would be `std::string_view eap::internal::trim_pretty_function(std::string_view const &)`.
 * This trims that down to `eap::internal::trim_pretty_function`.
 *
 * @param func The output of __PRETTY_FUNCTION__
 * @return constexpr std::string_view Trimmed to just the function name
 */
constexpr std::string_view trim_pretty_function(std::string_view const &func) {
    size_t i = 0;

    i += parse_type(func);

    // skip white space
    while (i < func.length() && func[i] == ' ')
        i++;

    auto const without_type = func.substr(i);

    // function name is everything up to parentheses
    i = 0;
    while (i < func.length() && without_type[i] != '(')
        i++;

    return without_type.substr(0, i);
}
} // namespace internal
} // namespace error
} // namespace eap

#endif // EAP_ERROR_INTERNAL_HPP_