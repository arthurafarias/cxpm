cmake_minimum_required(VERSION 3.10)
project(cxpm)

file(GLOB_RECURSE cxpm_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(cxpm ${cxpm_SOURCES})

add_library(cxpm-interface INTERFACE)
target_include_directories(cxpm-interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(cxpm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)