//
// Created by soft01 on 2019/8/1.
//
#include "HttpSession.h"
#include "../net/Buffer.h"
#include "../net/TcpConnection.h"
#include "../Logging/base/Logging.h"
#include "../Logging/base/SingLeton.h"
#include "RedisConnectionPool.h"
#include "HttpServer.h"
#include "RapidJson.h"
#include "HttpResponse.h"
#include "HttpRequest.h"


//#ifndef debug
//#define debug

#ifdef debug
#include <chrono>
#endif

namespace
{
    #define MAX_URL_LENGTH 1024 * 3
}

HttpSession::HttpSession(const std::shared_ptr<TcpConnection>& ptr, int session, HttpServer* server)
        :tcpconnection_(ptr),
         sessionid_(session),
         httpContext_(new HttpContext),
         httpBuffer_(new Buffer),
         dbManager_(new DbManager),
         httpServer_(server)
{}

void HttpSession::onMessage(const std::shared_ptr<TcpConnection> &ptr, Buffer *buffer, Timestamp recvtime)
{
    if(buffer->readableBytes() < 4)
        return;
    if(buffer->readableBytes() > MAX_URL_LENGTH)
    {
        LOG_FATAL << "HttpSession::onMessage failed message to large " << buffer->readableBytes();
        ptr->forceClose();
    }
    httpContext_->reset();
    httpContext_->parseRequest(buffer);
    if(httpContext_->gotAll())
    {
        HttpRequest& request = httpContext_->request();
        recvtime_ = recvtime;
        HttpResponse response(httpBuffer_.get(), false);
        if(request.method() == HttpRequest::kOptions)
        {
            response.addHeader("Access-Control-Max-Age", "3600");
            send(ptr, request, response, "");
        }
        handleEvents(ptr, request, response);
    } else
    {
        return;
    }
    assert(buffer->readableBytes() == 0);
}

void HttpSession::handleEvents(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                               HttpResponse& httpResponse)
{
    std::string path(std::move(httpRequest.path()));
    if(memcmp(path.c_str(), "Login", 5) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doLogin(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doLogin speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }
    else if(memcmp(path.c_str(), "Regist", 6) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doRegist(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }
    else if(memcmp(path.c_str(), "Index", 5) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doGetIndex(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }
    else if(memcmp(path.c_str(), "Search", 6) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doSearch(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }
    else if(memcmp(path.c_str(), "Release", 7) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doRelease(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }else if(memcmp(path.c_str(), "Guss", 4) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doGuss(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }else if(memcmp(path.c_str(), "Logout", 6) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doLogout(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }else if(memcmp(path.c_str(), "Mod", 3) == 0)
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doMod(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }else if(memcmp(path.c_str(), "Chat", 4) == 0)
    {
        doChat(ptr, httpRequest, httpResponse);
    }else if(memcmp(path.c_str(), "Like", 4) == 0)
    {
        doLike(ptr, httpRequest, httpResponse);
    }
    else if(memcmp(path.c_str(), "Leftmessage", 11) == 0)
    {
        doProductionMessage(ptr, httpRequest, httpResponse);

    }else if(memcmp(path.c_str(), "Subscribe", 9) == 0)
    {
        doSubscribe(ptr, httpRequest, httpResponse);

    }else if(memcmp(path.c_str(), "Order", 5) == 0)
    {
        doOrder(ptr, httpRequest, httpResponse);

    }else if(memcmp(path.c_str(), "List", 4) == 0)
    {
        orderList(ptr, httpRequest, httpResponse);
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "unkown query type");
        ptr->forceClose();
    }
}

/*
 *实际逻辑处理函数
 */

//登陆
//每次登陆都重新更新token
void HttpSession::doLogin(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                          HttpResponse& httpResponse)
{
    if(!httpRequest.queryArgumentsExists("token"))
    {
        if(httpRequest.queryArgumentsExists("email") && httpRequest.queryArgumentsExists("pwd"))
        {
            int uid = dbManager_->checkUserAccount(httpRequest.getQueryARgumentsWithDecode("email"), httpRequest.getQueryArguments("pwd"));
            RapidJson json;
            if(uid > 0)
            {
                json.setObject("status", 1);
                json.setObject("uid", uid);
                std::string uuid(std::move(getUuid()));
                auto& redispool = Singletion<RedisConnectionPool>::instance();
                auto redisconn = redispool.getConnection();
                redisconn->selectTable(0);
                if(httpServer_->existUser(uid))//已经登陆过
                {
                    auto old = std::move(httpServer_->getUser(uid));//获取旧token
                    redisconn->delValue(old);//删除
                }
                httpServer_->addUser(uid, uuid);//设置新的
                redisconn->setStringValue(uuid, uid);
                redispool.close(redisconn);
                json.setObject("token", uuid.c_str());
                dbManager_->getSubscribeProductionList(json, uid, false);
                std::string json_body(std::move(json.toString()));
                send(ptr, httpRequest, httpResponse, json_body);
            } else
            {
                handleError(ptr, httpRequest, httpResponse, 2, "user account or pwd error");
            }
        } else
        {
            LOG_ERROR<<"doLogin failed " << httpRequest.getQueryArguments("email")<<" "<<httpRequest.getQueryArguments("pwd");
            handleError(ptr, httpRequest, httpResponse, 0, "input value error");
        }
    }else
    {
        //每次token登陆 生成新的token淘汰旧的token
        std::string token(std::move(httpRequest.getQueryArguments("token")));
        auto& redisPool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redisPool.getConnection();
        redisconn->selectTable(0);
        int uid = atoi(redisconn->getStringValue(token).c_str());
        if(uid > 0 && httpServer_->checkUser(uid, token))
        {
			std::string id(std::move(redisconn->getStringValue(token)));
            //生成并保存newtoken
            std::string newtoken(getUuid());
            redisconn->delValue(token);
            int uid = atoi(id.c_str());
            redisconn->setStringValue(newtoken, uid);
            redisPool.close(redisconn);
            RapidJson json;
            httpServer_->addUser(uid, newtoken);
            json.setObject("status", 1);
            json.setObject("token", newtoken.c_str());
            json.setObject("id", id.c_str());
            dbManager_->getSubscribeProductionList(json, uid, false);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            //TODO 客户端重新登陆
            redisPool.close(redisconn);
            LOG_DEBUG << "doLogin client neet to login again token is dead";
            httpServer_->deleteUser(uid);
            handleError(ptr, httpRequest, httpResponse, 5, "need to re login");
        }
    }
}

void HttpSession::doLogout(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                           HttpResponse &httpresponce) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(0);
        RedisConnectionPool_guard guard(redispool, redisconn);
        std::string token(std::move(httpRequest.getQueryArguments("token")));
        int uid = atoi(redisconn->getStringValue(token).c_str());
        RapidJson json;
        if(redisconn->delValue(token))
        {
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpresponce, json_body);
            LOG_DEBUG << "Logout success";
        }else//token 早已失效
        {
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpresponce, json_body);
            LOG_DEBUG << "Logout success which need login";
        }
        if(uid > 0)
            httpServer_->deleteUser(uid);
    }else
    {
        LOG_DEBUG << "logout without token";
        handleError(ptr, httpRequest, httpresponce, 0, "logout without token");
    }
}

//注册
void HttpSession::doRegist(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                           HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("email") && httpRequest.queryArgumentsExists("pwd") && httpRequest.queryArgumentsExists("name") && httpRequest.queryArgumentsExists("yzm"))
    {
        if(!dbManager_->accountExists(httpRequest.getQueryARgumentsWithDecode("email")))
        {
            auto& redispool = Singletion<RedisConnectionPool>::instance();
            auto redisconn = redispool.getConnection();
            std::string yzm(std::move(redisconn->getStringValue(httpRequest.getQueryARgumentsWithDecode("email"))));
            redispool.close(redisconn);
            if(memcmp(yzm.c_str(), httpRequest.getQueryArguments("yzm").c_str(), yzm.length()) == 0) {
                if (dbManager_->addUserInDB(httpRequest.getQueryARgumentsWithDecode("email"),
                                            httpRequest.getQueryArguments("pwd"),
                                            httpRequest.getQueryARgumentsWithDecode("name"))) {
                    int uid = dbManager_->checkUserAccount(httpRequest.getQueryARgumentsWithDecode("email"),
                                                           httpRequest.getQueryArguments("pwd"));
                    RapidJson json;
                    if (uid > 0) {
                        json.setObject("status", 1);
                        json.setObject("uid", uid);
                        std::string uuid(std::move(getUuid()));
                        httpServer_->addUser(uid, uuid);
                        auto &redispool = Singletion<RedisConnectionPool>::instance();
                        auto redisconn = redispool.getConnection();
                        redisconn->selectTable(0);
                        redisconn->setStringValue(uuid, uid);
                        redispool.close(redisconn);
                        json.setObject("token", uuid.c_str());
                        std::string json_body(std::move(json.toString()));
                        send(ptr, httpRequest, httpResponse, json_body);
                    } else {
                        handleError(ptr, httpRequest, httpResponse, 0, "doRegist insert success but can't get id");
                        LOG_ERROR << "HttpSession::doRegist insert success but can't get id";
                    }
                } else {
                    handleError(ptr, httpRequest, httpResponse, 0, "doRegist insert error");
                    LOG_ERROR << "HttpSession::doRegist insert error";
                }
            } else
            {
                handleError(ptr, httpRequest, httpResponse, 0, "yzm error");
            }
        }else
        {
            LOG_LOG << "HttpSession::doRegist email already have " << httpRequest.getQueryARgumentsWithDecode("email");
            handleError(ptr, httpRequest, httpResponse, 3, "email already have");
        }
    }

}

void HttpSession::doGetIndex(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                             HttpResponse& httpResponse)
{
    RapidJson json;
    if(dbManager_->getProductionByRand(json))
    {
        std::string json_body(std::move(json.toString()));
        send(ptr, httpRequest, httpResponse, json_body);
    }else
    {
        LOG_ERROR << "doIndex failed";
        handleError(ptr, httpRequest, httpResponse, 0, "GetIndex failed");
    }
}

void HttpSession::doRelease(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                            HttpResponse& httpResponse){
    if(httpRequest.queryArgumentsExists("type") && httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("name") && httpRequest.queryArgumentsExists("description")
        && httpRequest.queryArgumentsExists("price") && httpRequest.queryArgumentsExists("cid") && httpRequest.queryArgumentsExists("images") && httpRequest.queryArgumentsExists("address"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(0);
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "token error");
            LOG_ERROR << "HttpSession::doRelease failed token error";
        }
        double price = atof(httpRequest.getQueryArguments("price").c_str());
        if(price < 0.0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild price");
            LOG_ERROR << "HttpSession::doRelease failed price error";
        }
        int cid = atoi(httpRequest.getQueryArguments("cid").c_str());
        if(cid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild cid");
            LOG_ERROR << "HttpSession::doRelease failed cid error";
        }
        int type = atoi(httpRequest.getQueryArguments("type").c_str());
        if(type == 0)
        {
            if(dbManager_->releaseWithoutGuarantee(uid, cid, httpRequest.getQueryARgumentsWithDecode("name"), httpRequest.getQueryARgumentsWithDecode("description"), httpRequest.getQueryARgumentsWithDecode("images"), httpRequest.getQueryARgumentsWithDecode("address"), price))
            {
                RapidJson json;
                json.setObject("status", 1);
                std::string json_body(std::move(json.toString()));
                send(ptr, httpRequest, httpResponse, json_body);
            }else
            {
                handleError(ptr, httpRequest, httpResponse, 0, "releaseWithoutGuarantee failed");
                LOG_ERROR << "HttpSession::doRelease releaseWithoutGuarantee failed";
            }
        }else if(type == 1)
        {
            if(dbManager_->releasewithGuarantee(uid, cid, httpRequest.getQueryARgumentsWithDecode("name"), httpRequest.getQueryARgumentsWithDecode("description"), httpRequest.getQueryARgumentsWithDecode("images"), httpRequest.getQueryARgumentsWithDecode("address"), price))
            {
                RapidJson json;
                json.setObject("status", 1);
                std::string json_body(std::move(json.toString()));
                send(ptr, httpRequest, httpResponse, json_body);
            }else
            {
                handleError(ptr, httpRequest, httpResponse, 0, "releaseWithGuarantee failed");
                LOG_ERROR << "HttpSession::doRelease releaseWithGuarantee failed";
            }
        } else
        {
            LOG_ERROR << "HttpSession::doRelease failed type error " << type;
            handleError(ptr, httpRequest, httpResponse, 0, "type error");
        }

    }else
    {
        LOG_ERROR << "HttpSession::doRelease failed input error";
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
    }

}

void HttpSession::doSearch(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                           HttpResponse& httpResponse)
{
	std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "user", 4) == 0)
    {
        searchAndSendUser(ptr, httpRequest, httpResponse);
    }
    else if(memcmp(type.c_str(), "goods", 5) == 0)
    {
        searchAndSendGoods(ptr, httpRequest, httpResponse);
    }
    else if(memcmp(type.c_str(), "category", 8) == 0)
    {
        searchAndSendCategory(ptr, httpRequest, httpResponse);
    }
    else if(memcmp(type.c_str(), "categories", 10) == 0) {

        searchAndSendCategories(ptr, httpRequest, httpResponse);

    }else if(memcmp(type.c_str(), "goodlist", 8) == 0)
    {
        searchAndSendUserProductions(ptr, httpRequest, httpResponse);
    }
    else{
        LOG_ERROR << "unkown type " << type;
        handleError(ptr, httpRequest, httpResponse, 0, "search type error");
    }
}

//TODO cache
//查找自己或别人的信息
void HttpSession::searchAndSendUser(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                                    HttpResponse& httpResponse)
{
   if(httpRequest.queryArgumentsExists("uid") || httpRequest.queryArgumentsExists("token"))
   {
       auto& redispool = Singletion<RedisConnectionPool>::instance();
       auto redisconn = redispool.getConnection();
	   int uid = 0;
	   if (httpRequest.queryArgumentsExists("token"))//自己的信息
	   {
		   redisconn->selectTable(0);
		   uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
	   }
	   else//别人或自己
	   {
           uid = atoi(httpRequest.getQueryArguments("uid").c_str());
	   }
	   if (uid <= 0)
	   {
			handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
			LOG_ERROR << "HttpSession::searchAndSendUser failed uid <= 0 " << uid;
			return;
	   }
	   redisconn->selectTable(2);
       if(redisconn->existsKey(uid))
       {
           std::string json_body(std::move(redisconn->getStringValue(uid)));
           send(ptr, httpRequest, httpResponse, json_body);
           LOG_DEBUG << "HttpSession::searchAndSendUser by cache";
       } else
       {
           RapidJson json;
           //read from cache
           if(dbManager_->getUserInfoByUid(json, uid))
           {
               std::string json_body(std::move(json.toString()));
               redisconn->setStringValue(uid, json_body);
               send(ptr, httpRequest, httpResponse, json_body);
           }else
           {
               handleError(ptr, httpRequest, httpResponse, 0, "can't get user info");
               LOG_ERROR << "HttpSession::searchAndSendUser failed get from db failed";
           }
       }
       redisconn->selectTable(0);
       redispool.close(redisconn);
   } else
   {
       LOG_ERROR << "searchAndSendUser but uid error " << httpRequest.getQueryArguments("uid");
       handleError(ptr, httpRequest, httpResponse, 0, "no uid or token");
   }
}

void HttpSession::searchAndSendCategories(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                                          HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("codelist"))
    {
        RapidJson json;
        if(dbManager_->getProductionByCid(json, atoi(httpRequest.getQueryArguments("codelist").c_str())))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            LOG_ERROR << "searchAndSendCategories failed no such categories " << httpRequest.getQueryArguments("codelist");
            handleError(ptr, httpRequest, httpResponse, 0, "no such categories");
        }
    }else
    {
        LOG_ERROR << "searchAndSendCategories failed can't get codelist ";
        handleError(ptr, httpRequest, httpResponse, 0, "no codelist");
    }
}

void HttpSession::searchAndSendCategory(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                                        HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("kw"))
    {
        RapidJson json;
        if(dbManager_->getProductionByName(json, httpRequest.getQueryARgumentsWithDecode("kw")))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            LOG_ERROR << "searchAndSendCategory failed no such kw " << httpRequest.getQueryArguments("kw");
            handleError(ptr, httpRequest, httpResponse, 0, "no such kw");
        }
    }else
    {
        LOG_ERROR << "searchAndSendCategory failed can't get kw";
        handleError(ptr, httpRequest, httpResponse, 0, "no kw");
    }
}

//TODO cache
void HttpSession::searchAndSendGoods(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                                     HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("pid"))
    {
        int pid = atoi(httpRequest.getQueryArguments("pid").c_str());
        if(pid > 0)
        {
            auto& redispool = Singletion<RedisConnectionPool>::instance();
            auto redisconn = redispool.getConnection();
            redisconn->selectTable(1);
            if(redisconn->existsKey(pid))
            {
                std::string json_body(std::move(redisconn->getStringValue(pid)));
                send(ptr, httpRequest, httpResponse, json_body);
                LOG_DEBUG << "searchAndSendGoods by cache";
            } else
            {
                RapidJson json;
                if(dbManager_->getProductionByPid(json, pid))
                {
                    std::string json_body(std::move(json.toString()));
                    redisconn->setStringValue(pid, json_body);
                    send(ptr, httpRequest, httpResponse, json_body);
                }else
                {
                    LOG_DEBUG << "searchAndSendGoods failed no such pid";
                    handleError(ptr, httpRequest, httpResponse, 0, "no such pid");
                }
            }
            redisconn->selectTable(0);
            redispool.close(redisconn);
        } else
        {
            LOG_ERROR << "searchAndSendGoods failed pid = 0";
            handleError(ptr, httpRequest, httpResponse, 0, "pid is zero");
        }
    }else
    {
        LOG_ERROR << "searchAndSendGoods failed no pid input";
        handleError(ptr, httpRequest, httpResponse, 0, "no pid input");
    }
}

void HttpSession::searchAndSendUserProductions(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                               HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("uid"))
    {
        RapidJson json;
        if(dbManager_->getUserProduction(json, atoi(httpRequest.getQueryArguments("uid").c_str())))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else{
            LOG_ERROR << "HttpSession::searchAndSendUserProductions no production ";
            handleError(ptr, httpRequest, httpResponse, 0, "no production");
        }
    }else
    {
        LOG_ERROR << "HttpSession::searchAndSendUserProductions input value error " << httpRequest.getQueryArguments("uid");
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
    }


}

void HttpSession::doMod(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                        HttpResponse& httpResponse)
{
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "user", 4) == 0)
        doModUser(ptr, httpRequest, httpResponse);
    else if(memcmp(type.c_str(), "production", 10) == 0)
        doModProduction(ptr, httpRequest, httpResponse);
    else
    {
        LOG_DEBUG << "HttpSession::doMod error type " << httpRequest.getQueryArguments("type");
        handleError(ptr, httpRequest, httpResponse, 0, "error type");
    }
}

//获取用户所有信息并展示出来返回所有信息
//UDF cache
void HttpSession::doModUser(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                            HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("nickname") && httpRequest.queryArgumentsExists("description") && httpRequest.queryArgumentsExists("sex") && httpRequest.queryArgumentsExists("token"))
    {
		auto& redispool = Singletion<RedisConnectionPool>::instance();
		auto redisconn = redispool.getConnection();
		redisconn->selectTable(0);
		int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
		if (uid <= 0)
		{
			handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
			return;
		}
        RapidJson json;
        if(dbManager_->modUserInfoByUid(httpRequest.getQueryARgumentsWithDecode("nickname"), httpRequest.getQueryARgumentsWithDecode("sex"), httpRequest.getQueryARgumentsWithDecode("description"), uid))
        {
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
           handleError(ptr, httpRequest, httpResponse, 0, "mod error");
           LOG_ERROR <<"HttpSession::doModUser failed";
        }
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "HttpSession::doModUser failed input value empty");
        LOG_DEBUG << "HttpSession::doModUser empty value " << httpRequest.getQueryARgumentsWithDecode("nickname") <<
        httpRequest.getQueryARgumentsWithDecode("description") << httpRequest.getQueryARgumentsWithDecode("sex") << httpRequest.getQueryArguments("uid");
    }
}

//同上
//UDF cache
void HttpSession::doModProduction(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                  HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("name") && httpRequest.queryArgumentsExists("description") && httpRequest.queryArgumentsExists("price") && httpRequest.queryArgumentsExists("pid") && httpRequest.queryArgumentsExists("token"))
    {
		//TODO 判断所有权
		auto& redispool = Singletion<RedisConnectionPool>::instance();
		auto redisconn = redispool.getConnection();
		int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
		redispool.close(redisconn);
		if(uid <= 0)
        {
		    handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
		    return;
        }
		int pid = atoi(httpRequest.getQueryArguments("pid").c_str());
		if(!dbManager_->checkProductionUser(pid, uid))
        {
		    handleError(ptr, httpRequest, httpResponse, 0, "invaild action");
		    return;
        }
        RapidJson json;
        if(dbManager_->modProductionByPid(httpRequest.getQueryARgumentsWithDecode("name"), httpRequest.getQueryARgumentsWithDecode("description"), atol(httpRequest.getQueryArguments("price").c_str()), pid))
        {
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "doMod error");
        }
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "HttpSession::doModUser failed input value empty or not your production");
        LOG_DEBUG << "HttpSession::doModProdution empty value " << httpRequest.getQueryARgumentsWithDecode("name") <<
                  httpRequest.getQueryARgumentsWithDecode("description") << httpRequest.getQueryArguments("price") << httpRequest.getQueryArguments("pid");
    }
}

//协议 suid=xxx ruid=xxx message=xxx
//只负责持久化聊天信息 使用UDF将数据存至redis此时为未读数据
//客户端使用循环请求聊天数据
void HttpSession::doChat(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                         HttpResponse& httpResponse)
{
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "get", 3) == 0)
    {
        getChatMessage(ptr, httpRequest, httpResponse);
    }else if(memcmp(type.c_str(), "add", 3) == 0)
    {
        addChatMessage(ptr, httpRequest, httpResponse);
    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
        LOG_DEBUG << "HttpSession::doChat input error " << httpRequest.getQueryArguments("type");
    }
}

//get from redis
void HttpSession::getChatMessage(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                 HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
		redisconn->selectTable(0);
		int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
		if (uid <= 0)
		{
			handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
			return;
		}
		redisconn->selectTable(3);
        int len = redisconn->getListLen(uid);
        RapidJson json;
        if(len > 0)
        {
            json.setObject("status", 1);
            if(len > 8)//大于8时，只返回前8条，同时返回总条数
            {
				std::vector<std::string> message(std::move(redisconn->getListRange(uid, 0, 7)));
				//若count 大于 0 前端应该快速请求
				json.setObject("count", 1);
				for(auto& i : message)
				{
					json.setObjectToArray("message", i.c_str());
					json.flushToArray();
				}
				redisconn->removeListRange(uid, 8, len);//pop items
				json.flushToRoot("data");
            }else//小于8
            {
                std::vector<std::string> message(redisconn->getListRange(uid, 0, len));
                for(auto& i : message)
                {
                    json.setObjectToArray("message", i.c_str());
                    json.flushToArray();
                }
                json.flushToRoot("data");
                //若count为0 应放慢请求次数
                json.setObject("count", 1);
                redisconn->removeListRange(uid, len, len + 1);
            }
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            json.setObject("status", 1);
            json.setObject("count", 0);
			std::string json_body(std::move(json.toString()));
			send(ptr, httpRequest, httpResponse, json_body);
        }
		redisconn->selectTable(0);
		redispool.close(redisconn);
    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
    }
}

//save to mysql
void HttpSession::addChatMessage(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                       HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("ruid") && httpRequest.queryArgumentsExists("message"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(0);
        int suid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(suid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
            return;
        }
        int ruid = atoi(httpRequest.getQueryArguments("ruid").c_str());
        if(ruid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild ruid");
            return;
        }
        if(dbManager_->addChatMessage(suid, ruid, httpRequest.getQueryARgumentsWithDecode("message")))
        {
            RapidJson json;
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "insert error");
            LOG_ERROR << "HttpSession::addChatMessage failed " << suid << " " << ruid << " " << httpRequest.getQueryARgumentsWithDecode("message");
        }
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "input value error");
        LOG_DEBUG << "HttpSession::addChatMessage failed input value error";
    }
}

void HttpSession::doGuss(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                         HttpResponse &httpResponse){

}

void HttpSession::doLike(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                         HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("pid") && httpRequest.queryArgumentsExists("token"))
    {
        if(dbManager_->addLikeCount(atoi(httpRequest.getQueryArguments("pid").c_str())))
        {
            RapidJson json;
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "update failed");
            LOG_ERROR << "HttpSession::doLike failed";
        }
    } else
    {
        LOG_ERROR << "HttpSession::doLike failed input value error";
        handleError(ptr, httpRequest, httpResponse, 0, "input error or not login");
    }
}


void HttpSession::doProductionMessage(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                      HttpResponse &httpResponse) {
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "get", 3) == 0)
        getProductionMessage(ptr, httpRequest, httpResponse);
    else if(memcmp(type.c_str(), "add", 3) == 0)
        addProductionMessage(ptr, httpRequest, httpResponse);
    else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "type error");
        LOG_ERROR << "HttpSession::doProductionMessage failed unkown type " << type;
    }
}

void HttpSession::getProductionMessage(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                         HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("pid"))
    {
        RapidJson json;
        if(dbManager_->getProductionMessageByPid(json, atoi(httpRequest.getQueryArguments("pid").c_str())))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);

        }else
        {
            LOG_DEBUG << "HttpSession::doGetProductionMessage no message";
            handleError(ptr, httpRequest, httpResponse, 0, "no message");
        }
    } else
    {
        LOG_DEBUG << "HttpSession::doGetProductionMessage input value error ";
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
    }
}

void HttpSession::addProductionMessage(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                       HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("pid") && httpRequest.queryArgumentsExists("message"))
    {
        RapidJson json;
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
            return;
        }
        if(dbManager_->addProductionMessageByPid(uid, atoi(httpRequest.getQueryArguments("pid").c_str()), httpRequest.getQueryARgumentsWithDecode("message")))
        {
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "addmessage failed");
        }
    }else
    {
        LOG_DEBUG << "HttpSession::addProductionMessage failed input error";
        handleError(ptr, httpRequest, httpResponse, 0, "input value error or not login");
    }
}

void HttpSession::doSubscribe(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                              HttpResponse &httpResponse) {
    std::string action(std::move(httpRequest.getQueryArguments("action")));
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "user", 4) == 0)
    {
        if(memcmp(action.c_str(), "get", 3) == 0)
            getSubscribeUserList(ptr, httpRequest, httpResponse);
        else if(memcmp(action.c_str(), "add", 3) == 0)
            addSubscribeUserList(ptr, httpRequest, httpResponse);
        else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild action");
            LOG_ERROR << "HttpSession::doSubscribe failed " << action;
        }

    }else if(memcmp(type.c_str(), "production", 10) == 0)
    {
        if(memcmp(action.c_str(), "get", 3) == 0)
            getSubscribeProductionList(ptr, httpRequest, httpResponse);
        else if(memcmp(action.c_str(), "add", 3) == 0)
            addSubscribeProductionList(ptr, httpRequest, httpResponse);
        else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild action");
            LOG_ERROR << "HttpSession::doSubscribe failed " << action;
        }

    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "unkonw type or action");
        LOG_ERROR << "HttpSession::doSubscribe failed " << type << " " << action;
    }
}

void HttpSession::getSubscribeUserList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                       HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
        }
        RapidJson json;
        if(dbManager_->getSubscribeUserList(json, uid, true))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "get failed");
        }
        redispool.close(redisconn);
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "no token");
        LOG_DEBUG << "HttpSession::getSubscribeUserList failed no token";
    }
}

void HttpSession::addSubscribeUserList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                       HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("uid"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
        }
        int suid = atoi(httpRequest.getQueryArguments("uid").c_str());
        if(suid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild uid");
        }
        if(dbManager_->addSubscribeUserList(uid, suid))
        {
            RapidJson json;
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "insert error");
            LOG_DEBUG << "HttpSession::addSubscribeUserList failed";
        }
    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "invaild value");
        LOG_DEBUG << "HttpSession::addSubscribeUserList failed invaild value";
    }
}

void HttpSession::addSubscribeProductionList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                             HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("pid"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
        }
        int pid = atoi(httpRequest.getQueryArguments("pid").c_str());
        if(pid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild pid");
        }
        RapidJson josn;
        if(dbManager_->addSubscribeProductionList(uid, pid))
        {
            RapidJson json;
            json.setObject("status", 1);
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "insert failed");
            LOG_DEBUG << "HttpSession::addSubscribeProductionList insetrt failed";
        }
    }else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "invaild value");
        LOG_DEBUG << "HttpSession::addSubscribeProductionList failed invaild value";
    }

}

void HttpSession::getSubscribeProductionList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                             HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
        }
        RapidJson json;
        if(dbManager_->getSubscribeProductionList(json, uid, true))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "get error");
            LOG_DEBUG << "HttpSession::getSubscribeProductionList failed get error";
        }
    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "no token");
        LOG_DEBUG << "HttpSession::getSubscribeProductionList failed no token";
    }
}

void HttpSession::doOrder(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                          HttpResponse &httpResponse) {
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "start", 4) == 0)
    {
        startOrder(ptr, httpRequest, httpResponse);
    }else if(memcmp(type.c_str(), "finish", 6) == 0)
    {
        finishOrder(ptr, httpRequest, httpResponse);
    }else
    {
        LOG_ERROR << "HttpSession::doOrder failed unkown type " << type;
        handleError(ptr, httpRequest, httpResponse, 0, "unkown type");
    }
}

void HttpSession::startOrder(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                             HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token") && httpRequest.queryArgumentsExists("pid"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
            return;
        }
        int pid = atoi(httpRequest.getQueryArguments("pid").c_str());
        if(pid <= 0)
        {
            handleError(ptr, httpRequest, httpResponse, 0, "invaild pid");
            return;
        }
        std::string orderlist;
        if(dbManager_->doOrder(uid, pid, orderlist))
        {
            RapidJson json;
            auto& redispool = Singletion<RedisConnectionPool>::instance();
            auto rediconn = redispool.getConnection();
            rediconn->selectTable(5);
            redisconn->setStringValue(orderlist.c_str(), pid, 1200);
            redisconn->selectTable(0);
            redispool.close(redisconn);
            //放入异步任务队列
            httpServer_->pushTask(840, orderlist);
            json.setObject("status", 1);
            json.setObject("orderlist", orderlist.c_str());
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            LOG_ERROR << "HttpSession::doOrder failed";
            handleError(ptr, httpRequest, httpResponse, 0, "not avaliable or system error");
        }

    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
        LOG_ERROR << "HttpSession::doOrder failed input error";
    }
}

void HttpSession::finishOrder(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                              HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("orderlist"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(5);
        std::string order(std::move(httpRequest.getQueryArguments("orderlist")));
        if(redisconn->existsKey(order))
        {
            if(!httpServer_->cancelTask(httpRequest.getQueryArguments("orderlist")))
            {
                LOG_ERROR << "HttpSession::finishOrder cancelTask failed " << httpRequest.getQueryArguments("orderlist");
                handleError(ptr, httpRequest, httpResponse, 0, "cancel failed");
            } else
            {
                LOG_DEBUG << "HttpSession::cancel task success";
                redisconn->delValue(order);
                handleError(ptr, httpRequest, httpResponse, 1, "ok");
            }
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "not avaliable");
        }
        redisconn->selectTable(0);
        redispool.close(redisconn);
    } else
    {
        handleError(ptr, httpRequest, httpResponse, 0, "input error");
    }
}

void HttpSession::orderList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                            HttpResponse &httpResponse) {
    std::string type(std::move(httpRequest.getQueryArguments("type")));
    if(memcmp(type.c_str(), "history", 7) == 0)
    {
        getHistoryOrderList(ptr, httpRequest, httpResponse);
    }else if(memcmp(type.c_str(), "unfinish", 8) == 0)
    {
        getUnfinishOrderList(ptr, httpRequest, httpResponse);
    } else
    {
        LOG_ERROR << "HttpSession::orderList failed unkown type " << type;
        handleError(ptr, httpRequest, httpResponse, 0, "unkown type");
    }
}

void HttpSession::getHistoryOrderList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                      HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(0);
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            LOG_ERROR << "HttpSession::getHistoryOrderList failed invaild token " << httpRequest.getQueryArguments("token");
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
            return;
        }
        RapidJson json;
        if(dbManager_->getHistoryOrderList(json, uid))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "get error");
            LOG_ERROR << "HttpSession::getHistoryOrderList failed";
        }

    } else
    {
        LOG_DEBUG << "HttpSession::getHistoryOrderList failed to token input";
        handleError(ptr, httpRequest, httpResponse, 0, "no token");
    }
}

void HttpSession::getUnfinishOrderList(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                                       HttpResponse &httpResponse) {
    if(httpRequest.queryArgumentsExists("token"))
    {
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(0);
        int uid = atoi(redisconn->getStringValue(httpRequest.getQueryArguments("token")).c_str());
        redispool.close(redisconn);
        if(uid <= 0)
        {
            LOG_ERROR << "HttpSession::getUnfinishOrderList failed invaild token " << httpRequest.getQueryArguments("token");
            handleError(ptr, httpRequest, httpResponse, 0, "invaild token");
        }
        RapidJson json;
        if(dbManager_->getUnfinishOrderList(json, uid))
        {
            std::string json_body(std::move(json.toString()));
            send(ptr, httpRequest, httpResponse, json_body);
        } else
        {
            handleError(ptr, httpRequest, httpResponse, 0, "get error");
            LOG_ERROR << "HttpSession::getUnfinishOrderList failed";
        }

    } else
    {
        LOG_DEBUG << "HttpSession::getUnfinishOrderList failed to token input";
        handleError(ptr, httpRequest, httpResponse, 0, "no token");
    }
}

void HttpSession::handleError(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest,
                              HttpResponse httpResponse, int statusCode, const char* debug)
{
    LOG_DEBUG << "in handleError";
    RapidJson json;
    json.setObject("status", statusCode);
    json.setObject("debug", debug);
    std::string json_body(std::move(json.toString()));
    send(ptr, httpRequest, httpResponse, json_body);
}

void HttpSession::send(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                       HttpResponse &httpResponse, const std::string& body){
    httpResponse.setResponseCode(200);
    httpResponse.setContentType("text/json; charset=utf-8");
    httpResponse.addHeader("Access-Control-Allow-Origin", httpRequest.getHeader("Origin"));
    httpResponse.addHeader("Access-Control-Allow-Credentials","true");
    httpResponse.addHeader("Access-Control-Allow-Method", "GET, POST, OPTIONS");
    httpResponse.setBody(body);
    httpResponse.appendToBuffer();
    ptr->send(httpBuffer_.get());
    httpBuffer_->retrieveAll();
}
//#endif