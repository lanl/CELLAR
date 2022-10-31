/**
 * @file utility-algorithm.cpp
 *
 * @brief Implementation of common algorithms
 * @date 2019-05-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 *
 */

// Self Include
#include <utility-algorithm.hpp>

void eap::utility::InclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output) {
    InclusiveSumScan(input, output);
}

void eap::utility::InclusiveExclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output) {
    InclusiveExclusiveSumScan(input, output);
}

void eap::utility::ExclusiveSumScanHost(
    Kokkos::View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace> input,
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace> output) {
    ExclusiveSumScan(input, output);
}