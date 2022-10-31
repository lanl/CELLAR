file(GLOB SOURCES src/*.cpp)

add_executable(eap-test_cxx_fortran-test ${SOURCES})
target_link_libraries(eap-test_cxx_fortran-test eap-test_cxx_fortran)
