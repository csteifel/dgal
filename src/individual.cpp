#include "individual.h"

#include <memory>
#include <random>
#include <limits>
#include <iostream>

dgal::individual::individual(const size_t numWeights) : weights(numWeights) {
	//Generate random weights for this individual
	for(size_t i = 0; i < numWeights; i++){
		//TODO: fix random generation to be better and more random
		weights[i] = (double) std::rand() / RAND_MAX * ((std::rand() % 3) - 1) * std::numeric_limits<double>::max();
	}
}

dgal::individual::individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB){
	//TODO: switch away from C style random
	weights.resize(parentA->weights.size());
	size_t cutOff = std::rand() % weights.size();
	for(size_t i = 0; i < weights.size(); i++){
		weights[i] = (i < cutOff) ? parentA->weights[i] : parentB->weights[i];
	}
}