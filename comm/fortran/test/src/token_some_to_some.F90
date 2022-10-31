#include "comm-mpi.F90.h"

program token_some_to_some
  ! This contructs a virtual numpe x 1 grid. Rank pe contains row pe in
  ! the grid. Each rank requests the prior and following row (wrapping).
  use, intrinsic :: iso_fortran_env
  use, intrinsic :: iso_c_binding

  MPI_USE
  
  use token
  use utility_kokkos, only : kokkos_initialize, kokkos_finalize

  implicit none

  MPI_INCLUDE

  integer :: rank, size
  integer :: mpierror
  integer :: prev_rank, next_rank
  
  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, mpierror)
  call MPI_Comm_size(MPI_COMM_WORLD, size, mpierror)

  if (rank .eq. 0) then
    prev_rank = size - 1
  else
    prev_rank = rank - 1
  end if

  if (rank .eq. size - 1) then
    next_rank = 0
  else
    next_rank = rank + 1
  end if

  block
    type(token_builder_t) :: builder
    type(token_t) :: token

    integer :: to_pes(0:size-1)
    integer :: away_data(2)
    integer :: expected(2)

    to_pes = 0
    to_pes(prev_rank) = 1
    to_pes(next_rank) = 1
    
    expected = [prev_rank, next_rank]

    builder = new_token_builder(MPI_COMM_WORLD)
    call builder%set_num_cell(1)
    if (size .ne. 1) then
      call builder%set_to_pes(to_pes)
    end if

    token = builder%build_global(&
      [1, 2], [int(prev_rank + 1, INT64), int(next_rank + 1, INT64)])

    call token%get(cto_copy, [rank], away_data)

    if (any(away_data .ne. expected)) then
      write(error_unit,*) "did not match expected!"
      stop 1
    end if

    call token%free()
    call builder%free()
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program token_some_to_some