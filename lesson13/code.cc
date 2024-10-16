#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <vector>
#include <functional>
#include "task.h"
using namespace std;

typedef function<void()> task_t;

void Task(vector<task_t> & task)
{
	task.push_back(PrintLog);
	task.push_back(Download);
	task.push_back(Backup);
}
int main()
{
	pid_t id =fork();
	if(id<0)
	{
		perror("fork");
	}
	else if(id==0)
	{
		int cnt=0;
		while(cnt!=5)
		{	cnt++;
			printf("sub process create success,pid:%d\n",getpid());
			sleep(1);
		}
		exit(0);
	}
	else 
	{
		while(true)
		{
			sleep(1);
			pid_t rid =waitpid(id,nullptr,WNOHANG);
			if(rid<0)
			{
				printf("wait failure!\n");
				break;
			}
			else if(rid>0)
			{
				printf("wait is over,pid:%d\n",rid);
				break;
			}
			else 
			{
				printf("waitting!\n");	
				vector<task_t> task;
				Task(task);
				for(auto &e:task)
				{
					e();
				}
				printf("\n");

			}
		}

	}
	return 0;
}
