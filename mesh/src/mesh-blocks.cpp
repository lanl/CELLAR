/**
 * @file mesh-blocks.cpp
 *
 * @brief Implementation of BlockLayout
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Matching header
#include <mesh-blocks.hpp>

// Local includes
#include "mesh-internal-log.hpp"

using namespace eap::mesh;

BlockLayout::BlockLayout(BlockLayoutMethod const method,
                         global_index_t const extent0,
                         global_index_t const extent1,
                         global_index_t const extent2)
    : method(method), extents_{extent0, extent1, extent2} {
    auto const it = std::find_if(
        extents_.cbegin(), extents_.cend(), [](auto const extent) { return extent <= 0; });

    if (it != extents_.end()) {
        throw InvalidBlockExtentsError(*it);
    }

    if (method == BlockLayoutMethod::Hilbert) {
        this->method = BlockLayoutMethod::ColumnMajor;
        internal::mesh_err()
            << "BlockLayoutMethod::Hilbert (block_method = 2) is no longer supported. "
               "Falling back to BlockLayoutMethod::ColumnMajor (block_method = 1)"
            << std::endl;
    }
}
