#include "comm-mpi.F90.h"

program get_put_int64_v
  !
  ! This test builds, for each rank, a numpe-by-numpe matrix with the
  ! following data:
  !
  !   0: | 11| 51| 91|131|
  !      | 21| 61|101|141|
  !      | 31| 71|111|151|
  !      | 41| 81|121|161|
  !
  !   1: | 12| 52| 92|132|
  !      | 22| 62|102|142|
  !      | 32| 72|112|152|
  !      | 42| 82|122|162|
  !
  !   2: | 13| 53| 93|133|
  !      | 23| 63|103|143|
  !      | 33| 73|113|153|
  !      | 43| 83|123|163|
  !
  !   3: | 14| 54| 94|134|
  !      | 24| 64|104|144|
  !      | 34| 74|114|154|
  !      | 44| 84|124|164|
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
  !   0: | 11| 51| 91|131|
  !      | 12| 52| 92|132|
  !      | 13| 53| 93|133|
  !      | 14| 54| 94|134|
  !
  ! The final values in the grid should be:
  !
  !   0: | 11| 51| 91|131|
  !      | 42|122|202|282|
  !      | 93|213|333|453|
  !      |164|324|484|644|
  !
  !   1: | 12| 52| 92|132|
  !      | 44|124|204|284|
  !      | 96|216|336|456|
  !      |168|328|488|648|
  !
  !   2: | 13| 53| 93|133|
  !      | 46|126|206|286|
  !      | 99|219|339|459|
  !      |172|332|492|652|
  !
  !   3: | 14| 54| 94|134|
  !      | 28| 68|108|148|
  !      |102|222|342|462|
  !      |176|336|496|656|
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
    integer(INT64), dimension(numpe,numpe) :: mydata, received_data, get_ans, &
                                              put_ans
    integer, dimension(numpe) :: ltop
    integer(INT64), dimension(numpe) :: global_needed
    integer :: i, j

    do i = 1, numpe
      do j = 1, numpe
        mydata(j, i) = ((i - 1) * numpe + j) * 10 + mype + 1
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
        get_ans(j, i) = ((i - 1) * numpe + mype + 1) * 10 + j
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
end program get_put_int64_v