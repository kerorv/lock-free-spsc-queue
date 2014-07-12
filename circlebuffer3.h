#pragma once

#include <cassert>
#include <atomic>

template <class T>
class CircleBuffer3
{
public:
	CircleBuffer3(size_t capacity)
		: rpos_(0)
		, wpos_(0)
	{
		assert(capacity != 0 && (capacity & (capacity -1)) == 0);
		buffer_  = (T*)malloc(capacity * sizeof(T));
		capacity_ = capacity;
	}

	~CircleBuffer3()
	{
		free(buffer_);
	}

#define PMASK(N) ((N) & (capacity_ - 1))

	bool Put(const T& t)
	{
		size_t wpos = wpos_.load(std::memory_order_relaxed);
		size_t rpos = rpos_.load(std::memory_order_acquire);
		size_t npos = PMASK(wpos + 1);
		if (rpos == npos)
			return false;

		buffer_[wpos] = t;
		wpos_.store(npos, std::memory_order_release);
		
		return true;
	}

	bool Get(T& t)
	{
		size_t rpos = rpos_.load(std::memory_order_relaxed);
		size_t wpos = wpos_.load(std::memory_order_acquire);
		if (rpos == wpos)
			return false;

		t = buffer_[rpos];
		rpos_.store(PMASK(rpos + 1), std::memory_order_release);

		return true;
	}

private:
	T* buffer_;
	size_t capacity_;
	std::atomic<size_t> rpos_;
	std::atomic<size_t> wpos_;
};

