#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

using std::cerr;
using std::cout;
using std::endl;

void PrintPending()
{
    sigset_t set;
    sigemptyset(&set);
    sigpending(&set);
    cout << "cur pending :";
    for (int i = 31; i >= 1; i--)
    {
        if (sigismember(&set, i) == 1)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;
}
void PrintBlock()
{
    sigset_t set;
    sigemptyset(&set);
    sigprocmask(SIG_BLOCK,nullptr,&set);
    cout << "cur block :";
    for (int i = 31; i >= 1; i--)
    {
        if (sigismember(&set, i) == 1)
            cout << 1;
        else
            cout << 0;
    }
    cout << endl;
}
void Handler(int signo)
{
    cout << "signo: " << signo << endl;
    PrintBlock();
}
int main()
{
    sigset_t set, oset;
    ::sigemptyset(&set);
    ::sigemptyset(&oset);
    sigaddset(&set,3);
    sigprocmask(SIG_BLOCK,&set,nullptr);

    // C++里面struct也是类，struct可写可不写，
    // 但是为了可读性，我们写上struct
    struct sigaction sa, osa;
    sa.sa_handler = Handler;
    sa.sa_flags = 0;
    sa.sa_mask = set;
    
    osa = sa;
    ::sigaction(SIGINT, &sa, &osa);
    cout<<"PID :"<<getpid()<<endl;
    PrintPending();
    while (true)
    {
        PrintBlock();
        //pause();
        sleep(2);
    }

    return 0;
}

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