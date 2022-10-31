!!!! THIS FILE IS GENERATED - DO NOT EDIT DIRECTLY !!!!

  function to_nd_array_lv_1d(array) result(ndarray)
    type(loc_val_t), target, intent(in) :: array(:)

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
  end function to_nd_array_lv_1d

  subroutine from_nd_array_lv_1d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    type(loc_val_t), pointer, intent(out) :: array(:)

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:1))
  end subroutine from_nd_array_lv_1d


