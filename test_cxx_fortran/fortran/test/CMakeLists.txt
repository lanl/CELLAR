file(GLOB SOURCES src/*.F90)

include_directories(${eap-test_cxx_fortran-fortran_BINARY_DIR})

function(add_test_cxx_fortran_fortran_test test_path)
    get_filename_component(test_name ${test_path} NAME_WE)

    add_executable(${test_name} src/${test_name}.F90)
    set_target_properties(${test_name} PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries(
        ${test_name}
        PRIVATE eap-test_cxx_fortran-fortran eap-totalview)

    eap_core_add_serial_test(NAME ${test_name} COMMAND ${test_name})
endfunction(add_test_cxx_fortran_fortran_test)

file(GLOB TESTS src/*.F90)

foreach(TEST ${TESTS})
    add_test_cxx_fortran_fortran_test(${TEST})
endforeach(TEST)
