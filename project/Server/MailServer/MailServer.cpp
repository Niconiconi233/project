//
// Created by soft01 on 2019/8/15.
//

#include <functional>

#include "MailServer.h"
#include "../../net/InetAddress.h"
#include "../../Logging/base/Logging.h"
#include "../../Logging/base/SingLeton.h"
#include "../../net/EventLoop.h"
#include "../RequestParse.h"
#include "../RedisConnectionPool.h"

namespace
{
    std::pair<int, std::string> arr[] = {
            std::make_pair(200, "HTTP/1.1 200 OK\r\n"),
            std::make_pair(400, "HTTP/1.1 400 Bad Request\r\n"),
            std::make_pair(403, "HTTP/1.1 403 Forbidden\r\n"),
            std::make_pair(404, "HTTP/1.1 404 Not Found\r\n"),
            std::make_pair(500, "HTTP/1.1 500 Internal Server Error\r\n"),
            std::make_pair(503, "HTTP/1.1 503 Server Unavailable\r\n")

    };

    std::string headerConstructor(int code)
    {
        std::string header_buff;
        header_buff += arr[code].second;
        header_buff += "Content-Type: text/html;charset=utf-8\r\n";
        header_buff += "Content-Length: 0\r\n";
        header_buff += "Connection: close\r\n";
        header_buff += "Server: Nico's Web Server\r\n";
        header_buff += "Cache-Control: max-age=0\r\n";
        //header_buff += "Access-Control-Allow-Origin:http://10.1.180.138:8080\r\n";
        //FIXME to ip port
        //header_buff += "Access-Control-Allow-Credentials:true\r\n";
        header_buff += "\r\n";
        return header_buff;
    }

    void close(const std::shared_ptr<TcpConnection>& ptr)
    {
        std::string header(headerConstructor(0));
        ptr->send(header.c_str(), header.length());
    }


}


MailServer::MailServer(const std::string &servaddress, uint16_t port, EventLoop *loop)
    :loop_(loop),
    address_(std::move(servaddress)),
    port_(port),
    parse_(new RequestParse)
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
    //loop_->quit();
    //servPtr_.reset();
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
    std::string header(buffer->peek(), buffer->readableBytes());
    LOG_DEBUG << header;

    if(parse_->parse(header))
    {
        if(parse_->parseOk())
        {
            if(parse_->methodOption())
            {
                string str;
                str += "HTTP/1.1 200 OK\r\n";
                str += "Access-Control-Allow-Headers: content-type\r\n";
                str += "Access-Control-Allow-Origin: *\r\n";
                str += "Access-Control-Allow-Methods: GET,POST,OPTIONS\r\n";
                str += "Access-Control-Allow-Credentials: true\r\n";
                str += "Access-Control-Max-Age: 86400\r\n";
                str += "\r\n";
                //LOG_DEBUG << str;
                ptr->send(str.c_str(), str.length());
            }else
                sendMail(ptr);
        }else
        {
            LOG_ERROR << "Request parse failed";
        }
        buffer->retrieve(header.length());
    }else
    {
        if(parse_->headerSmall())
        {
            LOG_DEBUG << "header not enought";
            return;
        }else if(parse_->headerError())
        {
            LOG_ERROR << "header error " << header;
            ptr->forceClose();
        }else if(parse_->headerBad())
        {
            LOG_ERROR << "header bad " << header;
            ptr->forceClose();

        }
    }
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
    //PyRun_SimpleString("print os.path.abspath('.')");
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

void MailServer::sendMail(const std::shared_ptr<TcpConnection> &ptr)
{
    std::string email(std::move(parse_->getAsStringWithDecode("email")));
    if(email != "")
    {
        auto& pool = Singletion<RedisConnectionPool>::instance();
        auto conn = pool.getConnection();
        conn->selectTable(1);
        if(!conn->existsKey(email))
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
            conn->selectTable(1);
            conn->setValue(email, str, 60);
            conn->selectTable(2);
            conn->setValue(email, str, 3600);
            LOG_DEBUG << "send success to " << email;
            Py_DECREF(pRet);
            Py_DECREF(pRet2);
            close(ptr);
        }
        else
        {
            close(ptr);
        }
        pool.close(conn);
    }else
    {
        LOG_ERROR << "MailServer::sendMail failed can't get email number";
        ptr->forceClose();
    }
}
