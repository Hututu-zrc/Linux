#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <string.h>
using std::cerr;
using std::cout;
using std::endl;

std::string  ToHex(pthread_t a)
{
    char buff[64];
    snprintf(buff,sizeof(buff)/sizeof(char),"0x%lx",a);
    return buff;
}

class ThreadData
{
public:
    ThreadData(std::string name ,int a=0,int b=0,int result=0)
    :_name(name),
    _a(a),
    _b(b),
    _result(a+b)
    {}
    void execute()
    {
        _result=_a+_b;
    }
    std::string GetName()
    {
        return _name;
    }
    int GetResult()
    {
        return _result;
    }
private:
    std::string _name;
    int _a;
    int _b;
    int _result;
};

void *rountine(void *args)
{
    ThreadData *td=static_cast<ThreadData*> (args);
    td->execute();
    sleep(1);
    cout<<"sub thread id: "<< ToHex(pthread_self())<<endl;
    return (void*)td;
}

int main()
{
    pthread_t tid;
    ThreadData *td = new ThreadData("thread_1",10,20);
    pthread_create(&tid, nullptr, rountine, (void *)td);
    int cnt=5;
    while(cnt--)
    {
        cout<<"main thread id:"<< ToHex(pthread_self())<<endl;
        sleep(1);
    }
    ThreadData * rtd=nullptr;
    int n =pthread_join(tid,(void**)&rtd);
    if(n!=0)
    {
        cout<<"thread exit fail!"<<endl;
        cout<<"exit errno:"<<n<<"error result"<<strerror(n)<<endl;
        return 0;
    }
    cout<<"result:"<<rtd->GetResult()<<endl;
    return 0;
}

// //创建的新线程
// void* run(void * args)
// {
//     std::string name =static_cast<const char *> (args);
//     while(true)
//     {
//         cout<<"new thread,pid:"<<gettid()<<endl;
//         cout <<"thread name :"<<name <<endl;
//         sleep(2);
//     }
//     return nullptr;
// }

// int main()
// {
//     cout<<"i am a process:"<<getpid()<<endl;
//     pthread_t tid;
//     pthread_create(&tid,nullptr,run,(void*)"thread-1");

//     //我们的主线程
//     while(true)
//     {
//         cout<<"main thread,pid:"<<gettid()<<endl;
//         sleep(1);
//     }
//     return 0;
// }