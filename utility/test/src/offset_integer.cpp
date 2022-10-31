/**
 * @file offset_integer.cpp
 *
 * @brief Tests for OffsetInteger
 * @date 2019-02-28
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// STL Includes
#include <numeric>

// Third Party Includes
#include <gtest/gtest.h>

// Internal Includes
#include <utility-fortran_index.hpp>
#include <utility-offset_integer.hpp>

using eap::utility::FortranIndex;
using eap::utility::OffsetInteger;

using namespace eap::utility::literals;

template <typename T>
T peek_value(FortranIndex<T> const &value) {
    return *reinterpret_cast<T const *>(&value);
}

TEST(OffsetInteger, Basic) {
    OffsetInteger<unsigned int, 1> value = 0;
    ASSERT_EQ(1, peek_value(value));

    FortranIndex<unsigned int> fortran_index = 7_fiu;
    ASSERT_EQ(8, peek_value(fortran_index));

    ASSERT_EQ(7, 7_fiu);
    ASSERT_EQ(8, peek_value(7_fiu));
}

TEST(OffsetInteger, PlusEquals) {
    {
        FortranIndex<unsigned int> index = 7_fiu;
        index += 5;

        ASSERT_EQ(12, index);
        ASSERT_EQ(13, peek_value(index));
    }

    {
        FortranIndex<unsigned int> index = 7_fiu;
        index += 5_fiu;

        ASSERT_EQ(12, index);
        ASSERT_EQ(13, peek_value(index));
    }
}

TEST(OffsetInteger, MinusEquals) {
    {
        FortranIndex<unsigned int> index = 7_fiu;
        index -= 5;

        ASSERT_EQ(2, index);
        ASSERT_EQ(3, peek_value(index));
    }

    {
        FortranIndex<unsigned int> index = 7_fiu;
        index -= 5_fiu;

        ASSERT_EQ(2, index);
        ASSERT_EQ(3, peek_value(index));
    }
}

TEST(OffsetInteger, Times) {
    {
        auto index = 7_fiu;
        index *= 5;

        ASSERT_EQ(35, index);
        ASSERT_EQ(36, peek_value(index));
    }

    ASSERT_EQ(35, 7 * 5_fiu);
    ASSERT_EQ(35, 7_fiu * 5);
    ASSERT_EQ(35, 7_fiu * 5_fiu);
}

TEST(OffsetInteger, Divide) {
    for (unsigned int ones = 5; ones < 10; ones++) {
        FortranIndex<unsigned int> index = ones;
        index /= 5;

        ASSERT_EQ(1, index);
        ASSERT_EQ(2, peek_value(index));
    }

    for (unsigned int twos = 10; twos < 15; twos++) {
        FortranIndex<unsigned int> index = twos;
        index /= 5;

        ASSERT_EQ(2, index);
        ASSERT_EQ(3, peek_value(index));
    }

    ASSERT_EQ(1, 9 / 5_fiu);
    ASSERT_EQ(1, 9_fiu / 5);
    ASSERT_EQ(1, 9_fiu / 5_fiu);
    ASSERT_EQ(2, 10 / 5_fiu);
    ASSERT_EQ(2, 10_fiu / 5);
    ASSERT_EQ(2, 10_fiu / 5_fiu);
}

TEST(OffsetInteger, Modulo) {
    {
        auto index = 10_fiu;
        index %= 4;

        ASSERT_EQ(2, index);
    }
}

TEST(OffsetInteger, PlusPlus) {
    auto index = 7_fiu;
    ASSERT_EQ(8, ++index);

    ASSERT_EQ(8, index);
    ASSERT_EQ(9, peek_value(index));

    ASSERT_EQ(8, index++);

    ASSERT_EQ(9, index);
    ASSERT_EQ(10, peek_value(index));
}

TEST(OffsetInteger, MinusMinus) {
    auto index = 7_fiu;
    ASSERT_EQ(6, --index);

    ASSERT_EQ(6, index);
    ASSERT_EQ(7, peek_value(index));

    ASSERT_EQ(6, index--);

    ASSERT_EQ(5, index);
    ASSERT_EQ(6, peek_value(index));
}

TEST(OffsetInteger, UnaryArithmetic) {
    // Just verify unary operators work correctly
    ASSERT_EQ(7, +(7_fiu));
    ASSERT_EQ(-7, -(7_fiu));

    ASSERT_EQ(0x0F0F0F0Fu, ~(0xF0F0F0F0_fiu));
}

TEST(OffsetInteger, Logical) {
    // Just verify that logical operators work correctly
    ASSERT_TRUE(1_fiu);
    ASSERT_FALSE(0_fiu);

    ASSERT_TRUE(!(0_fiu));
    ASSERT_FALSE(!(1_fiu));

    ASSERT_FALSE(0 && 1_fiu);
    ASSERT_FALSE(0_fiu && 1);
    ASSERT_FALSE(0_fiu && 1_fiu);

    ASSERT_TRUE(1 && 1_fiu);
    ASSERT_TRUE(1_fiu && 1);
    ASSERT_TRUE(1_fiu && 1_fiu);

    ASSERT_TRUE(0 || 1_fiu);
    ASSERT_TRUE(0_fiu || 1);
    ASSERT_TRUE(0_fiu || 1_fiu);

    ASSERT_FALSE(0 || 0_fiu);
    ASSERT_FALSE(0_fiu || 0);
    ASSERT_FALSE(0_fiu || 0_fiu);
}

TEST(OffsetInteger, Indexing) {
    constexpr int N = 10;
    std::vector<int> data(N);

    std::vector<FortranIndex<int>> indices(N);
    std::iota(indices.rbegin(), indices.rend(), 0);

    for (auto i = 0; i < N; i++) {
        data[indices[i]] = i;
    }

    for (auto i = 0; i < N; i++) {
        ASSERT_EQ(N - 1 - i, data[i]);
    }
}
