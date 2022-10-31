#include <gtest/gtest.h>
#include <utility-kokkos.hpp>

using namespace eap::utility::kokkos;

TEST(IsTranspose, HostDefaultToTransposed) {
    Kokkos::View<int **> view("HostDefaultToTransposed", 4, 4);

    for (auto i = 0; i < 4; i++) {
        for (auto j = 0; j < 4; j++) {
            view(i, j) = i * 4 + j;
        }
    }

    auto const transposed = Transpose(view);

    for (auto i = 0; i < 4; i++) {
        for (auto j = 0; j < 4; j++) {
            ASSERT_EQ(j * 4 + i, transposed(i, j));
        }
    }
}

TEST(IsTranspose, LayoutLeftToLayoutRight) {
    Kokkos::View<int **, Kokkos::LayoutLeft> view("LayoutRightToLayoutLeft", 4, 4);

    for (auto j = 0; j < 4; j++) {
        for (auto i = 0; i < 4; i++) {
            view(i, j) = i * 4 + j;
        }
    }

    auto const transposed = Transpose(view);

    for (auto i = 0; i < 4; i++) {
        for (auto j = 0; j < 4; j++) {
            ASSERT_EQ(j * 4 + i, transposed(i, j));
        }
    }
}

TEST(IsTranspose, LayoutStride) {
    // Construct LayoutStride from LayoutRight
    Kokkos::View<int **, Kokkos::LayoutStride> view =
        Kokkos::View<int **, Kokkos::LayoutRight>("LayoutStride", 4, 4);

    for (auto j = 0; j < 4; j++) {
        for (auto i = 0; i < 4; i++) {
            view(i, j) = i * 4 + j;
        }
    }

    auto const transposed = Transpose(view);

    for (auto i = 0; i < 4; i++) {
        for (auto j = 0; j < 4; j++) {
            ASSERT_EQ(j * 4 + i, transposed(i, j));
        }
    }
}

TEST(RankConversion, LayoutRight) {
    Kokkos::View<int *, Kokkos::LayoutRight> view("LayoutRight", 4);

    for (auto i = 0; i < 4; i++) {
        view(i) = i;
    }

    auto const two_dim_array = Convert1DTo2D(view);

    using View2D = decltype(two_dim_array);
    static_assert(View2D::rank == 2, "two_dim_array should be 2D");
    static_assert(std::is_same<typename decltype(two_dim_array)::data_type, int * [1]>::value,
                  "two_dim_array is int*[1]");

    for (auto i = 0; i < 4; i++) {
        ASSERT_EQ(i, two_dim_array(i, 0));
    }
}

TEST(IsLayoutRight, Test) {
    Kokkos::LayoutRight layout;
    // 1D
    {
        Kokkos::View<int *, Kokkos::LayoutRight> right("LayoutRight", 8);
        Kokkos::View<int *, Kokkos::LayoutLeft> left("LayoutLeft", 8);
        Kokkos::View<int *, Kokkos::LayoutRight> stride = right;

        ASSERT_TRUE(IsLayoutRight(right, layout));
        ASSERT_TRUE(IsLayoutRight(left, layout));
        ASSERT_TRUE(IsLayoutRight(stride, layout));
    }

    // 2D
    {
        Kokkos::View<int **, Kokkos::LayoutRight> right("LayoutRight", 8, 8);
        Kokkos::View<int **, Kokkos::LayoutLeft> left("LayoutLeft", 8, 8);
        Kokkos::View<int **, Kokkos::LayoutStride> stride = right;

        ASSERT_TRUE(IsLayoutRight(right, layout));
        ASSERT_FALSE(IsLayoutRight(left, layout));
        ASSERT_TRUE(IsLayoutRight(stride, layout));
    }
}

TEST(IsLayoutLeft, Test) {
    Kokkos::LayoutLeft layout;
    // 1D
    {
        Kokkos::View<int *, Kokkos::LayoutLeft> left("LayoutLeft", 8);
        Kokkos::View<int *, Kokkos::LayoutRight> right("LayoutRight", 8);
        Kokkos::View<int *, Kokkos::LayoutRight> stride = left;

        ASSERT_TRUE(IsLayoutLeft(right, layout));
        ASSERT_TRUE(IsLayoutLeft(left, layout));
        ASSERT_TRUE(IsLayoutLeft(stride, layout));
    }

    // 2D
    {
        Kokkos::View<int **, Kokkos::LayoutLeft> left("LayoutLeft", 8, 8);
        Kokkos::View<int **, Kokkos::LayoutRight> right("LayoutRight", 8, 8);
        Kokkos::View<int **, Kokkos::LayoutStride> stride = left;

        ASSERT_TRUE(IsLayoutLeft(left, layout));
        ASSERT_FALSE(IsLayoutLeft(right, layout));
        ASSERT_TRUE(IsLayoutLeft(stride, layout));
    }
}