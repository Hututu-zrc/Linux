BIN=test
SRC=$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)

$(BIN):$(OBJ)
	g++ -o $@ $^ -std=c++17

%.o:%.c 
	g++ -c $^

.PHONY:clean
clean:
	rm -f $(BIN) $(OBJ)