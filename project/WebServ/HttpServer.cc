#include "HttpServer.h"

#include "../net/Buffer.h"
#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/InetAddress.h"
#include "../Logging/base/Logging.h"
#include "HttpSession.h"

#include <functional>

HttpServer::HttpServer(const std::string ip, const int16_t port, EventLoop* loop, int threadNumber)
    :loop_(loop),
    sessionId_(0),
    ip_(ip),
    port_(port),
    Gzip_(false),
    threadNumber_(threadNumber),
    mutex_()
{

}

HttpServer::~HttpServer()
{
    tcpServer_.reset();
    for(auto& i : sessionLists_)
        i.second.reset();
}

void HttpServer::init()
{
    //InetAddr servaddr(ip_, port_);
    InetAddress servaddr(ip_, port_);
    tcpServer_.reset(new TcpServer(loop_, servaddr, "server"));
    //tcpServer_->setThreadNumber(threadNumber_);
    tcpServer_->setThreadNum(threadNumber_);
    tcpServer_->setConnectionCallback(std::bind(&HttpServer::onConnect, this, std::placeholders::_1));
    tcpServer_->start();
}

void HttpServer::onConnect(const TcpConnectionPtr& ptr)
{
    if(ptr->connected()){
        std::shared_ptr<HttpSession> session(new HttpSession(ptr, sessionId_.load()));
        if(Gzip_)
            session->setEnableGzip(true);
        ++sessionId_;
        {
            MutexLockGurard lock(mutex_);
            sessionLists_[ptr->getFd()] = session;
        }
        ptr->setMessageCallback(std::bind(&HttpSession::onMessage, session.get(), std::placeholders::_1, std::placeholders::_2));
        LOG_DEBUG << "new connection from " << ptr->peerAddress().toIpPort();
    }
    else
    {
        {
            MutexLockGurard lock(mutex_);
            size_t ret = sessionLists_.erase(ptr->getFd());
            assert(ret == 1);
            LOG_DEBUG << "HttpServer::onconnection connection closed";
        }
    }
    
}

void HttpServer::enableGzip(bool on)
{
    if(on)
        LOG_LOG << "HttpServer enabled Gzip";
    Gzip_ = on;
}
