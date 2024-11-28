#pragma once
#include <iostream>
#include <unordered_map>
#include <functional>
#include <ctime>
// 使用task_tdai代替我们的任务函数名
using task_t = std::function<void()>;

// 简化命名空间
using std::cout;
using std::endl;

// 记录我们的TaskManage里面有几个
static int number = 0;

void DownLoad()
{
    cout << "Download Task" << endl;
}

void Upload()
{
    cout << "Upload Task" << endl;
}

void Log()
{
    cout << "Log Task" << endl;
}

// 这个类就是用来任务管理
// 初始化任务，然后使用unordered_map存着
// 我们主要管理方式就是使用任务码
class TaskManage
{
public:
    TaskManage()
    {
        srand(time(nullptr));
        InsertTask(DownLoad);
        InsertTask(Upload);
        InsertTask(Log);
    }
    void InsertTask(task_t t) // 将我们的任务插入到我们的TaskManage类里面
    {
        _tasks[number++] = t;
    }

    int SelectTask() // 随机挑选任务，返回任务码
    {
        return rand() % number;
    }
    void ExecuteTask(int taskcode)
    {
        if (_tasks.find(taskcode) != _tasks.end())
        {
            _tasks[taskcode]();
        }
        else
        {
            std::cerr << "taskcode error" << endl;
        }
    }
    ~TaskManage() {}

private:
    std::unordered_map<int, task_t> _tasks;
};

TaskManage tm; // 创建一个全局的类对象，方便调用

void Worker()
{
    // read->0
    // 进程里面的每个进程都在从我们的管道里面读数据
    // 父进程写入数据后，子进程就是管道里面读，看哪个读的到，就输出
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
            break;
        }
        else
        {
        }
        // sleep(1);
    }
}