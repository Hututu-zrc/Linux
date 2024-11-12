#include "ProcessPool.hpp"
#include "Task.hpp"


void Usage()
{
    std::cout << "augment" << " augment_num" << std::endl;
}


int main(int argc, char *argv[])
{
    // 我们现在的程序就是master
    // 我们这argc<2,代表参数错误，要求命令行输入的第二个参数是我们进程池里面进程的个数
    if (argc < 2)
    {
        Usage();
        ::exit(1);
    }
    int num = std::stoi(argv[1]);  // 想要创建的子进程的个数
    std::vector<Channel> channels; // 用来记录每个管道

    // 1、初始化进程池
    ProcessPool *pp=new ProcessPool(num,Worker);
    pp->InitProcessPool();
    // DebugChannel(channels);

    // 2、派发任务
    pp->DispatchTask();

    // 3、清理线程池
    // 如果我们不关闭写端，管道的读端会一直阻塞式等待
    // 这里我们就是等待派发任务完成以后，我们关闭掉写端，读端会读到0，然后返回退出
    pp->ClearProcessPool();

    delete pp;

    return 0;
}