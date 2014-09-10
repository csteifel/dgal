#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <utility>
#include <algorithm>
#include <atomic>
#include <ctime>
#include <thread>
#include <memory>
#include <random>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "individual.h"
#include "dgalutility.h"

namespace dgal {
	template <typename indType> class population {
//		static_assert(std::is_function<indType::createFromSerialized>::value, "Types derived from dgal::individual must have a static createFromSerialized(std::string) method");

		typedef std::shared_ptr<indType> custIndPtr;

		public:
			population();
		protected:
			void nextGeneration();
			void generateNewIndividuals();
			void chooseParents();
			void run();
			void initiateMessaging();
			bool checkGoals() const; //potentially virtual later 'to allow for custom population controller'
			void sendBests();
			void getBests();
			void sendHeartBeat(int);

			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);

			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr> individualBuffer;
			std::atomic<bool> bufferDirty;

			clock_t initClock;
			size_t numIndividuals = 10;
			size_t generationNum = 0;
			size_t maxGeneration = -1;
			size_t maxFitnessLevel = -1;
			size_t maxTime = -1;

			bool stop = false;
	};

	template <typename indType, typename messagingType> population<indType, messagingType>::population(){
		initClock = clock();
		std::ifstream cfgFile("info.cfg");
		if (!cfgFile){ 
			dgal::log("ERROR: cannot find config file 'info.cfg'"); 
			return;
		}

		std::string token, condition, goalCond;
		double parameter;
		goalCond = "Goal_Condition:";
		while(cfgFile >> token){ //Ignore instructions to begin reading
			if (token == "***"){
				break;
			}
		}
		while (cfgFile >> token){
			if (token == goalCond){
				cfgFile >> condition >> parameter;
				if (condition == "numGen"){
					maxGeneration = parameter;
					dgal::log("Goal Condition set: Max number of generations = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "fitnessLevel"){
					maxFitnessLevel = parameter;
					dgal::log("Goal Condition set: Fitness level by most fit individual = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "time"){
					maxTime = parameter;
					dgal::log("Goal Condition set: Max amount of time can pass in seconds = <to_string>parameter");// + std::to_string(parameter))
				}
				else if (condition == "manual"){
					//TODO: implement
				}
			}
		}
		cfgFile.close();

	template <typename indType> population<indType>::population(){
		bufferDirty.store(false);
		if(generationNum == 0){
			generateNewIndividuals();
		}

		std::srand(std::time(0));
//		std::thread t(&population<indType, messagingType>::initiateMessaging, this);
		std::thread t(&population<indType>::getBests, this);
		run();
		stop = true;
//		t.join();
	}

	template <typename indType> void population<indType>::getBests(){
		struct addrinfo hints, *res;

		memset(&hints, 0, sizeof(struct addrinfo));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		//FIXME: remove hard coded server
		int rc = getaddrinfo("localhost", "25665", &hints, &res);

		if(rc == -1){
			std::cerr << "Error looking up server (" << strerror(errno) << "), proceeding without connecting to server...\n";
			return;
		}

		int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(fd == -1){
			std::cerr << "Error creating socket (" << strerror(errno) << "), proceeding without connecting to server...\n";
			return;
		}

		rc = connect(fd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);
		if(rc == -1){
			std::cerr << "Could not connect socket (" << strerror(errno) << "), proceding without connecting to server...\n";
			return;
		}

		std::thread heartBeatThread(&population<indType>::sendHeartBeat, this, fd);
		//create buffer to read in type and size information
		char buf[sizeof(uint8_t) + sizeof(uint64_t)];

		//Set a timeout for recving so that if stop is set it can stop
		struct timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, (socklen_t) sizeof(struct timeval));

		while(!stop){
			memset(buf, 0, sizeof(uint8_t) + sizeof(uint64_t));
			int amountRecv = recv(fd, buf, sizeof(uint8_t) + sizeof(uint64_t), 0);

			if(amountRecv > 0){
				if(buf[0] == dgal::BESTMESSAGE){
					uint64_t messageLength = 0;
					memcpy(&messageLength, &buf[1], sizeof(uint64_t));
					messageLength = dgal::ntohll(messageLength);

					std::string bestsMessage(messageLength, 0);
					amountRecv = recv(fd, &bestsMessage[0], messageLength, 0);
					
				}
			}else if(amountRecv == -1){
				if(!(errno == EAGAIN || errno == EWOULDBLOCK)){
					std::cerr << "Error reading from socket: " << errno << " " << strerror(errno) << "\n";
				}
			}else{
				std::cerr << "Error server connection lost!" << std::endl;
				stop = true;
				break;
			}
		}
		heartBeatThread.join();
		shutdown(fd, SHUT_RDWR);
		close(fd);
	}


	template <typename indType> void population<indType>::sendHeartBeat(int fd){
		while(!stop){
			char buf[sizeof(uint8_t) + sizeof(uint64_t)];
			memset(buf, 0, sizeof(uint8_t) + sizeof(uint64_t));
			buf[0] = dgal::HEARTBEAT;
			send(fd, buf, sizeof(uint8_t) + sizeof(uint64_t),0);
			sleep(15);
		}
	}

	//template <typename indType, typename messagingType> void population<indType, messagingType>::addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders){

	template <typename indType> void population<indType>::addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders){
		for(size_t i = 0; i < outsiders.size(); i++){
			custIndPtr recieved(new indType(outsiders[i].first, outsiders[i].second));
			individualBuffer.push_back(std::move(recieved));
		}
		bufferDirty.store(true);
	}

	template <typename indType> void population<indType>::nextGeneration(){
		//Pick out bests if not just starting up
		chooseParents();

		//Fill in the rest of the population with new individuals
		generateNewIndividuals();


		dgal::log("Sorting");
		std::sort(individuals.begin(), individuals.end(),
			[](const std::shared_ptr<dgal::individual> a, const std::shared_ptr<dgal::individual> b){
				if(a->getFitness() <= b->getFitness()){ return false; } return true;
			});


		for(size_t i = 0; i < individuals.size(); i++){
			dgal::log("Fitness <to_string>individual[i]->getFitness()");// + std::to_string(individuals[i]->getFitness()));
		}
	
	}

	template <typename indType> void population<indType>::generateNewIndividuals(){
		while(individuals.size() < numIndividuals){
			dgal::log("Adding individual");
			individuals.push_back(custIndPtr(new indType));
		}
	}

	template <typename indType> void population<indType>::chooseParents(){
		//TODO: implement
	}

	template <typename indType> void population<indType>::run(){
		dgal::log("Running generation " + std::to_string(generationNum));

		//Run this generation
		//TODO: put in thread pool individual running
		for(size_t i = 0; i < individuals.size(); i++){
			individuals[i]->run();
		}

		++generationNum;

		if(checkGoals() == false){
			nextGeneration();
			run();
		}else{
			std::cout << "done" << std::endl;
		}
	}

	template <typename indType, typename messagingType> bool population<indType, messagingType>::checkGoals() const{
		if (maxGeneration != -1 && generationNum >= maxGeneration){
			dgal::log("Goal Satisfied: Max Generation met");
			return true;
		}
		else if (maxFitnessLevel != -1)
		{
			if(individuals[0]->getFitness() >= maxFitnessLevel)
			{
				dgal::log("Goal Satisfied: Fitness Level achieved (local node)");
				return true;
			}
			//Assuming Individual Buffer is not sorted. Can be changed later if sorted
			for (std::vector<custIndPtr>::iterator i = individualBuffer.begin(); i != individualBuffer.end(); ++i)
			{
				if(*i->getFitness() >= maxFitnessLevel)
				{
					dgal::log("Goal Satisfied: Fitness Level achieved (outside node)");
					return true;
				}
			}
		}
		else if (maxTime != -1 && (clock() - initClock)/CLOCKS_PER_SEC >= maxTime){
			dgal::log("Goal Satisfied: Time has run out");
			return true;
		}
		return false;
	}
}

#endif