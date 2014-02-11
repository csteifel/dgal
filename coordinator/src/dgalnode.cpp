#include "dgalnode.h"

void dgalNode::sentHeartBeat(){
	//Check to see/make sure we have recieved a heart beat if we have sent one otherwise we should just mark as dead
	if(hbcStart.time_since_epoch().count() != 0){
		//Not the first update of clock we need to check
		if(hbcStart > hbcEnd){
			//TODO: find some safe way to delete self or mark for death
			return;
		}
	}
	hbcStart = clock::now();
}

void dgalNode::recievedHeartBeat(){
	hbcEnd = clock::now();
}