cmake_minimum_required(VERSION 2.8)

project(server)

SET(SRC_LIST
        main.cpp
        HttpSession.cpp
        HttpServer.cpp
        HttpResponse.cpp
        HttpContext.cpp
        HttpContext.cpp
        Redis.cpp
        RapidJson.cpp)

add_executable(server ${SRC_LIST})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")
target_link_libraries(server pthread)
target_link_libraries(server log)
target_link_libraries(server net)
target_link_libraries(server mysqlcppconn)
target_link_libraries(server hiredis)