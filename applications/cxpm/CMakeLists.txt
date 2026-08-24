cmake_minimum_required(VERSION 3.16)
project(cxpm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_link_libraries(${PROJECT_NAME} PUBLIC cxpm-interface)

# So the coverage target (tests/CMakeLists.txt) can count lines actually exercised through
# real `cxpm` invocations -- the CLI/integration CTest tier -- not just cxpm_tests' unit runs.
# See docs/testing.md.
if(CXPM_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage -O0 -g)
    target_link_options(${PROJECT_NAME} PRIVATE --coverage)
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})