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
    Threadpool<task_t> pool;
    pool.Start();
    int cnt = 10;
    while (cnt--)
    {
        // sleep(1);
        pool.Equeue(Routine);
    }
    pool.Stop();
    pool.Wait();
    return 0;
}