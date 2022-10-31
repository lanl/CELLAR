file(GLOB SOURCES src/*.F90)

include_directories(${eap-abi-fortran_BINARY_DIR})

function(add_abi_fortran_test test_path)
    get_filename_component(test_name ${test_path} NAME_WE)

    add_executable(${test_name} src/${test_name}.F90)
    set_target_properties(${test_name} PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries(
        ${test_name}
        PRIVATE eap-abi-fortran eap-totalview)

    eap_core_add_mpi_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:${test_name}>
    )
endfunction(add_abi_fortran_test)

file(GLOB TESTS src/*.F90)

foreach(TEST ${TESTS})
    add_abi_fortran_test(${TEST})
endforeach(TEST)

