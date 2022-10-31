file(GLOB SOURCES src/*.cpp)

add_library(eap-utility ${SOURCES})
target_compile_features(eap-utility PUBLIC cxx_std_14)
target_include_directories(eap-utility PUBLIC include)
target_link_libraries(
    eap-utility
    PUBLIC
        eap-abi-base eap-error
        eap-kokkos nonstd::span-lite nonstd::optional-lite)

if (EAP_CORE_ENABLE_TESTING)
    add_subdirectory(test)
endif()

if (EAP_CORE_ENABLE_ABI)
    add_subdirectory(ffi)
endif()

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()

if (EAP_CORE_ENABLE_BENCHMARK)
    add_subdirectory(bench)
endif()