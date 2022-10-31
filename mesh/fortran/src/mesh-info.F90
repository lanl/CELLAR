#include "abi-fortran_interop-f.h"

module mesh_info_types
  use, intrinsic :: iso_c_binding
  implicit none

  public :: cpp_sim_info_t
  public :: cpp_mesh_info_t

  !=================================================================================================
  !=================================================================================================
  !===
  !===
  !===  WHEN REMOVING LEGACY MESH CODE, CHANGE THE FOLLOWING HERE:
  !===
  !===  - remove cpp_ prefix from all the types defined here and propagate changes to macros
  !===  - rename all fields from cpp_* to just *
  !===  - rename all functions/occurrances of eap_mesh_cpp_* to eap_mesh_*
  !===
  !===
  !=================================================================================================
  !=================================================================================================

  ! We use a flat structure for eap::mesh::SimulationInfo and eap::mesh::MeshInfo in C++, but we
  ! need to replicate the hierarchical structure here that is used in xRage. All the member
  ! variables are documented in the C++ code. We use different names here to conform with xRage, the
  ! corresponding C++ name is given next to the Fortran declaration.

  ! All of these integer types are unsigned in C++, but Fortran doesn't have unsigned integers,
  ! so they're signed here.

  ! this corresponds to geometry_type in xRage, but is now directly contained in SimulationInfo
  type cpp_geometry_info
    integer(c_int32_t), pointer :: cpp_geom_flag => null()  ! geometry_dimension_flag_
    logical(c_bool),    pointer :: cpp_usegeo => null()     ! use_geometry_factor_
    logical(c_bool),    pointer :: cpp_cylin => null()      ! cylindrical_
    logical(c_bool),    pointer :: cpp_sphere => null()     ! spherical_
    integer(c_int32_t), pointer :: cpp_geop => null()       ! geometry_type_
    real(c_double),     pointer :: cpp_geofac => null()     ! geometry_factor_
  end type cpp_geometry_info

  type cpp_mesh_size
    integer(c_int64_t), pointer, dimension(:) :: cpp_grid_dimension => null() ! num_zones_

    ! convenience accessors to the components of grid_dimension
    integer(c_int64_t), pointer :: cpp_imxset => null() ! num_zones_[0]
    integer(c_int64_t), pointer :: cpp_jmxset => null() ! num_zones_[1]
    integer(c_int64_t), pointer :: cpp_kmxset => null() ! num_zones_[2]
  end type cpp_mesh_size

  type cpp_mesh_location
    real(c_double), pointer, dimension(:) :: cpp_grid_zero => null() ! lower_bounds_

    ! convenience accessors to the components of grid_zero
    real(c_double), pointer :: cpp_xzero => null() ! lower_bounds_[0]
    real(c_double), pointer :: cpp_yzero => null() ! lower_bounds_[1]
    real(c_double), pointer :: cpp_zzero => null() ! lower_bounds_[2]
  end type cpp_mesh_location

  type cpp_cell_size
    real(c_double),   pointer, dimension(:) :: cpp_grid_size => null()  ! cell_size_
    logical(c_bool),  pointer               :: cpp_not_square => null() ! non_square_cells_allowed

    ! convenience accessors to the components of grid_size
    real(c_double), pointer :: cpp_dxset => null() ! cell_size_[0]
    real(c_double), pointer :: cpp_dyset => null() ! cell_size_[1]
    real(c_double), pointer :: cpp_dzset => null() ! cell_size_[2]
  end type cpp_cell_size

  type cpp_boundary_conditions
    logical(c_bool), pointer, dimension(:)   :: cpp_grid_period => null() ! periodic_boundary_
    logical(c_bool), pointer, dimension(:,:) :: cpp_outflow => null()     ! outflow_boundary_

    ! convenience accessors to the components of grid_size
    logical(c_bool), pointer :: cpp_periodx => null() ! periodic_boundary_[0]
    logical(c_bool), pointer :: cpp_periody => null() ! periodic_boundary_[1]
    logical(c_bool), pointer :: cpp_periodz => null() ! periodic_boundary_[2]
  end type cpp_boundary_conditions

  type cpp_mesh_info
    type(cpp_mesh_size)     :: cpp_s
    type(cpp_mesh_location) :: cpp_l
  end type cpp_mesh_info

  type cpp_cell_info
    type(cpp_cell_size) :: cpp_s
  end type cpp_cell_info

  type cpp_sim_info_t
    integer(c_int32_t), pointer :: cpp_numdim => null() ! num_dim_
    integer(c_int32_t), pointer :: cpp_numvel => null() ! num_velocity_components
    integer(c_int64_t), pointer :: cpp_imxset => null() ! num_zones_[0]
    integer(c_int64_t), pointer :: cpp_jmxset => null() ! num_zones_[1]
    integer(c_int64_t), pointer :: cpp_kmxset => null() ! num_zones_[2]

    type(cpp_geometry_info) :: cpp_g

    ! for backward compatibility, add pointers to data stored in cpp_mesh_info_t
    integer(c_int32_t), pointer :: cpp_num_split => null()
    type(cpp_mesh_info) :: cpp_m
    type(cpp_cell_info) :: cpp_c
    type(cpp_boundary_conditions) :: cpp_bc

    ! Internal pointer that points to the C instance
    type(c_ptr), private :: ptr = c_null_ptr

    contains
      ! IMPORTANT: NO SPACE AFTER COMMA
      procedure :: free => FREE(mesh,cpp_sim_info)
      procedure :: is_associated => IS_ASSOCIATED(mesh,cpp_sim_info)
  end type cpp_sim_info_t

  type cpp_mesh_info_t
    integer(c_int32_t), pointer :: cpp_num_split => null() ! num_cells_per_block_
    type(cpp_mesh_info) :: cpp_m
    type(cpp_cell_info) :: cpp_c
    type(cpp_boundary_conditions) :: cpp_bc

    ! Internal pointer that points to the C instance
    type(c_ptr), private :: ptr = c_null_ptr

    contains
      ! IMPORTANT: NO SPACE AFTER COMMA
      procedure :: free => FREE(mesh,cpp_mesh_info)
      procedure :: is_associated => IS_ASSOCIATED(mesh,cpp_mesh_info)
  end type cpp_mesh_info_t

  interface
    ! bindings to C functions
    ! IMPORTANT: NO SPACE AFTER COMMA

    ! cpp_sim_info
    subroutine CREATE_C(mesh,cpp_sim_info)(num_zones, num_velocity_components, cylindrical, &
        spherical, use_geometry_factor, cpp_sim_info) CREATE_BIND(mesh,cpp_sim_info)
      use iso_c_binding
      implicit none
      integer(c_int64_t),        intent(in)    :: num_zones(3)
      integer(c_int32_t), value, intent(in)    :: num_velocity_components
      logical(c_bool),    value, intent(in)    :: cylindrical, spherical, use_geometry_factor
      type(c_ptr),               intent(inout) :: cpp_sim_info
    end subroutine

    FC_INTEROP_DESTRUCTOR(mesh,cpp_sim_info)

    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,num_dim)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,num_velocity_components)
    FC_INTEROP_WRAP_ARRAY(mesh,cpp_sim_info,num_zones)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,geometry_dimension_flag)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,use_geometry_factor)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,cylindrical)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,spherical)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,geometry_type)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_sim_info,geometry_factor)

    ! cpp_mesh_info
    subroutine CREATE_C(mesh,cpp_mesh_info)(sim_info, lower_bounds, non_square_cells_allowed, &
        cell_size, periodic_boundary, outflow_boundary, cpp_mesh_info) &
        CREATE_BIND(mesh,cpp_mesh_info)
      use iso_c_binding
      implicit none
      type(c_ptr),             intent(inout) :: sim_info
      real(c_double),          intent(in)    :: lower_bounds(3)
      logical(c_bool),  value, intent(in)    :: non_square_cells_allowed
      real(c_double),          intent(in)    :: cell_size(3)
      logical(c_bool),         intent(in)    :: periodic_boundary(3)
      logical(c_bool),         intent(in)    :: outflow_boundary(2,3)
      type(c_ptr),             intent(inout) :: cpp_mesh_info
    end subroutine

    FC_INTEROP_DESTRUCTOR(mesh,cpp_mesh_info)

    FC_INTEROP_WRAP_SCALAR(mesh,cpp_mesh_info,num_cells_per_block)
    FC_INTEROP_WRAP_ARRAY(mesh,cpp_mesh_info,lower_bounds)
    FC_INTEROP_WRAP_SCALAR(mesh,cpp_mesh_info,non_square_cells_allowed)
    FC_INTEROP_WRAP_ARRAY(mesh,cpp_mesh_info,cell_size)
    FC_INTEROP_WRAP_ARRAY(mesh,cpp_mesh_info,periodic_boundary)
    FC_INTEROP_WRAP_2D_VIEW(mesh,cpp_mesh_info,outflow_boundary)

  end interface

contains

  ! sim_info
  function eap_mesh_new_cpp_sim_info_t(num_zones, num_velocity_components, cylindrical, spherical, &
        use_geometry_factor) result(new)
    implicit none
    integer(c_int64_t), intent(in) :: num_zones(3)
    integer(c_int32_t), intent(in) :: num_velocity_components
    logical(c_bool), intent(in) :: cylindrical, spherical, use_geometry_factor
    type(cpp_sim_info_t) :: new

    ! create C instance
    call CREATE_C(mesh,cpp_sim_info)(num_zones, num_velocity_components, cylindrical, spherical, &
        use_geometry_factor, new%ptr)

    ! assign pointers
    new%cpp_numdim  => GET_FIELD(mesh,cpp_sim_info,num_dim)(new)
    new%cpp_numvel  => GET_FIELD(mesh,cpp_sim_info,num_velocity_components)(new)
    new%cpp_imxset  => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,1)
    new%cpp_jmxset  => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,2)
    new%cpp_kmxset  => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,3)

    new%cpp_m%cpp_s%cpp_grid_dimension => GET_FIELD(mesh,cpp_sim_info,num_zones)(new)
    new%cpp_m%cpp_s%cpp_imxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,1)
    new%cpp_m%cpp_s%cpp_jmxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,2)
    new%cpp_m%cpp_s%cpp_kmxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(new,3)

    new%cpp_g%cpp_geom_flag => GET_FIELD(mesh,cpp_sim_info,geometry_dimension_flag)(new)
    new%cpp_g%cpp_usegeo    => GET_FIELD(mesh,cpp_sim_info,use_geometry_factor)(new)
    new%cpp_g%cpp_cylin     => GET_FIELD(mesh,cpp_sim_info,cylindrical)(new)
    new%cpp_g%cpp_sphere    => GET_FIELD(mesh,cpp_sim_info,spherical)(new)
    new%cpp_g%cpp_geop      => GET_FIELD(mesh,cpp_sim_info,geometry_type)(new)
    new%cpp_g%cpp_geofac    => GET_FIELD(mesh,cpp_sim_info,geometry_factor)(new)
  end function

  ! IMPORTANT: NO SPACE AFTER COMMA
  FC_INTEROP_FREE_IMPL(mesh,cpp_sim_info)
  FC_INTEROP_IS_ASSOCIATED_IMPL(mesh,cpp_sim_info)

  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,num_dim,integer(c_int32_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,num_velocity_components,integer(c_int32_t))
  FC_INTEROP_WRAP_ARRAY_IMPL(mesh,cpp_sim_info,num_zones,integer(c_int64_t))
  FC_INTEROP_WRAP_ARRAY_COMPONENT_IMPL(mesh,cpp_sim_info,num_zones,integer(c_int64_t))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,geometry_dimension_flag,integer(c_int))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,use_geometry_factor,logical(c_bool))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,cylindrical,logical(c_bool))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,spherical,logical(c_bool))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,geometry_type,integer(c_int))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_sim_info,geometry_factor,real(c_double))

  ! mesh_info
  function eap_mesh_new_cpp_mesh_info_t(sim_info, lower_bounds, non_square_cells_allowed, &
      cell_size, periodic_boundary, outflow_boundary) result(new)
    implicit none
    type(cpp_sim_info_t), intent(inout) :: sim_info
    real(c_double),   intent(in)    :: lower_bounds(3)
    logical(c_bool),  intent(in)    :: non_square_cells_allowed
    real(c_double),   intent(in)    :: cell_size(3)
    logical(c_bool),  intent(in)    :: periodic_boundary(3)
    logical(c_bool),  intent(in)    :: outflow_boundary(2,3)
    type(cpp_mesh_info_t) :: new

    ! create C instance
    call CREATE_C(mesh,cpp_mesh_info)(sim_info%ptr, lower_bounds, non_square_cells_allowed, &
        cell_size, periodic_boundary, outflow_boundary, new%ptr)

    ! assign pointers
    new%cpp_num_split => GET_FIELD(mesh,cpp_mesh_info,num_cells_per_block)(new)

    new%cpp_m%cpp_l%cpp_grid_zero => GET_FIELD(mesh,cpp_mesh_info,lower_bounds)(new)
    new%cpp_m%cpp_l%cpp_xzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,1)
    new%cpp_m%cpp_l%cpp_yzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,2)
    new%cpp_m%cpp_l%cpp_zzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,3)

    new%cpp_c%cpp_s%cpp_grid_size => GET_FIELD(mesh,cpp_mesh_info,cell_size)(new)
    new%cpp_c%cpp_s%cpp_dxset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,1)
    new%cpp_c%cpp_s%cpp_dyset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,2)
    new%cpp_c%cpp_s%cpp_dzset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,3)
    new%cpp_c%cpp_s%cpp_not_square => GET_FIELD(mesh,cpp_mesh_info,non_square_cells_allowed)(new)

    new%cpp_bc%cpp_grid_period => GET_FIELD(mesh,cpp_mesh_info,periodic_boundary)(new)
    new%cpp_bc%cpp_periodx => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,1)
    new%cpp_bc%cpp_periody => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,2)
    new%cpp_bc%cpp_periodz => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,3)
    new%cpp_bc%cpp_outflow => GET_FIELD(mesh,cpp_mesh_info,outflow_boundary)(new)

    ! pointers to sim_info fields
    new%cpp_m%cpp_s%cpp_grid_dimension => GET_FIELD(mesh,cpp_sim_info,num_zones)(sim_info)
    new%cpp_m%cpp_s%cpp_imxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(sim_info,1)
    new%cpp_m%cpp_s%cpp_jmxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(sim_info,2)
    new%cpp_m%cpp_s%cpp_kmxset => GET_FIELD_COMP(mesh,cpp_sim_info,num_zones)(sim_info,3)

    ! set sim_info pointers
    ! we can't point to things in new, because it will be relocated
    sim_info%cpp_num_split => GET_FIELD(mesh,cpp_mesh_info,num_cells_per_block)(new)

    sim_info%cpp_m%cpp_l%cpp_grid_zero => GET_FIELD(mesh,cpp_mesh_info,lower_bounds)(new)
    sim_info%cpp_m%cpp_l%cpp_xzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,1)
    sim_info%cpp_m%cpp_l%cpp_yzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,2)
    sim_info%cpp_m%cpp_l%cpp_zzero => GET_FIELD_COMP(mesh,cpp_mesh_info,lower_bounds)(new,3)

    sim_info%cpp_c%cpp_s%cpp_grid_size => GET_FIELD(mesh,cpp_mesh_info,cell_size)(new)
    sim_info%cpp_c%cpp_s%cpp_dxset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,1)
    sim_info%cpp_c%cpp_s%cpp_dyset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,2)
    sim_info%cpp_c%cpp_s%cpp_dzset => GET_FIELD_COMP(mesh,cpp_mesh_info,cell_size)(new,3)
    sim_info%cpp_c%cpp_s%cpp_not_square => GET_FIELD(mesh,cpp_mesh_info,non_square_cells_allowed)(new)

    sim_info%cpp_bc%cpp_grid_period => GET_FIELD(mesh,cpp_mesh_info,periodic_boundary)(new)
    sim_info%cpp_bc%cpp_periodx => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,1)
    sim_info%cpp_bc%cpp_periody => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,2)
    sim_info%cpp_bc%cpp_periodz => GET_FIELD_COMP(mesh,cpp_mesh_info,periodic_boundary)(new,3)
    sim_info%cpp_bc%cpp_outflow => GET_FIELD(mesh,cpp_mesh_info,outflow_boundary)(new)

  end function

  FC_INTEROP_FREE_IMPL(mesh,cpp_mesh_info)
  FC_INTEROP_IS_ASSOCIATED_IMPL(mesh,cpp_mesh_info)

  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_mesh_info,num_cells_per_block,integer(c_int32_t))
  FC_INTEROP_WRAP_ARRAY_IMPL(mesh,cpp_mesh_info,lower_bounds,real(c_double))
  FC_INTEROP_WRAP_ARRAY_COMPONENT_IMPL(mesh,cpp_mesh_info,lower_bounds,real(c_double))
  FC_INTEROP_WRAP_SCALAR_IMPL(mesh,cpp_mesh_info,non_square_cells_allowed,logical(c_bool))
  FC_INTEROP_WRAP_ARRAY_IMPL(mesh,cpp_mesh_info,cell_size,real(c_double))
  FC_INTEROP_WRAP_ARRAY_COMPONENT_IMPL(mesh,cpp_mesh_info,cell_size,real(c_double))
  FC_INTEROP_WRAP_ARRAY_IMPL(mesh,cpp_mesh_info,periodic_boundary,logical(c_bool))
  FC_INTEROP_WRAP_ARRAY_COMPONENT_IMPL(mesh,cpp_mesh_info,periodic_boundary,logical(c_bool))
  FC_INTEROP_WRAP_2D_VIEW_IMPL(mesh,cpp_mesh_info,outflow_boundary,logical(c_bool))

end module mesh_info_types