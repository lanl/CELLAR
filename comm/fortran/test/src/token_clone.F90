#include "comm-mpi.F90.h"

program token_clone
  !
  ! This test builds a virtual grid that looks like this:
  !
  !   0: | 1.1| 2.1| 3.1| 4.1| 5.1| 6.1| 7.1| 8.1| 9.1|10.1|
  !   1: | 1.2| 2.2| 3.2| 4.2| 5.2| 6.2| 7.2| 8.2| 9.2|10.2|11.2|
  !   2: | 1.3| 2.3| 3.3| 4.3| 5.3| 6.3| 7.3| 8.3| 9.3|10.3|11.3|12.3|
  !   3: | 1.4| 2.4| 3.4| 4.4| 5.4| 6.4| 7.4| 8.4| 9.4|10.4|11.4|12.4|13.4|
  !
  ! Notice that each row in the grid is a different size.
  !
  ! Each rank requires the global column corresponding to 1+mype*2.
  !
  ! This test builds a token object corresponding to this requirement,
  ! retrieves the data from the remote ranks, validates it receives the
  ! correct data, doubles it, and returns it to the remote ranks.
  !
  ! The final values in the grid should be:
  !
  !   0: | 2.2| 2.1| 6.2| 4.1|10.2| 6.1|14.2| 8.1| 9.1|10.1|
  !   1: | 2.4| 2.2| 6.4| 4.2|10.4| 6.2|14.4| 8.2| 9.2|10.2|11.2|
  !   2: | 2.6| 2.3| 6.6| 4.3|10.6| 6.3|14.6| 8.3| 9.3|10.3|11.3|12.3|
  !   3: | 2.8| 2.4| 6.8| 4.4|10.8| 6.4|14.8| 8.4| 9.4|10.4|11.4|12.4|13.4|
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
  integer(c_size_t), dimension(:), allocatable :: global_needed
  real(REAL32), dimension(:), allocatable :: get_ans, put_ans, mydata, &
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

  do n=1,cellnum
    mydata(n) = n+((mype+1)*0.1_REAL32)
  enddo

  i = 1
  j = 10
  do n=1,num_needed
    global_needed(n) = int(i,INT64) + int(mype,INT64) * 2
    i = i + j
    j = j + 1
  enddo

  do n= 1,num_needed
    get_ans(n) = mype * 2 + 1 + (0.1_REAL32 * n)
  end do

  put_ans = mydata
  do n=0, num_needed - 1
    put_ans(1 + n * 2) = put_ans(1 + n * 2) * 2
  end do

  block
    type(token_builder_t) :: builder
    type(token_t) :: token, token_c1

    builder = new_token_builder(MPI_COMM_WORLD);
    call builder%set_num_cell(cellnum)
    call builder%require_rank_order_request_completion(.true.)

    token = builder%build_global(ltop, global_needed)

    !clone token and do same operation as get_put_float to see if cloning works

    token_c1 = token%clone()

    ! see if clone still exists after freeing original 
    call token%free()
     
    call token_c1%get(cto_copy, mydata, received_data)

    if (any(get_ans .ne. received_data)) then
      write(error_unit, *) "get_ans was not equal to received_data!"
      passed = .false.
    end if

    received_data = received_data * 2

    call token_c1%put(cto_copy, received_data, mydata)

    if (any(put_ans .ne. mydata)) then
      write(error_unit, *) "put_ans was not equal to mydata!"
      passed = .false.
    end if

    call token_c1%free()
    call builder%free()
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)

  if (.not. passed) stop 1
end program token_clone