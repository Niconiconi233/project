//
// Created by soft01 on 2019/8/28.
//

#ifndef SERVER_HTTPCONTEXT_H
#define SERVER_HTTPCONTEXT_H

#include "HttpRequest.h"

class Buffer;

class HttpContext {
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
        :state_(kExpectRequestLine)
    {}

    bool parseRequest(Buffer* buf);

    bool gotAll() const
    {
        return state_ == kGotAll;
    }

    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
        //assert(dummy.method() == HttpRequest::kInvaild);
    }

    const HttpRequest& request() const
    {
        return request_;
    }

    HttpRequest& request()
    {
        return request_;
    }

private:
    bool processRequestLine(const char* begin, const char* end);

private:
    HttpRequestParseState state_;
    HttpRequest request_;

};


#endif //SERVER_HTTPCONTEXT_H
