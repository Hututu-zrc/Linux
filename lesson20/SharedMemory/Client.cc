#include <iostream>
#include "Common.hpp"

int main()
{
    //key是给系统内部使用的唯一标识符
    key_t k = ::ftok(gpath.c_str(), gid);
    if (k < 0)
    {
        std::cerr << "ftok" << std::endl;
    }
    cout << "k: " << To_HEX(k) << endl;

    //shmid是给用户使用的唯一标识符
    int shmid = ::shmget(k, gsize, IPC_CREAT);
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

    //将我们的共享内存标识为销毁段
    
    return 0;
}