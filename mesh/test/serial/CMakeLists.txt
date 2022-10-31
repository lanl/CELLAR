file(GLOB SOURCES src/*.cpp)

add_executable(eap-mesh-serial-test ${SOURCES})
target_link_libraries(
    eap-mesh-serial-test
    PRIVATE eap-mesh eap-test-serial-main)

eap_core_add_serial_test(
    NAME eap-mesh-serial-test
    COMMAND $<TARGET_FILE:eap-mesh-serial-test>)
