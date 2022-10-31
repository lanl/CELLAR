/**
 * @file mesh-bench-scatter.cpp
 *
 * @brief Benchmarks for ScatterView
 * @date 2019-06-04
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// STL Includes
#include <iostream>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <Kokkos_ScatterView.hpp>
#include <benchmark/benchmark.h>

// Internal Includes
#include <mesh-types.hpp>

namespace eap {
namespace mesh {
namespace bench {
MeshView<local_index_t const *> GetTestCellLevels(local_index_t num_cell, local_index_t num_level) {
    auto cell_level = MeshView<local_index_t *>("cell_levels", num_cell);

    // This is super-synthetic cell_level array
    Kokkos::parallel_for(
        "eap::mesh::bench::GetTestCellLevels::init_cell_level",
        num_cell,
        KOKKOS_LAMBDA(local_index_t const i) {
            local_index_t const i1 = i % (1 << num_level);

            local_index_t current_level = num_level;
            while (current_level > 0 && (i1 < (1ul << (current_level - 1)) - 1))
                current_level--;

            cell_level(i) = current_level;
        });

    return cell_level;
}

template <typename ExecSpace>
void ScatterCount(
    MeshView<local_index_t const *> cell_level,
    Kokkos::View<local_index_t *, Kokkos::LayoutRight, eap::HostMemorySpace> level_counts) {
    // We allocate the ScatterView inside of the benchmark loop since it can't be amortized in
    // our use case.
    auto level_counts_scatter =
        Kokkos::Experimental::create_scatter_view<Kokkos::Experimental::ScatterSum,
                                                  void,
                                                  void,
                                                  local_index_t *,
                                                  Kokkos::LayoutRight>(level_counts);

    // Count the number of cells at each level
    Kokkos::parallel_for(
        "eap::mesh::KidMom::BuildPack::count_levels",
        Kokkos::RangePolicy<ExecSpace>(0, cell_level.extent_int(0)),
        KOKKOS_LAMBDA(int const i) {
            auto level_counts = level_counts_scatter.access();
            level_counts(cell_level(i)) += 1;
        });

    Kokkos::Experimental::contribute(level_counts, level_counts_scatter);
}

/**
 * @brief Baseline test for serial level count with a raw loop.
 */
void SerialRawCount(benchmark::State &state) {
    auto const num_level = 10;
    auto const num_cell = (local_index_t)state.range(0);

    auto const cell_level = GetTestCellLevels(num_cell, num_level);

    MeshView<local_index_t *> level_counts("eap::mesh::Kidmom::BuildPack::level_counts", num_level);

    for (auto _ : state) {
        // Count the number of cells at each level
        for (int i = 0; i < cell_level.extent_int(0); i++) {
            level_counts(cell_level(i))++;
        }
    }
}

BENCHMARK(SerialRawCount)->RangeMultiplier(2)->Range(1 << 7, 1 << 17);

/**
 * @brief Baseline test for serial level count with a Kokkos::parallel_for
 */
void SerialCount(benchmark::State &state) {
    auto const num_level = 10;
    auto const num_cell = (local_index_t)state.range(0);

    auto const cell_level = GetTestCellLevels(num_cell, num_level);

    MeshView<local_index_t *> level_counts("eap::mesh::Kidmom::BuildPack::level_counts", num_level);

    for (auto _ : state) {
        // Count the number of cells at each level
        Kokkos::parallel_for(
            "eap::mesh::KidMom::BuildPack::count_levels",
            Kokkos::RangePolicy<Kokkos::Serial>(0, cell_level.extent_int(0)),
            KOKKOS_LAMBDA(int const i) { level_counts(cell_level(i))++; });
    }
}

BENCHMARK(SerialCount)->RangeMultiplier(2)->Range(1 << 7, 1 << 17);

/**
 * @brief Baseline test for serial level count using ScatterView
 */
void ScatterCountSerial(benchmark::State &state) {
    auto const num_level = 10;
    auto const num_cell = (local_index_t)state.range(0);

    auto const cell_level = GetTestCellLevels(num_cell, num_level);

    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_counts(
        "eap::mesh::Kidmom::BuildPack::level_counts", num_level);

    for (auto _ : state) {
        ScatterCount<Kokkos::Serial>(cell_level, level_counts);
    }
}

BENCHMARK(ScatterCountSerial)->RangeMultiplier(2)->Range(1 << 7, 1 << 17);

/**
 * @brief Baseline test for parallel level count using atomics
 */
void AtomicCount(benchmark::State &state) {
    auto const num_level = 10;
    auto const num_cell = (local_index_t)state.range(0);

    auto const cell_level = GetTestCellLevels(num_cell, num_level);

    MeshView<local_index_t *> level_counts("eap::mesh::Kidmom::BuildPack::level_counts", num_level);

    for (auto _ : state) {
        // Count the number of cells at each level
        Kokkos::parallel_for(
            "eap::mesh::KidMom::BuildPack::count_levels",
            cell_level.extent_int(0),
            KOKKOS_LAMBDA(int const i) { Kokkos::atomic_increment(&level_counts(cell_level(i))); });
    }
}

BENCHMARK(AtomicCount)->RangeMultiplier(2)->Range(1 << 7, 1 << 17)->UseRealTime();

/**
 * @brief Baseline test for parallel level count using ScatterView
 */
void ScatterCountHost(benchmark::State &state) {
    auto const num_level = 10;
    auto const num_cell = (local_index_t)state.range(0);

    auto const cell_level = GetTestCellLevels(num_cell, num_level);

    Kokkos::View<local_index_t *, Kokkos::LayoutRight> level_counts(
        "eap::mesh::Kidmom::BuildPack::level_counts", num_level);

    for (auto _ : state) {
        ScatterCount<Kokkos::DefaultExecutionSpace>(cell_level, level_counts);
    }
}

BENCHMARK(ScatterCountHost)->RangeMultiplier(2)->Range(1 << 7, 1 << 17)->UseRealTime();
} // namespace bench
} // namespace mesh
} // namespace eap