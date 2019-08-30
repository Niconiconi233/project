//
// Created by soft01 on 2019/7/29.
//

#include "TimerQueue_with_heap.h"

#include "../Logging/base/Logging.h"
#include "TimerId.h"
#include "EventLoop.h"

#include <unistd.h>
#include <sys/timerfd.h>
#include <algorithm>//find
#include <errno.h>

namespace
{
    int createTimerFd()
    {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        if(timerfd < 0)
        {
            LOG_ERROR << "createTimerFd failed";
        }
        return timerfd;
    }

    /*
     * 相对时间
     */
    timespec checkTime(Timestamp exp)
    {
        int64_t ms = exp.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
        if(ms < 100)
            ms = 100;
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
                ms / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(
                (ms % Timestamp::kMicroSecondsPerSecond) * 1000);
        return ts;
    }

    void readTimerfd(int timerfd, Timestamp now)
    {
        uint64_t howmany;
        ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
        LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
        if (n != sizeof howmany)
        {
            LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
        }
    }

}

TimerQueue_with_heap::TimerQueue_with_heap(EventLoop *loop)
    :loop_(loop),
    heap_(std::bind(&TimerQueue_with_heap::NodeCompare, this, std::placeholders::_1, std::placeholders::_2)),
    timerfd_(createTimerFd()),
    timerfdChannel_(loop, timerfd_),
    doCalling_(false)
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue_with_heap::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue_with_heap::~TimerQueue_with_heap()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    for(auto& i : map_)
        i.second.reset();
}

TimerId TimerQueue_with_heap::addTimer(TimerCallback cb, Timestamp when, double interval)
{
    std::shared_ptr<Timer> ptr(new Timer(std::move(cb), when, interval));
    loop_->runInLoop(std::bind(&TimerQueue_with_heap::addTimerInLoop, this, ptr));//异步调用
    return TimerId(ptr.get(), ptr->sequence());

}

void TimerQueue_with_heap::addTimerInLoop(std::shared_ptr<Timer> &ptr)
{
    loop_->assertInLoopThread();
    bool changed = insert(ptr);
    map_.insert(std::make_pair(ptr->sequence(), ptr));
    if(changed)
    {
        resetTimerFd(timerfd_, heap_.getTop().first);
    }
}

bool TimerQueue_with_heap::insert(std::shared_ptr<Timer> &ptr)
{
    if(heap_.size() == 0)
    {
        heap_.insert(std::make_pair(ptr->expiration(), ptr->sequence()));
        return true;
    }
    NodeItem tmp = heap_.getTop();
    heap_.insert(std::make_pair(ptr->expiration(), ptr->sequence()));
    NodeItem tmp2 = heap_.getTop();
    if(tmp.first == tmp2.first)
        return false;
    else
        return true;
}

void TimerQueue_with_heap::resetTimerFd(int timerfd, Timestamp exp)
{
    itimerspec old_value;
    itimerspec new_value;
    memZero(&old_value, sizeof old_value);
    memZero(&new_value, sizeof new_value);
    new_value.it_value = checkTime(exp);
    int ret = ::timerfd_settime(timerfd, 0, &new_value, &old_value);
    if(ret)
    {
        LOG_ERROR << "resetTimerFd failed";
    }
}

void TimerQueue_with_heap::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);
    doCalling_.store(true, std::memory_order_relaxed);
    auto list = getExpired(now);
    for(auto& i : list)
    {
        map_[i]->run();
    }
    removeOrAdd(list);
    doCalling_.store(false, std::memory_order_relaxed);
}

std::vector<uint64_t > TimerQueue_with_heap::getExpired(Timestamp &now)
{
    NodeItem tm;
    std::vector<uint64_t> vector_;
    heap_.getAndDelete(tm);
    vector_.emplace_back(tm.second);
    while(true)
    {
        tm = heap_.getTop();
        if(tm.first <= now)
        {
            heap_.getAndDelete(tm);
            vector_.emplace_back(tm.second);
        }else
            break;
    }
    return vector_;
}

void TimerQueue_with_heap::removeOrAdd(std::vector<uint64_t> &list)
{
    for(auto& i : list)
    {
        if(map_[i]->repeat()) {
            auto it = std::find(cancelingList_.begin(), cancelingList_.end(), map_[i]);
            if (it == cancelingList_.end()) {//是否存在与取消队列中
                map_[i]->restart(Timestamp::now());
                std::shared_ptr<Timer> ptr = map_[i];
                heap_.insert(std::make_pair(ptr->expiration(), ptr->sequence()));
            } else
            {
                map_.erase(i);
            }
        }else
        {
            int ret = map_.erase(i);
            if(ret != 1)
            {
                LOG_ERROR << "removeOrAdd failed in erase item";
            }
        }
    }
    cancelingList_.clear();//被取消的定时器此时析构
    NodeItem item(heap_.getTop());
    resetTimerFd(timerfd_, item.first);
}

void TimerQueue_with_heap::cancel(const TimerId id)
{
    loop_->runInLoop(std::bind(&TimerQueue_with_heap::cancleInLoop, this, id));
}

void TimerQueue_with_heap::cancleInLoop(TimerId id)
{
    loop_->assertInLoopThread();
    auto it = map_.find(id.sequence_);
    if(it == map_.end())
    {
        LOG_DEBUG << "cancel timer but timer not alive";
        return;
    }
    NodeItem item(it->second->expiration(), it->first);
    if(!doCalling_)//没有在处理时钟 此时可能可以删除定时器
    {
        NodeItem tmp(heap_.getTop());
        bool flag = heap_.remove(item, std::bind(&TimerQueue_with_heap::compare, this, std::placeholders::_1, std::placeholders::_2));
        if(!flag)
        {
            LOG_ERROR << "cancelInLoop: why did not remove the item from heap?";
            return;
        }
        if(tmp.first != heap_.getTop().first)//删除了当前定时器的节点
        {
            resetTimerFd(timerfd_, heap_.getTop().first);
        }
        map_.erase(it);//释放空间
    }else//当前正在处理定时器 此时该值可能在处理队列中
    {//添加到取消队列中
        cancelingList_.emplace_back(it->second);
    }
}
