BIN=processpool 
CC=g++
FLAGS=-c -Wall -std=c++11
LDFLAGS=-o

SRC=$(wildcard *.cc)  # 修改文件后缀为 .cc
OBJ=$(SRC:.cc=.o)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $@ $^

%.o: %.cc
	$(CC) $(FLAGS) $<

.PHONY: clean
clean:
	rm -f $(BIN) $(OBJ)


