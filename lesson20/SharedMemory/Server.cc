#include <iostream>
#include "SharedMemory.hpp"


//这里不能每个程序都是自己的shm
//这样就看不到同一份资源了
int main()
{
    SharedMemory shm;
    shm.CreateShm();
    shm.AttachShm();
    shm.DetachShm();
    shm.DestroyShm();
    return 0;
}

// int main()
// {

//     // 1、第一步实现关键key
//     key_t k = ::ftok(gpath.c_str(), gid);
//     if (k < 0)
//     {
//         std::cerr << "ftok" << std::endl;
//     }
//     cout << "k: " << To_HEX(k) << endl;

//     // 第二步，调用shmget函数
//     // IPC_CREAT保证，不存在就创建，存在就返回共享内存的id
//     // IPC_EXCL保证，不存在就创建，存在就保存

//     // IPC_CREAT + IPC_EXCL 功能就是，保证我们开辟的共享内存是新的
//     // 这里我们还要有gmode，也就是进程对于共享内存的权限必要也要说明
//     int shmid = ::shmget(k, gsize, IPC_CREAT | IPC_EXCL | gmode);
//     if (shmid < 0)
//     {
//         std::cerr << "shmget" << std::endl;
//     }
//     cout << "shmid: " << To_HEX(shmid) << endl;

//     sleep(1);
//     // 3、将开辟的共享内存关联到我们的程序里面
//     void *shmaddr = ::shmat(shmid, nullptr, 0);
//     if ((long long)shmaddr < 0)
//     {
//         std::cerr << "shmat" << endl;
//     }
//     cout << "shmaddr :" << (void *)shmaddr << endl;
//     sleep(1);

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
//     return 0;
// }