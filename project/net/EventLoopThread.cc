#include "EventLoopThread.h"

#include "EventLoop.h"
#include <sys/prctl.h>
#include <future>

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
  : loop_(NULL),
    exiting_(false),
    mutex_(),
    callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
  {
    loop_->quit();
    if(thread_.joinable())
        thread_.join();
  }
}

/*
 * 使用promise & future来启动县城
 */
EventLoop* EventLoopThread::startLoop()
{
  std::promise<EventLoop*> promise;
  std::future<EventLoop*> future = promise.get_future();
  std::thread tmp([&promise]{EventLoop loop;
                            promise.set_value(&loop);
                            ::prctl(PR_SET_NAME, "looping");
                            loop.loop();});
  thread_.swap(tmp);
  loop_ = future.get();
  return loop_;
}

