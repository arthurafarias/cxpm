add_executable(ClangdManagerTest ClangdManagerTest.cpp)
target_link_libraries(ClangdManagerTest PRIVATE cpkg-base)
target_compile_definitions(ClangdManagerTest PRIVATE "CPKG_BUILD_HEADERS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src\"")