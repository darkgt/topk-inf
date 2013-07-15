#ifndef _TERMWEIGHT_H_
#define _TERMWEIGHT_H_

#include <map>
#include <string>

class VectorSpaceModel{
protected:
	std::map<int, int> tfTable;
	std::string termweightFile;
	std::string frequencyFile;
	std::string docFile;
public:
	VectorSpaceModel(std::string &ltermweightFile, std::string &utermweightFile, std::string &){}
	virtual ~VectorSpaceModel(){}
};
#endif