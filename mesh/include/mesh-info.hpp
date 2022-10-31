/**
 * @brief Defines several classes that hold information about the mesh.
 *
 * @file mesh-info.hpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @date 2019-03-28
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_INFO_HPP_
#define EAP_MESH_INFO_HPP_

// STL Includes
#include <array>

// Local Includes
#include "mesh-types.hpp"

namespace eap {
namespace mesh {

// these magic numbers are assumed in xRage
enum class GeometryType { Slab = 1, Cylindrical = 2, Spherical = 3 };

// these magic numbers are assumed in xRage
enum class GeometryDimensionFlag {
    Slab_1D = 1,
    Cylindrical_1D = 2,
    Spherical_1D = 3,
    Slab_2D = 4,
    Cylindrical_2D = 5,
    Slab_3D = 6
};

class SimulationInfo {
  public:
    SimulationInfo(const Kokkos::Array<global_index_t, 3> num_zones,
                   const rank_t num_velocity_components,
                   const bool cylindrical,
                   const bool spherical,
                   const bool use_geometry_factor);

    // Accessors (read-only), need to return reference so we can take the address of it in the FFI
    const auto &num_dim() const { return num_dim_; }
    const auto &num_velocity_components() const { return num_velocity_components_; }
    const auto &num_zones() const { return num_zones_; }
    const auto &geometry_dimension_flag() const { return geometry_dimension_flag_; }
    const auto &use_geometry_factor() const { return use_geometry_factor_; }
    const auto &geometry_factor() const { return geometry_factor_; }
    const auto &cylindrical() const { return cylindrical_; }
    const auto &spherical() const { return spherical_; }
    const auto &geometry_type() const { return geometry_type_; }

    // Convenience accessors for each dimension (read-only)
    auto num_zones_x() const { return num_zones_[0]; }
    auto num_zones_y() const { return num_zones_[1]; }
    auto num_zones_z() const { return num_zones_[2]; }

  private:
    /**
     * @brief Number of dimensions in the problem
     */
    const uint32_t num_dim_;

    /**
     * @brief Number of components of velocity vectors
     */
    const uint32_t num_velocity_components_;

    /**
     * @brief Number of zones in each dimension
     */
    const Kokkos::Array<global_index_t, 3> num_zones_;

    /**
     * @brief Enum that contains the number of dimensions and the geometry.
     */
    const GeometryDimensionFlag geometry_dimension_flag_;

    /**
     * @brief If true, geometry correction factor (\ref geometry_factor_) is added in printed output
     * for cylindrical and spherical geometries.
     */
    const bool use_geometry_factor_;

    /**
     * @brief Geometry correction factor that is used if \ref use_geometry_factor_ is true. It's pi
     * in cylindrical coordinates and 4/3*pi in spherical coordinates.
     */
    const double geometry_factor_;

    /**
     * @brief True if cylindrical geometry is being used.
     */
    const bool cylindrical_;

    /**
     * @brief True if spherical geometry is being used.
     */
    const bool spherical_;

    /**
     * @brief Enum containing only the geometry type and not the number of dimensions.
     */
    const GeometryType geometry_type_;
};

/**
 * @brief Contains the lower bounds of the grid (mesh).
 */
class MeshInfo {
  public:
    MeshInfo(const SimulationInfo &simulation_info,
             const Kokkos::Array<double, 3> lower_bounds,
             const bool non_square_cells_allowed,
             const Kokkos::Array<double, 3> cell_size,
             const Kokkos::Array<bool, 3> periodic_boundary,
             const MeshView<const bool[2][3]> outflow_boundary);

    // Accessors (read-only), need to return reference so we can take the address of it in the FFI
    const auto &num_cells_per_block() const { return num_cells_per_block_; }
    const auto &lower_bounds() const { return lower_bounds_; }
    const auto &cell_size() const { return cell_size_; }
    const auto &non_square_cells_allowed() const { return non_square_cells_allowed_; }
    const auto &periodic_boundary() const { return periodic_boundary_; }
    const auto &outflow_boundary() const { return outflow_boundary_; }

    // Convenience accessors for each dimension (read-only)
    auto lower_bound_x() const { return lower_bounds_[0]; }
    auto lower_bound_y() const { return lower_bounds_[1]; }
    auto lower_bound_z() const { return lower_bounds_[2]; }
    auto cell_size_x() const { return cell_size_[0]; }
    auto cell_size_y() const { return cell_size_[1]; }
    auto cell_size_z() const { return cell_size_[2]; }
    auto periodic_boundary_x() const { return periodic_boundary_[0]; }
    auto periodic_boundary_y() const { return periodic_boundary_[1]; }
    auto periodic_boundary_z() const { return periodic_boundary_[2]; }

  private:
    /**
     * @brief Number of cells per block, this is 2^D where D is the dimension of the simulation
     */
    uint32_t num_cells_per_block_;

    /**
     * @brief Coordinates of the lower bounds of the grid (mesh) in each dimension
     */
    const Kokkos::Array<double, 3> lower_bounds_;

    /**
     * @brief True if non-square cells are allowed.
     */
    const bool non_square_cells_allowed_;

    /**
     * @brief Size of a level 1 cell in each dimension
     */
    const Kokkos::Array<double, 3> cell_size_;

    /**
     * @brief True if the boundary conditions are periodic in the given dimension
     */
    const Kokkos::Array<bool, 3> periodic_boundary_;

    /**
     * @brief Specifies if the boundary condition is the outflow condition on a given boundary.
     *
     * This is a 2x3 array, where the first dimension represents the side of boundary in a given
     * direction. The first index is the low side and the second index is the high side. The second
     * dimension specifies the x-, y-, or z-direction. So (1,2) would be the high side boundary in
     * the z-direction (in C++ using zero-based indexing).
     */
    const MeshView<const bool[2][3]> outflow_boundary_;
};

} // namespace mesh
} // namespace eap

#endif // EAP_MESH_INFO_HPP_