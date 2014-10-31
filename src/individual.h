#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <memory>
#include <random>
#include <vector>
#include <string>
#include <iostream>

namespace dgal {
	class individual {
		public:
			individual() = delete;
			individual(const size_t numWeights);
			individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB);

//			static std::shared_ptr<dgal::individual> createFromSerialized(const std::string serialized){return std::shared_ptr<dgal::individual>(NULL);};
//			virtual std::string serialize() const = 0;
			virtual void fitness() = 0;

			double getFitness() const {return fitness;}
			double getProbability() const {return probability;}
		protected:
//			virtual void mutate();

			double fitness;
			std::vector<double> weights;
			double probability;

			bool fitnessPreCalced = false;
	};


	individual::individual(const size_t numWeights) : weights(numWeights) {
		//Generate random weights for this individual
		for(size_t i = 0; i < numWeights; i++){
			//TODO: fix random generation to be better and more random
			weights[i] = (double) std::rand() / RAND_MAX * ((std::rand() % 3) - 1) * std::numeric_limits<double>::max();
		}
	}

	individual::individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB){
		//TODO: switch away from C style random
		weights.resize(parentA->weights.size());
		size_t cutOff = std::rand() % weights.size();
		for(size_t i = 0; i < weights.size(); i++){
			weights[i] = (i < cutOff) ? parentA->weights[i] : parentB->weights[i];
		}
	}

}

#endif
