/**
 * @file mesh-ffi-cells.cpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Implementation of Mesh Cells FFI
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-ffi-cells.h>

// STL Includes
#include <memory>

// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <comm-ffi-interop.hpp>
#include <error-macros.hpp>
#include <mesh-cells.hpp>

// Local Includes
#include <mesh-ffi-interop.hpp>

using eap::comm::ffi::TokenBuilderFromFFI;
using eap::mesh::Cells;
using eap::mesh::ffi::CellsFromFFI;
using eap::mesh::ffi::CellsToFFI;

FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(mesh, cpp_cells, Cells)

EXTERN_C_BEGIN

void eap_mesh_cpp_cells_create(comm_token_builder_t const *token_builder,
                               eap_mesh_cpp_cells_t **newobj) {
    EAP_EXTERN_PRE

    auto cells = std::make_unique<Cells>(*TokenBuilderFromFFI(token_builder));

    *newobj = CellsToFFI(cells.release());

    EAP_EXTERN_POST
}

FC_INTEROP_DELETE_C_IMPL(mesh, cpp_cells)

FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_cells, num_global_cells, int64_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_cells, num_local_cells, int32_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_cells, max_num_local_cells, int32_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_cells, num_local_cells_with_clones, int32_t)
FC_INTEROP_WRAP_SCALAR_C_IMPL(mesh, cpp_cells, num_local_reserve_cells, int32_t)
FC_INTEROP_WRAP_1D_VIEW_C_IMPL(mesh, cpp_cells, all_num_local_cells, int32_t)
FC_INTEROP_WRAP_1D_VIEW_C_IMPL(mesh, cpp_cells, global_base_address, int64_t)
FC_INTEROP_WRAP_1D_VIEW_C_IMPL(mesh, cpp_cells, cell_address, int64_t)
FC_INTEROP_WRAP_1D_VIEW_C_IMPL(mesh, cpp_cells, cell_active, bool)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_cells, cell_center, double)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_cells, cell_position, double)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_cells, cell_half, double)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_cells, cell_half_lo, double)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(mesh, cpp_cells, cell_half_hi, double)
FC_INTEROP_WRAP_1D_VIEW_C_IMPL(mesh, cpp_cells, cell_volume, double)

bool eap_mesh_cpp_cells_cell_arrays_allocated(const eap_mesh_cpp_cells_t *obj) {
    EAP_EXTERN_PRE
    return CellsFromFFI(obj)->Cell_arrays_allocated();
    EAP_EXTERN_POST
}

bool eap_mesh_cpp_cells_global_arrays_allocated(const eap_mesh_cpp_cells_t *obj) {
    EAP_EXTERN_PRE
    return CellsFromFFI(obj)->Global_arrays_allocated();
    EAP_EXTERN_POST
}

void eap_mesh_cpp_cells_set_num_local_cells(eap_mesh_cpp_cells_t *obj, int32_t num_local_cells) {
    EAP_EXTERN_PRE
    CellsFromFFI(obj)->SetNumLocalCells(num_local_cells);
    EAP_EXTERN_POST
}

void eap_mesh_cpp_cells_update_global_base(eap_mesh_cpp_cells_t *obj) {
    EAP_EXTERN_PRE
    CellsFromFFI(obj)->UpdateGlobalBase();
    EAP_EXTERN_POST
}

void eap_mesh_cpp_cells_prepare_global_base(eap_mesh_cpp_cells_t *obj, int32_t num_local_cells) {
    EAP_EXTERN_PRE
    CellsFromFFI(obj)->PrepareGlobalBase(num_local_cells);
    EAP_EXTERN_POST
}

void eap_mesh_cpp_cells_check_global_base(eap_mesh_cpp_cells_t *obj) {
    EAP_EXTERN_PRE
    CellsFromFFI(obj)->CheckGlobalBase();
    EAP_EXTERN_POST
}

void eap_mesh_cpp_cells_resize_cell_arrays(eap_mesh_cpp_cells_t *obj,
                                           const int *size,
                                           const int *num_dims) {
    EAP_EXTERN_PRE
    CellsFromFFI(obj)->ResizeCellArrays(*size, *num_dims);
    EAP_EXTERN_POST
}

int32_t eap_mesh_cpp_cells_to_local(eap_mesh_cpp_cells_t *obj, const int64_t *global_index) {
    EAP_EXTERN_PRE
    return CellsFromFFI(obj)->ToLocal(*global_index);
    EAP_EXTERN_POST
}

int64_t eap_mesh_cpp_cells_to_global(eap_mesh_cpp_cells_t *obj, const int32_t *local_index) {
    EAP_EXTERN_PRE
    return CellsFromFFI(obj)->ToGlobal(*local_index);
    EAP_EXTERN_POST
}

EXTERN_C_END
