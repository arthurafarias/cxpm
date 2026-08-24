cmake_minimum_required(VERSION 3.16)
project(cxpm-build-system)

option(CXPM_BUILD_TESTS "Build the cxpm_tests unit suite and CLI CTest tooling" ON)
option(CXPM_ENABLE_SANITIZERS "Enable ASan and UBSan for cxpm_tests" ON)
option(CXPM_ENABLE_COVERAGE "Instrument cxpm_tests for gcov/gcovr coverage" OFF)
option(CXPM_BUILD_CLI_INTEGRATION_TESTS
       "Additionally register slow CLI integration tests that JIT-compile a real toolchain plugin and shell out to a real compiler (needs a C++ compiler on PATH; off by default because of that cost/fragility, see docs/SRS-ctest-tooling.md)"
       OFF)

add_subdirectory(libraries/cxpm-interface)

add_subdirectory(applications/cxpm)
add_subdirectory(applications/cxpm-descriptor-sandbox)

add_subdirectory(modules/toolchains/g++)
add_subdirectory(modules/toolchains/gcc)
add_subdirectory(modules/toolchains/nvcc)

if(CXPM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
