/**
 * @file mesh-ffi-handles.h
 *
 * @brief Exposes opaque handle types for eap::mesh
 * @date 2019-01-08
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 *
 */

#ifndef EAP_MESH_FFI_BASE_HANDLES_H_
#define EAP_MESH_FFI_BASE_HANDLES_H_

// STL Includes
#include <stddef.h>
#include <stdint.h>

// Internal Includes
#include <abi-fortran_interop.h>

/**
 * @brief Opaque handle type for eap::mesh::BlockLayout.
 */
typedef struct eap_mesh_block_layout_t eap_mesh_block_layout_t;

/**
 * @brief Opaque handle type for eap::mesh::Cells
 */
DECLARE_HANDLE_TYPE(mesh, cpp_cells);

/**
 * @brief Opaque handle type for eap::mesh::Levels
 */
DECLARE_HANDLE_TYPE(mesh, cpp_levels);

/**
 * @brief Opaque handle type for eap::mesh::DefinedZoneResolutionInfo
 */
DECLARE_HANDLE_TYPE(mesh, dzn);

/*
 * @brief Opaque handle type for eap::mesh::SimulationInfo
 */
DECLARE_HANDLE_TYPE(mesh, cpp_sim_info);

/**
 * @brief Opaque handle type for eap::mesh::MeshInfo
 */
DECLARE_HANDLE_TYPE(mesh, cpp_mesh_info);

/**
 * @brief Method to layout blocks
 */
typedef enum _eap_mesh_block_layout_method_t {
    /**
     * @brief Blocks are laid out first in the first-dimension ascending, then second-dimension,
     * then third-dimension.
     */
    eap_mesh_block_layout_method_column_major = 1,
    /**
     * @brief UNIMPLEMENTED
     */
    eap_mesh_block_layout_method_hilbert = 2,
    /**
     * @brief Blocks are laid out first-dimension ascending in even number rows and first-dimension
     * descending in odd number rows, and in a typical manner for the third-dimension.
     */
    eap_mesh_block_layout_method_column_major_zig_zag = 3,
    /**
     * @brief Blocks are laid out first in the first-dimension ascending, then third-dimension, then
     * second-dimension.
     */
    eap_mesh_block_layout_method_column_major_reverse = 4,
} eap_mesh_block_layout_method_t;

/**
 * @brief The geometry type used in the simulation
 */
typedef enum _eap_mesh_geometry_type_t {
    /**
     * @brief This is a simple rectangular geometry, valid in 1, 2, or 3 dimensions.
     */
    eap_mesh_geometry_type_slab = 1,

    /**
     * @brief Cylindrical geometry consists of infinitely long cylindrical shells in 1-D or stacked
     * cylindrical shells in 2-D, in both cases there is axi-symmetry around the axis of the
     * cylinder.
     */
    eap_mesh_geometry_type_cylindrical = 2,

    /**
     * @brief Spherical geometry is only valid in 1-D and it denotes spherical symmetry, i.e. the
     * mesh consists of concentric spherical shells
     */
    eap_mesh_geometry_type_spherical = 3
} eap_mesh_geometry_type_t;

/**
 * @brief The geometry dimension flag is a combination of the geometry type (slab, cylindrical,
 * spherical) and the dimension (1, 2, 3) of the problem.
 */
typedef enum _eap_mesh_geometry_dimension_flag_t {
    eap_mesh_geometry_dimension_flag_slab_1d = 1,
    eap_mesh_geometry_dimension_flag_cylindrical_1d = 2,
    eap_mesh_geometry_dimension_flag_spherical_1d = 3,
    eap_mesh_geometry_dimension_flag_slab_2d = 4,
    eap_mesh_geometry_dimension_flag_cylindrical_2d = 5,
    eap_mesh_geometry_dimension_flag_slab_3d = 6
} eap_mesh_geometry_dimension_flag_t;

#endif // EAP_MESH_FFI_BASE_HANDLES_H_