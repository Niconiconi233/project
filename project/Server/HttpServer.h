//
// Created by soft01 on 2019/8/1.
//

#ifndef TEST_HTTPSERVER_H
#define TEST_HTTPSERVER_H

#include <map>
#include <memory>
#include <atomic>

#include "../net/TcpConnection.h"
#include "../Logging/base/MutexLock.h"

class HttpSession;
class EventLoop;
class Buffer;
class TcpServer;

class HttpServer
{
public:
    HttpServer(const std::string ip, const int16_t port, EventLoop*, int threadNumber = 0);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    void init();
    void enableGzip(bool on);

    void onConnect(const TcpConnectionPtr&);
    // void onMessage(TcpConnectionPtr&, Buffer*);

private:
    EventLoop* loop_;
    std::unique_ptr<TcpServer> tcpServer_;
    static std::atomic_int sessionId_;
    std::string ip_;
    int16_t port_;
    int threadNumber_;
    std::map<int, std::shared_ptr<HttpSession>> sessionLists_;
    MutexLock mutex_;
};

#endif //TEST_HTTPSERVER_H
