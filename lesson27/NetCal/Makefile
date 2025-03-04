.PHONY:ALL
ALL:server_tcp client_tcp
server_tcp:TcpServerMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp
client_tcp:TcpClientMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f server_tcp client_tcp
