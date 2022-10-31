module comm_timer
  use perf_registry

  implicit none
  private

  public comm_timer_registry

  interface
    function eap_comm_timer_registry() bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr) :: eap_comm_timer_registry
    end function eap_comm_timer_registry
  end interface
contains
  function comm_timer_registry()
    type(timer_registry_t) :: comm_timer_registry

    comm_timer_registry = &
      timer_registry_from_raw(eap_comm_timer_registry())
  end function comm_timer_registry
end module comm_timer