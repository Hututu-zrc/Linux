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
const int gsize = 4096;
const int gmode = 0600;

std::string To_HEX(int x)
{
    char buffer[gsize];

    snprintf(buffer, sizeof(buffer), "%x", x);
    return buffer;
}

class SharedMemory
{

private:
    void CreateShmHelper(int shmflag)
    {
        _key = ::ftok(gpath.c_str(), gid);
        if (_key < 0)
        {
            std::cerr << "ftok" << std::endl;
        }
        cout << "k: " << To_HEX(_key) << endl;
        _shmid = ::shmget(_key, gsize, shmflag);
        if (_shmid < 0)
        {
            std::cerr << "shmget" << std::endl;
            return ;
        }
        cout << "shmid: " << To_HEX(_shmid) << endl;
    }

public:
    SharedMemory()
        : _key(-1),
          _shmid(-1),
          _shmaddr(nullptr)
    {
    }

    void CreateShm()
    {
        if (_shmid < 0)
        {
            CreateShmHelper(IPC_CREAT | IPC_EXCL | gmode);
        }
    }
    void GetShm()
    {
        CreateShmHelper(IPC_CREAT | gmode);
    }

    void AttachShm()
    {
        _shmaddr = ::shmat(_shmid, nullptr, 0);
        if ((long long)_shmaddr < 0)
        {
            std::cerr << "shmat" << endl;
            return ;
        }
        cout << "shmaddr :" << (void *)_shmaddr << endl;
    }

    void DetachShm()
    {
        int ret_shmdt = shmdt(_shmaddr);
        if (ret_shmdt < 0)
        {
            std::cerr << "shmdt" << endl;
        }
    }

    void DestroyShm()
    {
        int ret_shmctl = ::shmctl(_shmid, IPC_RMID, nullptr);
        if (ret_shmctl < 0)
        {
            std::cerr << "shmctl" << endl;
        }
    }
    void * GetAddress()
    {
        return _shmaddr;
    }
    ~SharedMemory()
    {
    }

private:
    key_t _key;
    int _shmid;
    void *_shmaddr;
};


