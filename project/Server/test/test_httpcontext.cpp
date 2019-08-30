//
// Created by soft01 on 2019/8/28.
//
#include <iostream>

#include "../HttpContext.h"
#include "../HttpRequest.h"
#include "../HttpResponse.h"
#include "../../net/Buffer.h"


int main()
{
    std::string str("GET /s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=c%2B%2B+%E5%9C%A8%E7%BA%BF+-baijiahao&oq=iris%2520-baijiahao&rsv_pq=bc2c035c00529a6e&rsv_t=90acH1N7S4LZIkgOkqmxAQKaHEkNhp7yOH3U9rPWtllu1y%2BfCzjOsJICYqs&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=6191 HTTP/1.1\r\n"
                    "Host: www.baidu.com\r\n"
                    "Connection: keep-alive\r\n"
                    "Upgrade-Insecure-Requests: 1\r\n"
                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
                    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                    "Referer: https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=c%2B%2B+%E5%9C%A8%E7%BA%BF&oq=iris%2520-baijiahao&rsv_pq=bc2c035c00529a6e&rsv_t=90acH1N7S4LZIkgOkqmxAQKaHEkNhp7yOH3U9rPWtllu1y%2BfCzjOsJICYqs&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=6191\r\n"
                    "Accept-Encoding: gzip, deflate, sdch, br\r\n"
                    "Accept-Language: zh-CN,zh;q=0.8\r\n"
                    "Cookie: BAIDUID=8FF3E5575CE32353EBA990A21CDA0213:FG=1; BIDUPSID=8FF3E5575CE32353EBA990A21CDA0213; PSTM=1560927898; sugstore=0; COOKIE_SESSION=1121518_0_4_3_10_13_1_0_3_4_16_0_1121520_0_5_0_1566956033_0_1566956028%7C9%230_0_1566956028%7C1; BD_UPN=123353; delPer=0; BD_CK_SAM=1; PSINO=6; H_PS_PSSID=1434_21095_18559_29523_29521_29098_29567_29220_26350_29461; ORIGIN=1; ISSW=1; ISSW=1\r\n\r\n");
    Buffer buf;
    buf.append(str.c_str(), str.length());
    HttpContext context;
    context.parseRequest(&buf);
    if(context.gotAll())
    {
        HttpRequest& request = context.request();
        std::cout<<request.methodString()<<std::endl;
        std::cout<<request.query()<<std::endl;
        std::cout<<request.path()<<std::endl;
        std::cout<<request.getHeader("Host")<<std::endl;
        std::cout<<request.getHeader("Cookie")<<std::endl;
        std::cout<<request.getHeader("Accept-Language")<<std::endl;
        std::cout<<std::boolalpha<<(request.getVersion() == HttpRequest::kHttp11)<<std::noboolalpha<<std::endl;
        std::cout<<"------------args-----------------"<<std::endl;
        std::cout<<request.getQueryArguments("ie")<<std::endl;
        std::cout<<request.getQueryArguments("tn")<<std::endl;
        std::cout<<request.getQueryArguments("wd")<<std::endl;
        std::cout<<request.getQueryARgumentsWithDecode("wd")<<std::endl;
    }
    std::cout<<buf.readableBytes()<<std::endl;
    std::string str2("GET /cplusplus/cpp-tutorial.html HTTP/1.1\r\n"
                     "Host: www.runoob.com\r\n"
                     "Connection: keep-alive\r\n"
                     "Cache-Control: max-age=0\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
                     "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                     "Referer: https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=c%2B%2B+%E5%9C%A8%E7%BA%BF&oq=iris%2520-baijiahao&rsv_pq=bc2c035c00529a6e&rsv_t=90acH1N7S4LZIkgOkqmxAQKaHEkNhp7yOH3U9rPWtllu1y%2BfCzjOsJICYqs&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=6191\r\n"
                     "Accept-Encoding: gzip, deflate, sdch, br\r\n"
                     "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n");
    std::cout<<std::endl;
    buf.append(str2.c_str(), str2.length());
    context.reset();
    context.parseRequest(&buf);
    if(context.gotAll())
    {
        HttpRequest& request = context.request();
        std::cout<<request.methodString()<<std::endl;
        std::cout<<request.query()<<std::endl;
        std::cout<<request.path()<<std::endl;
        std::cout<<request.getHeader("Host")<<std::endl;
        //std::cout<<request.getHeader("Cookie")<<std::endl;
        std::cout<<request.getHeader("Accept-Language")<<std::endl;
        std::cout<<std::boolalpha<<(request.getVersion() == HttpRequest::kHttp11)<<std::noboolalpha<<std::endl;
    }
    assert(buf.readableBytes() == 0);

    std::cout<<"---------------------------------"<<std::endl;
    std::string str3("POST /cplusplus/cpp-tutorial.html HTTP/1.1\r\n"
                     "Host: www.runoob.com\r\n"
                     "Connection: keep-alive\r\n"
                     "Cache-Control: max-age=0\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
                     "Content-Length: 47\r\n"
                     "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                     "Referer: https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=c%2B%2B+%E5%9C%A8%E7%BA%BF&oq=iris%2520-baijiahao&rsv_pq=bc2c035c00529a6e&rsv_t=90acH1N7S4LZIkgOkqmxAQKaHEkNhp7yOH3U9rPWtllu1y%2BfCzjOsJICYqs&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=6191\r\n"
                     "Accept-Encoding: gzip, deflate, sdch, br\r\n"
                     "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"
                     "{nmsl:nmsl;wocaonima:wocaonima;fuckyou:fuckyou}");
    std::cout<<std::endl;
    buf.append(str3.c_str(), str3.length());
    context.reset();
    context.parseRequest(&buf);
    if(context.gotAll())
    {
        HttpRequest& request = context.request();
        std::cout<<request.methodString()<<std::endl;
        std::cout<<request.query()<<std::endl;
        std::cout<<request.path()<<std::endl;
        std::cout<<request.getHeader("Host")<<std::endl;
        //std::cout<<request.getHeader("Cookie")<<std::endl;
        std::cout<<request.getHeader("Accept-Language")<<std::endl;
        std::cout<<std::boolalpha<<(request.getVersion() == HttpRequest::kHttp11)<<std::noboolalpha<<std::endl;
        std::cout<<request.getBody()<<std::endl;
    }



    return 0;
}
