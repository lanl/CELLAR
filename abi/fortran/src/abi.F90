module abi
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env

  implicit none
  private

  public :: &
    nd_array_t, &
    to_nd_array, &
    from_nd_array, &
    ND_ARRAY_MAX_RANK, &
    check_nd_array_layout

  integer, parameter :: ND_ARRAY_MAX_RANK = 8

  type, bind(C) :: nd_array_t
    integer(c_size_t) :: rank = 0
    integer(c_size_t) :: dims(ND_ARRAY_MAX_RANK) = 0
    integer(c_size_t) :: strides(ND_ARRAY_MAX_RANK) = 0
    type(c_ptr) :: data = c_null_ptr
  end type nd_array_t

  interface to_nd_array
    ! 1D specializations
    module procedure to_nd_array_l_1d
    module procedure to_nd_array_i32_1d
    module procedure to_nd_array_i64_1d
    module procedure to_nd_array_r32_1d
    module procedure to_nd_array_r64_1d

    ! 2D specializations
    module procedure to_nd_array_l_2d
    module procedure to_nd_array_i32_2d
    module procedure to_nd_array_i64_2d
    module procedure to_nd_array_r32_2d
    module procedure to_nd_array_r64_2d

    ! 3D specializations
    module procedure to_nd_array_l_3d
    module procedure to_nd_array_i32_3d
    module procedure to_nd_array_i64_3d
    module procedure to_nd_array_r32_3d
    module procedure to_nd_array_r64_3d

    ! 4D specializations
    module procedure to_nd_array_l_4d
    module procedure to_nd_array_i32_4d
    module procedure to_nd_array_i64_4d
    module procedure to_nd_array_r32_4d
    module procedure to_nd_array_r64_4d

    ! 5D specializations
    module procedure to_nd_array_l_5d
    module procedure to_nd_array_i32_5d
    module procedure to_nd_array_i64_5d
    module procedure to_nd_array_r32_5d
    module procedure to_nd_array_r64_5d

    ! 6D specializations
    module procedure to_nd_array_l_6d
    module procedure to_nd_array_i32_6d
    module procedure to_nd_array_i64_6d
    module procedure to_nd_array_r32_6d
    module procedure to_nd_array_r64_6d

    ! 7D specializations
    module procedure to_nd_array_l_7d
    module procedure to_nd_array_i32_7d
    module procedure to_nd_array_i64_7d
    module procedure to_nd_array_r32_7d
    module procedure to_nd_array_r64_7d
  end interface

  interface from_nd_array
    ! 1D specializations
    module procedure from_nd_array_l_1d
    module procedure from_nd_array_i32_1d
    module procedure from_nd_array_i64_1d
    module procedure from_nd_array_r32_1d
    module procedure from_nd_array_r64_1d

    ! 2D specializations
    module procedure from_nd_array_l_2d
    module procedure from_nd_array_i32_2d
    module procedure from_nd_array_i64_2d
    module procedure from_nd_array_r32_2d
    module procedure from_nd_array_r64_2d

    ! 3D specializations
    module procedure from_nd_array_l_3d
    module procedure from_nd_array_i32_3d
    module procedure from_nd_array_i64_3d
    module procedure from_nd_array_r32_3d
    module procedure from_nd_array_r64_3d

    ! 4D specializations
    module procedure from_nd_array_l_4d
    module procedure from_nd_array_i32_4d
    module procedure from_nd_array_i64_4d
    module procedure from_nd_array_r32_4d
    module procedure from_nd_array_r64_4d

    ! 5D specializations
    module procedure from_nd_array_l_5d
    module procedure from_nd_array_i32_5d
    module procedure from_nd_array_i64_5d
    module procedure from_nd_array_r32_5d
    module procedure from_nd_array_r64_5d

    ! 6D specializations
    module procedure from_nd_array_l_6d
    module procedure from_nd_array_i32_6d
    module procedure from_nd_array_i64_6d
    module procedure from_nd_array_r32_6d
    module procedure from_nd_array_r64_6d

    ! 7D specializations
    module procedure from_nd_array_l_7d
    module procedure from_nd_array_i32_7d
    module procedure from_nd_array_i64_7d
    module procedure from_nd_array_r32_7d
    module procedure from_nd_array_r64_7d
  end interface
contains
  subroutine check_nd_array_layout(ndarray)
    type(nd_array_t), intent(in) :: ndarray

    integer(c_size_t) :: n
    integer(c_size_t) :: current

    current = 1
    do n = 1,ndarray%rank
      if (current .ne. ndarray%strides(n)) then
        stop "Can only convert unstrided column-major nd_array layouts!"
      end if
      current = current * ndarray%dims(n)
    end do
  end subroutine check_nd_array_layout

  include "abi-to_ndarray_impls.f90"
end module abi