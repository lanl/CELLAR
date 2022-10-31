file(GLOB SOURCES src/*.F90)

include_directories(${eap-comm-fortran_BINARY_DIR})

function(add_comm_fortran_test test_path)
    get_filename_component(test_name ${test_path} NAME_WE)

    add_executable(${test_name} src/${test_name}.F90)
    set_target_properties(${test_name} PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries(
        ${test_name}
        PRIVATE eap-comm-fortran eap-utility-fortran eap-totalview)

    eap_core_add_mpi_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:${test_name}>
    )
endfunction()

file(GLOB TESTS src/*.F90)

foreach(TEST ${TESTS})
    add_comm_fortran_test(${TEST})
endforeach(TEST)

# disable -Wcompare-reals warning for the real tests because we actually want
# to bit-wise compare reals since the comparison is between copied or
# communicated data and not different calculations
set(DISABLE_COMPARE_REALS
  get_put_double_v_inv
  get_put_double_v_strided
  get_put_double_v
  get_put_double_with_gaps
  get_put_double
  get_put_float
  token_clone
)

if (("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Clang"))
    foreach(TEST ${DISABLE_COMPARE_REALS})
        target_compile_options(${TEST} PRIVATE "-Wno-compare-reals")
    endforeach(TEST)
elseif ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel") # TODO XL
  # Intel doesn't seem to emit this warning, so do nothing
endif ()
