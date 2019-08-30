#ifndef _COUNTDOWNLATCH_H
#define _COUNTDOWNLATCH_H

#include "MutexLock.h"
#include "Condition.h"
#include "noncopyable.h"

//确保的确进入了执行函数

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int cnt);
    void wait();
    void countdown();

private:
    mutable MutexLock _mutex;
    Condition _cond;
    int count;

};



#endif // !_COUNTDOWNLATCH_H