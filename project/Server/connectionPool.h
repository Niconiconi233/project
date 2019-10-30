//
// Created by soft01 on 19-7-16.
//

#ifndef MYSQL_CONNECTIONPOOL_H
#define MYSQL_CONNECTIONPOOL_H

#include <mysql/jdbc.h>
#include <vector>
#include <memory>
#include <pthread.h>
#include "../Logging/base/SingLeton.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/Logging.h"

#include <stdio.h>//printf

/*
 * 阻塞式MYSQL连接池
 */


class ConnectionPool
{
public:
    using connPtr = std::shared_ptr<sql::Connection>;

    ConnectionPool()
    {
        auto& reader = Singletion<ConfigReader>::instance();
        pthread_mutexattr_init(&mutexAttr_);
        pthread_mutexattr_setrobust(&mutexAttr_, PTHREAD_MUTEX_ROBUST);
        pthread_mutexattr_settype(&mutexAttr_, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex_, &mutexAttr_);
        pthread_cond_init(&cond_, NULL);
        map_["hostName"] = reader.getAsString("host");
        map_["userName"] = reader.getAsString("user");
        map_["password"] = reader.getAsString("pwd");
        map_["schema"] = reader.getAsString("schema");
        map_["OPT_CHARSET_NAME"] = "utf8";
        map_["OPT_CONNECT_TIMEOUT"] = 10;
        map_["OPT_RECONNECT"] = true;
        initsize_ = 5;
        currsize_ = initsize_;
        this->init();
    }

    ConnectionPool(const ConnectionPool&) = delete;

    ConnectionPool& operator=(const ConnectionPool&) = delete;

    ~ConnectionPool()
    {
        assert(vector_.size() == currsize_);
        for(auto& i : vector_)
            i.reset();
    }

    void init()
    {
        try{
            //非线程安全
            sql::Driver* driver = sql::mysql::get_driver_instance();
            for(size_t i = 0; i < currsize_; ++i) {
                vector_.emplace_back(driver->connect(map_));
            }
        }catch (sql::SQLException& e)
        {
            LOG_FATAL << "connectionPool create connection failed";
            printf("%s", e.what());
            abort();
        }

    }

    connPtr getConnection()
    {
        pthread_mutex_lock(&mutex_);
        while(vector_.size() == 0)
        {
            pthread_cond_wait(&cond_, &mutex_);
        }
        {
            connPtr ptr(std::move(vector_.back()));
            vector_.pop_back();
            refixPool();
            pthread_mutex_unlock(&mutex_);
            return ptr;
        }
    }

    void close(connPtr& ptr)
    {
        assert(ptr != NULL);
        {
            pthread_mutex_lock(&mutex_);
            vector_.emplace_back(ptr);
            refixPool();
            pthread_cond_broadcast(&cond_);
            pthread_mutex_unlock(&mutex_);
        }
    }
private:
    void refixPool()
    {
        if(vector_.size() - 1 == 0)//扩张链接池
        {
            if(currsize_ < KMaxSize)
            {
                sql::Driver* driver = sql::mysql::get_driver_instance();
                for(int i = 0; i < 2 && currsize_ < KMaxSize; ++i, ++currsize_)
                {
                    vector_.emplace_back(driver->connect(map_));
                }
                LOG_LOG << "group pool " << currsize_;
            } else
            {
                LOG_WARN << "ConnectioinPool's size up to top " << currsize_;
            }
        }else if(vector_.size() > initsize_)
        {
            for(size_t i = 0; i < vector_.size() - initsize_; ++i, --currsize_)
                vector_.pop_back();
            LOG_LOG << "make smaill " << currsize_<<" & "<<vector_.size();
        }
    }
private:
    size_t initsize_;
    size_t currsize_;
    size_t KMaxSize{8};
    std::vector<connPtr> vector_;
    sql::ConnectOptionsMap map_;

    pthread_cond_t cond_;
    pthread_mutex_t mutex_;
    pthread_mutexattr_t mutexAttr_;
};


#endif //MYSQL_CONNECTIONPOOL_H
