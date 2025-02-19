#pragma once
#include <pthread.h>
#include <vector>
#include <iostream>
#include <queue>
#include <memory>
#include "Mutex.hpp"
#include "Cond.hpp"
#include "Thread.hpp"
#include "Log.hpp"
using namespace MutexModule;
using namespace LogModule;
using namespace CondModule;
using namespace ThreadModule;

namespace ThreadPoolModule
{
    const static int defaultnum = 5;
    void TestFunc()
    {
    }

    template <typename T>
    class Threadpool
    {

    private:
        bool IsEmpty()
        {
            return _tasks.empty();
        }
        void ExecuteTask()
        {

            while (true)
            {
                // 1.拿到任务
                // 访问了临界资源任务队列，需要加锁
                sleep(1);
                T t;
                {
                    LockGuard lock(_mutex);
                    while (IsEmpty())
                    {
                        _wait_num++;
                        _cond.Wait(_mutex);
                        _wait_num--;
                    }
                    t = _tasks.front();
                    _tasks.pop();
                }
                // 2.处理任务
                // 处理任务自己处理就好了，不需要再加锁期间处理
                t();
            }
        }

    public:
        Threadpool(int num = defaultnum) : _thread_num(num),
                                           _wait_num(0)
        {
            for (int i = 0; i < _thread_num; ++i)
            {
                _threads.push_back(std::make_shared<Thread>(std::bind(&Threadpool::ExecuteTask, this)));
            }
        }

        void Equeue(T &&t) // 任务进入队列的函数
        {
            // 这个地方要访问临界资源，所以要加锁保护
            LockGuard lock(_mutex);
            _tasks.push(move(t));



            if (_wait_num > 0)
            {
                _cond.Notify();
            }
        }
        void Start() // 线程池启动函数
        {
            for (auto &e : _threads)
            {
                e->Start();
            }
        }
        void Wait() // 线程池等待函数
        {
            for (auto &e : _threads)
            {
                e->Join();
            }
        }
        void Stop() // 线程池停止函数
        {
        }
        ~Threadpool()
        {
        }

    private:
        int _thread_num;
        int _wait_num;                                 // 有多少个线程在等待
        std::vector<std::shared_ptr<Thread>> _threads; // 线程存放的地方

        // 判断一个地方要不要使用锁，核心就是盖地方要不要访问临界资源
        std::queue<T> _tasks; // 任务队列 临界资源（多线程访问）
        Mutex _mutex;
        Cond _cond;
    };
}