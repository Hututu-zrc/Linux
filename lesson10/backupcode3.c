#include <stdio.h>
#include <unistd.h>

extern char ** environ;

int main()
{
	for(int i=0;environ[i];i++)
	{
		printf("env[%d]:%s\n",i,environ[i]);
	}

	return 0;
}

