cmake_minimum_required(VERSION 3.10)
project(cxpm-toolchain-g++ VERSION 0.1.0)

file(GLOB_RECURSE cxpm-toolchain-g++_SOURCES src/*.cpp)
add_library(cxpm-toolchain-g++ SHARED ${cxpm-toolchain-g++_SOURCES})
target_include_directories(cxpm-toolchain-g++ PRIVATE src)
target_link_libraries(cxpm-toolchain-g++ PRIVATE cxpm-interface)

project_target_install(cxpm-toolchain-g++ lib/cxpm/toolchains bin include lib/cmake/cxpm-toolchain-g++)