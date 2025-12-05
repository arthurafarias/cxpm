cmake_minimum_required(VERSION 3.16)
project(cpkg VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(cxpm-base)
add_subdirectory(cxpm)


add_subdirectory(toolchains/gcc)
add_subdirectory(toolchains/g++)

