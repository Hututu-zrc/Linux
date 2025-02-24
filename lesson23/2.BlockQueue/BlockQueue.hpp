#pragma once

#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <queue>
#include "Mutex.hpp"
#include "Cond.hpp"
#define MAXNUM 10;
// 这里写的是生产消费者模型里面的阻塞队列
// 采用的是stl里面的队列结构
// 阻塞队列属于是生产者和消费者的中间缓冲区

//--version2
namespace BlockQueueModule
{
    using namespace MutexModule;
    using namespace CondModule;
    template <typename T>
    class BlockQueue
    {

    public:
        BlockQueue(int cap = 10) : _cap(cap), _pnum(0), _cnum(0)
        {
           
        }
        bool IsFull()
        {
            return _cap == _q.size();
        }

        bool IsEmpty()
        {
            return _q.empty();
        }
        void Equeue(const T &data) // 输入型参数：将外部变量存入队列当中
        {

           LockGuard lock(_mutex);
            // 1.这里的等待在临界区里面是必然的（目前），因为判断条件就是对临界资源的访问
            //  这里要进行判断，如果达到队列的上限就等待

            // 5.对于这里的条件判断，需要改成while循环
            // 因为在等待后会释放锁，会造成" 伪唤醒 "
            // 就是其他进程抢占锁，并且使用了原本进程所需要的资源，导致该进程不按照理想状态运行

            while (IsFull())
            {
                _pnum++;
                // 2.这里的等待就是释放互斥锁
                _productor.Wait(_mutex);
                // 3.返回以后，线程被唤醒然后开始重新竞争锁（此时会在临界区代码里面醒来）
                _pnum--;
            }
            // 4.到这段代码，只有两种可能
            // if(isfull())没满足 || 线程等待后被唤醒
            _q.push(data);

            // 这里我们数据入队列以后，就可以直接唤醒消费者
            if (_q.size() > 0)
            {
                _consumer.Notify();
            }

            
        }
        void Pop(T *data) // 输出型参数：将内部变量拿出来放到外部变量当中
        {
            // 这里要进行判断，如果队列为空就要等待
           LockGuard lock(_mutex);

            while (IsEmpty())
            {
                _cnum++;
                _consumer.Wait(_mutex);
                _cnum--;
            }
            *data = _q.front();
            _q.pop();

            // 这里我们数据出队列以后，就可以直接唤醒生产者
            if (_q.size() < _cap)
            {
                _productor.Notify();
            }

        }
        ~BlockQueue()
        {
            
        }

    private:
        std::queue<T> _q;          // 队列
        int _cap;                  // 队列里面最大数据的长度
        Mutex _mutex;    // 互斥锁
        Cond _consumer;  // 消费者的条件变量：队列不是空就可以消费
        Cond _productor; // 生产者的条件变量：队列是空就可以生产，但是不能超过队列的最大值

        // 下面的两个计数的作用主要是为了多生产者和多消费者模型
        int _pnum; // 生产者信号量
        int _cnum; // 消费者信号量
    };
}


////-version1
// #include <unistd.h>
// #include <pthread.h>
// #include <iostream>
// #include <string>
// #include <queue>
// #define MAXNUM 10;
// // 这里写的是生产消费者模型里面的阻塞队列
// // 采用的是stl里面的队列结构
// // 阻塞队列属于是生产者和消费者的中间缓冲区
// namespace BlockQueueModule
// {
//     template <typename T>
//     class BlockQueue
//     {

//     public:
//         BlockQueue(int cap = 10) : _cap(cap), _pnum(0), _cnum(0)
//         {
//             pthread_mutex_init(&_mutex, nullptr);
//             pthread_cond_init(&_consumer, nullptr);
//             pthread_cond_init(&_productor, nullptr);
//         }
//         bool IsFull()
//         {
//             return _cap == _q.size();
//         }

//         bool IsEmpty()
//         {
//             return _q.empty();
//         }
//         void Equeue(const T &data) // 输入型参数：将外部变量存入队列当中
//         {

//             pthread_mutex_lock(&_mutex);
//             // 1.这里的等待在临界区里面是必然的（目前），因为判断条件就是对临界资源的访问
//             //  这里要进行判断，如果达到队列的上限就等待

//             // 5.对于这里的条件判断，需要改成while循环
//             // 因为在等待后会释放锁，会造成" 伪唤醒 "
//             // 就是其他进程抢占锁，并且使用了原本进程所需要的资源，导致该进程不按照理想状态运行

//             while (IsFull())
//             {
//                 _pnum++;
//                 // 2.这里的等待就是释放互斥锁
//                 pthread_cond_wait(&_productor, &_mutex);
//                 // 3.返回以后，线程被唤醒然后开始重新竞争锁（此时会在临界区代码里面醒来）
//                 _pnum--;
//             }
//             // 4.到这段代码，只有两种可能
//             // if(isfull())没满足 || 线程等待后被唤醒
//             _q.push(data);

//             // 这里我们数据入队列以后，就可以直接唤醒消费者
//             if (_q.size() > 0)
//             {
//                 pthread_cond_signal(&_consumer);
//             }

//             pthread_mutex_unlock(&_mutex);
//         }
//         void Pop(T *data) // 输出型参数：将内部变量拿出来放到外部变量当中
//         {
//             pthread_mutex_lock(&_mutex);
//             // 这里要进行判断，如果队列为空就要等待

//             while (IsEmpty())
//             {
//                 _cnum++;
//                 pthread_cond_wait(&_consumer, &_mutex);
//                 _cnum--;
//             }
//             *data = _q.front();
//             _q.pop();

//             // 这里我们数据出队列以后，就可以直接唤醒生产者
//             if (_q.size() < _cap)
//             {
//                 pthread_cond_signal(&_productor);
//             }

//             pthread_mutex_unlock(&_mutex);
//         }
//         ~BlockQueue()
//         {
//             pthread_mutex_destroy(&_mutex);
//             pthread_cond_destroy(&_consumer);
//             pthread_cond_destroy(&_productor);
//         }

//     private:
//         std::queue<T> _q;          // 队列
//         int _cap;                  // 队列里面最大数据的长度
//         pthread_mutex_t _mutex;    // 互斥锁
//         pthread_cond_t _consumer;  // 消费者的条件变量：队列不是空就可以消费
//         pthread_cond_t _productor; // 生产者的条件变量：队列是空就可以生产，但是不能超过队列的最大值

//         // 下面的两个计数的作用主要是为了多生产者和多消费者模型
//         int _pnum; // 生产者信号量
//         int _cnum; // 消费者信号量
//     };
// }