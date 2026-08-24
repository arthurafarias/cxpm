cmake_minimum_required(VERSION 3.23)
project(cxpm-interface)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Every */Testing/*.hpp header (the harness itself under CXPM/Testing/, plus each *Test.hpp
# colocated with the class/function it covers, e.g. CXPM/Core/Containers/Testing/StringTest.hpp)
# ships in the source tree so tests stay next to the code they exercise (see docs/testing.md),
# but none of it is part of the installed public API and must not be packaged.
list(FILTER ${PROJECT_NAME}_HEADERS EXCLUDE REGEX "/Testing/")

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME}
  INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
    FILES ${${PROJECT_NAME}_HEADERS}
)

# cxpm_SANDBOX_HELPER_PATH points CXPM::Controllers::DescriptorSandbox (see
# CXPM/Controllers/DescriptorSandbox.hpp, docs/SRS-sandbox.md) at the cxpm-descriptor-sandbox
# helper this same build produces, so a build/install of cxpm never depends on that helper
# already being installed system-wide. $<TARGET_FILE:...> is a generator expression: CMake only
# needs the target to exist somewhere by the end of configuration, not before this line runs.
target_compile_definitions(${PROJECT_NAME} INTERFACE
  cxpm_BASE_SOURCE_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}"
  cxpm_SANDBOX_HELPER_PATH="$<TARGET_FILE:cxpm-descriptor-sandbox>"
)
target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)