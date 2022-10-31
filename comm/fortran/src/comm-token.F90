#include "comm-mpi.F90.h"

module token
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env
  use abi
  use comm_patterns
  use comm_types

  MPI_USE

  implicit none
  private

  MPI_INCLUDE

  public &
    token_builder_t, &
    token_t, &
    new_token_builder

  public &
    cto_copy, &
    cto_add, &
    cto_sub, &
    cto_max, &
    cto_min

  enum, bind(C)
    enumerator &
      cto_copy, &
      cto_add, &
      cto_sub, &
      cto_max, &
      cto_min
  end enum

  type token_builder_t
    type(c_ptr) :: builder = c_null_ptr
  contains
    procedure :: free => token_builder_t_free
    procedure :: is_associated => token_builder_t_is_associated
    procedure :: clone => token_builder_t_clone

    procedure :: set_num_cell => token_builder_t_set_num_cell

    procedure :: use_rma_all_to_all => token_builder_t_use_rma_all_to_all

    procedure :: require_rank_order_request_completion => &
      token_builder_t_require_rank_order_request_completion

    procedure :: set_to_pes => token_builder_t_set_to_pes

    procedure :: set_to_and_from_pes => token_builder_t_set_to_and_from_pes
    procedure :: clear_to_and_from_pes => token_builder_t_clear_to_and_from_pes

    procedure :: set_max_gs_receive_size => &
      token_builder_t_set_max_gs_receive_size
    procedure :: clear_max_gs_receive_size => &
      token_builder_t_clear_max_gs_receive_size

    procedure :: pes_and_addresses => token_builder_t_pes_and_addresses
    procedure :: flag_pes => token_builder_t_flag_pes
    procedure :: build_global => token_builder_t_build_global
    procedure :: build_local => token_builder_t_build_local
  end type token_builder_t

  type token_t
    type(c_ptr), private :: token = c_null_ptr
  contains
    procedure :: clone => token_t_clone
    procedure :: free => token_t_free
    procedure :: is_associated => token_t_is_associated

    procedure :: home_num => token_t_home_num
    procedure :: home_size => token_t_home_size
    procedure :: fill_home_arrays => token_t_fill_home_arrays

    generic :: get => &
      token_t_get_l, &
      token_t_get_i32, &
      token_t_get_i64, &
      token_t_get_r32, &
      token_t_get_r64, &
      token_t_get_l_2d, &
      token_t_get_i32_2d, &
      token_t_get_i64_2d, &
      token_t_get_r32_2d, &
      token_t_get_r64_2d

    procedure, private :: &
      token_t_get_l, &
      token_t_get_i32, &
      token_t_get_i64, &
      token_t_get_r32, &
      token_t_get_r64, &
      token_t_get_l_2d, &
      token_t_get_i32_2d, &
      token_t_get_i64_2d, &
      token_t_get_r32_2d, &
      token_t_get_r64_2d

    generic :: get_inv => &
      token_t_get_inv_l_2d, &
      token_t_get_inv_i32_2d, &
      token_t_get_inv_i64_2d, &
      token_t_get_inv_r32_2d, &
      token_t_get_inv_r64_2d

    procedure, private :: &
      token_t_get_inv_l_2d, &
      token_t_get_inv_i32_2d, &
      token_t_get_inv_i64_2d, &
      token_t_get_inv_r32_2d, &
      token_t_get_inv_r64_2d

    generic :: put => &
      token_t_put_l, &
      token_t_put_i32, &
      token_t_put_i64, &
      token_t_put_r32, &
      token_t_put_r64, &
      token_t_put_l_2d, &
      token_t_put_i32_2d, &
      token_t_put_i64_2d, &
      token_t_put_r32_2d, &
      token_t_put_r64_2d

    procedure, private :: &
      token_t_put_l, &
      token_t_put_i32, &
      token_t_put_i64, &
      token_t_put_r32, &
      token_t_put_r64, &
      token_t_put_l_2d, &
      token_t_put_i32_2d, &
      token_t_put_i64_2d, &
      token_t_put_r32_2d, &
      token_t_put_r64_2d

    generic :: put_inv => &
      token_t_put_inv_l_2d, &
      token_t_put_inv_i32_2d, &
      token_t_put_inv_i64_2d, &
      token_t_put_inv_r32_2d, &
      token_t_put_inv_r64_2d

    procedure, private :: &
      token_t_put_inv_l_2d, &
      token_t_put_inv_i32_2d, &
      token_t_put_inv_i64_2d, &
      token_t_put_inv_r32_2d, &
      token_t_put_inv_r64_2d
  end type token_t

  interface
    subroutine comm_token_builder_create(comm, token_builder) &
      bind(C, name="comm_token_builder_create_f")
      import

#ifdef TOKEN_USE_MPI_F08
      type(MPI_Comm), value, intent(in) :: comm
#else
      integer(c_int), value, intent(in) :: comm
#endif
      type(c_ptr), intent(out) :: token_builder
    end subroutine comm_token_builder_create

    subroutine comm_token_builder_free(token_builder) &
      bind(C, name="comm_token_builder_free")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
    end subroutine comm_token_builder_free

    subroutine comm_token_builder_clone(token_builder, new_token_builder) &
      bind(C, name="comm_token_builder_clone")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
      type(c_ptr), intent(out) :: new_token_builder
    end subroutine comm_token_builder_clone

    subroutine comm_token_builder_set_num_cells(token_builder, num_local_cell) &
      bind(C, name="comm_token_builder_set_num_cells")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
      integer(c_int32_t), value, intent(in) :: num_local_cell
    end subroutine comm_token_builder_set_num_cells

    subroutine comm_token_builder_use_rma_all_to_all(token_builder, rma) &
      bind(C, name="comm_token_builder_use_rma_all_to_all")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder, rma
    end subroutine comm_token_builder_use_rma_all_to_all

    subroutine comm_token_builder_require_rank_order_request_completion(&
      token_builder, require_rank_order_completion) &
      bind(C, name="comm_token_builder_require_rank_order_request_completion")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
      logical(c_bool), value, intent(in) :: require_rank_order_completion
    end subroutine comm_token_builder_require_rank_order_request_completion

    subroutine comm_token_builder_set_to_pes(&
      token_builder, to_pes, to_pes_length) &
      bind(C, name="comm_token_builder_set_to_pes")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
      integer(c_int), intent(in) :: to_pes(*)
      integer(c_size_t), value, intent(in) :: to_pes_length
    end subroutine comm_token_builder_set_to_pes

    subroutine comm_token_builder_set_to_and_from_pes(&
      token_builder, to_pes, to_pes_length, from_pes, from_pes_length) &
      bind(C, name="comm_token_builder_set_to_and_from_pes")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
      integer(c_int), intent(in) :: to_pes(*)
      integer(c_size_t), value, intent(in) :: to_pes_length
      integer(c_int), intent(in) :: from_pes(*)
      integer(c_size_t), value, intent(in) :: from_pes_length
    end subroutine comm_token_builder_set_to_and_from_pes

    subroutine comm_token_builder_clear_to_and_from_pes(token_builder) &
      bind(C, name="comm_token_builder_clear_to_and_from_pes")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder
    end subroutine comm_token_builder_clear_to_and_from_pes

    subroutine comm_token_builder_set_max_gs_receive_size(&
      builder, max_gs_receive_size) &
      bind(C, name = "comm_token_builder_set_max_gs_receive_size")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: builder
      integer(c_size_t), value, intent(in) :: max_gs_receive_size
    end subroutine comm_token_builder_set_max_gs_receive_size

    subroutine comm_token_builder_clear_max_gs_receive_size(builder) &
      bind(C, name = "comm_token_builder_clear_max_gs_receive_size")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: builder
    end subroutine comm_token_builder_clear_max_gs_receive_size

    subroutine comm_token_builder_pes_and_addresses(&
      token_builder, away_globals, away_globals_length, pes, pes_length, &
      addresses, addresses_length) &
      bind(C, name="comm_token_builder_pes_and_addresses_f")
      use, intrinsic :: iso_c_binding
      use, intrinsic :: iso_fortran_env

      type(c_ptr), value, intent(in) :: token_builder

      integer(c_int64_t), intent(in) :: away_globals(*)
      integer(c_size_t), value, intent(in) :: away_globals_length

      integer(c_int), intent(inout) :: pes(*)
      integer(c_size_t), value, intent(in) :: pes_length

      integer(c_int32_t), intent(inout) :: addresses(*)
      integer(c_size_t), value, intent(in) :: addresses_length
    end subroutine comm_token_builder_pes_and_addresses

    subroutine comm_token_builder_flag_pes(&
      token_builder, &
      away_globals, away_globals_length, &
      flag_pes, flag_pes_length) &
      bind(C, name="comm_token_builder_flag_pes_f")
      use, intrinsic :: iso_c_binding
      use, intrinsic :: iso_fortran_env

      type(c_ptr), value, intent(in) :: token_builder

      integer(c_int64_t), intent(in) :: away_globals(*)
      integer(c_size_t), value, intent(in) :: away_globals_length

      integer(c_int), intent(inout) :: flag_pes(*)
      integer(c_size_t), value, intent(in) :: flag_pes_length
    end subroutine comm_token_builder_flag_pes

    subroutine comm_token_builder_build_global(&
      token_builder, home_addresses, home_addresses_length, &
      away_global, away_global_length, token) &
      bind(C, name="comm_token_builder_build_global_f")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder

      integer(c_int32_t), intent(in) :: home_addresses(*)
      integer(c_size_t), value, intent(in) :: home_addresses_length

      integer(c_int64_t), intent(in) :: away_global(*)
      integer(c_size_t), value, intent(in) :: away_global_length

      type(c_ptr), intent(out) :: token
    end subroutine comm_token_builder_build_global

    subroutine comm_token_builder_build_local(&
      token_builder, &
      home_addresses, home_addresses_length, &
      away_pe, away_pe_length, &
      away_address, away_address_length, &
      token) &
      bind(C, name="comm_token_builder_build_local_f")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: token_builder

      integer(c_int32_t), intent(in) :: home_addresses(*)
      integer(c_size_t), value, intent(in) :: home_addresses_length

      integer(c_int), intent(in) :: away_pe(*)
      integer(c_size_t), value, intent(in) :: away_pe_length

      integer(c_int32_t), intent(in) :: away_address(*)
      integer(c_size_t), value, intent(in) :: away_address_length

      type(c_ptr), intent(out) :: token
    end subroutine comm_token_builder_build_local

    subroutine comm_token_clone(token,new_token) &
        bind(C,name="comm_token_clone")
      use iso_c_binding
      type(c_ptr),value,intent(in) :: token
      type(c_ptr),intent(out) :: new_token
    end subroutine comm_token_clone

    subroutine comm_token_free(token) bind(C, name="comm_token_free")
      use iso_c_binding

      type(c_ptr), value, intent(in) :: token
    end subroutine comm_token_free

    subroutine comm_token_get_home_num(&
      token, home_num &
    ) bind(C, name="comm_token_get_home_num")
      import

      type(c_ptr), value :: token
      integer(c_size_t), intent(out) :: home_num
    end subroutine comm_token_get_home_num

    ! void comm_token_fill_home_arrays_f(
    !   comm_token_t const *token,
    !   size_t home_num,
    !   int *ranks,
    !   int32_t *los,
    !   int32_t *lengths,
    !   size_t home_size,
    !   int32_t *indices);
    subroutine comm_token_fill_home_arrays_f(&
      token, home_num, ranks, los, lengths, home_size, indices &
    ) bind(C, name="comm_token_fill_home_arrays_f")
      import

      type(c_ptr), value :: token
      integer(c_size_t), value :: home_num
      integer(c_int), intent(out) :: ranks(*)
      integer(c_int32_t), intent(out) :: &
        los(*), &
        lengths(*)
      integer(c_size_t), value :: home_size
      integer(c_int32_t), intent(out) :: indices(*)
    end subroutine comm_token_fill_home_arrays_f

    subroutine comm_token_get_home_size(&
      token, home_size &
    ) bind(C, name="comm_token_get_home_size")
      import

      type(c_ptr), value :: token
      integer(c_size_t), intent(out) :: home_size
    end subroutine comm_token_get_home_size

    subroutine comm_token_get(token, dowhat, input, output, datatype) &
      bind(C, name="comm_token_get")
      use abi
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: token
      integer(c_int), value :: dowhat
      type(nd_array_t) :: input, output
      integer(c_int), value :: datatype
    end subroutine comm_token_get

    subroutine comm_token_put(token, dowhat, input, output, datatype) &
      bind(C, name="comm_token_put")
      use abi
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: token
      integer(c_int), value :: dowhat
      type(nd_array_t) :: input, output
      integer(c_int), value :: datatype
    end subroutine comm_token_put

    subroutine comm_token_get_inv(token, dowhat, input, output, datatype) &
      bind(C, name="comm_token_get_inv")
      use abi
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: token
      integer(c_int), value :: dowhat
      type(nd_array_t) :: input, output
      integer(c_int), value :: datatype
    end subroutine comm_token_get_inv

    subroutine comm_token_put_inv(token, dowhat, input, output, datatype) &
      bind(C, name="comm_token_put_inv")
      use abi
      use, intrinsic :: iso_c_binding

      type(c_ptr), value :: token
      integer(c_int), value :: dowhat
      type(nd_array_t) :: input, output
      integer(c_int), value :: datatype
    end subroutine comm_token_put_inv
  end interface
contains
  function new_token_builder(comm) result(builder)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer(c_int), intent(in) :: comm
#endif
    type(token_builder_t) :: builder

    call comm_token_builder_create(comm, builder%builder)
  end function new_token_builder

  subroutine token_builder_t_free(builder)
    class(token_builder_t), intent(inout) :: builder

    if (builder%is_associated()) call comm_token_builder_free(builder%builder)
    builder%builder = c_null_ptr
  end subroutine token_builder_t_free

  logical function token_builder_t_is_associated(builder)
    class(token_builder_t), intent(in) :: builder

    token_builder_t_is_associated = c_associated(builder%builder)
  end function token_builder_t_is_associated

  function token_builder_t_clone(builder) result(new_builder)
    class(token_builder_t), intent(in) :: builder
    type(token_builder_t) :: new_builder

    call comm_token_builder_clone(builder%builder, new_builder%builder)
  end function token_builder_t_clone

  subroutine token_builder_t_set_num_cell(builder, num_local_cell)
    class(token_builder_t), intent(inout) :: builder
    integer(c_int32_t) :: num_local_cell

    call comm_token_builder_set_num_cells(builder%builder, num_local_cell)
  end subroutine token_builder_t_set_num_cell

  subroutine token_builder_t_use_rma_all_to_all(builder, rma)
    class(token_builder_t), intent(inout) :: builder
    type(rma_all_to_all_int_t), intent(inout) :: rma

    call comm_token_builder_use_rma_all_to_all(builder%builder, rma%handle())
  end subroutine token_builder_t_use_rma_all_to_all

  subroutine token_builder_t_require_rank_order_request_completion(&
    builder, require_rank_order_completion)
    class(token_builder_t), intent(inout) :: builder
    logical :: require_rank_order_completion

    call comm_token_builder_require_rank_order_request_completion(&
      builder%builder, logical(require_rank_order_completion, c_bool))
  end subroutine token_builder_t_require_rank_order_request_completion

  subroutine token_builder_t_set_to_pes(builder, to_pes)
    class(token_builder_t), intent(inout) :: builder
    integer :: to_pes(:)

    call comm_token_builder_set_to_pes(&
      builder%builder, &
      to_pes, size(to_pes, 1, c_size_t))
  end subroutine token_builder_t_set_to_pes

  subroutine token_builder_t_set_to_and_from_pes(builder, to_pes, from_pes)
    class(token_builder_t), intent(inout) :: builder
    integer, intent(in) :: to_pes(:), from_pes(:)

    call comm_token_builder_set_to_and_from_pes(&
      builder%builder, &
      to_pes, size(to_pes, 1, c_size_t), &
      from_pes, size(from_pes, 1, c_size_t))
  end subroutine token_builder_t_set_to_and_from_pes

  subroutine token_builder_t_clear_to_and_from_pes(builder)
    class(token_builder_t), intent(inout) :: builder

    call comm_token_builder_clear_to_and_from_pes(builder%builder)
  end subroutine token_builder_t_clear_to_and_from_pes

  subroutine token_builder_t_set_max_gs_receive_size(&
    builder, max_gs_receive_size)

    class(token_builder_t), intent(inout) :: builder
    integer(INT64), intent(in) :: max_gs_receive_size

    call comm_token_builder_set_max_gs_receive_size(&
      builder%builder, max_gs_receive_size)
  end subroutine token_builder_t_set_max_gs_receive_size

  subroutine token_builder_t_clear_max_gs_receive_size(builder)
    class(token_builder_t), intent(inout) :: builder

    call comm_token_builder_clear_max_gs_receive_size(builder%builder)
  end subroutine token_builder_t_clear_max_gs_receive_size

  subroutine token_builder_t_pes_and_addresses(&
    builder, away_globals, pes, addresses)
    class(token_builder_t), intent(in) :: builder
    integer(INT64) :: away_globals(:)
    integer :: pes(:)
    integer(INT32) :: addresses(:)

    call comm_token_builder_pes_and_addresses(&
      builder%builder, &
      away_globals, size(away_globals, 1, c_size_t), &
      pes, size(pes, 1, c_size_t), &
      addresses, size(addresses, 1, c_size_t))
  end subroutine token_builder_t_pes_and_addresses

  subroutine token_builder_t_flag_pes(builder, away_globals, flag_pes)
    class(token_builder_t), intent(in) :: builder
    integer(INT64) :: away_globals(:)
    integer :: flag_pes(:)

    call comm_token_builder_flag_pes(&
      builder%builder, &
      away_globals, size(away_globals, 1, c_size_t), &
      flag_pes, size(flag_pes, 1, c_size_t))
  end subroutine token_builder_t_flag_pes

  function token_builder_t_build_global(&
    builder, home_addresses, away_globals) result(token)
    class(token_builder_t), intent(in) :: builder
    integer(INT32), intent(in) :: home_addresses(:)
    integer(INT64), intent(in) :: away_globals(:)
    type(token_t) :: token

    call comm_token_builder_build_global(&
      builder%builder, &
      home_addresses, size(home_addresses, 1, c_size_t), &
      away_globals, size(away_globals, 1, c_size_t), &
      token%token)
  end function token_builder_t_build_global

  function token_builder_t_build_local(&
    builder, home_addresses, away_pe, away_address) result(token)
    class(token_builder_t), intent(in) :: builder
    integer(INT32), intent(in) :: home_addresses(:)
    integer, intent(in) :: away_pe(:)
    integer(INT32), intent(in) :: away_address(:)
    type(token_t) :: token

    call comm_token_builder_build_local(&
      builder%builder, &
      home_addresses, size(home_addresses, 1, c_size_t), &
      away_pe, size(away_pe, 1, c_size_t), &
      away_address, size(away_address, 1, c_size_t), &
      token%token)
  end function token_builder_t_build_local

  function token_t_clone(token) result (new_token) 
    class(token_t), intent(in) :: token
    type(token_t) :: new_token

    if (token%is_associated()) then
      call comm_token_clone(token%token,new_token%token)
    end if
  end function token_t_clone

  subroutine token_t_free(token)
    class(token_t), intent(inout) :: token

    if (token%is_associated()) call comm_token_free(token%token)
    token%token = c_null_ptr
  end subroutine token_t_free

  logical function token_t_is_associated(token)
    class(token_t), intent(in) :: token

    token_t_is_associated = c_associated(token%token)
  end function token_t_is_associated

  integer(INT32) function token_t_home_num(self)
    class(token_t), intent(in) :: self

    integer(c_size_t) :: home_num
    call comm_token_get_home_num(self%token, home_num)
    token_t_home_num = int(home_num, INT32)
  end function token_t_home_num

  integer(INT32) function token_t_home_size(self)
    class(token_t), intent(in) :: self

    integer(c_size_t) :: home_size
    call comm_token_get_home_size(self%token, home_size)
    token_t_home_size = int(home_size, INT32)
  end function token_t_home_size

  subroutine token_t_fill_home_arrays(self, ranks, los, lengths, indices)
    class(token_t), intent(in) :: self
    integer(INT32) :: ranks(:), los(:), lengths(:), indices(:)

    call comm_token_fill_home_arrays_f(&
      self%token, size(ranks, 1, c_size_t), ranks, los, lengths, &
      size(indices, 1, c_size_t), indices)
  end subroutine token_t_fill_home_arrays

  subroutine token_t_get_l(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:)
    logical(c_bool), target, intent(out) :: output(:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_get_l

  subroutine token_t_get_i32(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:)
    integer(INT32), target, intent(out) :: output(:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_get_i32

  subroutine token_t_get_i64(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:)
    integer(INT64), target, intent(out) :: output(:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_get_i64

  subroutine token_t_get_r32(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:)
    real(REAL32), target, intent(out) :: output(:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_get_r32

  subroutine token_t_get_r64(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:)
    real(REAL64), target, intent(out) :: output(:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_get_r64

  subroutine token_t_get_l_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:,:)
    logical(c_bool), target, intent(out) :: output(:,:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_get_l_2d

  subroutine token_t_get_i32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:,:)
    integer(INT32), target, intent(out) :: output(:,:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_get_i32_2d

  subroutine token_t_get_i64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:,:)
    integer(INT64), target, intent(out) :: output(:,:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_get_i64_2d

  subroutine token_t_get_r32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:,:)
    real(REAL32), target, intent(out) :: output(:,:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_get_r32_2d

  subroutine token_t_get_r64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call comm_token_get(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_get_r64_2d

  subroutine token_t_get_inv_l_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:,:)
    logical(c_bool), target, intent(out) :: output(:,:)

    call comm_token_get_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_get_inv_l_2d

  subroutine token_t_get_inv_i32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:,:)
    integer(INT32), target, intent(out) :: output(:,:)

    call comm_token_get_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_get_inv_i32_2d

  subroutine token_t_get_inv_i64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:,:)
    integer(INT64), target, intent(out) :: output(:,:)

    call comm_token_get_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_get_inv_i64_2d

  subroutine token_t_get_inv_r32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:,:)
    real(REAL32), target, intent(out) :: output(:,:)

    call comm_token_get_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_get_inv_r32_2d

  subroutine token_t_get_inv_r64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call comm_token_get_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_get_inv_r64_2d

  subroutine token_t_put_l(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:)
    logical(c_bool), target, intent(out) :: output(:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_put_l

  subroutine token_t_put_i32(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:)
    integer(INT32), target, intent(out) :: output(:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_put_i32

  subroutine token_t_put_i64(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:)
    integer(INT64), target, intent(out) :: output(:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_put_i64

  subroutine token_t_put_r32(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:)
    real(REAL32), target, intent(out) :: output(:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_put_r32

  subroutine token_t_put_r64(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:)
    real(REAL64), target, intent(out) :: output(:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_put_r64

  subroutine token_t_put_l_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:,:)
    logical(c_bool), target, intent(out) :: output(:,:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_put_l_2d

  subroutine token_t_put_i32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:,:)
    integer(INT32), target, intent(out) :: output(:,:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_put_i32_2d

  subroutine token_t_put_i64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:,:)
    integer(INT64), target, intent(out) :: output(:,:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_put_i64_2d

  subroutine token_t_put_r32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:,:)
    real(REAL32), target, intent(out) :: output(:,:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_put_r32_2d

  subroutine token_t_put_r64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call comm_token_put(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_put_r64_2d

  subroutine token_t_put_inv_l_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    logical(c_bool), target, intent(in) :: input(:,:)
    logical(c_bool), target, intent(out) :: output(:,:)

    call comm_token_put_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_bool)
  end subroutine token_t_put_inv_l_2d

  subroutine token_t_put_inv_i32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT32), target, intent(in) :: input(:,:)
    integer(INT32), target, intent(out) :: output(:,:)

    call comm_token_put_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int32)
  end subroutine token_t_put_inv_i32_2d

  subroutine token_t_put_inv_i64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    integer(INT64), target, intent(in) :: input(:,:)
    integer(INT64), target, intent(out) :: output(:,:)

    call comm_token_put_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_int64)
  end subroutine token_t_put_inv_i64_2d

  subroutine token_t_put_inv_r32_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL32), target, intent(in) :: input(:,:)
    real(REAL32), target, intent(out) :: output(:,:)

    call comm_token_put_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_float)
  end subroutine token_t_put_inv_r32_2d

  subroutine token_t_put_inv_r64_2d(token, dowhat, input, output)
    class(token_t), intent(in) :: token
    integer, intent(in) :: dowhat
    real(REAL64), target, intent(in) :: input(:,:)
    real(REAL64), target, intent(out) :: output(:,:)

    call comm_token_put_inv(token%token, dowhat, &
      to_nd_array(input), &
      to_nd_array(output), &
      ct_double)
  end subroutine token_t_put_inv_r64_2d
end module token