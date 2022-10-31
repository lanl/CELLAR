/**
 * @file mesh-ffi-blocks.h
 *
 * @brief Provides foreign interface to eap::mesh::BlockLayout
 * @date 2019-01-08
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_FFI_BLOCKS_H_
#define EAP_MESH_FFI_BLOCKS_H_

#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <abi-extern.h>
#include <abi-fortran_interop.h>

#include <mesh-ffi-handles.h>

EXTERN_C_BEGIN

/**
 * @brief Builds an eap::mesh::BlockLayout
 *
 * @param method the layout method to use.
 * @param extent0 x-dim extent
 * @param extent1 y-dim extent
 * @param extent2 z-dim extent
 * @param layout new layout object
 */
void eap_mesh_block_layout_create(eap_mesh_block_layout_method_t method,
                                  uint64_t extent0,
                                  uint64_t extent1,
                                  uint64_t extent2,
                                  eap_mesh_block_layout_t **layout);

/**
 * @brief Frees the block layout
 *
 * @param layout existing layout to destroy
 */
void eap_mesh_block_layout_free(eap_mesh_block_layout_t *layout);

/**
 * @brief Translates a block address to its 3D coordinates.
 *
 * @param layout The BlockLayout object
 * @param address Linear block address
 * @param coordinate0 x-address of block
 * @param coordinate1 y-address of block
 * @param coordinate2 z-address of block
 */
void eap_mesh_block_layout_get_coordinates(eap_mesh_block_layout_t *layout,
                                           uint64_t address,
                                           uint64_t *coordinate0,
                                           uint64_t *coordinate1,
                                           uint64_t *coordinate2);

void eap_mesh_block_layout_get_address(eap_mesh_block_layout_t *layout,
                                       uint64_t coordinate0,
                                       uint64_t coordinate1,
                                       uint64_t coordinate2,
                                       uint64_t *address);

EXTERN_C_END

#endif // EAP_MESH_FFI_BLOCKS_H_