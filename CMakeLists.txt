cmake_minimum_required(VERSION 3.16)
project(cxpm-build-system)

add_subdirectory(libraries/cxpm-interface)

add_subdirectory(applications/cxpm)

add_subdirectory(modules/toolchains/g++)
add_subdirectory(modules/toolchains/gcc)
