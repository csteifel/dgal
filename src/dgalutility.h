#ifndef DGALUTILITY_H
#define DGALUTILITY_H

#include <iostream>
#include <string>

namespace dgal {

#ifdef DEBUG
	void log(std::string message){
		std::cout << message << std::endl;
	}
#else
	void log(std::string message){
		(void) message;
	}
#endif

}
#endif