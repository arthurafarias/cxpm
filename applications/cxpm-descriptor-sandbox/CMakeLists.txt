cmake_minimum_required(VERSION 3.16)
project(cxpm-descriptor-sandbox)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC cxpm-interface)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

# See applications/cxpm/CMakeLists.txt and docs/testing.md: lets the coverage target count
# lines exercised by the sandbox_* CTest cases, which run this binary directly.
if(CXPM_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage -O0 -g)
    target_link_options(${PROJECT_NAME} PRIVATE --coverage)
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
