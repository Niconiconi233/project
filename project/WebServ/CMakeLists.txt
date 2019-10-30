cmake_minimum_required(VERSION 2.8)

project(server)

SET(SRC_LIST
    HttpSession.cc 
    HttpServer.cc 
    main.cc)

add_executable(main ${SRC_LIST})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")
target_link_libraries(main log)
target_link_libraries(main net)
target_link_libraries(main pthread)
target_link_libraries(main z)
