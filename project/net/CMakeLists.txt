cmake_minimum_required(VERSION 2.8)

project(static_lib)

set(net_SRCS
        Acceptor.cc
        Buffer.cc
        Channel.cc
        Connector.cc
        EventLoop.cc
        EventLoopThread.cc
        EventLoopThreadPool.cc
        InetAddress.cc
        EPollPoller.cc
        Socket.cc
        SocketsOps.cc
        TcpClient.cc
        TcpConnection.cc
        TcpServer.cc
        Timer.cc
        TimerQueue.cc
        Timestamp.cc
        TimerQueue_with_heap.cpp
        TimerHeap.hpp)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ADD_LIBRARY (net STATIC ${net_SRCS})
add_definitions("-Wall -g")
target_link_libraries(net pthread)
target_link_libraries(net log)