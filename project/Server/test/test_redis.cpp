//
// Created by soft01 on 2019/8/8.
//

#include "../Redis.h"
#include "../RedisConnectionPool.h"
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

RedisConnectionPool pool;

void func()
{
    auto conn = pool.getConnection();
    RedisConnectionPool_guard guard(pool, conn);
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main()
{
    /*RedisConnectionPool pool;
    auto conn = pool.getConnection();
    conn->selectTable(0);
    conn->setStringValue(1, "{\"uid\": 1,\"token\": \"hhhhhhhajkfhkjdhfuiwehfhdjkfhkjsdhfk\",\"status\": 1}");
    conn->setStringValue(2, "{\"uid\": 2,\"token\": \"hhhhhhhajkfhkjdhfuiwehfhdjkfhkjsdhfk\",\"status\": 1}", 60 * 60);
    conn->setStringValue(3, 3);
    conn->setStringValue(4, 4, 60 * 60);

    cout<<conn->getStringValue(1)<<endl;
    cout<<conn->getStringValue(2)<<endl;
    cout<<conn->getStringValue(3)<<endl;
    cout<<conn->getStringValue(4)<<endl;

    conn->setSetValue(5, 1024);
    conn->setSetValue(5, 2048);
    conn->setSetValue(6, 10086);
    conn->setSetValue(5, 4096);
    conn->setSetValue(7, 233, 60 * 60);

    auto a = conn->getSetValue(5);
    auto b = conn->getSetValue(6);
    auto c = conn->getSetValue(7);
    for(auto& i : a)
        cout<<i<<" ";
    cout<<endl;
    for(auto& i : b)
        cout<<i<<" ";
    cout<<endl;
    for(auto& i : c)
        cout<<i<<" ";
    cout<<endl;

    //conn->selectTable(1);
    conn->setSortSetValue("丁瑞的老婆", 1);
    conn->sortSetIncr("丁瑞的老婆", 1);
    conn->setSortSetValue("手机", 1);
    conn->setSortSetValue("狗", 1);
    //conn->setSortSetValue("丁瑞的老婆", 1);
    conn->sortSetIncr("丁瑞的老婆", 1);
    conn->sortSetIncr("手机", 1);

    auto d = conn->sortSetTop();
    for(auto& i : d)
        cout<<i<<" ";
    cout<<endl;

    pool.close(conn);

    std::this_thread::sleep_for(std::chrono::seconds(5));*/

    //std::vector<std::thread> list(15);
    //for(int i = 0; i < 15; ++i)
    //{
    //    list[i] = std::thread(func);
    //}
    //for(auto& i : list)
    //    i.join();
    auto conn = pool.getConnection();
    conn->setListValue(1, "nmsl");
    conn->setListValue(2, "nmsl");
    conn->setListValue(1, "cnm");
    std::cout<<conn->getListLen(1)<<std::endl;
    auto list = conn->getListRange(1, 0, 5);
    for(auto& i : list)
        std::cout<<i<<std::endl;
    pool.close(conn);
    return 0;
}

