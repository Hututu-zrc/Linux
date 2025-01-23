#pragma once

#include <iostream>
#include <string>
#include <functional>
namespace ThreadModule
{
    static int count = 0;
    enum ThreadStauts
    {
        NEW,
        RUNNING,
        STOP
    };
    using func_t=std::function<void()>;
    class Thread
    {
    public:
        Thread()
        {
            _name = "Thread_" + std::to_string(count++);
        }
        void Start()
        {
        }
        void Stop()
        {
        }
        void Join()
        {
        }
        void EnableDetach()
        {
        }

        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        pid_t _pid;
        bool _joinable;
        func_t  _func;
        ThreadStauts _status;
    };
}