#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include "individual.h"

namespace dgal {
	class population {
		public:
			
		protected:
			//virtual to allow for custom population controller
			virtual void nextGeneration();
			virtual void generateNewIndividuals();
			std::vector<dgal::individual> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<dgal::individual> individualBuffer;
	};
}

#endif