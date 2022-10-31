function(generate_nd_array MAX_DIM VAR_TYPE VAR_SHORTHAND CONTENTS)
  set(${CONTENTS} "")

  foreach(DIM RANGE 1 ${MAX_DIM})
    set(FORTRAN_DIMS ":")
    if (DIM GREATER 1)
      foreach(X RANGE 2 ${DIM} 1)
        string(APPEND FORTRAN_DIMS ",:")
      endforeach()
    endif()

    string(APPEND ${CONTENTS}
"  function to_nd_array_${VAR_SHORTHAND}_${DIM}d(array) result(ndarray)
    ${VAR_TYPE}, target, intent(in) :: array(${FORTRAN_DIMS})

    type(nd_array_t) :: ndarray
")

    foreach(X RANGE 1 ${DIM})
      string(APPEND ${CONTENTS} "
    ndarray%dims(${X}) = size(array, ${X}, c_size_t)")
    endforeach()

    string(APPEND ${CONTENTS} "
")

    # Strides are irrelevant if the span of the array is 0
    string(APPEND ${CONTENTS} "
    ndarray%rank = ${DIM}
    if (size(array) .eq. 0) then
      ndarray%strides(1:${DIM}) = 1
      ndarray%data = c_null_ptr
    else")

    set(BASE_DIMS)
    foreach(X RANGE 1 ${DIM})
      list(APPEND BASE_DIMS 1)
    endforeach()
    list(JOIN BASE_DIMS "," BASE_DIMS_STR)

    foreach(X RANGE 1 ${DIM})
      set(STRIDE_DIMS)
      foreach(Y RANGE 1 ${DIM})
        if (${X} EQUAL ${Y})
          list(APPEND STRIDE_DIMS 2)
        else()
          list(APPEND STRIDE_DIMS 1)
        endif()
      endforeach()
      list(JOIN STRIDE_DIMS "," STRIDE_DIMS_STR)

      if (${X} EQUAL 1)
        set(UNIT_STRIDE "1")
      else()
        math(EXPR PREV_X "${X} - 1")
        set(UNIT_STRIDE "size(array, ${PREV_X}, c_size_t) * ndarray%strides(${PREV_X})")
      endif()

      string(APPEND ${CONTENTS} "
      if (size(array, ${X}) .ge. 2) then
        ndarray%strides(${X}) = &
          (transfer(c_loc(array(${STRIDE_DIMS_STR})), 1_c_size_t) - &
          transfer(c_loc(array(${BASE_DIMS_STR})), 1_c_size_t)) / c_sizeof(array(${BASE_DIMS_STR}))
      else
        ndarray%strides(${X}) = ${UNIT_STRIDE}
      end if
")
    endforeach()

    set(TESTS)
    foreach(X RANGE 1 ${DIM})
      list(APPEND TESTS "size(array, ${X}) .eq. 0")
    endforeach()
    list(JOIN TESTS " .or. " TESTS_STR)

    string(APPEND ${CONTENTS} "
      ndarray%data = c_loc(array(${BASE_DIMS_STR}))
    end if
  end function to_nd_array_${VAR_SHORTHAND}_${DIM}d

  subroutine from_nd_array_${VAR_SHORTHAND}_${DIM}d(ndarray, array)
    type(nd_array_t), intent(in) :: ndarray
    ${VAR_TYPE}, pointer, intent(out) :: array(${FORTRAN_DIMS})

    call check_nd_array_layout(ndarray)

    call c_f_pointer(ndarray%data, array, ndarray%dims(1:${DIM}))
  end subroutine from_nd_array_${VAR_SHORTHAND}_${DIM}d

"
    )
  endforeach()

  set(${CONTENTS} ${${CONTENTS}} PARENT_SCOPE)
endfunction()