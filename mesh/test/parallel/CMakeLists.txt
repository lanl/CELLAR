file(GLOB SOURCES src/*.cpp)

add_executable(eap-mesh-parallel-test ${SOURCES})
target_link_libraries(
    eap-mesh-parallel-test
    PRIVATE mpi-cpp eap-mesh eap-test-parallel-main)

eap_core_add_mpi_test(
    NAME eap-mesh-parallel-test
    COMMAND
        $<TARGET_FILE:eap-mesh-parallel-test>
            --kokkos-threads=${EAP_NUM_CORES_MPI4}
    )
