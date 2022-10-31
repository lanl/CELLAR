file(GLOB SOURCES src/*.cpp)

option(
    EAP_PERF_FFI_BUILD_EXAMPLES
    "Build examples for eap-perf-ffi"
    ${EAP_PERF_BUILD_EXAMPLES})

# eap-perf-ffi exposes a C API on top of eap::perf
add_library(eap-perf-ffi ${SOURCES})
target_include_directories(eap-perf-ffi PUBLIC include/library)
target_link_libraries(
    eap-perf-ffi
    PUBLIC eap-perf-ffi-base
    PRIVATE eap-perf-ffi-interop)
target_compile_features(eap-perf-ffi PRIVATE cxx_std_14)

# eap-perf-ffi-base exposes a set of data types that can be used for interop
# in projects that don't use eap-perf-ffi
add_library(eap-perf-ffi-base INTERFACE)
target_include_directories(eap-perf-ffi-base INTERFACE include/base)
target_link_libraries(eap-perf-ffi-base INTERFACE eap-abi-base)

# eap-perf-ffi-interop exposes interfaces for converting between FFI types and
# eap::perf types
add_library(eap-perf-ffi-interop INTERFACE)
target_include_directories(eap-perf-ffi-interop INTERFACE include/interop)
target_link_libraries(eap-perf-ffi-interop INTERFACE eap-perf eap-perf-ffi-base)
target_compile_features(eap-perf-ffi-interop INTERFACE cxx_std_14)

if (EAP_PERF_FFI_BUILD_EXAMPLES)
    enable_language(C)

    file(GLOB EXAMPLES examples/*.c)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        target_link_libraries(
            ${EXAMPLE_EXE}
            PRIVATE eap-perf-ffi eap-totalview)

        eap_core_add_serial_test(
            NAME ${EXAMPLE_EXE}
            COMMAND $<TARGET_FILE:${EXAMPLE_EXE}>)
    endforeach()
endif()
