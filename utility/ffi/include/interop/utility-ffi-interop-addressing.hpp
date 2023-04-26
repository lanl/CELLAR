/**
 * @file utility-ffi-interop-addressing.hpp
 *
 * @brief Interop between C addresses and C++ addresses
 * @date 2019-09-11
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_FFI_INTEROP_ADDRESSING_HPP_
#define EAP_UTILITY_FFI_INTEROP_ADDRESSING_HPP_

// Third Party Includes
#include <span>

// Local Includes
#include <utility-addressing.hpp>
#include <utility-ffi-addressing.h>

namespace eap {
namespace utility {
namespace ffi {
/// Used primarily for mpi ranks that can be "null" (negative)
inline std::span<utility::NonNegativeInteger<int>> NonNegativeIntSpan(int *addresses,
                                                                         size_t count) {
    static_assert(sizeof(utility::NonNegativeInteger<int>) == sizeof(int), "Incompatible types");

    return std::span<utility::NonNegativeInteger<int>>(
        reinterpret_cast<utility::NonNegativeInteger<int> *>(addresses), count);
}

/// Used primarily for mpi ranks that can be "null" (negative)
inline std::span<utility::NonNegativeInteger<int> const> NonNegativeIntSpan(int const *addresses,
                                                                               size_t count) {
    static_assert(sizeof(utility::NonNegativeInteger<int>) == sizeof(int), "Incompatible types");

    return std::span<utility::NonNegativeInteger<int> const>(
        reinterpret_cast<utility::NonNegativeInteger<int> const *>(addresses), count);
}

inline std::span<local_index_t> LocalAddressSpan(eap_local_t *addresses, size_t count) {
    static_assert(sizeof(local_index_t) == sizeof(eap_local_t), "Incompatible types");

    return std::span<local_index_t>(reinterpret_cast<local_index_t *>(addresses), count);
}

inline std::span<local_index_t const> LocalAddressSpan(eap_local_t const *addresses,
                                                          size_t count) {
    static_assert(sizeof(local_index_t) == sizeof(eap_local_t), "Incompatible types");

    return std::span<local_index_t const>(reinterpret_cast<local_index_t const *>(addresses),
                                             count);
}

inline std::span<FortranLocalIndex> FortranLocalAddressSpan(eap_local_t *addresses,
                                                               size_t count) {
    static_assert(sizeof(FortranLocalIndex) == sizeof(eap_local_t), "Incompatible types");

    return std::span<FortranLocalIndex>(reinterpret_cast<FortranLocalIndex *>(addresses), count);
}

inline std::span<FortranLocalIndex const> FortranLocalAddressSpan(eap_local_t const *addresses,
                                                                     size_t count) {
    static_assert(sizeof(FortranLocalIndex) == sizeof(eap_local_t), "Incompatible types");

    return std::span<FortranLocalIndex const>(
        reinterpret_cast<FortranLocalIndex const *>(addresses), count);
}

inline std::span<OptionalFortranLocalIndex>
OptionalFortranLocalAddressSpan(eap_local_t *addresses, size_t count) {
    static_assert(sizeof(OptionalFortranLocalIndex) == sizeof(eap_local_t), "Incompatible types");

    return std::span<OptionalFortranLocalIndex>(
        reinterpret_cast<OptionalFortranLocalIndex *>(addresses), count);
}

inline std::span<OptionalFortranLocalIndex const>
OptionalFortranLocalAddressSpan(eap_local_t const *addresses, size_t count) {
    static_assert(sizeof(OptionalFortranLocalIndex) == sizeof(eap_local_t), "Incompatible types");

    return std::span<OptionalFortranLocalIndex const>(
        reinterpret_cast<OptionalFortranLocalIndex const *>(addresses), count);
}

inline std::span<global_index_t> GlobalAddressSpan(eap_global_t *addresses, size_t count) {
    static_assert(sizeof(global_index_t) == sizeof(eap_global_t), "Incompatible types");

    return std::span<global_index_t>(reinterpret_cast<global_index_t *>(addresses), count);
}

inline std::span<global_index_t const> GlobalAddressSpan(eap_global_t const *addresses,
                                                            size_t count) {
    static_assert(sizeof(global_index_t) == sizeof(eap_global_t), "Incompatible types");

    return std::span<global_index_t const>(reinterpret_cast<global_index_t const *>(addresses),
                                              count);
}

inline std::span<FortranGlobalIndex> FortranGlobalAddressSpan(eap_global_t *addresses,
                                                                 size_t count) {
    static_assert(sizeof(FortranGlobalIndex) == sizeof(eap_global_t), "Incompatible types");

    return std::span<FortranGlobalIndex>(reinterpret_cast<FortranGlobalIndex *>(addresses),
                                            count);
}

inline std::span<FortranGlobalIndex const>
FortranGlobalAddressSpan(eap_global_t const *addresses, size_t count) {
    static_assert(sizeof(FortranGlobalIndex) == sizeof(eap_global_t), "Incompatible types");

    return std::span<FortranGlobalIndex const>(
        reinterpret_cast<FortranGlobalIndex const *>(addresses), count);
}

inline std::span<OptionalFortranGlobalIndex>
OptionalFortranGlobalAddressSpan(eap_global_t *addresses, size_t count) {
    static_assert(sizeof(OptionalFortranGlobalIndex) == sizeof(eap_global_t), "Incompatible types");

    return std::span<OptionalFortranGlobalIndex>(
        reinterpret_cast<OptionalFortranGlobalIndex *>(addresses), count);
}

inline std::span<OptionalFortranGlobalIndex const>
OptionalFortranGlobalAddressSpan(eap_global_t const *addresses, size_t count) {
    static_assert(sizeof(OptionalFortranGlobalIndex) == sizeof(eap_global_t), "Incompatible types");

    return std::span<OptionalFortranGlobalIndex const>(
        reinterpret_cast<OptionalFortranGlobalIndex const *>(addresses), count);
}
} // namespace ffi
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_FFI_INTEROP_ADDRESSING_HPP_