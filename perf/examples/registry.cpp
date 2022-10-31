#include <chrono>
#include <thread>

#include <perf-registry.hpp>
#include <utility-diagnostic.hpp>

using namespace std::chrono;   // import common chrono names
using eap::perf::DefaultClock; // For accessing the default "duration" typename

// Most applications will declare a global variable for their timer registry, and may declare
// multiple timer registries for different domains.
//
// A `TimerRegistry` can be declared in smaller scopes if desired.
eap::perf::TimerRegistry<> timer_registry;

// Forward declaring our computations.
void foo();
void bar();
void zap();

void foo() {
    // "foo timer" starts when 'TimeSection' is called, and stops when
    // 'foo_timer' goes out of scope.
    //
    // Since "foo timer" is the outermost timer, its time will be recorded, whereas
    // the inner times will not be tracked.
    eap::perf::TimedSection<> foo_timer = timer_registry.TimeSection("foo timer");
    bar();
}

void bar() {
    // You can also register timers manually using "InsertOrLookupTimer". This will return a
    // TimerHandle object that can be then used manually with TimerRegistry<>::PushTimer.
    eap::perf::TimerHandle bar_timer_handle = timer_registry.InsertOrLookupTimer("bar timer");

    timer_registry.PushTimer(bar_timer_handle);

    zap();

    // 'TimerRegistry<Clock>::PopTimer' returns the 'TimerHandle' of the top (innermost) timer in
    // the stack.
    auto const popped_timer = timer_registry.PopTimer();
    assert(popped_timer == bar_timer_handle);
    MARK_UNUSED(popped_timer);
}

void zap() {
    // 'TimerHandle's are valid for the lifetime of a 'TimerRegistry', and so they can be stored
    // across timer runs.
    static auto const zap_timer_handle = timer_registry.InsertOrLookupTimer("zap timer");

    // 'TimerRegistry<>::TimeSection` is overloaded for both `string_view` and `TimerHandle`.
    //
    // It is recommended to just use "auto" with TimeSection so that the Clock can be changed
    // without having to change all call-sites.
    auto const zap_timer = timer_registry.TimeSection(zap_timer_handle);
}

int main() {
    // Run our computation many times.
    for (auto i = 0; i < 1000; i++) {
        foo();
    }

    // Run an internal computation, too
    for (auto i = 0; i < 1000; i++) {
        bar();
    }

    // Dump timer information
    for (auto const handle : timer_registry.Timers()) {
        eap::perf::Timer<> const &timer = timer_registry.GetTimer(handle);
        auto const sum_time_ns = duration_cast<nanoseconds>(timer.SumTime());

        std::cout << "Timer: " << timer_registry.GetTimerName(handle)
                  << ", count = " << timer.TimerCount() << ", sum_time = " << sum_time_ns.count()
                  << " ns";

        if (nonstd::optional<DefaultClock::duration> const avg_time = timer.AverageTime()) {
            auto const avg_time_ns = duration_cast<nanoseconds>(avg_time.value());

            std::cout << ", avg_time = " << avg_time_ns.count() << " ns";
        }

        std::cout << std::endl;
    }
}