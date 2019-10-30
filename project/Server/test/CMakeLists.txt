cmake_minimum_required(VERSION 2.8)

project(test)

add_executable(main test_tasklist.cpp ../TaskList.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC")

add_definitions("-Wall -g")
target_link_libraries(main pthread)
target_link_libraries(main log)
target_link_libraries(main net)

