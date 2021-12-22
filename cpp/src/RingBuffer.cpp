/*
 * @Author: your name
 * @Date: 2021-05-04 15:09:40
 * @LastEditTime: 2021-05-04 15:11:34
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \practise\RingBuffer.cpp
 */
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <sys/time.h>
#include "RingBuffer.hpp"

class Test
{
public:
   Test(int id = 0, int value = 0)
   {
    this->id = id;
        this->value = value;
    sprintf(data, "id = %d, value = %d\n", this->id, this->value);
   }
   void display()
   {
    printf("%s", data);
   }
private:
   int id;
   int value;
   char data[128];
};

double getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}

RingBuffer<Test> queue(1 << 12);

#define N (10 * (1 << 20))

void produce()
{
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    unsigned int i = 0;
    while(i < N)
    {
        if(queue.push(Test(i % 1024, i)))
        {
       i++;
        }
    }
    gettimeofday(&end, NULL);
    double tm = getdetlatimeofday(&begin, &end);
    printf("producer tid=%lu %f MB/s %f msg/s elapsed= %f size= %u\n", pthread_self(), N * sizeof(Test) * 1.0 / (tm * 1024 * 1024), N * 1.0 / tm, tm, i);
}

void consume()
{
    sleep(1);
    Test test;
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    unsigned int i = 0;
    while(i < N)
    {
        if(queue.pop(test))
    {
       // test.display();
       i++;
    }
    }
    gettimeofday(&end, NULL);
    double tm = getdetlatimeofday(&begin, &end);
    printf("consumer tid=%lu %f MB/s %f msg/s elapsed= %f, size=%u \n", pthread_self(), N * sizeof(Test) * 1.0 / (tm * 1024 * 1024), N * 1.0 / tm, tm, i);
}

int main(int argc, char const *argv[])
{
    std::thread producer1(produce);
    std::thread consumer(consume);
    producer1.join();
    consumer.join();
    return 0;
}
