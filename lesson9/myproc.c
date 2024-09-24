#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
	//chdir("/home/ZRC/linux");
	//FILE * file=fopen("test.txt","w");
	//fprintf(file,"%s","zhuruncai");
	//fclose(file);
	//printf("PID:%d\n",getpid());
	//printf("PPID:%d\n",getppid());
	

	//pid_t id= fork();
	//int cnt=10;
	while(1)
	{
		printf("hello world\n");
		sleep(1);
	}
	//while(1)
	//{

	//	printf("hello linux\n");
	//	sleep(1);
	//}

	return 0;
}
