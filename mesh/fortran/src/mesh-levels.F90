#include "abi-fortran_interop-f.h"

module mesh_levels
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env

  use abi, only : nd_array_t, from_nd_array, to_nd_array
  use comm_types, only : ct_int32, ct_double
  use mesh_cells, only : cpp_cells_t
  use mesh_core_types, only : loc_val_t, to_nd_array_lv

  implicit none
  private

  public :: new_cpp_levels

  !=================================================================================================
  !=================================================================================================
  !===
  !===
  !===  WHEN REMOVING LEGACY MESH CODE, CHANGE THE FOLLOWING HERE:
  !===
  !===  - rename cpp_levels_t to levels_t (and don't make it a derived type)
  !===  - rename all fields from cpp_* to just *
  !===  - rename all functions/occurrances of eap_mesh_cpp_levels to eap_mesh_levels
  !===  - rename all occurrances of cpp_levels to levels in macros
  !===
  !===
  !=================================================================================================
  !=================================================================================================

  type, public :: cpp_levels_t
    integer, dimension(:), pointer :: &
      cell_level => null() ! levels_t::cell_level

    real(REAL64), dimension(:), pointer :: &
      area => null(), & ! levels_t::area
      gdxx => null(), & ! levels_t::gdxx
      dxyzmn => null(), & ! levels_t::dxyzmn
      dtsize => null() ! levels_t::dtsize

    real(REAL64), dimension(:,:), pointer :: &
      sides => null(), & ! levels_t::sides
      sizes => null(), & ! levels_t::sizes
      gsizes => null() ! levels_t::gsizes

    integer, dimension(:), pointer :: levelmx => null() ! levels_t::levelmx

    integer, dimension(:), pointer :: flag => null() ! levels_t::flag
    integer, dimension(:), pointer :: flag_tag => null() ! levels_t::flag_tag
    integer, dimension(:), pointer :: amr_tag => null() ! levels_t::amr_tag

    integer :: numtop ! levels_t::numtop
    integer :: allnumtop = 0 ! levels_t::allnumtop

    integer(INT64) :: sum_numtop ! levels_t::sum_numtop

    integer :: chunk_count ! levels_t::chunk_count
    real(REAL64) :: average_run_length ! levels_t::average_run_length
    integer, dimension(:), pointer :: &
      chunk_start => null(), & ! levels_t::chunk_start
      chunk_end => null(), & ! levels_t::chunk_end
      chunk_ids => null() ! levels_t::chunk_ids

    integer :: &
      numpack, & ! levels_t::numpack
      max_numpack ! levels_t::max_numpack

    integer(INT64), dimension(:), pointer :: &
      cell_mother, & ! levels_t::cell_mother
      cell_daughter ! levels_t::cell_daughter

    integer, dimension(:), pointer :: &
      ltop => null(), & ! levels_t::ltop
      alltop => null() ! levels_t::alltop

    ! Internal pointer to eap_mesh_cpp_levels_t
    type(c_ptr), private :: ptr = c_null_ptr
  contains
    procedure :: free => FREE(mesh,cpp_levels)
    procedure :: is_associated => IS_ASSOCIATED(mesh,cpp_levels)

    procedure :: initialize => cpp_levels_t_initialize ! levels_t::init
    procedure :: initialize_level1 => &
      cpp_levels_t_initialize_level1 ! levels_t::initialize_level1

    procedure :: build_pack => cpp_levels_t_build_pack ! levels_t::bldpck
    procedure :: build_top => cpp_levels_t_build_top ! levels_t::bldtop

    procedure :: ltop_nv => cpp_levels_t_ltop_nv ! levels_t::ltop_nv
    procedure :: alltop_nv => cpp_levels_t_alltop_nv ! levels_t::alltop_nv

    ! levels_t::reset_mothers_and_daughters
    procedure :: reset_mothers_and_daughters => &
      cpp_levels_t_reset_mothers_and_daughters
    ! levels_t::reset_mothers_and_daughters_new
    procedure :: reset_mothers_and_daughters_new => &
      cpp_levels_t_reset_mothers_and_daughters_new

    procedure :: setup_alltop => &
      cpp_levels_t_setup_alltop ! levels_t::setup_alltop

    procedure :: cell_mother_set => &
      cpp_levels_t_cell_mother_set ! levels_t::cell_mother_set
    procedure :: cell_daughter_set => &
      cpp_levels_t_cell_daughter_set ! levels_t::cell_daughter_set

    procedure :: levelmx_set => &
      cpp_levels_t_levelmx_set ! levels_t::levelmx_set
    procedure :: setall_levelmx => &
      cpp_levels_t_setall_levelmx ! levels_t::setall_levelmx

    procedure :: resize_local => cpp_levels_t_resize_local ! levels_t::resize
    ! TODO: levels_t::clone

    procedure :: recon_move => cpp_levels_t_recon_move ! levels_t::recon_move

    procedure :: foreach_at_level => &
      cpp_levels_t_foreach_at_level ! levels_t::foreach_at_level
    procedure :: map_at_level => &
      cpp_levels_t_map_at_level ! levels_t::map_at_level
    procedure :: filter_at_level => &
      cpp_levels_t_filter_at_level ! levels_t::filter_at_level
    procedure :: collect_at_level => &
      cpp_levels_t_collect_at_level ! levels_t::collect_at_level
    procedure :: clear_at_level => &
      cpp_levels_t_clear_at_level ! levels_t::clear_at_level
    procedure :: num_at_level => &
      cpp_levels_t_num_at_level ! levels_t::num_at_level

    procedure :: recon_pack => cpp_levels_t_recon_pack ! levels_t::recpack

    procedure :: set_flags => cpp_levels_t_set_flags ! levels_t::set_flags
    procedure :: set_flags_scalar => &
      cpp_levels_t_set_flags_scalar ! levels_t::set_flags_const
    procedure :: set_flags_all => &
      cpp_levels_t_set_flags_all ! levels_t::set_flags_all
    procedure :: set_flags_scalar_when => &
      cpp_levels_t_set_flags_scalar_when ! levels_t::set_flags_when_const
    procedure :: set_flags_scalar_from_list => &
      cpp_levels_t_set_flags_scalar_from_list ! levels_t::set_flags_from_list_const
    procedure :: set_flags_scalar_and_tags_from_list => &
      cpp_levels_t_set_flags_scalar_and_tags_from_list ! levels_t::set_flags_from_list_const
    procedure :: set_flags_from_tuples => &
      cpp_levels_t_set_flags_from_tuples ! levels_t::set_flags_from_tuples
    procedure :: set_flag_tags_from_tuples => &
      cpp_levels_t_set_flag_tags_from_tuples ! levels_t::set_flag_tags_from_tuples
    procedure :: set_flags_from_tuples_key_and_scalar => &
      cpp_levels_t_set_flags_from_tuples_key_and_scalar ! levels_t::set_flags_from_tuples_key_const
    procedure :: set_flag_tags_from_tuples_key_and_scalar => &
      cpp_levels_t_set_flag_tags_from_tuples_key_and_scalar ! levels_t::set_flag_tags_from_tuples_key_const

    procedure :: mom_kid_build => &
      cpp_levels_t_mom_kid_build ! levels_t::mom_kid_build
    procedure :: mom_kid_free => &
      cpp_levels_t_mom_kid_free ! levels_t::mom_kid_free
    procedure :: kid_mom_build => &
      cpp_levels_t_kid_mom_build ! levels_t::kid_mom_build
    procedure :: mom_kids_build => &
      cpp_levels_t_mom_kids_build ! levels_t::mom_kids_build

    procedure :: check_lohipack_and_levels => &
      cpp_levels_t_check_lohipack_and_levels ! levels_t::check_lohipack_and_levels
    ! TODO: levels_t::update_cell_index
    ! TODO: levels_t::check

    procedure :: recon_div_parent => &
      cpp_levels_t_recon_div_parent ! levels_t::recon_div_parent
    procedure :: recon_div_child => &
      cpp_levels_t_recon_div_child ! levels_t::recon_div_child

    procedure :: clear_daughter => &
      cpp_levels_t_clear_daughter ! levels_t::clear_daughter

    generic :: kid_get => &
      cpp_levels_t_kid_get_i32, & ! levels_t::kid_get_i4
      cpp_levels_t_kid_get_r64 ! levels_t::kid_get_r8

    procedure, private :: &
      cpp_levels_t_kid_get_i32, &
      cpp_levels_t_kid_get_r64

    generic :: kid_put => &
      cpp_levels_t_kid_put_i32, & ! levels_t::kid_put_i4
      cpp_levels_t_kid_put_r64, & ! levels_t::kid_put_r8
      cpp_levels_t_kid_put_r64_2d ! levels_t::kid_put_r8_v

    procedure, private :: &
      cpp_levels_t_kid_put_i32, &
      cpp_levels_t_kid_put_r64, &
      cpp_levels_t_kid_put_r64_2d

    generic :: kid_put_inv => &
      cpp_levels_t_kid_put_inv_r64_2d ! levels_t::kid_put_inv

    procedure, private :: &
      cpp_levels_t_kid_put_inv_r64_2d

    generic :: mom_get => &
      cpp_levels_t_mom_get_r64 ! levels_t::mom_get_r8

    procedure, private :: &
      cpp_levels_t_mom_get_r64

    procedure :: set_num_pack => &
      cpp_levels_t_set_num_pack ! levels_t::set_numpack_i
    procedure :: set_level_max_from_tuples => &
      cpp_levels_t_set_level_max_from_tuples ! levels_t::set_levelmx_from_tuples
    ! TODO: levels_t::set_amr_tags_from_rmlist

    procedure :: apply_dzns => cpp_levels_t_apply_dzns
  end type cpp_levels_t

  type, public, bind(C) :: cpp_levels_options_t
    integer(c_int8_t) :: num_dim
    real(c_double) :: cell_size(3)
    logical(c_bool) :: kid_mom_use_s2s
    logical(c_bool) :: mom_kid_use_s2s
    logical(c_bool) :: mom_kids_use_s2s
  end type cpp_levels_options_t

  type, bind(C) :: eap_mesh_cpp_levels_resize_local_fields
    type(nd_array_t) :: &
      cell_daughter, &
      cell_mother, &
      ltop, &
      cell_level, &
      flag, &
      flag_tag
  end type eap_mesh_cpp_levels_resize_local_fields

  type, bind(C) :: eap_mesh_cpp_levels_setup_all_top_fields
    type(nd_array_t) :: &
      all_top
  end type eap_mesh_cpp_levels_setup_all_top_fields

  interface
    ! bindings to C functions
    ! IMPORTANT: NO SPACE AFTER COMMA
    FC_INTEROP_CONSTRUCTOR(mesh,cpp_levels)
    FC_INTEROP_DESTRUCTOR(mesh,cpp_levels)

    subroutine eap_mesh_cpp_levels_initialize(levs, opts) &
      bind(C,name="eap_mesh_cpp_levels_initialize")
      use, intrinsic :: iso_c_binding
      import cpp_levels_options_t

      type(c_ptr), value :: levs
      type(cpp_levels_options_t), intent(in) :: opts
    end subroutine eap_mesh_cpp_levels_initialize

    subroutine eap_mesh_cpp_levels_initialize_level1(levs) &
      bind(C,name="eap_mesh_cpp_levels_initialize_level1")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: levs
    end subroutine eap_mesh_cpp_levels_initialize_level1

    subroutine eap_mesh_cpp_levels_build_pack(levs, cells) &
      bind(C,name="eap_mesh_cpp_levels_build_pack")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_build_pack

    subroutine eap_mesh_cpp_levels_build_top(levs, cells) &
      bind(C,name="eap_mesh_cpp_levels_build_top")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_build_top

    subroutine eap_mesh_cpp_levels_reset_mothers_and_daughters(levs, cells) &
      bind(C,name="eap_mesh_cpp_levels_reset_mothers_and_daughters")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_reset_mothers_and_daughters

    subroutine eap_mesh_cpp_levels_reset_mothers_and_daughters_new(&
      levs, cells, new_cells &
    ) bind(C,name="eap_mesh_cpp_levels_reset_mothers_and_daughters_new")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: levs, cells
      integer(c_int32_t), value :: new_cells
    end subroutine eap_mesh_cpp_levels_reset_mothers_and_daughters_new

    subroutine eap_mesh_cpp_levels_setup_all_top(levs, cells, fields) &
      bind(C,name="eap_mesh_cpp_levels_setup_all_top")
      use, intrinsic :: iso_c_binding
      import

      type(c_ptr), value :: levs, cells
      type(eap_mesh_cpp_levels_setup_all_top_fields), intent(inout) :: fields
    end subroutine eap_mesh_cpp_levels_setup_all_top

    subroutine eap_mesh_cpp_levels_resize_local(&
      levs, num_cells, newsize, fields &
    ) bind(C,name="eap_mesh_cpp_levels_resize_local")
      use, intrinsic :: iso_c_binding
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: num_cells
      integer(c_int32_t), value :: newsize
      type(eap_mesh_cpp_levels_resize_local_fields), intent(inout) :: fields
    end subroutine eap_mesh_cpp_levels_resize_local

    subroutine eap_mesh_cpp_levels_recon_move_f(&
      levs, data_length, length, send_start, send_length, recv_start, &
      recv_length &
    ) bind(C, name="eap_mesh_cpp_levels_recon_move_f")
      use, intrinsic :: iso_c_binding
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: data_length
      integer(c_size_t), value :: length
      integer(c_int32_t), dimension(*) :: &
        send_start, send_length, recv_start, recv_length
    end subroutine eap_mesh_cpp_levels_recon_move_f

    subroutine eap_mesh_cpp_levels_foreach_at_level(&
      levs, level, context, f &
    ) bind(C, name = "eap_mesh_cpp_levels_foreach_at_level")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(c_ptr), value :: context
      interface
        subroutine f(context, l) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: l
        end subroutine f
      end interface
    end subroutine eap_mesh_cpp_levels_foreach_at_level

    subroutine eap_mesh_cpp_levels_map_at_level_f(&
      levs, level, out, context, f &
    ) bind(C, name = "eap_mesh_cpp_levels_map_at_level_f")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), value :: out
      type(c_ptr), value :: context
      interface
        integer(c_int32_t) function f(context, l) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: l
        end function f
      end interface
    end subroutine eap_mesh_cpp_levels_map_at_level_f

    subroutine eap_mesh_cpp_levels_count_filter_at_level(&
      levs, cells, level, context, f, count &
    ) bind(C, name = "eap_mesh_cpp_levels_count_filter_at_level")
      import

      type(c_ptr), value :: levs
      type(c_ptr), value :: cells
      integer(c_int32_t), value :: level
      type(c_ptr), value :: context
      interface
        logical(c_bool) function f(context, l) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: l
        end function f
      end interface
      integer(c_int32_t), intent(out) :: count
    end subroutine eap_mesh_cpp_levels_count_filter_at_level

    subroutine eap_mesh_cpp_levels_filter_at_level_f(&
      levs, cells, level, out, context, f &
    ) bind(C, name = "eap_mesh_cpp_levels_filter_at_level_f")
      import

      type(c_ptr), value :: levs
      type(c_ptr), value :: cells
      integer(c_int32_t), value :: level
      type(nd_array_t), value :: out
      type(c_ptr), value :: context
      interface
        logical(c_bool) function f(context, l) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: l
        end function f
      end interface
    end subroutine eap_mesh_cpp_levels_filter_at_level_f

    subroutine eap_mesh_cpp_levels_collect_at_level_f(&
      levs, cells, level, out &
    ) bind(C, name = "eap_mesh_cpp_levels_collect_at_level_f")
      import

      type(c_ptr), value :: levs
      type(c_ptr), value :: cells
      integer(c_int32_t), value :: level
      type(nd_array_t), value :: out
    end subroutine eap_mesh_cpp_levels_collect_at_level_f

    subroutine eap_mesh_cpp_levels_clear_at_level(&
      levs, level, flags, num_cleared &
    ) bind(C, name = "eap_mesh_cpp_levels_clear_at_level")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), value :: flags
      integer(c_int32_t), intent(out) :: num_cleared
    end subroutine eap_mesh_cpp_levels_clear_at_level

    subroutine eap_mesh_cpp_levels_num_at_level(&
      levs, level, num &
    ) bind(C, name = "eap_mesh_cpp_levels_num_at_level")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      integer(c_int32_t), intent(out) :: num
    end subroutine eap_mesh_cpp_levels_num_at_level

    subroutine eap_mesh_cpp_levels_recon_pack(&
      levs, nlow, nhigh, nstep, move_num, move_from, move_to &
    ) bind(C, name = "eap_mesh_cpp_levels_recon_pack")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: nlow
      integer(c_int32_t), value :: nhigh
      integer(c_int32_t), value :: nstep
      integer(c_int32_t), value :: move_num
      integer(c_int32_t), intent(in) :: move_from(*)
      integer(c_int32_t), intent(in) :: move_to(*)
    end subroutine eap_mesh_cpp_levels_recon_pack

    subroutine eap_mesh_cpp_levels_set_flags(&
      levs, low, high, context, func &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: low
      integer(c_int32_t), value :: high
      type(c_ptr), value :: context
      interface
        integer(c_int32_t) function func(context, i) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: i
        end function func
      end interface
    end subroutine eap_mesh_cpp_levels_set_flags

    subroutine eap_mesh_cpp_levels_set_flags_scalar(&
      levs, low, high, value &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_scalar")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: low
      integer(c_int32_t), value :: high
      integer(c_int32_t), value :: value
    end subroutine eap_mesh_cpp_levels_set_flags_scalar

    subroutine eap_mesh_cpp_levels_set_flags_all(&
      levs, value &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_all")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: value
    end subroutine eap_mesh_cpp_levels_set_flags_all

    subroutine eap_mesh_cpp_levels_set_flags_scalar_when(&
      levs, low, high, value, context, func &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_scalar_when")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: low
      integer(c_int32_t), value :: high
      integer(c_int32_t), value :: value
      type(c_ptr), value :: context
      interface
        integer(c_int32_t) function func(context, i) bind(C)
          import

          type(c_ptr), value :: context
          integer(c_int32_t), value :: i
        end function func
      end interface
    end subroutine eap_mesh_cpp_levels_set_flags_scalar_when

    subroutine eap_mesh_cpp_levels_set_flags_scalar_from_list(&
      levs, list_size, list, value &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_scalar_from_list")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: list_size
      integer(c_int32_t) :: list(*)
      integer(c_int32_t), value :: value
    end subroutine eap_mesh_cpp_levels_set_flags_scalar_from_list

    subroutine eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list(&
      levs, list_size, list, value, tags_size, tags &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: list_size
      integer(c_int32_t), intent(in) :: list(*)
      integer(c_int32_t), value :: value
      integer(c_size_t), value :: tags_size
      integer(c_int32_t), intent(in) :: tags(*)
    end subroutine eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list

    subroutine eap_mesh_cpp_levels_set_flags_from_tuples(&
      levs, tups_size, tups &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_from_tuples")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: tups_size
      type(loc_val_t), intent(in) :: tups(*)
    end subroutine eap_mesh_cpp_levels_set_flags_from_tuples

    subroutine eap_mesh_cpp_levels_set_flag_tags_from_tuples(&
      levs, tups_size, tups &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flag_tags_from_tuples")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: tups_size
      type(loc_val_t), intent(in) :: tups(*)
    end subroutine eap_mesh_cpp_levels_set_flag_tags_from_tuples

    subroutine eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar(&
      levs, tups_size, tups, value &
    ) bind(C, name = "eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: tups_size
      type(loc_val_t), intent(in) :: tups(*)
      integer(c_int32_t), value :: value
    end subroutine eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar

    subroutine eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar(&
      levs, tups_size, tups, value &
    ) bind(&
      C, &
      name = "eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar" &
    )
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: tups_size
      type(loc_val_t), intent(in) :: tups(*)
      integer(c_int32_t), value :: value
    end subroutine eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar

    subroutine eap_mesh_cpp_levels_mom_kid_build(&
      levs, cells &
    ) bind(C, name = "eap_mesh_cpp_levels_mom_kid_build")
      import

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_mom_kid_build

    subroutine eap_mesh_cpp_levels_mom_kid_free(&
      levs &
    ) bind(C, name = "eap_mesh_cpp_levels_mom_kid_free")
      import

      type(c_ptr), value :: levs
    end subroutine eap_mesh_cpp_levels_mom_kid_free

    subroutine eap_mesh_cpp_levels_kid_mom_build(&
      levs, cells &
    ) bind(C, name = "eap_mesh_cpp_levels_kid_mom_build")
      import

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_kid_mom_build

    subroutine eap_mesh_cpp_levels_mom_kids_build(&
      levs, cells &
    ) bind(C, name = "eap_mesh_cpp_levels_mom_kids_build")
      import

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_mom_kids_build

    subroutine eap_mesh_cpp_levels_check_lohipack_and_levels(&
      levs, cells &
    ) bind(C, name = "eap_mesh_cpp_levels_check_lohipack_and_levels")
      import

      type(c_ptr), value :: levs, cells
    end subroutine eap_mesh_cpp_levels_check_lohipack_and_levels

    subroutine eap_mesh_cpp_levels_recon_div_parent(&
      levs, cells, parentl, childl &
    ) bind(C, name="eap_mesh_cpp_levels_recon_div_parent")
      import

      type(c_ptr), value :: levs, cells
      integer(c_int32_t), value :: parentl, childl
    end subroutine eap_mesh_cpp_levels_recon_div_parent

    subroutine eap_mesh_cpp_levels_recon_div_child(&
      levs, cells, level, parentl, l &
    ) bind(C, name="eap_mesh_cpp_levels_recon_div_child")
      import

      type(c_ptr), value :: levs, cells
      integer(c_int32_t), value :: level, parentl, l
    end subroutine eap_mesh_cpp_levels_recon_div_child

    subroutine eap_mesh_cpp_levels_clear_daughter(&
      levs, l &
    ) bind(C, name="eap_mesh_cpp_levels_clear_daughter")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: l
    end subroutine eap_mesh_cpp_levels_clear_daughter

    subroutine eap_mesh_cpp_levels_kid_get(&
      levs, level, input, output, datatype &
    ) bind(C, name="eap_mesh_cpp_levels_kid_get")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), intent(in) :: input
      type(nd_array_t), intent(in) :: output
      integer(c_int), value :: datatype
    end subroutine eap_mesh_cpp_levels_kid_get

    subroutine eap_mesh_cpp_levels_kid_put(&
      levs, level, input, output, datatype &
    ) bind(C, name="eap_mesh_cpp_levels_kid_put")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), intent(in) :: input
      type(nd_array_t), intent(in) :: output
      integer(c_int), value :: datatype
    end subroutine eap_mesh_cpp_levels_kid_put

    subroutine eap_mesh_cpp_levels_kid_put_v(&
      levs, level, input, output, datatype &
    ) bind(C, name="eap_mesh_cpp_levels_kid_put_v")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), intent(in) :: input
      type(nd_array_t), intent(in) :: output
      integer(c_int), value :: datatype
    end subroutine eap_mesh_cpp_levels_kid_put_v

    subroutine eap_mesh_cpp_levels_kid_put_inv(&
      levs, level, input, output, datatype &
    ) bind(C, name="eap_mesh_cpp_levels_kid_put_inv")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), intent(in) :: input
      type(nd_array_t), intent(in) :: output
      integer(c_int), value :: datatype
    end subroutine eap_mesh_cpp_levels_kid_put_inv

    subroutine eap_mesh_cpp_levels_mom_get(&
      levs, level, input, output, datatype &
    ) bind(C, name="eap_mesh_cpp_levels_mom_get")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: level
      type(nd_array_t), intent(in) :: input
      type(nd_array_t), intent(in) :: output
      integer(c_int), value :: datatype
    end subroutine eap_mesh_cpp_levels_mom_get

    subroutine eap_mesh_cpp_levels_apply_dzns(&
      levs, dzn, numdim, cells, time, kode &
    ) bind(C,name="eap_mesh_cpp_levels_apply_dzns")
      import

      type(c_ptr), value :: levs
      type(c_ptr), value :: dzn
      integer(c_int), value :: numdim
      type(c_ptr), value :: cells
      real(c_double), value :: time
      integer(c_int), value :: kode
    end subroutine eap_mesh_cpp_levels_apply_dzns

    subroutine eap_mesh_cpp_levels_set_num_pack(&
      levs, num_pack &
    ) bind(C,name="eap_mesh_cpp_levels_set_num_pack")
      import

      type(c_ptr), value :: levs
      integer(c_int32_t), value :: num_pack
    end subroutine eap_mesh_cpp_levels_set_num_pack

    subroutine eap_mesh_cpp_levels_set_level_max_from_tuples(&
      levs, tups_size, tups &
    ) bind(C, name="eap_mesh_cpp_levels_set_level_max_from_tuples")
      import

      type(c_ptr), value :: levs
      integer(c_size_t), value :: tups_size
      type(loc_val_t), intent(in) :: tups(*)
    end subroutine eap_mesh_cpp_levels_set_level_max_from_tuples
  end interface 
contains
  subroutine new_cpp_levels(levs)
    implicit none
    type(cpp_levels_t), intent(out) :: levs

    call CREATE_C(mesh,cpp_levels)(levs%ptr)
  end subroutine new_cpp_levels

  ! IMPORTANT: NO SPACE AFTER COMMA
  FC_INTEROP_FREE_IMPL(mesh,cpp_levels)
  FC_INTEROP_IS_ASSOCIATED_IMPL(mesh,cpp_levels)

  subroutine cpp_levels_t_initialize(self, opts)
    class(cpp_levels_t), intent(inout) :: self
    type(cpp_levels_options_t), intent(in) :: opts

    call eap_mesh_cpp_levels_initialize(self%ptr, opts)
  end subroutine cpp_levels_t_initialize

  subroutine cpp_levels_t_initialize_level1(self)
    class(cpp_levels_t), intent(inout) :: self

    call eap_mesh_cpp_levels_initialize_level1(self%ptr)
  end subroutine cpp_levels_t_initialize_level1

  subroutine cpp_levels_t_build_pack(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells

    call eap_mesh_cpp_levels_build_pack(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_build_pack

  subroutine cpp_levels_t_build_top(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells

    call eap_mesh_cpp_levels_build_top(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_build_top

  pure function cpp_levels_t_ltop_nv(self, l) result(ret)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: l
    integer :: ret

    ret = self%ltop(l)
  end function cpp_levels_t_ltop_nv

  pure function cpp_levels_t_alltop_nv(self, l) result(ret)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: l
    integer :: ret

    ret = self%alltop(l)
  end function cpp_levels_t_alltop_nv

  subroutine cpp_levels_t_reset_mothers_and_daughters(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells

    call eap_mesh_cpp_levels_reset_mothers_and_daughters(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_reset_mothers_and_daughters

  subroutine cpp_levels_t_reset_mothers_and_daughters_new(&
    self, cells, new_cells &
  )
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells
    integer, intent(in) :: new_cells

    call eap_mesh_cpp_levels_reset_mothers_and_daughters_new(&
      self%ptr, cells%ptr, new_cells)
  end subroutine cpp_levels_t_reset_mothers_and_daughters_new

  subroutine cpp_levels_t_setup_alltop(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells

    type(eap_mesh_cpp_levels_setup_all_top_fields) :: fields

    call eap_mesh_cpp_levels_setup_all_top(self%ptr, cells%ptr, fields)

    call from_nd_array(fields%all_top, self%alltop)
  end subroutine cpp_levels_t_setup_alltop

  pure subroutine cpp_levels_t_cell_mother_set(self, l, val)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: l
    integer(INT64), intent(in) :: val

    self%cell_mother(l) = val
  end subroutine cpp_levels_t_cell_mother_set

  pure subroutine cpp_levels_t_cell_daughter_set(self, l, val)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: l
    integer(INT64), intent(in) :: val

    self%cell_daughter(l) = val
  end subroutine cpp_levels_t_cell_daughter_set

  pure subroutine cpp_levels_t_levelmx_set(self, l, val)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: l, val

    self%levelmx(l) = val
  end subroutine cpp_levels_t_levelmx_set

  pure subroutine cpp_levels_t_setall_levelmx(self, cells, levelmx_tmp)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells
    integer, intent(in) :: levelmx_tmp(:)

    self%levelmx(:cells%cpp_numcell_clone) = &
      levelmx_tmp(:cells%cpp_numcell_clone)
  end subroutine cpp_levels_t_setall_levelmx

  subroutine cpp_levels_t_resize_local(self, num_cells, newsize)
    class(cpp_levels_t), intent(inout) :: self
    integer(INT32), intent(in) :: num_cells, newsize

    type(eap_mesh_cpp_levels_resize_local_fields) :: fields

    call eap_mesh_cpp_levels_resize_local(self%ptr, num_cells, newsize, fields)

    call from_nd_array(fields%cell_daughter, self%cell_daughter)
    call from_nd_array(fields%cell_mother, self%cell_mother)
    call from_nd_array(fields%ltop, self%ltop)
    call from_nd_array(fields%cell_level, self%cell_level)
    call from_nd_array(fields%flag, self%flag)
    call from_nd_array(fields%flag_tag, self%flag_tag)
  end subroutine cpp_levels_t_resize_local

  subroutine cpp_levels_t_recon_move(&
    self, data_length, send_start, send_length, recv_start, recv_length &
  )
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: data_length
    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length

    call eap_mesh_cpp_levels_recon_move_f(&
      self%ptr, data_length, size(send_start, kind=c_size_t), &
      send_start, send_length, recv_start, recv_length)
  end subroutine cpp_levels_t_recon_move

  subroutine cpp_levels_t_foreach_at_level(&
    self, level, f &
  )
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    interface
      subroutine f(l)
        integer, intent(in) :: l
      end subroutine f
    end interface

    call eap_mesh_cpp_levels_foreach_at_level(&
      self%ptr, level - 1, c_null_ptr, trampoline)
  contains
    subroutine trampoline(context, l) bind(C)
      type(c_ptr), value :: context
      integer(c_int32_t), value :: l

      ! Suppress unused-dummy-argument - we don't need context in Fortran since
      ! subroutines implicitly capture the state of their parents
      if (.false.) context = c_null_ptr

      call f(l + 1)
    end subroutine trampoline
  end subroutine cpp_levels_t_foreach_at_level

  subroutine cpp_levels_t_map_at_level(&
    self, level, out, f &
  )
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    type(loc_val_t), intent(inout) :: out(:)
    interface
      integer function f(l)
        integer, intent(in) :: l
      end function f
    end interface

    call eap_mesh_cpp_levels_map_at_level_f(&
      self%ptr, level - 1, to_nd_array_lv(out), c_null_ptr, trampoline)
  contains
    integer(c_int32_t) function trampoline(context, l) bind(C)
      type(c_ptr), value :: context
      integer(c_int32_t), value :: l

      ! Suppress unused-dummy-argument - we don't need context in Fortran since
      ! subroutines implicitly capture the state of their parents
      if (.false.) context = c_null_ptr

      trampoline = f(l + 1)
    end function trampoline
  end subroutine cpp_levels_t_map_at_level

  function cpp_levels_t_filter_at_level(&
    self, cells, level, pred &
  ) result(filtered)
    class(cpp_levels_t), intent(in) :: self
    class(cpp_cells_t), intent(in) :: cells
    integer, intent(in) :: level
    interface
      logical function pred(l)
        integer, intent(in) :: l
      end function pred
    end interface
    integer, allocatable :: filtered(:)

    integer :: count

    count = 0
    call eap_mesh_cpp_levels_count_filter_at_level(&
      self%ptr, cells%ptr, level - 1, c_null_ptr, trampoline, count &
    )

    allocate(filtered(count))
    call eap_mesh_cpp_levels_filter_at_level_f(&
      self%ptr, cells%ptr, level - 1, to_nd_array(filtered), c_null_ptr, &
      trampoline &
    )
  contains
    logical(c_bool) function trampoline(context, l) bind(C)
      type(c_ptr), value :: context
      integer(c_int), value :: l

      ! Suppress unused-dummy-argument - we don't need context in Fortran since
      ! subroutines implicitly capture the state of their parents
      if (.false.) context = c_null_ptr

      trampoline = logical(pred(l + 1), c_bool)
    end function trampoline
  end function cpp_levels_t_filter_at_level

  function cpp_levels_t_collect_at_level(&
    levs, cells, level &
  ) result(collected)
    class(cpp_levels_t), intent(in) :: levs
    class(cpp_cells_t), intent(in)  :: cells
    integer, intent(in) :: level
    integer, allocatable :: collected(:)

    integer :: num

    num = levs%num_at_level(level)
    allocate(collected(num))

    call eap_mesh_cpp_levels_collect_at_level_f(&
      levs%ptr, cells%ptr, level - 1, to_nd_array(collected) &
    )
  end function cpp_levels_t_collect_at_level

  integer function cpp_levels_t_clear_at_level(&
    levs, level, flags &
  ) result(num_cleared)
    class(cpp_levels_t), intent(in) :: levs
    integer, intent(in) :: level
    logical, intent(in) :: flags(:)

    logical(c_bool) :: flags_c_bool(size(flags, 1))

    flags_c_bool = logical(flags, c_bool)

    call eap_mesh_cpp_levels_clear_at_level(&
      levs%ptr, level - 1, to_nd_array(flags_c_bool), num_cleared &
    )
  end function cpp_levels_t_clear_at_level

  integer function cpp_levels_t_num_at_level(levs, level) result(num)
    class(cpp_levels_t), intent(in) :: levs
    integer, intent(in) :: level

    call eap_mesh_cpp_levels_num_at_level(levs%ptr, level - 1, num)
  end function cpp_levels_t_num_at_level

  subroutine cpp_levels_t_recon_pack(&
    levs, nlow, nhigh, nstep, move_from, move_to &
  )
    class(cpp_levels_t), intent(in) :: levs
    integer, intent(in) :: nlow, nhigh, nstep
    integer, intent(in) :: move_from(:), move_to(:)

    call eap_mesh_cpp_levels_recon_pack(&
      levs%ptr, nlow - 1, nhigh - 1, nstep, size(move_from, 1), move_from, &
      move_to &
    )
  end subroutine cpp_levels_t_recon_pack

  subroutine cpp_levels_t_set_flags(self, low, high, func)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: low, high
    interface
      integer function func(i)
        integer, intent(in) :: i
      end function func
    end interface

    call eap_mesh_cpp_levels_set_flags(&
      self%ptr, low - 1, high, c_null_ptr, trampoline)
  contains
    integer(c_int32_t) function trampoline(context, i) bind(C)
      type(c_ptr), value :: context
      integer(c_int32_t), value :: i

      ! Suppress unused-dummy-argument - we don't need context in Fortran since
      ! subroutines implicitly capture the state of their parents
      if (.false.) context = c_null_ptr

      trampoline = func(i + 1)
    end function trampoline
  end subroutine cpp_levels_t_set_flags

  subroutine cpp_levels_t_set_flags_scalar(self, low, high, value)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: low, high, value

    call eap_mesh_cpp_levels_set_flags_scalar(&
      self%ptr, low - 1, high, value)
  end subroutine cpp_levels_t_set_flags_scalar

  subroutine cpp_levels_t_set_flags_all(self, value)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: value

    call eap_mesh_cpp_levels_set_flags_all(self%ptr, value)
  end subroutine cpp_levels_t_set_flags_all

  subroutine cpp_levels_t_set_flags_scalar_when(self, low, high, value, func)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: low, high, value
    interface
      integer function func(i)
        integer, intent(in) :: i
      end function func
    end interface

    call eap_mesh_cpp_levels_set_flags_scalar_when(&
      self%ptr, low - 1, high, value, c_null_ptr, trampoline)
  contains
    integer(c_int32_t) function trampoline(context, i) bind(C)
      type(c_ptr), value :: context
      integer(c_int32_t), value :: i

      ! Suppress unused-dummy-argument - we don't need context in Fortran since
      ! subroutines implicitly capture the state of their parents
      if (.false.) context = c_null_ptr

      trampoline = func(i + 1)
    end function trampoline
  end subroutine cpp_levels_t_set_flags_scalar_when

  subroutine cpp_levels_t_set_flags_scalar_from_list(self, list, value)
    class(cpp_levels_t), intent(inout) :: self
    integer, contiguous, intent(in) :: list(:)
    integer, intent(in) :: value

    call eap_mesh_cpp_levels_set_flags_scalar_from_list(&
      self%ptr, size(list, 1, c_size_t), list, value)
  end subroutine cpp_levels_t_set_flags_scalar_from_list

  subroutine cpp_levels_t_set_flags_scalar_and_tags_from_list(&
    self, list, value, tags &
  )
    class(cpp_levels_t), intent(inout) :: self
    integer, contiguous, intent(in) :: list(:)
    integer, intent(in) :: value
    integer, contiguous, intent(in) :: tags(:)

    call eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list(&
      self%ptr, size(list, 1, c_size_t), list, value, &
      size(tags, 1, c_size_t), tags)
  end subroutine cpp_levels_t_set_flags_scalar_and_tags_from_list

  subroutine cpp_levels_t_set_flags_from_tuples(self, tups)
    class(cpp_levels_t), intent(inout) :: self
    type(loc_val_t), contiguous, intent(in) :: tups(:)

    call eap_mesh_cpp_levels_set_flags_from_tuples(&
      self%ptr, size(tups, 1, c_size_t), tups)
  end subroutine cpp_levels_t_set_flags_from_tuples

  subroutine cpp_levels_t_set_flag_tags_from_tuples(self, tups)
    class(cpp_levels_t), intent(inout) :: self
    type(loc_val_t), contiguous, intent(in) :: tups(:)

    call eap_mesh_cpp_levels_set_flag_tags_from_tuples(&
      self%ptr, size(tups, 1, c_size_t), tups)
  end subroutine cpp_levels_t_set_flag_tags_from_tuples

  subroutine cpp_levels_t_set_flags_from_tuples_key_and_scalar(&
    self, tups, value &
  )
    class(cpp_levels_t), intent(inout) :: self
    type(loc_val_t), contiguous, intent(in) :: tups(:)
    integer, intent(in) :: value

    call eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar(&
      self%ptr, size(tups, 1, c_size_t), tups, value)
  end subroutine cpp_levels_t_set_flags_from_tuples_key_and_scalar

  subroutine cpp_levels_t_set_flag_tags_from_tuples_key_and_scalar(&
    self, tups, value &
  )
    class(cpp_levels_t), intent(inout) :: self
    type(loc_val_t), contiguous, intent(in) :: tups(:)
    integer, intent(in) :: value

    call eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar(&
      self%ptr, size(tups, 1, c_size_t), tups, value)
  end subroutine cpp_levels_t_set_flag_tags_from_tuples_key_and_scalar

  subroutine cpp_levels_t_mom_kid_build(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(inout) :: cells

    call eap_mesh_cpp_levels_mom_kid_build(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_mom_kid_build

  subroutine cpp_levels_t_mom_kid_free(self)
    class(cpp_levels_t), intent(inout) :: self

    call eap_mesh_cpp_levels_mom_kid_free(self%ptr)
  end subroutine cpp_levels_t_mom_kid_free

  subroutine cpp_levels_t_kid_mom_build(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(inout) :: cells

    call eap_mesh_cpp_levels_kid_mom_build(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_kid_mom_build

  subroutine cpp_levels_t_mom_kids_build(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(inout) :: cells

    call eap_mesh_cpp_levels_mom_kids_build(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_mom_kids_build

  subroutine cpp_levels_t_check_lohipack_and_levels(self, cells)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells

    call eap_mesh_cpp_levels_check_lohipack_and_levels(self%ptr, cells%ptr)
  end subroutine cpp_levels_t_check_lohipack_and_levels

  subroutine cpp_levels_t_recon_div_parent(self, cells, parentl, childl)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells
    integer, intent(in) :: parentl, childl

    call eap_mesh_cpp_levels_recon_div_parent(&
      self%ptr, cells%ptr, parentl - 1, childl - 1)
  end subroutine cpp_levels_t_recon_div_parent

  subroutine cpp_levels_t_recon_div_child(self, cells, level, parentl, l)
    class(cpp_levels_t), intent(inout) :: self
    class(cpp_cells_t), intent(in) :: cells
    integer, intent(in) :: level, parentl, l

    call eap_mesh_cpp_levels_recon_div_child(&
      self%ptr, cells%ptr, level - 1, parentl - 1, l - 1)
  end subroutine cpp_levels_t_recon_div_child

  subroutine cpp_levels_t_clear_daughter(self, l)
    class(cpp_levels_t), intent(inout) :: self
    integer, intent(in) :: l

    call eap_mesh_cpp_levels_clear_daughter(self%ptr, l - 1)
  end subroutine cpp_levels_t_clear_daughter

  subroutine cpp_levels_t_kid_get_i32(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    integer(INT32), target, intent(in) :: input(:)
    integer(INT32), target, intent(out) :: output(:)

    call eap_mesh_cpp_levels_kid_get(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_int32)
  end subroutine cpp_levels_t_kid_get_i32

  subroutine cpp_levels_t_kid_get_r64(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    real(REAL64), target, intent(in) :: input(:)
    real(REAL64), target, intent(out) :: output(:)

    call eap_mesh_cpp_levels_kid_get(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_double)
  end subroutine cpp_levels_t_kid_get_r64

  subroutine cpp_levels_t_kid_put_i32(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    integer(INT32), target, intent(in) :: input(:)
    integer(INT32), target, intent(out) :: output(:)

    call eap_mesh_cpp_levels_kid_put(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_int32)
  end subroutine cpp_levels_t_kid_put_i32

  subroutine cpp_levels_t_kid_put_r64(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    real(REAL64), target, intent(in) :: input(:)
    real(REAL64), target, intent(out) :: output(:)

    call eap_mesh_cpp_levels_kid_put(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_double)
  end subroutine cpp_levels_t_kid_put_r64

  subroutine cpp_levels_t_kid_put_r64_2d(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call eap_mesh_cpp_levels_kid_put_v(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_double)
  end subroutine cpp_levels_t_kid_put_r64_2d

  subroutine cpp_levels_t_kid_put_inv_r64_2d(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call eap_mesh_cpp_levels_kid_put_inv(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_double)
  end subroutine cpp_levels_t_kid_put_inv_r64_2d

  subroutine cpp_levels_t_mom_get_r64(self, level, input, output)
    class(cpp_levels_t), intent(in) :: self
    integer, intent(in) :: level
    real(REAL64), target, intent(in) :: input(:)
    real(REAL64), target, intent(out) :: output(:)

    call eap_mesh_cpp_levels_mom_get(&
      self%ptr, level - 1, to_nd_array(input), to_nd_array(output), ct_double)
  end subroutine cpp_levels_t_mom_get_r64

  subroutine cpp_levels_t_apply_dzns(self, dzn, numdim, cells, time, kode)
    use mesh_dzn, only : dzn_t

    class(cpp_levels_t), intent(inout) :: self
    type(dzn_t), intent(in) :: dzn
    integer, intent(in) :: numdim
    type(cpp_cells_t), intent(in) :: cells
    real(REAL64), intent(in) :: time
    integer, intent(in) :: kode

    call eap_mesh_cpp_levels_apply_dzns(&
      self%ptr, dzn%ptr, numdim, cells%ptr, time, kode)
  end subroutine cpp_levels_t_apply_dzns

  subroutine cpp_levels_t_set_num_pack(self, num_pack)
    class(cpp_levels_t), intent(inout) :: self
    integer(INT32), intent(in) :: num_pack

    call eap_mesh_cpp_levels_set_num_pack(self%ptr, num_pack)
  end subroutine cpp_levels_t_set_num_pack

  subroutine cpp_levels_t_set_level_max_from_tuples(self, tups)
    class(cpp_levels_t), intent(inout) :: self
    type(loc_val_t), intent(in) :: tups(:)

    call eap_mesh_cpp_levels_set_level_max_from_tuples(&
      self%ptr, size(tups, 1, c_size_t), tups)
  end subroutine cpp_levels_t_set_level_max_from_tuples
end module mesh_levels
