subroutine dump_mesh_info(inf)
  use mesh_info_types
  implicit none
  type(cpp_mesh_info), intent(in)  :: inf

  write(*,*) 'grid_dimension = ', inf%cpp_s%cpp_grid_dimension
  write(*,*) 'grid_dimension = ', inf%cpp_s%cpp_imxset, inf%cpp_s%cpp_jmxset, inf%cpp_s%cpp_kmxset
  write(*,*) 'grid_zero = ', inf%cpp_l%cpp_grid_zero
  write(*,*) 'gird_zero = ', inf%cpp_l%cpp_xzero, inf%cpp_l%cpp_yzero, inf%cpp_l%cpp_zzero
end subroutine dump_mesh_info

subroutine dump_cell_info(inf)
  use mesh_info_types
  implicit none
  type(cpp_cell_info), intent(in)  :: inf

  write(*,*) 'grid_size = ', inf%cpp_s%cpp_grid_size
  write(*,*) 'grid_size = ', inf%cpp_s%cpp_dxset, inf%cpp_s%cpp_dyset, inf%cpp_s%cpp_dzset
  write(*,*) 'not_square = ', inf%cpp_s%cpp_not_square
end subroutine dump_cell_info

subroutine dump_boundary_info(inf)
  use mesh_info_types
  implicit none
  type(cpp_boundary_conditions), intent(in)  :: inf

  write(*,*) 'grid_periodic = ', inf%cpp_grid_period
  write(*,*) 'grid_periodic = ', inf%cpp_periodx, inf%cpp_periody, inf%cpp_periodz
  write(*,*) 'outflow(1,:) = ', inf%cpp_outflow(1,:)
  write(*,*) 'outflow(2,:) = ', inf%cpp_outflow(2,:)
end subroutine dump_boundary_info


program eap_fortran_mesh_info_test
  use mesh_info_types
  implicit none

  type(cpp_sim_info_t) :: sim_info
  type(cpp_mesh_info_t) :: mesh_info
  integer(c_int64_t) :: num_zones(3)
  integer(c_int32_t) :: num_vel
  logical(c_bool) :: cylindrical, spherical, use_geometry_factor, non_square, periodic(3), &
      outflow(2,3)
  real(c_double) :: lower_bounds(3), cell_size(3)

  real(c_double), parameter :: pi = 3.141592653589793_8;

  num_zones = (/ 10, 20, 1 /)
  num_vel = 3
  cylindrical = .true.
  spherical = .false.
  use_geometry_factor = .true.

  lower_bounds = (/ 1.5, 0.5, 0.0 /)
  non_square = .false.
  cell_size = (/ 3.0, 3.0, 1.0 /)
  periodic = (/ .false., .true., .false. /)
  outflow = reshape((/ .false., .true., .false., .false., .false., .true. /), (/ 2, 3 /))

  write(*,*) 'outflow(1,1) = ', outflow(1,1)
  write(*,*) 'outflow(1,2) = ', outflow(1,2)
  write(*,*) 'outflow(1,3) = ', outflow(1,3)
  write(*,*) 'outflow(2,1) = ', outflow(2,1)
  write(*,*) 'outflow(2,2) = ', outflow(2,2)
  write(*,*) 'outflow(2,3) = ', outflow(2,3)

  sim_info = eap_mesh_new_cpp_sim_info_t(num_zones, num_vel, cylindrical, spherical, &
      use_geometry_factor)

  mesh_info = eap_mesh_new_cpp_mesh_info_t(sim_info, lower_bounds, non_square, cell_size, periodic, &
      outflow)

  write(*,*) 'numdim = ', sim_info%cpp_numdim
  write(*,*) 'numvel = ', sim_info%cpp_numvel
  write(*,*) 'imxset = ', sim_info%cpp_imxset
  write(*,*) 'jmxset = ', sim_info%cpp_jmxset
  write(*,*) 'kmxset = ', sim_info%cpp_kmxset
  write(*,*) 'geom_flag = ', sim_info%cpp_g%cpp_geom_flag
  write(*,*) 'usegeo = ', sim_info%cpp_g%cpp_usegeo
  write(*,*) 'cylin = ', sim_info%cpp_g%cpp_cylin
  write(*,*) 'sphere = ', sim_info%cpp_g%cpp_sphere
  write(*,*) 'geop = ', sim_info%cpp_g%cpp_geop
  write(*,*) 'geofac = ', sim_info%cpp_g%cpp_geofac
  write(*,*) 'num_split = ', sim_info%cpp_num_split
  call dump_mesh_info(sim_info%cpp_m)
  call dump_cell_info(sim_info%cpp_c)
  call dump_boundary_info(sim_info%cpp_bc)

  if (sim_info%cpp_numdim .ne. 2) stop 1
  if (sim_info%cpp_numvel .ne. num_vel) stop 1
  if (sim_info%cpp_imxset .ne. num_zones(1)) stop 1
  if (sim_info%cpp_jmxset .ne. num_zones(2)) stop 1
  if (sim_info%cpp_kmxset .ne. num_zones(3)) stop 1
  if (sim_info%cpp_g%cpp_geom_flag .ne. 5) stop 1
  if (sim_info%cpp_g%cpp_usegeo .neqv. use_geometry_factor) stop 1
  if (sim_info%cpp_g%cpp_cylin .neqv. cylindrical) stop 1
  if (sim_info%cpp_g%cpp_sphere .neqv. spherical) stop 1
  if (sim_info%cpp_g%cpp_geop .ne. 2) stop 1
  if (abs(sim_info%cpp_g%cpp_geofac - pi) / pi .gt. 1.0E-12) stop 1

  if (sim_info%cpp_num_split .ne. 4) stop 1

  if (any(sim_info%cpp_m%cpp_s%cpp_grid_dimension .ne. num_zones)) stop 1
  if (sim_info%cpp_m%cpp_s%cpp_imxset .ne. num_zones(1)) stop 1
  if (sim_info%cpp_m%cpp_s%cpp_jmxset .ne. num_zones(2)) stop 1
  if (sim_info%cpp_m%cpp_s%cpp_kmxset .ne. num_zones(3)) stop 1

  if (any(sim_info%cpp_m%cpp_l%cpp_grid_zero .ne. lower_bounds)) stop 1
  if (sim_info%cpp_m%cpp_l%cpp_xzero .ne. lower_bounds(1)) stop 1
  if (sim_info%cpp_m%cpp_l%cpp_yzero .ne. lower_bounds(2)) stop 1
  if (sim_info%cpp_m%cpp_l%cpp_zzero .ne. lower_bounds(3)) stop 1

  if (any(sim_info%cpp_c%cpp_s%cpp_grid_size .ne. cell_size)) stop 1
  if (sim_info%cpp_c%cpp_s%cpp_dxset .ne. cell_size(1)) stop 1
  if (sim_info%cpp_c%cpp_s%cpp_dyset .ne. cell_size(2)) stop 1
  if (sim_info%cpp_c%cpp_s%cpp_dzset .ne. cell_size(3)) stop 1
  if (sim_info%cpp_c%cpp_s%cpp_not_square .neqv. non_square) stop 1

  if (any(sim_info%cpp_bc%cpp_grid_period .neqv. periodic)) stop 1
  if (sim_info%cpp_bc%cpp_periodx .neqv. periodic(1)) stop 1
  if (sim_info%cpp_bc%cpp_periody .neqv. periodic(2)) stop 1
  if (sim_info%cpp_bc%cpp_periodz .neqv. periodic(3)) stop 1

  if (any(sim_info%cpp_bc%cpp_outflow(1,:) .neqv. outflow(1,:))) stop 1
  if (any(sim_info%cpp_bc%cpp_outflow(2,:) .neqv. outflow(2,:))) stop 1

  write(*,*) ''

  write(*,*) 'num_split = ', mesh_info%cpp_num_split
  call dump_mesh_info(mesh_info%cpp_m)
  call dump_cell_info(mesh_info%cpp_c)
  call dump_boundary_info(mesh_info%cpp_bc)

  if (mesh_info%cpp_num_split .ne. 4) stop 1

  if (any(mesh_info%cpp_m%cpp_s%cpp_grid_dimension .ne. num_zones)) stop 1
  if (mesh_info%cpp_m%cpp_s%cpp_imxset .ne. num_zones(1)) stop 1
  if (mesh_info%cpp_m%cpp_s%cpp_jmxset .ne. num_zones(2)) stop 1
  if (mesh_info%cpp_m%cpp_s%cpp_kmxset .ne. num_zones(3)) stop 1

  if (any(mesh_info%cpp_m%cpp_l%cpp_grid_zero .ne. lower_bounds)) stop 1
  if (mesh_info%cpp_m%cpp_l%cpp_xzero .ne. lower_bounds(1)) stop 1
  if (mesh_info%cpp_m%cpp_l%cpp_yzero .ne. lower_bounds(2)) stop 1
  if (mesh_info%cpp_m%cpp_l%cpp_zzero .ne. lower_bounds(3)) stop 1

  if (any(mesh_info%cpp_c%cpp_s%cpp_grid_size .ne. cell_size)) stop 1
  if (mesh_info%cpp_c%cpp_s%cpp_dxset .ne. cell_size(1)) stop 1
  if (mesh_info%cpp_c%cpp_s%cpp_dyset .ne. cell_size(2)) stop 1
  if (mesh_info%cpp_c%cpp_s%cpp_dzset .ne. cell_size(3)) stop 1
  if (mesh_info%cpp_c%cpp_s%cpp_not_square .neqv. non_square) stop 1

  if (any(mesh_info%cpp_bc%cpp_grid_period .neqv. periodic)) stop 1
  if (mesh_info%cpp_bc%cpp_periodx .neqv. periodic(1)) stop 1
  if (mesh_info%cpp_bc%cpp_periody .neqv. periodic(2)) stop 1
  if (mesh_info%cpp_bc%cpp_periodz .neqv. periodic(3)) stop 1

  if (any(mesh_info%cpp_bc%cpp_outflow(1,:) .neqv. outflow(1,:))) stop 1
  if (any(mesh_info%cpp_bc%cpp_outflow(2,:) .neqv. outflow(2,:))) stop 1

  call mesh_info%free()
  call sim_info%free()
end program