option(
    EAP_ERROR_ENABLE_TESTING
    "eap-error tests should be built."
    ${EAP_CORE_ENABLE_TESTING})

option(
    EAP_ERROR_BUILD_EXAMPLES
    "eap-error examples should be built."
    OFF)

file(GLOB SOURCES src/*.cpp)
add_library(eap-error ${SOURCES})
target_include_directories(eap-error PUBLIC include)
target_link_libraries(
    eap-error
    PUBLIC
        mpi-cpp eap-kokkos)
target_compile_features(eap-error PUBLIC cxx_std_14)
target_compile_definitions(
    eap-error PUBLIC EAP_FILE_ROOT="${EAP_ERROR_ROOT_DIRECTORY}")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(eap-error PUBLIC "-Wno-gnu-zero-variadic-macro-arguments")
endif()

if (EAP_ERROR_ENABLE_TESTING)
    add_subdirectory(test)
endif()

if (EAP_ERROR_BUILD_EXAMPLES)
    file(GLOB EXAMPLES examples/*.cpp)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        target_link_libraries(${EXAMPLE_EXE} PUBLIC eap-error)
    endforeach()
endif()
