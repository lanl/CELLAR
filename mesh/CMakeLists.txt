file(GLOB SOURCES src/*.cpp)

add_library(eap-mesh ${SOURCES})
target_link_libraries(
    eap-mesh
    PUBLIC
        mpi-cpp eap-kokkos
        eap-comm eap-totalview)
target_include_directories(eap-mesh PUBLIC include PRIVATE internal)
target_compile_features(eap-mesh PUBLIC cxx_std_14)

if (EAP_CORE_ENABLE_ABI)
    add_subdirectory(ffi)
endif()

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()

if(EAP_CORE_ENABLE_TESTING)
    add_subdirectory(test)
endif()

if (EAP_CORE_ENABLE_BENCHMARK)
    add_subdirectory(bench)
endif()
