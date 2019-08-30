//
// Created by soft01 on 2019/8/15.
//

#ifndef SERVER_MAILSERVER_H
#define SERVER_MAILSERVER_H

#include "../../net/Timestamp.h"

#include <memory>
#include <python2.7/Python.h>

#include "../../net/TcpServer.h"
#include "../../net/TcpConnection.h"
#include "../../net/Buffer.h"


class EventLoop;
class RequestParse;

class MailServer {
public:
    MailServer(const std::string& servaddress, uint16_t port, EventLoop* loop);
    ~MailServer();

    MailServer(const MailServer&)=delete;
    MailServer& operator=(const MailServer&)=delete;

    void onConnection(const std::shared_ptr<TcpConnection>&);

    void onMessage(const std::shared_ptr<TcpConnection>&, Buffer* buffer, Timestamp time);

    void sendMail(const std::shared_ptr<TcpConnection>& ptr);

    void pythonModeInit();

private:
    EventLoop* loop_;
    std::string address_;
    uint16_t port_;
    std::unique_ptr<TcpServer> servPtr_;
    std::unique_ptr<RequestParse> parse_;
    PyObject *pModule, *pDict, *pClass, *pMail, *pRet, *pRet2;
};


#endif //SERVER_MAILSERVER_H
