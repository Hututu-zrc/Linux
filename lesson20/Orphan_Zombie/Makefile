CC=g++
CODE=mycode
CODE_SRC=mycode.cc

$(CODE):$(CODE_SRC)
	$(CC) -std=c++11 -o $@ $^

.PHONY:clean
clean:
	rm -f $(CODE)