#ifndef FILESERV_SINGLETON_H
#define FILESERV_SINGLETON_H

#include <memory>
#include <atomic>
#include <mutex>

template <typename T>
class Singleton_with_lock
{
public:
    Singleton_with_lock() = delete;
    Singleton_with_lock(const Singleton_with_lock&) = delete;
    Singleton_with_lock& operator=(const Singleton_with_lock&) = delete;

    static T* instance()
    {
        T* tmp = ptr_.load();
        if(tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            T* tmp2 = ptr_.load();
            if(tmp2 == nullptr)
            {
                T* tmp3 = new T;
                ptr_.store(tmp3, std::memory_order_relaxed);
            }
        }
        return ptr_;
    }

private:
    static std::atomic<T*> ptr_{nullptr};
    static std::mutex mutex_;
};

template <typename T>
class Singletion
{
public:
    Singletion() = delete;
    Singletion(const Singletion&) = delete;
    Singletion& operator=(const Singletion&) = delete;

    static T& instance()
    {
        static T item;
        return item;
    }
};



#endif // !FILESERV_SINGLETON_H