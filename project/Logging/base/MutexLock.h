#ifndef _MUTEXLOCK_H
#define _MUTEXLOCK_H

/*
    锁
*/

#include <pthread.h>
#include "noncopyable.h"

#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

#define CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

///声明数据成员受给定功能的保护。对数据的读操作需要共享访问，而写操作需要独占访问。
#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))
///用于指针和智能指针。数据成员本身没有约束，但它指向的数据受给定功能的保护。
#define PT_GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))
///ACQUIRED_BEFORE并且ACQUIRED_AFTER是成员声明的属性，特别是互斥锁或其他功能的声明。这些声明强制执行必须获取互斥锁的特定顺序，以防止死锁。
#define ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))
///声明调用线程必须具有对给定功能的独占访问权
#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))
///REQUIRES_SHARED 类似，但只需要共享访问。
#define REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))
///声明函数获取一个功能，但不释放它。调用者不能在进入时保持给定的能力，并且它将在退出时保持该能力。 ACQUIRE_SHARED类似。
#define ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))
///RELEASE和RELEASE_SHARED声明该函数释放给定的功能。呼叫者必须具有进入功能，并且在退出时将不再保留该功能
#define RELEASE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))
///声明调用者不能保持给定的功能。此注释用于防止死锁。许多互斥实现不可重入，因此如果函数第二次获取互斥锁，则会发生死锁。
#define EXCLUDES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))
//声明函数返回对给定功能的引用。它用于注释返回互斥锁的getter方法
#define RETURN_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))
///函数或方法的属性，它关闭该方法的线程安全检查
///NO_THREAD_SAFETY_ANALYSIS不是函数接口的一部分，因此应放在函数定义（在.cc或.cpp文件中）而不是函数声明
#define NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

// End of thread safety annotations }

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail (int errnum,
                                  const char *file,
                                  unsigned int line,
                                  const char *function)
    noexcept __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum;})

#endif // CHECK_PTHREAD_RETURN_VALUE

class MutexLock : noncopyable
{
    friend class Condition;
public:
    MutexLock()
    {
        pthread_mutexattr_init(&attr_);
        pthread_mutexattr_settype(&attr_, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex, &attr_);
    }

    ~MutexLock()
    {
        pthread_mutex_unlock(&mutex);
        pthread_mutexattr_destroy(&attr_);
        pthread_mutex_destroy(&mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }

    void trylock()
    {
        pthread_mutex_trylock(&mutex);
    }

    pthread_mutex_t* get()
    {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr_;
};

class MutexLockGurard
{
public:
    explicit MutexLockGurard(MutexLock& mutex):mutex(mutex)
    {
        mutex.lock();
    }
    
    ~MutexLockGurard()
    {
        mutex.unlock();
    }

private:
    MutexLock& mutex;
};

#endif // !_MUTEXLOCK_H
