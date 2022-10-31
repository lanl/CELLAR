/**
 * @file totalview-viz-i32.cpp
 *
 * @brief C++View Type Transformations for int32_t views
 * @date 2019-07-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <totalview-viz.hpp>

// STL Includes
#include <cstdint>

template void eap::totalview::internal::MarkImplAsUsed<std::int32_t>();

EAP_TOTALVIEW_DEFINE_COMMON_FOR_VALUE_TYPE(std::int32_t);
