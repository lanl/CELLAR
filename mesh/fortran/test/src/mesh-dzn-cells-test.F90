#include "comm-mpi.F90.h"

program mesh_cells_test
  use, intrinsic :: iso_fortran_env

  MPI_USE

  use mesh_cells
  use mesh_constants
  use mesh_dzn
  use token
  use utility_kokkos

  implicit none

  MPI_INCLUDE

  integer :: mpierror

  call MPI_Init(mpierror)
  call kokkos_initialize
  
  block
    type(cpp_cells_t) :: cells
    type(dzn_t) :: dzn
    type(token_builder_t) :: builder
    integer :: n

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

    builder = new_token_builder(MPI_COMM_WORLD)
    cells = eap_mesh_new_cpp_cells_t(builder)

    call cells%resize_cell_arrays(size = 100, num_dims = 1)

    ! spread cells across range of DZNs
    do n = 1,100
      cells%cpp_cell_center(n, X_DIR) = &
        real((n - 1), REAL64) / 100.0_REAL64 * 6.0_REAL64 - 3.0_REAL64
    end do

    block
      integer :: level_base(100), level_set(100)

      level_base = 0
      level_set = 0

      call dzn_process_cells_in_dzn_filter_kode(&
        dzn, 1, cells, 0.0_REAL64, 2, 1, 100, level_base, level_set)

      do n = 1,100
        block
          real(REAL64) :: center

          center = real((n - 1), REAL64) / 100.0_REAL64 * 6.0_REAL64 - 3.0_REAL64
          if (abs(center) > 2.0_REAL64) then
            if (level_set(n) .ne. 0) &
              error stop "Incorrect level"
          else if (abs(center) > 1.0_REAL64) then
            if (level_set(n) .ne. 4) &
              error stop "Incorrect level"
          else
            if (level_set(n) .ne. 2) &
              error stop "Incorrect level"
          endif
        end block
      end do
    end block
  end block

  call MPI_Finalize(mpierror)
  call kokkos_finalize
end program