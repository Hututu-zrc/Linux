.PHONY:ALL
ALL:server_tcp client_tcp
server_tcp:tcpServerMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread
client_tcp:tcpClientMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread

.PHONY:clean
clean:
	rm -f server_tcp client_tcp
