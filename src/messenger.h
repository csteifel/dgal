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
			virtual std::vector<std::string> getBests(std::vector<std::pair<std::string, double> >& bests) const = 0;
			virtual std::vector<std::string> sendBests(const std::vector<std::pair<std::string, double> >& bests) const = 0;
		private:
	};
}


#endif