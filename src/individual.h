#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <string>

namespace dgal {
	class individual {
		public:
			virtual std::string serialize() const = 0;
			const double getFitness() const {return fitness;}
		protected:
			double fitness;
	};

}
#endif