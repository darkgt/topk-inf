#ifndef _IURTREE_H_
#define _IURTREE_H_

#include <direct.h>
#include "RTreeIndex.h"
#include "btree.h"
#include "WriteNodeStrategy.h"
#include "Tool.h"
//#include "config.h"

/*
IURTree is built for location index
*/

//Defined in config.h
extern string docFile;
extern string locFile;

extern string leafnodeFile;
extern string indexnodeFile;

extern string termweightFile;
extern string treeFile;

extern string vocabFolder;
extern string btreeFolder;
extern string subdocFolder;

extern int numOfEntry;
extern int Blocksize;

#define UNION        -1
#define INTERSECTION -2

class IURTree
{
	int nodeNum;//Rtree node num
	map<int, vector<int> *> leaves;
	vector<pair<int, vector<int> *>> indexNodes;

public:

	virtual ~IURTree(){
		map<int, vector<int> *>::iterator iter = leaves.begin();
		for(;iter != leaves.end(); ++iter)
		{
			delete iter->second;
		}
		vector<pair<int, vector<int> *>>::iterator iter2 = indexNodes.begin();
		for(;iter2 != indexNodes.end(); ++iter2)
		{
			vector<int> * p = iter2->second;
			delete p;
		}	
	}

	void ReadLeafNodes();
	void ReadIndexNodes();
	void ComputeVocabularyLeaf();
	void ComputeVocabularyIndex();
	void BuildIRTree();
};

#endif