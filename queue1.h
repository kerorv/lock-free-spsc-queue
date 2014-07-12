#pragma once

#include <deque>
#include <mutex>

template <class T>
class Queue1
{
public:
    Queue1()
    {
    }

    ~Queue1()
    {
    }

    bool Put(const T& t)
    {
        mutex_.lock();
        q_.push_back(t);
        mutex_.unlock();
        return true;
    }

    bool Get(T& t)
    {
        mutex_.lock();
        if (q_.size() == 0)
        {
            mutex_.unlock();
            return false;
        }

        t = q_.front();
        q_.pop_front();
        mutex_.unlock();

        return true;
    }

private:
    std::deque<T> q_;
    std::mutex mutex_;
};

