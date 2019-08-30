#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int cnt)
    : _mutex(),
    _cond(_mutex),
    count(cnt)
{
    
}

void CountDownLatch::wait()
{
    MutexLockGurard lock(_mutex);
    while(count > 0)
        _cond.wait();
}

void CountDownLatch::countdown()
{
    MutexLockGurard lock(_mutex);
    --count;
    if(count == 0)
        _cond.notifyall();
}

