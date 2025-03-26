#include <iostream>
#include <memory>
#include "Listener.hpp"
#include "EpollServer.hpp"

int main(int argc, char *argv[])
{
    ENABLE_CONSOLE_LOG();
    if (argc != 2)
    {
        std::cout << "select_server + " << "port" << std::endl;
        return 0;
    }

    // 最顶层    Listen
    // 中间层    Connection
    // 最底层    EpollServer

    // 使用listen封装listensockfd文件描述符，然后将listen封装为connection传入EpollServer里面

    uint16_t port = std::stoi(argv[1]);
    Listener listen(port);
    std::shared_ptr<Connection> conn = std::make_shared<Connection>(listen.GetSockFd());
    conn->SetEvents(EPOLLIN | EPOLLOUT);
    conn->Register([&listen]()
                   { listen.Accept(); }, nullptr, nullptr);

    // 主函数当中插入listensockfd的connection，然后插入到EpollServer的map里面去
    std::unique_ptr<EpollServer> ssvr = std::make_unique<EpollServer>();

    ssvr->Init();
    ssvr->InsertConnection(conn);
    ssvr->Loop();
    return 0;
}
