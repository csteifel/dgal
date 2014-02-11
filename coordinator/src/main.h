#ifndef MAIN_H
#define MAIN_H value

#include <memory>
#include <vector>
#include "dgalnode.h"

void setUpListening(int&);
void handleNewConnections(const char *, std::vector<std::unique_ptr<dgalNode> >&);
void heartBeatCheck(const std::vector<std::unique_ptr<dgalNode> >&);

#endif