//
// Created by soft01 on 2019/9/11.
//

#include <functional>
#include <iostream>

#include "TaskList.h"
#include "Redis.h"
#include "RedisConnectionPool.h"
#include "../Logging/base/Logging.h"
#include "../Logging/base/SingLeton.h"

namespace
{
    bool compare(const taskItem& lhs, const taskItem& rhs)
    {
        return lhs.first > rhs.first;
    }

    bool compare1(const taskItem& lhs, const taskItem& rhs)
    {
        return lhs.second == rhs.second;
    }
}

TaskList::TaskList()
    :timerHeap_(new TimerHeap<taskItem >(std::bind(&compare, std::placeholders::_1, std::placeholders::_2))),
    handleing_(false)
{

}

TaskList::~TaskList()
{

}

void TaskList::push(int time, const std::string &orderlist)
{
    Timestamp left(addTime(Timestamp::now(), time));
    auto node = std::make_pair(left, orderlist);
    timerHeap_->insert(node);
}

void TaskList::check()
{
    if(timerHeap_->isEmpty())
        return;
    auto top(timerHeap_->getTop());
    Timestamp check(addTime(Timestamp::now(), 3));
    while(top.first <= check && !timerHeap_->isEmpty())
    {
        timerHeap_->getAndDelete(top);
        taskList_.emplace_back(top);
        top = timerHeap_->getTop();
    }
    handleTask();
}

void TaskList::handleTask()
{
    handleing_ = true;
    for(auto& i : taskList_)
    {
        //std::cout<<i.second<<std::endl;
        auto& redispool = Singletion<RedisConnectionPool>::instance();
        auto redisconn = redispool.getConnection();
        redisconn->selectTable(5);
        int pid = atoi(redisconn->getStringValue(i.second).c_str());
        redispool.close(redisconn);
        if(pid <= 0)
        {
            LOG_ERROR << "TaskList::handleTask failed orderlist not exits " << i.second;
            continue;
        }
        dbManager_.releaseOrder(i.second, pid);
    }
    handleing_ = false;
    taskList_.clear();
}


bool TaskList::remove(const std::string &orderlist)
{
    taskItem tmp(std::make_pair(Timestamp::now(), orderlist));
    if(!timerHeap_->remove(tmp, std::bind(&compare, std::placeholders::_1, std::placeholders::_2)))
    {
        if(handleing_)
            LOG_DEBUG << "TaskList::remove remove orderlist but it has deleted " << orderlist;
        return false;
    }else
        return true;
}

bool TaskList::exist(const std::string &orderlist)
{
    taskItem tmp(std::make_pair(Timestamp::now(), orderlist));
    return timerHeap_->exist(tmp, std::bind(&compare1, std::placeholders::_1, std::placeholders::_2));
}