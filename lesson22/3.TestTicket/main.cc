#include "Thread.hpp"
#include "Mutex.hpp"
#include <vector>

using namespace ThreadModule;
int ticketnum = 1000;

// 这里是直接定义了一个全局的锁，属于共享资源
// 然后使用的是PTHREAD_MUTEX_INITIALIZER宏初始化;

// pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

// 1. 锁本身是全局的，那么锁也是共享资源
// pthread_mutex:加锁和解锁被设计成为原子的了 --- TODO??
// 2. 如何看待锁呢？二元信号量就是锁！
//    2.1 加锁本质就是对资源展开预订！
//    2.2 整体使用资源！！
// 3. 如果申请锁的时候，锁被别人已经拿走了，其他线程要进行阻塞等待
// 4. 线程在访问临界区代码的时候，可以不可以切换？？可以切换！！
//    4.1 我被切走的时候，别人能进来吗？？不能！我是抱着锁，被切换的！！不就是串行吗！效率低的原因！原子性！
// 5. 不遵守这个约定？？bug！

#define Num 4

class ThreadData
{
public:
    std::string name;
    pthread_mutex_t *mutex_ptr;
};

MutexMudole::Mutex mtx;


void Ticket(ThreadData &data) // 这里使用的是引用，所以Thread模板里面也必须是引用
{
    while (true)
    {
        //lockguard放在while循环里面
        //这个括号里面的就叫做临界区
        {
            //这个地方将原本的lock又封装了一层，可以使用lockguard的构造函数和析构函数自动上锁和关锁
            MutexMudole::LockGuard guard(mtx);
            // pthread_mutex_lock(data.mutex_ptr);
            if (ticketnum > 0)
            {
                usleep(1000);

                // 开始抢票的过程
                printf("Now ThreadName is %s and TicketNum is %d\n", data.name.c_str(), ticketnum--);

                // pthread_mutex_unlock(data.mutex_ptr);
                //  票入库的过程
                usleep(1000);
            }
            else
            {
                // pthread_mutex_unlock(data.mutex_ptr);
                break;
            }
        }
    }
}
int main()
{
    // 我们使用的其实就是二元信号量，所有的线程都用一个锁
    pthread_mutex_t mutex; // 局部的锁
    // 创建一个变量类传入我们的thread类里面
    pthread_mutex_init(&mutex, nullptr);
    std::vector<Thread<ThreadData>> threads;
    ThreadData datas[Num];
    for (int i = 0; i < Num; i++)
    {
        datas[i].mutex_ptr = &mutex;

        threads.emplace_back(Ticket, datas[i]);
        datas[i].name = threads[i].GetName();
    }
    for (int i = 0; i < Num; ++i)
    {
        threads[i].Start();
    }

    for (int i = 0; i < Num; ++i)
    {
        threads[i].Join();
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}