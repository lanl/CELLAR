program get_block_addresses
  use, intrinsic :: iso_fortran_env
  use :: mesh_blocks

  type(block_layout_t) :: blocks
  integer(INT64) :: n

  blocks = new_block_layout(blm_column_major_reverse, 4_INT64, 4_INT64, 2_INT64)

  do n = 1,32
    block
      integer(INT64) :: c0, c1, c2, n2

      call blocks%get_coordinates(n, c0, c1, c2)
      call blocks%get_address(c0, c1, c2, n2)

      write(*,*) n, n2, c0, c1, c2
    end block
  end do

  call blocks%free()
end program get_block_addresses