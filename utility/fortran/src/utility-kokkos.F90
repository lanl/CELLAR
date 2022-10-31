module utility_kokkos
  implicit none
  private

  public :: &
    kokkos_initialize, &
    kokkos_finalize

  interface
    subroutine eap_utility_kokkos_initialize() &
      bind(C, name="eap_utility_kokkos_initialize")
    end subroutine eap_utility_kokkos_initialize

    subroutine eap_utility_kokkos_finalize() &
      bind(C, name="eap_utility_kokkos_finalize")
    end subroutine eap_utility_kokkos_finalize
  end interface
contains
  subroutine kokkos_initialize
    call eap_utility_kokkos_initialize
  end subroutine kokkos_initialize

  subroutine kokkos_finalize
    call eap_utility_kokkos_finalize
  end subroutine kokkos_finalize
end module utility_kokkos