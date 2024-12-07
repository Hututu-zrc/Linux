CC=g++
SRC=thread.cc
BIN=thread

$(BIN):$(SRC)
	$(CC) -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f $(BIN)