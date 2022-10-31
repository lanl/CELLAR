/**
 * @file error-records.hpp
 *
 * @brief Defines error types
 * @date 2019-02-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_ERROR_RECORDS_HPP_
#define EAP_ERROR_RECORDS_HPP_

// STL Includes
#include <exception>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

// Third Party includes
#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>

namespace eap {
namespace error {
/**
 * @brief Describes a file location in EAP Core
 */
struct Location {
    // Strings are stored in global memory
    nonstd::string_view file;
    nonstd::string_view func;
    size_t line;
};

/**
 * @brief Writes location to output stream
 */
std::ostream &operator<<(std::ostream &os, Location const &location);

/**
 * @brief Base error for eap Errors. Cannot be directly constructed.
 */
class Error : public std::exception {
  public:
    Location const &location() const { return location_; }

  protected:
    explicit Error(::eap::error::Location const &location) : location_(location) {}

    // Lists move/copy operations as protected so they can't be directly invoked.
    Error(Error const &) = default;
    Error &operator=(Error const &) = default;

    Error(Error &&) = default;
    Error &operator=(Error &&) = default;

  private:
    ::eap::error::Location location_;
};

/**
 * @brief
 *  Error represents an unrecoverable error in EAP. Used to propagate debugging information up
 *  the stack.
 */
class RaisedError : public eap::error::Error {
  public:
    RaisedError(::eap::error::Location const &location, std::string &&msg) : Error(location) {
        std::stringstream eap_error_internal_ss;
        eap_error_internal_ss << this->location() << " -> " << std::move(msg);
        what_ = eap_error_internal_ss.str();
    }

    const char *what() const noexcept override { return what_.c_str(); }

  private:
    std::string what_;
};

/**
 * @brief
 *  Error propagates additional exceptions up from a lower level, with more information on how to
 *  resolve the issue.
 */
class PropagatedError : public eap::error::Error {
  public:
    template <typename T>
    PropagatedError(::eap::error::Location const &location,
                    T const &exception,
                    nonstd::optional<nonstd::string_view> statement,
                    std::string &&msg)
        : Error(location),
          exception_(std::make_shared<T>(exception)),
          msg_(std::move(msg)),
          statement_(statement) {}

    // Accessors
    char const *what() const noexcept final;
    nonstd::string_view msg() const noexcept { return msg_; }
    std::exception const *exception() const { return exception_.get(); }

    // Methods
    void PrintStackTrace(std::ostream &stream) const noexcept;

  private:
    // This must be a shared_ptr to make the exception copyable, as is required.
    std::shared_ptr<std::exception> exception_;
    nonstd::optional<std::string> what_;
    std::string msg_;
    // stored in global memory - string literal
    nonstd::optional<nonstd::string_view> statement_;
};

namespace internal {
/**
 * @brief Re-throws the "current" exception (via re-throw) with additional context. Should not use
 * directly
 *
 * @param location
 * @param statement
 * @param message
 */
[[noreturn]] void PropagateException(::eap::error::Location const &location,
                                     nonstd::optional<nonstd::string_view> statement,
                                     std::string &&message);

[[noreturn]] void AbortWithException(::eap::error::Location const &location) noexcept;
} // namespace internal
} // namespace error
} // namespace eap

#endif // EAP_ERROR_RECORDS_HPP_