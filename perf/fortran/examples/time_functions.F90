program time_functions
  use perf_registry

  implicit none

  ! Most applications will declare a global variable for their timer registry,
  ! and may declare multiple timer registries for different domains.
  !
  ! A "timer_registry_t" can be declared in smaller scopes if desired.
  type(timer_registry_t) :: registry
  type(timer_iterator_t) :: timers
  integer :: n

  registry = new_timer_registry()

  ! Running foo many times
  do n = 1,1000
    call foo
  end do

  ! Running bar many times
  do n = 1,1000
    call bar
  end do

  write(*,*) "Num timers: ", registry%num_timers()

  ! Dump timer information
  timers = registry%timers()
  do while (timers%next())
    block
      character(len=:), allocatable :: name
      type(timer_statistics_t) :: stats

      name = registry%timer_name(timers%timer())
      stats = registry%timer_stats(timers%timer())

      write(*,*) "Timer: ", name, ", count = ", stats%timer_count, &
        ", sum_time = ", stats%sum_time, "ns, avg_time = ", stats%avg_time, "ns"
    end block
  end do
contains
  subroutine foo
    ! "foo_timer" starts when "push_timer" is called and stops when "pop_timer"
    ! is called.
    !
    ! Since "foo_timer" is the outermost timer, its time will be recorded,
    ! whereas the inner timers will not be tracked.
    call registry%push_timer("foo timer")
    call bar
    call registry%pop_timer()
  end subroutine foo

  subroutine bar
    ! You can also register timers manually using "insert_or_lookup_timer". This
    ! will return a timer_handle_t object that can be then used with
    ! "push_timer"
    type(timer_handle_t) :: bar_timer, popped
    bar_timer = registry%insert_or_lookup_timer("bar timer")

    ! "push_timer" is overloaded for both strings and "timer_handle_t"s. It's
    ! a little more efficient to store the "timer_handle_t" and use that.
    call registry%push_timer(bar_timer)
    call zap
    call registry%pop_timer(popped)
    if (bar_timer .ne. popped) then
      write(*,*) "Did not get the expected timer on pop!"
      stop 1
    end if
  end subroutine bar

  subroutine zap
    ! "timer_handle_t"s are valid for the lifetime of the associated
    ! "timer_registry_t", and so they can be stored across timer runs.
    type(timer_handle_t), save :: zap_timer

    if (.not. zap_timer%is_associated()) then
      zap_timer = registry%insert_or_lookup_timer("zap timer")
    end if

    call registry%push_timer(zap_timer)
    call registry%pop_timer
  end subroutine zap
end program time_functions