#pragma once
#include <pthread.h>

namespace MutexModule
{
    class Mutex
    {

    public:
        // 需要注意的是锁是不能被拷贝的
        Mutex(const Mutex &mutex) = delete;
        const Mutex &operator=(const Mutex &) = delete;
        Mutex()
        {
            pthread_mutex_init(&_lock, nullptr);
        }
        void lock()
        {
            pthread_mutex_lock(&_lock);
        }
        void unlock()
        {
            pthread_mutex_unlock(&_lock);
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&_lock);
        }

    private:
        pthread_mutex_t _lock;
    };

    class LockGuard
    {
    public:
        LockGuard(MutexModule::Mutex &mutex) : _mutex(mutex)
        {
            _mutex.lock();
        }
        ~LockGuard()
        {
            _mutex.unlock();
        }

    private:
        MutexModule::Mutex & _mutex;
    };

}
