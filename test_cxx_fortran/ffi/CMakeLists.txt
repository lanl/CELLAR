file(GLOB SOURCES src/*.cpp)

add_library(eap-test_cxx_fortran-ffi ${SOURCES})
target_link_libraries(
    eap-test_cxx_fortran-ffi
    PUBLIC eap-abi-base
    PRIVATE eap-test_cxx_fortran eap-abi-interop)
