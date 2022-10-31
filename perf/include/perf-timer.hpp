/**
 * @brief Implements a timer type that tracks runtime of some computation and maintains running
 *  statistics, including min, max, sum, and average.
 *
 * @file perf-timer.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_TIMER_HPP
#define EAP_PERF_TIMER_HPP

#include "perf-internal-fwd.hpp"

#include "perf-clock.hpp"
#include "perf-error.hpp"

#include <nonstd/optional.hpp>
#include <stdexcept>

namespace eap {
namespace perf {
/**
 * @brief
 *  An object for tracking the runtime of a computation and keeping statistics.
 *
 * @tparam Clock
 *  A [C++-compatible TrivialClock](https://en.cppreference.com/w/cpp/named_req/TrivialClock) to be
 *  used by the Timer.
 */
template <typename Clock = DefaultClock>
class Timer {
  public:
    using clock = Clock;
    using duration = typename clock::duration;
    using time_point = typename clock::time_point;

    /**
     * @brief Put the Timer into the running state.
     *
     * @details
     *  Captures the current Clock time to track the total Timer runtime. If the timer is already
     *  running, then throws TimerAlreadyRunningException.
     */
    void Start() {
        if (start_time_) {
            throw TimerAlreadyRunningException();
        }

        start_time_ = clock::now();
    }

    /**
     * @brief Stops the timer.
     *
     * @details
     *  Captures the current Clock time and updates statistics about the Timer run-time.
     *  If the timer is not running, then throws TimerNotRunningException.
     */
    void Stop() {
        if (!start_time_) {
            throw TimerNotRunningException();
        }

        auto const time = clock::now() - start_time_.value();

        start_time_ = nonstd::nullopt;

        timer_count_ += 1;
        sum_time_ += time;

        if (min_time_) {
            min_time_ = std::min(min_time_.value(), time);
            max_time_ = std::max(max_time_.value(), time);
        } else {
            assert(!max_time_);
            min_time_ = time;
            max_time_ = time;
        }
    }

    /**
     * @brief Get the number of times Timer has been run.
     *
     * @return Number of computations
     */
    size_t TimerCount() const { return timer_count_; }

    /**
     * @brief Gets the total amount of time the Timer was running.
     *
     * @return std::chrono::duration object that accumulates all Timer runtimes
     */
    duration SumTime() const { return sum_time_; }

    /**
     * @brief Gets the average runtime of the Timer runs.
     *
     * @return Optional std::chrono::duration object. Returns the average run-time if the Timer has
     *  been run at least once. Otherwise returns nonstd::nullopt.
     */
    nonstd::optional<duration> AverageTime() const {
        return timer_count_ == 0 ? nonstd::nullopt
                                 : nonstd::make_optional<duration>(sum_time_ / timer_count_);
    }

    /**
     * @brief Get the longest Timer runtime.
     *
     * @return Optional std::chrono::duration object. Returns the longest run-time if the Timer has
     *  been run at least once. Otherwise returns nonstd::nullopt.
     */
    nonstd::optional<duration> MaxTime() const { return max_time_; }

    /**
     * @brief Get the shortest Timer runtime
     *
     * @return Optional std::chrono::duration object. Returns the shortest run-time if the Timer has
     *  been run at least once. Otherwise returns nonstd::nullopt.
     */
    nonstd::optional<duration> MinTime() const { return min_time_; }

    /**
     * @brief Returns true if the Timer is running.
     *
     * @return true - Timer is running
     * @return false - Timer is not running
     */
    bool IsRunning() const { return start_time_.has_value(); }

  private:
    std::size_t timer_count_ = 0;
    duration sum_time_{0};
    nonstd::optional<duration> min_time_;
    nonstd::optional<duration> max_time_;

    nonstd::optional<time_point> start_time_;
};

} // namespace perf
} // namespace eap

#endif // EAP_PERF_TIMER_HPP