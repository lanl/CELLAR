file(GLOB SOURCES src/*.cpp)

# eap-comm-ffi exposes a C API on top of eap::comm
add_library(eap-comm-ffi ${SOURCES})
target_include_directories(
    eap-comm-ffi
    PUBLIC include/library)
target_link_libraries(
    eap-comm-ffi
    PUBLIC eap-comm-ffi-base eap-utility-ffi
    PRIVATE eap-comm-ffi-interop
            eap-perf-ffi-interop
            eap-utility-ffi-interop)
target_compile_features(eap-comm-ffi PRIVATE cxx_std_14)

# eap-comm-ffi-base exposes a set of data types that can be used for interop
# in projects that don't use eap-comm-ffi
add_library(eap-comm-ffi-base INTERFACE)
target_include_directories(eap-comm-ffi-base INTERFACE include/base)
target_link_libraries(eap-comm-ffi-base INTERFACE eap-abi-base)

# eap-comm-ffi-interop exposes interfaces for converting between FFI types and
# eap::comm types
add_library(eap-comm-ffi-interop INTERFACE)
target_include_directories(eap-comm-ffi-interop INTERFACE include/interop)
target_link_libraries(eap-comm-ffi-interop INTERFACE eap-comm eap-comm-ffi-base)
target_compile_features(eap-comm-ffi-interop INTERFACE cxx_std_14)

if (EAP_COMM_ABI_OPTIMIZE_LAYOUT_LEFT)
    target_compile_definitions(
        eap-comm-ffi PRIVATE TOKEN_OPTIMIZE_FOR_LAYOUT_LEFT)
endif()

if (EAP_COMM_ABI_OPTIMIZE_LAYOUT_RIGHT)
    target_compile_definitions(
        eap-comm-ffi PRIVATE TOKEN_OPTIMIZE_FOR_LAYOUT_RIGHT)
endif()