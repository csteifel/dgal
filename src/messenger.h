#ifndef MESSENGER_H
#define MESSENGER_H


#include <vector>
#include <string>
#include <utility>

namespace dgal {
	class messenger {
		/**
		* Base class/interface for messaging options
		*/
		public:
			//Get bests from other nodes
			virtual void getBests(std::vector<std::pair<std::string, double> >& bests) const = 0;
			virtual void sendBests(const std::vector<std::pair<std::string, double> >& bests) const = 0;
		private:
	};
}


#endif