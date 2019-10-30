#include "HttpSession.h"

#include "../Logging/base/Logging.h"
#include "../net/Buffer.h"
#include "../Server/Tools.h"
#include "../net/File.h"
#include "../zlib1.2.11/zlib.h"//gzip

#include <sys/sendfile.h>//sendfile
//#include <sstream>
#include <cstring>//strcmp
#include <string>//string
#include <cassert>//assert

#define MAX_URL_LENGTH 2048

std::pair<int, std::string> arr[] = {
    std::make_pair(200, "HTTP/1.1 200 OK\r\n"),
    std::make_pair(400, "HTTP/1.1 400 Bad Request\r\n"),
    std::make_pair(403, "HTTP/1.1 403 Forbidden\r\n"),
    std::make_pair(404, "HTTP/1.1 404 Not Found\r\n"),
    std::make_pair(500, "HTTP/1.1 500 Internal Server Error\r\n"),
    std::make_pair(503, "HTTP/1.1 503 Server Unavailable\r\n")

};

std::unordered_map<int, std::string> HttpSession::state(arr, arr + sizeof(arr)/sizeof(arr[0]));

std::pair<std::string, std::string> mimearr[] = {
    std::make_pair(".html", "text/html;charset=utf-8"),
    std::make_pair(".avi", "video/x-msvideo"),
    std::make_pair(".bmp", "image/bmp"),
    std::make_pair(".c", "text/plain"),
    std::make_pair(".doc", "application/msword"),
    std::make_pair(".gif", "image/gif"),
    std::make_pair(".gz", "application/x-gzip"),
    std::make_pair(".ico", "image/x-icon"),
    std::make_pair(".jpg", "image/jpeg"),
    std::make_pair(".png", "image/jpeg"),
    std::make_pair(".gif", "image/gif"),
    std::make_pair(".txt", "text/plain"),
    std::make_pair(".mp3", "audio/mp3"),
    std::make_pair(".js", "application/javascript"),
    std::make_pair(".css", "text/css"),
    std::make_pair(".default", "text/html;charset=utf-8"),
};

std::unordered_map<std::string, std::string> HttpSession::MimeType_(mimearr, mimearr + sizeof(mimearr)/sizeof(mimearr[0]));

std::string HttpSession::getMime(const std::string mime)
{
    std::unordered_map<std::string, std::string>::const_iterator it = MimeType_.find(mime);
    if(it == MimeType_.end())
        return MimeType_[".default"];
    else
    {
        return it->second;
    }
    
}

//检查指针是否有效

HttpSession::HttpSession(const TcpConnectionPtr& ptr, int sessionid)
    :sessionPtr_(ptr),
     sessionId_(sessionid),
     version_(HTTP_11),
     method_(HTTP_INIT),
     headState_(HEADER_INIT),
     enableGzip_(false)
{

}

HttpSession::~HttpSession()
{

}

void HttpSession::onMessage(const TcpConnectionPtr& ptr, Buffer* buffer)
{
    //LOG_DEBUG << buffer->readableBytes();
    //数据量不够
    if(buffer->readableBytes() < 4)
        return;
    std::string str(buffer->peek(), buffer->readableBytes());
    LOG_DEBUG<<str;
    std::string pos = str.substr(str.length() - 4);
    if(pos != "\r\n\r\n")
        return;
    if(str.length() > MAX_URL_LENGTH)
        return;
    parse(str);
    buffer->retrieveAll();
}

void HttpSession::parse(std::string& str)
{
    if(parseHeader(str))
    {
        if(headState_ == HEADER_OK)
        {
            //log
            doResponce();
        }
    }else
    {
        if(headState_ == HEADER_BAD)
            sendError(REQUEST_FORBIDDEN);
    }
}

void HttpSession::doResponce()
{
    assert(headState_ == HEADER_OK);
    std::string uri("html");
    uri += requests_["URI"];
    size_t pos = uri.find_last_of(".");
    if(pos == std::string::npos)
    {
        sendError(REQUEST_BAD);
        return;
    }
    std::string mime(uri, pos , uri.length() - pos);
    //LOG_DEBUG << uri << "------" << mime;
    if(enableGzip_)
    {
        if(strcmp(mime.c_str() , ".html") == 0 || strcmp(mime.c_str(), ".css") == 0 || strcmp(mime.c_str(), ".js") == 0)
        {
            std::string gzip(uri);
            gzip += ".gz";
            if(!sendPage(gzip, mime, true))//当前没有.gz文件案
            {
                if(makeGzip(uri))//可能会出错
                    sendPage(gzip, mime, true);
                else 
                    sendError(REQUEST_BAD);
            }
        }//不压缩的文件
        else
        {
            if(!sendPage(uri, mime, false))
                sendError(REQUEST_NOTFOUNT);
        }
    }
    else
    {
        if(!sendPage(uri, mime, false))
            sendError(REQUEST_NOTFOUNT);
    }
}

void HttpSession::sendError(RequestState rs)
{
    TcpConnectionPtr ptr = sessionPtr_.lock();
    if(ptr != nullptr){
        char send_buff[4096];
        std::string body_buff, header_buff;
        body_buff += "<html><title>出错了</title>";
        body_buff += "<body bgcolor=\"ffffff\">";
        body_buff += std::to_string(rs);
        body_buff += "<hr><em> Nico's Web Server</em>\n</body></html>";

        header_buff += state[rs];
        header_buff += "Content-Type: text/html;charset=utf-8\r\n";
        header_buff += "Connection: Close\r\n";
        header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
        header_buff += "Server: Nico's Web Server\r\n";;
        header_buff += "\r\n";
        // 错误处理不考虑writen不完的情况
        sprintf(send_buff, "%s", header_buff.c_str());
        //ptr->Send(send_buff, strlen(send_buff));
        //ptr->send(send_buff, strlen(send_buff));
        ptr->send(send_buff, strlen(send_buff));
        sprintf(send_buff, "%s", body_buff.c_str());
        //ptr->send(send_buff, strlen(send_buff));
        ptr->send(send_buff, strlen(send_buff));
    }
}

std::string HttpSession::headerConstructor(RequestState st, const std::string mimeType, int length, bool gzip)
{
    std::string header_buff;
    header_buff += state[st];
    header_buff += "Content-Type: "+ getMime(mimeType) + "\r\n";
    if(gzip)
    {
        header_buff += "content-encoding:gzip\r\n";
    }
    header_buff += "Content-Length: " + std::to_string(length) + "\r\n";
    header_buff += "Server: Nico's Web Server\r\n";;
    header_buff += "\r\n";
    return header_buff;
}

bool HttpSession::parseHeader(std::string& header)
{
    requests_.clear();
    std::vector<std::string> lists = splice(header, "\r\n");//分行
    std::vector<std::string> line = splice(lists[0], " ");//分解GET / HTTP/1.1
    //todo 解析问号后的请求
    if(strcmp(line[0].c_str(), "GET") == 0)
    {
        method_ = HTTP_GET;
    }
    else if (strcmp(line[0].c_str(), "POST") == 0)
    {
        method_ = HTTP_POST;
    }
    else if (strcmp(line[0].c_str(), "HEAD") == 0)
    {
        method_ = HTTP_HEAD;
    }else
    {
        headState_ = HEADER_BAD;
        return false;
    }
    
    //暂时写到head

    if(strcmp(line[2].c_str(), "HTTP/1.1") == 0)
    {
        version_ = HTTP_11;
    }
    else if (strcmp(line[2].c_str(), "HTTP/1.0") == 0)
    {
        version_ = HTTP_10;
    }
    else
    {
        headState_ = HEADER_BAD;
        return false;
    }
    if(line[1] == "/")//如果是直接访问ip和端口
    {
        requests_["URI"] = "/index.html";
    }
    else
    {
        requests_["URI"] = line[1];
    }
    for(size_t i = 1; i < lists.size(); ++i)
    {
        size_t pos = lists[i].find(":");
        if(pos == std::string::npos)
            continue;
        requests_.insert(std::make_pair(std::string(lists[i], 0, pos), std::string(lists[i], pos + 1, lists[i].length() - (pos + 1))));
    }
    headState_ = HEADER_OK;
    return true;
}

bool HttpSession::makeGzip(const std::string& name)
{
    char buf[1024 * 64];
    int fd = ::open(name.c_str(), O_RDONLY);
    if(fd < 0)
    {
        LOG_ERROR << "HttpSession::makeGzip open failed fd = " << fd;
        return false;
    }
    int n = ::read(fd, buf, sizeof buf);
    if(n <= 0)
    {
        LOG_ERROR << "HttpSession::makeGzip read failed n = " << n;
        return false;
    }
    gzFile file = gzopen((name + ".gz").c_str(), "wb");
    if(file == nullptr)
    {
        LOG_ERROR << "HttpSession::makeGzip gzopen failed";
        return false;
    }
    n = gzwrite(file, buf, n);
    if(n <= 0)
    {
        LOG_ERROR << "HttpSession::makeGzip gzwrite failed n = " << n;
        return false;
    }
    gzclose(file);
    return true;
}


bool HttpSession::sendPage(std::string& pagename, std::string& mime, bool gzip)
{
    File file(pagename);
    if(file.ensure())
    {
        if(file.can_read() && file.is_file())
        {
            std::string header = headerConstructor(REQUEST_OK, mime, file.getFileSize(), gzip);
            TcpConnectionPtr ptr = sessionPtr_.lock();
            if(ptr)
            {
                std::string data = file.get_file_data();
                ptr->send(header);
                ptr->send(data);
                //sendfile(ptr->getFd(), file.get_fd(), 0, file.getFileSize());
            }
        }
        else
        {
            LOG_ERROR << "HttpSession::sendPage file can't be read";
            sendError(REQUEST_FORBIDDEN);
        } 
        return true; 
    }
    //没有文件先返回
    return false;
}




