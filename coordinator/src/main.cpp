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

#include "main.h"


void setUpListening(int &listeningSocket){
	//TODO: Move listening socket set up into this function
}


//TODO: change this function to simply accept connections move out socket set up
void handleNewConnections(const char *port, std::vector<std::unique_ptr<dgalNode> >& nodes){
	struct addrinfo *info, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, port, &hints, &info);

	int listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if(listenSocket == -1){
		std::cerr << "Error listening: " << errno << " " << strerror(errno) << std::endl;
		return;
	}

	if(bind(listenSocket, info->ai_addr, info->ai_addrlen) != 0){
		std::cerr << "Error binding to port " << port << std::endl;
		return;
	}

	if(listen(listenSocket, 10) == 0){
		//Ensure we don't tie up the addr and port combination
		int optval = 1;
		setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		//Turn off ipv6 only so that we can listen for both v4 and v6 on the same socket
		optval = 0;
		setsockopt(listenSocket, SOL_SOCKET, IPV6_V6ONLY, &optval, sizeof(optval));


		while(true){
			struct sockaddr_storage clientInfo;
			int clientSocket;
			socklen_t sockStoreSize = sizeof(struct sockaddr_storage);

			//Accept new connections and add them to the nodes list
			clientSocket = accept(listenSocket, (struct sockaddr *) &clientInfo, &sockStoreSize);
			if(clientSocket == -1){
				std::cerr << "Error accepting " << errno << " " << strerror(errno) << std::endl;
				continue;
			}


			std::unique_ptr<dgalNode> newClientNode(new dgalNode(clientSocket, "test", 2));
			nodes.push_back(std::move(newClientNode));
		}
	}else{
		std::cerr << "Error listening " << errno << " " << strerror(errno) << std::endl;
		return;
	}

}

void heartBeatCheck(const std::vector<std::unique_ptr<dgalNode> >& nodes){
	int retVal;
	while(true){
		//TODO: Fix this so that it doesn't execute too fast like when there is 1 node or 0 nodes (will eat cpu time in useless while loop)
		for(size_t i = 0; i < nodes.size(); i++){
			//Check if alive
			retVal = send(nodes[i]->sockfd, "h", 1, 0);
			if(retVal != -1){
				nodes[i]->sentHeartBeat();
			}else{
				std::cerr << "Error sending heart beat check " << errno << " " << strerror(errno) << std::endl;
			}
		}
	}
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

	setUpListening(listeningSocket);

	std::thread heartBeat(heartBeatCheck, std::ref(nodes));
	handleNewConnections(port.c_str(), nodes);
	heartBeat.join();

	return 0;
}
