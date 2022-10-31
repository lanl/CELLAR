/**
 * @file mesh-ffi-info.cpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Implementation of Mesh Info FFI
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-ffi-info.h>

// STL Includes
#include <memory>

// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <error-macros.hpp>

// Local Includes
#include <mesh-ffi-interop.hpp>
#include <mesh-info.hpp>
#include <mesh-types.hpp>

using eap::mesh::MeshInfo;
using eap::mesh::SimulationInfo;
using eap::mesh::ffi::MeshInfoToFFI;
using eap::mesh::ffi::SimulationInfoFromFFI;
using eap::mesh::ffi::SimulationInfoToFFI;

FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(mesh, cpp_sim_info, SimulationInfo)
FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(mesh, cpp_mesh_info, MeshInfo)

EXTERN_C_BEGIN

// Note that the constness of the values of SimulationInfo and MeshInfo is implicitly cast away in
// the FFI. This is unfortunately necessary, since Fortran doesn't have a mechanism to declare
// pointers to const data.

// SimulationInfo
void eap_mesh_cpp_sim_info_create(const uint64_t *num_zones,
                                  const uint32_t num_velocity_components,
                                  const bool cylindrical,
                                  const bool spherical,
                                  const bool use_geometry_factor,
                                  eap_mesh_cpp_sim_info_t **obj) {
    EAP_EXTERN_PRE
    auto simulation_info = std::make_unique<SimulationInfo>(
        Kokkos::Array<uint64_t, 3>{num_zones[0], num_zones[1], num_zones[2]},
        num_velocity_components,
        cylindrical,
        spherical,
        use_geometry_factor);

    *obj = SimulationInfoToFFI(simulation_info.release());

    EAP_EXTERN_POST
}

FC_INTEROP_DELETE_C_IMPL(mesh, cpp_sim_info)

FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, num_dim, uint32_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, num_velocity_components, uint32_t)
FC_INTEROP_WRAP_ARRAY_C_IMPL(mesh, cpp_sim_info, num_zones, uint64_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh,
                              cpp_sim_info,
                              geometry_dimension_flag,
                              eap_mesh_geometry_dimension_flag_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, use_geometry_factor, bool)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, cylindrical, bool)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, spherical, bool)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, geometry_type, eap_mesh_geometry_type_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_sim_info, geometry_factor, double)

// MeshInfo
void eap_mesh_cpp_mesh_info_create(const eap_mesh_cpp_sim_info_t **simulation_info,
                                   const double *lower_bounds,
                                   const bool non_square_cells_allowed,
                                   const double *cell_size,
                                   const bool *periodic_boundary,
                                   const bool *outflow_boundary,
                                   eap_mesh_cpp_mesh_info_t **obj) {
    EAP_EXTERN_PRE

    if (!Kokkos::is_initialized()) Kokkos::initialize();

    // I wonder if there is a better way to do this. Assume outflow_boundary is given in
    // COLUMN-MAJOR form (since it's coming from Fortran)
    eap::mesh::MeshView<bool[2][3]> outflow_boundary_view("outflow_boundary");
    Kokkos::parallel_for(
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {2, 3}),
        KOKKOS_LAMBDA(const int i, const int j) {
            outflow_boundary_view(i, j) = outflow_boundary[2 * j + i];
            // printf("C: outflow(%i,%i) = %s\n", i, j, outflow_boundary_view(i, j)
            // ? "T" : "F");
        });

    auto mesh_info = std::make_unique<MeshInfo>(
        *SimulationInfoFromFFI(*simulation_info),
        Kokkos::Array<double, 3>{lower_bounds[0], lower_bounds[1], lower_bounds[2]},
        non_square_cells_allowed,
        Kokkos::Array<double, 3>{cell_size[0], cell_size[1], cell_size[2]},
        Kokkos::Array<bool, 3>{periodic_boundary[0], periodic_boundary[1], periodic_boundary[2]},
        outflow_boundary_view);

    *obj = MeshInfoToFFI(mesh_info.release());

    EAP_EXTERN_POST
}

FC_INTEROP_DELETE_C_IMPL(mesh, cpp_mesh_info)

FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_mesh_info, num_cells_per_block, uint32_t)
FC_INTEROP_WRAP_ARRAY_C_IMPL(mesh, cpp_mesh_info, lower_bounds, double)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_mesh_info, non_square_cells_allowed, bool)
FC_INTEROP_WRAP_ARRAY_C_IMPL(mesh, cpp_mesh_info, cell_size, double)
FC_INTEROP_WRAP_ARRAY_C_IMPL(mesh, cpp_mesh_info, periodic_boundary, bool)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_mesh_info, outflow_boundary, bool)

EXTERN_C_END
