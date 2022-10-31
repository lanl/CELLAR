/**
 * @file mesh-constants.hpp
 *
 * @brief Constants used by the EAP Mesh
 * @date 2019-02-14
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_CONSTANTS_HPP_
#define EAP_MESH_CONSTANTS_HPP_

// STL Includes
#include <cstddef>

// Local Includes
#include "mesh-types.hpp"

namespace eap {
namespace mesh {
static constexpr std::size_t MAX_LEVELS = 50;

enum : size_t { X_DIR = 0, Y_DIR = 1, Z_DIR = 2 };

/**
 * @brief The maximum supported spacial dimension (currently 3).
 */
constexpr rank_t MAX_DIM = 3;

static constexpr std::size_t ELEM_MAX = 32;

/**
 * @brief Maximum value for a global_index_t
 */
static constexpr global_index_t MAX_GLOBAL = std::numeric_limits<global_index_t>::max();

/**
 * @brief Maximum value for a local_index_t
 */
static constexpr local_index_t MAX_LOCAL = std::numeric_limits<local_index_t>::max();
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_CONSTANTS_HPP_