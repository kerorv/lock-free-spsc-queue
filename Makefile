objects = main.o

mq: $(objects)
	g++ -o mq $(objects) -pthread

main.o: main.cpp circlebuffer1.h circlebuffer2.h circlebuffer3.h queue1.h queue2.h queue3.h
	g++ -std=c++0x -c main.cpp

clean:
	rm *.o mq

