cmake_minimum_required(VERSION 2.8)

project(test)

add_executable(main test_httpcontext.cpp ../HttpContext.cpp ../HttpResponse.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_definitions("-Wall -g")
target_link_libraries(main jsoncpp)
target_link_libraries(main hiredis)
target_link_libraries(main pthread)
target_link_libraries(main net)
target_link_libraries(main log)
