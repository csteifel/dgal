#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <string>
#include <iostream>

namespace dgal {
	class individual {
		public:
			individual() = delete;
			individual(const int numWeights);
			individual(const std::string serialized, const double preCalcedFitness);
			individual(const dgal::individual * parentA, const dgal::individual * parentB){std::cout << "individual constructor" << std::endl;}

			virtual std::string serialize() const = 0;
			double getFitness() const {return fitness;}
		protected:
			double fitness;
			std::vector<double> weights;
	};

}
#endif