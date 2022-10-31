/**
 * @file test-serial.cpp
 *
 * @brief Main function for serial unit tests
 * @date 2019-07-16
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    Kokkos::initialize(argc, argv);

    auto exit_code = RUN_ALL_TESTS();

    Kokkos::finalize();
    return exit_code;
}