#include <chrono>
#include <gtest/gtest.h>
#include <perf-timer.hpp>
#include <thread>

using namespace std::literals::chrono_literals;

TEST(EAPTimer, SleepDecisecond) {
    constexpr auto sleep_time = 100ms;

    eap::perf::Timer<> timer;

    timer.Start();
    std::this_thread::sleep_for(sleep_time);
    timer.Stop();

    ASSERT_EQ(1, timer.TimerCount());
    ASSERT_LT(sleep_time / 2, timer.SumTime());
    ASSERT_GT(sleep_time * 3 / 2, timer.SumTime());
    ASSERT_EQ(timer.SumTime(), timer.MaxTime());
    ASSERT_EQ(timer.SumTime(), timer.MinTime());
    ASSERT_EQ(timer.AverageTime(), timer.SumTime());
}

TEST(EAPTimer, AverageSleep) {
    constexpr auto sleep_time = 10ms;
    constexpr auto loop_count = 10;

    constexpr auto total_sleep = sleep_time * loop_count;

    eap::perf::Timer<> timer;

    for (auto i = 0; i < 10; i++) {
        timer.Start();
        std::this_thread::sleep_for(sleep_time);
        timer.Stop();
    }

    ASSERT_EQ(loop_count, timer.TimerCount());

    ASSERT_LT(total_sleep / 2, timer.SumTime());
    ASSERT_GT(total_sleep * 3 / 2, timer.SumTime());

    ASSERT_LT(sleep_time / 2, timer.MaxTime());
    ASSERT_GT(sleep_time * 3 / 2, timer.MaxTime());

    ASSERT_LT(sleep_time / 2, timer.MinTime());
    ASSERT_GT(sleep_time * 3 / 2, timer.MinTime());

    ASSERT_LT(sleep_time / 2, timer.AverageTime());
    ASSERT_GT(sleep_time * 3 / 2, timer.AverageTime());
}

TEST(EAPTimer, ThrowOnStartWhileRunning) {
    eap::perf::Timer<> timer;
    timer.Start();
    ASSERT_THROW(timer.Start(), eap::perf::TimerAlreadyRunningException);
}

TEST(EAPTimer, ThrowOnStopWhileStopped) {
    eap::perf::Timer<> timer;
    ASSERT_THROW(timer.Stop(), eap::perf::TimerNotRunningException);
}