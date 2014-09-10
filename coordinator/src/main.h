#ifndef MAIN_H
#define MAIN_H value

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "dgalnode.h"


namespace{
	struct processingArgContainer {
		processingArgContainer() = delete;
		processingArgContainer(std::queue<int>& sTP, std::queue<std::shared_ptr<dgalNode> >& nTP, std::unordered_set<int>& qS) : socketsToProcess(sTP), nodesToProcess(nTP), queuedSockets(qS) {}
		std::queue<int>& socketsToProcess;
		std::queue<std::shared_ptr<dgalNode> >& nodesToProcess;
		std::unordered_set<int>& queuedSockets;
	};
}



bool setUpListening(int&, const char*);
int handleNewConnections(const int, std::unordered_map<int, std::shared_ptr<dgalNode> >&);
void socketWatch(const int);
void processMessages(processingArgContainer, std::queue<std::pair<int, std::string> >&);
void sendMessages(std::unordered_map<int, std::shared_ptr<dgalNode> >&, std::queue<std::pair<int, std::string> >&);
#endif