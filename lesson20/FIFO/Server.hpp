#pragma once
#include <iostream>
#include "Common.hpp"

//这个类用来创建命名管道文件
//不能放在common.hpp文件里面，如果放入里面，两个进程都可以调用，这样不符号我们的预期
class InitFifo
{
    public:
    InitFifo()
    {
        umask(0);
        int n =::mkfifo(gpath.c_str(),gmode);
        if(n<0)
        {
            std::cerr<<"mkfifo error"<<std::endl;
            return;
        }
        std::cout<<"mkfifo success"<<std::endl;
        //sleep(5);
    }

    ~InitFifo()
    {
         int n=::unlink(gpath.c_str());
        if(n<0)
        {
            std::cerr<<"unlink error"<<std::endl;
        }
        std::cout<<"unlink success"<<std::endl;
    }
private:
};

InitFifo init;


//命名管道的创建，主要是先创建管道，我们的两个进程，一个Server创建管道并且负责接收，一个Client发送
//这里的管道的编写和使用，和我们之间学的文件的打开和关闭没什么区别
//主要的区别就是以前是用C语言写的，主要是写在函数里面
//但是，现在是主要用的是C++，写在封装号的类里面
class Server
{
public:
    Server()
    { }


    //这里的openpipe和closepipe重复了，可以重新调整下
    bool OpenPipeForRead()
    {
        //这样是错误的包装，因为我们只是传过去了我们的打开方式
        //但是，我们需要更改我们Server类里面的_fd参数,如果直接这样包装，我们的参数就是一直默认是-1了
        //return OpenPipe(g_open_mode_read);
        _fd=OpenPipe(g_open_mode_read);
        if(_fd<0)
        {
            return false;
        }
        return true;
        
    }

    int RecvPipe(std::string *out)
    {
        char buffer[gsize];
        ssize_t n=::read(_fd,buffer,sizeof(buffer)-1);
        if(n>0)
        {
            buffer[n]=0;
            *out=buffer;
        }
        else if(n==0)
        {
            return n;
        }
        return n;
    }

    void ClosePipeForRead()
    {
        ClosePipe(_fd);
    }

    ~Server()
    {  }



private:
    int _fd=-1;


};