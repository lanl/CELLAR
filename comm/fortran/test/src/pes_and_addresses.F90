#include "comm-mpi.F90.h"

program pes_and_addresses
  use, intrinsic :: iso_fortran_env
  use, intrinsic :: iso_c_binding

  MPI_USE
  
  use token
  use utility_kokkos, only : kokkos_initialize, kokkos_finalize

  implicit none

  MPI_INCLUDE

  type(token_builder_t) :: builder
  integer :: numcells(0:3) = [7, 0, 6, 10]
  integer :: rank, size
  integer :: mpierror

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_size(MPI_COMM_WORLD, size, mpierror)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, mpierror)

  if (size .ne. 4) then
    write(error_unit, *) "This test is only valid for 4 ranks"
    stop 1
  end if

  builder = new_token_builder(MPI_COMM_WORLD)
  call builder%set_num_cell(numcells(rank))

  block
    integer(INT64) :: test(4) = [2, 3, 9, 11]
    integer :: expected(0:3) = [1, 0, 1, 0]
    integer :: flag_pes(0:3) = 0

    call builder%flag_pes(test, flag_pes)

    if (any(flag_pes .ne. expected)) then
      write(*,*) flag_pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if
  end block

  block
    integer(INT64) :: test(4) = [1, 8, 8, 14]
    integer :: expected(0:3) = [1, 0, 1, 1]
    integer :: flag_pes(0:3) = 0

    call builder%flag_pes(test, flag_pes)

    if (any(flag_pes .ne. expected)) then
      write(*,*) flag_pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if
  end block

  block
    integer(INT64) :: test(4) = [1, 8, 8, 14]
    integer :: pes(4), addresses(4)
    integer :: expected_pes(4) = [0, 2, 2, 3]
    integer :: expected_addresses(4) = [1, 1, 1, 1]

    call builder%pes_and_addresses(test, pes, addresses)

    if (any(pes .ne. expected_pes)) then
      write(*,*) pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if

    if (any(addresses .ne. expected_addresses)) then
      write(*,*) pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if
  end block

  block
    integer(INT64) :: test(6) = [2, 3, 9, 10, 15, 16]
    integer :: pes(6), addresses(6)
    integer :: expected_pes(6) = [0, 0, 2, 2, 3, 3]
    integer :: expected_addresses(6) = [2, 3, 2, 3, 2, 3]

    call builder%pes_and_addresses(test, pes, addresses)

    if (any(pes .ne. expected_pes)) then
      write(*,*) pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if

    if (any(addresses .ne. expected_addresses)) then
      write(*,*) pes
      write(error_unit, *) "unexpected value!"
      stop 1
    end if
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program pes_and_addresses