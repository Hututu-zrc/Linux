#pragma once
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <semaphore.h>


template <typename T>
class BlockQueue
{
public:
    BlockQueue(int cap) : _cap(cap)
    {
    }
    bool empty()
    {

        return _q.empty();
    }
    bool isfull()
    {

        return _cap <= _q.size();
    }
    void Enqueue(const T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (isfull())
        {
            _provider.wait(lock);
        }

        // 队列没有满
        _q.push(value);
        _consumer.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (empty())
        {
            _consumer.wait(lock);
        }
        T t = _q.front();

        _q.pop();
        _provider.notify_one();
        return t;
    }
    ~BlockQueue()
    {
    }

private:
    int _cap;
    std::queue<T> _q;
    std::mutex _mutex;

    std::condition_variable _provider;
    std::condition_variable _consumer;
    
};