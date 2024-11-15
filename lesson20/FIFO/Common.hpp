#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string gpath = "./fifo";
const mode_t gmode =0666;
const int gsize=1024;
const int g_open_mode_read=O_RDONLY;
const int g_open_mode_write=O_WRONLY;


    int OpenPipe(int flag)
    {
        int fd=::open(gpath.c_str(),flag);
        if(fd<0)
        {
            std::cerr<<"open"<<std::endl;
            return fd;
        }
        //这里将我们打开文件的参数fd返回，到我们的类内里面在做判断
        return fd;
    }

    void ClosePipe(int fd)
    {
        if(fd>0)
        {
            ::close(fd);
        }
    }
