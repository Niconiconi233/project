//
// Created by soft01 on 2019/8/1.
//
#include "HttpSession.h"
#include "../net/Buffer.h"
#include "../net/TcpConnection.h"
#include "../Logging/base/Logging.h"
#include "../Logging/base/SingLeton.h"
#include "connectionPool.h"
#include "RedisConnectionPool.h"
#include "RapidJson.h"
#include "HttpResponse.h"
#include "HttpRequest.h"

#include <mysql/jdbc.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

//#ifndef debug
//#define debug

#ifdef debug
#include <chrono>
#endif


namespace
{
    #define MAX_URL_LENGTH 512
    static boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    static const std::string getUuid()
    {
        return boost::uuids::to_string(a_uuid);
    }
}

void HttpSession::onMessage(const std::shared_ptr<TcpConnection> &ptr, Buffer *buffer, Timestamp recvtime)
{
    if(buffer->readableBytes() < 4)
        return;
    if(buffer->readableBytes() > MAX_URL_LENGTH)
    {
        LOG_FATAL << "HttpSession::onMessage failed message to large " << buffer->readableBytes();
        ptr->forceClose();
    }
    httpeContext_->reset();
    httpeContext_->parseRequest(buffer);
    //LOG_DEBUG << buffer->peek();
    if(httpeContext_->gotAll())
    {
        HttpRequest& request = httpeContext_->request();
        recvtime_ = recvtime;
        Buffer sendBuf;
        HttpResponse response(sendBuf, false);
        handleEvents(ptr, request, response);
    } else
    {
        return;
    }
    assert(buffer->readableBytes() == 0);
}

void HttpSession::handleEvents(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(httpRequest.path() == "Login")
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
    else if(httpRequest.path() == "Regist")
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
    else if(httpRequest.path() == "Index")
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
    else if(httpRequest.path() == "Search")
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
    else if(httpRequest.path() == "Release")
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doRelease(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }else if(httpRequest.path() == "Guss")
    {
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        doGuss(ptr, httpRequest, httpResponse);
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doGetIndex speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
    }
    else
        handleError(ptr, httpRequest, httpResponse, 0, "unkown query type");
}

/*
 *实际逻辑处理函数
 */

//登陆
void HttpSession::doLogin(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(!httpRequest.queryArgumentsExists("token"))
    {
        if(httpRequest.queryArgumentsExists("email") && httpRequest.queryArgumentsExists("pwd"))
        {
            ConnectionPool& pool = Singletion<ConnectionPool>::instance();
            auto conn = pool.getConnection();
            std::unique_ptr<sql::PreparedStatement> prep_stmp(conn->prepareStatement("select id from t_user where email=? and pwd=md5(concat(?, ?))"));
            prep_stmp->setString(1, httpRequest.getQueryARgumentsWithDecode("email").c_str());
		    prep_stmp->setString(2, httpRequest.getQueryARgumentsWithDecode("email").c_str());
		    prep_stmp->setString(3, httpRequest.getQueryArguments("pwd").c_str());
#ifdef debug
            auto begin = std::chrono::high_resolution_clock::now();
#endif
            std::shared_ptr<sql::ResultSet> res(prep_stmp->executeQuery());
#ifdef debug
            auto end = std::chrono::high_resolution_clock::now();
            LOG_DEBUG <<"doLogin select sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
            if(res->next())
            {
                RapidJson json;
                std::string uid(getUuid());
                int id = res->getInt("id");
                json.setObject("status", 1);
                json.setObject("token", uid.c_str());
                json.setObject("uid", id);
                auto& redispool = Singletion<RedisConnectionPool>::instance();
                auto redis = redispool.getConnection();
                redis->setStringValue(uid, id);
                redispool.close(redis);
                std::string json_body(json.toString());
                send(ptr, httpRequest, httpResponse, json_body);
            }else//登陆失败
            {
                LOG_LOG << "Login failed ";
                //FIXME logfailed
                handleError(ptr, httpRequest, httpResponse, 0, "username or pwd error");
            }
            pool.close(conn);
        } else
        {
            LOG_ERROR<<"doLogin failed " << httpRequest.getQueryArguments("email")<<" "<<httpRequest.getQueryArguments("pwd");
            handleError(ptr, httpRequest, httpResponse, 0, "input value error");
        }
    }else
    {
        //TODO 万一redis中此token不存在 如何解决
        std::string uid(httpRequest.getQueryArguments("token"));
        auto& redisPool = Singletion<RedisConnectionPool>::instance();
        auto conn = redisPool.getConnection();
        if(conn->existsKey(uid))
        {
            auto id = conn->getStringValue(uid);
            redisPool.close(conn);
            RapidJson json;
            json.setObject("status", 1);
            json.setObject("token", uid.c_str());
            json.setObject("id", id.c_str());
            std::string json_body(json.toString());
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            //TODO 客户端重新登陆
            LOG_DEBUG << "doLogin client neet to login again token is dead";
            handleError(ptr, httpRequest, httpResponse, 0, "need to re login");
        }
    }
}

//注册
void HttpSession::doRegist(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("email") && httpRequest.queryArgumentsExists("pwd"))
    {
        auto& pool = Singletion<ConnectionPool>::instance();
        auto conn = pool.getConnection();
        std::shared_ptr<sql::PreparedStatement> pre_stmp(conn->prepareStatement("select id from t_user where email=?"));
        pre_stmp->setString(1, httpRequest.getQueryARgumentsWithDecode("email"));
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::shared_ptr<sql::ResultSet> res(pre_stmp->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
        LOG_DEBUG <<"doRegist select sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        //判断是否存在email地址 同时可以防止同时的相同请求
        //不存在
        if(!res->next())
        {
            std::unique_ptr<sql::PreparedStatement> pre_stmt2(conn->prepareStatement("insert into t_user (email, pwd) values (?, md5(concat(?, ?)));"));
            pre_stmt2->setString(1, httpRequest.getQueryARgumentsWithDecode("email"));
            pre_stmt2->setString(2, httpRequest.getQueryARgumentsWithDecode("email"));
            pre_stmt2->setString(3, httpRequest.getQueryArguments("pwd"));
            try{
#ifdef debug
                auto begin = std::chrono::high_resolution_clock::now();
#endif
                int ret = pre_stmt2->executeUpdate();//插入
#ifdef debug
                auto end = std::chrono::high_resolution_clock::now();
                LOG_DEBUG <<"doRegist insert sql speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
                if(ret == 1)
                {
                    std::unique_ptr<sql::PreparedStatement> pre_stmt3(conn->prepareStatement("select id from t_user where email=?"));
                    pre_stmt3->setString(1, httpRequest.getQueryARgumentsWithDecode("email"));
#ifdef debug
                    auto begin = std::chrono::high_resolution_clock::now();
#endif
                    std::unique_ptr<sql::ResultSet> res2(pre_stmt3->executeQuery());
#ifdef debug
                    auto end = std::chrono::high_resolution_clock::now();
                    LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
                    //获取id
                    if(res2->next())
                    {
                        RapidJson json;
                        int id = res2->getInt("id");
                        std::string uid(getUuid());
                        json.setObject("status", 1);
                        json.setObject("token", uid.c_str());
                        json.setObject("uid", id);
                        auto& redisPool = Singletion<RedisConnectionPool>::instance();
                        auto redis = redisPool.getConnection();
                        redis->setStringValue(uid, id);
                        redisPool.close(redis);
                        std::string json_body(json.toString());
                        send(ptr, httpRequest, httpResponse, json_body);
                    } else
                    {
                        LOG_FATAL << "doRegist what happened insert success but can't get id";
                    }
                } else
                {
                    LOG_ERROR << "doRegist failed ret returned " << ret;
                    handleError(ptr, httpRequest, httpResponse, 0, "doRegist ret failed");
                }
                pool.close(conn);
            }catch (sql::SQLException& e)
            {
                LOG_FATAL << "doReg catch a exception " << e.what();
                pool.close(conn);
                handleError(ptr, httpRequest, httpResponse, 0, "catch a exp");
            }
        } else
        {
            LOG_LOG << "doReg failed email address already has";
            handleError(ptr, httpRequest, httpResponse, 2, "emaill address already has");
        }
    }else
    {
        LOG_WARN << "doRegist but some value is empty " << httpRequest.getQueryArguments("email")
        <<" "<<httpRequest.getQueryArguments("pwd");
        handleError(ptr, httpRequest, httpResponse, 0, "input value error");
    }

}

void HttpSession::doGetIndex(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    auto& pool = Singletion<ConnectionPool>::instance();
    auto conn = pool.getConnection();
    std::unique_ptr<sql::PreparedStatement> pre_stmt(conn->prepareStatement("select p_id, p_name, p_price, p_wcount, group_concat(p_imageaddr separator ';') as images,\n"
                                                                            "uid, nickname, image\n"
                                                                            "from mydb.production_info\n"
                                                                            "group by p_id\n"
                                                                            "order by RAND()"
                                                                            "limit 8;"));

    std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
    if(res->next())
    {
        RapidJson json;
        json.setObject("status", 1);
        do{
            json.setObjectToArray("pid", res->getInt("p_id"));
            json.setObjectToArray("pname", res->getString("p_name").c_str());
            json.setObjectToArray("pimages", res->getString("images").c_str());
            json.setObjectToArray("watched", res->getInt("p_wcount"));
            json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
            json.setObjectToArray("uid", res->getInt("uid"));
            json.setObjectToArray("nickname", res->getString("nickname").c_str());
            json.setObjectToArray("image", res->getString("image").c_str());
            json.flushToArray();
        }while(res->next());
        json.flushToRoot("data");
        std::string json_body(json.toString());
        send(ptr, httpRequest, httpResponse, json_body);
    }else
    {
        LOG_ERROR << "doIndex failed";
        handleError(ptr, httpRequest, httpResponse, 0, "GetIndex failed");
    }
}

void HttpSession::doRelease(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{

}

void HttpSession::doSearch(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    std::string type = httpRequest.getQueryArguments("type");
    if(type == "user")
    {
        searchAndSendUser(ptr, httpRequest, httpResponse);
    }
    else if(type == "goods")
    {
        searchAndSendGoods(ptr, httpRequest, httpResponse);
    }
    else if(type == "category")
    {
        searchAndSendCategory(ptr, httpRequest, httpResponse);
    }
    else if(type == "categories") {
        searchAndSendCategories(ptr, httpRequest, httpResponse);
    }
    else{
        LOG_ERROR << "unkown type " << type;
        handleError(ptr, httpRequest, httpResponse, 0, "search type error");
    }
}

void HttpSession::searchAndSendUser(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
   if(httpRequest.queryArgumentsExists("uid"))
   {
       auto& pool = Singletion<ConnectionPool>::instance();
       auto conn = pool.getConnection();
       std::shared_ptr<sql::PreparedStatement> pre_stmt(conn->prepareStatement("select nickname, description, sex, image, credit from t_user where id = ?"));
       pre_stmt->setInt(1, atoi(httpRequest.getQueryArguments("uid").c_str()));
#ifdef debug
       auto begin = std::chrono::high_resolution_clock::now();
#endif
       std::shared_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
#ifdef debug
       auto end = std::chrono::high_resolution_clock::now();
       LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
       if(res->next())
       {
           RapidJson json;
           json.setObject("status", 1);
           json.setObject("nickname", res->getString("nickname").c_str());
           json.setObject("description", res->getString("description").c_str());
           json.setObject("sex", res->getString("sex").c_str());
           json.setObject("image", res->getString("image").c_str());
           json.setObject("credit", res->getInt("credit"));
           std::string json_body(json.toString());
           send(ptr, httpRequest, httpResponse, json_body);
       }else
       {
           LOG_ERROR << "searchAndSendUser but no such user";
           handleError(ptr, httpRequest, httpResponse, 0, "no such user");
       }
       pool.close(conn);
   } else
   {
       LOG_ERROR << "searchAndSendUser but uid error " << httpRequest.getQueryArguments("uid");
       handleError(ptr, httpRequest, httpResponse, 0, "uid failed");
   }
}

void HttpSession::searchAndSendCategories(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("codelist"))
    {
        auto& pool = Singletion<ConnectionPool>::instance();
        auto conn = pool.getConnection();
        std::unique_ptr<sql::PreparedStatement> pre_stmt(conn->prepareStatement("select p_id, p_name, p_price, p_wcount, group_concat(p_imageaddr separator ';') as images,\n"
                                                                                "uid, nickname, image\n"
                                                                                "from mydb.production_info\n"
                                                                                "where p_cid = ?\n"
                                                                                "group by p_id\n"
                                                                                "limit 10;"));

        pre_stmt->setInt(1, atoi(httpRequest.getQueryArguments("codelist").c_str()));
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
       LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next())
        {
            RapidJson json;
            json.setObject("status", 1);
            do{
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                //json.setObjectToArray("p_description", res->getString("p_description").c_str());
                json.setObjectToArray("pimages", res->getString("images").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                //json.setObjectToArray("sex", res->getString("sex").c_str());
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                //json.setObjectToArray("description", res->getString("description").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            std::string json_body(json.toString());
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            LOG_ERROR << "searchAndSendCategories failed no such categories " << httpRequest.getQueryArguments("codelist");
            handleError(ptr, httpRequest, httpResponse, 0, "no such categories");
        }
        pool.close(conn);
    }else
    {
        LOG_ERROR << "searchAndSendCategories failed can't get cid ";
        handleError(ptr, httpRequest, httpResponse, 0, "cid failed");
    }
}

void HttpSession::searchAndSendCategory(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("kw"))
    {
        //LOG_DEBUG << httpRequest.getQueryARgumentsWithDecode("kw");
        auto& pool = Singletion<ConnectionPool>::instance();
        auto conn = pool.getConnection();
        std::unique_ptr<sql::PreparedStatement> pre_stmt(conn->prepareStatement("select p_id, p_name, p_price, p_wcount, group_concat(p_imageaddr separator ';') as images,\n"
                                                                                "uid, nickname, image\n"
                                                                                "from mydb.production_info\n"
                                                                                "where p_name like ?\n"
                                                                                "group by p_id\n"
                                                                                "limit 10;"));

        pre_stmt->setString(1, "%" + httpRequest.getQueryARgumentsWithDecode("kw") + "%");
#ifdef debug
        auto begin = std::chrono::high_resolution_clock::now();
#endif
        std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
#ifdef debug
        auto end = std::chrono::high_resolution_clock::now();
       LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
        if(res->next())
        {
            RapidJson json;
            json.setObject("status", 1);
            do{
                json.setObjectToArray("pid", res->getInt("p_id"));
                json.setObjectToArray("pname", res->getString("p_name").c_str());
                //json.setObjectToArray("p_description", res->getString("p_description").c_str());
                json.setObjectToArray("pimages", res->getString("images").c_str());
                json.setObjectToArray("price", static_cast<double>(res->getDouble("p_price")));
                json.setObjectToArray("uid", res->getInt("uid"));
                //json.setObjectToArray("sex", res->getString("sex").c_str());
                json.setObjectToArray("nickname", res->getString("nickname").c_str());
                //json.setObjectToArray("description", res->getString("description").c_str());
                json.setObjectToArray("image", res->getString("image").c_str());
                json.flushToArray();
            }while(res->next());
            json.flushToRoot("data");
            std::string json_body(json.toString());
            send(ptr, httpRequest, httpResponse, json_body);
        }else
        {
            LOG_ERROR << "searchAndSendCategory failed no such kw " << httpRequest.getQueryArguments("kw");
            handleError(ptr, httpRequest, httpResponse, 0, "no such kw");
        }
        pool.close(conn);
    }else
    {
        LOG_ERROR << "searchAndSendCategory failed can't get kw";
        handleError(ptr, httpRequest, httpResponse, 0, "kw failed");
    }
}

void HttpSession::searchAndSendGoods(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
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
                std::string json_body(redisconn->getStringValue(pid));
                send(ptr, httpRequest, httpResponse, json_body);
                redispool.close(redisconn);
                LOG_DEBUG << "searchAndSendGoods by cache";
            } else
            {
                auto &pool = Singletion<ConnectionPool>::instance();
                auto conn = pool.getConnection();
                std::unique_ptr<sql::PreparedStatement> pre_stmt(conn->prepareStatement(
                        "select p_id, p_name, p_description, p_price, p_wcount, group_concat(p_imageaddr separator ';') as images,\n"
                        "uid, sex, nickname, description, image\n"
                        "from mydb.production_info\n"
                        "where p_id = ?\n"
                        "group by p_id"));
                pre_stmt->setInt(1, pid);
#ifdef debug
                auto begin = std::chrono::high_resolution_clock::now();
#endif
                std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());
#ifdef debug
                auto end = std::chrono::high_resolution_clock::now();
                LOG_DEBUG <<"doRegist select sql 2 speed :"<< std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
#endif
                if (res->next()) {
                    RapidJson json;
                    json.setObject("status", 1);
                    json.setObject("pid", res->getInt("p_id"));
                    json.setObject("pname", res->getString("p_name").c_str());
                    json.setObject("p_description", res->getString("p_description").c_str());
                    json.setObject("pimages", res->getString("images").c_str());
                    json.setObject("price", static_cast<double>(res->getDouble("p_price")));
                    json.setObject("uid", res->getInt("uid"));
                    json.setObject("sex", res->getString("sex").c_str());
                    json.setObject("nickname", res->getString("nickname").c_str());
                    json.setObject("description", res->getString("description").c_str());
                    json.setObject("image", res->getString("image").c_str());
                    std::string json_body(json.toString());
                    redisconn->setStringValue(pid, json_body);
                    redisconn->selectTable(0);
                    redispool.close(redisconn);
                    send(ptr, httpRequest, httpResponse, json_body);
                }else
                {
                    LOG_DEBUG << "searchAndSendGoods failed system error";
                    handleError(ptr, httpRequest, httpResponse, 0, "system error");
                }
            }
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

void HttpSession::doMod(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{

}

void HttpSession::doChat(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{

}

void HttpSession::doGuss(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                         HttpResponse &httpResponse){

}

void HttpSession::handleError(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse httpResponse, int statusCode, const char* debug)
{
    LOG_DEBUG << "in handleError";
    RapidJson json;
    json.setObject("status", statusCode);
    json.setObject("debug", debug);
    std::string json_body(json.toString());
    send(ptr, httpRequest, httpResponse, json_body);
}

void HttpSession::send(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                       HttpResponse &httpResponse, std::string& body){
    httpResponse.setResponseCode(200);
    httpResponse.setContentType("text/json; charset=utf-8");
    httpResponse.addHeader("Access-Control-Allow-Origin", "http://10.1.180.138:8080");
    httpResponse.addHeader("Access-Control-Allow-Credentials","true");
    httpResponse.setBody(body);
    httpResponse.appendToBuffer();
    Buffer& buf = httpResponse.getBuffer();
    ptr->send(&buf);
    buf.retrieveAll();
}


//#endif




























