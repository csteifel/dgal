#ifndef DGALNODE_H
#define DGALNODE_H value

#include <chrono>
#include <string>

class dgalNode{
	public:
		dgalNode(const int socket, const std::string addr, const unsigned short port) : sockfd(socket), listenPort(port), addressInfo(addr){}
		dgalNode(const dgalNode&) = delete;
		void recievedHeartBeat();
		int getSockFD() const {return sockfd;}
	private:
		const int sockfd;
		typedef std::chrono::steady_clock clock;
		unsigned short listenPort;
		std::string addressInfo;
		std::chrono::time_point<clock> hbcStart, hbcEnd;
};

#endif