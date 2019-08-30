//
// Created by soft01 on 2019/8/12.
//

#include <iostream>
#include <chrono>

#include "../../json/json.h"
#include "../RapidJson.h"


int main()
{

    /*json.setObject("status", 1);
    json.setObject("code", 13456);
    json.setObject("flag", false);
    json.setObject("price", 123.31);
    json.setObject("url", "http://localhost/default.jpg");
    json.setObjectToArray("name", "alice");
    json.setObjectToArray("is_boy", false);
    json.setObjectToArray("age", 23);
    json.setObjectToArray("hight", 165.23);
    json.flushToArray();
    json.setObjectToArray("name", "tom");
    json.setObjectToArray("is_boy", true);
    json.setObjectToArray("age", 20);
    json.setObjectToArray("hight", 170.21);
    json.flushToArray();
    json.flushToRoot("data");
    std::cout<<json.toString()<<std::endl;*/
    //json.clear();
    auto beg = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 1000; ++i)
    {
        RapidJson json;
        json.setObject("code", 132);
        json.setObject("price", 132.312);
        json.setObject("hello", "我草你妈的");
        json.setObjectToArray("name", "fuck");
        json.setObjectToArray("hight", 132465);
        json.setObjectToArray("double", 13465.654);
        json.flushToArray();
        json.setObjectToArray("name", "you");
        json.setObjectToArray("hight", 654321);
        json.setObjectToArray("double", 654.321);
        json.flushToRoot("code");
        //std::cout<<json.toString()<<std::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count()<<std::endl;

    beg = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 1000; ++i) {
        Json::Value root, data;
        //json.setObject("code", 132);
        root["code"] = 132;
        //json.setObject("price", 132.312);
        root["price"] = 132.312;
        //json.setObject("hello", "我草你妈的");
        root["hello"] = "我草你妈的";
        //json.setObjectToArray("name", "fuck");
        data["name"] = "fuck";
        //json.setObjectToArray("hight", 132465);
        data["hight"] = 123456;
        //json.setObjectToArray("double", 13465.654);
        data["double"] = 13246.321;
        root["data"].append(data);
        //json.flushToArray();
        data["name"] = "you";
        //json.setObjectToArray("hight", 132465);
        data["hight"] = 654321;
        //json.setObjectToArray("double", 13465.654);
        data["double"] = 65412.321;
        root["data"].append(data);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count()<<std::endl;


    beg = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 1000; ++i)
    {
        rapidjson::Document d;
        d.SetObject();
        auto& a = d.GetAllocator();
        rapidjson::Value v(rapidjson::kArrayType);
        rapidjson::Value b(rapidjson::kObjectType);
        rapidjson::Value b1(rapidjson::kObjectType);
        d.AddMember("code", 132, a);
        d.AddMember("price", 132.312, a);
        d.AddMember("hello", "我草你妈的", a);
        b.AddMember("name", "fuck", a);
        b.AddMember("hight", 132465, a);
        b.AddMember("double", 13465.654, a);
        v.PushBack(b, a);
        b1.AddMember("name", "you", a);
        b1.AddMember("hight", 654321, a);
        b1.AddMember("double", 654.321, a);
        v.PushBack(b1, a);
        d.AddMember("data", v, a);
        //std::cout<<json.toString()<<std::endl;
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count()<<std::endl;

}
