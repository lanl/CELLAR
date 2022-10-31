#include "comm-mpi.F90.h"

module comm_patterns
  use, intrinsic :: iso_c_binding
  use, intrinsic :: iso_fortran_env
  use abi
  use comm_types

  MPI_USE

  implicit none
  private

  MPI_INCLUDE

  public rma_all_to_all_int_t
  public new_rma_all_to_all_int_t
  public comm_move

  type :: rma_all_to_all_int_t
    type(c_ptr), private :: rma = c_null_ptr
  contains
    procedure :: free => rma_all_to_all_int_t_free

    procedure :: is_associated => rma_all_to_all_int_t_is_associated

    procedure :: handle => rma_all_to_all_int_t_handle

    procedure :: all_to_all => rma_all_to_all_int_t_all_to_all
  end type rma_all_to_all_int_t

  interface
    subroutine comm_rma_all_to_all_create(&
      comm, datatype, count, rma_all_to_all) &
      bind(C, name="comm_rma_all_to_all_create_f")
      import

#ifdef TOKEN_USE_MPI_F08
      type(MPI_Comm), value, intent(in) :: comm
#else
      integer(c_int), value, intent(in) :: comm
#endif
      integer(c_int), value, intent(in) :: datatype
      integer(c_int), value, intent(in) :: count
      type(c_ptr), intent(out) :: rma_all_to_all
    end subroutine comm_rma_all_to_all_create

    subroutine comm_rma_all_to_all_free(rma_all_to_all) &
      bind(C, name="comm_rma_all_to_all_free")
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: rma_all_to_all
    end subroutine comm_rma_all_to_all_free

    subroutine comm_rma_all_to_all_invoke(rma_all_to_all, send, recv) &
      bind(C, name="comm_rma_all_to_all_invoke")
      use, intrinsic :: iso_c_binding
      use abi

      type(c_ptr), value, intent(in) :: rma_all_to_all
      type(nd_array_t), intent(in) :: send, recv
    end subroutine comm_rma_all_to_all_invoke

    subroutine eap_comm_move(&
      comm, datatype, send_start, send_length, send_data, &
      recv_start, recv_length, recv_data) &
      bind(C, name="eap_comm_move_f")
      import

#ifdef TOKEN_USE_MPI_F08
      type(MPI_Comm), value, intent(in) :: comm
#else
      integer(c_int), value, intent(in) :: comm
#endif
      integer(c_int), value, intent(in) :: datatype
      integer(c_int), dimension(*), intent(in) :: &
        send_start, send_length, recv_start, recv_length
      type(nd_array_t), intent(in) :: send_data, recv_data
    end subroutine eap_comm_move
  end interface

  interface comm_move
    module procedure comm_move_l_c_bool
    module procedure comm_move_i32
    module procedure comm_move_i64
    module procedure comm_move_r32
    module procedure comm_move_r64
  end interface comm_move
contains
  function new_rma_all_to_all_int_t(comm, count) result(rma)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif
    integer, intent(in) :: count

    type(rma_all_to_all_int_t) :: rma

    call comm_rma_all_to_all_create(comm, ct_int32, count, rma%rma)
  end function new_rma_all_to_all_int_t

  subroutine rma_all_to_all_int_t_free(rma)
    class(rma_all_to_all_int_t), intent(inout) :: rma

    call comm_rma_all_to_all_free(rma%rma)
  end subroutine rma_all_to_all_int_t_free

  function rma_all_to_all_int_t_is_associated(rma)
    class(rma_all_to_all_int_t), intent(in) :: rma
    logical :: rma_all_to_all_int_t_is_associated

    rma_all_to_all_int_t_is_associated = c_associated(rma%rma)
  end function rma_all_to_all_int_t_is_associated

  function rma_all_to_all_int_t_handle(rma)
    class(rma_all_to_all_int_t), intent(inout) :: rma
    type(c_ptr) :: rma_all_to_all_int_t_handle

    rma_all_to_all_int_t_handle = rma%rma
  end function rma_all_to_all_int_t_handle

  subroutine rma_all_to_all_int_t_all_to_all(rma, send, recv)
    class(rma_all_to_all_int_t), intent(inout) :: rma
    integer, target, intent(in) :: send(:)
    integer, target, intent(out) :: recv(:)

    call comm_rma_all_to_all_invoke(&
      rma%rma, to_nd_array(send), to_nd_array(recv))
  end subroutine rma_all_to_all_int_t_all_to_all

  subroutine comm_move_l_c_bool(&
    comm, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif

    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length
    logical(c_bool), dimension(:), target, intent(in) :: send_data
    logical(c_bool), dimension(:), target, intent(inout) :: recv_data

    call eap_comm_move(&
      comm, ct_bool, send_start, send_length, to_nd_array(send_data), &
      recv_start, recv_length, to_nd_array(recv_data))
  end subroutine comm_move_l_c_bool

  subroutine comm_move_i32(&
    comm, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif

    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length
    integer(INT32), dimension(:), target, intent(in) :: send_data
    integer(INT32), dimension(:), target, intent(inout) :: recv_data

    call eap_comm_move(&
      comm, ct_int32, send_start, send_length, to_nd_array(send_data), &
      recv_start, recv_length, to_nd_array(recv_data))
  end subroutine comm_move_i32

  subroutine comm_move_i64(&
    comm, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif

    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length
    integer(INT64), dimension(:), target, intent(in) :: send_data
    integer(INT64), dimension(:), target, intent(inout) :: recv_data

    call eap_comm_move(&
      comm, ct_int64, send_start, send_length, to_nd_array(send_data), &
      recv_start, recv_length, to_nd_array(recv_data))
  end subroutine comm_move_i64

  subroutine comm_move_r32(&
    comm, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif

    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length
    real(REAL32), dimension(:), target, intent(in) :: send_data
    real(REAL32), dimension(:), target, intent(inout) :: recv_data

    call eap_comm_move(&
      comm, ct_float, send_start, send_length, to_nd_array(send_data), &
      recv_start, recv_length, to_nd_array(recv_data))
  end subroutine comm_move_r32

  subroutine comm_move_r64(&
    comm, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)
#ifdef TOKEN_USE_MPI_F08
    type(MPI_Comm), intent(in) :: comm
#else
    integer, intent(in) :: comm
#endif

    integer, dimension(:), intent(in) :: &
      send_start, send_length, recv_start, recv_length
    real(REAL64), dimension(:), target, intent(in) :: send_data
    real(REAL64), dimension(:), target, intent(inout) :: recv_data

    call eap_comm_move(&
      comm, ct_double, send_start, send_length, to_nd_array(send_data), &
      recv_start, recv_length, to_nd_array(recv_data))
  end subroutine comm_move_r64
end module comm_patterns