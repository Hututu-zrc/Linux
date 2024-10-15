#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	pid_t id=fork();
	if(id<0)
	{
		perror("fork error");
	}
	else if(id==0)
	{
		int cnt=5;
		while(cnt--)
		{
	
			printf("sub process successed and pid:%d\n",getpid());
			sleep(2);
		}
		exit(0);
	}
	else
	{
		pid_t rid=wait(nullptr);
		if(rid<0)
		{
			perror("wait");
		}
		else
		{
			printf("sub process exit success, pid:%d\n",rid);
		}
		while(true)
		{
			printf("parent process success and pid:%d\n",getpid());
			sleep(2);
		}

	}

	return 0;

}
