/**
 * @file error-records.cpp
 *
 * @brief Manages error records and propagation/printing.
 * @date 2019-02-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self include
#include <error-records.hpp>

// STL Includes
#include <iostream>

// Third Party includes
#include <Kokkos_Core.hpp>
#include <mpi/mpi.hpp>
#include <optional>

std::ostream &eap::error::operator<<(std::ostream &os, eap::error::Location const &location) {
    os << location.file << ":" << location.line << " (" << location.func << ")";
    return os;
}

char const *eap::error::PropagatedError::what() const noexcept {
    if (!what_) {
        // Lazily creates what_
        auto &what = *const_cast<std::optional<std::string> *>(&what_);

        std::stringstream ss;
        ss << "EAP Error:";
        PrintStackTrace(ss);

        what = ss.str();

        return what.value().c_str();
    } else {
        return what_.value().c_str();
    }
}

void eap::error::PropagatedError::PrintStackTrace(std::ostream &stream) const noexcept {
    std::exception const *ex = this;
    while (ex) {
        stream << "\n\t";
        if (auto prop_ex = dynamic_cast<PropagatedError const *>(ex)) {

            stream << prop_ex->location() << " -> Failed";
            if (prop_ex->statement_) {
                stream << " (" << *prop_ex->statement_ << ")";
            }
            stream << ": " << prop_ex->msg_;
            ex = prop_ex->exception_.get();
        } else {
            stream << ex->what();
            ex = nullptr;
        }
    }
}

void eap::error::internal::PropagateException(::eap::error::Location const &location,
                                              std::optional<std::string_view> statement,
                                              std::string &&message) {
#define PROPAGATE_EXCEPTION(exception_type)                                                        \
    catch (exception_type const &err) {                                                            \
        throw ::eap::error::PropagatedError(location, err, statement, std::move(message));         \
    }

    try {
        throw;
    }
    PROPAGATE_EXCEPTION(mpi::Exception)
    PROPAGATE_EXCEPTION(std::bad_optional_access)
    PROPAGATE_EXCEPTION(eap::error::RaisedError)
    PROPAGATE_EXCEPTION(eap::error::PropagatedError)
    PROPAGATE_EXCEPTION(std::bad_alloc)
    PROPAGATE_EXCEPTION(std::length_error)
    PROPAGATE_EXCEPTION(std::runtime_error)
    PROPAGATE_EXCEPTION(std::exception)

#undef PROPAGATE_EXCEPTION
}

void eap::error::internal::AbortWithException(::eap::error::Location const &location) noexcept {
    // This function should only be called in a catch(...) block!
    try {
        // Wraps another PropagatedError to get the current file/func/line
        PropagateException(location, std::nullopt, "Uncaught C++ exception at FFI Boundary");
    } catch (eap::error::PropagatedError const &err) {
        std::cerr << "ERROR: Terminating ";
        if (mpi::initialized()) {
            std::cerr << "(rank = " << mpi::Comm::world().rank() << ") ";
        }
        std::cerr << "due to C++ Exception at C++/FFI/Fortran Boundary:";
        err.PrintStackTrace(std::cerr);
        std::cerr << std::endl;
    } catch (...) {
        std::cerr
            << "ERROR: Terminating due to unrecognized C++ Exception at C++/FFI/Fortran Boundary"
            << std::endl;
    }

    if (Kokkos::is_initialized()) {
        Kokkos::finalize();
    }

    if (mpi::initialized()) {
        mpi::Comm::world().abort(EXIT_FAILURE);
    } else {
        std::exit(EXIT_FAILURE);
    }
}