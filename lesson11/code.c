#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
int main(int argc,char * argv[],char *env[])
{

	FILE * fp =fopen("test.txt","r");
	if(fp==NULL)
	{
		error(0,errno,"fopen error");
		perror("perror: fopen error:");
		//printf("\n");
		printf("strerror: fopen error:%s\n",strerror(errno));
	}
	else
	{
		printf("fopen success\n");
		fclose(fp);
	}
	

	//putenv("zhuruncai=100");
	//printf("zhuruncai:%s\n",getenv("zhuruncai"));
	//for(int i=0;env[i];i++)
	//{
	//	printf("%s\n",env[i]);
	//
	//}
	_exit(10);
	return 0;
}
