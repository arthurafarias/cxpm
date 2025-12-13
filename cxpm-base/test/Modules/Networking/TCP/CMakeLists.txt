add_executable(${PROJECT_NAME}_Modules_Networking_HTTP_URLTest URLTest.cpp)
target_link_libraries(${PROJECT_NAME}_Modules_Networking_HTTP_URLTest PUBLIC cxpm-base)

add_executable(${PROJECT_NAME}_Modules_Networking_HTTP_SocketTest SocketTest.cpp)
target_link_libraries(${PROJECT_NAME}_Modules_Networking_HTTP_SocketTest PUBLIC cxpm-base)