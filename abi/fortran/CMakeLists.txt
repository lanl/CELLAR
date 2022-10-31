file(GLOB SOURCES src/*.F90)

add_library(eap-abi-fortran ${SOURCES})
target_set_mod_includes(eap-abi-fortran)
target_include_directories(
    eap-abi-fortran
    INTERFACE include
    PRIVATE generated)

if (EAP_ABI_ENABLE_TESTING)
    add_subdirectory(test)
endif()

# This is code for generating generated/abi-to_ndarray_impls.f90
set(ND_ARRAY_TYPES LOGICAL_BOOL INT32 INT64 REAL32 REAL64)

set(LOGICAL_BOOL_FT "logical(c_bool)")
set(INT32_FT "integer(INT32)")
set(INT64_FT "integer(INT64)")
set(REAL32_FT "real(REAL32)")
set(REAL64_FT "real(REAL64)")

set(LOGICAL_BOOL_SH l)
set(INT32_SH i32)
set(INT64_SH i64)
set(REAL32_SH r32)
set(REAL64_SH r64)

set(NEW_CONTENTS
"!!!! THIS FILE IS GENERATED - DO NOT EDIT DIRECTLY !!!!
")

foreach(ARRAY_TYPE ${ND_ARRAY_TYPES})
    set(VAR_TYPE ${${ARRAY_TYPE}_FT})
    set(VAR_SHORTHAND ${${ARRAY_TYPE}_SH})

    generate_nd_array(7 ${${ARRAY_TYPE}_FT} ${${ARRAY_TYPE}_SH} APPEND_CONTENTS)

    set(
        NEW_CONTENTS
"${NEW_CONTENTS}
${APPEND_CONTENTS}
")
endforeach()

file(
    READ
        "${CMAKE_CURRENT_SOURCE_DIR}/generated/abi-to_ndarray_impls.f90"
        CURRENT_CONTENTS
)

if (NOT "${CURRENT_CONTENTS}" STREQUAL "${NEW_CONTENTS}")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/generated/abi-to_ndarray_impls.f90" ${NEW_CONTENTS})
endif()
