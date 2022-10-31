/**
 * @file abi-fortran_interop_impl.hpp
 *
 * @brief Implementation of functions defined by abi-fortran_interop.h
 * @date 2019-02-27
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_ABI_FORTRAN_INTEROP_IMPL_HPP_
#define EAP_ABI_FORTRAN_INTEROP_IMPL_HPP_

// Internal Includes
#include <abi-fortran_interop.h>
#include <error-macros.hpp>

#define FROM_HANDLE_INTERNAL_IMPL(package, class, CxxClass)                                        \
    namespace { /* anonymous namespace so these functions are only available in this file */       \
    CxxClass *FROM_HANDLE(package, class)(HANDLE_TYPE(package, class) * handle) {                  \
        return reinterpret_cast<CxxClass *>(handle);                                               \
    }                                                                                              \
    const CxxClass *FROM_HANDLE(package, class)(const HANDLE_TYPE(package, class) * handle) {      \
        return reinterpret_cast<const CxxClass *>(handle);                                         \
    }                                                                                              \
    } // namespace

#define FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(package, class, CxxClass)                         \
    namespace { /* anonymous namespace so these functions are only available in this file */       \
    CxxClass *FROM_HANDLE(package, class)(HANDLE_TYPE(package, class) * handle) {                  \
        return reinterpret_cast<CxxClass *>(handle);                                               \
    }                                                                                              \
    } // namespace

#define INTEROP_IMPL(package, c_type, CxxClass)                                                    \
    inline HANDLE_TYPE(mesh, c_type) * CxxClass##ToFFI(CxxClass *obj) {                            \
        return reinterpret_cast<HANDLE_TYPE(mesh, c_type) *>(obj);                                 \
    }                                                                                              \
    inline HANDLE_TYPE(mesh, c_type) const *CxxClass##ToFFI(CxxClass const *obj) {                 \
        return reinterpret_cast<HANDLE_TYPE(mesh, c_type) const *>(obj);                           \
    }                                                                                              \
    inline CxxClass *CxxClass##FromFFI(HANDLE_TYPE(mesh, c_type) * obj) {                          \
        return reinterpret_cast<CxxClass *>(obj);                                                  \
    }                                                                                              \
    inline CxxClass const *CxxClass##FromFFI(HANDLE_TYPE(mesh, c_type) const *obj) {               \
        return reinterpret_cast<CxxClass const *>(obj);                                            \
    }

#define FC_INTEROP_CREATE_C_IMPL(package, class, CONSTRUCTOR)                                      \
    EXTERN_C void CREATE_C(package, class)(HANDLE_TYPE(package, class) * *obj) {                   \
        EAP_EXTERN_PRE                                                                             \
        *obj = reinterpret_cast<HANDLE_TYPE(package, class) *>(new CONSTRUCTOR);                   \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_DELETE_C_IMPL(package, class)                                                   \
    EXTERN_C void DELETE_C(package, class)(HANDLE_TYPE(package, class) * *obj) {                   \
        EAP_EXTERN_PRE                                                                             \
        delete FROM_HANDLE(package, class)(*obj);                                                  \
        *obj = nullptr;                                                                            \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_WRAP_SCALAR_C_IMPL(package, class, field, TYPE)                                 \
    EXTERN_C void GET_FIELD_C(package, class, field)(HANDLE_TYPE(package, class) * obj,            \
                                                     TYPE * *field) {                              \
        EAP_EXTERN_PRE                                                                             \
        *field = (TYPE *)&FROM_HANDLE(package, class)(obj)->field();                               \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_WRAP_ARRAY_C_IMPL(package, class, field, TYPE)                                  \
    void GET_FIELD_C(package, class, field)(                                                       \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * size) {                        \
        EAP_EXTERN_PRE                                                                             \
        auto c_obj = FROM_HANDLE(package, class)(obj);                                             \
        *field = (TYPE *)c_obj->field().data();                                                    \
        *size = c_obj->field().size();                                                             \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_WRAP_1D_VIEW_C_IMPL(package, class, field, TYPE)                                \
    EXTERN_C void GET_FIELD_C(package, class, field)(                                              \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * n0) {                          \
        EAP_EXTERN_PRE                                                                             \
        auto c_obj = FROM_HANDLE(package, class)(obj);                                             \
        *field = (TYPE *)c_obj->field().data();                                                    \
        *n0 = c_obj->field().extent(0);                                                            \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_WRAP_2D_VIEW_C_IMPL(package, class, field, TYPE)                                \
    EXTERN_C void GET_FIELD_C(package, class, field)(                                              \
        HANDLE_TYPE(package, class) * obj, TYPE * *field, int64_t * n0, int64_t * n1) {            \
        EAP_EXTERN_PRE                                                                             \
        auto c_obj = FROM_HANDLE(package, class)(obj);                                             \
        *field = (TYPE *)c_obj->field().data();                                                    \
        *n0 = c_obj->field().extent(0);                                                            \
        *n1 = c_obj->field().extent(1);                                                            \
        EAP_EXTERN_POST                                                                            \
    }

#define FC_INTEROP_WRAP_3D_VIEW_C_IMPL(package, class, field, TYPE)                                \
    EXTERN_C void GET_FIELD_C(package, class, field)(HANDLE_TYPE(package, class) * obj,            \
                                                     TYPE * *field,                                \
                                                     int64_t * n0,                                 \
                                                     int64_t * n1,                                 \
                                                     int64_t * n2) {                               \
        EAP_EXTERN_PRE                                                                             \
        auto c_obj = FROM_HANDLE(package, class)(obj);                                             \
        *field = (TYPE *)c_obj->field().data();                                                    \
        *n0 = c_obj->field().extent(0);                                                            \
        *n1 = c_obj->field().extent(1);                                                            \
        *n2 = c_obj->field().extent(2);                                                            \
        EAP_EXTERN_POST                                                                            \
    }

#endif // EAP_ABI_FORTRAN_INTEROP_IMPL_HPP_