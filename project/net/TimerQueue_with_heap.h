//
// Created by soft01 on 2019/7/29.
//

#ifndef STATIC_LIB_TIMERQUEUE_WITH_HEAP_H
#define STATIC_LIB_TIMERQUEUE_WITH_HEAP_H

//#include <set>
#include <unordered_map>
#include <functional>
#include <memory>
#include <atomic>

#include "TimerHeap.hpp"
#include "Timestamp.h"
#include "Timer.h"
#include "Callbacks.h"
#include "Channel.h"

class TimerId;
class EventLoop;

class TimerQueue_with_heap {
public:
    TimerQueue_with_heap(EventLoop* loop);

    TimerQueue_with_heap(const TimerQueue_with_heap&) = delete;
    TimerQueue_with_heap& operator=(const TimerQueue_with_heap&) = delete;
    ~TimerQueue_with_heap();

    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

    void cancel(const TimerId id);//通过timerid的second来取消heap中的元素


private:
    using NodeItem = std::pair<Timestamp, uint64_t>;
    EventLoop* loop_;
    TimerHeap<NodeItem> heap_;
    std::unordered_map<uint64_t, std::shared_ptr<Timer>> map_;//引索 获取Timer*
    int timerfd_;
    Channel timerfdChannel_;
    std::atomic_bool doCalling_;
    std::vector<std::shared_ptr<Timer>> cancelingList_;


private:
    bool NodeCompare(const NodeItem& lhs, const NodeItem& rhs)
    {
        return lhs.first > rhs.first;
    }

    void handleRead();

    void addTimerInLoop(std::shared_ptr<Timer>&);

    bool insert(std::shared_ptr<Timer>&);

    void resetTimerFd(const int, Timestamp);

    std::vector<uint64_t > getExpired(Timestamp& now);

    void removeOrAdd(std::vector<uint64_t >& list);

    void cancleInLoop(TimerId id);

    bool compare(const NodeItem& lhs, const NodeItem& rhs)
    {
        return lhs.second == rhs.second;
    }

};


#endif //STATIC_LIB_TIMERQUEUE_WITH_HEAP_H
