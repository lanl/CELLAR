#include "comm-mpi.F90.h"

program rma_all_to_all
  use, intrinsic :: iso_fortran_env
  use, intrinsic :: iso_c_binding

  MPI_USE
  
  use comm_patterns
  use utility_kokkos, only : kokkos_initialize, kokkos_finalize

  implicit none

  MPI_INCLUDE

  integer :: rank, size
  integer :: mpierror

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_size(MPI_COMM_WORLD, size, mpierror)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, mpierror)

  block
    type(rma_all_to_all_int_t) :: rma
    integer :: recv(0:size-1)
    integer :: n

    rma = new_rma_all_to_all_int_t(MPI_COMM_WORLD, 1)

    call rma%all_to_all([(mod(rank, 2), n = 0, size - 1)], recv)

    if (any(recv .ne. [(mod(n, 2), n = 0, size - 1)])) then
      write(error_unit, *) "recv was not the expected answer!"
      stop 1
    end if
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program rma_all_to_all
