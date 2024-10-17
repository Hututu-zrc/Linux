#include <iostream>
#include <unistd.h>


using namespace std;

int main()
{
	//1.execl函数，第一个参数是路径，第二个是运行的程序名称，后面是可变参数，必须是nullptr || NULL 结尾
	//execl("/usr/bin/ls","ls","--color","-a","-l",nullptr);
	

	//2.execlp第一个参数就是运行程序的名称，后面与execl相同
	//该函数会自己在环境变量当中寻找路径
	//execlp("ls","--color","-a","-l",nullptr);
	

	//3.execv函数，和execl函数相比后面的可变参数变成了指针数组传递
	//char  * const argv[]={
	//	"ls",
	//	"--color",
	//	"-a",
	//	"-l",
	//	nullptr
	//};
	//execv("/usr/bin/ls",argv);

	//4.execle函数，后面可以添加自己的环境变量
	//子进程可以继承父进程的环境变量，环境变量是全局性的
	char *const envp[]={
		"grade=100",
		nullptr
	};
	execle("/bin/ls","ls","--color","-a","-l",nullptr,envp);

	return 0;
}
