// Library under test
#include <error-macros.hpp>

// STL includes
#include <string>

// Third party includes
#include <gtest/gtest.h>

using namespace std::string_literals;

TEST(Error, Originate) {
    EE_PRELUDE

    auto threw = false;
    size_t line = 0;
    try {
        // clang-format off
        line = __LINE__; EE_RAISE("An error indeed happened here at line " << line);
        // clang-format on
    } catch (eap::error::RaisedError const &err) {
        ASSERT_EQ(line, err.location().line);
        ASSERT_TRUE(err.location().file.ends_with("error_test.cpp"));
        std::cout << err.what() << std::endl;
        threw = true;
    }

    ASSERT_TRUE(threw);
}

void DoTheThing(std::string color, size_t &line) {
    EE_PRELUDE

    // clang-format off
    line = __LINE__; EE_RAISE("I hate " << color << "!");
    // clang-format on
}

TEST(Error, Propagate) {
    EE_PRELUDE

    size_t line = 0;
    auto color = "blue"s;
    try {
        EE_CHECK(DoTheThing(color, line), "We really didn't like " << color << "...");
    } catch (eap::error::PropagatedError const &err) {
        auto child_err = static_cast<eap::error::RaisedError const *>(err.exception());
        ASSERT_EQ(line, child_err->location().line);

        std::cout << err.what() << std::endl;
    }
}

TEST(Error, Interop) {
    EE_PRELUDE

    try {
        auto big_vec = EE_CHECK(std::vector<int>(std::numeric_limits<size_t>::max()),
                                "Had trouble running this routine with a big allocation...");
    } catch (eap::error::PropagatedError const &err) {
        std::cout << err.what() << std::endl;
    }
}

TEST(Error, ShouldAbort) {
    ASSERT_DEATH(([]() {
                     EAP_EXTERN_PRE

                     EE_RAISE("I should be causing death...");

                     EAP_EXTERN_POST
                 }()),
                 "");
}