#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <utility>
#include "individual.h"

namespace dgal {
	class population {
		public:
			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);
		protected:
			//virtual to allow for custom population controller
			virtual void nextGeneration();
			virtual void generateNewIndividuals();
			virtual bool checkGoals() const;
			std::vector<dgal::individual> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<dgal::individual> individualBuffer;
	};
}

#endif