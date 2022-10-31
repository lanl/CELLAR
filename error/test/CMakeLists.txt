file(GLOB SOURCES src/*.cpp)

add_executable(eap-error-test ${SOURCES})
target_link_libraries(
    eap-error-test
    PRIVATE eap-test-serial-main eap-error)

eap_core_add_serial_test(
    NAME eap-error-test
    COMMAND
        $<TARGET_FILE:eap-error-test> --kokkos-threads=1)
set_tests_properties(
    eap-error-test
    PROPERTIES ENVIRONMENT OMP_NUM_THREADS=1
)