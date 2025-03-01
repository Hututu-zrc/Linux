#include "UdpServer.hpp"
#include "User.hpp"

// ./server_udp localport
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " localport" << std::endl;
        Die(USAGE_ERR);
    }
    // std::string ip = argv[1];
    uint16_t port = std::stoi(argv[1]);

    ENABLE_CONSOLE_LOG();

    // 用户管理模块
    std::shared_ptr<UserManager> um = std::make_shared<UserManager>();
    // 网络模块
    std::unique_ptr<UdpServer> svr_uptr = std::make_unique<UdpServer>(port);
    //这里是将原本的usermanag里面的AddUser有包装了一层，
    //使用一个和adduser函数一样的参数，返回值的函数，在这个函数内部又调用了一层um里面的指针
    svr_uptr->RegisterService([&um](InetAddr &id){ um->AddUser(id); },
                              [&um](int sockfd, const std::string &message){ um->Router(sockfd, message);},
                              [&um](InetAddr &id){ um->DelUser(id);}
                              );
    svr_uptr->InitServer();

    svr_uptr->Start();

    return 0;
}