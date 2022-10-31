/**
 * @brief Implements a registry for timers. This registry allows timers to only be stopped in the
 *  reverse order in which they were started.
 *
 * @file perf-registry.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_REGISTRY_HPP
#define EAP_PERF_REGISTRY_HPP

#include "perf-internal-fwd.hpp"
#include "perf-internal-sys.hpp"

#include "perf-clock.hpp"
#include "perf-error.hpp"
#include "perf-timer.hpp"

#include <iostream>
#include <iterator>
#include <nonstd/string_view.hpp>
#include <stack>
#include <vector>

namespace eap {
namespace perf {

/**
 * @brief A handle object that can be used to interact with Timer objects in the TimerRegistry.
 *
 * @details Any TimerHandle retrieved from a TimerRegistry object is guaranteed to be valid for the
 *  lifetime of TimerRegistry. Using a TimerHandle with another TimerRegistry is undefined behavior.
 */
class TimerHandle {
    template <typename Clock>
    friend class TimerRegistry;
    friend class TimerRegistryIterator;

  public:
    /** @brief Swaps this TimerHandle with the handle in other */
    void swap(TimerHandle &other) { std::swap(index_, other.index_); }

    /** @brief Returns true if this TimerHandle and other are the same */
    bool operator==(TimerHandle const &other) const { return index_ == other.index_; }

    /** @brief Returns true if this TimerHandle and other are different */
    bool operator!=(TimerHandle const &other) const { return index_ != other.index_; }

  private:
    explicit TimerHandle(std::size_t index) : index_(index) {}

    std::size_t index_;
};

/** @brief Swap the TimerHandle objects in a and b */
inline void swap(TimerHandle &a, TimerHandle &b) { a.swap(b); }

/**
 * @brief TimerRegistryIterator is a C++ STL BidirectionalIterator that iterates over all
 *  TimerHandle objects in the Registry.
 */
class TimerRegistryIterator {
    friend class TimerRegistryRange;

  public:
    // Iterator Requirements - https://en.cppreference.com/w/cpp/named_req/Iterator
    TimerRegistryIterator(TimerRegistryIterator const &) = default;
    TimerRegistryIterator &operator=(TimerRegistryIterator const &) = default;
    TimerRegistryIterator(TimerRegistryIterator &&) = default;
    TimerRegistryIterator &operator=(TimerRegistryIterator &&) = default;
    ~TimerRegistryIterator() = default;

    void swap(TimerRegistryIterator &other) { std::swap(idx_, other.idx_); }

    using value_type = TimerHandle;
    using difference_type = std::ptrdiff_t;
    using reference = TimerHandle const &;
    using pointer = TimerHandle const *;
    using iterator_category = std::bidirectional_iterator_tag;

    reference operator*() const { return idx_; }

    TimerRegistryIterator &operator++() {
        ++idx_.index_;
        return *this;
    }

    // InputIterator requirements - https://en.cppreference.com/w/cpp/named_req/InputIterator
    bool operator==(TimerRegistryIterator const &other) const { return idx_ == other.idx_; }
    bool operator!=(TimerRegistryIterator const &other) const { return idx_ != other.idx_; }

    pointer operator->() const { return &idx_; }

    TimerRegistryIterator operator++(int) { return TimerRegistryIterator(idx_.index_++); }

    // BidirectionalIterator requirements -
    // https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
    TimerRegistryIterator &operator--() {
        --idx_.index_;
        return *this;
    }

    TimerRegistryIterator operator--(int) { return TimerRegistryIterator(idx_.index_--); }

  private:
    explicit TimerRegistryIterator(std::size_t index) : idx_(index){};
    TimerHandle idx_;
};

/** @brief Swaps the TimerRegistryIterator objects a and b */
inline void swap(TimerRegistryIterator &a, TimerRegistryIterator &b) { a.swap(b); }

/**
 * @brief A C++ STL Range that represents the set of all TimerHandle objects in a TimerRegistry.
 */
class TimerRegistryRange {
    template <typename Clock>
    friend class TimerRegistry;

  public:
    TimerRegistryIterator begin() const { return TimerRegistryIterator(0); }
    TimerRegistryIterator end() const { return TimerRegistryIterator(size_); }

  private:
    explicit TimerRegistryRange(std::size_t size) : size_(size) {}
    std::size_t size_;
};

/**
 * @brief A collection of associated timers that can be started and tracked simultaneously.
 *
 * @details
 *  TimerRegistry track timer runs using a stack. Ergo, timers must be popped in the reverse order
 *  of the order they were pushed on the stack. Only the outermost (i.e. bottom timer on the stack)
 *  is actually started when its pushed onto an empty stack and stopped when it is popped (making
 *  the stack empty again). Even though additional timers pushed onto a non-empty stack won't be
 *  started or stopped, the stack exists to allow timers to be pushed onto the stack inside
 *  functions that may or may not be called when the stack is empty. If the function is called when
 *  the stack in the TimerRegistry is empty, that function will be timed. Otherwise, pushing that
 *  timer onto the stack inside the function has no effect and the function will not be timed
 *  directly, because it is being timed as part of the outermost timer that was pushed onto the
 *  stack at an earlier point.
 *
 *  It is recommended to always use \ref TimedSection produced by the \ref TimeSection methods, as
 *  they will automatically pop timers from the stack in the reverse order of which they were
 *  pushed.
 *
 *  TimerRegistry can use any clock that implements the C++ STL
 *  [TrivialClock](https://en.cppreference.com/w/cpp/named_req/TrivialClock) interface. By default,
 *  TimerRegistry uses eap::perf::DefaultClock. You can use the default clock by declaring your
 *  TimerRegistry value as TimerRegistry<>.
 *
 *  TimerRegistry is not thread-safe, and its stack-based implementation does not make it amenable
 *  for use across multiple threads. If you want to time operations across multiple threads,
 *  then your best option is to create a TimerRegistry per-thread and accumulate the results at the
 *  end of the run.
 *
 * @tparam DefaultClock
 *  The C++ STL [TrivialClock](https://en.cppreference.com/w/cpp/named_req/TrivialClock) used by
 *  TimerRegistry.
 */
template <typename Clock = DefaultClock>
class TimerRegistry {
  public:
    /**
     * @brief The C++ STL [TrivialClock](https://en.cppreference.com/w/cpp/named_req/TrivialClock)
     *  used by this TimerRegistry.
     */
    using clock = Clock;

    /**
     * @brief Converts a human-readable timer name into a TimerHandle. If a timer has not been
     *  previously registered with that name, the timer is registered.
     *
     * @param timer_name
     *  Human readable label for timer
     * @return TimerHandle
     *  a handle to the Timer object managed by TimerRegistry
     */
    TimerHandle InsertOrLookupTimer(nonstd::string_view timer_name) {
        auto const it = std::find_if(timers_.cbegin(), timers_.cend(), [&](auto const &timer) {
            return timer.first == timer_name;
        });

        if (it != timers_.cend()) {
            return TimerHandle(it - timers_.cbegin());
        } else {
            timers_.push_back(std::make_pair(nonstd::to_string(timer_name), Timer<Clock>()));
            return TimerHandle(timers_.size() - 1);
        }
    }

    /**
     * @brief Returns a reference to the timer name associated with index
     *
     * @param index
     *  Handle to Timer in Registry. Behavior undefined if handle came from different TimerRegistry.
     * @return nonstd::string_view
     *  An immutable reference to the name of the timer associated with index
     */
    nonstd::string_view GetTimerName(TimerHandle index) const {
        return timers_[index.index_].first;
    }

    /**
     * @brief Gets an immutable reference to the Timer associated with index
     *
     * @param index
     *  Handle to Timer in Registry. Behavior undefined if handle came from different TimerRegistry.
     * @return Timer<Clock> const&
     *  Immutable reference to timer associated with index. Used to retrieve statistics.
     */
    Timer<Clock> const &GetTimer(TimerHandle index) const { return timers_[index.index_].second; }

    /**
     * @brief Pushes a timer onto the current timer stack, starting it if the stack is empty.
     *
     * @details
     *  TimerRegistry will only track time for the outermost timer, which is the bottom timer in
     *  the stack.
     *
     * @param index
     *  Handle to Timer in Registry. Behavior undefined if handle came from different TimerRegistry.
     */
    void PushTimer(TimerHandle index) {
        if (current_timers_.empty()) {
            auto &timer_registration = timers_.at(index.index_);

            auto &timer = timer_registration.second;
            if (timer.IsRunning()) {
                throw TimerAlreadyRunningException(timer_registration.first);
            }

            timer.Start();
        }

        current_timers_.push(index);
    }

    /**
     * @brief Pops the top-most timer from the current timer stack, stopping the timer if it's the
     *  last timer.
     *
     * @details
     *  Terminates the program if the user attempted to a pop a timer from an empty Timer stack.
     *
     * @return TimerHandle
     *  A handle to the timer popped from the stack.
     */
    TimerHandle PopTimer() {
        if (current_timers_.empty()) {
            std::cerr << "eap::perf::TimerRegistry::PopTimer(): Unbalanced timer push and pop!"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        auto const index = current_timers_.top();
        if (current_timers_.size() == 1) {
            timers_[index.index_].second.Stop();
        }
        current_timers_.pop();
        return index;
    }

    /**
     * @brief Pushes a timer to the timer stack, then pops it when the TimedSection object is
     *  destroyed
     *
     * @details
     *  You must assign the return value to a member, otherwise the timer will start and immediately
     *  stop.
     *
     *  The specified timer is only started if the current stack is empty, i.e. if there isn't
     *  another outer timer or TimedSection.
     *
     *  E.g.
     *  ```cpp
     *  // this section untimed
     *  {
     *      // this section untimed
     *      auto timed = registry.TimeSection(handle);
     *      // this section timed (provided there isn't another outer timer that
     *      // already times this section)
     *  }
     *  // this section untimed
     *  ```
     *
     *  TimedSection behaves similarly to
     *  [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock).
     *
     * @param index
     *  Timer to run
     *
     * @return TimedSection<Clock>
     *  An RAII-style object that stops the timer when it goes out of scope.
     */
    TimedSection<Clock> TimeSection(TimerHandle index) {
        PushTimer(index);
        return TimedSection<Clock>(*this, index);
    }

    /**
     * @brief Overload of TimeSection(TimerHandle) taking a string timer name instead
     *
     * @details
     *  See TimeSection(TimerHandle)
     *
     * @param timer_name
     *  Name of the timer to use
     */
    TimedSection<Clock> TimeSection(nonstd::string_view timer_name) {
        return TimeSection(InsertOrLookupTimer(timer_name));
    }

    /**
     * @brief Returns a Range over the TimerHandle objects in the TimerRegistry
     *
     * @details
     *  Example Usage:
     *  ```cpp
     *  for (auto handle : registry.Timers()) {
     *      nonstd::string_view const name = registry.GetTimerName(handle);
     *      eap::perf::Timer const &timer = registry.GetTimer(handle);
     *  }
     *  ```
     *
     * @return TimerRegistryRange
     *  A C++ STL Range over TimerHandle objects
     */
    TimerRegistryRange Timers() const { return TimerRegistryRange(timers_.size()); }

    /**
     * @brief Returns the number of timers currently registered with the TimerRegistry.
     *
     * @return std::size_t
     *  The number of timers
     */
    std::size_t Count() const { return timers_.size(); }

  private:
    std::vector<std::pair<std::string, Timer<clock>>> timers_;
    std::stack<TimerHandle> current_timers_;
};

/**
 * @brief RAII-style class that runs a timer until it leaves scope.
 *
 * @details
 *  TimedSection behaves similarly to
 *  [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock).
 *
 * @tparam DefaultClock
 *  The Clock type used by the matching TimerRegistry.
 */
template <typename Clock = DefaultClock>
class EAP_PERF_MUST_USE TimedSection {
    template <typename>
    friend class TimerRegistry;

  public:
    ~TimedSection() {
        if (is_active()) {
            auto const popped = registry_->PopTimer();

            if (popped != timer_) {
                std::cerr << "eap::perf::TimedSection::~TimedSection: Tried to pop timer '"
                          << registry_->GetTimerName(timer_) << "' while '"
                          << registry_->GetTimerName(popped)
                          << "' was on the top of the timer stack!" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    TimedSection(TimedSection const &) = delete;
    TimedSection &operator=(TimedSection const &) = delete;

    TimedSection(TimedSection &&other) : timer_(std::move(other.timer_)) {
        std::swap(registry_, other.registry_);
    }

    TimedSection &operator=(TimedSection &&other) {
        timer_ = std::move(other.timer_);
        std::swap(registry_, other.registry_);
        return *this;
    }

  private:
    TimedSection(TimerRegistry<Clock> &registry, TimerHandle timer)
        : registry_(&registry), timer_(timer) {}

    bool is_active() const { return registry_ != nullptr; }

    TimerRegistry<Clock> *registry_ = nullptr;
    TimerHandle timer_;
};

extern template class TimerRegistry<DefaultClock>;

} // namespace perf
} // namespace eap

#endif // EAP_PERF_REGISTRY_HPP