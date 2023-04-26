cmake_minimum_required(VERSION 3.13)
project(eap-core VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)

option(
    EAP_CORE_ENABLE_FORTRAN
    "Fortran interface is enabled by default. Dependents that do not need \
    Fortran can disable this"
    ON)

if (EAP_CORE_ENABLE_FORTRAN)
    enable_language(Fortran)
endif()

option(
    EAP_CORE_ENABLE_ABI
    "ABI interface is enabled by default if the Fortran interface is enabled. \
    The ABI is not considered stable, and so explicitly requesting it is \
    considered an advanced option."
    ${EAP_CORE_ENABLE_FORTRAN})

mark_as_advanced(EAP_CORE_ENABLE_ABI)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(NDArray)
include(UnitTests)
include(Fortran)

# Block in-source builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif ()

# If the user doesn't specify a build type, prefer Debug. This is just to get a
# good default for developers who typically won't specify additional options to
# CMake.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# Useful for development environments: produces a compile_commands.json file
# that indicates which include paths are needed by each source file in the
# project.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Used to control the file paths emitted by eap-error
set(EAP_ERROR_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Turn on extra warnings, we want to compile cleanly (without any warnings) at
# a high warning level
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")) # TODO XL
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif ()

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR
    ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")) # TODO XL
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif ()

if (("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Clang"))
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wall -Wextra -pedantic")
elseif ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel") # TODO XL
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -warn all")
endif ()

# Fortran macros don't have any line breaks and thus create very long lines,
# turn off any Fortran line length limits
if (("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Clang"))
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -ffree-line-length-none")
elseif ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel") # TODO XL
  # By default, ifort treats *.F90 and *.f90 files as free-form with no limit
  # on the line length
endif ()

# force inline of functions marked for inline even if Intel thinks they're
# too big
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  add_compile_options(-no-inline-max-size -no-inline-max-total-size)
endif ()

# Turn on position independent code globally, this is needed for linking Clang
# C++ static libraries to Fortran executables
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(
    ENABLE_BOUNDS_CHECKING
    "Enable Fortran bounds checking"
    OFF)

if (ENABLE_BOUNDS_CHECKING)
    if ("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fcheck=bounds")
    endif()
endif()

# Options for controlling how EAP Core is built
option(
    EAP_CORE_ENABLE_TESTING
    "Enables testing for eap-core"
    ON)

if (EAP_CORE_ENABLE_TESTING)
    enable_testing()
endif()

if (EAP_CORE_ENABLE_FORTRAN AND NOT EAP_CORE_ENABLE_ABI)
    message(FATAL_ERROR
        "The EAP_CORE_ENABLE_FORTRAN option depends on EAP_CORE_ENABLE_ABI. \
        Set EAP_CORE_ENABLE_ABI to ON to resolve.")
endif()

# Development dependencies
find_package(mpi-cpp 0.3.1 REQUIRED)

if (EAP_CORE_ENABLE_FORTRAN)
    set(MPI_COMPONENTS Fortran)
endif()
find_package(MPI 3.0 REQUIRED COMPONENTS C ${MPI_COMPONENTS})
find_package(Kokkos REQUIRED)
find_package(benchmark QUIET)
find_package(TotalView QUIET 2018)

if (NOT TOTALVIEW_FOUND)
    message(WARNING "TotalView Missing: Building with TotalView can enhance \
        debuggability of the resulting binary")
endif()

# define a eap-specific Kokkos library with the compiler flags we need
add_library(eap-kokkos INTERFACE)
target_link_libraries(eap-kokkos INTERFACE Kokkos::kokkos)
if (CMAKE_CXX_COMPILER MATCHES "nvcc")
    target_compile_options(
        eap-kokkos
        INTERFACE
            --expt-relaxed-constexpr --expt-extended-lambda
            -Xcudafe --diag_suppress=esa_on_defaulted_function_ignored)
endif()

option(
    EAP_CORE_ENABLE_BENCHMARK
    "Build the benchmarks for eap-core"
    ${benchmark_FOUND})

# Test dependencies
if (EAP_CORE_ENABLE_TESTING)
    find_package(GTest REQUIRED)
    find_package(gtest-mpi 0.1 REQUIRED)

    # Query the number of cores for testing purposes
    cmake_host_system_information(RESULT EAP_NUM_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    math(EXPR EAP_NUM_CORES_MPI4 "${EAP_NUM_CORES}/4")
endif()

# Documentation dependencies
find_package(Doxygen 1.8.0)

add_subdirectory(abi)
add_subdirectory(comm)
add_subdirectory(error)
add_subdirectory(mesh)
add_subdirectory(perf)
add_subdirectory(test_cxx_fortran)
add_subdirectory(tv)
add_subdirectory(utility)

if (EAP_CORE_ENABLE_TESTING)
    add_subdirectory(test)
endif()

# Add documentation for all targets
if (DOXYGEN_FOUND)
    # Set universal Doxygen options
    set(DOXYGEN_SHOW_NAMESPACES YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_SOURCE_DIR})
    set(DOXYGEN_EXCLUDE_SYMBOLS internal)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

    set(DOXYGEN_OUTPUT_DIRECTORY docs-cpp)
    doxygen_add_docs(
        # Target Name
        docs-cpp
        # Files and Directories
        README.md
        abi/include
        comm/include
        comm/src
        error/include
        error/src
        mesh/include
        mesh/src
        perf/include
        perf/src
        utility/include
        COMMENT "eap-core C++ documentation"
    )

    set(DOXYGEN_OUTPUT_DIRECTORY docs-ffi)
    doxygen_add_docs(
        # Target Name
        docs-ffi
        # Files and Directories
        README.md
        comm/ffi/include
        comm/ffi/src
        perf/ffi/include/base
        perf/ffi/include/library
        perf/ffi/src
        COMMENT "eap-core FFI documentation"
    )

    set(DOXYGEN_OUTPUT_DIRECTORY docs-fortran)
    doxygen_add_docs(
        # Target Name
        docs-fortran
        # Files and Directories
        README.md
        abi/fortran/src
        comm/fortran/src
        perf/fortran/src
        COMMENT "eap-core Fortran documentation"
    )

    # Umbrella target for all docs
    add_custom_target(docs DEPENDS docs-cpp docs-ffi docs-fortran)
endif()


# install all files
install(
    TARGETS eap-abi-fortran eap-error eap-utility eap-perf eap-comm eap-utility-ffi eap-comm-ffi eap-perf-ffi eap-perf-fortran eap-comm-fortran eap-mesh eap-mesh-ffi eap-mesh-fortran
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)
