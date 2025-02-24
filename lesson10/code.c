#include<stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char * argv[],char *env[])
{
	putenv("zhuruncai=100");
	printf("zhuruncai:%s\n",getenv("zhuruncai"));
	//for(int i=0;env[i];i++)
	//{
	//	printf("%s\n",env[i]);
	//}
	return 0;
}
