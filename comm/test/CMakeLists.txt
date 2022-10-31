file(GLOB SOURCES src/*.cpp)

add_executable(eap-comm-test ${SOURCES})
target_link_libraries(
    eap-comm-test
    PRIVATE eap-test-parallel-main eap-comm)
target_compile_definitions(eap-comm-test PRIVATE span_CONFIG_CONTRACT_LEVEL_OFF)

eap_core_add_mpi_test(NAME eap-comm-test COMMAND $<TARGET_FILE:eap-comm-test>)
