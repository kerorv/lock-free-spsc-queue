#pragma once

#include <cassert>
#include <atomic>

template <class T>
class Queue3
{
public:
	Queue3(size_t init_blocks = 2, size_t block_size = 512)
        : rpos_(0)
        , wpos_(0)
        , block_size_(block_size)
	{
        assert((init_blocks > 0) && (block_size > 0));

        Node* node = MakeNode();
        fnode_ = node;
        for (size_t i = 0; i < init_blocks - 1; ++i)
        {
           Node* next = MakeNode(); 
           node->next = next;
           node = next;
        }

        rnode_ = node;
        wnode_ = node;
	}

	~Queue3()
	{
        Node* node = fnode_.load();
        while (node)
        {
            Node* next = node->next;
            FreeNode(node);
            node = next;
        }
	}

	bool Put(const T& t)
	{
		size_t wpos = wpos_.load();
		Node* wnode = wnode_.load();
		wnode->block[wpos] = t;
		wpos++;
		if (wpos >= block_size_)
		{
			Node* next = ObtainNode();
			wnode->next = next;
			wnode_.store(next, std::memory_order_release);
			wpos_.store(0, std::memory_order_release);
		}
		else
		{
			wpos_.store(wpos, std::memory_order_release);
		}

		return true;
	}

	bool Get(T& t)
	{
		size_t rpos = rpos_.load(std::memory_order_relaxed);
		Node* rnode = rnode_.load(std::memory_order_relaxed);
		size_t wpos = wpos_.load(std::memory_order_acquire);
		Node* wnode = wnode_.load(std::memory_order_acquire);
		if (rnode == wnode && rpos == wpos)
			return false;

		t = rnode->block[rpos];
		rpos++;
		if (rpos >= block_size_)
		{
			rnode_.store(rnode->next, std::memory_order_release);
			rpos_.store(0, std::memory_order_release);
		}
		else
		{
			rpos_.store(rpos, std::memory_order_release);
		}

		return true;
	}

private:
    struct Node;
    Node* MakeNode()
    {
        Node* node;
        node = (Node*)malloc(sizeof(Node));
        node->next = 0;
        node->block = (T*)malloc(sizeof(T) * block_size_);
        return node;
    }

    void FreeNode(Node* node)
    {
        free(node->block);
        free(node);
    }

	Node* ObtainNode()
	{
		Node* node;
        Node* fnode = fnode_.load(std::memory_order_relaxed);
        Node* rnode = rnode_.load(std::memory_order_acquire);
        if (rnode == fnode)
        {
            node = MakeNode();
        }
        else
        {
            node = fnode;
            fnode = fnode->next;
            node->next = 0;
            fnode_.store(fnode, std::memory_order_relaxed);
        }

        return node;
	}

private:
	struct Node
	{
		struct Node* next;
		T* block;
	};

    const size_t block_size_;
	std::atomic<size_t> rpos_;
	std::atomic<size_t> wpos_;
	std::atomic<Node*> rnode_;
	std::atomic<Node*> wnode_;
	std::atomic<Node*> fnode_;
};

