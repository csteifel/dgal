#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <memory>
#include "messenger.h"
#include "population.h"

namespace dgal {
	class nodeManager {
		public:
			
		private:
			dgal::population pop;
			std::unique_ptr<dgal::messenger> nodeMessenging;
	};
}

#endif