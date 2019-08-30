#ifndef MUDUO_NET_TIMERID_H
#define MUDUO_NET_TIMERID_H

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId
{
 public:
  TimerId()
    : timer_(NULL),
      sequence_(0)
  {
  }

  TimerId(Timer* timer, int64_t seq)
    : timer_(timer),
      sequence_(seq)
  {
  }

  //int64_t seq()
  //{
  //    return sequence_;
  //}

  // default copy-ctor, dtor and assignment are okay

  friend class TimerQueue;
  friend class TimerQueue_with_heap;

 private:
  Timer* timer_;
  int64_t sequence_;
};

#endif  // MUDUO_NET_TIMERID_H
