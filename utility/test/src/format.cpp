/**
 * @file format.cpp
 *
 * @brief Tests for utility-format.hpp
 * @date 2019-03-25
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// STL Includes
#include <vector>

// Third Party Includes
#include <gtest/gtest.h>
#include <nonstd/span.hpp>

// Internal Includes
#include <utility-format.hpp>

TEST(Formatting, Strjoin) {
    using eap::utility::StringJoin;
    {
        std::vector<int> nums{3, 2, 1};
        ASSERT_EQ("3,2,1", StringJoin(nums, ","));

        // Verify it works with spans
        nonstd::span<int const> nums_span = nums;
        ASSERT_EQ("3,2,1", StringJoin(nums_span, ","));
    }

    {
        std::vector<int> nums{7};
        ASSERT_EQ("7", StringJoin(nums, ","));
    }
}