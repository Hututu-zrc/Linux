#include <iostream>
#include "SelectServer.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "select_server + " << "port" << std::endl;
        return 0;
    }
    std::unique_ptr<SelectServer> ssvr = std::make_unique<SelectServer>(argv[1]);
    ssvr->Init();
    ssvr->Start();
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