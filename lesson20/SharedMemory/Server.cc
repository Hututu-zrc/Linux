#include <iostream>
#include "Common.hpp"

int main()
{

    // 1、第一步实现关键key
    key_t k = ::ftok(gpath.c_str(), gid);
    if (k < 0)
    {
        std::cerr << "ftok" << std::endl;
    }
    cout << "k: " << To_HEX(k) << endl;

    // 第二步，调用shmget函数
    // IPC_CREAT保证，不存在就创建，存在就返回共享内存的id
    // IPC_EXCL保证，不存在就创建，存在就保存

    // IPC_CREAT + IPC_EXCL 功能就是，保证我们开辟的共享内存是新的
    int shmid = ::shmget(k, gsize, IPC_CREAT | IPC_EXCL);
    if (shmid < 0)
    {
        std::cerr << "shmget" << std::endl;
    }
    cout << "shmid: " << To_HEX(shmid) << endl;

    int ret = ::shmctl(shmid, IPC_RMID, nullptr);
    if (ret < 0)
    {
        std::cerr << "shmctl" << endl;
    }
    return 0;
}