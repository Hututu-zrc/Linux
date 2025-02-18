#include "Thread.hpp"
#include <unordered_map>
#include<memory>


using namespace ThreadModule;
using std::cerr;
using std::cout;
using std::endl;


void Run(int a)
{
    while (a--)
    {
        cout << "hello world" << endl;
        sleep(1);
    }
}


//如果是多参数的问题，我们可以定义一个类，类里面放入我们的参数
//这样相对于CPP里面的多参数，更好使用和维护

//单参数的传入问题
int main()
{
    Thread<int > td(Run ,10);
    td.Start();
    cout<<td.GetName()<<endl;
    td.Join();
    return 0;
}

// //我们封装成为类的原因就是为了去配套使用CPP的模板
// //多线程

// #define NUM 10
// using thread_ptr_t=std::shared_ptr<Thread>;

// int main()
// {
//     //使用哈希结构将线程的名字和线程的指针管理起来
//     //这里使用的是智能指针
//     std::unordered_map<std::string ,thread_ptr_t>threads;

//     //创建进程并且放入哈希结构当中
//     for(int i=0;i<NUM;++i)
//     {
//         thread_ptr_t td=std::make_shared<Thread> (Run);//创建智能指针
//         threads[td->GetName()]=td;
        
//     }

//     //使用进程
//     for(auto & e:threads)
//     {
//         e.second->Start();
//     }

//     for(auto & e:threads)
//     {
//         e.second->Join();
//     }
//     return 0;
// }

// int main()
// {
//     Thread td(Run);
//     td.Start();
//     cout << td.GetName() << endl;
//     // td.Detach();
//     // cout << td.GetName() << endl;
//     // if (td.Join())
//     //     cout << "pthread_join is success!" << endl;
//     // else
//     //     cerr << "pthread_join is success!" << endl;
//     return 0;
// }