#pragma once
#include <pthread.h>
#include <vector>
#include <unordered_map>
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

    template <typename T>
    class Threadpool
    {

    private:
        bool IsEmpty()
        {
            return _tasks.empty();
        }
        void ExecuteTask(std::string name)
        {

            while (true)
            {
                // 1.拿到任务
                // 访问了临界资源任务队列，需要加锁
                sleep(1);
                T t;

                // 这里的锁在{}当中，出了这个作用域就会被销毁了
                {
                    LockGuard lock(_mutex);

                    // 任务队列不为空 && 线程池处于进行状态
                    // 此时线程池正常运行
                    while (IsEmpty() && _isrunning)
                    {

                        _wait_num++;
                        _cond.Wait(_mutex);
                        _wait_num--;
                    }

                    if (IsEmpty() && !_isrunning)
                    {
                        LOG(LogLevel::DEBUG) << "threadpool closed";
                        break;
                    }

                    // 如果线程池为空 && 线程池处于关闭状态，此时跳出循环
                    // 准备回收线程

                    LOG(LogLevel::DEBUG) << name << " is running";

                    t = _tasks.front();
                    _tasks.pop();
                }
                // 2.处理任务
                // 处理任务自己处理就好了，不需要再加锁期间处理
                t();
            }
        }


        //单例模式不允许赋值和拷贝
        Threadpool(const Threadpool<T> & pool)=delete;
        Threadpool<T>& operator=(const Threadpool<T> & pool)=delete;

        //单例模式还是需要构造函数初始化的
        Threadpool(int num = defaultnum) : _thread_num(num),
                                           _wait_num(0),
                                           _isrunning(false)
        {
            for (int i = 0; i < _thread_num; ++i)
            {
                _threads.push_back(std::make_shared<Thread>(std::bind(&Threadpool::ExecuteTask, this, std::placeholders::_1)));
            }
        }
    public:
        
        static Threadpool<T>* CreateSingleThreadPool()
        {
            if(_instance==nullptr)
            {
                LockGuard lcokguard(_mutex);
                if(_instance==nullptr)
                {
                    _instance=new Threadpool<T> ();
                    _instance->Start();
                }
            }
            return _instance;
        }
        void Equeue(T &&t) // 任务进入队列的函数
        {
            // 这个地方要访问临界资源，所以要加锁保护
            LockGuard lock(_mutex);
            if(_isrunning==false)
                return ;
            _tasks.push(move(t));

            if (_wait_num > 0)
            {
                _cond.Notify();
            }
        }
        void Start() // 线程池启动函数
        {
            LockGuard lock(_mutex);

            if (_isrunning)
                return;
            //这里如果不加锁 && _isrunning 放到最后的话，就会产生问题
            //因为线程是并发跑的，可能前一个线程刚启动的时候，就直接执行ExecuteTask
            //但是ExecuteTask里面有if (IsEmpty() && !_isrunning) 就直接退出掉这个进程了
            //解决办法：1、加锁  2、将_isrunning=true 放到if (_isrunning)后面
            _isrunning = true;


            for (auto &e : _threads)
            {
                e->Start();
                LOG(LogLevel::DEBUG) << e->GetName() << " is start";
            }



        }
        void Wait() // 线程池等待函数
        {
            for (auto &e : _threads)
            {
                e->Join();
                LOG(LogLevel::DEBUG) << e->GetName() << " is stop";
            }
        }
        void Stop() // 线程池停止函数
        {
            // 线程池停止我们必须保证
            // 1.任务队列里面的任务全部都进行完了
            // 2.将所有的休眠进程都唤醒

            LockGuard lock(_mutex);

            if (!_isrunning) // 关闭状态就退出
            {
                return;
            }
            _isrunning = false;
            if (_wait_num > 0)
            {
                _cond.Notify();
            }
        }
        ~Threadpool()
        {
        }
    private:
        int _thread_num;                               // 线程池里面的线程的个数
        int _wait_num;                                 // 有多少个线程在等待
        bool _isrunning;                               // 线程池的启动状态，默认是关闭
        std::vector<std::shared_ptr<Thread>> _threads; // 线程存放的地方

        // 判断一个地方要不要使用锁，核心就是盖地方要不要访问临界资源
        std::queue<T> _tasks; // 任务队列 临界资源（多线程访问）
        Mutex _mutex;
        Cond _cond;
        static Threadpool<T> * _instance;

        
    };
    
    template <typename T>
    Threadpool<T> * Threadpool<T>::_instance=nullptr;
}