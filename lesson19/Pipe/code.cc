#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
    std::cout << "Test Git" << std::endl;
    std::cout.flush();
    int fds[2] = {0}; // 读写操作,fds[0]是读操作，fds[1]是写操作
    int n = pipe(fds);
    if (n != 0)
    {
        perror("pipe");
    }
    // std::cout<<"fds[0] :"<<fds[0]<<std::endl;
    // std::cout<<"fds[1] :"<<fds[1]<<std::endl;
    // std::cerr<<"err !"<<std::endl;//fd是2
    // std::cout<<"cout !"<<std::endl;//fd是1
    int pid = ::fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) // 子进程实现写操作
    {

        ::close(fds[0]);//关闭读端
        int cnt = 0;

        while (cnt <10)
        {
            std::string str = "hello";
            str += ",cnt: ";
            str += std::to_string(cnt);
            ++cnt;
            ::write(fds[1], str.c_str(), str.size());
            sleep(1);
        }
        exit(0);
    }
    else // 父进程实现读操作
    {
        ::close(fds[1]);//关闭写端

        char buf[1024];

        while (true)
        {

            int len = ::read(fds[0], buf, 1024);
            if (len == 0)
            {
                std::cout << "pipe is empty!" << std::endl;
                break;
            }
            else
            {

                buf[len] = '\0';
                std::cout << "child -> father : " << buf << std::endl;
            }
        }
        int status = 0;
        int rid = ::waitpid(pid, &status, 0);
        if (rid < 0)
        {
            std::cerr<<"waitpid failed"<<std::endl;
            exit(1);
        }
        else
        {
            std::cout << "wait success" << std::endl;
            if (WIFEXITED(status))
            {
                std::cout << "sub process exit code :" << WEXITSTATUS(status) << std::endl;
            }
            else 
            {
                std::cerr<<"sub process exit abnormally "<<std::endl;
            }

             if (WIFSIGNALED(status))
            {
                std::cout << "sub process  signal code  :" << WTERMSIG(status) << std::endl;
            }
            else 
            {
                std::cerr<<"sub process did not terminate by signal "<<std::endl;
            }
        }
    }

    return 0;
}