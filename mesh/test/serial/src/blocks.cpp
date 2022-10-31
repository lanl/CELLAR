#include <array>
#include <gtest/gtest.h>
#include <mesh-blocks.hpp>

using namespace eap;

using BC = mesh::BlockCoordinates;

template <size_t N>
void check_layout(mesh::BlockLayout const &layout,
                  std::array<mesh::BlockCoordinates, N> const &expected) {
    for (size_t i = 0; i < N; i++) {
        ASSERT_EQ(expected[i], layout.GetCoordinates(i));
        ASSERT_EQ(i, layout.GetAddress(expected[i]));
    }
}

TEST(Blocks, OneDimensional) {
    constexpr auto N = 2;

    std::array<mesh::BlockCoordinates, N> expected{
        BC{0},
        BC{1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajor, N};

    check_layout(layout, expected);
}

TEST(Blocks, OneDimensionalZigZag) {
    constexpr auto N = 2;

    std::array<mesh::BlockCoordinates, N> expected{
        BC{0},
        BC{1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorZigZag, N};

    check_layout(layout, expected);
}

TEST(Blocks, OneDimensionalReverse) {
    constexpr auto N = 2;

    std::array<mesh::BlockCoordinates, N> expected{
        BC{0},
        BC{1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorReverse, N};

    check_layout(layout, expected);
}

TEST(Blocks, TwoDimensional) {
    constexpr auto M = 2;
    constexpr auto N = 3;

    std::array<mesh::BlockCoordinates, M * N> expected{
        BC{0, 0},
        BC{1, 0},
        BC{2, 0},
        BC{0, 1},
        BC{1, 1},
        BC{2, 1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajor, N, M};

    check_layout(layout, expected);
}

TEST(Blocks, TwoDimensionalZigZag) {
    constexpr auto M = 2;
    constexpr auto N = 3;

    std::array<mesh::BlockCoordinates, M * N> expected{
        BC{0, 0},
        BC{1, 0},
        BC{2, 0},
        BC{2, 1},
        BC{1, 1},
        BC{0, 1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorZigZag, N, M};

    check_layout(layout, expected);
}

TEST(Blocks, TwoDimensionalReverse) {
    constexpr auto M = 2;
    constexpr auto N = 3;

    std::array<mesh::BlockCoordinates, M * N> expected{
        BC{0, 0},
        BC{1, 0},
        BC{2, 0},
        BC{0, 1},
        BC{1, 1},
        BC{2, 1},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorReverse, N, M};

    check_layout(layout, expected);
}

TEST(Blocks, ThreeDimensional) {
    constexpr auto X = 2;
    constexpr auto Y = 3;
    constexpr auto Z = 4;

    std::array<mesh::BlockCoordinates, X * Y * Z> expected{
        BC{0, 0, 0}, BC{1, 0, 0}, BC{0, 1, 0}, BC{1, 1, 0}, BC{0, 2, 0}, BC{1, 2, 0},
        BC{0, 0, 1}, BC{1, 0, 1}, BC{0, 1, 1}, BC{1, 1, 1}, BC{0, 2, 1}, BC{1, 2, 1},
        BC{0, 0, 2}, BC{1, 0, 2}, BC{0, 1, 2}, BC{1, 1, 2}, BC{0, 2, 2}, BC{1, 2, 2},
        BC{0, 0, 3}, BC{1, 0, 3}, BC{0, 1, 3}, BC{1, 1, 3}, BC{0, 2, 3}, BC{1, 2, 3},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajor, X, Y, Z};

    check_layout(layout, expected);
}

TEST(Blocks, ThreeDimensionalZigZag) {
    constexpr auto X = 2;
    constexpr auto Y = 3;
    constexpr auto Z = 4;

    std::array<mesh::BlockCoordinates, X * Y * Z> expected{
        BC{0, 0, 0}, BC{1, 0, 0}, BC{1, 1, 0}, BC{0, 1, 0}, BC{0, 2, 0}, BC{1, 2, 0},
        BC{0, 0, 1}, BC{1, 0, 1}, BC{1, 1, 1}, BC{0, 1, 1}, BC{0, 2, 1}, BC{1, 2, 1},
        BC{0, 0, 2}, BC{1, 0, 2}, BC{1, 1, 2}, BC{0, 1, 2}, BC{0, 2, 2}, BC{1, 2, 2},
        BC{0, 0, 3}, BC{1, 0, 3}, BC{1, 1, 3}, BC{0, 1, 3}, BC{0, 2, 3}, BC{1, 2, 3},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorZigZag, X, Y, Z};

    check_layout(layout, expected);
}

TEST(Blocks, ThreeDimensionalReverse) {
    constexpr auto X = 2;
    constexpr auto Y = 3;
    constexpr auto Z = 4;

    std::array<mesh::BlockCoordinates, X * Y * Z> expected{
        BC{0, 0, 0}, BC{1, 0, 0}, BC{0, 0, 1}, BC{1, 0, 1}, BC{0, 0, 2}, BC{1, 0, 2},
        BC{0, 0, 3}, BC{1, 0, 3}, BC{0, 1, 0}, BC{1, 1, 0}, BC{0, 1, 1}, BC{1, 1, 1},
        BC{0, 1, 2}, BC{1, 1, 2}, BC{0, 1, 3}, BC{1, 1, 3}, BC{0, 2, 0}, BC{1, 2, 0},
        BC{0, 2, 1}, BC{1, 2, 1}, BC{0, 2, 2}, BC{1, 2, 2}, BC{0, 2, 3}, BC{1, 2, 3},
    };

    mesh::BlockLayout layout{mesh::BlockLayoutMethod::ColumnMajorReverse, X, Y, Z};

    check_layout(layout, expected);
}