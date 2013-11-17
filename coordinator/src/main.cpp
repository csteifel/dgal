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


class dgalNode{
	public:
		dgalNode(const int socket, const std::string addr, const unsigned short port) : sockfd(socket), listenPort(port), addressInfo(addr){}
		dgalNode(const dgalNode&) = delete;
	private:
		const int sockfd;
		unsigned short listenPort;
		std::string addressInfo;
};


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
			//Accept new connections and add them to the nodes list
		}
	}else{
		std::cerr << "Error listening " << errno << " " << strerror(errno) << std::endl;
		return;
	}

}

void heartBeatCheck(const std::vector<std::unique_ptr<dgalNode> >& nodes){
	while(true){
		for(size_t i = 0; i < nodes.size(); i++){
			//Check if alive
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

	std::vector<std::unique_ptr<dgalNode> > nodes;

	std::thread heartBeat(heartBeatCheck, std::ref(nodes));
	handleNewConnections(port.c_str(), nodes);
	heartBeat.join();

	return 0;
}
