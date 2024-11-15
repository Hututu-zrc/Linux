#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using std::cout;
using std::endl;

// 这里的路径一定要是正确的才可以
// 实现共享内存，第一步是使用ftok生成关键key
const std::string gpath = "/home/zrc/linux";
const int gid = 1;
const int gsize = 1024;
std::string To_HEX(int x)
{
    char buffer[gsize];

    snprintf(buffer, sizeof(buffer), "%x", x);
    return buffer;
}

const int g_open_mode_read = O_RDONLY;
const int g_open_mode_write = O_WRONLY;
