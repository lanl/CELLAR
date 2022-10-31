/**
 * @file utility-ffi-kokkos.cpp
 *
 * @brief Utilities for controlling Kokkos from C and Fortran
 * @date 2019-04-25
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <error-macros.hpp>
#include <utility-ffi-kokkos.h>

// Third Party Includes
#include <Kokkos_Core.hpp>

EXTERN_C void eap_utility_kokkos_initialize() {
    EAP_EXTERN_PRE
    Kokkos::initialize();
    EAP_EXTERN_POST
}

EXTERN_C void eap_utility_kokkos_finalize() {
    EAP_EXTERN_PRE
    Kokkos::finalize();
    EAP_EXTERN_POST
}
