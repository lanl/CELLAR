#include <chrono>
#include <gtest/gtest.h>
#include <map>
#include <perf-registry.hpp>
#include <string>
#include <thread>

using namespace std::literals::chrono_literals;

using std::string_view;
using std::map;
using std::chrono::steady_clock;

TEST(EAPTimerRegistry, Basic) {
    constexpr auto sleep_time = 100ms;

    eap::perf::TimerRegistry<steady_clock> registry;

    {
        auto const a = registry.TimeSection("a");
        {
            auto const b = registry.TimeSection("b");
            {
                auto const c = registry.TimeSection("c");
                { std::this_thread::sleep_for(sleep_time); }
            }
        }
    }

    map<string_view, bool> saw{{"a", false}, {"b", false}, {"c", false}};

    for (auto handle : registry.Timers()) {
        auto const name = registry.GetTimerName(handle);
        auto const &timer = registry.GetTimer(handle);

        ASSERT_FALSE(saw[name]);
        saw[name] = true;

        if (name == "a") {
            ASSERT_EQ(1, timer.TimerCount());
            ASSERT_LT(sleep_time / 2, timer.SumTime());
            ASSERT_GT(sleep_time * 3 / 2, timer.SumTime());
        } else if (name == "b" || name == "c") {
            ASSERT_EQ(0, timer.TimerCount());
            ASSERT_EQ(0s, timer.SumTime());
            ASSERT_EQ(std::nullopt, timer.AverageTime());
            ASSERT_EQ(std::nullopt, timer.MinTime());
            ASSERT_EQ(std::nullopt, timer.MaxTime());
        } else {
            ASSERT_TRUE(false);
        }
    }

    ASSERT_TRUE(saw["a"]);
    ASSERT_TRUE(saw["b"]);
    ASSERT_TRUE(saw["c"]);
}

TEST(EAPTimerRegistry, ExitsOnUnbalancedPop) {
    eap::perf::TimerRegistry<steady_clock> registry;

    ASSERT_DEATH(registry.PopTimer(), "Unbalanced timer push and pop");
}

TEST(EAPTimerRegistry, TimedSectionUnbalanced) {
    eap::perf::TimerRegistry<steady_clock> registry;

    ASSERT_DEATH(
        {
            auto first = registry.TimeSection("first");
            auto second = registry.TimeSection("second");
            registry.PopTimer();
        },
        "Tried to pop timer 'second' while 'first' was on the top of the timer stack!");
}

TEST(EAPTimerRegistry, UnorderedSectionExit) {
    eap::perf::TimerRegistry<steady_clock> registry;

    ASSERT_DEATH(
        {
            std::optional<eap::perf::TimedSection<steady_clock>> second;

            auto first = registry.TimeSection("first");
            second = registry.TimeSection("second");
        },
        "Tried to pop timer 'first' while 'second' was on the top of the timer stack!");
}