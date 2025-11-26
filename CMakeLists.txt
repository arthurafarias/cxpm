cmake_minimum_required(VERSION 3.16)
project(cpkg VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(cpkg-base)
add_subdirectory(cpkg-build)
# add_subdirectory(cpkg-clean)
# add_subdirectory(cpkg-create)
# add_subdirectory(cpkg-install)


add_subdirectory(toolchains/gcc)