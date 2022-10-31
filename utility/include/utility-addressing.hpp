/**
 * @file utility-addressing.hpp
 *
 * @brief Defines types for addressing in C++
 * @date 2019-01-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_ADDRESSING_HPP_
#define EAP_UTILITY_ADDRESSING_HPP_

// STL Includes
#include <cstdint>

// Local Includes
#include <utility-fortran_index.hpp>
#include <utility-offset_integer.hpp>
#include <utility-optional_integer.hpp>

// HACK: We use these preprocessor defines to bypass type aliasing in limited situations where
// TotalView is not resolving type transformations for aliased sub-types.
#define LOCAL_INDEX_T_ALIAS std::uint32_t
#define LOCAL_DIFF_T_ALIAS std::int32_t
#define GLOBAL_INDEX_T_ALIAS std::uint64_t

namespace eap {

/**
 * @brief Cell index or address type used for local indices on a single PE.
 */
using local_index_t = LOCAL_INDEX_T_ALIAS;

/**
 * @brief Type for differences between local cell indices.
 */
using local_diff_t = LOCAL_DIFF_T_ALIAS;

/**
 * @brief Cell index or address type used for global indices across the entire simulations.
 */
using global_index_t = GLOBAL_INDEX_T_ALIAS;

/**
 * @brief Local cell index that may be uninhabited
 */
using OptionalLocalIndex = utility::OptionalInteger<local_index_t>;

/**
 * @brief Global cell index that may be uninhabited
 */
using OptionalGlobalIndex = utility::OptionalInteger<global_index_t>;

/**
 * @brief
 * Presents a 0-index interface, but stores as 1-index. For sharing local index arrays across the
 * language boundary.
 */
using FortranLocalIndex = utility::FortranIndex<local_index_t>;

/**
 * @brief
 * Presents a 0-index interface, but stores as 1-index. For sharing global index arrays across the
 * language boundary.
 */
using FortranGlobalIndex = utility::FortranIndex<global_index_t>;

/**
 * @brief
 * Presents an "optional" 0-index interface, but stores as 1-index. Logical values that fall within
 * the "negative" signed range of the address type are considred nullint values.
 *
 * @details
 * On the Fortran side, all non-positive integers are considered illegal. This maps to that.
 */
using OptionalFortranLocalIndex = utility::OptionalFortranIndex<local_index_t>;

/**
 * @brief
 * Presents an "optional" 0-index interface, but stores as 1-index. Logical values that fall within
 * the "negative" signed range of the address type are considred nullint values.
 *
 * @details
 * On the Fortran side, all non-positive integers are considered illegal. This maps to that.
 */
using OptionalFortranGlobalIndex = utility::OptionalFortranIndex<global_index_t>;
} // namespace eap

#endif // EAP_UTILITY_ADDRESSING_HPP_