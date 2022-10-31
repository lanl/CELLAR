/**
 * @brief mesh.hpp defines data types and routines for creating and using EAP-style Mesh objects.
 *
 * @file mesh.hpp
 *
 * @date 2018-08-21
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_MESH_HPP_
#define EAP_MESH_MESH_HPP_

// STL Includes
#include <array>
#include <cstdint>
#include <initializer_list>
#include <numeric>

// Third Party Includes
#include <mpi/comm.hpp>

// Local Includes
#include "mesh-blocks.hpp"
#include "mesh-cells.hpp"
#include "mesh-types.hpp"

/**
 * @mainpage EAP Mesh (eap-mesh)
 *
 * # Introduction
 * EAP Mesh is a multi-dimensional Mesh implementation designed for good cache-locality. It
 * describes a layout of cells with support for addressing neighbors, clone cells, and block-based
 * layouts.
 *
 * ## Usage
 * TODO
 */

/**
 * @brief The eap (Eulerian Applications Project) namespace contains routines and types implementing
 * a 3D mesh. 
 */
namespace eap {

/**
 * @brief The mesh namespace contains data types and routines for working with a specific style of
 * 1D, 2D, or 3D mesh that supports EAP.
 */
namespace mesh {

// Forward declarations
class Mesh;

class MeshBuilder {
    mpi::Comm comm_;
    rank_t num_extents_ = 0;
    std::array<global_index_t, 3> global_extents_{0, 0, 0};
    std::vector<double> rank_weights_;

  public:
    static MeshBuilder from_comm(mpi::Comm *comm);

    rank_t block_size() const { return (1 << num_extents_); }
    global_index_t num_global_blocks() const { return num_global_cells() / block_size(); }
    global_index_t num_global_cells() const {
        return global_extents_[0] * global_extents_[1] * global_extents_[2];
    }

    std::array<global_index_t, 3> global_extents() const { return global_extents_; }
    std::array<global_index_t, 3> global_block_extents() const {
        std::array<global_index_t, 3> block_extents{0, 0, 0};
        for (auto i = 0; i < 3; i++) {
            block_extents[i] = global_extents_[i] / 2;
        }
        return block_extents;
    }

    void set_rank_weights(std::vector<double> &&weights);

    void set_global_extents(std::initializer_list<global_index_t> list);
    void set_global_extents(global_index_t x) { set_global_extents({x}); }
    void set_global_extents(global_index_t x, global_index_t y) { set_global_extents({x, y}); }
    void set_global_extents(global_index_t x, global_index_t y, global_index_t z) {
        set_global_extents({x, y, z});
    }

    Mesh build();
};

/**
 * @brief A distributed mesh supporting refinement.
 *
 */
class Mesh {
    mpi::Comm comm_;
    // std::uint8_t num_extents_ = 0;
    std::array<local_index_t, 3> local_extents_{0, 0, 0};

  public:
    static Mesh from_global_2d_dimensions(mpi::Comm *comm, global_index_t x, global_index_t y);

    local_index_t local_extent_0() const { return local_extents_[0]; }
    local_index_t local_extent_1() const { return local_extents_[1]; }
    local_index_t local_extent_2() const { return local_extents_[2]; }
};

} // namespace mesh
} // namespace eap

#endif // EAP_MESH_MESH_HPP_