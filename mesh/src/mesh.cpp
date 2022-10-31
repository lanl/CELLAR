/**
 * @file mesh.cpp
 *
 * @brief Implementation of Mesh class
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#include <mesh.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "mesh-internal-log.hpp"

using namespace eap::mesh;

using internal::mesh_err;

using std::uint32_t;

MeshBuilder MeshBuilder::from_comm(mpi::Comm *comm) {
    MeshBuilder builder;
    builder.comm_ = *comm;
    return builder;
}

void MeshBuilder::set_rank_weights(std::vector<double> &&weights) {
    if (weights.size() != (size_t)comm_.size()) {
        mesh_err() << "Rank weights array must be the same size as the Comm size" << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    rank_weights_ = std::move(weights);
}

void MeshBuilder::set_global_extents(std::initializer_list<global_index_t> extents_list) {
    auto const num_extents = extents_list.size();

    if (num_extents == 0) {
        mesh_err() << "0 dimensions not supported." << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    if (num_extents > 3) {
        mesh_err() << "Max of 3 dimensions supported. " << num_extents << " dimensions passed."
                   << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    if (std::any_of(
            extents_list.begin(), extents_list.end(), [](auto extent) { return extent == 0; })) {
        mesh_err() << "Extents of 0 are not supported." << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    if (std::any_of(extents_list.begin(), extents_list.end(), [](auto extent) {
            return (extent % 2) != 0;
        })) {
        mesh_err() << "Extents must be divisible by 2." << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    num_extents_ = num_extents;
    auto it = std::copy(extents_list.begin(), extents_list.end(), global_extents_.begin());
    std::fill(it, global_extents_.end(), 1);

    assert((num_global_cells() % block_size()) == 0);
}

Mesh MeshBuilder::build() {
    if (global_extents_.end() == std::find(global_extents_.begin(), global_extents_.end(), 0)) {
        mesh_err()
            << "MeshBuilder::set_global_extents must be called prior to calling MeshBuilder::build"
            << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    /* TODO complete this implementation
    auto const num_global_cells = this->num_global_cells();
    auto const num_global_blocks = this->num_global_blocks();

    std::vector<uint32_t> num_blocks_per_rank(comm_.size());
    if (rank_weights_.empty()) {
        auto const local_blocks = num_global_blocks / comm_.size();
        auto const remainder = (num_global_blocks % comm_.size());
        // distribute the blocks as evenly as possible.
        for (mpi::rank_t rank = 0; rank < comm_.size(); rank++) {
            num_blocks_per_rank[rank] = local_blocks + (rank < (mpi::rank_t)remainder);
        }
    } else {
        mesh_err() << "rank_weights UNIMPLEMENTED!" << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    auto const block_start = std::accumulate(num_blocks_per_rank.begin(),
                                             num_blocks_per_rank.begin() + comm_.rank(),
                                             0ul,
                                             std::plus<uint32_t>());

    auto const num_local_blocks = num_blocks_per_rank[comm_.rank()];

    auto const block_end = block_start + num_local_blocks;

    auto const num_local_cells = num_local_blocks * block_size();

    assert(!num_blocks_per_rank.empty());

    auto const max_num_blocks =
        *std::max_element(num_blocks_per_rank.begin(), num_blocks_per_rank.end());

    auto const max_num_cells = max_num_blocks * block_size();

    auto const global_block_extents = this->global_block_extents();
    for (auto block = block_start; block != block_end; block++) {
        auto const block_i = block % global_block_extents[0];
    }
    */

    return Mesh();
}

Mesh Mesh::from_global_2d_dimensions(mpi::Comm *comm, global_index_t x, global_index_t y) {
    auto builder = MeshBuilder::from_comm(comm);
    builder.set_global_extents(x, y);
    return builder.build();
}