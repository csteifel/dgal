/*
	Sample Test Problem
	-Farms (Individuals) are given $10,000 to spend on Farm animals and and 20 acres to operate on
	-Fitness Function Calculates the output of the farm

	Goal: Maximize output value of farm 

	Farm Animals
		0.  Pig
				Cost=100
				Output=1000
				Space=0.2
		1.  Cow 
				Cost=200
				Output=6000
				Space=1.5
		2.  Chicken
				Cost=15
				Output=300
				Space=0.0125
		3.  Horse
				Cost=450
				Output=3000
				Space=1
		4.  Sheep
				Cost=125
				Output=1250
				Space=0.25
		5.  Donkey
				Cost=175
				Output=1000
				Space=0.5
		6.  Goat
				Cost=75
				Output=1250
				Space=0.25
		7.  Lamb
				Cost=125
				Output=2500
				Space=0.25
		8.  Rooster
				Cost=35
				Output=350
				Space=0.0125
		9. Goose
				Cost=25
				Output=300
				Space=0.2
*/

#include "population.h"
#include "messenger.h"
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <utility>
#include <unistd.h>

class farm : public dgal::individual {
	public:
		//Number of animals in farm defaults to 10
		farm() : dgal::individual() {}
		farm(const size_t numAnimals);
		farm(const std::shared_ptr<farm> a, const std::shared_ptr<farm> b) : dgal::individual(a, b) {}
		void print(){ for(size_t i = 0; i < weights.size(); i++){ std::cout << weights[i] << " "; } std::cout << std::endl; }
		void fitness(){}
		static std::shared_ptr<dgal::individual> createFromSerialized(std::string){return std::shared_ptr<dgal::individual>(NULL);}

		bool unfilled(const size_t numAnimals);

	protected:
		double funds;
		double space;
		std::vector<double> maxAnimals;
};

farm::farm(const size_t numAnimals) : weights(numAnimals), maxAnimals(numAnimals) {
	funds=10000;
	space=20;
	fitness=0;
	probability=0;
	
	//maxAnimals determined by limiting factor (cost or space)
	int type=0;
	while(type<10){
		//0.  Pig, Cost=100, Output=1000, Space=0.2
		if(type==0){
			maxAnimals[type]=(double) std::rand() % 100;
		}
		//1.  Cow, Cost=200, Output=6000, Space=1.5
		else if(type==1){
			maxAnimals[type]=(double) std::rand() % 13;
		}
		//2.  Chicken, Cost=15, Output=300,	Space=0.0125
		else if(type==2){
			maxAnimals[type]=(double) std::rand() % 666;
		}
		//3.  Horse, Cost=450, Output=3000, Space=1
		else if(type==3){
			maxAnimals[type]=(double) std::rand() % 20;
		}
		//4.  Sheep, Cost=125, Output=1250, Space=0.25
		else if(type==4){
			maxAnimals[type]=(double) std::rand() % 80;
		}
		//5.  Donkey, Cost=175, Output=1000, Space=0.5
		else if(type==5){
			maxAnimals[type]=(double) std::rand() % 40;
		}
		//6.  Goat, Cost=75, Output=1250, Space=0.25
		else if(type==6){
			maxAnimals[type]=(double) std::rand() % 80;
		}
		//7.  Lamb,	Cost=125, Output=2500,	Space=0.25
		else if(type==7){
			maxAnimals[type]=(double) std::rand() % 80;
		}
		//8.  Rooster, Cost=35, Output=350,	Space=0.0125
		else if(type==8){
			maxAnimals[type]=(double) std::rand() % 285;
		}
		//9. Goose, Cost=25, Output=300, Space=0.2		
		else if(type==9){
			maxAnimals[type]=(double) std::rand() % 100;
		}
		++type;
	}

	while(unfilled(numAnimals)){
		int temp = rand() % 10;
		if(maxAnimals[temp] > 0){
			//0.  Pig, Cost=100, Output=1000, Space=0.2
			if(temp==0){
				if(funds <100 || space < 0.2){
					continue;
				}
				funds-=100;
				space-=.2;
			}
			//1.  Cow, Cost=200, Output=6000, Space=1.5
			else if(temp==1){
				if(funds <200 || space < 1.5){
					continue;
				}
				funds-=200;
				space-=1.5;
			}
			//2.  Chicken, Cost=15, Output=300,	Space=0.0125
			else if(temp==2){
				if(funds < 15 || space < 0.0125){
					continue;
				}
				funds-=15;
				space-=0.0125;
			}
			//3.  Horse, Cost=450, Output=3000, Space=1
			else if(temp==3){
				if(funds < 450 || space < 1){
					continue;
				}
				funds-=450;
				space-=1;
			}
			//4.  Sheep, Cost=125, Output=1250, Space=0.25
			else if(temp==4){
				if(funds < 125 || space < 0.25){
					continue;
				}
				funds-=125;
				space-=0.25;
			}
			//5.  Donkey, Cost=175, Output=1000, Space=0.5
			else if(temp==5){
				if(funds < 175 || space < 0.5){
					continue;
				}
				funds-=175;
				space-=0.5;
			}
			//6.  Goat, Cost=75, Output=1250, Space=0.25
			else if(temp==6){
				if(funds < 75 || space < 0.25){
					continue;
				}
				funds-=75;
				space-=0.25;
			}
			//7.  Lamb,	Cost=125, Output=2500,	Space=0.25
			else if(temp==7){
				if(funds < 125 || space < 0.25){
					continue;
				}
				funds-=125;
				space-=0.25;
			}
			//8.  Rooster, Cost=35, Output=350,	Space=0.0125
			else if(temp==8){
				if(funds < 35 || space < 0.0125){
					continue;
				}
				funds-=35;
				space-=0.0125;
			}
			//9. Goose, Cost=25, Output=300, Space=0.2		
			else if(temp==9){
				if(funds < 25 || space < 0.2){
					continue;
				}
				funds-=25;
				space-=0.2;
			}
			--maxAnimals[temp];
			++weights[temp];
		}
	}
}

bool farm::unfilled(const size_t numAnimals){
	for(int i=0; i<numAnimals; ++i){
		//0.  Pig, Cost=100, Output=1000, Space=0.2
		if(i==0){
			if(maxAnimals[i]>0){
				if(funds>100&&space>0.2){
					return true;
				}
			}
		}
		//1.  Cow, Cost=200, Output=6000, Space=1.5
		else if(i==1){
			if(maxAnimals[i]>0){
				if(funds>200&&space>1.5){
					return true;
				}	
			}	
		}
		//2.  Chicken, Cost=15, Output=300,	Space=0.0125
		else if(i==2){
			if(maxAnimals[i]>0){
				if(funds>15&&space>0.0125){
					return true;
				}
			}		
		}
		//3.  Horse, Cost=450, Output=3000, Space=1
		else if(i==3){
			if(maxAnimals[i]>0){
				if(funds>450&&space>1){
					return true;
				}
			}	
		}
		//4.  Sheep, Cost=125, Output=1250, Space=0.25
		else if(i==4){
			if(maxAnimals[i]>0){
				if(funds>125&&space>0.25){
					return true;
				}
			}	
		}
		//5.  Donkey, Cost=175, Output=1000, Space=0.5
		else if(i==5){
			if(maxAnimals[i]>0){
				if(funds>175&&space>0.5){
					return true;
				}
			}	
		}
		//6.  Goat, Cost=75, Output=1250, Space=0.25
		else if(i==6){
			if(maxAnimals[i]>0){
				if(funds>75&&space>0.25){
					return true;
				}
			}	
		}
		//7.  Lamb,	Cost=125, Output=2500,	Space=0.25
		else if(i==7){
			if(maxAnimals[i]>0){
				if(funds>125&&space>0.25){
					return true;
				}
			}	
		}
		//8.  Rooster, Cost=35, Output=350,	Space=0.0125
		else if(i==8){
			if(maxAnimals[i]>0){
				if(funds>35&&space>0.0125){
					return true;
				}
			}		
		}
		//9. Goose, Cost=25, Output=300, Space=0.2		
		else if(i==9){
			if(maxAnimals[i]>0){
				if(funds>25&&space>0.2){
					return true;
				}
			}		
		}
	}
	return false;
}

void farm::fitness(){
	for (int i = 0; i < maxAnimals.size(); ++i){
		//0.  Pig, Cost=100, Output=1000, Space=0.2
		if(i==0){
			weights[i]*=1000;
			fitness+=weights[i];
		}
		//1.  Cow, Cost=200, Output=6000, Space=1.5
		else if(i==1){
			weights[i]*=6000;
			fitness+=weights[i];
		}
		//2.  Chicken, Cost=15, Output=300,	Space=0.0125
		else if(i==2){
			weights[i]*=300;
			fitness+=weights[i];
		}
		//3.  Horse, Cost=450, Output=3000, Space=1
		else if(i==3){
			weights[i]*=3000;
			fitness+=weights[i];
		}
		//4.  Sheep, Cost=125, Output=1250, Space=0.25
		else if(i==4){
			weights[i]*=1250;
			fitness+=weights[i];
		}
		//5.  Donkey, Cost=175, Output=1000, Space=0.5
		else if(i==5){
			weights[i]*=1000;
			fitness+=weights[i];
		}
		//6.  Goat, Cost=75, Output=1250, Space=0.25
		else if(i==6){
			weights[i]*=1250;
			fitness+=weights[i];
		}
		//7.  Lamb,	Cost=125, Output=2500,	Space=0.25
		else if(i==7){
			weights[i]*=2500;
			fitness+=weights[i];
		}
		//8.  Rooster, Cost=35, Output=350,	Space=0.0125
		else if(i==8){
			weights[i]*=350;
			fitness+=weights[i];
		}
		//9. Goose, Cost=25, Output=300, Space=0.2		
		else if(i==9){
			weights[i]*=300;
			fitness+=weights[i];
		}
	}	
}



int main(){

	//farming custom individuals
	std::srand(std::time(0));
	std::shared_ptr<farm> A(new farm(10));
	std::shared_ptr<farm> B(new farm(10));

	std::cout << "A:\n";
	A->print();
	std::cout << "B:\n";
	B->print();
	//End farming of individuals

	// dgal::population<farm> pop;

	//Start farming of node manager
	//dgal::nodeManager<farm, aaa> node;
	dgal::log("Finished");

	return 0;
}
