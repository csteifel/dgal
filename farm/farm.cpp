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
		9.  Goose
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

class Animal
{
public:
	Animal(const size_t c, const size_t o, const size_t s) : cost(c), output(o), space(s) {}

	double getCost() { return cost; }
	double getOutput() { return output; }
	double getSpace() { return space; }

private:
	double cost;
	double output;
	double space;
};

class Farm : public dgal::individual {
	public:
		// Number of animals in farm defaults to 10
		Farm(const size_t numAnimals);
		bool unfilled(const size_t numAnimals);

		void run();

	private:
		double funds;
		double acreage;
		std::vector<Animal> animals;
		std::vector<int> maxAnimals;
};

farm::farm(const size_t numAnimals) : weights(numAnimals), animals(numAnimals), maxAnimals(numAnimals) {
	funds = 10000;
	acreage = 20;
	fitness = 0;
	
	animals.push_back(Animal(100, 1000, 0.2));	// Pig
	animals.push_back(Animal(200, 6000, 1.5));  // Cow
	animals.push_back(Animal(15, 300, 0.0125)); // Chicken
	animals.push_back(Animal(450, 3000, 1));	// Horse
	animals.push_back(Animal(125, 1250, 0.25)); // Sheep
	animals.push_back(Animal(175, 1000, 0.5));  // Donkey
	animals.push_back(Animal(75, 1250, 0.25));  // Goat
	animals.push_back(Animal(125, 2500, 0.25)); // Lamb
	animals.push_back(Animal(35, 350, 0.0125)); // Rooster
	animals.push_back(Animal(25, 300, 0.2));    // Goose

	maxAnimals.push_back(std::rand() % 100);
	maxAnimals.push_back(std::rand() % 13);
	maxAnimals.push_back(std::rand() % 666);
	maxAnimals.push_back(std::rand() % 20);
	maxAnimals.push_back(std::rand() % 80);
	maxAnimals.push_back(std::rand() % 40);
	maxAnimals.push_back(std::rand() % 80);
	maxAnimals.push_back(std::rand() % 80);
	maxAnimals.push_back(std::rand() % 285);
	maxAnimals.push_back(std::rand() % 100);

	while (unfilled(numAnimals)) {
		int randAnimal = rand() % 10;
		if(maxAnimals[randAnimal] > 0) {
			if(funds < animals[randAnimal].getCost() || space < animals[randAnimal].getSpace()) {
				continue;
			}

			funds -= animals[randAnimal].getCost();
			space -= animals[randAnimal].getSpace();
			--maxAnimals[randAnimal];
			++weights[randAnimal];
		}
	}
}

bool farm::unfilled(const size_t numAnimals){
	for(int i = 0; i < numAnimals; ++i) {
		if(maxAnimals[i] > 0 ) {
			if(funds > animals[i].getCost() && space > animals[i].getSpace()) {
					return true;
			}
		}
	}
	return false;
}

void farm::fitness(){
	for (int i = 0; i < maxAnimals.size(); ++i) {
		weights[i] *= animals[i].getOutput();
		fitness += weights[i];
	}	
}

int main(){

	//farming custom individuals
	std::srand(std::time(0));
	std::shared_ptr<farm> A(new farm(10));
	std::shared_ptr<farm> B(new farm(10));
	std::shared_ptr<farm> C(new farm(10));
	std::shared_ptr<farm> D(new farm(10));

	std::cout << "****************************\nA ---\n";
	A->print();
	std::cout << "****************************\nB ---\n";
	B->print();
	std::cout << "****************************\nC ---\n";
	C->print();
	std::cout << "****************************\nD ---\n";
	D->print();

	return 0;
}
