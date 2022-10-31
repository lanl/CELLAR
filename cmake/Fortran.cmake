function(target_set_mod_includes TARGET_NAME)
    target_include_directories(
        ${TARGET_NAME}
        INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/mod")
    set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
            Fortran_MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mod")
endfunction()