/**
 * @file comm-reserved_tags.hpp
 *
 * @brief Reserved MPI tags for eap::Comm
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_RESERVED_TAGS_HPP_
#define EAP_COMM_RESERVED_TAGS_HPP_

#include <mpi/datatype.hpp>

namespace eap {
namespace comm {
constexpr mpi::tag_t BUILD_GLOBAL_TAG = 1000;
constexpr mpi::tag_t TOKEN_GS_TAG = 1001;
constexpr mpi::tag_t SOME_TO_SOME_TAG = 1002;
constexpr mpi::tag_t MOVE_TAG = 1003;
} // namespace comm
} // namespace eap

#endif // EAP_COMM_RESERVED_TAGS_HPP_