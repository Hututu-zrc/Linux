BIN=code
CC=g++
SRC=$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)
LFLAGS=-o
FLAGS=-c
ADD=-std=c++17
$(BIN):$(OBJ)
	$(CC) $(LFLAGS) $@ $^

%.o:%.cc
	$(CC) $(FLAGS) $<

.PHONY:clean
clean:
	rm -f $(OBJ) $(BIN)
	

