program cxxclass_fortran_test

  use test_cxx_fortran

  implicit none

  type(cxxclass_t) :: c
  integer :: i, j

  c = eap_test_cxx_fortran_new_cxxclass_t()

  call c%print()

  c%some_int = 42

  call c%resize(3, 5)

  do i = 1, 3
    do j = 1, 5
      c%matrix(i, j) = (i-1) * 100.0 + (j-1)
    end do
  end do

  call c%print()

  c%matrix(2, 3) = -42.42
  call c%print()

  call c%free()

end program
