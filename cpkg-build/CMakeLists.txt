cmake_minimum_required(VERSION 3.16)
project(cpkg-build VERSION 0.1.0)

add_executable(cpkg-build src/cpkg-build.cpp)
target_link_libraries(cpkg-build PUBLIC cpkg-base)
target_compile_definitions(cpkg-build PRIVATE "CPKG_BUILD_HEADERS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src\"")