#ifndef MAIN_H
#define MAIN_H value

#include <memory>
#include <vector>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "dgalnode.h"

bool setUpListening(int&, const char*);
int handleNewConnections(const int, std::vector<std::shared_ptr<dgalNode> >&);
void socketWatch(const int);
void processMessages(std::queue<int>&, std::queue<std::shared_ptr<dgalNode> >&, std::unordered_set<int>&, std::mutex&);
#endif