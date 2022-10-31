/**
 * @file utility-ffi-kokkos.h
 *
 * @brief Routines for managing kokkos from a C or Fortran library
 * @date 2019-04-25
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_FFI_KOKKOS_H_
#define EAP_UTILITY_FFI_KOKKOS_H_

#include <abi-extern.h>

EXTERN_C_BEGIN

void eap_utility_kokkos_initialize();
void eap_utility_kokkos_finalize();

EXTERN_C_END

#endif // EAP_UTILITY_FFI_KOKKOS_H_