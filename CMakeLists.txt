cmake_minimum_required(VERSION 3.10)
project(cxpm-project VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
include(InstallHelpers)

add_compile_options(-Wall -Werror -Wextra -pedantic)

add_subdirectory(cxpm)
add_subdirectory(toolchains)