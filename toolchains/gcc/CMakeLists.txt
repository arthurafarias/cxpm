cmake_minimum_required(VERSION 3.10)
project(cxpm-toolchain-gcc)

file(GLOB_RECURSE cxpm-toolchain-gcc_SOURCES src/*.cpp)
add_library(cxpm-toolchain-gcc SHARED ${cxpm-toolchain-gcc_SOURCES})
target_include_directories(cxpm-toolchain-gcc PRIVATE src)
target_link_libraries(cxpm-toolchain-gcc PRIVATE cxpm-interface)
