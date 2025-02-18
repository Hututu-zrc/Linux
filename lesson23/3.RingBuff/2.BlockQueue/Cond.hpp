#pragma once
#include <pthread.h>
#include <unistd.h>
#include "Mutex.hpp"
#include "Cond.hpp"
namespace CondModule
{
    class Cond
    {
    public:
        Cond()
        {
            pthread_cond_init(&_cond,nullptr);
        }
        void Wait(MutexModule:: Mutex &mutex)
        {
            pthread_cond_wait(&_cond,mutex.LockPtr());
        }
        void Notify()
        {
            pthread_cond_signal(&_cond);
        }
        void NotifyAll()
        {
            pthread_cond_broadcast(&_cond);
        }
        ~Cond()
        {
            pthread_cond_destroy(&_cond);
        }
    private:
        pthread_cond_t _cond;
    };
}