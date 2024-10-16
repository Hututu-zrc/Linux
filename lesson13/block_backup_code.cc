#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <vector>

using namespace std;
vector<int> data;

enum{
	OK=0,
	ERR_FILE_OPEN
};

int  Save()
{
	string name =to_string(time(NULL));
	name+=".backup";
	FILE * fp=fopen(name.c_str(),"w");
	if(fp==nullptr)
	{
		//perror("file open");
		return ERR_FILE_OPEN;
	}
	else
	{
		for(const auto & e:data)
		{
			fputs(to_string(e).c_str(),fp);
			fputs(" ",fp);
		}


	}
	fclose(fp);
	return OK;
}

int main()
{
	int cnt=0;
	while(true)
	{
		data.push_back(cnt);
		cnt+=2;
		if(cnt%10==0)
		{
			pid_t id =fork();
			if(id<0)
			{
				perror("fork");
			}
			else if(id==0)
			{
				//改进地方：可以对save函数加一个返回参数来判断是否备份成功
				printf("sub process success,pid:%d\n",getpid());
				int code=Save();
				exit(code);
				//sleep(2);
			}
			else
			{
				int status=0;
				pid_t rid=waitpid(id,&status,0);
				if(WIFEXITED(status))
				{
					printf("BackUp is over!\n");
				}
				else
				{
					
					printf("BackUp is errornous!\n");
				}

				printf("father process success,pid:%d\n\n",getpid());
				sleep(2);
			}
		}
	
			
	}
	return 0;
}

