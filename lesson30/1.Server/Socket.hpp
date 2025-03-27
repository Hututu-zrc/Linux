#pragma once
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <error.h>

#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

namespace SocketModule
{
    using namespace LogModule;
    // 使用shared_ptr 因为要拷贝
    class Socket; // 这里必须提前声明，不然TcpServer.hpp里面会报类型不完整的错误
    using SockPtr = std::shared_ptr<Socket>;
    // 接口类
    class Socket
    {
    public:
        virtual ~Socket() = default;
        virtual void SocketOrDie() = 0;
        virtual bool BindOrDie(int port) = 0;
        virtual bool ListenOrDie() = 0;
        virtual void SetSocketOpt() = 0;
        virtual int AcceptOrDie(Inet_addr *Client,int *aerrno) = 0;
        virtual int Recv(std::string *In) = 0;
        virtual int Send(std::string &Out) = 0;
        virtual bool Close() = 0;
        virtual int GetFd() = 0;
        void BulidTcpServerMethod(int port)
        {
            SocketOrDie();
            SetSocketOpt();
            BindOrDie(port);
            ListenOrDie();
        }

    private:
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket(int sockfd = gdefaultsockfd)
            : _sockfd(sockfd)
        {
        }
        void SocketOrDie() override
        {
            _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(SOCKET_ERR);
            }

            SetNonBlock(_sockfd);

            LOG(LogLevel::INFO) << "socket create success";
        }
        bool BindOrDie(int port) override
        {
            // struct sockaddr_in peer;
            // memset(&peer,0,sizeof(peer));
            // int n=::bind(_sockfd,CONV(&peer),sizeof(peer));
            if (_sockfd < 0)
            {
                return false;
            }
            Inet_addr peer(port);
            int n = ::bind(_sockfd, peer.GetAddr(), peer.GetLen());

            if (n < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(BIND_ERR);
            }
            LOG(LogLevel::INFO) << "Bind success,sockfd: " << _sockfd;
            return true;
        }
        bool ListenOrDie() override
        {
            if (_sockfd < 0)
            {
                return false;
            }
            int n = ::listen(_sockfd, BACKLOG);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(LISTEN_ERR);
            }
            LOG(LogLevel::INFO) << "Listen success,sockfd: " << _sockfd;
            return true;
        }

        virtual void SetSocketOpt() override
        {
            // 保证服务器异常断开以后，还是可以直接立即重启的，不会有bind问题
            int opt = 1;
            int n = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        }

        // accpet函数需要做到的
        // 1、返回TcpServer.hpp里面，给出nersockfd
        // 2、返回client的信息
        // 这里不能直接返回int类型,所有的sockfd类型，除非是在Socket.hpp，也就是底层里面使用
        // 在上层使用的时候，统一返回封装着sockfd的Socket类型
        int AcceptOrDie(Inet_addr *Client,int *out_errno) override
        {
            struct sockaddr_in client;
            socklen_t socklen = sizeof(client);
            int newsockfd = ::accept(_sockfd, CONV(&client), &socklen);
            *out_errno=errno;
            if (newsockfd < 0)
            {
                LOG(LogLevel::FATAL) <<"socket accept done";
                // Die(ACCEPT_ERR);
            }
            Client->SetAddr(client, socklen);
            LOG(LogLevel::INFO) << "Accept success,newsockfd: " << newsockfd;
            return newsockfd;
            // return std::make_shared<TcpSocket>(newsockfd);
        }
        int Recv(std::string *Out) override
        {
            char buff[1024 * 6];
            memset(buff, 0, sizeof(buff));
            int n = ::recv(_sockfd, buff, sizeof(buff) - 1, 0);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(RECV_ERR);
            }
            buff[n] = 0;
            *Out = buff;
            LOG(LogLevel::INFO) << "Recv success,sockfd: " << _sockfd;

            return n;
        }
        int Send(std::string &In) override
        {
            int n = ::send(_sockfd, In.c_str(), In.size(), 0);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(SEND_ERR);
            }
            LOG(LogLevel::INFO) << "Send success,sockfd: " << _sockfd;

            return n;
        }
        bool Close() override
        {
            if (_sockfd < 0)
            {
                return false;
            }
            ::close(_sockfd);
            LOG(LogLevel::INFO) << "Close success";
            return true;
        }
        int GetFd()
        {
            return _sockfd;
        }
        ~TcpSocket() {}

    private:
        int _sockfd;
    };
}