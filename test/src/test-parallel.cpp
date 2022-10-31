/**
 * @file test-parallel.cpp
 *
 * @brief Main function for parallel unit tests
 * @date 2019-07-16
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <gtest-mpi/init.hpp>
#include <gtest/gtest.h>
#include <mpi.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    gtest_mpi::init(&argc, &argv);

    Kokkos::initialize(argc, argv);

    auto exit_code = RUN_ALL_TESTS();

    Kokkos::finalize();

    gtest_mpi::finalize();
    MPI_Finalize();

    return exit_code;
}