# eap-abi-base exposes a set of data types that can be used for interop
add_library(eap-abi-base INTERFACE)
target_include_directories(eap-abi-base INTERFACE include/base)

# eap-abi-interop exposes interfaces for implementing FFI, but are not
# themselves portable C
add_library(eap-abi-interop INTERFACE)
target_include_directories(eap-abi-interop INTERFACE include/interop)
target_link_libraries(eap-abi-interop INTERFACE eap-abi-base eap-error)
target_compile_features(eap-abi-interop INTERFACE cxx_std_14)

option(
    EAP_ABI_ENABLE_TESTING
    "Tests are enabled by default. Dependents should disable this option."
    ${EAP_CORE_ENABLE_TESTING})

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()