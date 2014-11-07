#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <memory>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace dgal {
	class individual {
		public:
			individual() = delete;
			individual(const size_t numWeights);

			//For use when creating from serialization 
			individual(const std::string& uuid);

			individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB);

//			static std::shared_ptr<dgal::individual> createFromSerialized(const std::string serialized){return std::shared_ptr<dgal::individual>(NULL);};
//			virtual std::string serialize() const = 0;
			virtual const std::string serialize() const = 0;
			virtual void run() = 0;

			const std::string generateId();
			const std::string wrapSerialization(const std::string&&) const;

			void print() const; 
			double getFitness() const {return fitness;}
			double getProbability() const {return probability;}

			const std::string uniqueID; //Ok to be public since it is const
		protected:
//			virtual void mutate();

			std::vector<double> weights;
			double probability;
			double fitness;

			bool fitnessPreCalced = false;
	};

void individual::print() const{
	for(size_t i = 0; i < weights.size(); ++i) { 
		if(i==0) 	 	std::cout<<"Pigs: ";
		else if(i==1)	 	std::cout<<"Cows: ";
		else if(i==2) 	std::cout<<"Chicken ";
		else if(i==3)		std::cout<<"Horse ";
		else if(i==4)		std::cout<<"Sheep ";
		else if(i==5)		std::cout<<"Donkey ";
		else if(i==6)		std::cout<<"Goat ";
		else if(i==7)		std::cout<<"Lamb ";
		else if(i==8)		std::cout<<"Rooster ";
		else if(i==9)		std::cout<<"Goose ";
		std::cout << weights[i] << " "<<std::endl;
	} 
}
	


	inline individual::individual(const size_t numWeights) : weights(numWeights), uniqueID(generateId()) {
		//Generate random weights for this individual
		std::random_device rd;
		std::mt19937_64 randEng(rd());
		//Why do we use numeric limits of int here you ask?
		//Its because if you do numeric limits of double you get infinity every time you ask for a random
		//because max-min for doubles is greater than numeric limits Real numbers
		std::uniform_real_distribution<double> rg(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

		for(size_t i = 0; i < numWeights; i++){
			//TODO: fix random generation to be better and more random
			weights[i] = rg(randEng);
		}	
	}

	inline individual::individual(const std::string& uuid) : uniqueID(uuid){}

	inline individual::individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB) : uniqueID(generateId()){
		//TODO: switch away from C style random
		weights.resize(parentA->weights.size());
		size_t cutOff = std::rand() % weights.size();
		for(size_t i = 0; i < weights.size(); i++){
			weights[i] = (i < cutOff) ? parentA->weights[i] : parentB->weights[i];
		}
	}

	inline const std::string individual::generateId(){
		return to_string(boost::uuids::random_generator()());
	}
}

#endif
