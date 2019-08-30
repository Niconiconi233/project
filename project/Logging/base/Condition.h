#ifndef _CONDITION_H
#define _CONDITION_H
/*
条件变量
*/

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "MutexLock.h"
#include "noncopyable.h"

class Condition : noncopyable
{
public:
    Condition(MutexLock& _mutex)
    : mutex(_mutex)
    {
        pthread_cond_init(&mutex_cond, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&mutex_cond);
    }

    void wait()
    {
        pthread_cond_wait(&mutex_cond, mutex.get());
    }

    void notify()
    {
        pthread_cond_signal(&mutex_cond);
    }

    void notifyall()
    {
        pthread_cond_broadcast(&mutex_cond);
    }

    bool timedwait(int sec)
    {
        struct timespec tp;
        clock_getres(CLOCK_REALTIME, &tp);
        tp.tv_sec += static_cast<time_t>(sec);
        return ETIMEDOUT == pthread_cond_timedwait(&mutex_cond, mutex.get(), &tp);
    }

private:
    MutexLock &mutex;
    pthread_cond_t mutex_cond;
};

#endif // !_CONDITION_H
