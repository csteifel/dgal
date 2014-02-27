#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <poll.h>

#include "main.h"

//Will return true on success, false on failure.
bool setUpListening(int &listenSocket, const char* port){
	struct addrinfo *info, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, port, &hints, &info);

	listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	//Ensure we don't tie up the addr and port combination after exit
	int optval = 1;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	//Turn off ipv6 only so that we can listen for both v4 and v6 on the same socket
	optval = 0;
	setsockopt(listenSocket, SOL_SOCKET, IPV6_V6ONLY, &optval, sizeof(optval));


	if(listenSocket == -1){
		std::cerr << "Error listening: " << errno << " " << strerror(errno) << std::endl;
		return false;
	}

	if(bind(listenSocket, info->ai_addr, info->ai_addrlen) != 0){
		std::cerr << "Error binding to port " << port << ": " << errno << " " << strerror(errno) << std::endl;
		return false;
	}

	if(listen(listenSocket, 10) != 0){
		std::cerr << "Error listening " << errno << " " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

void socketWatch(const int listenSocket, std::vector<std::unique_ptr<dgalNode> >& nodes){
	//TODO: Implement ability to watch sockets using poll

	std::vector<struct pollfd> socketsToPoll;
	struct pollfd listeningPollFD;

	const short lookForEvents = POLLIN | POLLPRI;

	listeningPollFD.fd = listenSocket;
	listeningPollFD.events = lookForEvents;

	socketsToPoll.push_back(listeningPollFD);

	while(true){
		//Wait a maximum of 30 seconds if no response than we either have no nodes or they all disconnected
		int res = poll(socketsToPoll.data(), socketsToPoll.size(), 30000);

		if(res > 0){
			for(unsigned int i = 0; i < socketsToPoll.size(); i++){
				if(socketsToPoll[i].revents == 0){
					continue;
				}
				if(socketsToPoll[i].fd == listenSocket){
					std::cout << "Accepting connection" << std::endl;
					//We are accepting new connections
					struct pollfd newPollWatch;
					newPollWatch.fd = handleNewConnections(listenSocket, nodes);
					newPollWatch.events = lookForEvents;
					socketsToPoll.push_back(newPollWatch);
					continue;
				}


				if(socketsToPoll[i].revents & POLLHUP || socketsToPoll[i].revents & POLLRDHUP){
					std::cout << "POLLHUP" << std::endl;
				}


				switch(socketsToPoll[i].revents){
					case POLLERR:
						std::cout << "POLLERR" << std::endl;
						break;
					case POLLRDHUP:
						std::cout << "POLLRDHUP" << std::endl;
						break;
					case POLLHUP:
						std::cout << "POLLHUP" << std::endl;
						break;
					case POLLIN:
						char a[10];
						memset(a, 0, 10);
						if(recv(socketsToPoll[i].fd, a, 10, 0)){
							std::cout << a << std::endl;
						}
						break;
					case POLLPRI:
						std::cout << "POLLPRI" << std::endl;
						break;
					default:
						std::cout << "DEFAULT" << std::endl;
						exit(1);
				}

			}
		}else{
			//TODO: Go through and mark all nodes as dead if any exist
		}
	}

}


int handleNewConnections(const int listenSocket, std::vector<std::unique_ptr<dgalNode> >& nodes){
	struct sockaddr_storage clientInfo;
	int clientSocket;
	socklen_t sockStoreSize = sizeof(struct sockaddr_storage);

	//Accept new connections and add them to the nodes list
	clientSocket = accept(listenSocket, (struct sockaddr *) &clientInfo, &sockStoreSize);
	if(clientSocket == -1){
		std::cerr << "Error accepting " << errno << " " << strerror(errno) << std::endl;
		return -1;
	}


	std::unique_ptr<dgalNode> newClientNode(new dgalNode(clientSocket, "test", 2));
	nodes.push_back(std::move(newClientNode));

	return clientSocket;
}



int main(int argc, char *argv[]){
	std::string port;
	if(argc > 1){
		port = argv[1];
	}else{
		port = "25665";
	}


	int listeningSocket;
	std::vector<std::unique_ptr<dgalNode> > nodes;

	if(setUpListening(listeningSocket, port.c_str()) == false){
		return 1;
	}

	socketWatch(listeningSocket, nodes);

	return 0;
}
