option(
    EAP_PERF_FORTRAN_BUILD_EXAMPLES
    "Build examples for eap-perf-fortran"
    ${EAP_PERF_BUILD_EXAMPLES})

file(GLOB SOURCES src/*.F90)

add_library(eap-perf-fortran ${SOURCES})
target_set_mod_includes(eap-perf-fortran)
target_link_libraries(
    eap-perf-fortran
    PRIVATE eap-perf-ffi)

if (EAP_PERF_FORTRAN_BUILD_EXAMPLES)
    file(GLOB EXAMPLES examples/*.F90)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        set_target_properties(${EXAMPLE_EXE} PROPERTIES LINKER_LANGUAGE Fortran)
        target_link_libraries(
            ${EXAMPLE_EXE}
            PRIVATE eap-perf-fortran eap-totalview)

        eap_core_add_serial_test(
            NAME ${EXAMPLE_EXE}
            COMMAND $<TARGET_FILE:${EXAMPLE_EXE}>)
    endforeach()
endif()
