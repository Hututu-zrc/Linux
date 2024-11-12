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
    void InsertTask(task_t t)
    {
        _tasks[number++] = t;
    }
   
    int SelectTask()
    {
        return rand()%number;
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
    ~TaskManage()
    {
    }

private:
    std::unordered_map<int, task_t> _tasks;
};

TaskManage tm;