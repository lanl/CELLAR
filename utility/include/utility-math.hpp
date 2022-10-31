/**
 * @file utility-math.hpp
 *
 * @brief Common math functions
 * @date 2019-08-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_MATH_HPP_
#define EAP_UTILITY_MATH_HPP_

// Third Party Includes
#include <Kokkos_Core.hpp>

namespace eap {
namespace utility {
template <typename T>
KOKKOS_FUNCTION T Square(T x) {
    return x * x;
}

template <typename T>
KOKKOS_FUNCTION auto SquarePair(Kokkos::pair<T, T> x) {
    return Kokkos::make_pair(Square(x.first), Square(x.second));
}
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_MATH_HPP_