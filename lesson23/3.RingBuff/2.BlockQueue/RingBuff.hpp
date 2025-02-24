#pragma once
#include <pthread.h>
#include "Sem.hpp"
#include "Mutex.hpp"
#include <vector>
using std::cerr;
using std::cout;
using std::endl;
namespace RingBuffModule
{
    using namespace MutexModule;
    using namespace SemModule;

    template <typename T>
    class RingBuff
    {
    public:
        RingBuff(int cap = 5) : buff(cap),
                                _cap(cap),
                                _c_pos(0),
                                _p_pos(0),
                                _data_sem(0),
                                _space_sem(cap)
        {
        }
        void Equeue(const T &in)
        {

            // 1、先申请信号量，data有空位，data信号量p操作

            // //{
            //     这里需要注意一个问题，就是我们的锁是放在申请信号量之前，还是之后？
            //     答案是放在信号量之后，这样可以是多个消费者或者生产者拿到信号量，然后等待分配锁，效率更高。
            //     更形象的比喻：放在信号量之后，就属于是买票看电影，然后到电影院排队
            //                 放在信号量之前，就属于是到了电影院排队，一个一个开始买票进去看电影

            // //}

            // cout<<"space P"<<endl;
            _space_sem.P();
            // cout<<"after space P"<<endl;

            {
                LockGuard lock(_p_mutex);
                buff[_p_pos] = in;
                _p_pos++;
                _p_pos %= _cap;
            }

            // space信号量p操作
            // cout<<"_data_sem v"<<endl;
            _data_sem.V();
        }

        void Pop(T *out)
        {
            // 现在没有加锁，属于是单生产者单消费者模型

            // std::cout<<"pop"<<std::endl;
            _data_sem.P();
            // std::cout<<"buff: "<<buff[_c_pos]<<std::endl;

            {
                LockGuard lock(_c_mutex);
                *out = buff[_c_pos];
                // std::cout<<"data: "<<*out<<std::endl;
                _c_pos++;
                _c_pos %= _cap;
            }
            _space_sem.V();
        }
        ~RingBuff()
        {
        }

    private:
        std::vector<T> buff; // 环形队列

        int _cap;       // 代表环形队列的容量
        int _c_pos;     // 消费者所处于环形队列当中的下标
        int _p_pos;     // 生产者所处于环形队列当中的下标
        Sem _data_sem;  // 数据信号量，代表有几个数据位置可以被使用
        Sem _space_sem; // 空间信号量，代表有几个空间可以被使用

        // 没有锁就是单生产者单消费者模型
        // 有锁就是多个生产者或者多个消费者竞争出来一个，然后进入环形队列里面
        Mutex _c_mutex; // 多生产多消费模型当中的多消费者锁
        Mutex _p_mutex; // 多生产多消费模型当中的多生产者锁
    };
}