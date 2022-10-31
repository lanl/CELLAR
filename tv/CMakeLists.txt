if (TOTALVIEW_FOUND)
    add_library(
        eap-totalview
        src/totalview-viz-bool.cpp
        src/totalview-viz-double.cpp
        src/totalview-viz-float.cpp
        src/totalview-viz-i32.cpp
        src/totalview-viz-i64.cpp
        src/totalview-viz-u32.cpp
        src/totalview-viz-u64.cpp
        src/totalview-viz.cpp
        ${TOTALVIEW_ROOT}/src/tv_data_display.c)
    set_property(
        SOURCE ${TOTALVIEW_ROOT}/src/tv_data_display.c
        PROPERTY LANGUAGE CXX)
    target_include_directories(eap-totalview PUBLIC include)
    target_link_libraries(
        eap-totalview
        PUBLIC TotalView::API eap-kokkos
        PRIVATE eap-utility)
    target_compile_definitions(eap-totalview PUBLIC EAP_TOTALVIEW)
    target_compile_features(eap-totalview PUBLIC cxx_std_14)
    target_link_options(
        eap-totalview
        PUBLIC "LINKER:--require-defined=eap_totalview_mark_as_used")
else()
    add_library(eap-totalview INTERFACE)
    target_include_directories(eap-totalview INTERFACE include)
    target_compile_features(eap-totalview INTERFACE cxx_std_14)
endif()

if (TOTALVIEW_FOUND AND EAP_CORE_ENABLE_TESTING)
    add_subdirectory(test)
endif()