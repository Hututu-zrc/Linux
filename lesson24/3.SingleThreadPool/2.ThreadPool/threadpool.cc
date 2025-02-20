#include "ThreadPool.hpp"
#include "Task.hpp"

using namespace ThreadPoolModule;

void Routine()
{
    LOG(LogLevel::DEBUG) << "this is a Routine";
}
int main()
{
    ENABLE_CONSOLE_LOG();
    Threadpool<task_t>::CreateSingleThreadPool()->Start();
    // Threadpool<task_t> pool;
    Threadpool<task_t>::CreateSingleThreadPool()->Start();
    int cnt = 10;
    while (cnt--)
    {
        // sleep(1);
        Threadpool<task_t>::CreateSingleThreadPool()->Equeue(Routine);
    }
    Threadpool<task_t>::CreateSingleThreadPool()->Stop();
    Threadpool<task_t>::CreateSingleThreadPool()->Wait();
    return 0;
}