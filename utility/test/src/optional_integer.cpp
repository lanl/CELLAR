/**
 * @file optional_integer.cpp
 *
 * @brief Tests for OptionalInteger
 * @date 2019-02-28
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <gtest/gtest.h>

// Internal Includes
#include <utility-fortran_index.hpp>
#include <utility-offset_integer.hpp>
#include <utility-optional_integer.hpp>

using eap::utility::FortranIndex;
using eap::utility::nullint;
using eap::utility::OptionalFortranIndex;
using eap::utility::OptionalInteger;

TEST(OptionalInteger, Basic) {
    ASSERT_TRUE(!OptionalInteger<int>());

    ASSERT_TRUE(OptionalInteger<int>(0));
    ASSERT_TRUE(*OptionalInteger<int>(0) == 0);
}

TEST(OptionalInteger, Equals) {
    ASSERT_TRUE(OptionalInteger<int>() == OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>() == nullint);
    ASSERT_TRUE(nullint == OptionalInteger<int>());

    ASSERT_TRUE(!(OptionalInteger<int>(7) == OptionalInteger<int>(6)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) == 6));
    ASSERT_TRUE(!(7 == OptionalInteger<int>(6)));

    ASSERT_TRUE(OptionalInteger<int>(7) == OptionalInteger<int>(7));
    ASSERT_TRUE(7 == OptionalInteger<int>(7));
    ASSERT_TRUE(OptionalInteger<int>(7) == 7);

    ASSERT_TRUE(!(OptionalInteger<int>(7) == OptionalInteger<int>(8)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) == 8));
    ASSERT_TRUE(!(7 == OptionalInteger<int>(8)));

    ASSERT_TRUE(!(OptionalInteger<int>() == OptionalInteger<int>(OptionalInteger<int>::MinValue)));
    ASSERT_TRUE(!(nullint == OptionalInteger<int>(OptionalInteger<int>::MinValue)));

    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) == OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) == nullint));

    ASSERT_TRUE(!(OptionalInteger<int>() == OptionalInteger<int>::Sentinel()));
    ASSERT_TRUE(!(OptionalInteger<int>::Sentinel() == OptionalInteger<int>()));
}

TEST(OptionalInteger, NotEquals) {
    ASSERT_TRUE(!(OptionalInteger<int>() != OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>() != nullint));
    ASSERT_TRUE(!(nullint != OptionalInteger<int>()));

    ASSERT_TRUE(OptionalInteger<int>(7) != OptionalInteger<int>(6));
    ASSERT_TRUE(OptionalInteger<int>(7) != 6);
    ASSERT_TRUE(7 != OptionalInteger<int>(6));

    ASSERT_TRUE(!(OptionalInteger<int>(7) != OptionalInteger<int>(7)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) != 7));
    ASSERT_TRUE(!(7 != OptionalInteger<int>(7)));

    ASSERT_TRUE(OptionalInteger<int>(7) != OptionalInteger<int>(8));
    ASSERT_TRUE(OptionalInteger<int>(7) != 8);
    ASSERT_TRUE(7 != OptionalInteger<int>(8));

    ASSERT_TRUE(OptionalInteger<int>() != OptionalInteger<int>(OptionalInteger<int>::MinValue));
    ASSERT_TRUE(nullint != OptionalInteger<int>(OptionalInteger<int>::MinValue));

    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) != OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) != nullint);

    ASSERT_TRUE(OptionalInteger<int>() != OptionalInteger<int>::Sentinel());
    ASSERT_TRUE(OptionalInteger<int>::Sentinel() != OptionalInteger<int>());
}

TEST(OptionalInteger, Less) {
    ASSERT_TRUE(!(OptionalInteger<int>() < OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>() < nullint));
    ASSERT_TRUE(!(nullint < OptionalInteger<int>()));

    ASSERT_TRUE(!(OptionalInteger<int>(7) < OptionalInteger<int>(6)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) < 6));
    ASSERT_TRUE(!(7 < OptionalInteger<int>(6)));

    ASSERT_TRUE(!(OptionalInteger<int>(7) < OptionalInteger<int>(7)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) < 7));
    ASSERT_TRUE(!(OptionalInteger<int>(7) < 7));

    ASSERT_TRUE(OptionalInteger<int>(7) < OptionalInteger<int>(8));
    ASSERT_TRUE(OptionalInteger<int>(7) < 8);
    ASSERT_TRUE(7 < OptionalInteger<int>(8));

    ASSERT_TRUE(OptionalInteger<int>() < OptionalInteger<int>(OptionalInteger<int>::MinValue));
    ASSERT_TRUE(nullint < OptionalInteger<int>(OptionalInteger<int>::MinValue));

    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) < OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) < nullint));

    ASSERT_TRUE(OptionalInteger<int>() < OptionalInteger<int>::Sentinel());
    ASSERT_TRUE(!(OptionalInteger<int>::Sentinel() < OptionalInteger<int>()));
}

TEST(OptionalInteger, LessEquals) {
    ASSERT_TRUE(OptionalInteger<int>() <= OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>() <= nullint);
    ASSERT_TRUE(nullint <= OptionalInteger<int>());

    ASSERT_TRUE(!(OptionalInteger<int>(7) <= OptionalInteger<int>(6)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) <= 6));
    ASSERT_TRUE(!(7 <= OptionalInteger<int>(6)));

    ASSERT_TRUE(OptionalInteger<int>(7) <= OptionalInteger<int>(7));
    ASSERT_TRUE(OptionalInteger<int>(7) <= 7);
    ASSERT_TRUE(7 <= OptionalInteger<int>(7));

    ASSERT_TRUE(OptionalInteger<int>(7) <= OptionalInteger<int>(8));
    ASSERT_TRUE(OptionalInteger<int>(7) <= 8);
    ASSERT_TRUE(7 <= OptionalInteger<int>(8));

    ASSERT_TRUE(OptionalInteger<int>() <= OptionalInteger<int>(OptionalInteger<int>::MinValue));
    ASSERT_TRUE(nullint <= OptionalInteger<int>(OptionalInteger<int>::MinValue));

    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) <= OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>(OptionalInteger<int>::MinValue) <= nullint));

    ASSERT_TRUE(OptionalInteger<int>() <= OptionalInteger<int>::Sentinel());
    ASSERT_TRUE(!(OptionalInteger<int>::Sentinel() <= OptionalInteger<int>()));
}

TEST(OptionalInteger, Greater) {
    ASSERT_TRUE(!(OptionalInteger<int>() > OptionalInteger<int>()));
    ASSERT_TRUE(!(OptionalInteger<int>() > nullint));
    ASSERT_TRUE(!(nullint > OptionalInteger<int>()));

    ASSERT_TRUE(OptionalInteger<int>(7) > OptionalInteger<int>(6));
    ASSERT_TRUE(OptionalInteger<int>(7) > 6);
    ASSERT_TRUE(7 > OptionalInteger<int>(6));

    ASSERT_TRUE(!(OptionalInteger<int>(7) > OptionalInteger<int>(7)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) > 7));
    ASSERT_TRUE(!(7 > OptionalInteger<int>(7)));

    ASSERT_TRUE(!(OptionalInteger<int>(7) > OptionalInteger<int>(8)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) > 8));
    ASSERT_TRUE(!(7 > OptionalInteger<int>(8)));

    ASSERT_TRUE(!(OptionalInteger<int>() > OptionalInteger<int>(OptionalInteger<int>::MinValue)));
    ASSERT_TRUE(!(nullint > OptionalInteger<int>(OptionalInteger<int>::MinValue)));

    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) > OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) > nullint);

    ASSERT_TRUE(!(OptionalInteger<int>() > OptionalInteger<int>::Sentinel()));
    ASSERT_TRUE(OptionalInteger<int>::Sentinel() > OptionalInteger<int>());
}

TEST(OptionalInteger, GreaterEquals) {
    ASSERT_TRUE(OptionalInteger<int>() >= OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>() >= nullint);
    ASSERT_TRUE(nullint >= OptionalInteger<int>());

    ASSERT_TRUE(OptionalInteger<int>(7) >= OptionalInteger<int>(6));
    ASSERT_TRUE(OptionalInteger<int>(7) >= 6);
    ASSERT_TRUE(7 >= OptionalInteger<int>(6));

    ASSERT_TRUE(OptionalInteger<int>(7) >= OptionalInteger<int>(7));
    ASSERT_TRUE(OptionalInteger<int>(7) >= 7);
    ASSERT_TRUE(7 >= OptionalInteger<int>(7));

    ASSERT_TRUE(!(OptionalInteger<int>(7) >= OptionalInteger<int>(8)));
    ASSERT_TRUE(!(OptionalInteger<int>(7) >= 8));
    ASSERT_TRUE(!(7 >= OptionalInteger<int>(8)));

    ASSERT_TRUE(!(OptionalInteger<int>() >= OptionalInteger<int>(OptionalInteger<int>::MinValue)));
    ASSERT_TRUE(!(nullint >= OptionalInteger<int>(OptionalInteger<int>::MinValue)));

    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) >= OptionalInteger<int>());
    ASSERT_TRUE(OptionalInteger<int>(OptionalInteger<int>::MinValue) >= nullint);

    ASSERT_TRUE(!(OptionalInteger<int>() >= OptionalInteger<int>::Sentinel()));
    ASSERT_TRUE(OptionalInteger<int>::Sentinel() >= OptionalInteger<int>());
}

TEST(OptionalInteger, WithFortranIndex) {
    {
        OptionalFortranIndex<unsigned int> const opt;
        ASSERT_EQ(0, *(int const *)(&opt));
    }

    {
        OptionalFortranIndex<int> const opt;
        ASSERT_EQ(0, *(int const *)(&opt));
    }
}