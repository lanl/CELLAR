option(
    EAP_COMM_ENABLE_TESTING
    "Tests are enabled by default. Dependents should disable this option."
    ${EAP_CORE_ENABLE_TESTING})

option(
    EAP_COMM_ABI_OPTIMIZE_LAYOUT_LEFT
    "Requests that the Token ABI optimize for cases where the token is in \
    LayoutLeft. Default: Matches EAP_COMM_ENABLE_FORTRAN"
    ${EAP_COMM_ENABLE_FORTRAN})

option(
    EAP_COMM_ABI_OPTIMIZE_LAYOUT_RIGHT
    "Requests that the Token ABI optimize for cases where the token is in \
    LayoutRight. Default: ON"
    ON)


file(GLOB SOURCES src/*.cpp)

add_library(eap-comm ${SOURCES})
target_compile_features(eap-comm PUBLIC cxx_std_14)
target_include_directories(eap-comm PUBLIC include)
target_link_libraries(
    eap-comm
    PUBLIC
        eap-utility eap-perf eap-error
        mpi-cpp eap-kokkos)

if (EAP_CORE_ENABLE_ABI)
    add_subdirectory(ffi)
endif()

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()

if (EAP_COMM_ENABLE_TESTING)
    add_subdirectory(test)
endif()
