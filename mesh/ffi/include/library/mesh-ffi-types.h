/**
 * @file mesh-ffi-types.h
 *
 * @brief Mesh FFI simple types
 * @date 2019-08-30
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_FFI_TYPES_H_
#define EAP_MESH_FFI_TYPES_H_

// STL Includes
#include <stdint.h>

// Internal Includes
#include <utility-ffi-addressing.h>

typedef struct eap_mesh_loc_val {
    eap_local_t loc;
    int32_t val;
} eap_mesh_loc_val;

#endif // EAP_MESH_FFI_TYPES_H_