file(GLOB SOURCES src/*.cpp)

add_executable(eap-mesh-bench ${SOURCES})
target_link_libraries(
    eap-mesh-bench
    PUBLIC benchmark::benchmark eap-mesh)