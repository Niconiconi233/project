//
// Created by soft01 on 2019/8/1.
//

#include "../RequestParse.h"

#include <iostream>

int main()
{
    std::string str{"GET /Search?username=dingruirbq&pwd=132465798&email=110@qq.com HTTP/1.1\r\n"
                    "Host: www.baidu.com\r\n"
                    "Connection: keep-alive\r\n"
                    "Upgrade-Insecure-Requests: 1\r\n"
                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
                    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                    "Referer: https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=c%2B%2B+string+to+int&oq=c%252B%252B%2520%25E5%259C%25A8%25E7%25BA%25BF%2520-baijiahao&rsv_pq=8100acd400017e10&rsv_t=6ae1acKLbHPoaW4R8rEph%2FJKeT5iVQ5Sem7JmYhfWQfINXhyT7v%2FVkQBItk&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=15252\r\n"
                    "Accept-Encoding: gzip, deflate, sdch, br\r\n"
                    "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"};

    RequestParse p;
    p.parse(str);
    if(p.parseOk())
    {
        std::cout<<p.queryMethod()<<std::endl;
        std::cout<<p.queryType()<<std::endl;
        std::cout<<p.httpVersion()<<std::endl;

        std::cout<<p.getAsString("username")<<std::endl;
        std::cout<<p.getAsString("pwd")<<std::endl;
        std::cout<<p.getAsString("email")<<std::endl;

        std::cout<<p.getHeader("User-Agent")<<std::endl;
    }else
        std::cout<<"parse error"<<std::endl;

    std::string str1{"POST /Login HTTP/1.1\r\n"
                     "Host: blog.csdn.net\r\n"
                     "Connection: keep-alive\r\n"
                     "Cache-Control: max-age=0\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
                     "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
                     "Referer: https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&ch=&tn=baidu&bar=&wd=http+post%E6%95%B0%E6%8D%AE%E6%A0%B7%E5%BC%8F&oq=%25E7%25BF%25BB%25E8%25AF%2591%2520-baijiahao&rsv_pq=f86e98e700164388&rsv_t=0ff3WgWzzGbY%2BHCbW6R9IOydKV4ZYQFAr7Pg9DGumgJV%2BPltRLTOL5P2%2BJM&rqlang=cn&rsv_enter=1&rsv_dl=tb&inputT=15894\r\n"
                     "Accept-Encoding: gzip, deflate, sdch, br\r\n"
                     "Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"
                     "username=fdsfsdf&pwd=jflskfjklsdjf&price=132.321"};

    if(p.parse(str1)) {
        if (p.parseOk()) {
            std::cout << p.queryMethod() << std::endl;
            std::cout << p.queryType() << std::endl;
            std::cout << p.httpVersion() << std::endl;

            std::cout << p.getAsString("username") << std::endl;
            std::cout << p.getAsString("pwd") << std::endl;
            //std::cout << p.getAsInt("range") << std::endl;
            std::cout << p.getAsDouble("price") << std::endl;
            //std::cout<<p.getAsString("email")<<std::endl;

            std::cout << p.getHeader("User-Agent") << std::endl;
        } else
            std::cout << "parse error" << std::endl;
    }else
        std::cout<<"not engought"<<std::endl;





}

