/**
 * @file utility-kokkos.cpp
 *
 * @brief Implements utilities for working with Kokkos Views
 * @date 2019-07-16
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <utility-kokkos.hpp>

bool eap::utility::kokkos::IsLayoutRight(size_t rank,
                                         Kokkos::LayoutStride const &stride,
                                         Kokkos::LayoutRight *right) {
    // strides should be descending - greatest stride in first dimension, stride 1 in last dimension
    size_t dim = rank;
    size_t expected_stride = 1;
    while (dim-- > 0) {
        if (stride.stride[dim] != expected_stride) {
            return false;
        }

        expected_stride *= stride.dimension[dim];
    }

    if (right) {
        *right = Kokkos::LayoutRight{stride.dimension[0],
                                     stride.dimension[1],
                                     stride.dimension[2],
                                     stride.dimension[3],
                                     stride.dimension[4],
                                     stride.dimension[5],
                                     stride.dimension[6],
                                     stride.dimension[7]};
    }
    return true;
}

bool eap::utility::kokkos::IsLayoutLeft(size_t rank,
                                        Kokkos::LayoutStride const &stride,
                                        Kokkos::LayoutLeft *left) {
    // strides should be ascending - greatest stride in last dimension, stride 1 in first dimension
    size_t expected_stride = 1;
    for (size_t dim = 0; dim < rank; dim++) {
        if (stride.stride[dim] != expected_stride) {
            return false;
        }

        expected_stride *= stride.dimension[dim];
    }

    if (left) {
        *left = Kokkos::LayoutLeft{stride.dimension[0],
                                   stride.dimension[1],
                                   stride.dimension[2],
                                   stride.dimension[3],
                                   stride.dimension[4],
                                   stride.dimension[5],
                                   stride.dimension[6],
                                   stride.dimension[7]};
    }
    return true;
}
