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
    uint16_t _port = std::stoi(argv[2]);

    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        LOG(LogLevel::FATAL) << strerror(errno);
        Die(SOCKET_ERR);
    }

    // TCP客服端不需要bind绑定，会自动绑定的
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(_port);
    peer.sin_addr.s_addr = inet_pton(AF_INET, _ip.c_str(), &peer.sin_addr);
    int n = ::connect(sockfd, CONV(&peer), sizeof(peer));
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
            std::cout << buff << std::endl;
        }
    }
}