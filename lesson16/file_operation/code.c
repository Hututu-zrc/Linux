#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc,char *argv[])
{
	//umask设置起始的权限
	umask(0);
	//open第二个参数是我们的打开方式
	//open第三个参数是我们打开文件的起始权限
	//open有两个，一个是参数是两个的，一个是参数是三个的
	//两个参数的一般用来打开已存在文件
	//三个参数的一般用来打开未存在文件
	
	//O_TRUNC是存在就覆盖掉，全部清空
	//我们这里的三个参数就实现了c库函数当中的"w"的功能
	//int fd=open("log.txt",O_WRONLY | O_CREAT | O_TRUNC,0666);
	
	//O_APPEND就是追加的功能
	int fd=open("log.txt",O_WRONLY | O_CREAT | O_APPEND,0666);
	if(fd<0)
	{
		perror("fd");
		return 1;
	}
	//const char * buf="hello zhuruncai\n";
	//printf("buf:%d\n",strlen(buf));
	
	char buf[1024];
	ssize_t rret=read(0,buf,sizeof(buf)-1);
	printf("rret:%d\n",rret);
	ssize_t ret=write(1,buf,strlen(buf));
	printf("ret:%d\n",ret);
	printf("fd:%d\n",fd);
	close(fd);
	return 0;
}
