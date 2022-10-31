option(
    EAP_MESH_FORTRAN_BUILD_EXAMPLES
    "Build examples for eap-mesh-fortran"
    ${EAP_MESH_BUILD_EXAMPLES})

file(GLOB SOURCES src/*.F90)

add_library(eap-mesh-fortran ${SOURCES})
target_set_mod_includes(eap-mesh-fortran)
target_include_directories(
    eap-mesh-fortran
    PRIVATE generated)
target_link_libraries(
    eap-mesh-fortran
    PUBLIC eap-comm-fortran
    PRIVATE eap-mesh-ffi eap-abi-fortran)

if (EAP_MESH_FORTRAN_BUILD_EXAMPLES)
    file(GLOB EXAMPLES examples/*.F90)
    foreach (EXAMPLE ${EXAMPLES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)

        set(EXAMPLE_EXE ${EXAMPLE_NAME}-example)
        add_executable(${EXAMPLE_EXE} ${EXAMPLE})
        set_target_properties(${EXAMPLE_EXE} PROPERTIES LINKER_LANGUAGE Fortran)
        target_link_libraries(
            ${EXAMPLE_EXE}
            PUBLIC eap-mesh-fortran
            PRIVATE eap-utility-fortran eap-totalview # for kokkos_initialize/finalize
        )

        eap_core_add_mpi_test(
            NAME ${EXAMPLE_EXE}
            COMMAND $<TARGET_FILE:${EXAMPLE_EXE}>)
    endforeach()
endif()

if (EAP_CORE_ENABLE_TESTING)
    add_subdirectory(test)
endif()

set(NEW_CONTENTS
"!!!! THIS FILE IS GENERATED - DO NOT EDIT DIRECTLY !!!!
")

    generate_nd_array(1 "type(loc_val_t)" "lv" APPEND_CONTENTS)

    set(
        NEW_CONTENTS
"${NEW_CONTENTS}
${APPEND_CONTENTS}
")

file(
    READ
        "${CMAKE_CURRENT_SOURCE_DIR}/generated/mesh-to_ndarray_impls.f90"
        CURRENT_CONTENTS
)

if (NOT "${CURRENT_CONTENTS}" STREQUAL "${NEW_CONTENTS}")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/generated/mesh-to_ndarray_impls.f90" ${NEW_CONTENTS})
endif()
