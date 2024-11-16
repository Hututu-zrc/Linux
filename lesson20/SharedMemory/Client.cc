#include <iostream>
#include "SharedMemory.hpp"

int main()
{
    SharedMemory shm;
    shm.GetShm();
    shm.AttachShm();
    shm.DetachShm();
    shm.DestroyShm();
    return 0;
}

// int main()
// {

//     // 1、创建key关键字
//     // key是给系统内部使用的唯一标识符
//     key_t k = ::ftok(gpath.c_str(), gid);
//     if (k < 0)
//     {
//         std::cerr << "ftok" << std::endl;
//     }
//     cout << "k: " << To_HEX(k) << endl;

//     // 2、开辟共享内存
//     // shmid是给用户使用的唯一标识符
//     int shmid = ::shmget(k, gsize, IPC_CREAT | gmode);
//     if (shmid < 0)
//     {
//         std::cerr << "shmget" << std::endl;
//     }
//     cout << "shmid: " << To_HEX(shmid) << endl;

//     // 3、将开辟的共享内存关联到我们的程序里面
//     void *shmaddr = ::shmat(shmid, nullptr, 0);
//     if ((long long)shmaddr < 0)
//     {
//         std::cerr << "shmat " << endl;
//     }
//     cout << "shmaddr :" << (void *)shmaddr << endl;

//     // 4、将共享内存脱离当前的进程段
//     int ret_shmdt = shmdt(shmaddr);
//     if (ret_shmdt < 0)
//     {
//         std::cerr << "shmdt" << endl;
//     }

//     // 5、将我们的共享内存标识为销毁段
//     int ret_shmctl = ::shmctl(shmid, IPC_RMID, nullptr);
//     if (ret_shmctl < 0)
//     {
//         std::cerr << "shmctl" << endl;
//     }

//     //

//     return 0;
// }