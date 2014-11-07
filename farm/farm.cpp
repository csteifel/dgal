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
		Farm() : Farm(10) {}
		Farm(const size_t numAnimals);
		bool unfilled(const size_t numAnimals);
		void print() const { for(size_t i = 0; i < weights.size(); ++i) { std::cout << weights[i] << " "; } std::cout << std::endl; }

		virtual void run();
		virtual const std::string serialize() const { return ""; }

		static std::shared_ptr<dgal::individual> createFromSerialized(const std::string uuid, const double precalcedFitness,
					std::string&& serialization) { return std::shared_ptr<dgal::individual>(new Farm()); }

	private:
		double funds;
		double acreage;
		std::vector<Animal> animals;
		std::vector<int> maxAnimals;
};

Farm::Farm(const size_t numAnimals) : dgal::individual(numAnimals) {
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

	for (size_t i = 0; i < numAnimals; ++i) {
		weights[i] = 0;							// Using weights slightly differently
	}

	while (unfilled(numAnimals)) {
		int randAnimal = rand() % 10;
		if(maxAnimals[randAnimal] > 0) {
			if(funds < animals[randAnimal].getCost() || acreage < animals[randAnimal].getSpace()) {
				continue;
			}

			funds -= animals[randAnimal].getCost();
			acreage -= animals[randAnimal].getSpace();
			--maxAnimals[randAnimal];
			++weights[randAnimal];
		}
	}
}

bool Farm::unfilled(const size_t numAnimals){
	for(size_t i = 0; i < numAnimals; ++i) {
		if(maxAnimals[i] > 0 ) {
			if(funds > animals[i].getCost() && acreage > animals[i].getSpace()) {
					return true;
			}
		}
	}
	return false;
}

void Farm::run() {
	for (size_t i = 0; i < maxAnimals.size(); ++i) {
		fitness += weights[i] * animals[i].getOutput();
	}	
}

int main(){

	// Test Farm individuals
	std::srand(std::time(0));
	std::shared_ptr<Farm> A(new Farm(10));
	std::shared_ptr<Farm> B(new Farm(10));
	std::shared_ptr<Farm> C(new Farm(10));
	std::shared_ptr<Farm> D(new Farm(10));
	std::shared_ptr<Farm> E(new Farm(10));
	std::shared_ptr<Farm> F(new Farm(10));

	A->run();
	B->run();
	C->run();
	D->run();
	E->run();
	F->run();

	std::cout << "Testing 6 Farm Individuals\n";
	std::cout << "Pig Cow Chicken Horse Sheep Donkey Goat Lamb Rooster Goose\n";
	std::cout << "A - Number of Each Animal: ";
	A->print();
	std::cout << "Fitness: " << A->getFitness() << std::endl;
	std::cout << "B - Number of Each Animal: ";
	B->print();
	std::cout << "Fitness: " << B->getFitness() << std::endl;
	std::cout << "C - Number of Each Animal: ";
	C->print();
	std::cout << "Fitness: " << C->getFitness() << std::endl;
	std::cout << "D - Number of Each Animal: ";
	D->print();
	std::cout << "Fitness: " << D->getFitness() << std::endl;
	std::cout << "E - Number of Each Animal: ";
	E->print();
	std::cout << "Fitness: " << E->getFitness() << std::endl;
	std::cout << "F - Number of Each Animal: ";
	F->print();
	std::cout << "Fitness: " << F->getFitness() << std::endl;
	std::cout << "End Testing of Individuals\n";

	std::cout << "\nBegin Population Testing\n";
	dgal::population<Farm> pop;

	return 0;
}
