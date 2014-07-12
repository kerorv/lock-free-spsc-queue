#pragma once

#include <cassert>
#include <mutex>

template <class T>
class CircleBuffer1
{
public:
	CircleBuffer1(size_t capacity)
		: rpos_(0)
		, wpos_(0)
	{
		assert(capacity != 0 && (capacity & (capacity -1)) == 0);
		buffer_  = (T*)malloc(capacity * sizeof(T));
		capacity_ = capacity;
	}

	~CircleBuffer1()
	{
		free(buffer_);
	}

#define PMASK(N) ((N) & (capacity_ - 1))

	bool Put(const T& t)
	{
        mutex_.lock();
		size_t npos = PMASK(wpos_ + 1);
		if (rpos_ == npos)
        {
            mutex_.unlock();
			return false;
        }

		buffer_[wpos_] = t;
		wpos_ = npos;
        mutex_.unlock();
		
		return true;
	}

	bool Get(T& t)
	{
        mutex_.lock();
		if (rpos_ == wpos_)
        {
            mutex_.unlock();
			return false;
        }

		t = buffer_[rpos_];
		rpos_ = PMASK(rpos_ + 1);
        mutex_.unlock();

		return true;
	}

private:
	T* buffer_;
	size_t capacity_;
	size_t rpos_;
	size_t wpos_;
    std::mutex mutex_;
};

