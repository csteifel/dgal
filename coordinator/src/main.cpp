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
#include <mutex>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <stdint.h>

#include "main.h"
#include "dgalutility.h"


namespace {
	std::mutex processingLock;
	std::mutex sendingLock;
	std::mutex nodesLock;
}

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

void processMessages(processingArgContainer pC, std::queue<std::pair<int, std::string> >& sendingQueue){
	while(true){
		if(pC.socketsToProcess.size() != 0){
			processingLock.lock();
			int fd = pC.socketsToProcess.front();
			pC.socketsToProcess.pop();
			std::shared_ptr<dgalNode> node = pC.nodesToProcess.front();
			pC.nodesToProcess.pop();
			processingLock.unlock();

			//Create buffer to read in message type as well as size information
			char buf[sizeof(uint8_t) + sizeof(uint64_t)];
			memset(buf, 0, sizeof(buf));
			int res = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);

			if(res > 0){
				uint8_t type = (uint8_t) buf[0]; //No endian worry here only 1 byte


				uint64_t convert;
				memcpy(&convert, &buf[1], sizeof(uint64_t));

				uint64_t length = dgal::ntohll(convert); //Convert to host endianness

				if(type == dgal::HEARTBEAT){
					//Heart beat recieved
					node->recievedHeartBeat();
					std::cout << "Recieved heart beat from " << fd << std::endl;
				}else if(type == dgal::BESTMESSAGE){
					//Recieving serialized individuals
					std::cout << "Recieving population from " << fd << std::endl;

					//TODO: Add some safety barriers around allocating huge space
					std::string message(length, 0);
					res = recv(fd, &message[0], length, MSG_DONTWAIT);

					if(res > 0 && (uint64_t) res == length){
						//Recieve bests message, schedule to send out to the rest
						std::cout << message << std::endl;
						sendingLock.lock();
						sendingQueue.push(std::make_pair(fd, message));
						sendingLock.unlock();
					}else{
						std::cerr << "Error reading message from socket! Supplied size wrong!" << std::endl;
					}

				}else{
					//Stream error terminate connection
					std::cerr << "Recieved non conforming message shutting down socket" << std::endl;
					shutdown(fd, SHUT_RDWR);
				}
			}else if(res == -1){
				//Error reading
				std::cerr << "Error reading socket " << fd << ": " << errno << " " << strerror(errno) << std::endl;
			}else{
				//Disconnect
				shutdown(fd, SHUT_RDWR);
			}

			processingLock.lock();
			//Release this socket fd to be allowed to be added back into the queue
			pC.queuedSockets.erase(fd);
			processingLock.unlock();
		}
	}
}

/**
 * Thread to process messages that should be sent out to the nodes in the netework
 * @param nodes        list of nodes
 * @param sendingQueue queue that is added to from processMessages when it recieves a list of a nodes bests
 */
void sendMessages(std::unordered_map<int, std::shared_ptr<dgalNode> >& nodes, std::queue<std::pair<int, std::string> >& sendingQueue){
	while(true){
		if(sendingQueue.empty() == false){
			sendingLock.lock();
			std::pair<int, std::string> sendOut = std::move(sendingQueue.front());
			sendingQueue.pop();
			sendingLock.unlock();

			int dontSendTo = sendOut.first;
			std::string message = std::move(sendOut.second);

			std::vector<int> sendToFds;
			std::vector<std::shared_ptr<dgalNode> > sendToNodes;

			//Get a list of all the nodes first so that we aren't holding a lock while possibly on a blocking send call
			nodesLock.lock();
			for(auto itr = nodes.begin(), end = nodes.end(); itr != end; itr++){
				sendToFds.push_back(itr->first);
				sendToNodes.push_back(itr->second);
			}
			nodesLock.unlock();

			//Form the proper character buffer to send out
			uint64_t messageSize = message.length();
			messageSize = dgal::htonll(messageSize);

			std::string sendBuffer(sizeof(uint8_t) + sizeof(uint64_t) + message.length(), 0);

			sendBuffer[0] = dgal::BESTMESSAGE;
			memcpy(&sendBuffer[1], &messageSize, sizeof(uint64_t));
			sendBuffer += message;


			//Send to all nodes except the one that sent this message
			for(size_t i = 0; i < sendToFds.size(); i++){
				if(sendToFds[i] == dontSendTo){
					continue;
				}

				send(sendToFds[i], sendBuffer.data(), messageSize, 0);
			}

		}
	}
}


void socketWatch(const int listenSocket, std::unordered_map<int, std::shared_ptr<dgalNode> >& nodes){
	std::vector<struct pollfd> socketsToPoll;
	struct pollfd listeningPollFD;

	const short lookForEvents = POLLIN | POLLPRI;

	listeningPollFD.fd = listenSocket;
	listeningPollFD.events = lookForEvents;

	socketsToPoll.push_back(listeningPollFD);

	std::queue<int> socketsToProcess;
	std::queue<std::shared_ptr<dgalNode> > nodesToProcess;
	std::unordered_set<int> queuedSockets;

	
	processingArgContainer args(socketsToProcess, nodesToProcess, queuedSockets);


	std::queue<std::pair<int, std::string> > sendingQueue;


	size_t numThreads = std::thread::hardware_concurrency();
	if(numThreads == 0){
		std::cerr << "Could not determine number of threads to use. Defaulting to 2" << std::endl;
		numThreads = 1;
	}else{
		//Take one off to account for this thread
		numThreads--;
	}

	std::vector<std::thread> processingThreads;

	for(size_t i = 0; i < numThreads; i++){
		processingThreads.push_back(std::thread(processMessages, args, std::ref(sendingQueue)));
	}

	std::thread sendingThread(sendMessages, std::ref(nodes), std::ref(sendingQueue));

	while(true){
		//Wait a maximum of 30 seconds if no response than we either have no nodes or they all disconnected
		int res = poll(socketsToPoll.data(), socketsToPoll.size(), 3000);

		if(res > 0){
			for(unsigned int i = 0; i < socketsToPoll.size(); i++){
				if(socketsToPoll[i].revents == 0){
					continue;
				}


				if(socketsToPoll[i].fd == listenSocket){
					//We are accepting new connections
					struct pollfd newPollWatch;
					newPollWatch.fd = handleNewConnections(listenSocket, nodes);
					newPollWatch.events = lookForEvents;
					socketsToPoll.push_back(newPollWatch);
					continue;
				}


				bool foundNode = false;
				std::shared_ptr<dgalNode> node;
				nodesLock.lock();
				if(nodes.count(socketsToPoll[i].fd) != 0){
					foundNode = true;
					node = nodes[socketsToPoll[i].fd];
				}
				nodesLock.unlock();


				if(foundNode == false){
					std::cerr << "Cannot find node for socket " << socketsToPoll[i].fd << std::endl;
					shutdown(socketsToPoll[i].fd, SHUT_RDWR);
					close(socketsToPoll[i].fd);
					socketsToPoll.erase(socketsToPoll.begin()+i);
					continue;
				}

				if(socketsToPoll[i].revents & POLLERR){
					std::cerr << "POLLERR " << errno << " " << strerror(errno) << std::endl;
				}else if(socketsToPoll[i].revents & POLLHUP || socketsToPoll[i].revents & POLLRDHUP){
					close(socketsToPoll[i].fd);
					nodesLock.lock();
					nodes.erase(socketsToPoll[i].fd);
					nodesLock.unlock();
					socketsToPoll.erase(socketsToPoll.begin() + i);
				}else if(socketsToPoll[i].revents & POLLIN){
					if(queuedSockets.count(socketsToPoll[i].fd) == 1){
						//we already have processing lined up for this node
						continue;
					}
					processingLock.lock();
					socketsToProcess.push(socketsToPoll[i].fd);
					nodesToProcess.push(node);
					queuedSockets.insert(socketsToPoll[i].fd);
					processingLock.unlock();
				}

			}
		}
	}

	for(size_t i = 0; i < processingThreads.size(); i++){
		processingThreads[i].join();
	}
	sendingThread.join();
}


int handleNewConnections(const int listenSocket, std::unordered_map<int, std::shared_ptr<dgalNode> >& nodes){
	struct sockaddr_storage clientInfo;
	int clientSocket;
	socklen_t sockStoreSize = sizeof(struct sockaddr_storage);

	//Accept new connections and add them to the nodes list
	clientSocket = accept(listenSocket, (struct sockaddr *) &clientInfo, &sockStoreSize);
	if(clientSocket == -1){
		std::cerr << "Error accepting " << errno << " " << strerror(errno) << std::endl;
		return -1;
	}

	std::shared_ptr<dgalNode> newClientNode(new dgalNode(clientSocket, "test", 2));
	nodesLock.lock();
	nodes.insert(std::make_pair(clientSocket, newClientNode));
	nodesLock.unlock();

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
	std::unordered_map<int, std::shared_ptr<dgalNode> > nodes;

	if(setUpListening(listeningSocket, port.c_str()) == false){
		return 1;
	}

	socketWatch(listeningSocket, nodes);


	return 0;
}
