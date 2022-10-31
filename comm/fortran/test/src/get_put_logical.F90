#include "comm-mpi.F90.h"

program get_put_logical
  !
  ! This test builds a virtual grid that looks like this:
  !
  !   0: |F|F|F|F|F|F|F|F|F|F|
  !   1: |F|F|F|F|F|F|F|F|F|F|F|
  !   2: |F|F|F|F|F|F|F|F|F|F|F|F|
  !   3: |F|F|F|F|F|F|F|F|F|F|F|F|F|
  !
  ! Notice that each row in the grid is a different size.
  !
  ! Each rank requires the global column corresponding to 1+mype*2.
  !
  ! This test builds a token object corresponding to this requirement,
  ! retrieves the data from the remote ranks, validates it receives the
  ! correct data, inverts it, and returns it to the remote ranks.
  !
  ! The final values in the grid should be:
  !
  !   0: |T|F|T|F|T|F|T|F|F|F|
  !   1: |T|F|T|F|T|F|T|F|F|F|F|
  !   2: |T|F|T|F|T|F|T|F|F|F|F|F|
  !   3: |T|F|T|F|T|F|T|F|F|F|F|F|F|
  !
  ! For numpes of 1 and 2, there are fewer rows in the grid.
  !
  use, intrinsic :: iso_fortran_env
  use, intrinsic :: iso_c_binding
  
  MPI_USE
  
  use token
  use utility_kokkos, only : kokkos_initialize, kokkos_finalize

  implicit none

  MPI_INCLUDE

  integer :: mype, numpe
  integer :: mpierror
  integer :: cellnum
  integer :: num_needed
  integer, allocatable :: ltop(:)
  integer(c_size_t),dimension(:), allocatable :: global_needed
  logical(c_bool), dimension(:), allocatable :: get_ans, put_ans, mydata, &
                                                received_data
  integer :: n, i, j
  logical :: passed = .true.

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_rank(MPI_COMM_WORLD, mype, mpierror)
  call MPI_Comm_size(MPI_COMM_WORLD, numpe, mpierror)

  cellnum = 10 + mype
  num_needed = numpe
  allocate(global_needed(num_needed))
  allocate(ltop(num_needed), source = [(n, n = 1, num_needed)])
  allocate(mydata(cellnum))
  allocate(received_data(num_needed))
  allocate(get_ans(num_needed))
  allocate(put_ans(cellnum))

  mydata = .false.

  i = 1
  j = 10
  do n=1,num_needed
    global_needed(n) = int(i,INT64) + int(mype,INT64) * 2
    i = i + j
    j = j + 1
  enddo

  get_ans = .false.

  put_ans = mydata
  do n=0, num_needed - 1
    put_ans(1 + n * 2) = .not. put_ans(1 + n * 2)
  end do

  block
    type(token_builder_t) :: builder
    type(token_t) :: token

    builder = new_token_builder(MPI_COMM_WORLD);
    call builder%set_num_cell(cellnum)

    token = builder%build_global(ltop, global_needed)

    call token%get(cto_copy, mydata, received_data)

    if (any(get_ans .neqv. received_data)) then
      write(error_unit, *) "get_ans was not equal to received_data!"
      passed = .false.
    end if

    received_data = .not. received_data

    call token%put(cto_copy, received_data, mydata)

    if (any(put_ans .neqv. mydata)) then
      write(error_unit, *) "put_ans was not equal to mydata!"
      passed = .false.
    end if

    call token%free()
    call builder%free()
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)

  if (.not. passed) stop 1
end program get_put_logical