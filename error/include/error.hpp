/**
 * @file error.hpp
 *
 * @brief
 *  Umbrella header for error package. Includes all routines, types, and defines in the errors
 *  package.
 * @date 2019-02-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_ERROR_HPP_
#define EAP_ERROR_HPP_

#include "error-internal.hpp"
#include "error-macros.hpp"
#include "error-records.hpp"

namespace eap {
/**
 * @brief `eap::error` makes it easy to abort when an error is encountered while capturing as much
 * information about the failure as possible.
 */
namespace error {}
} // namespace eap

#endif // EAP_ERROR_HPP_