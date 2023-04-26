#include <perf-error.hpp>
#include <sstream>
#include <string>
#include <utility>

using std::string_view;
using std::move;
using std::string;
using std::stringstream;

eap::perf::TimerAlreadyRunningException::TimerAlreadyRunningException()
    : TimerException("Tried to start timer when it was already running.") {}

eap::perf::TimerAlreadyRunningException::TimerAlreadyRunningException(string_view timer_name)
    : TimerException(timer_name, get_named_what(timer_name)) {}

string eap::perf::TimerAlreadyRunningException::get_named_what(string_view timer_name) {
    stringstream ss;
    ss << "Tried to start timer '" << timer_name << "' when it was already running.";
    return ss.str();
}

eap::perf::TimerNotRunningException::TimerNotRunningException()
    : TimerException("Tried to stop timer when it was not running.") {}

eap::perf::TimerNotRunningException::TimerNotRunningException(string_view timer_name)
    : TimerException(timer_name, get_named_what(timer_name)) {}

string eap::perf::TimerNotRunningException::get_named_what(string_view timer_name) {
    stringstream ss;
    ss << "Tried to stop timer '" << timer_name << "' when it was not running.";
    return ss.str();
}
