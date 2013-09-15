#ifndef POPULATION_H
#define POPULATION_H

#include "individual.h"
#include <vector>
#include <utility>

namespace dgal {
	template <typename T> class population {
		typedef std::shared_ptr<T> custIndPtr;
		public:
			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);
		protected:
			//virtual to allow for custom population controller
			virtual void nextGeneration();
			//virtual void generateNewIndividuals();
			//virtual bool checkGoals() const;
			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr > individualBuffer;
	};


	template <typename indType> void population<indType>::nextGeneration(){
		//TODO: implementation
		std::cout << "TEST" << std::endl;
	}
	
}

#endif