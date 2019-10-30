#ifndef FILESERV_HTTPSESSION_H
#define FILESERV_HTTPSESSION_H

#include <unordered_map>
#include <memory>

#include "../net/TcpConnection.h"

class TcpConnection;
class Buffer;

enum RequestState{
    REQUEST_OK = 200,
    REQUEST_BAD = 400,
    REQUEST_FORBIDDEN = 403,
    REQUEST_NOTFOUNT = 404,
    REQUEST_SERVERERROR = 500,
    REQUEST_SERVERUNAVA = 503,
};

enum HttpVersion{
    HTTP_10,
    HTTP_11,
};

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
    HEADER_ERROR
};



class HttpSession
{
    using sessionCloseCallback = std::function<void (int)>;
public: 
    HttpSession(const TcpConnectionPtr& connptr, int id);
    ~HttpSession();

    HttpSession(const HttpSession&) = delete;
    HttpSession& operator=(const HttpSession&) = delete;

    void onMessage(const TcpConnectionPtr& connptr, Buffer* buff);
    void setEnableGzip(bool on)
    {
        enableGzip_ = on;
    }

private: 
    void sendError(RequestState rs);
    void parse(std::string& str);
    void doResponce();
    bool parseHeader(std::string& header);
    std::string headerConstructor(RequestState st, const std::string mimeType, int length, bool gzip);
    bool makeGzip(const std::string& name);
    bool sendPage(std::string& pagename, std::string& mime, bool gzip);

    static std::string getMime(const std::string mime);

private: 
    std::weak_ptr<TcpConnection> sessionPtr_;
    int sessionId_;
    HttpVersion version_;
    HttpMethod method_;
    HeaderState headState_;
    bool enableGzip_;
    std::unordered_map<std::string, std::string> requests_;//保存当前请求相关数据

    static std::unordered_map<int, std::string> state;//保存http响应号码对应的字符串
    static std::unordered_map<std::string, std::string> MimeType_;//Mime编码 应该调用getmime来获取
};

#endif // !FILESERV_HTTPSESSION_H
