#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>

const char *Root = "/";
const char *Path = "/dev/null";

void Daemon(bool ischdir, bool isclose)
{
    // 目的就是将服务器进程变成守护进程

    // 1、守护进程需要关闭某些信号
    ::signal(SIGCHLD, SIG_IGN);
    ::signal(SIGPIPE, SIG_IGN);
    // 2、创建父子进程，删除父进程
    if (fork() > 0)
        exit(0);

    // 3、使用setsid函数
    int _sid = setsid();

    // 4、看看是否需要将当前的CWD修改为根目录
    if (ischdir)
    {
        ::chdir(Root);
    }

    // 5、关闭输入输出，错误的提示

    if (isclose)
    {
        // 3.2直接关闭文件描述符
        ::close(0);
        ::close(1);
        ::close(2);
    }
    else
    {
        // 5.1可以将输入输出，错误全部重定向到/dec/null文件
        // 这个文件属于黑洞文件，输入里面的数据都会被自动丢掉
        int _fd = ::open(Path, O_WRONLY);
        if (_fd > 0)
        {
            ::dup2(0, _fd);
            ::dup2(1, _fd);
            ::dup2(2, _fd);
            ::close(_fd);
        }
    }
}
