# Run via: cmake -DCXPM_INSTALL_PREFIX=<dir> -DCXPM_TARGET_NAME=<name> -P verify_install.cmake
# Asserts the POSIX-aligned install layout documented in docs/tooling.md actually landed on
# disk after a `cxpm --install` run: <prefix>/bin/<name> and <prefix>/lib/pkgconfig/<name>.pc.
if(NOT DEFINED CXPM_INSTALL_PREFIX OR NOT DEFINED CXPM_TARGET_NAME)
    message(FATAL_ERROR "verify_install.cmake requires -DCXPM_INSTALL_PREFIX=... -DCXPM_TARGET_NAME=...")
endif()

set(_binary "${CXPM_INSTALL_PREFIX}/bin/${CXPM_TARGET_NAME}")
set(_pc_file "${CXPM_INSTALL_PREFIX}/lib/pkgconfig/${CXPM_TARGET_NAME}.pc")

if(NOT EXISTS "${_binary}")
    message(FATAL_ERROR "expected installed binary not found: ${_binary}")
endif()

if(NOT EXISTS "${_pc_file}")
    message(FATAL_ERROR "expected installed .pc file not found: ${_pc_file}")
endif()

message(STATUS "install layout verified: ${_binary} and ${_pc_file} both exist")
