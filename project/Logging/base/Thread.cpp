#include "Thread.h"
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "CurrentThread.h"

//每个线程一个
namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
}

//pthread_self获取的是线程id, 要获取线程pid要调用syscall
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid()
{
    if(t_cachedTid == 0){
        t_cachedTid = gettid();
        snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
    }
}
//对线程的包装，获取了线程的相关信息，实际调用的是这个的runinthread
struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func;
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& fun, const std::string& name, pid_t* tid, CountDownLatch* latch)
        :func(fun), 
        name_(name), 
        tid_(tid), 
        latch_(latch)
{

}

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->countdown();//notify
        latch_ = nullptr;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);//set thread name
        func();
        CurrentThread::t_threadName = "finished";
    } 
};

void* startThread(void* arg)
{
    ThreadData* data = static_cast<ThreadData*>(arg);
    data->runInThread();
    return NULL;
}

Thread::Thread(const ThreadFunc& func, const std::string& n)
    : joined_(false), 
    started_(false), 
    pthread_id(0), 
    tid_(0), 
    func_(func), 
    name_(n), 
    latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_){
        pthread_detach(pthread_id);
    }
}

void Thread::setDefaultName()
{
    if(name_.empty()){
        char buf[32];
        snprintf(buf, sizeof buf, "%s", "Thread");
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!this->started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthread_id, NULL, startThread, data)){
        delete data;
        started_ = false;
    }else{
        latch_.wait();
        assert(tid_> 0);
    }
}

void Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    pthread_join(pthread_id, NULL);
}
