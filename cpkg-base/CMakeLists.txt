cmake_minimum_required(VERSION 3.16)
project(cpkg-base VERSION 0.1.0)

include(CMakePackageConfigHelpers)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp src/*.hpp)

add_library(${PROJECT_NAME} SHARED
${${PROJECT_NAME}_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC "CPKG_BASE_SOURCE_PREFIX=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "CPKG_BASE_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PROJECT_NAME}
)

# # Install header files
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
#     DESTINATION include/${PROJECT_NAME}
#     FILES_MATCHING
#         PATTERN "*.h"
#         PATTERN "*.hpp"
# )

# # Install header files
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
#     DESTINATION share/${PROJECT_NAME}/headers
#     FILES_MATCHING
#         PATTERN "*.h"
#         PATTERN "*.hpp"
# )

# Install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION lib/${PROJECT_NAME}/headers
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

# Export target file
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Generate and install the ConfigVersion file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate Config file manually (you can also write a .in template if needed)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)