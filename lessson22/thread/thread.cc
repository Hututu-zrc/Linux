#include<iostream>
#include <unistd.h>
#include <pthread.h>

using std::cout;
using std::endl;
using std::cerr;

//创建的新线程
void* run(void * args)
{
    while(true)
    {
        cout<<"new thread,pid:"<<getpid()<<endl;
        sleep(2);
    }
    return nullptr;
}

int main()
{
    cout<<"i am a process:"<<getpid()<<endl;
    pthread_t tid;
    pthread_create(&tid,nullptr,run,(void*)"thread-1");
    
    //我们的主线程
    while(true)
    {
        cout<<"main thread,pid:"<<getpid()<<endl;
        sleep(1);
    }
    return 0;
}