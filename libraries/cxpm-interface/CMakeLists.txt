cmake_minimum_required(VERSION 3.16)
project(cxpm-interface)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(${PROJECT_NAME} INTERFACE cxpm_BASE_SOURCE_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra)