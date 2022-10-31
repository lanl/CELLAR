/**
 * @file mesh-log.cpp
 *
 * @brief Implementation of mesh log
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#include "mesh-internal-log.hpp"

#include <cstdlib>
#include <iostream>
#include <mpi/comm.hpp>

std::ostream &eap::mesh::internal::mesh_err() { return std::cerr << "Mesh: "; }

[[noreturn]] void eap::mesh::internal::unimplemented(char const *description) noexcept {
    mesh_err() << "Code currently unimplemented: " << description << std::endl;
    mpi::Comm::world().abort(EXIT_FAILURE);
}