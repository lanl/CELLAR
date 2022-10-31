
file(GLOB SOURCES src/*.F90)

add_library(eap-utility-fortran ${SOURCES})
target_set_mod_includes(eap-utility-fortran)
target_link_libraries(
    eap-utility-fortran
    PRIVATE eap-abi-fortran eap-utility-ffi)