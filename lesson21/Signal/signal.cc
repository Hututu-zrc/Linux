#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

using std::cerr;
using std::cout;
using std::endl;

void Handler(int signo)
{
    cout<<"signo: "<<signo<<endl;
    while(true)
    {
        int rid=waitpid(-1,nullptr,WNOHANG);
        if(rid<0)
        {
            cerr<<"waitpid error"<<endl;
            break;
        }
        else if(rid>0)
        {
            cout<<"subprocess is recycled."<<endl;
            break;
        }
        else 
        {
            cout<<"there is no subprocess."<<endl;
            break;
        }

    }
}
int main()
{
    signal(SIGCHLD, Handler);
    for (int i = 0; i < 5; i++)
    {
        if (fork() == 0)
        {
            cout << "i am subprocess,pid: " << getpid() << endl;
            exit(0);
        }
    }
    
    while (true)
    {
        //cout << "i am fatherprocess,pid: " << getpid() << endl;
        sleep(1);
    }
    
    //
    // if (::fork() == 0)
    // {
    //     while (true)
    //     {
    //         cout << "i am subprocess,pid: " << getpid() << endl;
    //         sleep(1);
    //     }
    //     exit(0);
    // }
    // signal(SIGCHLD,Handler);
    // 这里的父进程每次都要阻塞式的等待子进程进行完毕
    // 或者是waitpid采用不阻塞的方式，但是OS每次要轮询访问子进程，看子进程有没有退出
    //  pid_t rid = ::waitpid(-1, nullptr, WNOHANG);
    //  if (rid < 0)
    //  {
    //      cerr << "waitpid failure." << endl;
    //      exit(1);
    //  }
    //  else if(rid>0)
    //  {
    //      cout<<"subprocess exit succussfully!"<<endl;
    //  }
    //  else
    //  {
    //      cout<<"no subprocess is over!"<<endl;
    //  }

    return 0;
}

// void sigcb(int signo )
// {
//     cout<<"signo:"<<signo<<endl;
//     sleep(1);
//     exit(0);
// }

// int  main()
// {
//     cout<<"PID: "<<getpid()<<endl;
//     struct sigaction sa;
//     sa.sa_handler=sigcb;

//     sigset_t set;
//     ::sigemptyset(&set);

//     sa.sa_mask=set;
//     ::sigaction(SIGINT,&sa,nullptr);
//     while(true)
//     {
//         cout<<"running"<<endl;
//         sleep(1);
//     }
//     return 0;
// }

// long long  cnt = 0;
// void Handler(int signo)
// {
//     cout<<"signo:"<<signo<<endl;
//     //这里可以是3021601220
//     cout << "cnt:" << cnt << endl;

//     exit(0);
// }
// int main()
// {
//     alarm(1);
//     signal(SIGALRM,Handler);

//     while (true)
//     {
//         ++cnt;
//         //这里的IO严重拖延了效率，cnt只能是47000多
//         //cout << "cnt:" << cnt << endl;
//     }
//     return 0;
// }

// void PrintPending()
// {
//     sigset_t set;
//     sigemptyset(&set);
//     sigpending(&set);
//     cout << "cur pending :";
//     for (int i = 31; i >= 1; i--)
//     {
//         if (sigismember(&set, i) == 1)
//             cout << 1;
//         else
//             cout << 0;
//     }
//     cout << endl;
// }
// void PrintBlock()
// {
//     sigset_t set;
//     sigemptyset(&set);
//     sigprocmask(SIG_BLOCK, nullptr, &set);
//     cout << "cur block :";
//     for (int i = 31; i >= 1; i--)
//     {
//         if (sigismember(&set, i) == 1)
//             cout << 1;
//         else
//             cout << 0;
//     }
//     cout << endl;
// }
// void Handler(int signo)
// {
//     cout << "signo: " << signo << endl;
//     PrintBlock();
// }
// int main()
// {
//     sigset_t set, oset;
//     ::sigemptyset(&set);
//     ::sigemptyset(&oset);
//     sigaddset(&set, 3);
//     sigprocmask(SIG_BLOCK, &set, nullptr);

//     // C++里面struct也是类，struct可写可不写，
//     // 但是为了可读性，我们写上struct
//     struct sigaction sa, osa;
//     sa.sa_handler = Handler;
//     sa.sa_flags = 0;
//     sa.sa_mask = set;

//     osa = sa;
//     ::sigaction(SIGINT, &sa, &osa);
//     cout << "PID :" << getpid() << endl;
//     PrintPending();
//     while (true)
//     {
//         PrintBlock();
//         // pause();
//         sleep(2);
//     }

//     return 0;
// }

// void Handler(int signo)
// {

//     cout << "unlock signal" << endl;
//     exit(0);
// }
// int main()
// {

//     // 首先将block信号集里面的2号信号进行屏蔽
//     //这里我们是在栈上开辟的数据，可能会有数据在里面，首先需要清空一下
//     sigset_t bset, obset;
//     ::sigemptyset(&bset);
//     ::sigemptyset(&obset);
//     ::sigaddset(&bset, 2); // 这里只是修改了信号集，没有实际添加到进程当中
//     ::sigprocmask(SIG_SETMASK, &bset, &obset);

//     // 打印pending信号集
//     sigset_t pset;
//     ::sigemptyset(&pset);

//     signal(2, Handler);
//     int cnt = 10;
//     while (true)
//     {
//         // 这里的sigpending要放到循环里面，不然一直打印的都是之前的pending
//         int n = ::sigpending(&pset);
//         if (n < 0)
//         {
//             cerr << "sigpending" << endl;
//             exit(1);
//         }
//         cout << "curr pending list [" << getpid() << "]:";
//         for (int i = 1; i <= 31; i++)
//         {
//             // sigismember函数传入的信号的编号，不是信号位图的下标
//             int ret = ::sigismember(&pset, i);
//             if (ret == 1)
//                 cout << 1;
//             else if (ret == 0)
//                 cout << 0;
//             else
//                 cerr << "sigismember" << endl;
//         }
//         cout << endl;
//         sleep(1);

//         if((cnt--)==0)
//         {
//             sigprocmask(SIG_SETMASK, &obset, nullptr);
//         }
//     }
//     return 0;
// }

// void sigcb(int signo)
// {
//     cout << "signo:" << signo << endl;
//     // cout << "receive signo!" << endl;
// }
// int main()
// {
//     // int cnt = 10;
//     cout << "PID:" << getpid() << endl;
//     signal(SIGINT, sigcb);
//     while (true)
//     {
//         cout << "running..." << endl;
//         sleep(1);
//     }

//     return 0;
// }

// void Handler(int signo)
// {
//     cout << "signo:" << signo << endl;
//     cout << "receive signo!" << endl;
//     exit(0);
// }
// int main()
// {
//     // int cnt = 10;

//     signal(SIGALRM, Handler);
//     while (true)
//     {
//         std::cout << "hello" << std::endl;
//         sleep(1);

//         // alarm不能放在循环里面，不然永远没有机会终止程序，
//         // 因为刚准备终止，新的alarm信号又来了
//         //  signal(2,Handler);
//     }
//     return 0;
// }