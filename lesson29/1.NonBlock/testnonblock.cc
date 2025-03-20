#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <string>
#include <errno.h>
void SetNonBlock(int fd)
{
    // 使用fcntl读取原本fd的文件打开方式和打开的状态标志位
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        perror("fcntl");
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK); // 将原来的标志位加上不阻塞的标志
}

int main()
{
    // 这里主要操作的文件的就是键盘，文件描述符是0
    std::string tips = "Pleadse Enter# ";
    char buff[1024];
    SetNonBlock(0);
    while (true)
    {
        write(0, tips.c_str(), tips.size());
        // 这里的error，不管是没有读取到数据还是读取出错误返回的都是-1
        // 但是实际应该区分这两种情况的
        // 一般就是使用错误码区分的
        int n = ::read(0, buff, sizeof(buff) - 1);
        if (n > 0)
        {
            buff[n] = 0;
            std::cout << "echo# " << buff << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "read file end!" << std::endl;
            break;
        }
        else
        {
            // 读取失败的情况分类

            if (errno == EAGAIN || errno == EWOULDBLOCK) // 底层数据没有就绪
            {
                std::cout << "底层数据没有就绪" << std::endl;
                sleep(1);
                continue;
            }
            else if (errno == EINTR) // read读取过程中，被未屏蔽的信号打断
            {
                //read阻塞式等待的时候，处于S态，也就是浅睡眠状态，可能会被信号唤醒打断
                std::cout << "被中断，从头来" << std::endl;
                sleep(1);
                continue;
            }
            else // 读取失败，发生错误
            {
                std::cout << "read " << n << " byte" << ",errno: " << errno << std::endl;
            }
        }
    }
    return 0;
}