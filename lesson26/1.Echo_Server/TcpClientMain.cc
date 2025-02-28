#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>

#include <iostream>
#include <string>
#include <string.h>
#include <error.h>

#include "TcpClient.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

using namespace LogModule;
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        LOG(LogLevel::FATAL) << "argument less";
        Die(USAGE_ERR);
    }
    std::string _ip = argv[1];
    std::string _port = argv[2];

    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOG(LogLevel::FATAL) << strerror(errno);
        Die(SOCKET_ERR);
    }

    // TCP客服端不需要bind绑定，会自动绑定的
    Inet_addr peer(std::stoi(_port));
    int n = ::connect(sockfd, peer.GetAddr(), peer.GetLen());
    if (n < 0)
    {
        LOG(LogLevel::FATAL) << strerror(errno);
        Die(CONNECT_ERR);
    }
    LOG(LogLevel::INFO) << "connect success";

    while (true)
    {
        std::string line;
        std::cout << "Please# ";
        std::getline(std::cin, line);
        n = ::write(sockfd, line.c_str(), line.size());

        if (n > 0)
        {
            char buff[1024];
            int m = ::read(sockfd, buff, sizeof(buff) - 1);
            buff[m] = 0;
            LOG(LogLevel::INFO) << "client read: " << buff;
        }
    }
}