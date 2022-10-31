#include "comm-mpi.F90.h"

program comm_move_test
  use, intrinsic :: iso_fortran_env

  MPI_USE
  
  use comm_patterns, only : comm_move
  use utility_kokkos, only : kokkos_initialize, kokkos_finalize

  implicit none

  MPI_INCLUDE

  integer :: mype, numpe, maxpe
  integer :: mpierror
  integer :: n, m

  integer, dimension(:), allocatable :: &
    send_start, send_length, recv_start, recv_length, send_data, recv_data

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_rank(MPI_COMM_WORLD, mype, mpierror)
  call MPI_Comm_size(MPI_COMM_WORLD, numpe, mpierror)

  maxpe = numpe - 1

  allocate(&
    send_start(0:maxpe), send_length(0:maxpe), &
    recv_start(0:maxpe), recv_length(0:maxpe), source=0)

  recv_start(0) = 1
  recv_length(0) = mype + 1

  allocate(recv_data(mype + 1))
  recv_data = 0

  if (mype .eq. 0) then
    allocate(send_data(numpe * (numpe + 1) / 2))

    send_length = [(n, n = 1,numpe)]

    do n = 0, maxpe
      send_start(n) = (n * (n + 1) / 2) + 1

      do m = 0, send_length(n) - 1
        send_data(send_start(n) + m) = n
      end do
    end do
  else
    allocate(send_data(0))
  end if

  call comm_move(&
    MPI_COMM_WORLD, send_start, send_length, send_data, &
    recv_start, recv_length, recv_data)

  if (any(recv_data .ne. mype)) then
    write(error_unit, *) "recv_data was not the expected answer!"
    stop 1
  end if

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program comm_move_test