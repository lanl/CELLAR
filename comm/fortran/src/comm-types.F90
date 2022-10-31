module comm_types
  implicit none

  enum, bind(C)
    enumerator &
      ct_bool, &
      ct_int32, &
      ct_int64, &
      ct_float, &
      ct_double
  end enum
end module comm_types