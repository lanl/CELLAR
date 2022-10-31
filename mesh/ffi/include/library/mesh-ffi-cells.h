/**
 * @file mesh-ffi-cells.h
 *
 * @brief Header file for Mesh Cells FFI
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_FFI_CELLS_H_
#define EAP_MESH_FFI_CELLS_H_

// Internal Includes
#include <abi-extern.h>
#include <abi-fortran_interop.h>
#include <comm-ffi-types.h>

// Local includes
#include <mesh-ffi-handles.h>

EXTERN_C_BEGIN

void eap_mesh_cpp_cells_create(comm_token_builder_t const *token_builder,
                               eap_mesh_cpp_cells_t **newobj);

FC_INTEROP_DELETE_C_DECL(mesh, cpp_cells);

FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_cells, num_global_cells, int64_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_cells, num_local_cells, int32_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_cells, max_num_local_cells, int32_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_cells, num_local_cells_with_clones, int32_t);
FC_INTEROP_WRAP_SCALAR_C_DECL(mesh, cpp_cells, num_local_reserve_cells, int32_t);
FC_INTEROP_WRAP_1D_VIEW_C_DECL(mesh, cpp_cells, all_num_local_cells, int32_t);
FC_INTEROP_WRAP_1D_VIEW_C_DECL(mesh, cpp_cells, global_base_address, int64_t);
FC_INTEROP_WRAP_1D_VIEW_C_DECL(mesh, cpp_cells, cell_address, int64_t);
FC_INTEROP_WRAP_1D_VIEW_C_DECL(mesh, cpp_cells, cell_active, bool);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_cells, cell_center, double);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_cells, cell_position, double);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_cells, cell_half, double);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_cells, cell_half_lo, double);
FC_INTEROP_WRAP_2D_VIEW_C_DECL(mesh, cpp_cells, cell_half_hi, double);
FC_INTEROP_WRAP_1D_VIEW_C_DECL(mesh, cpp_cells, cell_volume, double);

bool eap_mesh_cpp_cells_cell_arrays_allocated(const eap_mesh_cpp_cells_t *obj);

bool eap_mesh_cpp_cells_global_arrays_allocated(const eap_mesh_cpp_cells_t *obj);

void eap_mesh_cpp_cells_set_num_local_cells(eap_mesh_cpp_cells_t *obj, int32_t num_local_cells);

void eap_mesh_cpp_cells_update_global_base(eap_mesh_cpp_cells_t *obj);

void eap_mesh_cpp_cells_prepare_global_base(eap_mesh_cpp_cells_t *obj, int32_t num_local_cells);

void eap_mesh_cpp_cells_check_global_base(eap_mesh_cpp_cells_t *obj);

void eap_mesh_cpp_cells_resize_cell_arrays(eap_mesh_cpp_cells_t *obj,
                                           const int *size,
                                           const int *num_dims);

int32_t eap_mesh_cpp_cells_to_local(eap_mesh_cpp_cells_t *obj, const int64_t *global_index);

int64_t eap_mesh_cpp_cells_to_global(eap_mesh_cpp_cells_t *obj, const int32_t *local_index);

EXTERN_C_END

#endif // EAP_MESH_FFI_CELLS_H_