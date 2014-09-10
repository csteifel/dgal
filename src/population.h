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
#include <fstream>
#include "individual.h"
#include "dgalutility.h"

namespace dgal {
	template <typename indType, typename messagingType> class population {
		typedef std::shared_ptr<indType> custIndPtr;
		public:
			population();
			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);
		protected:
			void nextGeneration();
			void generateNewIndividuals();
			void chooseParents();
			void run();
			void initiateMessaging();
			bool checkGoals() const; //potentially virtual later 'to allow for custom population controller'

			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr > individualBuffer;
			std::atomic<bool> bufferDirty;

			clock_t initClock;
			size_t numIndividuals = 10;
			size_t generationNum = 0;
			size_t maxGeneration = -1;
			size_t maxFitnessLevel = -1;
			size_t maxTime = -1;

			bool stop = false;
	};


	template <typename indType, typename messagingType> population<indType, messagingType>::population(){
		initClock = clock();
		std::ifstream cfgFile("info.cfg");
		if (!cfgFile){ 
			dgal::log("ERROR: cannot find config file 'info.cfg'"); 
			return;
		}

		std::string token, condition, goalCond;
		double parameter;
		goalCond = "Goal_Condition:";
		while(cfgFile >> token){ //Ignore instructions to begin reading
			if (token == "***"){
				break;
			}
		}
		while (cfgFile >> token){
			if (token == goalCond){
				cfgFile >> condition >> parameter;
				if (condition == "numGen"){
					maxGeneration = parameter;
					dgal::log("Goal Condition set: Max number of generations = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "fitnessLevel"){
					maxFitnessLevel = parameter;
					dgal::log("Goal Condition set: Fitness level by most fit individual = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "time"){
					maxTime = parameter;
					dgal::log("Goal Condition set: Max amount of time can pass in seconds = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "manual"){
					//TODO: implement
				}
			}
		}
		cfgFile.close();

		bufferDirty.store(false);
		if(generationNum == 0){
			generateNewIndividuals();
		}

		std::srand(std::time(0));

//		std::thread t(&population<indType, messagingType>::initiateMessaging, this);
		run();
		stop = true;
//		t.join();
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
			dgal::log("Fitness <to_string>individual[i]->getFitness()");// + std::to_string(individuals[i]->getFitness()));
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
		dgal::log("Running generation <to_string>generationNum"); //+ std::to_string(generationNum));

		//Run this generation
		//TODO: put in thread pool individual running
		for(size_t i = 0; i < individuals.size(); i++){
			individuals[i]->run();
		}

		++generationNum;

		if(checkGoals() == false){
			nextGeneration();
			run();
		}
	}

	template <typename indType, typename messagingType> bool population<indType, messagingType>::checkGoals() const{
		if (maxGeneration != -1 && generationNum >= maxGeneration){
			dgal::log("Goal Satisfied: Max Generation met");
			return true;
		}
		else if (maxFitnessLevel != -1)
		{
			if(individuals[0]->getFitness() >= maxFitnessLevel)
			{
				dgal::log("Goal Satisfied: Fitness Level achieved (local node)");
				return true;
			}
			//Assuming Individual Buffer is not sorted. Can be changed later if sorted
			for (std::vector<custIndPtr>::iterator i = individualBuffer.begin(); i != individualBuffer.end(); ++i)
			{
				if(*i->getFitness() >= maxFitnessLevel)
				{
					dgal::log("Goal Satisfied: Fitness Level achieved (outside node)");
					return true;
				}
			}
		}
		else if (maxTime != -1 && (clock() - initClock)/CLOCKS_PER_SEC >= maxTime){
			dgal::log("Goal Satisfied: Time has run out");
			return true;
		}
		return false;
	}
}

#endif