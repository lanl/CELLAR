#include "comm-mpi.F90.h"

program get_put_double_v
  !
  ! This test builds, for each rank, a numpe-by-numpe matrix with the
  ! following data:
  !
  !   0: | 1.1| 5.1| 9.1|13.1|
  !      | 2.1| 6.1|10.1|14.1|
  !      | 3.1| 7.1|11.1|15.1|
  !      | 4.1| 8.1|12.1|16.1|
  !
  !   1: | 1.2| 5.2| 9.2|13.2|
  !      | 2.2| 6.2|10.2|14.2|
  !      | 3.2| 7.2|11.2|15.2|
  !      | 4.2| 8.2|12.2|16.2|
  !
  !   2: | 1.3| 5.3| 9.3|13.3|
  !      | 2.3| 6.3|10.3|14.3|
  !      | 3.3| 7.3|11.3|15.3|
  !      | 4.3| 8.3|12.3|16.3|
  !
  !   3: | 1.4| 5.4| 9.4|13.4|
  !      | 2.4| 6.4|10.4|14.4|
  !      | 3.4| 7.4|11.4|15.4|
  !      | 4.4| 8.4|12.4|16.4|
  !
  ! Each rank accesses the mype+1 row from each other rank corresponding to
  ! mype.
  !
  ! This test builds a token object corresponding to this requirement,
  ! retrieves the data from the remote ranks, validates it receives the
  ! correct data, multiplies by (mype+1), and returns it to the remote ranks.
  !
  ! The received data in the case of rank 0 is:
  !
  !   0: | 1.1| 5.1| 9.1|13.1|
  !      | 1.2| 5.2| 9.2|13.2|
  !      | 1.3| 5.3| 9.3|13.3|
  !      | 1.4| 5.4| 9.4|13.4|
  !
  ! The final values in the grid should be:
  !
  !   0: | 1.1| 5.1| 9.1|13.1|
  !      | 4.2|12.2|20.2|28.2|
  !      | 9.3|21.3|33.3|45.3|
  !      |16.4|32.4|48.4|64.4|
  !
  !   1: | 1.2| 5.2| 9.2|13.2|
  !      | 4.4|12.4|20.4|28.4|
  !      | 9.6|21.6|33.6|45.6|
  !      |16.8|32.8|48.8|64.8|
  !
  !   2: | 1.3| 5.3| 9.3|13.3|
  !      | 4.6|12.6|20.6|28.6|
  !      | 9.9|21.9|33.9|45.9|
  !      |17.2|33.2|49.2|65.2|
  !
  !   3: | 1.4| 5.4| 9.4|13.4|
  !      | 2.8| 6.8|10.8|14.8|
  !      |10.2|22.2|34.2|46.2|
  !      |17.6|33.6|49.6|65.6|
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
  logical :: passed = .true.

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_rank(MPI_COMM_WORLD, mype, mpierror)
  call MPI_Comm_size(MPI_COMM_WORLD, numpe, mpierror)

  block
    real(REAL64), dimension(numpe,numpe) :: mydata, received_data, get_ans, &
                                            put_ans
    integer, dimension(numpe) :: ltop
    integer(INT64), dimension(numpe) :: global_needed
    integer :: i, j

    do i = 1, numpe
      do j = 1, numpe
        mydata(j, i) = (i - 1) * numpe + j + ((mype + 1) * 0.1_REAL64)
      end do
    end do

    do i = 1, numpe
      ltop(i) = i
    end do

    do i = 1, numpe
      global_needed(i) = numpe * (i - 1) + mype + 1
    end do

    do i = 1, numpe
      do j = 1, numpe
        get_ans(j, i) = (i - 1) * numpe + mype + 1 + j * 0.1_REAL64
      end do
    end do

    put_ans = mydata

    do j = 1, numpe
      put_ans(j, :) = put_ans(j, :) * j
    end do

    block
      type(token_builder_t) :: builder
      type(token_t) :: token

      builder = new_token_builder(MPI_COMM_WORLD);
      call builder%set_num_cell(numpe)

      token = builder%build_global(ltop, global_needed)

      call token%get(cto_copy, mydata, received_data)

      if (any(get_ans .ne. received_data)) then
        write(error_unit, *) "get_ans was not equal to received_data!"
        passed = .false.
      end if

      received_data = received_data * (mype + 1)

      call token%put(cto_copy, received_data, mydata)

      if (any(put_ans .ne. mydata)) then
        write(error_unit, *) "put_ans was not equal to mydata!"
        passed = .false.
      end if

      call token%free()
      call builder%free()
    end block
  end block

  call kokkos_finalize
  call MPI_Finalize(mpierror)

  if (.not. passed) stop 1
end program get_put_double_v