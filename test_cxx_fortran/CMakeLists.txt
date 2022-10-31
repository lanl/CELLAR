file(GLOB SOURCES src/*.cpp)

add_library(eap-test_cxx_fortran ${SOURCES})
target_compile_features(eap-test_cxx_fortran PUBLIC cxx_std_14)
target_include_directories(eap-test_cxx_fortran PUBLIC include)
target_link_libraries(
    eap-test_cxx_fortran
    PUBLIC
        eap-utility eap-perf
        mpi-cpp eap-kokkos)

if (EAP_CORE_ENABLE_ABI)
    add_subdirectory(ffi)
endif()

if (EAP_CORE_ENABLE_FORTRAN)
    add_subdirectory(fortran)
endif()

add_subdirectory(test)
