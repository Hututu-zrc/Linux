#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc,char *argv[])
{
	
	umask(0);

	int fd=open("bite.txt",O_WRONLY | O_CREAT ,0666);
	if(fd<0)
	{
		perror("fd");
		return 1;
	}
	char *buff="i like linux!";
    write(fd,buff,strlen(buff));
	close(fd);

    int fd1=open("bite.txt",O_RDONLY ,0666);
	if(fd1<0)
	{
		perror("fd1");
		return 1;
	}

	char buff1[1024];
    read(fd1,buff1,sizeof(buff1)/sizeof(char));
    printf("%s\n",buff);
	close(fd1);
	return 0;
}
