#include "abi-fortran_interop-f.h"

module test_cxx_fortran
  use iso_c_binding
  implicit none

  public :: cxxclass_t

  type cxxclass_t
    integer(c_int32_t), pointer :: some_int => null()
    real(c_double), pointer :: matrix(:,:) => null()

    ! internal pointer that points to the C instance
    type(c_ptr), private :: ptr = c_null_ptr

    contains
      procedure :: free => FREE(test_cxx_fortran,cxxclass)
      procedure :: is_associated => IS_ASSOCIATED(test_cxx_fortran,cxxclass)

      procedure :: resize => eap_test_cxx_fortran_cxxclass_t_resize
      procedure :: print => eap_test_cxx_fortran_cxxclass_t_print

  end type cxxclass_t

  ! bindings to C functions
  interface
    ! IMPORTANT: NO SPACE AFTER COMMA
    FC_INTEROP_CONSTRUCTOR(test_cxx_fortran,cxxclass) ! create_cxxclass_c
    FC_INTEROP_DESTRUCTOR(test_cxx_fortran,cxxclass)  ! delete_cxxclass_c

    FC_INTEROP_WRAP_SCALAR(test_cxx_fortran,cxxclass,some_int)  ! cxxclass_get_some_int_c
    FC_INTEROP_WRAP_2D_VIEW(test_cxx_fortran,cxxclass,matrix)   ! cxxclass_get_matrix_c
    FC_INTEROP_WRAP_3D_VIEW(test_cxx_fortran,cxxclass,view3d)   ! cxxclass_get_view3d_c

    subroutine eap_test_cxx_fortran_cxxclass_resize(cxxclass, n0, n1) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cxxclass
      integer(c_int32_t), intent(in) :: n0, n1
    end subroutine

    subroutine eap_test_cxx_fortran_cxxclass_print(cxxclass) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cxxclass
    end subroutine
  end interface

contains

  function eap_test_cxx_fortran_new_cxxclass_t() result(new)
    implicit none
    type(cxxclass_t) :: new

    ! create C instance
    call eap_test_cxx_fortran_cxxclass_create(new%ptr)

    ! assign pointers
    new%some_int => GET_FIELD(test_cxx_fortran,cxxclass,some_int)(new)
    new%matrix => GET_FIELD(test_cxx_fortran,cxxclass,matrix)(new)
  end function

  ! IMPORTANT: NO SPACE AFTER COMMAS
  FC_INTEROP_FREE_IMPL(test_cxx_fortran,cxxclass)
  FC_INTEROP_IS_ASSOCIATED_IMPL(test_cxx_fortran,cxxclass)

  FC_INTEROP_WRAP_SCALAR_IMPL(test_cxx_fortran,cxxclass,some_int,integer(c_int32_t))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(test_cxx_fortran,cxxclass,matrix,real(c_double))
  FC_INTEROP_WRAP_3D_VIEW_IMPL(test_cxx_fortran,cxxclass,view3d,real(c_double))

  subroutine eap_test_cxx_fortran_cxxclass_t_resize(this, n0, n1)
    implicit none
    class(cxxclass_t), intent(inout) :: this
    integer(c_int32_t), intent(in) :: n0, n1
    call eap_test_cxx_fortran_cxxclass_resize(this%ptr, n0, n1)
    this%matrix => GET_FIELD(test_cxx_fortran,cxxclass,matrix)(this)
  end subroutine

  subroutine eap_test_cxx_fortran_cxxclass_t_print(this)
    implicit none
    class(cxxclass_t), intent(in) :: this
    call eap_test_cxx_fortran_cxxclass_print(this%ptr)
  end subroutine

end module test_cxx_fortran
