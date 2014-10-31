#include "population.h"
#include "messenger.h"
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <utility>
#include <unistd.h>



double arr[] = {1.5,-65,4,-0.1151,8,30,-11,27,.222,-5.3};

class test : public dgal::individual {
	public:
		test() : dgal::individual(10) {}
		test(const std::shared_ptr<test> a, const std::shared_ptr<test> b) : dgal::individual(a, b) {}
		void print() const{ for(size_t i = 0; i < weights.size(); i++){ std::cout << weights[i] << " "; } std::cout << std::endl; }

		virtual void run();

		static std::shared_ptr<dgal::individual> createFromSerialized(const std::string, const double, std::string&&);
		virtual const std::string serialize() const;

};


void test::run(){
	fitness = 0;
	for(size_t i = 0; i < weights.size(); i++){
		fitness += weights[i] * arr[i];
	}
	usleep(2500);
}

std::shared_ptr<dgal::individual> test::createFromSerialized(const std::string uuid, const double precalcedFitness, std::string&& serialization){
	/*
	test * newFromSerial = new test();
	size_t i = 0;
	size_t offset = 0;
	size_t foundAt = std::string::npos;
	while((foundAt = serialized.find_first_of(",", offset)) != std::string::npos){
		newFromSerial->weights[i] = atof(serialized.substr(offset, foundAt).c_str());
		offset = foundAt + 1;
		++i;
	}
	newFromSerial->weights[i] = atof(serialized.substr(offset).c_str());
	newFromSerial->run();
	
	return std::shared_ptr<dgal::individual>(newFromSerial);
	*/

	//Fix this to actually create from serialized now
	return std::shared_ptr<dgal::individual>(new test());
}


const std::string test::serialize() const{
	std::string retVal;

	for(size_t i = 0; i < weights.size(); i++){
		retVal += std::to_string(weights[i]);
		if(i != weights.size()-1){
			retVal += ",";
		}
	}

	return retVal;
}




int main(){

	//Testing custom individuals
	std::srand(std::time(0));
	std::shared_ptr<test> A(new test);
	std::shared_ptr<test> B(new test);
	std::shared_ptr<test> C(new test(A,B));

	std::cout << "A:\n";
	A->print();
	std::cout << "B:\n";
	B->print();
	std::cout << "C:\n";
	C->print();
	//End testing of individuals

	dgal::population<test> pop;

	//Start testing of node manager
	//dgal::nodeManager<test, aaa> node;
	dgal::log("Finished");

	return 0;
}
