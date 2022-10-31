#include "comm-mpi.F90.h"

program mesh_cells_test
  use mesh_cells
  use token
  use utility_kokkos

  MPI_USE

  implicit none

  MPI_INCLUDE

  type(cpp_cells_t) :: cells
  type(token_builder_t) :: builder
  integer :: mpierror

  call MPI_Init(mpierror)
  call kokkos_initialize

  builder = new_token_builder(MPI_COMM_WORLD)

  cells = eap_mesh_new_cpp_cells_t(builder)

  call cells%free()
  call builder%free()

  call kokkos_finalize
  call MPI_Finalize(mpierror)
end program