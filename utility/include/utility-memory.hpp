/**
 * @file utility-memory.hpp
 *
 * @brief Types and tools for allocating memory in EAP
 * @date 2019-10-25
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_MEMORY_HPP_
#define EAP_UTILITY_MEMORY_HPP_

// Third Party Includes
#include <Kokkos_Core.hpp>

namespace eap {
#ifdef KOKKOS_ENABLE_CUDA
using HostMemorySpace = Kokkos::CudaUVMSpace;
#else
using HostMemorySpace = Kokkos::HostSpace;
#endif
} // namespace eap

#endif // EAP_UTILITY_MEMORY_HPP_