#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <utility>
#include <algorithm>
#include <atomic>
#include <ctime>
#include <thread>
#include <mutex>
#include <memory>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <unordered_set>
#include "individual.h"
#include "dgalutility.h"
#include <fstream>

//boost::property_tree for json parsing and creation
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace dgal {
	template <typename indType> class population {
//		static_assert(std::is_function<indType::createFromSerialized>::value, "Types derived from dgal::individual must have a static createFromSerialized(std::string) method");

		

		public:
			population();
		protected:
			void nextGeneration();
			void generateNewIndividuals();
			void chooseParents();
			void runGeneration();
			void initiateMessaging();
			bool checkGoals() const; //potentially virtual later 'to allow for custom population controller'
			void sendBests();
			void getBests();
			void sendHeartBeat();

			void addOutsideBests(std::vector<std::string>&&, std::vector<double>&&, std::vector<std::string>&&);
			void parseBests(const std::string&& recievedMessage, std::vector<std::string>&, std::vector<double>&, std::vector<std::string>&);

			std::vector<std::shared_ptr<dgal::individual> > individuals;
			std::unordered_set<std::string> individualUUIDs; //Quick access to whether or not an individual already exists in this pop.
			//Individual buffer for holding bests from other nodes until used.
			std::vector<std::shared_ptr<dgal::individual> > individualBuffer;
			std::atomic<bool> bufferDirty;

			clock_t initClock;
			size_t numIndividuals = 10;
			size_t generationNum = 0;
			size_t maxGeneration = -1;
			size_t maxFitnessLevel = -1;
			size_t maxTime = -1;
			size_t numKeepers = 8;

			int fd = -1;

			bool stop = false;

			std::mutex indBufferLock;

			typedef std::shared_ptr<indType> custIndPtr;
	};
	
	template <typename indType> void population<indType>::chooseParents(){
		//TODO: implement
		/*
			P[i] = getFitness() / 
		*/

		int sum;
		for(int i=0; i < individuals.size(); ++i){
			sum=0;
			for(int j=0; j<individuals.size(); ++j){
				sum+=individuals[j]->getFitness();
			}
		}
	}

	template <typename indType> population<indType>::population(){
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
		bufferDirty.store(false);
		if(generationNum == 1){
			generateNewIndividuals();
		}

		std::srand(std::time(0));
//		std::thread t(&population<indType, messagingType>::initiateMessaging, this);
		std::thread t(&population<indType>::getBests, this);
		runGeneration();
		stop = true;
//		t.join();
	}
	
	template <typename indType> void population<indType>::sendBests(){
		if(fd == -1){
			return;
		}
		dgal::log("Sending bests");


		std::string message(sizeof(uint8_t) + sizeof(uint64_t), 0);
		message[0] = dgal::BESTMESSAGE;

		//Strange way of creating a JSON array but its the easiest way to keep header only and dependencies to only boost
		boost::property_tree::ptree propTree, childArr;

		for(size_t i = 0; i < numKeepers; i++){
			boost::property_tree::ptree childInd;
			childInd.put("id", individuals[i]->uniqueID);
			childInd.put("fitness", individuals[i]->getFitness());
			childInd.put("serialization", individuals[i]->serialize());

			childArr.push_back(std::make_pair("", childInd)); //To create an array you append with a key of ""
		}
		propTree.add_child("individuals", childArr);

		std::string serializedObjectsStr;
		{
			//Will only write to a file or a stream not a regular string
			std::stringstream ss;
			write_json(ss, propTree, false); 


			std::fstream aaa("output.txt", std::fstream::out);	
			aaa.write(ss.str().c_str(), ss.str().length());
			aaa.close();

			serializedObjectsStr = ss.str();
		}

		uint64_t messageSize = serializedObjectsStr.size();
		messageSize = dgal::htonll(messageSize);
		memcpy(&message[1], &messageSize, sizeof(uint64_t));
		message += serializedObjectsStr;


		int rc = send(fd, message.data(), message.size(), 0);
		if(rc != message.size()){
			std::cerr << "Error sending bests to server " << errno << " " << strerror(errno) << "\n";
		}
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

		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(fd == -1){
			std::cerr << "Error creating socket (" << strerror(errno) << "), proceeding without connecting to server...\n";
			return;
		}

		rc = connect(fd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);
		if(rc == -1){
			std::cerr << "Could not connect socket (" << strerror(errno) << "), proceding without connecting to server...\n";
			shutdown(fd, SHUT_RDWR);
			close(fd);
			fd = -1;
			return;
		}

		std::thread heartBeatThread(&population<indType>::sendHeartBeat, this);
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

					//Create vector for storing the bests we just recieved and use in addOutsideBests
					std::vector<std::string> receivedUUIDs, receivedSerializations;
					std::vector<double> receivedFitnesses;

					parseBests(std::move(bestsMessage), receivedUUIDs, receivedFitnesses, receivedSerializations);
					addOutsideBests(std::move(receivedUUIDs), std::move(receivedFitnesses), std::move(receivedSerializations));
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

	template <typename indType> void population<indType>::parseBests(const std::string&& receivedMessage, std::vector<std::string>& receivedUUIDs, std::vector<double>& receivedFitnesses, std::vector<std::string>& receivedSerializations){
		//TODO: Implement using json and base64 encoding to allow for multiple bests to be sent!
		boost::property_tree::ptree sentJson;

		{
			std::stringstream ss(std::move(receivedMessage));
			read_json(ss, sentJson);
		}

		for(boost::property_tree::ptree::const_iterator itr = sentJson.get_child("individuals").begin(), end = sentJson.get_child("individuals").end(); itr != end; itr++){
			std::string indUUID = (*itr).second.get<std::string>("id");

			if(individualUUIDs.count(indUUID)){
				//This individual is already in our system.
				continue;
			}

			receivedUUIDs.push_back(indUUID);
			receivedFitnesses.push_back((*itr).second.get<double>("fitness"));
			receivedSerializations.push_back((*itr).second.get<std::string>("serialization"));
			//TODO: Need to refactor this so that it returns back not a pair with serial and fitness but 3 things, uuid, fitness, and serialization of weights.
			// receivedBests.push_back(std::make_pair((*itr).second.get<std::string>("serialization"), (*itr).second.get<double>("fitness")));
		}

	}

	template <typename indType> void population<indType>::sendHeartBeat(){
		while(!stop){
			char buf[sizeof(uint8_t) + sizeof(uint64_t)];
			memset(buf, 0, sizeof(uint8_t) + sizeof(uint64_t));
			buf[0] = dgal::HEARTBEAT;
			send(fd, buf, sizeof(uint8_t) + sizeof(uint64_t),0);
			sleep(15);
		}
	}

	template <typename indType> void population<indType>::addOutsideBests(std::vector<std::string>&& outsiderUUIDs, std::vector<double>&& outsiderFitnesses, std::vector<std::string>&& outsiderSerializations){
		assert(outsiderUUIDs.size() == outsiderFitnesses.size());
		assert(outsiderUUIDs.size() == outsiderSerializations.size());
		for(size_t i = 0; i < outsiderUUIDs.size(); i++){
			std::shared_ptr<dgal::individual>  received = indType::createFromSerialized(outsiderUUIDs[i], outsiderFitnesses[i], std::move(outsiderSerializations[i]));

			//std::shared_ptr<dgal::individual>  received(new indType(outsiders[i].first, outsiders[i].second));
			indBufferLock.lock();
			individualBuffer.push_back(received);
			indBufferLock.unlock();
		}

		if(outsiderUUIDs.size() > 0){
			bufferDirty.store(true);
		}
	}

	template <typename indType> void population<indType>::nextGeneration(){

		//Pull in the bests from other nodes
		if(bufferDirty.load() == true){
			dgal::log("Adding outside individuals");
			//There are items to add
			indBufferLock.lock();
			for(size_t i = 0; i < individualBuffer.size(); i++){
				std::pair<std::unordered_set<std::string>::iterator, bool> insertRes = individualUUIDs.insert(individualBuffer[i]->uniqueID);

				//Only want to insert into the buffer if we don't already have this individual
				if(insertRes.second == true){	
					individuals.push_back(individualBuffer[i]);
				}
			}
			individualBuffer.clear();
			indBufferLock.unlock();
			bufferDirty.store(false);
		}

		dgal::log("Sorting");
		std::sort(individuals.begin(), individuals.end(),
			[](const std::shared_ptr<dgal::individual> a, const std::shared_ptr<dgal::individual> b){
				if(a->getFitness() <= b->getFitness()){ return false; } return true;
			});


		dgal::log("Erasing bad individuals");
		//Choose to only keep the chosen retain amount
		for(size_t i = numKeepers; i < individuals.size(); ++i){
			individualUUIDs.erase(individuals[i]->uniqueID);
		}
		individuals.erase(individuals.begin() + numKeepers, individuals.end());

		//Pick out bests if not just starting up
		chooseParents();

		//Fill in the rest of the population with new individuals
		generateNewIndividuals();
	}

	template <typename indType> void population<indType>::generateNewIndividuals(){
		while(individuals.size() < numIndividuals){
			individuals.push_back(std::shared_ptr<dgal::individual> (new indType));
			individualUUIDs.insert(individuals[individuals.size()-1]->uniqueID); //Should never fail otherwise our UUIDs are not so unique
		}
	}



	template <typename indType> void population<indType>::runGeneration(){
		dgal::log("Running generation " + std::to_string(generationNum));

		//Run this generation
		//TODO: put in thread pool individual running
		for(size_t i = 0; i < individuals.size(); i++){
			individuals[i]->run();
		}


		dgal::log("Sorting...");
		std::sort(individuals.begin(), individuals.end(),
			[](const std::shared_ptr<dgal::individual> a, const std::shared_ptr<dgal::individual> b){
				if(a->getFitness() <= b->getFitness()){ return false; } return true;
			});
	
		dgal::log("Printing fitnesses\n-------------------");
		for(size_t i = 0; i < individuals.size(); i++){
			dgal::log(std::to_string(i+1) + ": " + std::to_string(individuals[i]->getFitness()));
		}
		dgal::log("-------------------");

		sendBests();

		++generationNum;

		if(checkGoals() == false){
			nextGeneration();
			runGeneration();
		}
	}

	template <typename indType> bool population<indType>::checkGoals() const{
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
			for (std::vector<std::shared_ptr<individual> >::const_iterator i = individualBuffer.begin(); i != individualBuffer.end(); ++i)
			{
				if((*i)->getFitness() >= maxFitnessLevel)
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
