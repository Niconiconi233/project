//
// Created by soft01 on 2019/8/29.
//

#ifndef SERVER_HTTPRESPONSE_H
#define SERVER_HTTPRESPONSE_H
#include <map>

class Buffer;

class HttpResponse {
public:
    explicit HttpResponse(Buffer& buffer, bool close)
        :buffer_(buffer),
        responceCode_(0),
        closeConnection_(close)
    {
    }

    void setResponseCode(int code)
    {
        responceCode_ = code;
    }


    void setCloseConnection(bool on)
    {
        closeConnection_ = on;
    }

    bool closeConnection() const
    {
        return closeConnection_;
    }

    void setContentType(const std::string& content)
    {
        addHeader("Content-Type", content);
    }

    void addHeader(const std::string& key, const std::string& value)
    {
        headers_[key] = value;
    }

    void setBody(const std::string& body)
    {
        body_ = body;
    }

    void appendToBuffer() const;

    Buffer& getBuffer()
    {
        return buffer_;
    }



private:
    std::map<std::string, std::string> headers_;
    Buffer& buffer_;
    int responceCode_;
    //HttpStatusCode statusCode_;
    //fixme add http version
    //std::string statusMessage_;
    bool closeConnection_;
    std::string body_;

    static  std::map<int, std::string> statusCode_;
};


#endif //SERVER_HTTPRESPONSE_H
