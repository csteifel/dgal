#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <string>

namespace dgal {
	class individual {
		public:
			virtual std::string serialize() = 0;
	};

}
#endif