#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
// 这个类，用来存储我们每次开辟的子进程的信息
// 不然，N次循环后，子进程无法使用了
class Channel
{
public:
    Channel(int wid, pid_t who) : _wid(wid), _who(who)
    {
        _name = "Channel " + std::to_string(wid) + "->" + std::to_string(who);
    }
    void SendTask(int taskcode)
    {
        ::write(_wid, &taskcode, sizeof(taskcode));
    }
    void Close()
    {
        ::close(_wid);
    }
    std::string GetName()
    {
        return _name;
    }
    int GetWid()
    {
        return _wid;
    }
    pid_t GetSubPid()
    {
        return _who;
    }
    ~Channel()
    {
    }

private:
    int _wid;          // 管道写端的pid
    std::string _name; // 对每个管道起一个名字
    // 我们将读端直接重定向到stdin，所以这里不需要管道读端的pid
    pid_t _who; // 子进程的pid
};

#endif