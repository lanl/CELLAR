/**
 * @file main.cpp
 *
 * @brief Benchmarks for eap-utility
 * @date 2019-03-01
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <benchmark/benchmark.h>

int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    Kokkos::initialize(argc, argv);

    benchmark::RunSpecifiedBenchmarks();

    Kokkos::finalize();
    return 0;
}