/**
 * @file utility-fortran_index.hpp
 *
 * @brief Defines integer types for dealing specifically with indices in Fortran
 * @date 2019-03-01
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_FORTRAN_INDEX_HPP_
#define EAP_UTILITY_FORTRAN_INDEX_HPP_

// Local Includes
#include "utility-offset_integer.hpp"
#include "utility-optional_integer.hpp"

namespace eap {
namespace utility {
/**
 * @brief
 * A FortranIndex presents a 0-based logical index, but stores as a 1-based index.
 *
 * @details
 * This is used to
 * shared arrays of indices between C++ and Fortran without having to copy. If the array does not
 * need to be shared, prefer raw integer types.
 *
 * @tparam Integer Integer type. Must satisfy is_integer
 */
template <typename Integer>
using FortranIndex = OffsetInteger<Integer, 1>;

/**
 * @brief
 * Presents an "optional" 0-index interface, but stores as 1-index. Logical values that fall within
 * the "negative" signed range of the address type are considred nullint values.
 *
 * @details
 * On the Fortran side, all non-positive integers are considered illegal. This maps to that.
 *
 * For unsigned int types, this corresponds to all values from (~0 >> 1) + 1 through ~0 being "null"
 *
 * For signed int types, all logical negative integer values are invalid, or non-positive Fortran
 * values.
 *
 * @tparam Integer Integer type. Must satisfy is_integer
 */
template <typename Integer>
using OptionalFortranIndex = OptionalInteger<
    FortranIndex<Integer>,
    Integer,
    std::is_signed<Integer>::value ? Integer(-1)
                                   : (~Integer(0) >> 1) + 1, // "-1" for signed,
                                                             // 0x80000000... for unsigned
    !std::is_signed<Integer>::value // "ToMax" - All negatives for signed, everything 0x80000000...
                                    // and above for unsigned
    >;

namespace literals {
constexpr FortranIndex<int> operator"" _fi(unsigned long long int x) noexcept { return (int)(x); }

constexpr FortranIndex<long int> operator"" _fil(unsigned long long int x) noexcept {
    return (long int)(x);
}
constexpr FortranIndex<long int> operator"" _fill(unsigned long long int x) noexcept {
    return (long long int)(x);
}

constexpr FortranIndex<unsigned int> operator"" _fiu(unsigned long long int x) noexcept {
    return (unsigned int)(x);
}

constexpr FortranIndex<unsigned long int> operator"" _fiul(unsigned long long int x) noexcept {
    return (unsigned long int)(x);
}

constexpr FortranIndex<unsigned long long int>
operator"" _fiull(unsigned long long int x) noexcept {
    return (unsigned long long int)(x);
}
} // namespace literals
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_FORTRAN_INDEX_HPP_