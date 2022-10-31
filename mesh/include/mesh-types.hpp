/**
 * @file mesh_types.hpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Contains type definitions used throughout the Mesh infrastructure.
 * @version 0.1
 * @date 2018-11-05
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_TYPES_HPP_
#define EAP_MESH_TYPES_HPP_

// STL Includes
#include <cstdint>
#include <limits>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

// Internal Includes
#include <utility-addressing.hpp>
#include <utility-memory.hpp>

namespace eap {
namespace mesh {

/**
 * @brief Type to hold rank number, which is typically 1, 2, or 3.
 */
using rank_t = std::uint8_t;

/**
 * @brief Standard multi-dimensional array type to use for Mesh data
 *
 * @details
 * We're using Kokkos::View with LayoutLeft to represent multi-dimensional arrays in column-major
 * layout so they can easily be accessed from FORTRAN. The dimension of the array is determined by
 * the data type T. For example, to get a 1-D int array we use T = int*, for a 2-D double array use
 * T = double**, etc.
 *
 * For now, we're enforcing HostSpace. This will probably change in the future (likely move to
 * DualView).
 *
 * @tparam T
 */
template <typename T>
using MeshView = Kokkos::View<T, Kokkos::LayoutLeft, eap::HostMemorySpace>;

/**
 * @brief Standard multi-dimensional array type to use for Mesh data for Unmanaged data
 *
 * @details
 * We're using Kokkos::View with LayoutLeft to represent multi-dimensional arrays in column-major
 * layout so they can easily be accessed from FORTRAN. The dimension of the array is determined by
 * the data type T. For example, to get a 1-D int array we use T = int*, for a 2-D double array use
 * T = double**, etc.
 *
 * For now, we're enforcing HostSpace. This will probably change in the future (likely move to
 * DualView).
 *
 * @tparam T
 */
template <typename T>
using MeshViewUnmanaged =
    Kokkos::View<T, Kokkos::LayoutLeft, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>;

/**
 * @brief MeshDualView type to use for Mesh data
 *
 * @details
 * We're using Kokkos::DualView with LayoutLeft to represent multi-dimensional arrays in
 * column-major layout so they can easily be accessed from FORTRAN. The dimension of the array is
 * determined by the data type T. For example, to get a 1-D int array we use T = int*, for a 2-D
 * double array use T = double**, etc.
 *
 * @tparam T
 */
template <typename T>
using MeshDualView = Kokkos::DualView<T, Kokkos::LayoutLeft>;

template <typename T>
class Coordinates {
  public:
    T x = 0; /**< first-dimension block coordinate */
    T y = 0; /**< second-dimension block coordinate */
    T z = 0; /**< third-dimension block coordinate */

    /**
     * @brief Tests two sets of coordinates for equality
     *
     * @param other
     *  Target for comparison
     * @return true
     *  coordinates are equal
     * @return false
     *  coordinates are inequal
     */
    constexpr bool operator==(Coordinates const &other) const noexcept {
        return pack() == other.pack();
    }

    /**
     * @brief Tests two sets of coordinates for inequality
     *
     * @param other
     *  Target for comparison
     * @return true
     *  coordinates are inequal
     * @return false
     *  coordinates are equal
     */
    constexpr bool operator!=(Coordinates const &other) const noexcept {
        return pack() != other.pack();
    }

  private:
    constexpr auto pack() const noexcept { return std::tie(x, y, z); }
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_TYPES_HPP_