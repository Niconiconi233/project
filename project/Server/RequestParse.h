//
// Created by soft01 on 2019/8/1.
//

#ifndef SERVER_REQUESTPARSE_H
#define SERVER_REQUESTPARSE_H

#include <string>
#include <unordered_map>
#include <vector>


struct serv_str_t;

class RequestParse {
    friend class HttpSession;
public:
    RequestParse()
        :method_(HTTP_INIT),
        headerState_(HEADER_INIT),
        queryType_(TypeInit),
        httpVersion_(HTTP_00),
        headerLen_(0)
    {

    }

    RequestParse(const RequestParse&) = delete;

    RequestParse& operator=(const RequestParse&) = delete;

    bool parse(const std::string& HttpHeader);

    const std::string getAsString(const std::string&& key)
    {
        auto it = queryList_.find(key);
        if(it != queryList_.end())
            return it->second;
        else
            return std::string{};
    }

    const std::string getAsStringWithDecode(const std::string&& key)
    {
        auto it = queryList_.find(key);
        if(it != queryList_.end())
            return unencode(it->second.c_str());
        else
            return std::string{};
    }

    int getAsInt(const std::string&& key)
    {
        auto it = queryList_.find(key);
        if(it != queryList_.end())
            return static_cast<int>(strtol(it->second.c_str(), NULL, 0));
        else
            return -1;
    }

    double getAsDouble(const std::string&& key)
    {
        auto it = queryList_.find(key);
        if(it != queryList_.end())
            return strtod(it->second.c_str(), NULL);
        else
            return 0.0;
    }

    const std::string getHeader(const std::string&& key)
    {
        auto it = headerList_.find(key);
        if(it != headerList_.end())
            return it->second;
        else
            return std::string{};
    }

    bool parseOk() const
    {
        return headerState_ == HEADER_OK;
    }

    bool headerSmall() const
    {
        return headerState_ == HEADER_NOTENOUGHT;
    }

    bool headerBad() const
    {
        return headerState_ == HEADER_BAD;
    }

    bool headerError() const
    {
        return headerState_ == HEADER_ERROR;
    }

    int httpVersion() const
    {
        return httpVersion_;
    }

    int queryType() const
    {
        return queryType_;
    }

    bool methodGet() const
    {
        return method_ == HTTP_GET;
    }

    bool methodPost() const
    {
        return method_ == HTTP_POST;
    }

    bool methodOption() const
    {
        return method_ == HTTP_OPTIONS;
    }


private:
    bool parseGetQuery(serv_str_t& str);

    bool parsePostQuery(serv_str_t& str, serv_str_t& kw);

    bool parseHeader();

    std::string unencode(const char *src) const ;

    void reset();


private:
    std::unordered_map<std::string, std::string> queryList_;
    std::unordered_map<std::string, std::string> headerList_;
    std::string str_;

    enum HttpMethod{
        HTTP_INIT = 0,
        HTTP_GET,
        HTTP_POST,
        HTTP_HEAD,
        HTTP_PUT,
        HTTP_DELETE,
        HTTP_TRACE,
        HTTP_CONNECT,
        HTTP_OPTIONS,
    };
    enum HeaderState{
        HEADER_INIT = 0,
        HEADER_OK,
        HEADER_BAD,
        HEADER_ERROR,
        HEADER_NOTENOUGHT
    };
    enum QueryType
    {
        TypeInit = 0,
        TypeLogin,
        TypeReg,
        TypeRelease,
        TypeSearch,
        TypeRandom,
        TypeMail,
        TypeGuss,
    };
    enum HttpVersion{
        HTTP_00 = 0,
        HTTP_10,
        HTTP_11,
    };

    HttpMethod method_;
    HeaderState headerState_;
    QueryType queryType_;
    HttpVersion httpVersion_;
    size_t headerLen_;

};


#endif //SERVER_REQUESTPARSE_H
