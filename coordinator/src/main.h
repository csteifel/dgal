#ifndef MAIN_H
#define MAIN_H value

#include <memory>
#include <vector>
#include "dgalnode.h"

bool setUpListening(int&, const char*);
int handleNewConnections(const int, std::vector<std::unique_ptr<dgalNode> >&);
void heartBeatCheck(const std::vector<std::unique_ptr<dgalNode> >&);
void socketWatch(const int);


#endif