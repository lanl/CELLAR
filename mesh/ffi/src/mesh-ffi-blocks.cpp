/**
 * @file mesh-ffi-blocks.cpp
 *
 * @brief Implementation of Mesh BlockLayout FFI
 * @date 2019-01-09
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-ffi-blocks.h>

// STL Includes
#include <memory>

// Internal Includes
#include <error-macros.hpp>
#include <mesh-blocks.hpp>
#include <mesh-ffi-interop.hpp>

using eap::mesh::BlockCoordinates;
using eap::mesh::BlockLayout;
using eap::mesh::BlockLayoutMethod;
using eap::mesh::ffi::LayoutFromFFI;
using eap::mesh::ffi::LayoutMethodFromFFI;
using eap::mesh::ffi::LayoutToFFI;

EXTERN_C void eap_mesh_block_layout_create(eap_mesh_block_layout_method_t method,
                                           uint64_t extent0,
                                           uint64_t extent1,
                                           uint64_t extent2,
                                           eap_mesh_block_layout_t **new_layout) {
    EAP_EXTERN_PRE

    auto layout =
        std::make_unique<BlockLayout>(LayoutMethodFromFFI(method), extent0, extent1, extent2);

    *new_layout = LayoutToFFI(layout.release());

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_block_layout_free(eap_mesh_block_layout_t *layout) {
    EAP_EXTERN_PRE
    delete LayoutFromFFI(layout);
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_block_layout_get_coordinates(eap_mesh_block_layout_t *layout,
                                                    uint64_t address,
                                                    uint64_t *coordinate0,
                                                    uint64_t *coordinate1,
                                                    uint64_t *coordinate2) {
    EAP_EXTERN_PRE

    auto const coordinates = LayoutFromFFI(layout)->GetCoordinates(address);

    *coordinate0 = coordinates.x;
    *coordinate1 = coordinates.y;
    *coordinate2 = coordinates.z;

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_block_layout_get_address(eap_mesh_block_layout_t *layout,
                                                uint64_t coordinate0,
                                                uint64_t coordinate1,
                                                uint64_t coordinate2,
                                                uint64_t *address) {
    EAP_EXTERN_PRE

    *address =
        LayoutFromFFI(layout)->GetAddress(BlockCoordinates{coordinate0, coordinate1, coordinate2});

    EAP_EXTERN_POST
}