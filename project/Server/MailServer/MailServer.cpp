//
// Created by soft01 on 2019/8/15.
//

#include <functional>

#include "MailServer.h"
#include "../../net/InetAddress.h"
#include "../../Logging/base/Logging.h"
#include "../../Logging/base/SingLeton.h"
#include "../../net/EventLoop.h"
#include "../HttpContext.h"
#include "../HttpRequest.h"
#include "../HttpResponse.h"
#include "../RedisConnectionPool.h"


MailServer::MailServer(const std::string &servaddress, uint16_t port, EventLoop *loop)
    :loop_(loop),
    address_(std::move(servaddress)),
    port_(port),
    httpContext_(new HttpContext),
    buffer_(new Buffer)
{
    InetAddress servaddr(address_, port);
    servPtr_.reset(new TcpServer(loop_, servaddr, "mailserver"));
    servPtr_->setConnectionCallback(std::bind(&MailServer::onConnection, this, std::placeholders::_1));
    pythonModeInit();
    servPtr_->start();
    LOG_DEBUG << "Mailserver started";
}

MailServer::~MailServer()
{
    Py_DECREF(pModule);
    Py_DECREF(pDict);
    Py_DECREF(pClass);
    Py_DECREF(pMail);
    Py_Finalize();
}

void MailServer::onConnection(const std::shared_ptr<TcpConnection> &ptr)
{
    if(ptr->connected())
    {
        ptr->setMessageCallback(std::bind(&MailServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }
}

void MailServer::onMessage(const std::shared_ptr<TcpConnection>& ptr, Buffer *buffer, Timestamp time)
{
    if(buffer->readableBytes() < 4)
        return ;
    httpContext_->reset();
    httpContext_->parseRequest(buffer);
    if(httpContext_->gotAll())
    {
        HttpRequest& request = httpContext_->request();
        HttpResponse response(buffer_.get(), false);
        sendMail(ptr, request, response);
    } else
    {
        return;
    }
    assert(buffer->readableBytes() == 0);
}

void MailServer::pythonModeInit()
{
    Py_Initialize();
    if(!Py_IsInitialized())
    {
        LOG_FATAL << "py_initialize failed";
        return;
    }
    PyRun_SimpleString("import sys, os");
    PyRun_SimpleString("sys.path.append('./')");
    pModule = PyImport_ImportModule("mail");
    if(!pModule)
    {
        LOG_FATAL << "can't find mail.py";
        return ;
    }
    pDict = PyModule_GetDict(pModule);
    if(!pDict)
    {
        LOG_FATAL << "pymodule_getdict failed";
        return ;
    }
    pClass = PyDict_GetItemString(pDict, "mail");
    if(!pClass)
    {
        LOG_FATAL << "cant't find mail";
        return ;
    }
    pMail = PyInstance_New(pClass, NULL, NULL);
    if(!pMail)
    {
        LOG_FATAL << "can't instance";
        return;
    }
}

void MailServer::sendMail(const std::shared_ptr<TcpConnection> &ptr, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
    if(httpRequest.queryArgumentsExists("email"))
    {
        std::string email(std::move(httpRequest.getQueryARgumentsWithDecode("email")));
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(4);
        if(!redisconn->existsKey(email))
        {
            pRet = PyObject_CallMethod(pMail, "get_code", NULL);
            if(!pRet)
            {
                LOG_FATAL << "cant't find get_code";
                return;
            }
            char* str;
            PyArg_Parse(pRet, "s", &str);
            pRet2 = PyObject_CallMethod(pMail, "send", "s,s", str, email.c_str());
            if(!pRet2)
            {
                LOG_FATAL << "can't find send";
                Py_DECREF(pRet);
                return;
            }
            redisconn->setStringValue(email.c_str(), str, 60);
            redisconn->selectTable(0);
            redispool.close(redisconn);
            LOG_DEBUG << "send success to " << email;
            Py_DECREF(pRet);
            Py_DECREF(pRet2);
            std::string json_body("{\"status\":1}");
            send(ptr, httpRequest, httpResponse, json_body);
        }
        else
        {
            redisconn->selectTable(0);
            redispool.close(redisconn);
            std::string json_body("{\"status\":1}");
            send(ptr, httpRequest, httpResponse, json_body);
        }
    }else
    {
        LOG_ERROR << "MailServer::sendMail failed can't get email number";
        ptr->forceClose();
    }
}

void MailServer::send(const std::shared_ptr<TcpConnection> &ptr, HttpRequest &httpRequest,
                       HttpResponse &httpResponse, std::string& body){
    httpResponse.setResponseCode(200);
    httpResponse.setContentType("text/json; charset=utf-8");
    httpResponse.addHeader("Access-Control-Allow-Origin", httpRequest.getHeader("Origin"));
    httpResponse.addHeader("Access-Control-Allow-Credentials","true");
    httpResponse.addHeader("Access-Control-Allow-Method", "GET,POST");
    httpResponse.setBody(body);
    httpResponse.appendToBuffer();
    ptr->send(buffer_.get());
    buffer_->retrieveAll();
}
