#include <iostream>
#include "PollServer.hpp"

int main(int argc, char *argv[])
{
    ENABLE_CONSOLE_LOG();
    if (argc != 2)
    {
        std::cout << "select_server + " << "port" << std::endl;
        return 0;
    }
    std::unique_ptr<PollServer> ssvr = std::make_unique<PollServer>(argv[1]);
    ssvr->Init();
    ssvr->Loop();
    return 0;
}

// int main()
// {
//     //如何查看fd_set的上限
//     fd_set fds;//fd_set里面是个结构体int fds[32] --> 32位(int)*32=32个位图
//     //这里就是计算出结构体里面的数组大小，然后乘以8，得到的就是可以表示的位图的数量
//     std::cout<<"fa_set underline: "<<sizeof(fds)*8<<std::endl;
//     return 0;
// }