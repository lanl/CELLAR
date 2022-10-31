include(FindPackageHandleStandardArgs)

if (NOT "$ENV{TVROOT}" STREQUAL "")
    set(TOTALVIEW_ROOT $ENV{TVROOT})

    find_program( 
        TOTALVIEW_EXECUTABLE
        totalview
        PATHS ${TOTALVIEW_ROOT}/bin
        NO_DEFAULT_PATH
    )

    find_program( 
        TOTALVIEW_CLI_EXECUTABLE
        totalviewcli
        PATHS ${TOTALVIEW_ROOT}/bin
        NO_DEFAULT_PATH
    )

    execute_process(
        COMMAND ${TOTALVIEW_CLI_EXECUTABLE} -version
        OUTPUT_VARIABLE TOTALVIEW_VERSION_EXTENDED)

    if (TOTALVIEW_VERSION_EXTENDED MATCHES "TotalView ([0-9]+\.[0-9]+\.[0-9]+)")
        set(TOTALVIEW_VERSION ${CMAKE_MATCH_1})
    endif()
endif()

find_package_handle_standard_args(
    TotalView
    FOUND_VAR TOTALVIEW_FOUND
    VERSION_VAR TOTALVIEW_VERSION
    REQUIRED_VARS
        TOTALVIEW_ROOT
        TOTALVIEW_EXECUTABLE
        TOTALVIEW_CLI_EXECUTABLE
)

add_executable(TotalView::TotalView IMPORTED GLOBAL)
set_target_properties(
    TotalView::TotalView
    PROPERTIES
        IMPORTED_LOCATION "${TOTALVIEW_EXECUTABLE}"
)

add_executable(TotalView::CLI IMPORTED GLOBAL)
set_target_properties(
    TotalView::CLI
    PROPERTIES
        IMPORTED_LOCATION "${TOTALVIEW_CLI_EXECUTABLE}"
)

add_library(TotalView::API INTERFACE IMPORTED)
set_target_properties(
    TotalView::API
    PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${TOTALVIEW_ROOT}/include"
)