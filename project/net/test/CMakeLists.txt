cmake_minimum_required(VERSION 2.8)

project(test)

add_executable(main test.cc)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
target_link_libraries(main net)
target_link_libraries(main log)
target_link_libraries(main pthread)