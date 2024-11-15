# code:code.cc
# 	g++ -std=c++11 code.cc -o code
	
# .PHONEY:clean
# clean:
# 	rm -f code

#首先，第一步是先定义变量
#一般来讲,$(变量),就是将变量替换到我们的目标位置
BIN=code
CC=g++
FLAGS=-c -Wall -std=c++11
LDFLAGS=-o
#这个命令就是将所有的.cc展示出来
#SRC:=$(shell ls *.cc)
SRC=$(wildcard *.cc)#使用wildcard函数获取当前目录下所有的.cc源文件，并将结果赋值给变量SRC

#通过模式替换，将源文件列表中的.cc后缀替换为.o后缀，得到目标文件列表
#这一步实际上是，只是进行了对应的名称的转换
OBJ=$(SRC:.cc=.o)

$(BIN):$(OBJ)
	$(CC) $(LDFLAGS) $@ $^
	
#这一步才是实际的编译过程，编译形成对应的.o文件，上面的OBJ就可以直接用文件名去最后编译
%.o:%.cc 
	$(CC) $(FLAGS) $<


.PHONY:clean
clean:
	rm -f $(BIN) $(OBJ)

