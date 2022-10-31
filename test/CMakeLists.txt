add_library(
    eap-test-serial-main src/test-serial.cpp)
target_link_libraries(
    eap-test-serial-main
    PUBLIC GTest::GTest eap-kokkos eap-totalview
)

add_library(
    eap-test-parallel-main src/test-parallel.cpp)
target_link_libraries(
    eap-test-parallel-main
    PUBLIC gtest-mpi-lib eap-kokkos eap-totalview
)