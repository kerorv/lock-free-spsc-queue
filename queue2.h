#pragma once

#include <cassert>
#include <atomic>

template <class T>
class Queue2
{
public:
	Queue2(size_t init_blocks = 2, size_t block_size = 512)
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

	~Queue2()
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
			wnode_.store(next);
			wpos_.store(0);
		}
		else
		{
			wpos_.store(wpos);
		}

		return true;
	}

	bool Get(T& t)
	{
		size_t rpos = rpos_.load();
		Node* rnode = rnode_.load();
		size_t wpos = wpos_.load();
		Node* wnode = wnode_.load();
		if (rnode == wnode && rpos == wpos)
			return false;

		t = rnode->block[rpos];
		rpos++;
		if (rpos >= block_size_)
		{
			rnode_.store(rnode->next);
			rpos_.store(0);
		}
		else
		{
			rpos_.store(rpos);
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
        Node* rnode = rnode_.load();
        Node* fnode = fnode_.load();
        if (rnode == fnode)
        {
            node = MakeNode();
        }
        else
        {
            node = fnode;
            fnode = fnode->next;
            node->next = 0;
            fnode_.store(fnode);
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

