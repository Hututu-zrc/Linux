#include "TcpServer.hpp"

#include <memory>
#include <iostream>
int main()
{
    std::unique_ptr<Command> cmd = std::make_unique<Command>();
    // unique_ptr不能被复制，所以lambda表达式不能复制捕获
    //这里的lambda表达式相当于多套了一层，和handle_t函数指针使用一样的参数，返回同样的参数
    //内部再次调用Command类里面的Execute函数
    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>();
    server->InitServer();
    server->Start();
    server->Stop();
    return 0;
}