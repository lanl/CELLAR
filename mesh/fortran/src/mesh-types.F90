module mesh_core_types
  use, intrinsic :: iso_c_binding
  
  use abi

  implicit none
  private

  public :: &
    to_nd_array_lv, &
    from_nd_array_lv

  type, public, bind(C) :: loc_val_t
    integer(c_int32_t) :: loc
    integer(c_int32_t) :: val
  end type loc_val_t

  interface to_nd_array_lv
    module procedure to_nd_array_lv_1d
  end interface to_nd_array_lv

  interface from_nd_array_lv
    module procedure from_nd_array_lv_1d
  end interface from_nd_array_lv
contains
  include "mesh-to_ndarray_impls.f90"
end module mesh_core_types