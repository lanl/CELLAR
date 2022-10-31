add_executable(
    eap-totalview-test
    src/visualization-testing.cpp)
target_link_libraries(
    eap-totalview-test
    PRIVATE eap-totalview eap-kokkos)

# This is just for debugging inspection