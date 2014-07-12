#pragma once

#include <cassert>
#include <atomic>

template <class T>
class CircleBuffer2
{
public:
	CircleBuffer2(size_t capacity)
		: rpos_(0)
		, wpos_(0)
	{
		assert(capacity != 0 && (capacity & (capacity -1)) == 0);
		buffer_  = (T*)malloc(capacity * sizeof(T));
		capacity_ = capacity;
	}

	~CircleBuffer2()
	{
		free(buffer_);
	}

#define PMASK(N) ((N) & (capacity_ - 1))

	bool Put(const T& t)
	{
		size_t rpos = rpos_.load();
		size_t wpos = wpos_.load();
		size_t npos = PMASK(wpos + 1);
		if (rpos == npos)
			return false;

		buffer_[wpos] = t;
		wpos_.store(npos);
		
		return true;
	}

	bool Get(T& t)
	{
		size_t rpos = rpos_.load();
		size_t wpos = wpos_.load();
		if (rpos == wpos)
			return false;

		t = buffer_[rpos];
		rpos_.store(PMASK(rpos + 1));

		return true;
	}

private:
	T* buffer_;
	size_t capacity_;
	std::atomic<size_t> rpos_;
	std::atomic<size_t> wpos_;
};

