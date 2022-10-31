/**
 * @brief Exports exception types used by eap-perf
 *
 * @file perf-error.hpp
 *
 * @date 2018-09-05
 */

#ifndef EAP_PERF_ERROR_HPP
#define EAP_PERF_ERROR_HPP

#include "perf-internal-fwd.hpp"

#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <stdexcept>
#include <string>

namespace eap {
namespace perf {
class TimerException : public std::logic_error {
  public:
    /** @brief Returns a reference to the timer's name, if it has one */
    nonstd::optional<nonstd::string_view> timer_name() const {
        return timer_name_ ? nonstd::make_optional<nonstd::string_view>(timer_name_.value())
                           : nonstd::nullopt;
    }

  protected:
    /**
     * @brief Instantiates the exception for a timer with no name.
     */
    explicit TimerException(std::string const &what) : logic_error(what) {}

    /**
     * @brief Instantiates the exception for a timer with a name
     *
     * @param timer_name
     *  name of the Timer that failed
     * @param what
     *  The error message associated with the exception.
     */
    explicit TimerException(nonstd::string_view timer_name, std::string const &what)
        : logic_error(what), timer_name_(std::string(timer_name)) {}

  private:
    nonstd::optional<std::string> timer_name_;
};

/**
 * @brief Thrown when attempting to start a timer that is already running.
 */
class TimerAlreadyRunningException : public TimerException {
  public:
    /** @brief Instantiates the exception for a timer with no name. */
    TimerAlreadyRunningException();

    /** @brief Instantiates the exception for a timer with a name. */
    explicit TimerAlreadyRunningException(nonstd::string_view timer_name);

  private:
    static std::string get_named_what(nonstd::string_view timer_name);
};

/**
 * @brief Thrown when attempting to stop a timer that is already stopped.
 */
class TimerNotRunningException : public TimerException {
  public:
    /** @brief Instantiates the exception for a timer with no name. */
    TimerNotRunningException();

    /** @brief Instantiates the exception for a timer with a name. */
    explicit TimerNotRunningException(nonstd::string_view timer_name);

  private:
    static std::string get_named_what(nonstd::string_view timer_name);
};

} // namespace perf
} // namespace eap

#endif // EAP_PERF_ERROR_HPP