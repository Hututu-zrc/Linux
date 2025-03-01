#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <set>

#include "Log.hpp"

using namespace LogModule;
class Command
{
public:
    Command()
    {
        _whitelist.insert("ls");
        _whitelist.insert("pwd");
        _whitelist.insert("ls -l");
        _whitelist.insert("ll");
        _whitelist.insert("touch");
        _whitelist.insert("who");
        _whitelist.insert("whoami");
    }
    bool SafeCheck(const std::string &cmdstr)
    {
        auto iter = _whitelist.find(cmdstr);
        return iter == _whitelist.end() ? false : true;
    }
    std::string Execute(const std::string &cmdstr)
    {
        // 设计思路
        // 1、pipe管道 创建管道用于父子进程传输
        // 2、fork()+exec* fork出来子进程，让子进程进行命令执行任务，结果写入到管道当中
        // 3、pipefd[0]写回 父进程从管道当中读取出来结果；
        // stdio.h里面有个函数实现了上述功能
        // FILE *popen(const char *command, const char *type);
        // 第一个参数是传入的命令，第二个是模式选择，可以选择"r"从管道里面读取，也可以是"w"，向管道写入
        // 返回值就是文件描述符
        // int pclose(FILE *stream);
        if (!SafeCheck(cmdstr))
        {
            return "\""+cmdstr+"\""+" is't permitted";
        }
        FILE *fp = ::popen(cmdstr.c_str(), "r");
        if (fp == NULL)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            exit(1);
        }
        char buff[1024];
        std::string result;
        while (fgets(buff, sizeof(buff) - 1, fp))
        {
            result += buff;
        }
        ::pclose(fp);
        return result.empty() ? "DONE" : result;
    }
    ~Command() {}

private:
    std::set<std::string> _whitelist;
};