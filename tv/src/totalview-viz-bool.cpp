/**
 * @file totalview-viz-bool.cpp
 *
 * @brief C++View Type Transformations for bool views
 * @date 2019-07-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <totalview-viz.hpp>

template void eap::totalview::internal::MarkImplAsUsed<bool>();

EAP_TOTALVIEW_DEFINE_COMMON_FOR_VALUE_TYPE(bool);
