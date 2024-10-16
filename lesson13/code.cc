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



void Save()
{
	string name =to_string(time(NULL));
	name+=".backup";
	FILE * fp=fopen(name.c_str(),"w");
	if(fp==nullptr)
	{
		perror("file open");
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
	printf("BackUp is over!\n");

}

int main()
{
	int cnt=0;
	while(true)
	{
		data.push_back(cnt);
		cnt+=2;
		pid_t id =fork();
		if(id<0)
		{
			perror("fork");
		}
		else if(id==0)
		{
			printf("sub process success,pid:%d\n",getpid());
			Save();
			exit(0);
			sleep(2);
		}
		else
		{
			int status=0;
			pid_t rid=waitpid(id,&status,0);
			if(WIFEXITED(status))
				printf("sub process exit success !pid : %d\n",rid);
			else
				perror("sub process exit false !\n");

			printf("father process success,pid:%d\n",getpid());
			sleep(2);
		}
	}
	return 0;
}

