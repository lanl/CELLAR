!>
!! @file mesh-dzn.F90
!! 
!! @brief Fortran interface to DZN
!! @date 2019-08-08
!!
!! @copyright Copyright (C) 2019 Triad National Security, LLC

#include "abi-fortran_interop-f.h"

!> @brief
!!  Routines and types for managing "Defined Zone Resolution" regions and
!!  looking up which DZN matches locations
module mesh_dzn
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env

  use abi
  use mesh_cells, only : cpp_cells_t

  implicit none
  private

  public :: &
    dzn_t, &
    dzn_t_create, &
    dzn_split_filter_kode, &
    dzn_process_cells_in_dzn_filter_kode, &
    dzn_process_cells_in_dzn_filter_missing_material, &
    dzn_process_cells_in_dzn_filter_with_material

  type dzn_t
    integer :: numdzn
    integer, pointer, dimension(:) :: &
      options, levels, materials

    real(REAL64), pointer, dimension(:,:) :: &
      radiuses, radius_deltas, xs, x_deltas, ys, y_deltas, zs, z_deltas

    real(REAL64), pointer, dimension(:) :: sizes

    type(c_ptr) :: ptr = c_null_ptr
  contains
    final :: FREE(mesh,dzn)
    procedure :: is_associated => IS_ASSOCIATED(mesh,dzn)

    procedure :: push => dzn_t_push

    procedure, private :: update_fields => dzn_t_update_fields
  end type dzn_t

  type, bind(C) :: eap_mesh_dzn_fields_t
    integer(c_int) :: numdzn
    type(nd_array_t) :: &
      options, &
      levels, &
      materials, &
      radiuses, &
      radius_deltas, &
      xs, &
      x_deltas, &
      ys, &
      y_deltas, &
      zs, &
      z_deltas, &
      sizes
  end type eap_mesh_dzn_fields_t

  type, bind(C) :: eap_mesh_new_dzn_t
    integer(c_int) :: options = 0
    integer(c_int) :: level = 0
    integer(c_int) :: material = 0
    real(c_double), dimension(2) :: &
      r = 0, &
      rd = 0, &
      x = 0, &
      xd = 0, &
      y = 0, &
      yd = 0, &
      z = 0, &
      zd = 0
    real(c_double) :: size = 0
  end type eap_mesh_new_dzn_t

  interface
    FC_INTEROP_CONSTRUCTOR(mesh,dzn)
    FC_INTEROP_DESTRUCTOR(mesh,dzn)
    
    subroutine eap_mesh_dzn_get_fields(&
      dznffi, fields &
    ) bind(C, name="eap_mesh_dzn_get_fields")
      import

      type(c_ptr), value :: dznffi
      type(eap_mesh_dzn_fields_t), intent(inout) :: fields
    end subroutine eap_mesh_dzn_get_fields

    subroutine eap_mesh_dzn_push(&
      dznffi, newdznffi &
    ) bind(C,name="eap_mesh_dzn_push")
      import

      type(c_ptr), value :: dznffi
      type(eap_mesh_new_dzn_t), intent(in) :: newdznffi
    end subroutine eap_mesh_dzn_push

    subroutine eap_mesh_split_dzn_filter_kode(&
      dznffi, numdim, time, kode, nlow, nhigh, testx, testy, testz, level_base, level_set &
    ) bind(C, name="eap_mesh_split_dzn_filter_kode")
      import

      type(c_ptr), value :: dznffi
      integer(c_int), value :: numdim
      real(c_double), value :: time
      integer(c_int), value :: kode, nlow, nhigh
      real(c_double), value :: testx, testy, testz
      integer(c_int), value :: level_base
      integer(c_int), intent(inout) :: level_set
    end subroutine eap_mesh_split_dzn_filter_kode

    subroutine eap_mesh_process_cells_in_dzn_filter_kode(&
      dznffi, numdim, cells, time, kode, llow, lhigh, level_base, level_set &
    ) bind(C, name="eap_mesh_process_cells_in_dzn_filter_kode")
      import

      type(c_ptr), value :: dznffi
      integer(c_int), value :: numdim
      type(c_ptr), value :: cells
      real(c_double), value :: time
      integer(c_int), value :: kode, llow, lhigh
      type(nd_array_t), intent(in) :: level_base
      type(nd_array_t), intent(in) :: level_set
    end subroutine eap_mesh_process_cells_in_dzn_filter_kode

    subroutine eap_mesh_process_cells_in_dzn_filter_missing_material(&
      dznffi, numdim, cells, time, llow, lhigh, level_base, level_set &
    ) bind(C, name="eap_mesh_process_cells_in_dzn_filter_missing_material")
      import
  
      type(c_ptr), value :: dznffi
      integer(c_int), value :: numdim
      type(c_ptr), value :: cells
      real(c_double), value :: time
      integer(c_int), value :: llow, lhigh
      type(nd_array_t), intent(in) :: level_base
      type(nd_array_t), intent(in) :: level_set
    end subroutine eap_mesh_process_cells_in_dzn_filter_missing_material
  
    subroutine eap_mesh_process_cells_in_dzn_filter_with_material(&
      dznffi, numdim, cells, time, material, llow, lhigh, level_base, level_set &
    ) bind(C, name="eap_mesh_process_cells_in_dzn_filter_with_material")
      import
  
      type(c_ptr), value :: dznffi
      integer(c_int), value :: numdim
      type(c_ptr), value :: cells
      real(c_double), value :: time
      integer(c_int), value :: material, llow, lhigh
      type(nd_array_t), intent(in) :: level_base
      type(nd_array_t), intent(in) :: level_set
    end subroutine eap_mesh_process_cells_in_dzn_filter_with_material
  end interface
contains
  subroutine dzn_t_create(dzn)
    type(dzn_t) :: dzn

    call eap_mesh_dzn_create(dzn%ptr)
  end subroutine dzn_t_create

  subroutine FREE(mesh,dzn)(this)
    use iso_c_binding
    implicit none
    type(dzn_t), intent(inout) :: this
    if (this%is_associated()) call DELETE_C(mesh,dzn)(this%ptr)
    this%ptr = c_null_ptr
  end subroutine

  FC_INTEROP_IS_ASSOCIATED_IMPL(mesh,dzn)

  subroutine dzn_t_push(&
    self, options, level, material, r, rd, x, xd, y, yd, z, zd, size &
  )
    class(dzn_t), intent(inout) :: self
    integer, intent(in) :: options
    integer, intent(in), optional :: level, material
    real(REAL64), intent(in), optional, dimension(2) :: &
      r, rd, x, xd, y, yd, z, zd
    real(REAL64), intent(in), optional :: size

    type(eap_mesh_new_dzn_t) :: newdzn

    newdzn%options = options
    if (present(level)) newdzn%level = level
    if (present(material)) newdzn%material = material
    if (present(r)) newdzn%r = r
    if (present(rd)) newdzn%rd = rd
    if (present(x)) newdzn%x = x
    if (present(xd)) newdzn%xd = xd
    if (present(y)) newdzn%y = y
    if (present(yd)) newdzn%yd = yd
    if (present(z)) newdzn%z = z
    if (present(zd)) newdzn%zd = zd
    if (present(size)) newdzn%size = size

    call eap_mesh_dzn_push(self%ptr, newdzn)
    call self%update_fields()
  end subroutine dzn_t_push

  subroutine dzn_t_update_fields(self)
    class(dzn_t), intent(inout) :: self

    type(eap_mesh_dzn_fields_t) :: fields

    call eap_mesh_dzn_get_fields(self%ptr, fields)

    self%numdzn = fields%numdzn
    call from_nd_array(fields%options, self%options)
    call from_nd_array(fields%levels, self%levels)
    call from_nd_array(fields%materials, self%materials)
    call from_nd_array(fields%radiuses, self%radiuses)
    call from_nd_array(fields%radius_deltas, self%radius_deltas)
    call from_nd_array(fields%xs, self%xs)
    call from_nd_array(fields%x_deltas, self%x_deltas)
    call from_nd_array(fields%ys, self%ys)
    call from_nd_array(fields%y_deltas, self%y_deltas)
    call from_nd_array(fields%zs, self%zs)
    call from_nd_array(fields%z_deltas, self%z_deltas)
    call from_nd_array(fields%sizes, self%sizes)
  end subroutine dzn_t_update_fields

  subroutine dzn_split_filter_kode(&
    dzn, numdim, time, kode, nlow, nhigh, testx, testy, testz, level_base, level_set &
  )
    class(dzn_t) :: dzn
    integer :: numdim
    real(REAL64) :: time
    integer :: kode, nlow, nhigh
    real(REAL64) :: testx, testy, testz
    integer :: level_base
    integer, intent(inout) :: level_set

    call eap_mesh_split_dzn_filter_kode(&
      dzn%ptr, numdim, time, kode, nlow - 1, nhigh, testx, testy, testz, &
      level_base, level_set)
  end subroutine dzn_split_filter_kode

  subroutine dzn_process_cells_in_dzn_filter_kode(&
    dzn, numdim, cells, time, kode, llow, lhigh, level_base, level_set &
  )
    class(dzn_t), intent(in) :: dzn
    integer, intent(in) :: numdim
    type(cpp_cells_t), intent(in) :: cells
    real(REAL64), intent(in) :: time
    integer, intent(in) :: kode, llow, lhigh
    integer, intent(in) :: level_base(:)
    integer, intent(inout) :: level_set(:)

    call eap_mesh_process_cells_in_dzn_filter_kode(&
      dzn%ptr, numdim, cells%ptr, time, kode, llow - 1, lhigh, &
      to_nd_array(level_base), to_nd_array(level_set))
  end subroutine dzn_process_cells_in_dzn_filter_kode

  subroutine dzn_process_cells_in_dzn_filter_missing_material(&
    dzn, numdim, cells, time, llow, lhigh, level_base, level_set &
  )
    class(dzn_t), intent(in) :: dzn
    integer, intent(in) :: numdim
    type(cpp_cells_t), intent(in) :: cells
    real(REAL64), intent(in) :: time
    integer, intent(in) :: llow, lhigh
    integer, intent(in) :: level_base(:)
    integer, intent(inout) :: level_set(:)

    call eap_mesh_process_cells_in_dzn_filter_missing_material(&
      dzn%ptr, numdim, cells%ptr, time, llow - 1, lhigh, &
      to_nd_array(level_base), to_nd_array(level_set))
  end subroutine dzn_process_cells_in_dzn_filter_missing_material

  subroutine dzn_process_cells_in_dzn_filter_with_material(&
    dzn, numdim, cells, time, material, llow, lhigh, level_base, level_set &
  )
    class(dzn_t), intent(in) :: dzn
    integer, intent(in) :: numdim
    type(cpp_cells_t), intent(in) :: cells
    real(REAL64), intent(in) :: time
    integer, intent(in) :: material, llow, lhigh
    integer, intent(in) :: level_base(:)
    integer, intent(inout) :: level_set(:)

    call eap_mesh_process_cells_in_dzn_filter_with_material(&
      dzn%ptr, numdim, cells%ptr, time, material - 1, llow - 1, lhigh, &
      to_nd_array(level_base), to_nd_array(level_set))
  end subroutine dzn_process_cells_in_dzn_filter_with_material
end module mesh_dzn