#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H
#include <atomic>
#include "Timestamp.h"
#include "../Logging/base/noncopyable.h"

#include "Callbacks.h"

class Timer : noncopyable
{
 public:
  Timer(TimerCallback cb, Timestamp when, double interval)
    : callback_(std::move(cb)),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0),
      sequence_(s_numCreated_.fetch_add(1, std::memory_order_relaxed))
  { }

  void run() const
  {
    callback_();
  }

  Timestamp expiration() const  { return expiration_; }
  //是否重复
  bool repeat() const { return repeat_; }

  int64_t sequence() const { return sequence_; }

  void restart(Timestamp now);

  static int64_t numCreated() { return s_numCreated_.load(); }

 private:
  const TimerCallback callback_;
  Timestamp expiration_;
  const double interval_;//间隔时间
  const bool repeat_;//是否重复
  const int64_t sequence_;

  //static AtomicInt64 s_numCreated_;
  static std::atomic<int64_t> s_numCreated_;
};

#endif  // MUDUO_NET_TIMER_H
