file(GLOB SOURCES src/*.F90)

add_library(eap-test_cxx_fortran-fortran ${SOURCES})
target_link_libraries(
    eap-test_cxx_fortran-fortran
    PRIVATE eap-test_cxx_fortran-ffi eap-abi-fortran)
target_set_mod_includes(eap-test_cxx_fortran-fortran)

add_subdirectory(test)
