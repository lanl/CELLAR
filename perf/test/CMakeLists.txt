file(GLOB SOURCES src/*.cpp)
add_executable(eap-perf-test ${SOURCES})
target_link_libraries(
    eap-perf-test
    PRIVATE eap-test-serial-main eap-perf)

eap_core_add_serial_test(
    NAME eap-perf-test
    COMMAND $<TARGET_FILE:eap-perf-test>)