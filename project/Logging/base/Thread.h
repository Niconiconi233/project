#ifndef _THREAD_H
#define _THREAD_H
/*
    启动线程实例 获取线程相关信息
*/

#include "noncopyable.h"
#include "MutexLock.h"
#include "CountDownLatch.h"
#include <pthread.h>
#include <functional>
#include <string>

class Thread
{
public:
    typedef std::function<void ()> ThreadFunc;
    explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
    ~Thread();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    void start();
    void join();
    bool started(){return this->started_;}
    pid_t tid(){return tid_;}
    const std::string& name(){return name_;}

private:
    bool joined_;
    bool started_;
    pthread_t pthread_id;//thread id
    pid_t tid_;//thread pid
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

private:
    void setDefaultName();
};


#endif // !_THREAD_H
