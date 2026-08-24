cmake_minimum_required(VERSION 3.16)
project(cxpm-toolchain-gcc-default)

install(FILES src/toolchain.cpp DESTINATION share/cxpm/toolchains/gcc)
