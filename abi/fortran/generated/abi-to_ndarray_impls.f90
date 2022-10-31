!!!! THIS FILE IS GENERATED - DO NOT EDIT DIRECTLY !!!!

  function to_nd_array_l_1d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)

    ndarray%rank = 1
    if (size(array) .eq. 0) then
      ndarray%strides(1:1) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2)), 1_c_size_t) - &
          transfer(c_loc(array(1)), 1_c_size_t)) / c_sizeof(array(1))
      else
        ndarray%strides(1) = 1
      end if

      ndarray%data = c_loc(array(1))
    end if
  end function to_nd_array_l_1d

  subroutine from_nd_array_l_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_l_1d

  function to_nd_array_l_2d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)

    ndarray%rank = 2
    if (size(array) .eq. 0) then
      ndarray%strides(1:2) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      ndarray%data = c_loc(array(1,1))
    end if
  end function to_nd_array_l_2d

  subroutine from_nd_array_l_2d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:2))
  end subroutine from_nd_array_l_2d

  function to_nd_array_l_3d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)

    ndarray%rank = 3
    if (size(array) .eq. 0) then
      ndarray%strides(1:3) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      ndarray%data = c_loc(array(1,1,1))
    end if
  end function to_nd_array_l_3d

  subroutine from_nd_array_l_3d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:3))
  end subroutine from_nd_array_l_3d

  function to_nd_array_l_4d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)

    ndarray%rank = 4
    if (size(array) .eq. 0) then
      ndarray%strides(1:4) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      ndarray%data = c_loc(array(1,1,1,1))
    end if
  end function to_nd_array_l_4d

  subroutine from_nd_array_l_4d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:4))
  end subroutine from_nd_array_l_4d

  function to_nd_array_l_5d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)

    ndarray%rank = 5
    if (size(array) .eq. 0) then
      ndarray%strides(1:5) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1))
    end if
  end function to_nd_array_l_5d

  subroutine from_nd_array_l_5d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:5))
  end subroutine from_nd_array_l_5d

  function to_nd_array_l_6d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)

    ndarray%rank = 6
    if (size(array) .eq. 0) then
      ndarray%strides(1:6) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1))
    end if
  end function to_nd_array_l_6d

  subroutine from_nd_array_l_6d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:6))
  end subroutine from_nd_array_l_6d

  function to_nd_array_l_7d(array) result(ndarray)
    logical(c_bool), target, intent(in) :: array(:,:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)
    ndarray%dims(7) = size(array, 7, c_size_t)

    ndarray%rank = 7
    if (size(array) .eq. 0) then
      ndarray%strides(1:7) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      if (size(array, 7) .ge. 2) then
        ndarray%strides(7) = &
          (transfer(c_loc(array(1,1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(7) = size(array, 6, c_size_t) * ndarray%strides(6)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1,1))
    end if
  end function to_nd_array_l_7d

  subroutine from_nd_array_l_7d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    logical(c_bool), pointer, intent(out) :: array(:,:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:7))
  end subroutine from_nd_array_l_7d



  function to_nd_array_i32_1d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)

    ndarray%rank = 1
    if (size(array) .eq. 0) then
      ndarray%strides(1:1) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2)), 1_c_size_t) - &
          transfer(c_loc(array(1)), 1_c_size_t)) / c_sizeof(array(1))
      else
        ndarray%strides(1) = 1
      end if

      ndarray%data = c_loc(array(1))
    end if
  end function to_nd_array_i32_1d

  subroutine from_nd_array_i32_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_i32_1d

  function to_nd_array_i32_2d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)

    ndarray%rank = 2
    if (size(array) .eq. 0) then
      ndarray%strides(1:2) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      ndarray%data = c_loc(array(1,1))
    end if
  end function to_nd_array_i32_2d

  subroutine from_nd_array_i32_2d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:2))
  end subroutine from_nd_array_i32_2d

  function to_nd_array_i32_3d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)

    ndarray%rank = 3
    if (size(array) .eq. 0) then
      ndarray%strides(1:3) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      ndarray%data = c_loc(array(1,1,1))
    end if
  end function to_nd_array_i32_3d

  subroutine from_nd_array_i32_3d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:3))
  end subroutine from_nd_array_i32_3d

  function to_nd_array_i32_4d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)

    ndarray%rank = 4
    if (size(array) .eq. 0) then
      ndarray%strides(1:4) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      ndarray%data = c_loc(array(1,1,1,1))
    end if
  end function to_nd_array_i32_4d

  subroutine from_nd_array_i32_4d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:4))
  end subroutine from_nd_array_i32_4d

  function to_nd_array_i32_5d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)

    ndarray%rank = 5
    if (size(array) .eq. 0) then
      ndarray%strides(1:5) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1))
    end if
  end function to_nd_array_i32_5d

  subroutine from_nd_array_i32_5d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:5))
  end subroutine from_nd_array_i32_5d

  function to_nd_array_i32_6d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)

    ndarray%rank = 6
    if (size(array) .eq. 0) then
      ndarray%strides(1:6) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1))
    end if
  end function to_nd_array_i32_6d

  subroutine from_nd_array_i32_6d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:6))
  end subroutine from_nd_array_i32_6d

  function to_nd_array_i32_7d(array) result(ndarray)
    integer(INT32), target, intent(in) :: array(:,:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)
    ndarray%dims(7) = size(array, 7, c_size_t)

    ndarray%rank = 7
    if (size(array) .eq. 0) then
      ndarray%strides(1:7) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      if (size(array, 7) .ge. 2) then
        ndarray%strides(7) = &
          (transfer(c_loc(array(1,1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(7) = size(array, 6, c_size_t) * ndarray%strides(6)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1,1))
    end if
  end function to_nd_array_i32_7d

  subroutine from_nd_array_i32_7d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT32), pointer, intent(out) :: array(:,:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:7))
  end subroutine from_nd_array_i32_7d



  function to_nd_array_i64_1d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)

    ndarray%rank = 1
    if (size(array) .eq. 0) then
      ndarray%strides(1:1) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2)), 1_c_size_t) - &
          transfer(c_loc(array(1)), 1_c_size_t)) / c_sizeof(array(1))
      else
        ndarray%strides(1) = 1
      end if

      ndarray%data = c_loc(array(1))
    end if
  end function to_nd_array_i64_1d

  subroutine from_nd_array_i64_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_i64_1d

  function to_nd_array_i64_2d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)

    ndarray%rank = 2
    if (size(array) .eq. 0) then
      ndarray%strides(1:2) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      ndarray%data = c_loc(array(1,1))
    end if
  end function to_nd_array_i64_2d

  subroutine from_nd_array_i64_2d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:2))
  end subroutine from_nd_array_i64_2d

  function to_nd_array_i64_3d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)

    ndarray%rank = 3
    if (size(array) .eq. 0) then
      ndarray%strides(1:3) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      ndarray%data = c_loc(array(1,1,1))
    end if
  end function to_nd_array_i64_3d

  subroutine from_nd_array_i64_3d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:3))
  end subroutine from_nd_array_i64_3d

  function to_nd_array_i64_4d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)

    ndarray%rank = 4
    if (size(array) .eq. 0) then
      ndarray%strides(1:4) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      ndarray%data = c_loc(array(1,1,1,1))
    end if
  end function to_nd_array_i64_4d

  subroutine from_nd_array_i64_4d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:4))
  end subroutine from_nd_array_i64_4d

  function to_nd_array_i64_5d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)

    ndarray%rank = 5
    if (size(array) .eq. 0) then
      ndarray%strides(1:5) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1))
    end if
  end function to_nd_array_i64_5d

  subroutine from_nd_array_i64_5d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:5))
  end subroutine from_nd_array_i64_5d

  function to_nd_array_i64_6d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)

    ndarray%rank = 6
    if (size(array) .eq. 0) then
      ndarray%strides(1:6) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1))
    end if
  end function to_nd_array_i64_6d

  subroutine from_nd_array_i64_6d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:6))
  end subroutine from_nd_array_i64_6d

  function to_nd_array_i64_7d(array) result(ndarray)
    integer(INT64), target, intent(in) :: array(:,:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)
    ndarray%dims(7) = size(array, 7, c_size_t)

    ndarray%rank = 7
    if (size(array) .eq. 0) then
      ndarray%strides(1:7) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      if (size(array, 7) .ge. 2) then
        ndarray%strides(7) = &
          (transfer(c_loc(array(1,1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(7) = size(array, 6, c_size_t) * ndarray%strides(6)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1,1))
    end if
  end function to_nd_array_i64_7d

  subroutine from_nd_array_i64_7d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    integer(INT64), pointer, intent(out) :: array(:,:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:7))
  end subroutine from_nd_array_i64_7d



  function to_nd_array_r32_1d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)

    ndarray%rank = 1
    if (size(array) .eq. 0) then
      ndarray%strides(1:1) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2)), 1_c_size_t) - &
          transfer(c_loc(array(1)), 1_c_size_t)) / c_sizeof(array(1))
      else
        ndarray%strides(1) = 1
      end if

      ndarray%data = c_loc(array(1))
    end if
  end function to_nd_array_r32_1d

  subroutine from_nd_array_r32_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_r32_1d

  function to_nd_array_r32_2d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)

    ndarray%rank = 2
    if (size(array) .eq. 0) then
      ndarray%strides(1:2) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      ndarray%data = c_loc(array(1,1))
    end if
  end function to_nd_array_r32_2d

  subroutine from_nd_array_r32_2d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:2))
  end subroutine from_nd_array_r32_2d

  function to_nd_array_r32_3d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)

    ndarray%rank = 3
    if (size(array) .eq. 0) then
      ndarray%strides(1:3) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      ndarray%data = c_loc(array(1,1,1))
    end if
  end function to_nd_array_r32_3d

  subroutine from_nd_array_r32_3d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:3))
  end subroutine from_nd_array_r32_3d

  function to_nd_array_r32_4d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)

    ndarray%rank = 4
    if (size(array) .eq. 0) then
      ndarray%strides(1:4) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      ndarray%data = c_loc(array(1,1,1,1))
    end if
  end function to_nd_array_r32_4d

  subroutine from_nd_array_r32_4d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:4))
  end subroutine from_nd_array_r32_4d

  function to_nd_array_r32_5d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)

    ndarray%rank = 5
    if (size(array) .eq. 0) then
      ndarray%strides(1:5) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1))
    end if
  end function to_nd_array_r32_5d

  subroutine from_nd_array_r32_5d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:5))
  end subroutine from_nd_array_r32_5d

  function to_nd_array_r32_6d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)

    ndarray%rank = 6
    if (size(array) .eq. 0) then
      ndarray%strides(1:6) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1))
    end if
  end function to_nd_array_r32_6d

  subroutine from_nd_array_r32_6d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:6))
  end subroutine from_nd_array_r32_6d

  function to_nd_array_r32_7d(array) result(ndarray)
    real(REAL32), target, intent(in) :: array(:,:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)
    ndarray%dims(7) = size(array, 7, c_size_t)

    ndarray%rank = 7
    if (size(array) .eq. 0) then
      ndarray%strides(1:7) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      if (size(array, 7) .ge. 2) then
        ndarray%strides(7) = &
          (transfer(c_loc(array(1,1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(7) = size(array, 6, c_size_t) * ndarray%strides(6)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1,1))
    end if
  end function to_nd_array_r32_7d

  subroutine from_nd_array_r32_7d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL32), pointer, intent(out) :: array(:,:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:7))
  end subroutine from_nd_array_r32_7d



  function to_nd_array_r64_1d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)

    ndarray%rank = 1
    if (size(array) .eq. 0) then
      ndarray%strides(1:1) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2)), 1_c_size_t) - &
          transfer(c_loc(array(1)), 1_c_size_t)) / c_sizeof(array(1))
      else
        ndarray%strides(1) = 1
      end if

      ndarray%data = c_loc(array(1))
    end if
  end function to_nd_array_r64_1d

  subroutine from_nd_array_r64_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_r64_1d

  function to_nd_array_r64_2d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)

    ndarray%rank = 2
    if (size(array) .eq. 0) then
      ndarray%strides(1:2) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1)), 1_c_size_t)) / c_sizeof(array(1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      ndarray%data = c_loc(array(1,1))
    end if
  end function to_nd_array_r64_2d

  subroutine from_nd_array_r64_2d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:2))
  end subroutine from_nd_array_r64_2d

  function to_nd_array_r64_3d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)

    ndarray%rank = 3
    if (size(array) .eq. 0) then
      ndarray%strides(1:3) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      ndarray%data = c_loc(array(1,1,1))
    end if
  end function to_nd_array_r64_3d

  subroutine from_nd_array_r64_3d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:3))
  end subroutine from_nd_array_r64_3d

  function to_nd_array_r64_4d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)

    ndarray%rank = 4
    if (size(array) .eq. 0) then
      ndarray%strides(1:4) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      ndarray%data = c_loc(array(1,1,1,1))
    end if
  end function to_nd_array_r64_4d

  subroutine from_nd_array_r64_4d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:4))
  end subroutine from_nd_array_r64_4d

  function to_nd_array_r64_5d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)

    ndarray%rank = 5
    if (size(array) .eq. 0) then
      ndarray%strides(1:5) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1))
    end if
  end function to_nd_array_r64_5d

  subroutine from_nd_array_r64_5d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:5))
  end subroutine from_nd_array_r64_5d

  function to_nd_array_r64_6d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)

    ndarray%rank = 6
    if (size(array) .eq. 0) then
      ndarray%strides(1:6) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1))
    end if
  end function to_nd_array_r64_6d

  subroutine from_nd_array_r64_6d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:6))
  end subroutine from_nd_array_r64_6d

  function to_nd_array_r64_7d(array) result(ndarray)
    real(REAL64), target, intent(in) :: array(:,:,:,:,:,:,:)

    type(nd_array_t) :: ndarray

    ndarray%dims(1) = size(array, 1, c_size_t)
    ndarray%dims(2) = size(array, 2, c_size_t)
    ndarray%dims(3) = size(array, 3, c_size_t)
    ndarray%dims(4) = size(array, 4, c_size_t)
    ndarray%dims(5) = size(array, 5, c_size_t)
    ndarray%dims(6) = size(array, 6, c_size_t)
    ndarray%dims(7) = size(array, 7, c_size_t)

    ndarray%rank = 7
    if (size(array) .eq. 0) then
      ndarray%strides(1:7) = 1
      ndarray%data = c_null_ptr
    else
      if (size(array, 1) .ge. 2) then
        ndarray%strides(1) = &
          (transfer(c_loc(array(2,1,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(1) = 1
      end if

      if (size(array, 2) .ge. 2) then
        ndarray%strides(2) = &
          (transfer(c_loc(array(1,2,1,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(2) = size(array, 1, c_size_t) * ndarray%strides(1)
      end if

      if (size(array, 3) .ge. 2) then
        ndarray%strides(3) = &
          (transfer(c_loc(array(1,1,2,1,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(3) = size(array, 2, c_size_t) * ndarray%strides(2)
      end if

      if (size(array, 4) .ge. 2) then
        ndarray%strides(4) = &
          (transfer(c_loc(array(1,1,1,2,1,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(4) = size(array, 3, c_size_t) * ndarray%strides(3)
      end if

      if (size(array, 5) .ge. 2) then
        ndarray%strides(5) = &
          (transfer(c_loc(array(1,1,1,1,2,1,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(5) = size(array, 4, c_size_t) * ndarray%strides(4)
      end if

      if (size(array, 6) .ge. 2) then
        ndarray%strides(6) = &
          (transfer(c_loc(array(1,1,1,1,1,2,1)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(6) = size(array, 5, c_size_t) * ndarray%strides(5)
      end if

      if (size(array, 7) .ge. 2) then
        ndarray%strides(7) = &
          (transfer(c_loc(array(1,1,1,1,1,1,2)), 1_c_size_t) - &
          transfer(c_loc(array(1,1,1,1,1,1,1)), 1_c_size_t)) / c_sizeof(array(1,1,1,1,1,1,1))
      else
        ndarray%strides(7) = size(array, 6, c_size_t) * ndarray%strides(6)
      end if

      ndarray%data = c_loc(array(1,1,1,1,1,1,1))
    end if
  end function to_nd_array_r64_7d

  subroutine from_nd_array_r64_7d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    real(REAL64), pointer, intent(out) :: array(:,:,:,:,:,:,:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:7))
  end subroutine from_nd_array_r64_7d


