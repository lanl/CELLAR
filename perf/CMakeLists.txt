option(
    EAP_PERF_ENABLE_TESTING
    "eap-perf tests should be built."
    ${EAP_CORE_ENABLE_TESTING})

option(
    EAP_PERF_BUILD_EXAMPLES
    "eap-perf examples should be built."
    ON)

file(GLOB SOURCES src/*.cpp)
add_library(eap-perf ${SOURCES})
target_include_directories(eap-perf PUBLIC include)
target_link_libraries(
    eap-perf
    PUBLIC
        nonstd::optional-lite nonstd::string_view-lite eap-utility)
target_compile_features(eap-perf PUBLIC cxx_std_14)

if (EAP_PERF_ENABLE_TESTING)
    add_subdirectory(test)
endif()

if (EAP_PERF_BUILD_EXAMPLES)
    file(GLOB EXAMPLES examples/*.cpp)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        target_link_libraries(${EXAMPLE_EXE} PUBLIC eap-perf)

        eap_core_add_serial_test(
            NAME ${EXAMPLE_EXE}
            COMMAND $<TARGET_FILE:${EXAMPLE_EXE}>)
    endforeach()
endif()

if (EAP_CORE_ENABLE_ABI)
    add_subdirectory(ffi)
endif()

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()
