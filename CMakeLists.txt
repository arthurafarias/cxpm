cmake_minimum_required(VERSION 3.10)
project(cxpm-project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_compile_options(-Wall -Werror -Wextra -pedantic)

add_subdirectory(cxpm)
add_subdirectory(toolchains)