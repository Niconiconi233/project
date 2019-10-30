#ifndef FILESERV_TIMERHEAP_HPP
#define FILESERV_TIMERHEAP_HPP

#include <vector>
#include <cstddef>
#include <functional>
#include <mutex>
#include <stack>

#include <iostream>

template<typename T>
bool defaultCompare(const T& lhs, const T& rhs)
{
    return lhs > rhs;
}

/*
 * 需要提供比较函数和相等函数
 * 通常比较的是父节点的值大于子节点的值
 */

template<typename T>
class TimerHeap
{
public: 
    using Compare = std::function<bool (const T&, const T&)>;
    TimerHeap(const Compare& cmp/* = std::bind(&defaultCompare, std::placeholders::_1, std::placeholders::_2)*/):arr_(), compare_(cmp){}
    ~TimerHeap()
    {
        //do nothing
    }

    /*
     *线程安全
     */
    void insert(const T value)
    {
        insertAndAdjust(value);
    }

    bool remove(const T& value, std::function<bool(const T& lhs, const T& rhs)> compare)
    {
       return searchAndRemove(value, compare);
    }

    /*
     *线程安全的获取函数
     */
    void getAndDelete(T& value)
    {
        removeAndAdjust(value, 0);
    }

    /*
     *获取头部的拷贝 应该是线程安全的
     */
    const T getTop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        T tmp(arr_[0]);
        return tmp;
    }

    bool isEmpty()
    {
        return size() == 0;
    }

    int size()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return arr_.size();
    }

    bool exist(const T& value, std::function<bool(const T& lhs, const T& rhs)> compare)
    {
        return search(value, compare);
    }

private: 
    size_t getLeftChild(size_t i)
    {
        return (i << 1) + 1;
    }
    size_t getRightChild(size_t i)
    {
        return (i << 1) + 2;
    }
    size_t getParent(size_t i)
    {
        return (i - 1) >> 1;
    }

    /*
     * 异常安全
     */
    void insertAndAdjust(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        arr_.emplace_back(value);
        floatingUpWithUp(arr_.size() - 1);
    }

    /*
     * 异常安全
     */
    void removeAndAdjust(T& value, size_t pos)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(arr_.size() == 0)
            return;
        if(arr_.size() == 1)
        {
            value = std::move(arr_[0]);
            arr_.pop_back();
            return;
        }else
        {
            std::swap(arr_[pos], arr_[arr_.size() - 1]);
            value = std::move(arr_[arr_.size() - 1]);
            arr_.pop_back();
        }
        sinkWithLock(0);
    }

    /*
     * 内部函数 必须带锁调用
     */
    void removeAndAdjustWithLock(size_t pos)
    {
        {
            std::swap(arr_[pos], arr_[arr_.size() - 1]);
            arr_.pop_back();
        }
        sinkWithLock(pos);
    }

    /*
     * 内部类 必须带锁调用
     */
    void floatingUpWithUp(size_t pos)
    {
        while(pos != 0)
        {
            size_t parent = getParent(pos);
            if(compare_(arr_[parent], arr_[pos]))
            {
                std::swap(arr_[parent], arr_[pos]);
                pos = parent;
            }else
            {
                break;
            }
        }
    }

    /*
     * 内部类 必须带锁调用
     */
    void sinkWithLock(size_t pos)
    {
        size_t l = getLeftChild(pos);
        while(l < arr_.size())
        {
            if(l + 1 < arr_.size() && compare_(arr_[l], arr_[l + 1]))
                ++l;
            if(compare_(arr_[l], arr_[pos]))
                break;
            else
            {
                std::swap(arr_[l], arr_[pos]);
                pos = l;
                l = getLeftChild(pos);
            }
        }
    }

    /*
     * 先序遍历进行处理 成功删除返回true 否则返回false
     */
    bool searchAndRemove(const T& value, std::function<bool(const T& lhs, const T& rhs)>& compare)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(arr_.size() == 0)
            return false;
        std::stack<size_t> stack_;
        size_t pos = 0;
        bool flag = false;
        while(pos < arr_.size() || !stack_.empty())
        {
            while(pos < arr_.size())
            {
                stack_.push(pos);
                if(compare(arr_[pos], value))
                {
                    removeAndAdjustWithLock(pos);
                    flag = true;
                    break;
                }
                pos = getLeftChild(pos);
            }
            pos = getLeftChild(pos);
            if(!stack_.empty())
            {
                pos = stack_.top();
                stack_.pop();
                pos = getRightChild(pos);
            }
        }
        return flag;
    }

    bool search(const T& value, std::function<bool(const T& lhs, const T& rhs)>& compare)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(arr_.size() == 0)
            return false;
        std::stack<size_t> stack_;
        size_t pos = 0;
        bool flag = false;
        while(pos < arr_.size() || !stack_.empty())
        {
            while(pos < arr_.size())
            {
                stack_.push(pos);
                if(compare(arr_[pos], value))
                {
                    //removeAndAdjustWithLock(pos);
                    flag = true;
                    break;
                }
                pos = getLeftChild(pos);
            }
            pos = getLeftChild(pos);
            if(!stack_.empty())
            {
                pos = stack_.top();
                stack_.pop();
                pos = getRightChild(pos);
            }
        }
        return flag;
    }

private: 
    std::vector<T> arr_;
    Compare compare_;
    std::mutex mutex_;
};


#endif // !FILESERV_TIMERHEAP_HPP