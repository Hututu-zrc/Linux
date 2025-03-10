#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include "Mutex.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

using namespace LogModule;
using namespace MutexModule;
namespace UserModule
{
    class UserInterface
    {
    public:
        virtual ~UserInterface() = default;
        virtual void SendTo(int &sockfd, const std::string &msg) = 0;
        virtual bool operator==(const Inet_addr &user) = 0;
        virtual std::string GetId() = 0;
    };
    class User : public UserInterface // 唯一标识符就是addr
    {
    public:
        User(const Inet_addr &user) : _user(user)
        {
        }

        void SendTo(int &sockfd, const std::string &msg) override
        {

            ::sendto(sockfd, msg.c_str(), msg.size(), 0, _user.GetAddr(), _user.GetLen());
        }
        bool operator==(const Inet_addr &user) override
        {
            // 这里Iner_addr没有写成常函数，所以一开始的时候我们传递const Inet_addr &user
            // 相当于一开始时普通权限的_user和const权限的_user比较
            // 这里的user是const的，常对象不能调用非常函数，所以不能调用operator运算符函数

            return user == _user;
        }
        std::string GetId()
        {
            return _user.GetIp();
        }
        ~User() {}

    private:
        Inet_addr _user;
    };

    class UserManage
    {
    public:
        UserManage() {}
        void AddUser(const Inet_addr &user)
        {
            LockGuard lockguard(_mutex);

            // 首先查找该用户是否存在于list当中
            for (auto &e : _users)
            {
                if (*e == user)
                {
                    return;
                }
            }
            // 如果不存在就添加
            LOG(LogLevel::INFO) << "Add NewUser";
            _users.push_back(std::make_shared<User>(user));
        }
        void DelUser(const Inet_addr &user)
        {
            LOG(LogLevel::DEBUG) << "deluser is running";

            _users.remove_if([&user](std::shared_ptr<UserInterface> id)
                             { return *id == user; });
            // _users.erase(_users.end());
            // LOG(LogLevel::DEBUG) << "deluser is over";
        }
        void Route(int sockfd, const std::string &msg) // 将消息转发到所有用户
        {
            LockGuard lockguard(_mutex);

            for (auto &e : _users)
            {
                e->SendTo(sockfd, msg);
                LOG(LogLevel::DEBUG) << " " << e->GetId();
            }
        }
        ~UserManage() {}

    private:
        // 转发和添加过程中，会同时访问_users，属于临界资源需要保护
        std::list<std::shared_ptr<UserInterface>> _users;

        Mutex _mutex;
    };
}
