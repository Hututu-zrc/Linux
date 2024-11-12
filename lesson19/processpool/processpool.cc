#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"
// C++11 新增了模板别名功能，可以用 using 给模板创建别名，这在定义复杂类型时尤其有用
using work_t = std::function<void()>;

enum
{
    OK = 0,
    Augment_Err,
    Pipe_Err,
    Fork_Err,
    Wait_Err
};

// 这个类，用来存储我们每次开辟的子进程的信息
// 不然，N次循环后，子进程无法使用了
class Channel
{
public:
    Channel(int wid, pid_t who) : _wid(wid), _who(who)
    {
        _name = "Channel " + std::to_string(wid)+"->"+std::to_string(who);
    }
    void SendTask(int taskcode)
    {
        ::write(_wid, &taskcode, sizeof(taskcode));
    }
    std::string GetName()
    {
        return _name;
    }
    int GetWid()
    {
        return _wid;
    }
    pid_t GetWho()
    {
        return _who;
    }
    ~Channel()
    {
    }

private:
    int _wid;          // 管道写端的pid
    std::string _name; // 对每个管道起一个名字
    pid_t _who;        // 子进程的pid
};

void Usage()
{
    std::cout << "augment" << " augment_num" << std::endl;
}

void Worker()
{
    // read->0
    //进程里面的每个进程都在从我们的管道里面读数据
    //父进程写入数据后，子进程就是管道里面读，看哪个读的到，就输出
    while (true)
    {
        int taskcode = 0;

        ssize_t n = ::read(0, &taskcode, sizeof(taskcode));
        if (n == sizeof(taskcode))
        {
            tm.ExecuteTask(taskcode);
        }
        else if (n == 0)
        {
        }
        else
        {
        }
        //sleep(1);
    }
}

void DebugChannel(std::vector<Channel> &channels)
{
    for (int i = 0; i < channels.size(); i++)
    {
        std::cout << channels[i].GetName() << "->" << channels[i].GetWid() << "->" << channels[i].GetWho() << std::endl;
    }
}
void InitProcessPool(const int &num, std::vector<Channel> &channels, work_t work)
{
    // 创建指定个数的子进程
    for (int i = 0; i < num; ++i)
    {
        // 创建管道
        int pfds[2] = {0};
        int n = ::pipe(pfds);
        if (n != 0)
        {
            std::cerr << "pipe" << std::endl;
            ::exit(Pipe_Err);
        }

        // 创建子进程
        int id = ::fork();
        if (id < 0)
        {
            std::cerr << "fork" << std::endl;
            ::exit(Fork_Err);
        }

        // 子进程用来读
        if (id == 0)
        {
            ::close(pfds[1]);

            // 改变文件描述符，使我们的Work读取命令的时候，直接从stdin里面读取
            ::dup2(pfds[0], 0);
            work();
            ::exit(OK);
        }

        // int status = 0;
        // int rid = ::waitpid(id, &status, 0);
        // if (rid < 0)
        // {
        //     std::cerr << "waitpid" << std::endl;
        //     exit(Wait_Err);
        // }

        // 父进程用来写
        //  使用上面定义的vector添加我们的每个管道
        channels.emplace_back(pfds[1], id);
        ::close(pfds[0]);
    }
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
    int num = std::stoi(argv[1]);
    std::vector<Channel> channels; // 用来记录每个管道

    // 1、初始化进程池
    InitProcessPool(num, channels, Worker);
    // DebugChannel(channels);

    // 2、派发任务
    int taskcode = 0;
    int who = 0;
    while (true)
    {

        // a、选择一个任务码
        taskcode = rand() % number;

        // b、选择一个管道
        Channel &chl = channels[who++];
        who %= channels.size();

        // c、分发任务
        // tm.ExecuteTask(taskcode);
        chl.SendTask(taskcode);
        cout<<chl.GetName()<<endl;
        sleep(2);
    }

    return 0;
}