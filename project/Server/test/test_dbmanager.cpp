//
// Created by soft01 on 2019/9/4.
//
#include <iostream>
#include <vector>
#include <thread>

#include "../DbManager.h"
#include "../RapidJson.h"




void thread_func()
{
    DbManager manager;
    RapidJson json;
    std::cout<<manager.checkUserAccount("1660994874@qq.com", "123456")<<std::endl;
    std::cout<<manager.accountExists("1660994874@qq.com")<<std::endl;
    std::cout<<manager.addUserInDB("nmsl1@163.com", "123456", "丁瑞")<<std::endl;
    std::cout<<manager.getProductionByRand(json)<<std::endl;
    std::cout<<json.toString()<<std::endl;
    RapidJson json2;
    std::cout<<manager.getUserInfoByUid(json2, 1)<<std::endl;
    std::cout<<json2.toString()<<std::endl;
    RapidJson json3;
    std::cout<<manager.getProductionByName(json3, "手机")<<std::endl;
    std::cout<<json3.toString()<<std::endl;
    RapidJson json4;
    std::cout<<manager.getProductionByPid(json4, 201)<<std::endl;
    std::cout<<json4.toString()<<std::endl;

    //std::cout<<manager.modProductionByPid("丁瑞", "丁瑞", 0.0, 2)<<std::endl;
    //std::cout<<manager.modUserInfoByUid("丁瑞爸爸", "男", "丁瑞的爸爸", 5)<<std::endl;
    //std::cout<<manager.addChatMessage(1, 4, "丁瑞傻逼")<<std::endl;
}

int main()
{
    std::vector<std::thread> list(4);
    for(int i = 0; i < 4; ++i)
    {
        list[i] = std::thread(thread_func);
    }
    for(auto& i : list)
        i.join();
}

