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
        
        int cnt=10;
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
            sleep(2);
        }
    }

    void ClearProcessPool()
    {
        for (auto &c : _channels)
        {
            c.Close();
        }
        for (auto &c : _channels)
        {
            int status = 0;
            int rid = ::waitpid(c.GetSubPid(), &status, 0);
            if (rid < 0)
            {
                std::cerr << "waitpid" << std::endl;
                exit(Wait_Err);
            }
            cout<<"sub process "<<rid<< " exit success"<<endl;
        }
    }

private:
    std::vector<Channel> _channels;
    int _num;
    work_t _w;
    /* data */
};
