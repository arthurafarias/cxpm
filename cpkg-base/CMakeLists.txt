cmake_minimum_required(VERSION 3.16)
project(cpkg-base VERSION 0.1.0)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp src/*.hpp)
add_library(cpkg-base SHARED
${${PROJECT_NAME}_SOURCES}
)

target_include_directories(cpkg-base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(cpkg-base PUBLIC "CPKG_BASE_INSTALL_HEADERS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src\"")
target_compile_definitions(cpkg-base PUBLIC "CPKG_BASE_HEADERS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src\"")