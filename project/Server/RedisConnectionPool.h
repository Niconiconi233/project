//
// Created by soft01 on 2019/8/8.
//

#ifndef TEST_REDISCONNECTIONPOOL_H
#define TEST_REDISCONNECTIONPOOL_H

#include "Redis.h"

#include <memory>
#include <vector>
#include <mutex>
#include <cassert>
#include <condition_variable>

//test
//#include <iostream>


class RedisConnectionPool {
    using redisPtr = std::shared_ptr<Redis>;
public:
    RedisConnectionPool() {
        for (size_t i = 0; i < initSize_; ++i)
            pool_.emplace_back(new Redis);
    }

    ~RedisConnectionPool()
    {
        assert(pool_.size() == initSize_);
        for(auto& i : pool_)
            i.reset();
    }

    RedisConnectionPool(const RedisConnectionPool&) = delete;

    RedisConnectionPool& operator=(const RedisConnectionPool&) = delete;

    redisPtr getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&] { return pool_.size() != 0; });
        {
            redisPtr ptr(pool_.back());
            pool_.pop_back();
            //std::cout<<"get one"<<std::endl;
            return ptr;
        }
    }

    void close(const redisPtr &ptr)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        {
            pool_.emplace_back(ptr);
            cond_.notify_one();

        }
    }

private:
    size_t initSize_{5};
    std::vector<std::shared_ptr<Redis>> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class RedisConnectionPool_guard
{
public:
    RedisConnectionPool_guard(RedisConnectionPool& pool, const std::shared_ptr<Redis>& ptr)
        :pool(pool),
        ptr(ptr)
    {}

    ~RedisConnectionPool_guard()
    {
        pool.close(ptr);
    }

private:
    RedisConnectionPool& pool;
    const std::shared_ptr<Redis>& ptr;
};

#endif //TEST_REDISCONNECTIONPOOL_H
