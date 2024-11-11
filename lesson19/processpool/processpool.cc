#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//C++11 新增了模板别名功能，可以用 using 给模板创建别名，这在定义复杂类型时尤其有用
using work_t = std::function<void()>;
std::vector<Channel> channels;//用来记录每个管道


enum
{
    OK = 0,
    Augment_Err,
    Pipe_Err,
    Fork_Err,
    Wait_Err
};

// 这个类，用来存储我们每次开辟的子进程的信息
// 不然，N次循环后，子进程无法使用了
class Channel
{
public:
    Channel(int wid):_wid(wid)
    {
        _name = "Channel "+ std::to_string(wid);
    }
    ~Channel()
    {
    }

private:
    int _wid;//写进程的pid
    std::string _name;//对每个管道起一个名字
};

void Usage()
{
    std::cout << "augment" << " augment_num" << std::endl;
}

void Worker()
{

}


int main(int argc, char *argv[])
{
    // 我们现在的程序就是master
    // 我们这argc<2,代表参数错误，要求命令行输入的第二个参数是我们进程池里面进程的个数
    if (argc < 2)
    {
        Usage();
        exit(1);
    }
    for (int i = 0; i < std::stoi(argv[1]); ++i)
    {
        // 创建管道
        int pfds[2] = {0};
        int n = ::pipe(pfds);
        if (n != 0)
        {
            std::cerr << "pipe" << std::endl;
            exit(Pipe_Err);
        }

        // 创建子进程
        int id = ::fork();
        if (id < 0)
        {
            std::cerr << "fork" << std::endl;
            exit(Fork_Err);
        }

        // 子进程用来读
        if (id == 0)
        {
            ::close(pfds[1]);
            Worker();
            exit(OK);
        }


        
        // 父进程用来写
        int status = 0;
        int rid = ::waitpid(id, &status, 0);
        if (rid < 0)
        {
            std::cerr << "waitpid" << std::endl;
            exit(Wait_Err);
        }

        //使用上面定义的vector添加我们的每个管道
        channels.emplace_back(id);
        ::close(pfds[0]);
    }
    return 0;
}