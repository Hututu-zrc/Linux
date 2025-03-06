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
#include "Protocol.hpp"

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
    LOG(LogLevel::DEBUG) << "running";
    // TCP客服端不需要bind绑定，会自动绑定的
    struct sockaddr_in peer;
    memset(&peer, 0, sizeof(peer));
    peer.sin_family = AF_INET;
    peer.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &peer.sin_addr);
    int n = ::connect(sockfd, CONV(&peer), sizeof(peer));
    if (n < 0)
    {
        LOG(LogLevel::FATAL) << strerror(errno);
        Die(CONNECT_ERR);
    }
    LOG(LogLevel::INFO) << "connect success";

    while (true)
    {

        // 这里实现的是简单的计算器
        // 如果用户输入的范围超过了int整型的范围
        // 输入流状态改变：同时cin的状态会被设置为错误状态（ios::failbit被置位）。
        // 在这种情况下，后续的cin操作不会从输入缓冲区中提取新的数据，
        // 而是会继续使用之前错误输入的数据进行转换，
        // 导致程序不断地发送相同的数据，出现无限制打印发送的现象。
        // 循环机制：程序使用了while (true)循环来不断获取用户输入并进行处理。
        // 只要没有遇到使程序终止的条件（如用户手动终止程序），
        // 循环就会一直执行。当cin处于错误状态时，循环条件始终为真，并且由于没有对cin的错误状态进行处理和恢复，
        // 所以程序会持续执行循环体中的代码，不断发送数据并打印相关信息。
        int _x;
        int _y;
        char _oper;
        std::cout << "Please# X: ";
        std::cin >> _x;
        std::cout << "Please# Y: ";
        std::cin >> _y;
        std::cout << "Please# OPERATION: ";
        std::cin >> _oper;

        // 1、将得到的数据进行序列化
        Request req(_x, _y, _oper);
        std::string package;
        req.Serialize(package);
        // 2、将序列化后的数据加上报头和分隔符
        Encode(package);
        // 此时的package是完整的报文
        //  3、将数据传送出去
        req.Print();
        n = ::send(sockfd, package.c_str(), package.size(), 0);
        LOG(LogLevel::DEBUG) << "send byte n: " << n;
        if (n > 0)
        {
            char buff[1024];
            memset(buff, 0, sizeof(buff));
            int m = ::recv(sockfd, buff, sizeof(buff) - 1, 0);
            buff[m] = 0;
            // LOG(LogLevel::DEBUG)<<"recv";
            // 从网络当中读取到字节流
            std::string message = buff; // 这里的message是整个报文的长度
            // 1、反序列化
            std::string context; // context就是Json
            int _result;
            int _code;

            if (!Decode(message, &context))
                continue;
            LOG(LogLevel::DEBUG) << "context: " << context;
            // 从Json当中拿到数据
            Response resp(_result);
            if (!resp.Deserialize(context))
                continue;
            // 4、打印出数据
            sleep(1);
            std::cout << "result: " << resp.GetResult() << " code: " << resp.GetCode() << std::endl;
        }
    }
}