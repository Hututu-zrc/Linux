CC=g++
FLAGS=-c -std=c++11
LDFLAGS=-o
SRC=mymmap.cc
BIN=mymmap


$(BIN):$(SRC)
	$(CC) $(LDFLAGS) $@ $^



.PHONY:clean
clean:
	rm -f $(BIN) 