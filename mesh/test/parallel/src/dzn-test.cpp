/**
 * @file dzn-test.cpp
 *
 * @brief Tests mesh-dzn
 * @date 2019-08-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <gtest/gtest.h>

// Internal Includes
#include <mesh-dzn_compute.hpp>

using namespace eap;

using eap::mesh::Coordinates;
using eap::mesh::DefinedZoneResolutionInfo;
using eap::mesh::Kode;
using eap::mesh::NewDzn;
using eap::mesh::ZoneOptions;
using eap::mesh::ZoneShape;

TEST(DZN, Basic) {
    ZoneOptions options;
    options.add = false;
    options.kode = Kode::Unconditional;
    options.outer = false;
    options.use = ZoneShape::BricksAndRectangles;

    DefinedZoneResolutionInfo dzn;

    NewDzn first;
    first.level = 4;
    first.x = Kokkos::make_pair(-2.0, 2.0);
    first.options.SetOptions(options);
    dzn.Push(first);

    NewDzn second;
    second.level = 2;
    second.x = Kokkos::make_pair(-1.0, 1.0);
    second.xd = Kokkos::make_pair(0.5, -0.5);
    second.options.SetOptions(options);
    dzn.Push(second);

    Coordinates<double> const point{0.75};

    // At time 0, second DZN applies because it applies after the first DZN.
    ASSERT_EQ(1, eap::mesh::FindDZN(dzn, 1, 0.0, point));

    // At time 1, the first DZN applies because the second was has shrunk to no longer contain
    // `point`.
    ASSERT_EQ(0, eap::mesh::FindDZN(dzn, 1, 1.0, point));

    // Outside of DZNs, nullopt is returned
    ASSERT_EQ(nonstd::nullopt, eap::mesh::FindDZN(dzn, 1, 0.0, Coordinates<double>{3.0}));
}

TEST(DZN, Circle) {
    local_index_t const OUTER_CIRCLE = 0;
    local_index_t const INNER_CIRCLE = 1;

    ZoneOptions options;
    options.add = false;
    options.kode = Kode::Unconditional;
    options.outer = false;
    options.use = ZoneShape::SpheresAndCircles;

    DefinedZoneResolutionInfo dzn;

    NewDzn first;
    first.level = 4;
    first.r = Kokkos::make_pair(2.0, 3.0);
    first.options.SetOptions(options);
    dzn.Push(first);

    NewDzn second;
    second.level = 2;
    second.r = Kokkos::make_pair(0.0, 0.0);
    second.rd = Kokkos::make_pair(0.0, 1.0);
    second.options.SetOptions(options);
    dzn.Push(second);

    // At r = 1.0
    Coordinates<double> const inner_point{0.5, -std::sqrt(0.75)};

    // At r = 2.5
    Coordinates<double> const outer_point{-2.0, -1.5};

    // Inner circle not yet expanded - so no matching DZN for now.
    ASSERT_EQ(nonstd::nullopt, eap::mesh::FindDZN(dzn, 2, 0.0, inner_point));

    // outer_point matches at time = 0.0
    ASSERT_EQ(OUTER_CIRCLE, eap::mesh::FindDZN(dzn, 2, 0.0, outer_point));

    // Inner circle matches at time >= 1.0, outer_point is not yet covered by INNER_CIRCLE
    ASSERT_EQ(INNER_CIRCLE, eap::mesh::FindDZN(dzn, 2, 1.1, inner_point));
    ASSERT_EQ(OUTER_CIRCLE, eap::mesh::FindDZN(dzn, 2, 1.1, outer_point));

    // At time >= 2.5, outer_point and inner_point are both in INNER_CIRCLE
    ASSERT_EQ(INNER_CIRCLE, eap::mesh::FindDZN(dzn, 2, 2.6, inner_point));
    ASSERT_EQ(INNER_CIRCLE, eap::mesh::FindDZN(dzn, 2, 2.6, outer_point));
}