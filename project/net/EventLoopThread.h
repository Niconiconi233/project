#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include <thread>
#include <mutex>
#include <functional>
#include "../Logging/base/noncopyable.h"

/*
 * eventLoop循环线程
 */

class EventLoop;

class EventLoopThread : noncopyable
{
 public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
  ~EventLoopThread();
  EventLoop* startLoop();

 private:

  EventLoop* loop_;
  bool exiting_;
  //Thread thread_;
  std::thread thread_;
  //MutexLock mutex_;
  std::mutex mutex_;
  //Condition cond_ GUARDED_BY(mutex_);
  ThreadInitCallback callback_;
};

#endif  // MUDUO_NET_EVENTLOOPTHREAD_H

