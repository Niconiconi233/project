//
// Created by soft01 on 2019/8/28.
//

#ifndef SERVER_HTTPREQUEST_H
#define SERVER_HTTPREQUEST_H

#include <map>
#include <string>

#include <assert.h>
#include <stdio.h>

#include "Tools.h"

class HttpRequest
{
public:
    enum Method
    {
        kInvaild,kGet,kPost,kHead,kPut,kDelete,kOptions
    };
    enum Version
    {
        kUnkown,kHttp10,kHttp11
    };
    HttpRequest()
        :method_(kInvaild),
        version_(kUnkown)
    {
    }

    HttpRequest(const HttpRequest&) = delete;

    HttpRequest& operator=(const HttpRequest&) = delete;

    void setVersion(Version&& v)
    {
        version_ = std::move(v);
    }

    Version getVersion()
    {
        return version_;
    }

    bool setMethod(const char* start, const char* end)
    {
        assert(method_ == kInvaild);
        std::string m(start, end);
        if(m == "GET")
            method_ = kGet;
        else if(m == "POST")
            method_ = kPost;
        else if(m == "HEAD")
            method_ = kHead;
        else if(m == "PUT")
            method_ = kPut;
        else if(m == "DELETE")
            method_ = kDelete;
        else if(m == "OPTIONS")
            method_ = kOptions;
        else
            method_ = kInvaild;
        return method_ != kInvaild;
    }

    Method method() const
    {
        return method_;
    }

    const char* methodString() const
    {
        const char* result = "UNKOWN";
        switch(method_)
        {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            case kOptions:
                result = "OPTIONS";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        path_.assign(start, end);
    }

    const std::string& path() const
    {
        return path_;
    }

    void setQuery(const char* start, const char* end)
    {
        query_.assign(start, end);
        detailQuery();
    }

    const std::string& query() const
    {
        return query_;
    }

    bool queryArgumentsExists(const std::string& key) const
    {
        auto it = queryArgs_.find(key);
        return it != queryArgs_.end();
    }

    const std::string getQueryArguments(const std::string& key)
    {
        return queryArgs_[key];
    }

    const std::string getQueryARgumentsWithDecode(const std::string& key)
    {
        return decode(queryArgs_[key].c_str());
    }

    void setBody(std::string& body)
    {
        body_.swap(body);
    }

    const std::string& getBody() const
    {
        return body_;
    }



    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        //跳过空格
        while(colon < end && isspace(*colon))
        {
            ++colon;
        }
        std::string value(colon, end);
        //清除末尾空格
        while(!value.empty() && isspace(value[value.size() - 1]))
        {
            value.resize(value.size() - 1);
        }
        headers_[field] = value;
    }

    std::string getHeader(const std::string& field) const
    {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if(it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    const std::map<std::string, std::string>& headers() const
    {
        return headers_;
    }

    void swap(HttpRequest& other)
    {
        std::swap(method_, other.method_);
        std::swap(version_, other.version_);
        path_.swap(other.path_);
        query_.swap(other.query_);
        headers_.swap(other.headers_);
        queryArgs_.swap(other.queryArgs_);
    }
private:
    void detailQuery()
    {
        std::string tmp(query_);
        auto list = splice(tmp, "&");
        for(auto& i : list)
        {
            size_t idx = i.find("=");
            if(idx != std::string::npos)
            {
                queryArgs_.insert(std::make_pair(std::string(i, 0, idx), std::string(i, idx + 1, i.length() - (idx + 1))));
            }
        }
    }

    std::string decode(const char *src) const
    {
        int code;
        char buf[512] = {0};
        char* dest = buf;
        for(; *src != 0; src++, dest++) {
            if (*src == '%') {
                if (sscanf(src + 1, "%2x", &code) != 1)
                    LOG_ERROR << "failed in unencode";
                *dest = code;
                src += 2;
            } else
            {
                *dest = *src;
            }
        }
        return buf;
    }


private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    std::string body_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> queryArgs_;
};

#endif //SERVER_HTTPREQUEST_H
