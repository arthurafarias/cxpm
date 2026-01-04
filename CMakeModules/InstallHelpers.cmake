set(CMAKE_CURRENT_MODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

function(PROJECT_TARGET_INSTALL TARGET_NAME INSTALL_LIB_DIR INSTALL_BIN_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR)
## Install Rules

include(CMakePackageConfigHelpers)

if (INSTALL_LIB_DIR STREQUAL "")
    set(INSTALL_LIB_DIR lib)
endif()

if (INSTALL_BIN_DIR STREQUAL "")
    set(INSTALL_BIN_DIR bin)
endif()

if (INSTALL_INCLUDE_DIR STREQUAL "")
    set(INSTALL_INCLUDE_DIR include)
endif()

if (INSTALL_CMAKE_DIR STREQUAL "")
    set(INSTALL_CMAKE_DIR lib/cmake/${TARGET_NAME})
endif()

# Install the library target
install(TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}Targets
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
    INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
)

# Install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION ${INSTALL_INCLUDE_DIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

# Export target file
install(EXPORT ${TARGET_NAME}Targets
    FILE ${TARGET_NAME}Targets.cmake
    NAMESPACE ${TARGET_NAME}::
    DESTINATION ${INSTALL_CMAKE_DIR}
)

# Generate and install the ConfigVersion file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate Config file manually (you can also write a .in template if needed)
configure_package_config_file(
    ${CMAKE_CURRENT_MODULE_SOURCE_DIR}/cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake
    INSTALL_DESTINATION ${INSTALL_CMAKE_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake
    DESTINATION ${INSTALL_CMAKE_DIR}
)

endfunction()