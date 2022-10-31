#include "comm-mpi.F90.h"

program mesh_cells_test
  use, intrinsic :: iso_fortran_env

  MPI_USE

  use mesh_dzn
  use utility_kokkos

  implicit none

  MPI_INCLUDE

  integer :: level_set, mpierror
  
  call MPI_Init(mpierror)
  call kokkos_initialize
  
  block
    type(dzn_t) :: dzn
    call dzn_t_create(dzn)

    call dzn%push( &
      ! Kode::Unconditional
      options = 200, &
      level = 4, &
      x = [-2.0_REAL64, 2.0_REAL64] &
    )

    call dzn%push( &
      ! Kode::Unconditional
      options = 200, &
      level = 2, &
      x = [-1.0_REAL64, 1.0_REAL64], &
      xd = [0.5_REAL64, -0.5_REAL64] &
    )

    call dzn_split_filter_kode(&
      dzn, 1, 0.0_REAL64, 2, 1, dzn%numdzn, 0.75_REAL64, 0.0_REAL64, &
      0.0_REAL64, 0, level_set)
    if (level_set .ne. 2) error stop "Point should be in 2nd dzn at time 0"

    call dzn_split_filter_kode(&
      dzn, 1, 1.0_REAL64, 2, 1, dzn%numdzn, 0.75_REAL64, 0.0_REAL64, &
      0.0_REAL64, 0, level_set)
    if (level_set .ne. 4) error stop "Point should be in 1st dzn at time 1"

    level_set = 0
    call dzn_split_filter_kode(&
      dzn, 1, 0.0_REAL64, 2, 1, dzn%numdzn, 3.0_REAL64, 0.0_REAL64, &
      0.0_REAL64, 0, level_set)
    if (level_set .ne. 0) error stop "Outside of dzn, level_set is not updated"
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program