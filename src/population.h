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
			//virtual to allow for custom population controller
			void nextGeneration();
			void generateNewIndividuals();
			void chooseParents();
			void run();
			void initiateMessaging();

			void sendBests();
			void getBests();
			void sendHeartBeat(int);

			void addOutsideBests(const std::vector<std::pair<std::string, double> >& outsiders);

			std::vector<custIndPtr> individuals;
			//Individual buffer for holding bests from other nodes until used.
			std::vector<custIndPtr> individualBuffer;
			std::atomic<bool> bufferDirty;

			size_t numIndividuals = 10;
			size_t generationNum = 0;
			size_t maxGeneration = 10;

			bool stop = false;

	};

	template <typename indType> population<indType>::population(){
		bufferDirty.store(false);
		if(generationNum == 0){
			generateNewIndividuals();
		}

		std::srand(std::time(0));

		std::thread t(&population<indType>::getBests, this);
		run();
		stop = true;
		t.join();
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
			dgal::log("Fitness " + std::to_string(individuals[i]->getFitness()));
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

		if(generationNum < maxGeneration){
			nextGeneration();
			run();
		}else{
			std::cout << "done" << std::endl;
		}
	}
}

#endif