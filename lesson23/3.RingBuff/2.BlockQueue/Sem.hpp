#pragma once
#include <semaphore.h>

namespace SemModule
{
    int defaultvalue = 1;
    class Sem
    {
    public:
        Sem(int value = defaultvalue):_init_value(value)
        {
            ::sem_init(&_sem, 0, _init_value);
            
        }
        void P()
        {
            ::sem_wait(&_sem); // p操作
        }

        void V()
        {
            ::sem_post(&_sem); // v操作
        }
        ~Sem()
        {
            ::sem_destroy(&_sem);
        }

    private:
        int _init_value;

        sem_t _sem;
    };
}