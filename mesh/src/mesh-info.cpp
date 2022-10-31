/**
 * @file mesh-info.cpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Implementation of Mesh-Info classes
 * @date 2019-03-29
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-info.hpp>

// STL Includes
#include <algorithm>
#include <cmath>

// Internal Include
#include <error.hpp>
#include <utility-kokkos.hpp>

using namespace eap::mesh;
using namespace eap::utility::kokkos;

using eap::global_index_t;

namespace { // anonymous namespace so this function can only be used here

rank_t GetNumDimFromGridSize(const Kokkos::Array<global_index_t, 3> grid_size) {
    rank_t num_dim = 1;
    if (grid_size[1] > 1) num_dim = 2;
    if (grid_size[2] > 1) num_dim = 3;
    return num_dim;
}
GeometryDimensionFlag
GetGeometryDimensionFlag(const rank_t num_dim, const bool cylindrical, const bool spherical) {
    EE_PRELUDE
    if (num_dim == 3) {
        return GeometryDimensionFlag::Slab_3D;
    } else if (num_dim == 2) {
        if (cylindrical)
            return GeometryDimensionFlag::Cylindrical_2D;
        else
            return GeometryDimensionFlag::Slab_2D;
    } else if (num_dim == 1) {
        if (spherical)
            return GeometryDimensionFlag::Spherical_1D;
        else if (cylindrical)
            return GeometryDimensionFlag::Cylindrical_1D;
        else
            return GeometryDimensionFlag::Slab_1D;
    } else {
        EE_RAISE(num_dim << " is an invalid number of dimensions");
    }
}

double
GetGeometryFactor(const bool use_geometry_factor, const bool cylindrical, const bool spherical) {
    double geometry_factor = 1.0;
    if (use_geometry_factor) {
        if (cylindrical) geometry_factor = M_PI;
        if (spherical) geometry_factor = 4.0 / 3.0 * M_PI;
    }
    return geometry_factor;
}

GeometryType GetGeometryType(const bool cylindrical, const bool spherical) {
    GeometryType geometry = GeometryType::Slab;
    if (cylindrical) geometry = GeometryType::Cylindrical;
    if (spherical) geometry = GeometryType::Spherical;
    return geometry;
}

} // anonymous namespace

SimulationInfo::SimulationInfo(const Kokkos::Array<global_index_t, 3> num_zones,
                               const rank_t num_velocity_components,
                               const bool cylindrical,
                               const bool spherical,
                               const bool use_geometry_factor)
    : num_dim_(GetNumDimFromGridSize(num_zones)),
      num_velocity_components_(num_velocity_components == 0 ? num_dim_ : num_velocity_components),
      num_zones_(num_zones),
      geometry_dimension_flag_(GetGeometryDimensionFlag(num_dim_, cylindrical, spherical)),
      use_geometry_factor_(num_dim_ == 3 ? false : use_geometry_factor),
      geometry_factor_(GetGeometryFactor(use_geometry_factor_, cylindrical, spherical)),
      cylindrical_(cylindrical),
      spherical_(spherical),
      geometry_type_(GetGeometryType(cylindrical, spherical)) {
    EE_DIAG_PRE

    EE_ASSERT(num_velocity_components_ >= num_dim_,
              "The number of velocity components must be greater or equal to the number of "
              "dimensions, but the number of velocity components is "
                  << int(num_velocity_components_) << " and the number of dimensions is "
                  << int(num_dim_));

    EE_ASSERT(num_velocity_components_ <= 3,
              "The number of velocity components cannot be larger than 3, but it is "
                  << int(num_velocity_components_));

    EE_ASSERT(!(cylindrical_ && spherical_), "Geometry cannot be both cylindrical and spherical");

    if (num_dim_ == 1) {
        EE_ASSERT(num_zones_x() >= 1,
                  "Grid size in the x-direction must be >= 1, but it is "
                      << global_index_t(num_zones_x()));
        EE_ASSERT_EQ(num_zones_y(),
                     1,
                     "Grid size in y-direction must be 1 in 1D, but it is "
                         << global_index_t(num_zones_y()));
        EE_ASSERT_EQ(num_zones_z(),
                     1,
                     "Grid size in z-direction must be 1 in 1D, but it is "
                         << global_index_t(num_zones_z()));

        if (spherical_) {
            EE_ASSERT_EQ(num_velocity_components_,
                         1,
                         "Number of velocity components must be 1 in spherical geometry in 1D, but "
                         "number of velocity components is "
                             << rank_t(num_velocity_components_));
        }
    } else if (num_dim_ == 2) {
        EE_ASSERT(num_zones_x() >= 1,
                  "Grid size in the x-direction must be >= 1, but it is "
                      << global_index_t(num_zones_x()));
        EE_ASSERT(num_zones_y() >= 1,
                  "Grid size in the y-direction must be >= 1, but it is "
                      << global_index_t(num_zones_y()));
        EE_ASSERT_EQ(num_zones_z(),
                     1,
                     "Grid size in z-direction must be 1 in 2D, but it is "
                         << global_index_t(num_zones_z()));

        EE_ASSERT(!spherical_, "Cannot use spherical geometry in 2D");
    } else if (num_dim_ == 3) {
        EE_ASSERT(num_zones_x() >= 1,
                  "Grid size in the x-direction must be >= 1, but it is "
                      << global_index_t(num_zones_x()));
        EE_ASSERT(num_zones_y() >= 1,
                  "Grid size in the y-direction must be >= 1, but it is "
                      << global_index_t(num_zones_y()));
        EE_ASSERT(num_zones_z() >= 1,
                  "Grid size in the z-direction must be >= 1, but it is "
                      << global_index_t(num_zones_z()));

        EE_ASSERT(!use_geometry_factor_, "Geometry factor must not be used in 3D");
        EE_ASSERT(!cylindrical_, "Cannot use cylindrical geometry in 3D");
        EE_ASSERT(!spherical_, "Cannot use spherical geometry in 3D");
    } else {
        EE_RAISE(num_dim_ << " is an invalid number of dimensions");
    }

    auto minmax = std::minmax({num_zones_[0], num_zones[1], num_zones[2]});
    // this should never happen due to the EE_ASSERTs above, but keep it as good defensive
    // programming
    EE_ASSERT(minmax.first >= 1,
              "The minimum number of zones in each dimension must be at least 1");

    // this could happen and needs to be checked
    EE_ASSERT(minmax.second > 1, "There must be more than one zone in at least one dimension");

    EE_DIAG_POST_MSG("Tried to construct SimulationInfo with the following parameters:"
                     << std::endl
                     << "  num_zones = " << num_zones << std::endl
                     << "  num_velocity_components = " << int(num_velocity_components) << std::endl
                     << "  cylindrical = " << cylindrical << std::endl
                     << "  spherical = " << spherical << std::endl
                     << "  use_geometry_factor = " << use_geometry_factor << std::endl)
}

MeshInfo::MeshInfo(const SimulationInfo &simulation_info,
                   const Kokkos::Array<double, 3> lower_bounds,
                   const bool non_square_cells_allowed,
                   const Kokkos::Array<double, 3> cell_size,
                   const Kokkos::Array<bool, 3> periodic_boundary,
                   const MeshView<const bool[2][3]> outflow_boundary)
    : num_cells_per_block_(1 << simulation_info.num_dim()),
      lower_bounds_(lower_bounds),
      non_square_cells_allowed_(non_square_cells_allowed),
      cell_size_(cell_size),
      periodic_boundary_(periodic_boundary),
      outflow_boundary_(outflow_boundary) {
    EE_DIAG_PRE
    if (simulation_info.num_dim() == 1) {
        EE_ASSERT_EQ(lower_bound_y(),
                     0.0,
                     "Lower bounds of grid in y-direction must be 0.0 in 1D, but it is "
                         << lower_bound_y());
        EE_ASSERT_EQ(lower_bound_z(),
                     0.0,
                     "Lower bounds of grid in z-direction must be 0.0 in 1D, but it is "
                         << lower_bound_z());

        EE_ASSERT(cell_size_x() > 0.0,
                  "Cell size in x-direction must be > 0.0, but it is " << cell_size_x());
        EE_ASSERT_EQ(cell_size_y(),
                     1.0,
                     "Cell size in y-direction must be 1.0 in 1D, but it is " << cell_size_y());
        EE_ASSERT_EQ(cell_size_z(),
                     1.0,
                     "Cell size in z-direction must be 1.0 in 1D, but it is " << cell_size_z());
    } else if (simulation_info.num_dim() == 2) {
        EE_ASSERT_EQ(lower_bound_z(),
                     0.0,
                     "Lower bounds of grid in z-direction must be 0.0 in 1D, but it is "
                         << lower_bound_z());

        EE_ASSERT(cell_size_x() > 0.0,
                  "Cell size in x-direction must be > 0.0, but it is " << cell_size_x());
        EE_ASSERT(cell_size_y() > 0.0,
                  "Cell size in y-direction must be > 0.0, but it is " << cell_size_y());
        EE_ASSERT_EQ(cell_size_z(),
                     1.0,
                     "Cell size in z-direction must be 1.0 in 2D, but it is " << cell_size_z());
    } else if (simulation_info.num_dim() == 3) {
        EE_ASSERT(cell_size_x() > 0.0,
                  "Cell size in x-direction must be > 0.0, but it is " << cell_size_x());
        EE_ASSERT(cell_size_y() > 0.0,
                  "Cell size in y-direction must be > 0.0, but it is " << cell_size_y());
        EE_ASSERT(cell_size_z() > 0.0,
                  "Cell size in z-direction must be > 0.0, but it is " << cell_size_z());
    } else {
        EE_RAISE(int(simulation_info.num_dim()) << " is an invalid number of dimensions");
    }

    if (!non_square_cells_allowed) {
        // cell size must be square
        if (simulation_info.num_dim() >= 2) {
            EE_ASSERT_EQ(cell_size_x(),
                         cell_size_y(),
                         "Only square cells are allowed, but cell size in y-direction is "
                             << cell_size_y() << " while cell size in x-direction is "
                             << cell_size_x());
        }

        if (simulation_info.num_dim() == 3) {
            EE_ASSERT_EQ(cell_size_x(),
                         cell_size_z(),
                         "Only square cells are allowed, but cell size in z-direction is "
                             << cell_size_z() << " while cell size in x-direction is "
                             << cell_size_x());
        }
    }

    EE_ASSERT((outflow_boundary_.extent(0) == 2) && (outflow_boundary_.extent(1) == 3),
              "Boundary condition outflow array has wrong shape: "
                  << outflow_boundary_.extent(0) << "x" << outflow_boundary_.extent(1));

    if (periodic_boundary_x()) {
        EE_ASSERT(!outflow_boundary_(0, 0) && !outflow_boundary_(1, 0),
                  "Boundary condition in x-direction is periodic and outflow");

        EE_ASSERT(!simulation_info.spherical() && !simulation_info.cylindrical(),
                  "Must use slab geometry if the x-boundary condition is periodic");
    }

    if (outflow_boundary_(0, 0)) {
        EE_ASSERT(!simulation_info.spherical() && !simulation_info.cylindrical(),
                  "Must use slab geometry if the x-boundary condition is outflow on the low side");
    }

    if (periodic_boundary_y()) {
        EE_ASSERT(!outflow_boundary_(0, 1) && !outflow_boundary_(1, 1),
                  "Boundary condition in y-direction is periodic and outflow");
    }

    if (periodic_boundary_z()) {
        EE_ASSERT(!outflow_boundary_(0, 2) && !outflow_boundary_(1, 2),
                  "Boundary condition in z-direction is periodic and outflow");
    }

    if (lower_bound_x() < 0.0) {
        EE_ASSERT(!simulation_info.spherical() && !simulation_info.cylindrical(),
                  "Must use slab geometry if the lower bounds in the x-direction are negative");
    }

    EE_DIAG_POST_MSG("Tried to construct MeshInfo with the following parameters:"
                     << std::endl
                     << "  lower_bounds = " << lower_bounds << std::endl
                     << "  non_square_cells_allowed = " << non_square_cells_allowed << std::endl
                     << "  cell_size = " << cell_size << std::endl
                     << "  periodic_boundary = " << periodic_boundary << std::endl
                     << "  outflow_boundary = " << outflow_boundary << std::endl)
}
