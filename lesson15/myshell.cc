#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>



using namespace std;
const int basesize=1024;

const int argvnum=64;
char * gargv[argvnum];
int gargc=0;
const char * sep=" ";

string GetUserName()
{
	string name=getenv("USER");
	return name.empty()?"None":name;
}

string GetHostName()
{
	string hostname=getenv("HOSTNAME");
	return hostname.empty()?"None":name;
}

string GetPwdName()
{
	string pwd =getenv("PWD");
	int pos=pwd.find_last_of("/");
	pwd=pwd.substr(pos+1,pwd.size()-pos);
	return pwd.empty()?"None":pwd;
}

string MakeCommandline()
{
	char  commandline[basesize];
	snprintf(commandline,basesize,"[%s@%s %s]# ",GetUserName().c_str(),GetHostName().c_str(),GetPwdName().c_str());
	return commandline;
}

void PrintCommandLine()
{
	const char * const  commandline=MakeCommandline().c_str();
	
	printf("%s",commandline);
	fflush(stdout);
}

bool GetCommandLine(char commandbuffer[])
{
	//这里的fgets每次都是覆盖，我们不需要担心重叠的问题
	char * ret=fgets(commandbuffer,basesize,stdin);
	if(ret==nullptr)
	{
		return false;
	}
	
	//这里需要将最后用户输入的回车改为\0
	commandbuffer[strlen(commandbuffer)-1]='\0';
	//为了防止用户只输入回车
	if(strlen(commandbuffer)==0)	return false;
	return true;


}

void PraseCommandLine(char commandbuffer[])
{
	//这里我们采用的是C语言的strtok
	//每次分析字符串，将我们的gargc,gargv复原
	memset(gargv,0,sizeof(gargv));
	gargc=0;
	
	gargv[gargc++]=strtok(commandbuffer,sep);
	while((bool)(gargv[gargc++]=strtok(NULL,sep)));
	--gargc;	
}	

void debug()
{
	printf("argc:%d\n",gargc);
	for(int i=0;i<gargc;i++)
	{
		printf("argv[%d]:%s\n",i,gargv[i]);	
	}
}




//cd等等命令，改变的是子进程的cwd不是父进程的
//我们父进程shell的没有改变
//这种需要父进程执行的命令叫做内建命令
//
bool ExecuteCommandLine()
{
	pid_t id=fork();
	if(id<0)
	{
		perror("fork");
		return false;
	}
	else if (id==0)
	{
		execvp(gargv[0],gargv);
		//这里会发生程序替换，只要子进程有返回值exit返回
		//就证明这里出了问题
		exit(1);
	}
	int status=0;
	pid_t rid=waitpid(id,&status,0);
	if(rid>0)
	{
		return true;
	}
	return false;
}


bool CheckAndExecBulidCommand()
{
	if(strcmp(gargv[0],"cd")==0)
	{
		if(gargc>=2)
		{
			chdir(gargv[1]);

		}
		return true;
	}
	return false;
}
int main()
{
	char  commandbuffer[basesize];
	while(true)
	{
		//sleep(1);
		//1.打印命令行
		//这里我们想法是用环境变量里面的数据
		PrintCommandLine();
		//2.得到命令行
		//创建一个字符串一次性保存用户输入的命令
		//这里我们通过返回值判断，如果返回值是false，证明用户输入有问题，我们直接跳过后续程序
		if(!GetCommandLine(commandbuffer))
		{ 
			continue;
		}
		//3.分析命令行
		//主要就是将我们读取的用户输入的整串字符串划分出来
		//printf("commandline:%s\n",commandbuffer);

		PraseCommandLine(commandbuffer);
		//debug();
		//printf("argc:%d\n",gargc);

		//	for(int i=0;i<gargc;i++)
		//	{
		//		printf("argv[%d]:%s\n",i,gargv[i]);
		//	}

		//检查内建命令
		if(CheckAndExecBulidCommand())
			continue;
		//4.执行命令行
		ExecuteCommandLine();

	}
	
	return 0;
}
