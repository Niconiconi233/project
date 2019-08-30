#include "AsyncLogging.h"
#include "LogFile.h"
#include <assert.h>
#include <unistd.h>
#include <functional>

#include <iostream>

AsyncLogging::AsyncLogging(const std::string& basename, int flushInterval )
    : flushInterval_(flushInterval), 
    running_(false), 
    basename_(basename),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Loggin"),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer_),
    nextBuffer_(new Buffer_),
    buffers_(),latch_(1)
{
    assert(basename.size() > 0);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

//生产者
void AsyncLogging::append(const char* logline, int len)
{
    MutexLockGurard lock(mutex_);
    if(currentBuffer_->avail() > static_cast<size_t>(len))
        currentBuffer_->append(logline, len);
    else
    {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if(nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);//buffer 2 si avaliable
        else
        {
            currentBuffer_.reset(new Buffer_);//get a new buffer
        }
        currentBuffer_->append(logline, len);
        cond_.notify();//wake up sleep thread
    } 
}
//守护进程 定时刷新缓冲区
void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countdown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer_);
    BufferPtr newBuffer2(new Buffer_);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_)
    {
        assert(newBuffer1 && newBuffer1->len() == 0);
        assert(newBuffer2 && newBuffer2->len() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGurard lock(mutex_);
            if (buffers_.empty())  // unusual usage!
            {
                cond_.timedwait(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());
        //如果前端发送了过量的数据，丢弃这些数据
        if (buffersToWrite.size() > 25)
        {
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            output.append(buffersToWrite[i]->begin(), buffersToWrite[i]->len());
        }

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);//智能指针被释放
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();//cur to data
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
