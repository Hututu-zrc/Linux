#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
int main()
{
    int fd = ::open("./Test.txt", O_RDWR | O_CREAT, 0664);
    if (fd < 0)
    {
        std::cout << "open error" << std::endl;
        return -1;
    }

    int m = ::fcntl(fd, F_GETFL);
    if (m < 0)
    {
        std::cerr << "fcntl get error" << std::endl;
        ::close(fd);
        return -1;
    }
    if (::fcntl(fd, F_SETFL ,m | O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl set error" << std::endl;
        ::close(fd);
        return -1;
    }
    std::string str;
    std::getline(std::cin, str);
    int n = ::write(fd, str.c_str(), str.size());
    if (n < 0)
    {
        std::cout << "write error" << std::endl;
        ::close(fd);

        return -1;
    }

    if (::lseek(fd, 0, SEEK_SET) < 0)
    {
        std::cout << "文件指针重置失败" << std::endl;
        ::close(fd);
        return -1;
    }

    char buff[1024];
    n = ::read(fd, buff, sizeof(buff) / sizeof(char));
    if (n < 0)
    {
        if (n == -1 && errno == EAGAIN)
        {
            std::cout << "data will be ready" << std::endl;
        }
        else if (n == -1 && errno == EWOULDBLOCK)
        {
            std::cout << "nonblock" << std::endl;
        }
        ::close(fd);
        return -1;
    }
    buff[n] = '\0';
    std::cout << "read data: " << buff << std::endl;
    ::close(fd);
    return 0;
}