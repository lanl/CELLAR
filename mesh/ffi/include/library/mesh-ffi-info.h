/**
 * @file mesh-ffi-info.h
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Header file for Mesh Info FFI
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019 Triad National Security, LLC
 */

#ifndef MESH_FFI_INFO_H_
#define MESH_FFI_INFO_H_

// Internal Includes
#include <abi-extern.h>
#include <abi-fortran_interop.h>

// Local includes
#include <mesh-ffi-handles.h>

EXTERN_C_BEGIN

// SimulationInfo
void eap_mesh_cpp_sim_info_create(const uint64_t *num_zones,
                                  const uint32_t num_velocity_components,
                                  const bool cylindrical,
                                  const bool spherical,
                                  const bool use_geometry_factor,
                                  eap_mesh_cpp_sim_info_t **obj);

FC_INTEROP_DELETE_C_DECL(mesh, cpp_sim_info);

FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, num_dim, uint32_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, num_velocity_components, uint32_t);
FC_INTEROP_WRAP_ARRAY_C_DECL(mesh, cpp_sim_info, num_zones, uint64_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh,
                              cpp_sim_info,
                              geometry_dimension_flag,
                              eap_mesh_geometry_dimension_flag_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, use_geometry_factor, bool);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, cylindrical, bool);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, spherical, bool);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, geometry_type, eap_mesh_geometry_type_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_sim_info, geometry_factor, double);

// MeshInfo
void eap_mesh_cpp_mesh_info_create(const eap_mesh_cpp_sim_info_t **simulation_info,
                                   const double *lower_bounds,
                                   const bool non_square_cells_allowed,
                                   const double *cell_size,
                                   const bool *periodic_boundary,
                                   const bool *outflow_boundary,
                                   eap_mesh_cpp_mesh_info_t **obj);

FC_INTEROP_DELETE_C_DECL(mesh, cpp_mesh_info);

FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_mesh_info, num_cells_per_block, uint32_t);
FC_INTEROP_WRAP_ARRAY_C_DECL(mesh, cpp_mesh_info, lower_bounds, double);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_mesh_info, non_square_cells_allowed, bool);
FC_INTEROP_WRAP_ARRAY_C_DECL(mesh, cpp_mesh_info, cell_size, double);
FC_INTEROP_WRAP_ARRAY_C_DECL(mesh, cpp_mesh_info, periodic_boundary, bool);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_mesh_info, outflow_boundary, bool);

EXTERN_C_END

#endif // MESH_FFI_INFO_H_