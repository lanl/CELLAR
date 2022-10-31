program test_2d_array
  use abi
  use, intrinsic :: iso_c_binding

  integer, target :: my_2d_array(4,4)
  integer, pointer :: my_2d_view(:,:)

  type(nd_array_t) :: array_desc

  integer(c_size_t) :: expected_strides(2), expected_dims(2)

  my_2d_view => my_2d_array(1:4:2, 1:1)

  array_desc = to_nd_array(my_2d_view)
  expected_dims = [2, 1]
  expected_strides = [2, 4]

  if (.not. all(array_desc%strides(1:2) .eq. expected_strides)) then
    write(*,*)"Invalid strides!"
    stop 1
  end if

  if (.not. all(array_desc%dims(1:2) .eq. expected_dims)) then
    write(*,*)"Invalid dims!"
    stop 1
  end if

end program test_2d_array