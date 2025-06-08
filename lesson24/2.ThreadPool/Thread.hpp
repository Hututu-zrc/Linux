#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <functional>

// v1
namespace ThreadModule
{
    static int count = 0;
    enum ThreadStauts
    {
        NEW,
        RUNNING,
        STOP
    };
    using func_t = std::function<void(std::string)>;
    class Thread
    {
    private:
        // 由于这里Rouine是类内函数，默认是隐藏需要一个this指针的
        // 这里为了避免使用this指针，使用静态成员函数
        static void *Routine(void *args) // 用来调用用户传入的函数
        {
            Thread *t = static_cast<Thread *>(args);
            t->_func(t->_name);
            t->_status = RUNNING;
            return nullptr;
        }

        void EnableDetach() // 更改线程是否可以分离
        {
            _joinable = false;
        }

    public:
        Thread(func_t func) : _joinable(true), _func(func), _status(NEW)
        {
            _name = "Thread_" + std::to_string(count++);
            _pid = ::getpid();
        }
        bool Start()
        {
            if (_status != RUNNING)
            {
                int n = ::pthread_create(&_tid, nullptr, Routine, this); // 这里传入this指针参数
                if (n != 0)
                    return false;
                return true;
            }
            return false;
        }
        bool Stop()
        {
            if (_status == RUNNING)
            {
                int n = ::pthread_cancel(_tid);
                if (n != 0)
                    return false;
                _status = STOP;
                return true;
            }
        }
        bool Join()
        {
            if (_joinable)
            {
                int n = ::pthread_join(_tid, nullptr);
                if (n != 0)
                    return false;
                _status = STOP;
                return true;
            }
            return false;
        }
        void Detach()
        {
            EnableDetach();
            ::pthread_detach(_tid);
        }
        bool IsJoinable() // 返回_joinable的属性
        {
            return _joinable;
        }

        std::string GetName()
        {
            return _name;
        }
        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        pid_t _pid;
        bool _joinable;       // 用来判断该线程是否是分离状态，默认不是分离状态
        func_t _func;         // 用来存储我们线程使用的函数
        ThreadStauts _status; // 用来保存我们线程的状态
    };
}

// // v2 可以传入单参数的版本
// namespace ThreadModule
// {
//     static int count = 0;
//     enum ThreadStauts
//     {
//         NEW,
//         RUNNING,
//         STOP
//     };

//     template <typename T>
//     class Thread
//     {
//     private:
//         // 由于这里Rouine是类内函数，默认是隐藏需要一个this指针的
//         // 这里为了避免使用this指针，使用静态成员函数
//         static void *Routine(void *args) // 用来调用用户传入的函数
//         {
//             Thread *t = static_cast<Thread *>(args);
//             t->_func(t->_data);
//             t->_status = RUNNING;
//             return nullptr;
//         }

//         void EnableDetach() // 更改线程是否可以分离
//         {
//             _joinable = false;
//         }

//     public:
//         using func_t = std::function<void(T)>; // 此时这个需要放在模板内部，这样两个模板就都是一样的
//         Thread(func_t func, T data) : _joinable(true), _func(func), _status(NEW), _data(data)
//         {
//             _name = "Thread_" + std::to_string(count++);
//             _pid = ::getpid();
//         }
//         bool Start()
//         {
//             if (_status != RUNNING)
//             {

//                 int n = ::pthread_create(&_tid, nullptr, Routine, this); // 这里传入this指针参数
//                 if (n != 0)
//                     return false;
//                 return true;
//             }
//             return false;
//         }
//         bool Stop()
//         {
//             if (_status == RUNNING)
//             {
//                 int n = ::pthread_cancel(_tid);
//                 if (n != 0)
//                     return false;
//                 _status = STOP;
//                 return true;
//             }
//             return false;
//         }
//         bool Join()
//         {
//             if (_joinable)
//             {
//                 int n = ::pthread_join(_tid, nullptr);
//                 if (n != 0)
//                     return false;
//                 _status = STOP;
//                 return true;
//             }
//             return false;
//         }
//         void Detach()
//         {
//             EnableDetach();
//             ::pthread_detach(_tid);
//         }
//         bool IsJoinable() // 返回_joinable的属性
//         {
//             return _joinable;
//         }

//         std::string GetName()
//         {
//             return _name;
//         }
//         ~Thread()
//         {
//         }

//     private:
//         std::string _name;
//         pthread_t _tid;
//         pid_t _pid;
//         bool _joinable;       // 用来判断该线程是否是分离状态，默认不是分离状态
//         func_t _func;         // 用来存储我们线程使用的函数
//         ThreadStauts _status; // 用来保存我们线程的状态
//         T _data;
//     };
// }
