//
// Created by soft01 on 2019/8/29.
//

#include "HttpResponse.h"

#include "../net/Buffer.h"

std::map<int, std::string> HttpResponse::statusCode_{std::make_pair(200, "HTTP/1.1 200 OK\r\n"),
                                                     std::make_pair(400, "HTTP/1.1 400 Bad Request\r\n"),
                                                     std::make_pair(403, "HTTP/1.1 403 Forbidden\r\n"),
                                                     std::make_pair(404, "HTTP/1.1 404 Not Found\r\n"),
                                                     std::make_pair(500, "HTTP/1.1 500 Internal Server Error\r\n"),
                                                     std::make_pair(503, "HTTP/1.1 503 Server Unavailable\r\n")};


void HttpResponse::appendToBuffer() const
{
    char buf[32];

    switch(responceCode_)
    {
        case 200:
            buffer_->append(statusCode_[responceCode_]);
            break;
        case 400:
            buffer_->append(statusCode_[responceCode_]);
            break;
        case 403:
            buffer_->append(statusCode_[responceCode_]);
            break;
        case 404:
            buffer_->append(statusCode_[responceCode_]);
            break;
        case 500:
            buffer_->append(statusCode_[responceCode_]);
            break;
        case 503:
            buffer_->append(statusCode_[responceCode_]);
            break;
        default:
            break;
    }
    if(closeConnection_)
    {
        buffer_->append("Connection: close\r\n");
    }else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
        buffer_->append(buf);
        buffer_->append("Connection: Keep-Alive\r\n");
    }
    for(const auto& header : headers_)
    {
        buffer_->append(header.first);
        buffer_->append(": ");
        buffer_->append(header.second);
        buffer_->append("\r\n");
    }
    buffer_->append("\r\n");
    buffer_->append(body_);
}
