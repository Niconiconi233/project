#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <atomic>
#include <functional>
#include <vector>
#include <mutex>
#include "Timestamp.h"
#include "../Logging/base/CurrentThread.h"
#include "../Logging/base/noncopyable.h"
#include "Callbacks.h"
#include "TimerId.h"

/*
 * 主要的循环 对epoll返回的事件进行调用
 * 只持有epoll和wakeupfd的unique_ptr 负责管理他们的生命周期
 */

class Channel;
class EPollPoller;
class TimerQueue;
class TimerQueue_with_heap;

class EventLoop : noncopyable
{
 public:
  typedef std::function<void()> Functor;

  EventLoop();
  ~EventLoop();  // force out-line dtor, for std::unique_ptr members.

  void loop();

  void quit();

  Timestamp pollReturnTime() const { return pollReturnTime_; }

  int64_t iteration() const { return iteration_; }


  void runInLoop(Functor cb);

  void queueInLoop(Functor cb);

  size_t queueSize() const;


  TimerId runAt(Timestamp time, TimerCallback cb);

  TimerId runAfter(double delay, TimerCallback cb);

  TimerId runEvery(double interval, TimerCallback cb);

  TimerId runAtWithHeap(Timestamp time, TimerCallback cb);

  TimerId runAfterWithHeap(double delay, TimerCallback cb);

  TimerId runEveryWithHeap(double interval, TimerCallback cb);

  void cancel(TimerId timerId);

  void cancelWithHeap(TimerId timerId);

  // internal usage
  void wakeup();
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  bool hasChannel(Channel* channel);

  // pid_t threadId() const { return threadId_; }
  void assertInLoopThread()
  {
    if (!isInLoopThread())
    {
      abortNotInLoopThread();
    }
  }
  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

  bool eventHandling() const { return eventHandling_; }

  //void setContext(const boost::any& context)
  //{ context_ = context; }

  //const boost::any& getContext() const
  //{ return context_; }

  //boost::any* getMutableContext()
  //{ return &context_; }

  static EventLoop* getEventLoopOfCurrentThread();

 private:
  void abortNotInLoopThread();
  void handleRead();  // waked up
  void doPendingFunctors();

  //void printActiveChannels() const; // DEBUG

  typedef std::vector<Channel*> ChannelList;

  //bool looping_; /* atomic */
  //bool eventHandling_; /* atomic */
  //bool callingPendingFunctors_; /* atomic */
  std::atomic<bool> looping_;
  std::atomic<bool> quit_;
  std::atomic<bool> eventHandling_;
  std::atomic<bool> callingPendingFunctors_;
  int64_t iteration_;
  const pid_t threadId_;
  Timestamp pollReturnTime_;
  std::unique_ptr<EPollPoller> poller_;
  std::unique_ptr<TimerQueue> timerQueue_;
  int wakeupFd_;
  // unlike in TimerQueue, which is an internal class,
  // we don't expose Channel to client.
  std::unique_ptr<Channel> wakeupChannel_;
  //boost::any context_;

  // scratch variables
  ChannelList activeChannels_;
  Channel* currentActiveChannel_;

  //mutable MutexLock mutex_;
  mutable std::mutex mutex_;
  std::vector<Functor> pendingFunctors_;
  std::unique_ptr<TimerQueue_with_heap> timerHeapQueue_;
};

#endif  // MUDUO_NET_EVENTLOOP_H
