//
// Created by soft01 on 2019/8/1.
//

#ifndef TEST_HTTPSESSION_H
#define TEST_HTTPSESSION_H

#include <memory>

#include "../net/Timestamp.h"
#include "HttpContext.h"
//#include "HttpRequest.h"
//#include "RequestParse.h"

class TcpConnection;
class Buffer;
class HttpRequest;
class HttpResponse;

class HttpSession {
public:
    HttpSession(const std::shared_ptr<TcpConnection>& ptr, int session)
        :tcpconnection_(ptr),
        sessionid_(session),
        httpeContext_(new HttpContext)
    {}

    ~HttpSession()
    {
        //parse_.reset();
    }

    HttpSession(const HttpSession&) = delete;
    HttpSession& operator=(const HttpSession&) = delete;

    void onMessage(const std::shared_ptr<TcpConnection>& ptr, Buffer* buffer, Timestamp recvtime);

private:
    //事件分发器
    void handleEvents(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //登陆相关
    void doLogin(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //注册相关
    void doRegist(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取首页相关
    void doGetIndex(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //查找相关
    void doSearch(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //发布相关
    void doRelease(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //修改信息相关
    void doMod(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //在线交流相关
    void doChat(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //错误处理相关
    void handleError(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse httpResponse);

    //std::string headerConstructor(int code, const std::string mime, int len);

    //查询用户信息
    void searchAndSendUser(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取页面缩略信息
    void searchAndSendCategories(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //根据关键字进行搜索
    void searchAndSendCategory(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取物品详细信息
    void searchAndSendGoods(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);


    //void close(const std::shared_ptr<TcpConnection>& ptr, const HttpRequest& httpRequest, const HttpResponse& httpResponse);

    //推测用户喜爱商品
    void doGuss(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);


    void send(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse, std::string& body);


private:
    std::weak_ptr<TcpConnection> tcpconnection_;
    int sessionid_;
    Timestamp recvtime_;
    std::unique_ptr<HttpContext> httpeContext_;
};


#endif //TEST_HTTPSESSION_H
