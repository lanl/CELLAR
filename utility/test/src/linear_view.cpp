/**
 * @file linear_view.cpp
 *
 * @date 2019-01-11
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// STL Includes
#include <numeric>

// Third party includes
#include <gtest/gtest.h>

// Internal includes
#include <utility-linear_view.hpp>

using eap::utility::LinearView;

TEST(LinearView, FlatArray) {
    constexpr size_t N = 100;

    Kokkos::View<int *, eap::HostMemorySpace> arr("test", N);

    {
        LinearView<int> arr_view(arr);

        auto view_span = arr_view.Span();

        ASSERT_EQ(arr.data(), &view_span[0]);

        std::iota(view_span.begin(), view_span.end(), 0);
    }

    for (size_t i = 0; i < N; i++) {
        ASSERT_EQ(i, arr(i));
    }
}

TEST(LinearView, StridedArray) {
    constexpr size_t N = 100;

    // Creates a row-major 2D array
    Kokkos::View<int **, Kokkos::LayoutRight, eap::HostMemorySpace> arr_2d("test", N, 2);

    // Takes the first dimension, stride of 2
    Kokkos::View<int *, Kokkos::LayoutStride, eap::HostMemorySpace> arr =
        Kokkos::subview(arr_2d, Kokkos::ALL, 0);

    ASSERT_EQ(2, arr.stride_0());

    {
        // Creates a view - this should allocate a linear buffer.
        LinearView<int> arr_view(arr);

        // Get a mutable span for the LinearView
        auto view_span = arr_view.Span();

        // The span should NOT point to the same data as the Kokkos::View.
        ASSERT_NE(arr.data(), &view_span[0]);

        // Fill out the span
        std::iota(view_span.begin(), view_span.end(), 0);
    }
    // LinearView is deallocated, so it should sync back to the original View.

    for (size_t i = 0; i < N; i++) {
        ASSERT_EQ(i, arr(i));
    }
}