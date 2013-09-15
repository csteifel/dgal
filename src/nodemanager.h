#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <memory>
#include <thread>
#include <iostream>
#include <ctime>
#include "messenger.h"
#include "population.h"

namespace dgal {
	template <typename indType> class nodeManager {
		public:
			nodeManager();
			void initialize();
		private:
			//dgal::population<indType> pop;
			std::unique_ptr<dgal::messenger> nodeMessenging;
	};



	template<typename indType> nodeManager<indType>::nodeManager(){
		initialize();
	}

	template <typename indType> void nodeManager<indType>::initialize(){
		//TODO: this is included because of temporary random generation scheme that should be replaced
		std::srand(std::time(0));

		std::cout << "TEST" << std::endl;
	}

}

#endif