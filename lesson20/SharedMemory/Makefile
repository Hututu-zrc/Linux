SERVER=server
CLIENT=client
CC=g++
SERVER_SRC=Server.cc
CLIENT_SRC=Client.cc

.PHONY:ALL
ALL:$(SERVER) $(CLIENT)

$(CLIENT):$(CLIENT_SRC)
	$(CC) -std=c++11 -o $@ $^


$(SERVER):$(SERVER_SRC)
	$(CC) -std=c++11 -o $@ $^



.PHONY:clean
clean:
	rm -f $(SERVER) $(CLIENT)