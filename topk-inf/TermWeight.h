#ifndef _TERMWEIGHT_H_
#define _TERMWEIGHT_H_

#include <map>
#include <string>
#include <cmath>
#include "Tool.h"

class VectorSpaceModel{
protected:
	std::map<int, int> tfTable;
	std::string locationTermweightFile;
	std::string userTermweightFile;
	std::string frequencyFile;
	std::string docFile;

public:
	VectorSpaceModel(std::string &ltermweightFile, std::string &utermweightFile, std::string &userdoc){
		locationTermweightFile	= ltermweightFile;
		userTermweightFile		= utermweightFile;
		docFile					= userdoc;
		frequencyFile			= docFile + ".tf";

		if( !MyTool::FileExist(frequencyFile))
		{
			StatFrequency();
		}
		else
		{
			ReadTermFrequency();
		}
	}
	virtual ~VectorSpaceModel(){}

	void StatFrequency(){
		std::string line;
		int totaluser = 0;
		ifstream docF(docFile.c_str());
		ofstream vsmF(frequencyFile.c_str());
		ofstream utwF(userTermweightFile.c_str());
		while(getline(docF, line))
		{
			istringstream iss(line);
			int keyword, uid; char c;
			iss >> uid;
			std::map<int, int> tfcount;
			while(iss >> c >> keyword)
			{
				totaluser++;
				tfcount[keyword]++;
			}
			utwF << uid;
			double len = 0.0;
			for(std::map<int, int>::iterator it = tfcount.begin(); it != tfcount.end(); it++)
				len += (it->second) * (it->second);
			len = sqrt(len);
			for(std::map<int, int>::iterator it = tfcount.begin(); it != tfcount.end(); it++){
				utwF << "," << it->first << ":" << (1 + log(it->second * 1.0)) / len;
				tfTable[it->first]++;
			}
			utwF << endl;
			//user term weight output:
			//uid, term1, weight, term2, weight
		}
		docF.close();
		tfTable[-1] = totaluser;

		map<int, int>::iterator iter;
		for(iter = tfTable.begin(); iter != tfTable.end(); ++iter)
		{
			vsmF<<iter->first<<":"<<iter->second<<endl;
		}	
		vsmF.close();
		utwF.close();
	}

	void ReadTermFrequency(){
		ifstream vsmF(frequencyFile.c_str());
		std::string line;
		while(getline(vsmF, line))
		{
			istringstream iss(line);
			int id, frequency; char c;
			iss >> id >> c >> frequency;
			tfTable[id] = frequency;
		}
		vsmF.close();
	}

	void ComputeTermWeight(std::string &locdoc){
		ifstream docF(locdoc.c_str());
		ofstream ltwF(locationTermweightFile.c_str());
		std::string line;
		while( getline(docF, line) ){
			istringstream iss(line);
			int id, keyword; char c;
			iss >> id >> c;
			std::vector<std::pair<int, double> > twVec;
			double len = 0.0;
			while(iss >> keyword >> c){
				if(tfTable.count(keyword) == 0){
					continue;
					//std::cerr << "Location Keyword [" << keyword << "] not found in dataset." << endl;
				}
				double weight = log(1.0 + tfTable[-1]) / tfTable[keyword];
				len += weight * weight;
				twVec.push_back(make_pair( keyword, weight ));
			}
			//ltwF << id;
			len = sqrt(len);
			for(unsigned i = 0 ; i < twVec.size(); i++)
				ltwF << twVec[i].first << ":" << twVec[i].second / len << ",";
			ltwF << endl;
			//location term weight output
			//loc term1, weight, term2, weight, ...
		}
		docF.close();
		ltwF.close();
	}
};
#endif