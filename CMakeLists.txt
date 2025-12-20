cmake_minimum_required(VERSION 3.16)
project(cxpm VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

include(CPack/Config.cmake)
include(CPack)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(cxpm-base)
add_subdirectory(cxpm)
add_subdirectory(cxpm-server)


add_subdirectory(toolchains/gcc)
add_subdirectory(toolchains/g++)

