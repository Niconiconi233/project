//
// Created by soft01 on 2019/8/1.
//

#ifndef TEST_HTTPSESSION_H
#define TEST_HTTPSESSION_H

#include <memory>

#include "../net/Timestamp.h"
#include "HttpContext.h"
#include "DbManager.h"

class TcpConnection;
class Buffer;
class HttpRequest;
class HttpResponse;
class HttpServer;

class HttpSession {
public:
    HttpSession(const std::shared_ptr<TcpConnection>& ptr, int session, HttpServer* server);

    ~HttpSession()
    {
    }

    HttpSession(const HttpSession&) = delete;
    HttpSession& operator=(const HttpSession&) = delete;

    void onMessage(const std::shared_ptr<TcpConnection>& ptr, Buffer* buffer, Timestamp recvtime);

private:
    //事件分发器
    void handleEvents(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //登陆相关
    void doLogin(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //logout
    void doLogout(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpresponce);

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

    //修改用户信息
    void doModUser(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //修改商品信息
    void doModProduction(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //在线交流相关
    void doChat(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //请求判断是否有聊天信息
    void getChatMessage(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //持久化聊天记录
    void addChatMessage(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //错误处理相关
    void handleError(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse httpResponse, int statusCode, const char* debug);

    //查询用户信息
    void searchAndSendUser(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取页面缩略信息
    void searchAndSendCategories(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //根据关键字进行搜索
    void searchAndSendCategory(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取物品详细信息
    void searchAndSendGoods(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //推测用户喜爱商品
    void doGuss(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //封装的发送函数
    void send(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse, const std::string& body);

    //获取用户发布的商品
    void searchAndSendUserProductions(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //点赞
    void doLike(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //处理留言
    void doProductionMessage(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取商品留言
    void getProductionMessage(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //添加商品留言
    void addProductionMessage(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //订阅相关
    void doSubscribe(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取订阅用户列表
    void getSubscribeUserList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //订阅用户
    void addSubscribeUserList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取订阅
    void getSubscribeProductionList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //添加订阅
    void addSubscribeProductionList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //订单相关
    void doOrder(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //创建订单
    void startOrder(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //完成订单
    void finishOrder(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //获取用户订单
    void orderList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //历史订单
    void getHistoryOrderList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

    //当前未完成订单
    void getUnfinishOrderList(const std::shared_ptr<TcpConnection>& ptr, HttpRequest& httpRequest, HttpResponse& httpResponse);

private:
    std::weak_ptr<TcpConnection> tcpconnection_;
    int sessionid_;
    Timestamp recvtime_;
    std::unique_ptr<HttpContext> httpContext_;
    std::unique_ptr<Buffer> httpBuffer_;
    std::unique_ptr<DbManager> dbManager_;
    HttpServer* httpServer_;
};


#endif //TEST_HTTPSESSION_H
