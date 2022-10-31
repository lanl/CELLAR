/**
 * @file integers.cpp
 *
 * @brief Benchmarks for Integer types vs. raw integers
 * @date 2019-03-01
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// STL Includes
#include <numeric>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <benchmark/benchmark.h>

// Internal Includes
#include <utility-fortran_index.hpp>
#include <utility-offset_integer.hpp>
#include <utility-optional_integer.hpp>

using eap::utility::FortranIndex;
using eap::utility::OptionalFortranIndex;
using eap::utility::OptionalInteger;

template <typename Integer>
using NonNegativeInteger = OptionalInteger<Integer, Integer, -1, false>;

static_assert(OptionalFortranIndex<int>::MinValue == 0,
              "Minimum logical value for OptionalFortranIndex<int> 0");

static_assert(OptionalFortranIndex<std::uint32_t>::MinValue == 0,
              "SentinelMin for Optional Fortran std::uint32_t is 0x80000000");
static_assert(OptionalFortranIndex<std::uint32_t>::MaxValue == 0x80000000 - 1,
              "SentinelMin for Optional Fortran std::uint32_t is 0x80000000");

static_assert(OptionalFortranIndex<std::uint32_t>(0).value() == 0,
              "OptionalFortranIndex<std::uint32_t>(0) should equal 0");
static_assert(!OptionalFortranIndex<std::uint32_t>(0xFFFFFFFF),
              "OptionalFortranIndex<std::uint32_t> should be null if explicitly set to the maximum "
              "invalid value");

namespace {

/**
 * @brief Baseline test for "cost" of assigning into an int.
 */
void AssignInt(benchmark::State &state) {
    volatile int value = 7;

    for (auto _ : state) {
        int val;
#pragma unroll
        for (int i = 0; i < 64; i++) {
            benchmark::DoNotOptimize(val = value);
        }
    }
}

BENCHMARK(AssignInt);

/**
 * @brief Baseline for "cost" of assigning an incremented int explicitly.
 */
void AssignIncrementedInt(benchmark::State &state) {
    volatile int value = 7;

    for (auto _ : state) {
        int val;
#pragma unroll
        for (int i = 0; i < 64; i++) {
            benchmark::DoNotOptimize(val = value + 1);
        }
    }
}

BENCHMARK(AssignIncrementedInt);

/**
 * @brief Test for "cost" of assigning into a FortranIndex<int>. Goal is to prove that it has tiny
 * overhead.
 *
 * @details
 * Expected to have the exact same instructions as `AssignIncrementedInt`
 */
void AssignFortranIndex(benchmark::State &state) {
    volatile int value = 7;

    for (auto _ : state) {
        FortranIndex<int> val;
#pragma unroll
        for (int i = 0; i < 64; i++) {
            benchmark::DoNotOptimize(val = value);
        }
    }
}

BENCHMARK(AssignFortranIndex);

/**
 * @brief Test for "cost" of assigning into an OptionalFortranIndex<int>. Shouldn't have any
 * overhead over assigning into a FortranIndex<int>.
 */
void AssignOptionalFortranIndex(benchmark::State &state) {
    volatile int value = 7;

    for (auto _ : state) {
        OptionalFortranIndex<int> val;
#pragma unroll
        for (int i = 0; i < 64; i++) {
            benchmark::DoNotOptimize(*val = value);
        }
    }
}

BENCHMARK(AssignOptionalFortranIndex);

/**
 * @brief Baseline for testing for non-negative values
 */
void CheckAndUseInt(benchmark::State &state) {
    volatile int input = 7;

    for (auto _ : state) {
#pragma unroll
        for (int i = 0; i < 64; i++) {
            int value = input;
            if (value >= 0) {
                benchmark::DoNotOptimize(value);
            }
        }
    }
}

BENCHMARK(CheckAndUseInt);

/**
 * @brief Using OptionalInteger to test for non-negative values. Should produce same instructions
 * as CheckAndUseInt
 */
void CheckAndUseNonNegativeInt(benchmark::State &state) {
    volatile int input = 7;

    for (auto _ : state) {
#pragma unroll
        for (int i = 0; i < 64; i++) {
            NonNegativeInteger<int> value = input;
            if (value) {
                benchmark::DoNotOptimize(value);
            }
        }
    }
}

BENCHMARK(CheckAndUseNonNegativeInt);

/**
 * @brief Using OptionalFortranIndex to test for non-negative values. Should have near-zero cost
 * over `CheckAndUseInt` and `CheckAndUseNonNegativeInt`
 */
void CheckAndUseNonNegativeFortranIndex(benchmark::State &state) {
    volatile int input = 7;

    for (auto _ : state) {
#pragma unroll
        for (int i = 0; i < 64; i++) {
            OptionalFortranIndex<int> value(input);
            if (value) {
                benchmark::DoNotOptimize(value);
            }
        }
    }
}

BENCHMARK(CheckAndUseNonNegativeFortranIndex);

void CheckVectorization(benchmark::State &state) {
    Kokkos::View<int *> indices("Indices", 10000);
    benchmark::DoNotOptimize(indices);

    for (auto _ : state) {
        for (int i = 0; i < indices.extent_int(0); i++) {
            indices(i) += 1;
        }
        benchmark::DoNotOptimize(indices);
    }
}

BENCHMARK(CheckVectorization);

void CheckVectorizationFortranIndex(benchmark::State &state) {
    Kokkos::View<FortranIndex<int> *> indices("Indices", 10000);
    benchmark::DoNotOptimize(indices);

    for (auto _ : state) {
        for (int i = 0; i < indices.extent_int(0); i++) {
            indices(i) += 1;
        }
        benchmark::DoNotOptimize(indices);
    }
}

BENCHMARK(CheckVectorizationFortranIndex);
} // namespace