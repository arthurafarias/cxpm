cmake_minimum_required(VERSION 3.16)
project(cxpm_server VERSION 0.1.0)

include(CMakePackageConfigHelpers)

find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE cxpm_server_SOURCES src/*.cpp src/*.hpp)

add_executable(cxpm_server ${cxpm_server_SOURCES})

# target_compile_options(cxpm_server PRIVATE -Wall -Werror -Wextra -pedantic)

target_link_libraries(cxpm_server PUBLIC cxpm-base SQLite::SQLite3)

target_include_directories(cxpm_server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include/cxpm_server>
)

target_compile_definitions(cxpm_server PUBLIC "cxpm_server_BUILD_SOURCE_PREFIX=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
target_compile_definitions(cxpm_server PUBLIC "cxpm_server_BUILD_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

install(TARGETS cxpm_server
    EXPORT cxpm_serverTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/cxpm_server
)

# Install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION include/cxpm_server
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

# Export target file
install(EXPORT cxpm_serverTargets
    FILE cxpm_serverTargets.cmake
    NAMESPACE cxpm_server::
    DESTINATION lib/cmake/cxpm_server
)

# Generate and install the ConfigVersion file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cxpm_serverConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate Config file manually (you can also write a .in template if needed)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cxpm_serverConfig.cmake
    INSTALL_DESTINATION lib/cmake/cxpm_server
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cxpm_serverConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cxpm_serverConfigVersion.cmake
    DESTINATION lib/cmake/cxpm_server
)

add_subdirectory(test)