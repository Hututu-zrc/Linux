#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
	//chdir("/home/ZRC/linux");
	//FILE * file=fopen("test.txt","w");
	//fprintf(file,"%s","zhuruncai");
	//fclose(file);
	//printf("PID:%d\n",getpid());
	//printf("PPID:%d\n",getppid());
	

	pid_t id= fork();
	while(1)
	{
		if(id>0)
		{
			printf("this is Father Process:  PID:%d   PPID:%d   ID:%d  \n",getpid(),getppid(),id);
			int n=wait(NULL);
			if(n>0)	printf("成功回收子进程");
			sleep(1);
		}
		else if (id==0)
		{
			printf("this is Son Process:  PID:%d   PPID:%d   ID:%d  \n",getpid(),getppid(),id);
			sleep(1);
	   	
		}
		printf("\n");
	}
	//while(1)
	//{

	//	printf("hello linux\n");
	//	sleep(1);
	//}

	return 0;
}
