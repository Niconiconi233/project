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
#include "TaskList.h"

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
    //void enableGzip(bool on);

    void onConnect(const TcpConnectionPtr&);

    void checkStatus();

    void pushTask(int time, const std::string& orderlist)
    {
        taskList_.push(time, orderlist);
    }

    bool cancelTask(const std::string& orderlist)
    {
        return taskList_.remove(orderlist);
    }

    void addUser(int uid, const std::string& token)
    {
        MutexLockGurard lock(userMutex_);
        userLists_[uid] = std::move(token);
    }

    bool checkUser(int uid, const std::string& token)
    {
        MutexLockGurard lock(userMutex_);
        auto it = userLists_.find(uid);
        if(it != userLists_.end() && memcmp(it->second.c_str(), token.c_str(), token.length()) == 0)
            return true;
        else
            return false;
    }

    bool existUser(int uid)
    {
        MutexLockGurard lock(userMutex_);
        return userLists_.find(uid) != userLists_.end();
    }

    std::string getUser(int uid)
    {
        MutexLockGurard lock(userMutex_);
        auto it = userLists_.find(uid);
        if(it != userLists_.end())
            return userLists_.find(uid)->second;
        else
            return std::string{};
    }

    void deleteUser(int uid)
    {
        MutexLockGurard lock(userMutex_);
        auto it = userLists_.find(uid);
        if(it != userLists_.end())
            userLists_.erase(it);
    }


private:
    EventLoop* loop_;
    std::unique_ptr<TcpServer> tcpServer_;
    static std::atomic_int sessionId_;
    std::string ip_;
    int16_t port_;
    int threadNumber_;
    std::map<int, std::shared_ptr<HttpSession>> sessionLists_;
    MutexLock sessionMutex_;
    TaskList taskList_;
    std::map<int, std::string> userLists_;
    MutexLock userMutex_;

};

#endif //TEST_HTTPSERVER_H
