add_library(cpkg-default-toolchain-g++ SHARED toolchain.cpp)
target_link_libraries(cpkg-default-toolchain-g++ PUBLIC cpkg-base)
target_compile_definitions(cpkg-build PRIVATE "CPKG_BUILD_HEADERS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src\"")

set_target_properties(cpkg-default-toolchain-g++
    PROPERTIES
        PREFIX ""
)