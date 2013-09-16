#ifndef POPULATION_H
#define POPULATION_H

#include "individual.h"
#include <vector>
#include <utility>
#include <algorithm>
#include "dgalutility.h"

namespace dgal {
	template <typename indType> class population {
		typedef std::shared_ptr<indType> custIndPtr;
		public:
			population();
			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);
		protected:
			//virtual to allow for custom population controller
			virtual void nextGeneration();
			virtual void generateNewIndividuals();
			virtual void chooseParents();
			//virtual bool checkGoals() const;

			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr > individualBuffer;
			size_t numIndividuals = 10;
			size_t generationNum = 0;
	};


	template <typename indType> population<indType>::population(){
		nextGeneration();
	}

	template <typename indType> void population<indType>::nextGeneration(){
		if(individuals.size() != 0){
			//Pick out bests if not just starting up
			chooseParents();
		}

		//Fill in the rest of the population with new individuals
		generateNewIndividuals();


		//Run this generation
		dgal::log("Running generation " + std::to_string(generationNum));
		for(size_t i = 0; i < individuals.size(); i++){
			individuals[i]->run();
		}

		std::sort(individuals.begin(), individuals.end());


		for(size_t i = 0; i < individuals.size(); i++){
			dgal::log("Fitness " + std::to_string(individuals[i]->getFitness()));
		}


	}

	template <typename indType> void population<indType>::chooseParents(){
		//TODO: implement
	}

	template <typename indType> void population<indType>::generateNewIndividuals(){
		while(individuals.size() < numIndividuals){
			dgal::log("Adding individual");
			individuals.push_back(custIndPtr(new indType));
		}
	}
}

#endif