#include <iostream>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

bool listen(char *port){
	struct addrinfo *info, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, port, &hints, &info);

	int listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if(listenSocket == -1){
		std::cerr << "Error listening: " << errno << " " << strerror(errno) << std::endl;
		return false;
	}

	if(bind(listenSocket, info->ai_addr, info->ai_addrlen) != 0){
		std::cerr << "Error binding to port " << port << std::endl;
		return false;
	}

	if(listen(listenSocket, 10) == 0){
		//Ensure we don't tie up the addr and port combination
		int optval = 1;
		setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		//Turn off ipv6 only so that we can listen for both v4 and v6 on the same socket
		optval = 0;
		setsockopt(listenSocket, SOL_SOCKET, IPV6_V6ONLY, &optval, sizeof(optval));

		while(true){
			
		}
	}else{
		std::cerr << "Error listening " << errno << " " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

void heartBeatCheck(){

}

int main(int argc, char *argv[]){
	listen("25665");

	return 0;
}
