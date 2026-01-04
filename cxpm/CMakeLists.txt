cmake_minimum_required(VERSION 3.10)
project(cxpm VERSION 0.1.0)

add_library(cxpm-interface INTERFACE)
target_include_directories(cxpm-interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE cxpm_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(cxpm ${cxpm_SOURCES})
target_link_libraries(cxpm PRIVATE cxpm-interface)

project_target_install(cxpm lib bin include lib/cmake/cxpm)