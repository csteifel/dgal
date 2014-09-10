#include "population.h"
#include "messenger.h"
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <utility>
#include <unistd.h>



class test : public dgal::individual {
	public:
		test() : dgal::individual(10) {}
		test(const std::shared_ptr<test> a, const std::shared_ptr<test> b) : dgal::individual(a, b) {}
		void print(){ for(size_t i = 0; i < weights.size(); i++){ std::cout << weights[i] << " "; } std::cout << std::endl; }

		void run(){ fitness = rand(); usleep(250000); }

		static std::shared_ptr<dgal::individual> createFromSerialized(std::string){return std::shared_ptr<dgal::individual>(NULL);}

};



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
