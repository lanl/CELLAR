file(GLOB SOURCES src/*.cpp)

add_executable(eap-utility-test ${SOURCES})
target_link_libraries(
    eap-utility-test
    PRIVATE eap-utility eap-test-serial-main)
target_compile_definitions(
    eap-utility-test
    PUBLIC span_CONFIG_CONTRACT_LEVEL_OFF)

eap_core_add_serial_test(NAME eap-utility-test COMMAND $<TARGET_FILE:eap-utility-test>)
