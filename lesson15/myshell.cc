#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>



using namespace std;
extern char ** environ;
const int basesize=1024;
const int envnum=64;
const int argvnum=64;
const int existenvnum=0;
int lastcode=-1;//错误码返回值echo

//命令行参数
char * gargv[argvnum];
int gargc=0;
const char * sep=" ";


//全局的工作路径
char pwdenv[basesize];//用来更新环境变量
char pwd[basesize];//用来保存当前的工作路径

//环境变量
char * genv[envnum];



string GetUserName()
{
	string name=getenv("USER");
	return name.empty()?"None":name;
}

string GetHostName()
{
	string hostname=getenv("HOSTNAME");
	return hostname.empty()?"None":hostname;

}

string GetPwdName()
{
	//我们的环境变量具有全局性，需要自己维护
	//如果chdir修改了当前工作路径，环境变量不会自动更新，需要自己去维护
	//系统自带的cd会自动更新
	//我们这里修改环境变量太麻烦，换一种方式getcwd
	//string pwd =getenv("PWD");
	//int pos=pwd.find_last_of("/");
	//pwd=pwd.substr(pos+1,pwd.size()-pos);
	//return pwd.empty()?"None":pwd;
	
	//getcwd函数
	//这里的方法是，定义一个全局的pwdenv，每次保存从系统里面得到的cwd
	//而不是从环境变量里面得到的
	//chdir修改cwd后，环境变量需要我们自己维护比较麻烦
	string spwd;
	if(nullptr== getcwd(pwd,sizeof(pwd)))
	{
		return "None";
	}


	//自己维护环境变量
	snprintf(pwdenv,sizeof(pwdenv),"PWD=%s",pwd);
	//printf("%s\n",pwdenv);
	putenv(pwdenv);

	//下面是修改我们得到的工作路径，而不是一长串
	spwd=pwd;
	int pos=spwd.find_last_of("/");
	spwd=spwd.substr(pos+1,spwd.size()-pos);
	strcpy(pwd,spwd.c_str());
	return pwd;



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


//我们这里采用直接从系统的bash里面拿到我们的环境变量表
void InitEnv()
{
	int index=0;
	while(environ[index])
	{
		//strlen不包括'\0'，所以我们开辟空间时候，要多开辟一个空间，给\0留一个位置
		//1.创建空间
		genv[index]=(char *)malloc(sizeof(char)*(strlen(environ[index]+1)));
		//2.复制
		strncpy(genv[index],environ[index],strlen(environ[index]+1));
		++index;
	}
	genv[index]=nullptr;
}


void DestroyEnv()
{
	for(int i=0;genv[i];++i)
	{
		free(genv[i]);
		genv[i]=nullptr;
	}
}



void AddEnv(const char * newenv)
{
	int index=0;
	while(genv[index])
		++index;
	genv[index]=(char* )malloc(sizeof(char)*(strlen(newenv)+1));
	strncpy(genv[index],newenv,strlen(newenv)+1);
	++index;
	genv[index]=nullptr;
}

void debugenviron()
{
	for(int i=0;environ[i];++i)
	{
		printf("environ[%d]:%s\n",i,environ[i]);
	}
}
void debugenv()
{
	for(int i=0;genv[i];++i)
	{
		printf("env[%d]:%s\n",i,genv[i]);
	}
}

bool ExecuteCommandLine()
{
	pid_t id=fork();
	if(id<0)
	{
		perror("fork");
		lastcode=99;
		return false;
	}
	else if (id==0)
	{
		execvpe(gargv[0],gargv,genv);
		//这里会发生程序替换，只要子进程有返回值exit返回
		//就证明这里出了问题
		exit(1);
	}
	int status=0;
	pid_t rid=waitpid(id,&status,0);
	if(rid>0)
	{
		if(WIFEXITED(status))
		{	
			//子进程运行成功
			lastcode=WEXITSTATUS(status);	
		}
		else
		{
			//子进程运行失败
			lastcode=100;
		}
		return true;
	}
	return false;
}


bool CheckAndExecBulidCommand()
{
	//1.cd命令修改当前的工作目录，不能交给子进程处理
	if(strcmp(gargv[0],"cd")==0)
	{
		if(gargc==2)
		{
			chdir(gargv[1]);
			lastcode=0;
		}
		else
		{
			lastcode=1;
		}
		return true;
	}
	//2.这种export有关环境变量的指令，我们要单独维护自己的环境变量
	else if (strcmp(gargv[0],"export")==0)
	{
		if(gargc==2)
		{	
			AddEnv(gargv[1]);		
			lastcode=0;
		}	
		else
		{
			lastcode=2;
		}
		return true;
	}
	else if(strcmp(gargv[0],"env")==0)
	{
		for(int i=0;genv[i];i++)
		{
			printf("%s\n",genv[i]);	
		
		}
		lastcode=0;
		return true;
	}
	else if(strcmp(gargv[0],"echo")==0)
	{
		//echo $?
		//echo $PATH
		//echo 字符串
		if(gargc==2)
		{
			if(gargv[1][0]=='$')
			{
				if(gargv[1][1]=='?')
				{
					printf("%d\n",lastcode);
					lastcode=0;
				}
				else
				{	//取出$之后的字符
					string name=gargv[1];
					int pos=name.find_last_of("$");
					name=name.substr(pos+1,name.size()-pos);
					
					//开始与环境表中对比
					for(int i=0;genv[i];++i)
					{
						string bname=genv[i];
						string hname;
						int bpos=bname.find_first_of("=");
						hname=bname.substr(pos,bname.size()-pos);
						bname=bname.substr(0,bpos);
						if(strcmp(name.c_str(),bname.c_str())==0)
						{	
							printf("%s\n",hname.c_str());
							lastcode=0;
							break;
						}
					}

					if(lastcode!=0)
					{
						printf("\n");
						lastcode=5;
					}
				}

			}
			else
			{
				printf("%s\n",gargv[1]);
				lastcode=0;
			}
			
		}
		else 
		{
			lastcode=3;
		}
		return true;
	}
	return false;
}
int main()
{
	char  commandbuffer[basesize];

	InitEnv();
	//debugenviron();
	//debugenv();
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
		//有些命令比如ls等等命令，我们可以让子进程去执行
		//但是有些命令，比如:cd,export,echo等等命令属于内部命令
		//这些命令是直接修改父进程，也就是shell的属性
		//如果这些命令交给子进程去执行，那么只会更改子进程的属性，改不了当前父进程也就是shell的属性
		//如此一来，达不到我们的要求，我们需要对这些内建命令单独处理
		if(CheckAndExecBulidCommand())
			continue;
		//我们这里为了将echo当作内建命令
		//为了echo返回错误码的功能，其余内建命令也要有lastcode值

		
		//4.执行命令行
		//我们现在运行的就是shell外壳，我们不能让我们自己的进程去执行命令
		//如果自己去执行命令，那么如果出问题，shell外壳直接就挂掉了
		//所以，我们使用多进程，使用子进程去执行命令，
		//子进程发生进程替换后，后找到对应的执行程序执行命令
		ExecuteCommandLine();

	}
	DestroyEnv();	
	return 0;
}
