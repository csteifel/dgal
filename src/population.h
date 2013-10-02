#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <utility>
#include <algorithm>
#include <atomic>
#include <ctime>
#include <thread>
#include <memory>
#include <random>
#include "individual.h"
#include "dgalutility.h"

namespace dgal {
	template <typename indType, typename messagingType> class population {
		typedef std::shared_ptr<indType> custIndPtr;
		public:
			population();
			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);
		protected:
			//virtual to allow for custom population controller
			void nextGeneration();
			void generateNewIndividuals();
			void chooseParents();
			void run();
			void initiateMessaging();
			//virtual bool checkGoals() const;

			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr > individualBuffer;
			std::atomic<bool> bufferDirty;

			size_t numIndividuals = 10;
			size_t generationNum = 0;
			size_t maxGeneration = 10;

			bool stop = false;

	};

	template <typename indType, typename messagingType> population<indType, messagingType>::population(){
		bufferDirty.store(false);
		if(generationNum == 0){
			generateNewIndividuals();
		}

		std::srand(std::time(0));

		std::thread t(&population<indType, messagingType>::initiateMessaging, this);
		run();
		stop = true;
		t.join();
	}

	template <typename indType, typename messagingType> void population<indType, messagingType>::initiateMessaging(){
		//TODO: implement
		messagingType messageController;
		std::vector<std::pair<std::string, double> > bestContainer;
		while(true){
			messageController.getBests(bestContainer);
			
			bestContainer.clear();
			if(stop){
				return;
			}
		}
	}


	template <typename indType, typename messagingType> void population<indType, messagingType>::addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders){
		for(size_t i = 0; i < outsiders.size(); i++){
			custIndPtr recieved(new indType(outsiders[i].first, outsiders[i].second));
			individualBuffer.push_back(std::move(recieved));
		}
		bufferDirty.store(true);
	}

	template <typename indType, typename messagingType> void population<indType, messagingType>::nextGeneration(){
		//Pick out bests if not just starting up
		chooseParents();

		//Fill in the rest of the population with new individuals
		generateNewIndividuals();


		dgal::log("Sorting");
		std::sort(individuals.begin(), individuals.end(),
			[](const std::shared_ptr<dgal::individual> a, const std::shared_ptr<dgal::individual> b){
				if(a->getFitness() <= b->getFitness()){ return false; } return true;
			});


		for(size_t i = 0; i < individuals.size(); i++){
			dgal::log("Fitness " + std::to_string(individuals[i]->getFitness()));
		}
	
	}

	template <typename indType, typename messagingType> void population<indType, messagingType>::generateNewIndividuals(){
		while(individuals.size() < numIndividuals){
			dgal::log("Adding individual");
			individuals.push_back(custIndPtr(new indType));
		}
	}

	template <typename indType, typename messagingType> void population<indType, messagingType>::chooseParents(){
		//TODO: implement
	}

	template <typename indType, typename messagingType> void population<indType, messagingType>::run(){
		dgal::log("Running generation " + std::to_string(generationNum));

		//Run this generation
		//TODO: put in thread pool individual running
		for(size_t i = 0; i < individuals.size(); i++){
			individuals[i]->run();
		}

		++generationNum;

		if(generationNum < maxGeneration){
			nextGeneration();
			run();
		}
	}
}

#endif