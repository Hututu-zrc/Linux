#pragma once
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <memory>
#include <filesystem> //C++17
#include <sstream>
#include "Mutex.hpp"

namespace LogModule
{

    using namespace MutexModule;

    // 日志需要类型
    enum class LogLevel
    {
        DEBUG = 1,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    // 1.首先创建一个基类的策略,下面会使用到多态
    class LogStrategy
    {
    public:
        //  将基类的多态变成虚函数，这样可以有效避免调用子类销毁的时候发生内存泄漏的问题
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0; // 负责刷新
    };

    // 2.分别创建刷新策略 ，分为屏幕刷新和文件刷新
    //      对于子类我们只需要完成虚函数的编写就行了，虚函数负责刷新
    class ConsoleStrategy : public LogStrategy
    {
    public:
        ConsoleStrategy() {}
        virtual void SyncLog(const std::string &message) override
        {
            MutexModule::LockGuard lock(_mutex);
            std::cout << message << std::endl;
        }
        ~ConsoleStrategy() {}

    private:
        MutexModule::Mutex _mutex;
    };
    //      文件刷新要主要路径和名称
    const std::string DefaultFilePath = "./log/";
    const std::string DefaultFileName = "log.txt";
    class FileStrategy : public LogStrategy
    {
    public:
        // 这里的构造函数缺少的功能是：建立出来指定的目录结构和文件结构
        FileStrategy(const std::string logpath = DefaultFilePath, const std::string logfilename = DefaultFileName)
            : _logpath(logpath),
              _logfilename(logfilename)
        {
            MutexModule::LockGuard lock(_mutex);
            if (std::filesystem::exists(_logpath))
            {
                return;
            }
            // std::cout<<"return "<<std::endl;
            try
            {
                // 可能创建不成功，这使用try-catch捕捉

                std::filesystem::create_directories(_logpath);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << '\n';
            }

            //
        }

        // 将日志信息写入文件当中
        virtual void SyncLog(const std::string &message) override
        {
            std::fstream file;
            // 打开文件，使用追加方式
            std::string wholepath = _logpath + _logfilename;
            file.open(wholepath, std::ios::app);
            if (file.is_open())
            {
                // 打开成功就，将日志写入到文件的当中
                file << message << "\n";
                file.close();
            }
            else
            {
                return;
            }
        }
        ~FileStrategy() {}

    private:
        std::string _logpath;     // 文件的存放路径
        std::string _logfilename; // 文件的名字
        MutexModule::Mutex _mutex;
    };

    // 3.日志的主体部分日志类

    // 内部LogMessage需要的函数GetCurTime 文件类型转换字符串函数
    std::string GetCurTime()
    {
        time_t currentTime = time(nullptr);
        struct tm *Localtime = localtime(&currentTime);
        char buff[1024];
        snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d",
                 Localtime->tm_year + 1900,
                 Localtime->tm_mon + 1,
                 Localtime->tm_mday,
                 Localtime->tm_hour,
                 Localtime->tm_min,
                 Localtime->tm_sec);
        return buff;
    }
    std::string LoglevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";

        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";

        default:
            return "UNKOWNED";
        }
    }

    class Logger
    {
    public:
        // 这里构造函数默认为屏幕刷新
        Logger() : _strategy(std::make_unique<ConsoleStrategy>())
        {
        }
        // 创建两个函数用来改变刷新策略
        void UseFileSrategy()
        {
            _strategy = std::make_unique<FileStrategy>();
        }
        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
        ~Logger() {}

        // 定义一个内部类，专门用来返回生成的日志信息
        //[2024-08-04 12:27:03] [DEBUG] [202938] [main.cc] [16] - hello world
        class LogMessage
        {
        public:
            // 构造函数初始化固定内容
            LogMessage(LogLevel loglevel, const std::string &filename, int line, Logger &logger)
                : _curtime(GetCurTime()),
                  _logtype(LoglevelToString(loglevel)),
                  _pid(getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _curtime << "] "
                   << "[" << _logtype << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "]"
                   << " - ";
                _logptr = ss.str();
            }

            // 重载<<运算符追加自定义内容
            template <typename T>
            LogMessage &operator<<(const T &in)
            {
                std::stringstream ss;
                ss << in;
                _logptr += ss.str();
                return *this;
            }
            ~LogMessage()
            {
                // 内部类通过传递参数的方式，拿到外部类变量
                //
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_logptr);
                }
            }

        private:
            std::string _curtime;  // 当前时间
            std::string _logtype;  // 日志类型
            pid_t _pid;            // 进程的pid
            std::string _filename; // 文件名称
            int _line;             // 行号
            Logger &_logger;

            std::string _logptr; // 最后合成的一条字符串
        };

        // 上面定义了一个宏 #define LOG(type) loggerr(type,__FILE__,__LINE__)
        //__FILE__可以直接获取文件的名字；  __LINE__
        // 可以获取调用__LINE__的行号
        // 这里是有意使用临时变量，这样每次都是建立临时变量，销毁的时候，就可以自动刷新，打印出来
        LogMessage operator()(LogLevel type, std::string filename, int line)
        {
            return LogMessage(type, filename, line, *this);
        }

    private:
        std::shared_ptr<LogStrategy> _strategy;
    };

    Logger logger;
// 创建logger变量，然后宏替换
#define LOG(type) logger(type, __FILE__, __LINE__)
#define ENABLE_CONSOLE_LOG() logger.UseConsoleStrategy()
#define ENABLE_FILE_LOG() logger.UseFileSrategy()

}