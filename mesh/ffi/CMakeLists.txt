file(GLOB SOURCES src/*.cpp)

option(
    EAP_MESH_FFI_BUILD_EXAMPLES
    "Build examples for eap-mesh-ffi"
    ${EAP_MESH_BUILD_EXAMPLES})

# eap-mesh-ffi exposes a C API on top of eap::mesh
add_library(eap-mesh-ffi ${SOURCES})
target_include_directories(eap-mesh-ffi PUBLIC include/library)
target_link_libraries(
    eap-mesh-ffi
    PUBLIC eap-mesh-ffi-base eap-comm-ffi-base
    PRIVATE
        eap-mesh-ffi-interop eap-abi-interop eap-comm-ffi-interop
        eap-utility-ffi-interop)
target_compile_features(eap-mesh-ffi PRIVATE cxx_std_14)

# eap-mesh-ffi-base exposes a set of data types that can be used for interop
# in projects that don't use eap-mesh-ffi
add_library(eap-mesh-ffi-base INTERFACE)
target_include_directories(eap-mesh-ffi-base INTERFACE include/base)
target_link_libraries(eap-mesh-ffi-base INTERFACE eap-abi-base)

# eap-mesh-ffi-interop exposes interfaces for converting between FFI types and
# eap::mesh types
add_library(eap-mesh-ffi-interop INTERFACE)
target_include_directories(eap-mesh-ffi-interop INTERFACE include/interop)
target_link_libraries(eap-mesh-ffi-interop INTERFACE eap-mesh eap-mesh-ffi-base)
target_compile_features(eap-mesh-ffi-interop INTERFACE cxx_std_14)

if (EAP_MESH_FFI_BUILD_EXAMPLES)
    enable_language(C)

    file(GLOB EXAMPLES examples/*.c)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        target_link_libraries(
            ${EXAMPLE_EXE}
            PRIVATE eap-mesh-ffi eap-totalview)

        eap_core_add_mpi_test(
            NAME ${EXAMPLE_EXE}
            COMMAND $<TARGET_FILE:${EXAMPLE_EXE}>)
    endforeach()
endif()
