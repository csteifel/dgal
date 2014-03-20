#ifndef DGALUTILITY_H
#define DGALUTILITY_H

#include <iostream>
#include <string>
#include <arpa/inet.h>

namespace dgal {


	enum {
		HEARTBEAT = 1, BESTMESSAGE
	};


#ifdef DEBUG
	void log(std::string message){
		std::cout << message << std::endl;
	}
#else
	void log(std::string message){
		(void) message;
	}
#endif


	bool bigEndian(){
		uint16_t endianTest = 1;
		if(((unsigned char *) &endianTest)[0] == 1){
			return false;
		}
		return true;
	}

	/**
	 * Custom network to host endian change nothing for 64 bit exists
	 * @param  uint64_t Input of 64 bit number
	 * @return          Correct host endian number
	 */
	uint64_t ntohll(uint64_t input){
		if(bigEndian()){
			//Network order is big endian
			return input;
		}

		uint32_t inLow = 0, inHigh = 0;

		inLow |= input;
		inHigh |= input >> 32;

		inLow = ntohl(inLow);
		inHigh = ntohl(inHigh);

		uint64_t result = ((uint64_t) inLow) << 32 | inHigh;


		return result;
	}

	/**
	 * Custom host to network endian change nothing for 64 bit exists
	 * @param  input Input of 64 bit number
	 * @return       Correct network endian number
	 */
	uint64_t htonll(uint64_t input){
		if(bigEndian()){
			return input;
		}

		uint32_t inLow = 0, inHigh = 0;

		inLow |= input;
		inHigh |= input >> 32;

		inLow = htonl(inLow);
		inHigh = htonl(inHigh);

		uint64_t result = ((uint64_t) inLow) << 32  | inHigh;

		return result;
	}


}
#endif