/**
 * @file utility-format.hpp
 *
 * @brief Provides utilities for text-formatting data
 * @date 2019-03-25
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_FORMAT_HPP_
#define EAP_UTILITY_FORMAT_HPP_

// STL Includes
#include <sstream>
#include <string>
#include <utility>

// Local Includes
#include "utility-linear_view.hpp"

namespace eap {
namespace utility {

/**
 * @brief Joins each value in `c` with `seperator`, returning the result as a `std::string`
 *
 * @detail
 * TODO: Return a type that implements operator<< so that it adopts format options from the stream.
 *
 * @tparam Collection A collection type that implements `begin` and `end`
 * @param c The collection of values to join
 * @param separator A string the separates each value in `c`
 * @return The joined string
 */
template <typename Collection>
std::string StringJoin(Collection &&c, std::string separator) {
    auto it = std::begin(c);
    auto const end = std::end(c);

    if (it == end) return "";

    std::stringstream ss;
    ss << *it++;

    for (; it != end; ++it) {
        ss << separator << *it;
    }

    return ss.str();
}

/**
 * @brief Joins each value in 1D View `v` with `separator`, returning the result as a `std::string`
 *
 * @detail
 * TODO: Return a type that implements operator<< so that it adopts format options from the stream.
 *
 * @tparam View A 1D Kokkos::View type
 * @param v A collection of values to join
 * @param separator A string that separates each value in `c`
 * @return The joined string
 */
template <typename View>
std::string StringJoinView(View &&v, std::string separator) {
    auto const linear = eap::utility::MakeLinearView(v);
    return StringJoin(linear.Span(), std::move(separator));
}
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_FORMAT_HPP_