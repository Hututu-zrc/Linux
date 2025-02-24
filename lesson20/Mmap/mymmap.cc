#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using std::cerr;
using std::cout;
using std::endl;

int main()
{
    // 打开文件
    int fd = ::open("example.txt", O_RDONLY);
    if (fd < 0)
    {
        cerr << "open error" << endl;
        exit(1);
    }

    // 用于获取fd文件的信息，里面包含fd文件的大小
    //下面会用到fd文件的大小
    struct stat sb;
    if (::fstat(fd, &sb) < 0)
    {
        cerr << "stat error" << endl;
        exit(2);
    }

    // 将文件内容关联到进程地址空间
    char *mmaped = (char *)::mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (*((int *)mmaped) < 0)
    {
        cerr << "mmap error" << endl;
        exit(3);
    }

    //打印文件内容
    cout<<mmaped<<endl;

    //取消映射
     if (::munmap(mmaped, sb.st_size) == -1) {
        cerr << "munmap error" << endl;
        exit(4);
    }

    ::close(fd);
    return 0;
}