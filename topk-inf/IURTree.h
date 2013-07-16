#ifndef _IURTREE_H_
#define _IURTREE_H_

#include <direct.h>
#include "RTreeIndex.h"
#include "btree.h"
#include "WriteNodeStrategy.h"
#include "Tool.h"
#include "TermWeight.h"

//#include "config.h"

/*
IURTree is built for location index
*/

//Defined in config.h
extern string docFile;
extern string locFile;

extern string leafnodeFile;
extern string indexnodeFile;

extern string ltermweightFile;
extern string utermweightFile;
extern string treeFile;
extern string userdoc;

extern string vocabFolder;
extern string btreeFolder;
extern string subdocFolder;

extern int numOfEntry;
extern int Blocksize;

#define UNION			-1
#define INTERSECTION	-2

class IURTree
{
protected:
	int nodeNum;//Rtree node num
	map<int, vector<int> *> leaves;
	vector<pair<int, vector<int> *>> indexNodes;

public:

	virtual ~IURTree();

	void ReadLeafNodes();
	void ReadIndexNodes();
	void ComputeVocabularyLeaf();
	void ComputeVocabularyIndex();
	void BuildIRTree();
};

#endif