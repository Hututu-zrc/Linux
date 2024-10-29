#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	ssize_t fd1=open("log.txt",O_CREAT | O_WRONLY | O_TRUNC,0666);
	dup2(fd1,1);
	printf("hello world");
	return 0;
}
