#include "comm-mpi.F90.h"

program mesh_levels_test
  use, intrinsic :: iso_fortran_env
  
  MPI_USE
  
  use mesh_cells
  use mesh_levels
  use mesh_core_types
  use token
  use utility_kokkos

  implicit none

  MPI_INCLUDE

  integer, parameter :: numcell = 1000, numdim = 3

  integer :: mype, numpe
  integer :: mpierror
  type(cpp_levels_t) :: levs
  type(cpp_levels_options_t) :: levs_opts
  type(cpp_cells_t) :: cells
  type(token_builder_t) :: builder

  integer, dimension(numcell) :: collected
  integer :: n

  call MPI_Init(mpierror)
  call kokkos_initialize

  call MPI_Comm_rank(MPI_COMM_WORLD, mype, mpierror)
  call MPI_Comm_size(MPI_COMM_WORLD, numpe, mpierror)

  builder = new_token_builder(MPI_COMM_WORLD)

  levs_opts%num_dim = 3
  levs_opts%cell_size = [1.0, 1.0, 1.0]
  levs_opts%kid_mom_use_s2s = .false.
  levs_opts%mom_kid_use_s2s = .false.

  cells = eap_mesh_new_cpp_cells_t(builder)
  call cells%resize_cell_arrays(numcell, numdim)
  call cells%set_num_local_cells(numcell)
  call cells%update_global_base()

  call new_cpp_levels(levs)
  call levs%initialize(levs_opts)
  ! over allocate levels
  call levs%resize_local(numcell, 1100)
  call levs%initialize_level1()

  call levs%build_pack(cells)
  call levs%check_lohipack_and_levels(cells)

  if (levs%num_at_level(1) .ne. 1000) then
    error stop "num_at_level did not return the correct number of cells"
  endif

  call levs%foreach_at_level(1, store)
  if (.not. all([(n, n=1,1000)] .eq. collected)) then
    error stop "foreach_at_level did not return correct array" 
  endif

  block
    type(loc_val_t) :: out(numcell)
    call levs%map_at_level(1, out, inc)

    if (.not. all([(n, n=1,1000)] .eq. out%loc)) then
      error stop "foreach_at_level did not return correct array" 
    endif

    if (.not. all([(n, n=2,1001)] .eq. out%val)) then
      error stop "foreach_at_level did not return correct array" 
    endif
  end block

  block
    integer, allocatable :: even_cells(:)

    ! address maybe-uninitialized
    allocate(even_cells(0))
  
    even_cells = levs%filter_at_level(cells, 1, is_even)

    if (.not. all([(n, n=2,1000,2)] .eq. even_cells)) then
      error stop "filter_at_level did not return correct array"
    endif
  end block

  block
    integer, allocatable :: level_1_cells(:)

    ! address maybe-uninitialized
    allocate(level_1_cells(0))

    level_1_cells = levs%collect_at_level(cells, 1)

    if (.not. all([(n, n=1,1000)] .eq. level_1_cells)) then
      error stop "collect_at_level did not return correct array"
    endif
  end block

  block
    call levs%set_flags(1, numcell, mod_four)

    if (any([(mod(n, 4), n=1,numcell)] .ne. levs%flag(1:numcell))) then
      write (*,*) levs%flag
      error stop "set_flags did not set the right flags"
    endif
  end block

  block
    call levs%set_flags_scalar(1, numcell, 7)

    if (any(7 .ne. levs%flag(1:numcell))) then
      write (*,*) levs%flag
      error stop "set_flags_scalar did not set the right flags"
    endif
  end block

  block
    call levs%set_flags_all(8)

    if (any(8 .ne. levs%flag)) then
      write (*,*) levs%flag
      error stop "set_flags_all did not set the right flags"
    endif
  end block

  block
    integer :: num_cleared

    num_cleared = levs%clear_at_level(1, [(mod(n, 2) == 0, n=1,1000)])

    if (num_cleared .ne. 500) then
      error stop &
        "clear_at_level did not clear the correct number of cell levels"
    endif

    if (.not. all(merge(1, 0, [(mod(n, 2) == 0, n=1,1000)]) .eq. levs%cell_level)) then
      error stop "clear_at_level did not clear the right cell levels"
    endif
  end block

  block
    ! Move the cell_levels back

    call levs%recon_pack(1, 500, 1, [(n, n=2,1000,2)], [(n, n=1,999,2)])

    if (any(levs%cell_level .ne. 1)) then
      error stop "recon_pack did not move cell levels"
    endif
  end block

  call levs%free()
  call cells%free()
  call builder%free()

  call kokkos_finalize
  call MPI_Finalize(mpierror)
contains
  subroutine store(l)
    integer, intent(in) :: l
    collected(l) = l
  end

  integer function inc(l)
    integer, intent(in) :: l
    inc = l + 1
  end

  logical function is_even(l)
    integer, intent(in) :: l

    is_even = mod(l, 2) .eq. 0
  end function is_even

  integer function mod_four(l)
    integer, intent(in) :: l

    mod_four = mod(l, 4)
  end function mod_four 
end program mesh_levels_test