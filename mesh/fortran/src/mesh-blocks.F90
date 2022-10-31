module mesh_blocks
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env
  
  implicit none
  private

  public new_block_layout
  public block_layout_t

  public &
    blm_column_major, &
    blm_hilbert, &
    blm_column_major_zig_zag, &
    blm_column_major_reverse

  type :: block_layout_t
    type(c_ptr) :: layout = c_null_ptr
  contains
    procedure :: free => block_layout_t_free
    procedure :: is_associated => block_layout_t_is_associated

    procedure :: get_coordinates => block_layout_t_get_coordinates
    procedure :: get_address => block_layout_t_get_address
    end type block_layout_t

  enum, bind(C)
    enumerator :: &
      blm_column_major = 1, &
      blm_hilbert = 2, &
      blm_column_major_zig_zag = 3, &
      blm_column_major_reverse = 4
  end enum

  ! C Interop
  interface
    subroutine eap_mesh_block_layout_create(&
      method, extent0, extent1, extent2, layout) bind(C)
      use, intrinsic :: iso_c_binding

      integer(c_int), value, intent(in) :: method
      integer(c_int64_t), value, intent(in) :: extent0, extent1, extent2
      type(c_ptr), intent(out) :: layout
    end subroutine eap_mesh_block_layout_create

    subroutine eap_mesh_block_layout_free(layout) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: layout
    end subroutine eap_mesh_block_layout_free

    subroutine eap_mesh_block_layout_get_coordinates(&
      layout, address, coordinate0, coordinate1, coordinate2) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: layout
      integer(c_int64_t), value, intent(in) :: address
      integer(c_int64_t), intent(out) :: coordinate0, coordinate1, coordinate2
    end subroutine eap_mesh_block_layout_get_coordinates

    subroutine eap_mesh_block_layout_get_address(&
      layout, coordinate0, coordinate1, coordinate2, address) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: layout
      integer(c_int64_t), value, intent(in) :: &
        coordinate0, coordinate1, coordinate2
      integer(c_int64_t), intent(out) :: address
    end subroutine eap_mesh_block_layout_get_address
  end interface
contains
  type(block_layout_t) function new_block_layout(&
    method, extent0, extent1, extent2)
    integer(c_int), intent(in) :: method
    integer(INT64), intent(in) :: extent0, extent1, extent2
    
    call eap_mesh_block_layout_create(&
      method, extent0, extent1, extent2, new_block_layout%layout)
  end function new_block_layout

  subroutine block_layout_t_free(layout)
    class(block_layout_t), intent(inout) :: layout

    if (layout%is_associated()) call eap_mesh_block_layout_free(layout%layout)
    layout%layout = c_null_ptr
  end subroutine block_layout_t_free

  logical function block_layout_t_is_associated(layout)
    class(block_layout_t), intent(in) :: layout

    block_layout_t_is_associated = c_associated(layout%layout)
  end function block_layout_t_is_associated

  subroutine block_layout_t_get_coordinates(&
    layout, address, coordinate0, coordinate1, coordinate2)
    class(block_layout_t), intent(in) :: layout
    integer(INT64), intent(in) :: address
    integer(INT64), intent(out) :: coordinate0, coordinate1, coordinate2

    call eap_mesh_block_layout_get_coordinates(&
      layout%layout, address - 1, coordinate0, coordinate1, coordinate2)

    coordinate0 = coordinate0 + 1
    coordinate1 = coordinate1 + 1
    coordinate2 = coordinate2 + 1
  end subroutine block_layout_t_get_coordinates

  subroutine block_layout_t_get_address(&
    layout, coordinate0, coordinate1, coordinate2, address)
    class(block_layout_t), intent(in) :: layout
    integer(INT64), intent(in) :: coordinate0, coordinate1, coordinate2
    integer(INT64), intent(out) :: address

    call eap_mesh_block_layout_get_address(&
      layout%layout, coordinate0 - 1, coordinate1 - 1, coordinate2 - 1, address)

    address = address + 1
  end subroutine block_layout_t_get_address
end module mesh_blocks