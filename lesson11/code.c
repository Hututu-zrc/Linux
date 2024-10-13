#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
int main(int argc,char * argv[],char *env[])
{

	FILE * fp =fopen("test.txt","r");
	if(fp==NULL)
	{
		error(0,errno,"fopen error");
	}
	else
	{
		printf("fopen success");
	}

	//putenv("zhuruncai=100");
	//printf("zhuruncai:%s\n",getenv("zhuruncai"));
	//for(int i=0;env[i];i++)
	//{
	//	printf("%s\n",env[i]);
	//}
	return 0;
}
