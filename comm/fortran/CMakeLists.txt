file(GLOB SOURCES src/*.F90)

add_library(eap-comm-fortran ${SOURCES})
target_set_mod_includes(eap-comm-fortran)
target_include_directories(
    eap-comm-fortran
    PUBLIC include)
target_link_libraries(eap-comm-fortran
    PUBLIC MPI::MPI_Fortran eap-perf-fortran
    PRIVATE eap-comm-ffi eap-abi-fortran)
if (MPI_Fortran_LIBRARIES MATCHES "mpi_ibm_usempi")
    target_compile_definitions(eap-comm-fortran PUBLIC TOKEN_USE_MPI)
else()
    target_compile_definitions(eap-comm-fortran PUBLIC TOKEN_USE_MPI_F08)
endif()

if (EAP_COMM_ENABLE_TESTING)
    add_subdirectory(test)
endif()
