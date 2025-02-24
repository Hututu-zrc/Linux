#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"
#include "Channel.hpp"
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

class ProcessPool
{

public:
    ProcessPool(int num, work_t w)
        : _num(num),
          _w(w)
    {
    }
    ~ProcessPool() {}
    void DebugChannel()
    {
        for (size_t i = 0; i < _channels.size(); i++)
        {
            std::cout << _channels[i].GetName() << "->" << _channels[i].GetWid() << "->" << _channels[i].GetSubPid() << std::endl;
        }
    }
    void InitProcessPool()
    {
        // 创建指定个数的子进程
        for (int i = 0; i < _num; ++i)
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
                // 我们使用vector存储的管道，每个子进程开辟之前，
                //(注意，我们现在准备修改的是子进程的文件描述符表)如果里面存储有管道，就遍历每一个管道，将管道里面的写端全部关闭掉
                // 假设前面管道的写端是6，父进程的文件描述符表和子进程的文件描述表里面，该管道的写端都是6号
                // 但是我们现在是子进程的代码段，使用的子进程的数据段的拷贝副本，所以，这里关掉的是子进程的文件描述符表里面的写端，不会影响到父进程

                // std::cout << getpid() << ", child close history fd: ";
                if (_channels.size() != 0)
                {
                    // 这里面的for循环，i<_channels.size()就可以了，不需要减1
                    for (size_t i = 0; i < _channels.size(); i++)
                    {
                        // std::cout << _channels[i].GetWid() << " ";
                        _channels[i].Close();
                    }
                }
                // std::cout << " over" << std::endl;

                ::close(pfds[1]);

                // 改变文件描述符，使我们的Work读取命令的时候，直接从stdin里面读取
                ::dup2(pfds[0], 0);
                _w();
                ::exit(OK);
            }

            // 父进程用来写
            //  使用上面定义的vector添加我们的每个管道
            _channels.emplace_back(pfds[1], id);
            ::close(pfds[0]);
        }
    }

    void DispatchTask()
    {
        int taskcode = 0;
        int who = 0;

        int cnt = 10;
        while (cnt--)
        {

            // a、选择一个任务码
            taskcode = tm.SelectTask();

            // b、选择一个管道(也就是选择了一个子进程),这里采用的方式是轮询，就是轮着来
            Channel &chl = _channels[who++];
            who %= _channels.size();

            // c、分发任务
            chl.SendTask(taskcode);
            cout << chl.GetName() << endl;
            sleep(1);
        }
    }

    void ClearProcessPool()
    {

        // version_3
        // 如果我们一定要从前往后关闭的话，也有方法
        // 详细请看上面的子进程的代码块

        for (auto &c : _channels)
        {
            c.Close();
            int status = 0;
            int rid = ::waitpid(c.GetSubPid(), &status, 0);
            if (rid < 0)
            {
                std::cerr << "waitpid" << std::endl;
                exit(Wait_Err);
            }
            cout << "sub process " << rid << " exit success" << endl;
        }

        // // version_2
        // // 这里会产生问题，程序会一直阻塞式等待
        // // 原因分析：我们创建管道的时候，主程序关掉读端，子进程关掉写端
        // // 但是多个管道的时候，主进程分配的还是3号读端，和x号写端
        // // 然后子进程拷贝为副本的时候，上个管道的写端被拷贝下来了，每创建一个管道就写端就会重复
        // // 然后我们从第一个管道开始关闭写端的时候，同一管道有多个写端，关闭一个没有用，程序就会一直阻塞式等待
        // // 解决方法一就是从最后一个管道开始关闭掉写端，反着向前关闭
        // // 当我们开始关闭掉最后一个管道的写端的时候，对应的子进程的文件描述符表里面有上一个管道的写端
        // // 但是，我们关闭掉最后一个管道的写端，对应的子进程也就被回收了，文件描述符表也就没有了
        // // 所以，上一个管道的写端就少了一个。
        // // 我们从后往前关闭，就不会因为管道的多写端而产生问题，第一个管道的写端是最多的

        // //这个地方还不能使用size_t,因为最后减成-1的时候，会变成很大很大的数
        // for (int i=_channels.size()-1;i>=0;--i)
        // {
        //     _channels[i].Close();
        //     int status = 0;
        //     int rid = ::waitpid(_channels[i].GetSubPid(), &status, 0);
        //     if (rid < 0)
        //     {
        //         cout<<"i:"<<i<<endl;
        //         std::cerr << "waitpid" << std::endl;
        //         exit(Wait_Err);
        //     }
        //     cout << "sub process " << rid << " exit success" << endl;
        // }

        // //version_1
        // //这里我们清理管道写端的时候，使用了两个for循环分开清理
        // //这里不会产生问题
        // for (auto &c : _channels)
        // {
        //     c.Close();
        // }
        // for (auto &c : _channels)
        // {
        //     int status = 0;
        //     int rid = ::waitpid(c.GetSubPid(), &status, 0);
        //     if (rid < 0)
        //     {
        //         std::cerr << "waitpid" << std::endl;
        //         exit(Wait_Err);
        //     }
        //     cout<<"sub process "<<rid<< " exit success"<<endl;
        // }
    }

private:
    std::vector<Channel> _channels;
    int _num;
    work_t _w;
    /* data */
};
