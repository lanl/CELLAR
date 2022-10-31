file(GLOB SOURCES src/*.cpp)

add_executable(eap-utility-bench ${SOURCES})
target_link_libraries(
    eap-utility-bench
    PUBLIC benchmark::benchmark eap-utility)