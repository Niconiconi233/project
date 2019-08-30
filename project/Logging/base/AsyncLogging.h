#ifndef _ASYNCLOGGING_H
#define _ASYNCLOGGING_H

#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <memory>
#include <functional>
#include <string>
#include <vector>

class AsyncLogging : noncopyable
{
public:
    AsyncLogging(const std::string& basename, int flushInterval = 3);
    ~AsyncLogging()
    {
        if(running_)
            stop();
    }
    void append(const char* logline, int len);
    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();//returned only when func actually running
    }

    void stop()
    {
        running_ = false;
        cond_.notify();//if thread func is now sleeping wake up thread func
        thread_.join();
    }


private:
    void threadFunc();
    typedef FixdBuffer<KLargeBuffer> Buffer_;
    typedef std::vector<std::shared_ptr<Buffer_>> BufferVector;
    typedef std::shared_ptr<Buffer_> BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;//buffer 1
    BufferPtr nextBuffer_;//buffer 2
    BufferVector buffers_;//vector to save buffer ptr
    CountDownLatch latch_;//条件变量
};

#endif // !_ASYNCLOGGIN_H   
