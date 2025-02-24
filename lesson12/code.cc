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
		exit(100);
	}
	else
	{
		int status=0;
		pid_t rid=waitpid(-1,&status,0);
		if(rid<0)
		{
			perror("wait");
		}
		else
		{
			printf("sub process exit success, pid:%d\n",rid);
			//printf("status :%d\n",status>>8 & 0xFF);
			if(WIFEXITED(status))
			{
			
				printf("sub process exit code : %d\n",WEXITSTATUS(status));
				printf("rid:%d\n",rid);

			}
		}
		while(true)
		{
			printf("parent process success and pid:%d\n",getpid());
			sleep(2);
		}

	}

	return 0;

}
