#include <chrono>
#include <perf-timer.hpp>

using std::chrono::steady_clock;

int main() {
    // eap::perf::Timer<> is equivalent to eap::perf::Timer<steady_clock>
    eap::perf::Timer<steady_clock> timer;

    for (auto i = 0; i < 100; i++) {
        timer.Start();
        // some expensive operation
        timer.Stop();
    }

    // avg_time, min_time, and max_time return values only when there is at least one time sample.
    if (std::optional<steady_clock::duration> avg_time = timer.AverageTime()) {
        // do something with avg_time
    }
}