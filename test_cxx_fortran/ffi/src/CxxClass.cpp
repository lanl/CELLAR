// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <abi.h>

// Local Includes
#include <test_cxx_fortran/CxxClass.hpp>

DECLARE_HANDLE_TYPE(test_cxx_fortran, cxxclass);

FROM_HANDLE_INTERNAL_IMPL(test_cxx_fortran, cxxclass, CxxClass)

EXTERN_C_BEGIN

FC_INTEROP_CREATE_C_IMPL(test_cxx_fortran, cxxclass, CxxClass())
FC_INTEROP_DELETE_C_IMPL(test_cxx_fortran, cxxclass)

FC_INTEROP_WRAP_SCALAR_C_IMPL(test_cxx_fortran, cxxclass, some_int, int)
FC_INTEROP_WRAP_2D_VIEW_C_IMPL(test_cxx_fortran, cxxclass, matrix, double)
FC_INTEROP_WRAP_3D_VIEW_C_IMPL(test_cxx_fortran, cxxclass, view3d, double)

void eap_test_cxx_fortran_cxxclass_resize(eap_test_cxx_fortran_cxxclass_t *obj, int *n0, int *n1) {
    FROM_HANDLE(test_cxx_fortran, cxxclass)(obj)->resize(*n0, *n1);
}

void eap_test_cxx_fortran_cxxclass_print(const eap_test_cxx_fortran_cxxclass_t *obj) {
    printf("cxxclass_print: %p\n", (void *)obj);
    FROM_HANDLE(test_cxx_fortran, cxxclass)(obj)->print();
}

EXTERN_C_END
