//
// Created by soft01 on 2019/8/28.
//

#include "HttpContext.h"

#include "../net/Buffer.h"

bool HttpContext::processRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if(space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if(space != end)
        {
            const char* question = std::find(start, space, '?');
            if(question != space)
            {
                request_.setPath(start + 1, question);
                request_.setQuery(question + 1, space);
            } else
            {
                request_.setPath(start + 1, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
            if(succeed)
            {
                if(*(end - 1) == '1')
                    request_.setVersion(HttpRequest::kHttp11);
                else if(*(end - 1) == '0')
                    request_.setVersion(HttpRequest::kHttp10);
                else
                    succeed = false;
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer *buf)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore)
    {
        if(state_ == kExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if(ok)
                {
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders;
                }else
                {
                    hasMore = false;
                }

            }else
            {
                hasMore = false;
            }

        }else if(state_ == kExpectHeaders)
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if(colon != crlf)
                {
                    request_.addHeader(buf->peek(), colon, crlf);
                }else
                {
                    //对于post请求 需要获得body
                    if(request_.method() == HttpRequest::kPost)
                    {
                        state_ = kExpectBody;
                    }else
                    {
                        state_ = kGotAll;
                        hasMore = false;
                    }
                }
                buf->retrieveUntil(crlf + 2);
            }else
            {
                hasMore = false;
            }
        }else if(state_ == kExpectBody)//处理post请求
        {
            size_t bodysize = atoi(request_.getHeader("Content-Length").c_str());
            if(buf->readableBytes() < bodysize)
            {
                state_ = kExpectBody;
                hasMore = false;
            }else
            {
                std::string body(buf->peek(), bodysize);
                request_.setBody(body);
                buf->retrieve(bodysize);
                state_ = kGotAll;
                hasMore = false;
            }
        }
    }
    return ok;
}