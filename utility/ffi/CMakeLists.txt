file(GLOB SOURCES src/*.cpp)

# C library
add_library(eap-utility-ffi ${SOURCES})
target_include_directories(
    eap-utility-ffi
    PUBLIC include/library)
target_link_libraries(
    eap-utility-ffi
    PUBLIC eap-utility-ffi-base
    PRIVATE eap-kokkos eap-utility eap-error)
target_compile_features(
    eap-utility-ffi
    PRIVATE cxx_std_14)

# Set of types to use in interop and C interface
add_library(eap-utility-ffi-base INTERFACE)
target_include_directories(
    eap-utility-ffi-base
    INTERFACE include/base)
target_link_libraries(
    eap-utility-ffi-base
    INTERFACE eap-abi-base)

# Exposes interfaces for interop between FFI and C++
add_library(eap-utility-ffi-interop INTERFACE)
target_include_directories(
    eap-utility-ffi-interop
    INTERFACE include/interop)
target_link_libraries(
    eap-utility-ffi-interop
    INTERFACE eap-utility-ffi-base)
target_compile_features(
    eap-utility-ffi-interop
    INTERFACE cxx_std_14)
