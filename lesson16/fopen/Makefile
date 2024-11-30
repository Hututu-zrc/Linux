CC=gcc
FLAGS=-c 
LDFLAGS=-o
SRC=myfile.c
BIN=myfile


$(BIN):$(SRC)
	$(CC) $(LDFLAGS) $@ $^



.PHONEY:clean
clean:
	rm -f $(BIN) 