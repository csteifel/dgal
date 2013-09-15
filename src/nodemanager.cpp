#include "nodemanager.h"

#include <thread>
#include <iostream>
#include <random>
#include <ctime>

namespace dgal {
	nodeManager::nodeManager(){
		initialize();
	}

	void nodeManager::initialize(){
		//TODO: this is included because of temporary random generation scheme that should be replaced
		std::srand(std::time(0));

		std::cout << "TEST" << std::endl;
	}
}