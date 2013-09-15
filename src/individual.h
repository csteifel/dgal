#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace dgal {
	class individual {
		public:
			individual() = delete;
			individual(const size_t numWeights);
			individual(const std::shared_ptr<dgal::individual> parentA, const std::shared_ptr<dgal::individual> parentB);

//			virtual void createFromSerialized(const std::string serialized) = 0;
//			virtual std::string serialize() const = 0;
			double getFitness() const {return fitness;}
		protected:
//			virtual void mutate();

			double fitness;
			std::vector<double> weights;

			bool fitnessPreCalced = false;
	};

}
#endif