/**
 * @file abi-fortran_interop.h
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Support library for C++/Fortran Interop
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef ABI_FORTRAN_INTEROP_H
#define ABI_FORTRAN_INTEROP_H

// Standard Includes
#include <stdint.h>

// These macros correspond to the macros of the same name in fortran_interop.F90 and must be kept in
// sync
#define CREATE_C(package, class) eap_##package##_##class##_create
#define DELETE_C(package, class) eap_##package##_##class##_delete
#define GET_FIELD_C(package, class, field) eap_##package##_##class##_get_##field

#define HANDLE_TYPE(package, class) eap_##package##_##class##_t
#define DECLARE_HANDLE_TYPE(package, class)                                                        \
    typedef struct HANDLE_TYPE(package, class) HANDLE_TYPE(package, class)

#define FROM_HANDLE(package, class) eap_##package##_##class##_from_handle

#define FC_INTEROP_CREATE_C_DECL(package, class)                                                   \
    void CREATE_C(package, class)(HANDLE_TYPE(package, class) * *obj)

#define FC_INTEROP_DELETE_C_DECL(package, class)                                                   \
    void DELETE_C(package, class)(HANDLE_TYPE(package, class) * *obj)

// These macros implement a C function to get member variables of a class. It is assumed that the
// C++ class an accessor function that of the name field()
#define FC_INTEROP_WRAP_SCALAR_C_DECL(package, class, field, TYPE)                                 \
    void GET_FIELD_C(package, class, field)(HANDLE_TYPE(package, class) * obj, TYPE * *field)

#define FC_INTEROP_WRAP_ARRAY_C_DECL(package, class, field, TYPE)                                  \
    void GET_FIELD_C(package, class, field)(                                                       \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * size)

#define FC_INTEROP_WRAP_1D_VIEW_C_DECL(package, class, field, TYPE)                                \
    void GET_FIELD_C(package, class, field)(                                                       \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * n0)

#define FC_INTEROP_WRAP_2D_VIEW_C_DECL(package, class, field, TYPE)                                \
    void GET_FIELD_C(package, class, field)(                                                       \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * n0, int64_t * n1)

#define FC_INTEROP_WRAP_3D_VIEW_C_DECL(package, class, field, TYPE)                                \
    void GET_FIELD_C(package, class, field)(HANDLE_TYPE(package, class) * obj,                     \
                                            TYPE * *field,                                         \
                                            int64_t * n0,                                          \
                                            int64_t * n1,                                          \
                                            int64_t * n2)

#endif // ABI_FORTRAN_INTEROP_H