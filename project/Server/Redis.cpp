//
// Created by soft01 on 2019/8/8.
//

#include "Redis.h"
#include "../Logging/base/SingLeton.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/Logging.h"

#include <exception>
#include <stdio.h>

namespace {
struct RedisException : public std::exception
    {
        RedisException(const char* str):str(str){}
        const char* what() const throw()
        {
            return str;
        }
        const char* str;
    };

}

Redis::Redis()
{
    init();
}

Redis::~Redis()
{
    redisFree(connect_);
}


void Redis::init()
{
    auto& reader = Singletion<ConfigReader>::instance();
    struct timeval timeout = { 1, 500000 };//连接超时为1.5秒
    connect_ =  redisConnectWithTimeout(reader.getAsString("redis_ip").c_str(), reader.getAsInt("redis_port"), timeout);
    if(connect_->err != 0)
    {
        //如果无法连接redis直接报错
       //printf("can't to redis failed\n");
       throw RedisException("redis not avaliable");
    }
}

std::string Redis::getStringValue(const std::string &key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "GET %s", key.c_str());
        if(reply_->type == REDIS_REPLY_STRING)
        {
            std::string res(reply_->str);
            freeReplyObject(reply_);
            return res;
        }else if(reply_->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply_);
            LOG_LOG << "Redis::getStringValue failed REDIS_REPLY_ERROR";
            return std::string{};
        }else if(reply_->type == REDIS_REPLY_NIL)
        {
            freeReplyObject(reply_);
            LOG_LOG << "Redis::getStringValue failed REDIS_REPLY_NIL";
            return std::string{};
        }
    }else
        return std::string{};
}

std::string Redis::getStringValue(unsigned int key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "GET %d", key);
        if(reply_->type == REDIS_REPLY_STRING)
        {
            std::string res(reply_->str);
            freeReplyObject(reply_);
            return res;
        }else if(reply_->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply_);
            //throw RedisException("redis_reply_error");
            LOG_LOG << "Redis::getStringValue failed REDIS_REPLY_ERROR";
            return std::string{};
        }else if(reply_->type == REDIS_REPLY_NIL)
        {
            freeReplyObject(reply_);
            //throw RedisException("redis_reply_nil");
            LOG_LOG << "Redis::getStringValue failed REDIS_REPLY_NIL";
            return std::string{};
        }
    }else
        return std::string{};
}

void Redis::setStringValue(unsigned int key, const std::string &value)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %d %s EX %d", key, value.c_str(), timeout_);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(const std::string &key, unsigned int value)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %s %d EX %d", key.c_str(), value, timeout_);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(const char* key, const char* value, int time)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %s %s EX %d", key, value, time);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(unsigned int key, const std::string &value, int time)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %d %s EX %d", key, value.c_str(), time);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue with timeout failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(const std::string &key, unsigned int value, int time)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %s %d EX %d", key.c_str(), value, time);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue with timeout failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(unsigned int key, int value)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %d %d EX %d", key, value, timeout_);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setStringValue(unsigned int key, int value, int time)
{
    if(!existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SET %d %d EX %d", key, value, time);
        if(strcmp(reply_->str, "OK") != 0)
        {
            LOG_ERROR << "Redis::setStringValue with timeout failed " << key << " " << value;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setSetValue(unsigned int key, unsigned int value)
{
    reply_ = (redisReply*)redisCommand(connect_, "SADD %d %d", key, value);
    if(reply_->integer != 1)
    {
        LOG_ERROR << "Redis::setSetValue set value failed " << key << value;
        freeReplyObject(reply_);
        return;
    }
    freeReplyObject(reply_);
    if(!hasExpire(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "EXPIRE %d %d", key, timeout_);
        if(reply_->integer != 1)
        {
            LOG_ERROR << "Redis::setSetValue set ex failed " << key;
            freeReplyObject(reply_);
            return;
        }
        freeReplyObject(reply_);
    }
}

void Redis::setSetValue(unsigned int key, unsigned int value, int time)
{
    reply_ = (redisReply*)redisCommand(connect_, "SADD %d %d", key, value);
    if(reply_->integer != 1)
    {
        LOG_ERROR << "Redis::setSetValue set value failed " << key << value;
        freeReplyObject(reply_);
        return;
    }
    freeReplyObject(reply_);
    if(!hasExpire(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "EXPIRE %d %d", key, time);
        if(reply_->integer != 1)
        {
            LOG_ERROR << "Redis::setSetValue set ex failed " << key;
            freeReplyObject(reply_);
            return;
        }
        freeReplyObject(reply_);
    }
}

std::vector<unsigned int> Redis::getSetValue(unsigned int key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "SMEMBERS %d", key);
        if(reply_->type == REDIS_REPLY_ARRAY)
        {
            std::vector<unsigned int> res;
            for(size_t i = 0; i < reply_->elements; ++i)
            {
                res.emplace_back(atoi(reply_->element[i]->str));
            }
            freeReplyObject(reply_);
            return res;
        }else
        {
            LOG_ERROR << "Redis::getSetValue failed " << key;
            freeReplyObject(reply_);
        }
    }else
    {
        LOG_DEBUG << "Redis::getSetValue failed key not exists " << key;
    }
    return std::vector<unsigned int>{};
}

void Redis::setSortSetValue(const std::string &key, unsigned int value)
{
    reply_ = (redisReply*)redisCommand(connect_, "ZADD hot %d %s", value, key.c_str());
    if(reply_->integer != 1)
    {
        LOG_ERROR << "Redis::setSortSetValue set value failed " << key << value;
        freeReplyObject(reply_);
        return;
    }
    freeReplyObject(reply_);
    if(!hasExpire("hot"))
    {
        reply_ = (redisReply*)redisCommand(connect_, "EXPIRE %s %d", "hot", time);
        if(reply_->integer != 1)
        {
            LOG_ERROR << "Redis::setSortSetValue set ex failed " << key;
            freeReplyObject(reply_);
            return;
        }
        freeReplyObject(reply_);
    }
}

void Redis::sortSetIncr(const std::string &key, int num)
{
    if(existsKey("hot"))
    {
        reply_ = (redisReply*)redisCommand(connect_, "ZINCRBY hot %d %s", num, key.c_str());
        freeReplyObject(reply_);
    }else
    {
        LOG_DEBUG << "Redis::SortSetIncr failed key not exists " << key;
    }
}

std::vector<std::string> Redis::sortSetTop()
{
    reply_ = (redisReply*)redisCommand(connect_, "ZREVRANGE hot 0 6");
    if(reply_->type != REDIS_REPLY_ARRAY)
    {
        LOG_ERROR << "Redis::sortSetTop failed";
        freeReplyObject(reply_);
        return std::vector<std::string>{};
    }
    std::vector<std::string> res;
    for(size_t i = 0; i < reply_->elements; ++i)
    {
        res.emplace_back(reply_->element[i]->str);
    }
    freeReplyObject(reply_);
    return res;
}

bool Redis::existsKey(const std::string &key)
{
    reply_ = (redisReply*)redisCommand(connect_, "EXISTS %s", key.c_str());
    if(reply_->integer == 1)
    {
        freeReplyObject(reply_);
        return true;
    }else
    {
        freeReplyObject(reply_);
        return false;
    }
}

bool Redis::existsKey(unsigned int key)
{
    reply_ = (redisReply*)redisCommand(connect_, "EXISTS %d", key);
    if(reply_->integer == 1)
    {
        freeReplyObject(reply_);
        return true;
    }else
    {
        freeReplyObject(reply_);
        return false;
    }
}

void Redis::selectTable(int num)
{
    reply_ = (redisReply*)redisCommand(connect_, "SELECT %d", num);
    if(strcmp(reply_->str, "OK") != 0)
        LOG_ERROR << "redis::selectTable failed " << num;
    freeReplyObject(reply_);
}

bool Redis::hasExpire(const std::string &key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "TTL %s", key.c_str());
        if(reply_->type == REDIS_REPLY_INTEGER)
        {
            if(reply_->integer > 0)
            {
                freeReplyObject(reply_);
                return true;
            }else
            {
                freeReplyObject(reply_);
                return false;
            }
        }else
        {
            LOG_ERROR << "Redis::hasExpire failed " << key;
            return false;
        }
    }else
        return false;
}

bool Redis::hasExpire(unsigned int key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "TTL %d", key);
        if(reply_->type == REDIS_REPLY_INTEGER)
        {
            if(reply_->integer > 0)
            {
                freeReplyObject(reply_);
                return true;
            }else
            {
                freeReplyObject(reply_);
                return false;
            }
        }else
        {
            LOG_ERROR << "Redis::hasExpire failed " << key;
            return false;
        }
    }else
        return false;
}

bool Redis::delValue(const std::string &key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "DEL %s", key.c_str());
        if(reply_->integer > 0)
        {
            freeReplyObject(reply_);
            return true;
        }else
        {
            freeReplyObject(reply_);
            LOG_DEBUG << "Redis::delValue " << reply_->integer;
            return false;
        }
    }else
        return false;
}

bool Redis::delValue(int key)
{
    if(existsKey(key))
    {
        reply_ = (redisReply*)redisCommand(connect_, "DEL %d", key);
        if(reply_->integer > 0)
        {
            freeReplyObject(reply_);
            return true;
        }else
        {
            freeReplyObject(reply_);
            LOG_DEBUG << "Redis::delValue " << reply_->integer;
            return false;
        }
    }else
        return false;
}


void Redis::setListValue(int uid, const char* message)
{
    reply_ = (redisReply*)redisCommand(connect_, "LPUSH %d %s", uid, message);
    if(reply_->integer > 0)
    {
        freeReplyObject(reply_);
    } else
    {
        LOG_ERROR << "Redis::setListValue failed";
        freeReplyObject(reply_);
    }
}

size_t Redis::getListLen(int uid)
{
    reply_ = (redisReply*)redisCommand(connect_, "LLEN %d", uid);
    size_t len = reply_->integer;
    freeReplyObject(reply_);
    return len;
}

std::vector<std::string> Redis::getListRange(int uid, size_t begin, size_t end)
{
    std::vector<std::string> res;
    reply_ = (redisReply*)redisCommand(connect_, "LRANGE %d %d %d", uid, begin, end);
    for(size_t i = 0; i < reply_->elements; ++i)
    {
        res.emplace_back(reply_->element[i]->str);
    }
    freeReplyObject(reply_);
    return res;
}

void Redis::removeListRange(int uid, int begin, int end)
{
    reply_ = (redisReply*)redisCommand(connect_, "LTRIM %d %d %d", uid, begin, end);
    if(strcmp(reply_->str, "OK") == 0)
    {
        freeReplyObject(reply_);
    }else
    {
        LOG_ERROR << "Redis::removeListRange failed " << reply_->str;
        freeReplyObject(reply_);
    }
}









