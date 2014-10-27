#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main(){

	boost::property_tree::ptree pt;



	std::fstream testfile("jsonreadtest.json", std::fstream::in);

	read_json(testfile, pt);

	testfile.close();

	for(boost::property_tree::ptree::const_iterator itr = pt.get_child("individuals").begin(), end = pt.get_child("individuals").end(); itr != end; itr++){
		std::cout << (*itr).second.get<std::string>("id") << std::endl; //TODO: finish
		std::cout << (*itr).second.get<std::string>("serialization") << std::endl; //TODO: finish
	}


	return 0;
}