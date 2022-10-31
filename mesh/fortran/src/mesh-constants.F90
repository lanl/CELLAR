!>
!! @file mesh-dzn.F90
!! 
!! @brief Fortran interface to DZN
!! @date 2019-08-08
!!
!! @copyright Copyright (C) 2019 Triad National Security, LLC

!> @brief
!!  Constants used with mesh
module mesh_constants
  implicit none
  private

  public X_DIR, Y_DIR, Z_DIR

  integer, parameter :: X_DIR = 1, Y_DIR = 2, Z_DIR = 3
end module mesh_constants