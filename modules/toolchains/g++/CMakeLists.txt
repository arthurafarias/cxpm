cmake_minimum_required(VERSION 3.16)
project(cxpm-toolchain-g++-default)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PUBLIC cxpm-interface)