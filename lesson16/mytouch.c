#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char *argv[])
{
	//umask设置起始的权限
	umask(0);
	//open第二个参数是我们的打开方式
	//open第三个参数是我们打开文件的起始权限
	//open有两个，一个是参数是两个的，一个是参数是三个的
	//两个参数的一般用来打开已存在文件
	//三个参数的一般用来打开未存在文件
	open(argv[1],O_WRONLY | O_CREAT,0666);
	return 0;
}
