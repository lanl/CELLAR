#include "abi-fortran_interop-f.h"

module mesh_cells
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env

  use token, only : token_builder_t

  implicit none
  private

  public :: &
    cpp_cells_t, &
    eap_mesh_new_cpp_cells_t

  !=================================================================================================
  !=================================================================================================
  !===
  !===
  !===  WHEN REMOVING LEGACY MESH CODE, CHANGE THE FOLLOWING HERE:
  !===
  !===  - rename cpp_cells_t to cells_t (and don't make it a derived type)
  !===  - rename all fields from cpp_* to just *
  !===  - rename all functions/occurrances of eap_mesh_cpp_cells to eap_mesh_cells
  !===  - rename all occurrances of cpp_cells to cells in macros
  !===
  !===
  !=================================================================================================
  !=================================================================================================

  type cpp_cells_t
    ! For documentation of these member variables, see the eap::mesh::Cells C++ class header file.
    ! We use the same names as in xRage here, but the corresponding variable names in C++ are
    ! slightly different to be more descriptive.
    !
    ! All of these integer types are unsigned in C++, but Fortran doesn't have unsigned integers,
    ! so they're signed here.

    integer(c_int64_t), pointer :: cpp_sum_numcell => null()    ! num_global_cells_
    integer(c_int32_t), pointer :: cpp_numcell => null()        ! num_local_cells_
    integer(c_int32_t), pointer :: cpp_max_numcell => null()    ! max_num_local_cells_
    integer(c_int32_t), pointer :: cpp_numcell_clone => null()  ! num_local_cells_with_clones
    integer(c_int32_t), pointer :: cpp_mxcell => null()         ! num_local_reserve_cells_

    integer(c_int32_t), pointer, dimension(:)   :: cpp_global_numcell => null() ! all_num_local_cells_
    integer(c_int64_t), pointer, dimension(:)   :: cpp_global_base => null()    ! global_base_address_
    integer(c_int64_t), pointer, dimension(:)   :: cpp_cell_address => null()   ! cell_address_
    logical(c_bool),    pointer, dimension(:)   :: cpp_cell_active => null()    ! cell_active_
    real(c_double),     pointer, dimension(:,:) :: cpp_cell_center => null()    ! cell_center_
    real(c_double),     pointer, dimension(:,:) :: cpp_cell_position => null()  ! cell_position_
    real(c_double),     pointer, dimension(:,:) :: cpp_cell_half => null()      ! cell_half
    real(c_double),     pointer, dimension(:,:) :: cpp_cell_half_lo => null()   ! cell_half_lo
    real(c_double),     pointer, dimension(:,:) :: cpp_cell_half_hi => null()   ! cell_half_hi
    real(c_double),     pointer, dimension(:)   :: cpp_vcell => null()          ! cell_volume_

    ! Internal pointer that points to the C instance
    type(c_ptr) :: ptr = c_null_ptr

    contains
      ! IMPORTANT: NO SPACE AFTER COMMA
      procedure :: free => FREE(mesh,cpp_cells)
      procedure :: is_associated => IS_ASSOCIATED(mesh,cpp_cells)

      procedure :: to_local => eap_mesh_cpp_cells_t_to_local
      procedure :: to_global => eap_mesh_cpp_cells_t_to_global
      procedure :: set_sum_numcell => eap_mesh_cpp_cells_t_set_num_global_cells

      procedure :: cell_arrays_allocated => eap_mesh_cpp_cells_t_cell_arrays_allocated
      procedure :: global_arrays_allocated => eap_mesh_cpp_cells_t_global_arrays_allocated

      procedure :: set_num_local_cells => eap_mesh_cpp_cells_t_set_num_local_cells
      procedure :: update_global_base => eap_mesh_cpp_cells_t_update_global_base
      procedure :: prepare_global_base => eap_mesh_cpp_cells_t_prepare_global_base
      procedure :: check_global_base => eap_mesh_cpp_cells_t_check_global_base
      procedure :: resize_cell_arrays => eap_mesh_cpp_cells_t_resize_cell_arrays

  end type cpp_cells_t

  interface
    ! bindings to C functions
    ! IMPORTANT: NO SPACE AFTER COMMA
    subroutine CREATE_C(mesh,cpp_cells)(token_builder, obj) &
      CREATE_BIND(mesh,cpp_cells)
      use iso_c_binding
      implicit none
      type(c_ptr), value, intent(in) :: token_builder
      type(c_ptr), intent(inout) :: obj
    end subroutine CREATE_C(mesh,cpp_cells)

    FC_INTEROP_DESTRUCTOR(mesh,cpp_cells)

    FC_INTEROP_WRAP_SCALAR(mesh,cpp_cells,num_global_cells)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_cells,num_local_cells)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_cells,max_num_local_cells)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_cells,num_local_cells_with_clones)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_cells,num_local_reserve_cells)

    FC_INTEROP_WRAP_1D_VIEW(mesh,cpp_cells,all_num_local_cells)
    FC_INTEROP_WRAP_1D_VIEW(mesh,cpp_cells,global_base_address)
    FC_INTEROP_WRAP_1D_VIEW(mesh,cpp_cells,cell_address)
    FC_INTEROP_WRAP_1D_VIEW(mesh,cpp_cells,cell_active)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_cells,cell_center)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_cells,cell_position)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_cells,cell_half)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_cells,cell_half_lo)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_cells,cell_half_hi)
    FC_INTEROP_WRAP_1D_VIEW(mesh,cpp_cells,cell_volume)

    pure function eap_mesh_cpp_cells_cell_arrays_allocated(cells) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      logical(c_bool) :: eap_mesh_cpp_cells_cell_arrays_allocated
    end function

    pure function eap_mesh_cpp_cells_global_arrays_allocated(cells) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      logical(c_bool) :: eap_mesh_cpp_cells_global_arrays_allocated
    end function

    subroutine eap_mesh_cpp_cells_set_num_local_cells(cells, numcell) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      integer(c_int32_t), intent(in), value :: numcell
    end subroutine

    subroutine eap_mesh_cpp_cells_update_global_base(cells) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
    end subroutine

    subroutine eap_mesh_cpp_cells_prepare_global_base(cells, numcell) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      integer(c_int32_t), intent(in), value :: numcell
    end subroutine

    subroutine eap_mesh_cpp_cells_check_global_base(cells) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
    end subroutine

    subroutine eap_mesh_cpp_cells_resize_cell_arrays(cells, size, num_dims) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      integer(c_int32_t), intent(in) :: size, num_dims
    end subroutine

    pure function eap_mesh_cpp_cells_to_local(cells, global_index) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      integer(c_int64_t), intent(in) :: global_index
      integer(c_int32_t) :: eap_mesh_cpp_cells_to_local
    end function

    pure function eap_mesh_cpp_cells_to_global(cells, local_index) bind(c)
      use iso_c_binding
      implicit none
      type(c_ptr), intent(in), value :: cells
      integer(c_int32_t), intent(in) :: local_index
      integer(c_int64_t) :: eap_mesh_cpp_cells_to_global
    end function

  end interface

contains

  function eap_mesh_new_cpp_cells_t(token_builder) result(new)
    implicit none
    type(token_builder_t), intent(in) :: token_builder
    type(cpp_cells_t) :: new

    ! create C instance
    call CREATE_C(mesh,cpp_cells)(token_builder%builder, new%ptr)

    ! assign pointers
    new%cpp_sum_numcell     => GET_FIELD(mesh,cpp_cells,num_global_cells)(new)
    new%cpp_numcell         => GET_FIELD(mesh,cpp_cells,num_local_cells)(new)
    new%cpp_max_numcell     => GET_FIELD(mesh,cpp_cells,max_num_local_cells)(new)
    new%cpp_numcell_clone   => GET_FIELD(mesh,cpp_cells,num_local_cells_with_clones)(new)
    new%cpp_mxcell          => GET_FIELD(mesh,cpp_cells,num_local_reserve_cells)(new)

    ! global_numcell and global_base are 0-indexed
    new%cpp_global_numcell(0:)  => GET_FIELD(mesh,cpp_cells,all_num_local_cells)(new)
    new%cpp_global_base(0:)     => GET_FIELD(mesh,cpp_cells,global_base_address)(new)

    new%cpp_cell_address    => GET_FIELD(mesh,cpp_cells,cell_address)(new)
    new%cpp_cell_active     => GET_FIELD(mesh,cpp_cells,cell_active)(new)
    new%cpp_cell_center     => GET_FIELD(mesh,cpp_cells,cell_center)(new)
    new%cpp_cell_position   => GET_FIELD(mesh,cpp_cells,cell_position)(new)
    new%cpp_cell_half       => GET_FIELD(mesh,cpp_cells,cell_half)(new)
    new%cpp_cell_half_lo    => GET_FIELD(mesh,cpp_cells,cell_half_lo)(new)
    new%cpp_cell_half_hi    => GET_FIELD(mesh,cpp_cells,cell_half_hi)(new)
    new%cpp_vcell           => GET_FIELD(mesh,cpp_cells,cell_volume)(new)

  end function eap_mesh_new_cpp_cells_t

  ! IMPORTANT: NO SPACE AFTER COMMA
  FC_INTEROP_FREE_IMPL(mesh,cpp_cells)
  FC_INTEROP_IS_ASSOCIATED_IMPL(mesh,cpp_cells)

  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_cells,num_global_cells,integer(c_int64_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_cells,num_local_cells,integer(c_int32_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_cells,max_num_local_cells,integer(c_int32_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_cells,num_local_cells_with_clones,integer(c_int32_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_cells,num_local_reserve_cells,integer(c_int32_t))

  FC_INTEROP_WRAP_1D_VIEW_IMPL(mesh,cpp_cells,all_num_local_cells,integer(c_int32_t))
  FC_INTEROP_WRAP_1D_VIEW_IMPL(mesh,cpp_cells,global_base_address,integer(c_int64_t))
  FC_INTEROP_WRAP_1D_VIEW_IMPL(mesh,cpp_cells,cell_address,integer(c_int64_t))
  FC_INTEROP_WRAP_1D_VIEW_IMPL(mesh,cpp_cells,cell_active,logical(c_bool))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_cells,cell_center,real(c_double))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_cells,cell_position,real(c_double))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_cells,cell_half,real(c_double))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_cells,cell_half_lo,real(c_double))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_cells,cell_half_hi,real(c_double))
  FC_INTEROP_WRAP_1D_VIEW_IMPL(mesh,cpp_cells,cell_volume,real(c_double))

  pure function eap_mesh_cpp_cells_t_cell_arrays_allocated(this) result(ret)
    implicit none
    class(cpp_cells_t), intent(in) :: this
    logical(c_bool) :: ret
    ret = eap_mesh_cpp_cells_cell_arrays_allocated(this%ptr)
  end function

  pure function eap_mesh_cpp_cells_t_global_arrays_allocated(this) result(ret)
    implicit none
    class(cpp_cells_t), intent(in) :: this
    logical(c_bool) :: ret
    ret = eap_mesh_cpp_cells_global_arrays_allocated(this%ptr)
  end function

  subroutine eap_mesh_cpp_cells_t_set_num_local_cells(this, numcell)
    class(cpp_cells_t), intent(inout) :: this
    integer(INT32), intent(in) :: numcell

    call eap_mesh_cpp_cells_set_num_local_cells(this%ptr, numcell)
  end subroutine eap_mesh_cpp_cells_t_set_num_local_cells

  subroutine eap_mesh_cpp_cells_t_update_global_base(this)
    class(cpp_cells_t), intent(inout) :: this

    call eap_mesh_cpp_cells_update_global_base(this%ptr)
  end subroutine eap_mesh_cpp_cells_t_update_global_base

  subroutine eap_mesh_cpp_cells_t_prepare_global_base(this, numcell)
    class(cpp_cells_t), intent(inout) :: this
    integer(INT32), intent(in) :: numcell

    call eap_mesh_cpp_cells_prepare_global_base(this%ptr, numcell)
  end subroutine eap_mesh_cpp_cells_t_prepare_global_base

  subroutine eap_mesh_cpp_cells_t_check_global_base(this)
    class(cpp_cells_t), intent(inout) :: this

    call eap_mesh_cpp_cells_check_global_base(this%ptr)
  end subroutine eap_mesh_cpp_cells_t_check_global_base

  subroutine eap_mesh_cpp_cells_t_resize_cell_arrays(this, size, num_dims)
    implicit none
    class(cpp_cells_t), intent(inout) :: this
    integer(INT32), intent(in) :: size, num_dims
    call eap_mesh_cpp_cells_resize_cell_arrays(this%ptr, size, num_dims)

    this%cpp_cell_address   => GET_FIELD(mesh,cpp_cells,cell_address)(this)
    this%cpp_cell_active    => GET_FIELD(mesh,cpp_cells,cell_active)(this)
    this%cpp_cell_center    => GET_FIELD(mesh,cpp_cells,cell_center)(this)
    this%cpp_cell_position  => GET_FIELD(mesh,cpp_cells,cell_position)(this)
    this%cpp_cell_half      => GET_FIELD(mesh,cpp_cells,cell_half)(this)
    this%cpp_cell_half_lo   => GET_FIELD(mesh,cpp_cells,cell_half_lo)(this)
    this%cpp_cell_half_hi   => GET_FIELD(mesh,cpp_cells,cell_half_hi)(this)
    this%cpp_vcell          => GET_FIELD(mesh,cpp_cells,cell_volume)(this)
  end subroutine

  pure function eap_mesh_cpp_cells_t_to_local(this, global_index) result(ret)
    use iso_c_binding
    implicit none
    class(cpp_cells_t), intent(in) :: this
    integer(INT64), intent(in) :: global_index
    integer(INT32) :: ret
    ret = eap_mesh_cpp_cells_to_local(this%ptr, global_index)
  end function

  pure function eap_mesh_cpp_cells_t_to_global(this, local_index) result(ret)
    use iso_c_binding
    implicit none
    class(cpp_cells_t), intent(in) :: this
    integer(INT32), intent(in) :: local_index
    integer(INT64) :: ret
    ret = eap_mesh_cpp_cells_to_global(this%ptr, local_index)
  end function

  pure subroutine eap_mesh_cpp_cells_t_set_num_global_cells(this, value)
    use iso_c_binding
    implicit none
    class(cpp_cells_t), intent(inout) :: this
    integer(INT64), intent(in) :: value
    this%cpp_sum_numcell = value
  end subroutine

end module mesh_cells
