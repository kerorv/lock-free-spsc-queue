#include <iostream>
#include <thread>
#include <chrono>
#include "circlebuffer1.h"
#include "circlebuffer2.h"
#include "circlebuffer3.h"
#include "queue1.h"
#include "queue2.h"
#include "queue3.h"

#define TEST_COUNT	10000000

template <class T>
void Product(T& c)
{
    for (int x = 0; x < TEST_COUNT; )
    {
        if (c.Put(x))
            x++;
    }
}

template <class T>
void Consume(T& c)
{
    int x;
    for (int y = 0; y < TEST_COUNT; )
    {
        if (c.Get(x))
        {
            assert(x == y);
            y++;
        }
    }
}

template <class T>
void SpscTest(T& c)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    start = std::chrono::high_resolution_clock::now();
    std::thread product(Product<T>, std::ref(c));
    std::thread consume(Consume<T>, std::ref(c));
    product.join();
    consume.join();
    end = std::chrono::high_resolution_clock::now();

    std::cout << "const time: " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << "(ms)."
        << std::endl;
}

int main(int argc, char* argv[])
{
	CircleBuffer1<int> cb1(1024);
    CircleBuffer2<int> cb2(1024);
    CircleBuffer3<int> cb3(1024);

    Queue1<int> q1;
    Queue2<int> q2;
    Queue3<int> q3;

    std::cout << "CircleBuffer1 ";
    SpscTest(cb1);
    std::cout << "CircleBuffer2 ";
    SpscTest(cb2);
    std::cout << "CircleBuffer3 ";
    SpscTest(cb3);
   
    std::cout << "Queue1 ";
    SpscTest(q1);
    std::cout << "Queue2 ";
    SpscTest(q2);
    std::cout << "Queue3 ";
    SpscTest(q3);
    
	return 0;
}

