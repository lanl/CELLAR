module perf_registry
  use, intrinsic :: iso_c_binding

  implicit none
  private
  public :: &
    new_timer_registry, &
    timer_registry_from_raw, &
    timer_registry_t, &
    timer_handle_t, &
    timer_statistics_t, &
    timer_iterator_t, &
    operator(.eq.), &
    operator(.ne.)

  type :: timer_registry_t
    type(c_ptr), private :: registry = c_null_ptr
  contains
    procedure :: free => timer_registry_t_free
    procedure :: insert_or_lookup_timer => &
      timer_registry_t_insert_or_lookup_timer

    generic :: push_timer => &
      timer_registry_t_push_timer_str, &
      timer_registry_t_push_timer_handle

    procedure, private :: &
      timer_registry_t_push_timer_str, &
      timer_registry_t_push_timer_handle

    procedure :: pop_timer => timer_registry_t_pop_timer

    procedure :: timers => timer_registry_t_timers
    procedure :: num_timers => timer_registry_t_num_timers

    procedure :: timer_name => timer_registry_t_timer_name
    procedure :: timer_stats => timer_registry_t_timer_stats
  end type timer_registry_t

  type :: timer_handle_t
    integer(c_size_t), private :: timer_handle = -1
  contains
    procedure :: is_associated => timer_handle_t_is_associated
  end type timer_handle_t

  type :: timer_iterator_t
    type(c_ptr), private :: registry = c_null_ptr
    integer(c_size_t), private :: timer_handle
  contains
    procedure :: timer => timer_iterator_t_timer
    procedure :: next => timer_iterator_t_next
  end type timer_iterator_t

  interface operator(.eq.)
    module procedure timer_handle_t_eq
  end interface

  interface operator(.ne.)
    module procedure timer_handle_t_ne
  end interface

  type, bind(C) :: timer_statistics_t
    integer(c_size_t) :: timer_count
    integer(c_int64_t) :: sum_time
    integer(c_int64_t) :: avg_time
    integer(c_int64_t) :: max_time
    integer(c_int64_t) :: min_time
  end type timer_statistics_t

  enum, bind(C)
    enumerator &
      tre_success, &
      tre_buffer_too_small
  end enum

  interface
    subroutine eap_perf_timer_registry_create(registry) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), intent(out) :: registry
    end subroutine eap_perf_timer_registry_create

    subroutine eap_perf_timer_registry_free(registry) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
    end subroutine eap_perf_timer_registry_free

    subroutine eap_perf_timer_registry_insert_or_lookup_timer_f(&
      registry, timer_name, timer_name_length, timer_handle) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      character(kind=c_char), intent(in) :: timer_name(*)
      integer(c_size_t), value, intent(in) :: timer_name_length
      integer(c_size_t), intent(out) :: timer_handle
    end subroutine eap_perf_timer_registry_insert_or_lookup_timer_f

    subroutine eap_perf_timer_registry_push_timer(registry, timer_handle) &
      bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), value, intent(in) :: timer_handle
    end subroutine eap_perf_timer_registry_push_timer

    subroutine eap_perf_timer_registry_pop_timer(registry, timer_handle) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), intent(out) :: timer_handle
    end subroutine eap_perf_timer_registry_pop_timer

    subroutine eap_perf_timer_registry_get_timer_name_length(&
      registry, timer_handle, timer_name_length) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), value, intent(in) :: timer_handle
      integer(c_size_t), intent(out) :: timer_name_length
    end subroutine eap_perf_timer_registry_get_timer_name_length

    function eap_perf_timer_registry_get_timer_name(&
      registry, timer_handle, timer_name, timer_name_length) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), value, intent(in) :: timer_handle
      character(kind=c_char), intent(in) :: timer_name(*)
      integer(c_size_t), value, intent(in) :: timer_name_length
      integer(c_int) :: eap_perf_timer_registry_get_timer_name
    end function eap_perf_timer_registry_get_timer_name

    subroutine eap_perf_timer_registry_get_statistics(&
      registry, timer_handle, timer_stats) bind(C)
      use, intrinsic :: iso_c_binding
      import timer_statistics_t

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), value, intent(in) :: timer_handle
      type(timer_statistics_t), intent(out) :: timer_stats
    end subroutine eap_perf_timer_registry_get_statistics

    subroutine eap_perf_timer_registry_get_num_timers(registry, num_timers) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), intent(out) :: num_timers
    end subroutine eap_perf_timer_registry_get_num_timers

    subroutine eap_perf_timer_registry_create_timer_iterator(registry, start) &
      bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), intent(inout) :: start
    end subroutine eap_perf_timer_registry_create_timer_iterator

    function eap_perf_timer_registry_next_timer(registry, timer_handle)&
      result(more) bind(C)
      use, intrinsic :: iso_c_binding

      type(c_ptr), value, intent(in) :: registry
      integer(c_size_t), intent(inout) :: timer_handle
      logical(c_bool) :: more
    end function eap_perf_timer_registry_next_timer
  end interface
contains
  ! timer_registry_t
  function new_timer_registry()
    type(timer_registry_t) :: new_timer_registry

    call eap_perf_timer_registry_create(new_timer_registry%registry)
  end function new_timer_registry

  function timer_registry_from_raw(registry)
    type(c_ptr), intent(in) :: registry
    type(timer_registry_t) :: timer_registry_from_raw

    timer_registry_from_raw%registry = registry
  end function timer_registry_from_raw

  subroutine timer_registry_t_free(self)
    class(timer_registry_t) :: self

    call eap_perf_timer_registry_free(self%registry)
  end subroutine timer_registry_t_free

  function timer_registry_t_insert_or_lookup_timer(self, timer_name) &
    result(timer)

    class(timer_registry_t), intent(in) :: self
    character(len=*, kind=c_char), target :: timer_name
    type(timer_handle_t) :: timer

    call eap_perf_timer_registry_insert_or_lookup_timer_f(&
      self%registry, timer_name, len(timer_name, c_size_t), &
      timer%timer_handle)
  end function timer_registry_t_insert_or_lookup_timer

  subroutine timer_registry_t_push_timer_str(self, timer_name)
    class(timer_registry_t), intent(in) :: self
    character(len=*, kind=c_char) :: timer_name
    call self%push_timer(self%insert_or_lookup_timer(timer_name))
  end subroutine timer_registry_t_push_timer_str

  subroutine timer_registry_t_push_timer_handle(self, timer)
    class(timer_registry_t), intent(in) :: self
    type(timer_handle_t) :: timer
    call eap_perf_timer_registry_push_timer(self%registry, timer%timer_handle)
  end subroutine timer_registry_t_push_timer_handle

  subroutine timer_registry_t_pop_timer(self, timer)
    class(timer_registry_t), intent(in) :: self
    type(timer_handle_t), intent(out), optional :: timer

    integer(c_size_t) :: timer_handle = -1

    call eap_perf_timer_registry_pop_timer(self%registry, timer_handle)
    if (present(timer)) then
      timer%timer_handle = timer_handle
    end if
  end subroutine timer_registry_t_pop_timer

  function timer_registry_t_timers(self) result(iterator)
    class(timer_registry_t), intent(in) :: self
    type(timer_iterator_t) :: iterator

    iterator%registry = self%registry
    call eap_perf_timer_registry_create_timer_iterator(&
      self%registry, iterator%timer_handle)
  end function timer_registry_t_timers

  function timer_registry_t_num_timers(self) result(count)
    class(timer_registry_t), intent(in) :: self
    integer(c_size_t) :: count

    call eap_perf_timer_registry_get_num_timers(self%registry, count)
  end function timer_registry_t_num_timers

  function timer_registry_t_timer_name(self, timer) result(name)
    class(timer_registry_t), intent(in) :: self
    type(timer_handle_t), intent(in) :: timer

    integer(c_size_t) :: timer_name_length
    character(len=:), allocatable, target :: name, name0
    integer :: ierr

    call eap_perf_timer_registry_get_timer_name_length(&
      self%registry, timer%timer_handle, timer_name_length)

    allocate(character(timer_name_length + 1) :: name0)

    ierr = eap_perf_timer_registry_get_timer_name(&
      self%registry, timer%timer_handle, name0, timer_name_length + 1)
    if (ierr .ne. 0) then
      stop 1
    end if

    allocate(character(timer_name_length) :: name)
    name = name0(1:timer_name_length)
  end function timer_registry_t_timer_name

  function timer_registry_t_timer_stats(self, timer) result(stats)
    class(timer_registry_t), intent(in) :: self
    type(timer_handle_t), intent(in) :: timer

    type(timer_statistics_t) :: stats

    call eap_perf_timer_registry_get_statistics(&
      self%registry, timer%timer_handle, stats)
  end function timer_registry_t_timer_stats

  ! timer_handle_t routines
  function timer_handle_t_is_associated(self) result(is_associated)
    class(timer_handle_t), intent(in) :: self
    logical :: is_associated
    is_associated = self%timer_handle .ne. -1
  end function timer_handle_t_is_associated

  function timer_handle_t_eq(a,b) result(test)
    type(timer_handle_t), intent(in) :: a,b
    logical :: test

    test = a%timer_handle .eq. b%timer_handle
  end function timer_handle_t_eq

  function timer_handle_t_ne(a,b) result(test)
    type(timer_handle_t), intent(in) :: a,b
    logical :: test

    test = a%timer_handle .ne. b%timer_handle
  end function timer_handle_t_ne

  ! timer_iterator_t routines
  function timer_iterator_t_timer(self) result(timer)
    class(timer_iterator_t), intent(inout) :: self
    type(timer_handle_t) :: timer

    timer%timer_handle = self%timer_handle
  end function timer_iterator_t_timer

  function timer_iterator_t_next(self) result(more)
    class(timer_iterator_t), intent(inout) :: self
    logical :: more

    more = eap_perf_timer_registry_next_timer(self%registry, self%timer_handle)
  end function timer_iterator_t_next
end module perf_registry