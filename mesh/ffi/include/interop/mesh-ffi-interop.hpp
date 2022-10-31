/**
 * @file mesh-ffi-interop.hpp
 *
 * @brief Provides interfaces for going back-and-forth between eap-mesh-ffi and eap::mesh.
 * @date 2019-01-08
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 *
 */

// STL Includes
#include <chrono>

// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <mesh-blocks.hpp>
#include <mesh-cells.hpp>
#include <mesh-dzn.hpp>
#include <mesh-ffi-handles.h>
#include <mesh-info.hpp>
#include <mesh-levels.hpp>

namespace eap {
namespace mesh {
namespace ffi {
inline eap_mesh_block_layout_t *LayoutToFFI(BlockLayout *layout) {
    return reinterpret_cast<eap_mesh_block_layout_t *>(layout);
}

inline BlockLayout *LayoutFromFFI(eap_mesh_block_layout_t *registry) {
    return reinterpret_cast<BlockLayout *>(registry);
}

inline eap_mesh_block_layout_method_t LayoutMethodToFFI(BlockLayoutMethod method) {
    return static_cast<eap_mesh_block_layout_method_t>(method);
}

inline BlockLayoutMethod LayoutMethodFromFFI(eap_mesh_block_layout_method_t method) {
    return static_cast<BlockLayoutMethod>(method);
}

INTEROP_IMPL(mesh, cpp_cells, Cells)
INTEROP_IMPL(mesh, cpp_levels, Levels)
INTEROP_IMPL(mesh, dzn, DefinedZoneResolutionInfo)

INTEROP_IMPL(mesh, cpp_sim_info, SimulationInfo)

INTEROP_IMPL(mesh, cpp_mesh_info, MeshInfo)

} // namespace ffi
} // namespace mesh
} // namespace eap