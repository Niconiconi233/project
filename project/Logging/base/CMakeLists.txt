cmake_minimum_required(VERSION 2.8)

project(static_lib)

SET(SRC_LIST 
    AsyncLogging.cpp
    Logging.cpp
    CountDownLatch.cpp
    FileUtil.cpp 
    LogFile.cpp 
    LogStream.cpp 
    Thread.cpp
    ConfigReader.cc 
    )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")
add_library(log ${SRC_LIST})
TARGET_LINK_LIBRARIES(log pthread)

