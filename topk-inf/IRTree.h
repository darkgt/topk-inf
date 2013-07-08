#ifndef _IRTREE_H_
#define _IRTREE_H_

#include <direct.h>
#include "RTreeIndex.h"
//#include "TermWeight.h"
//#include "WriteNodeStrategy.h"
#include "btree.h"

extern string docFile;	//text description
extern string locFile;	//location
extern string leafnodeFile;	//Ҷ���
extern string indexnodeFile;	//��Ҷ���
extern string termweightFile;	//���language modelԤ���ֵ
extern string treeFile; //rtree index

extern string vocabFolder;	//ÿ������term weight����
extern string btreeFolder;	//B+ tree����
extern string subdocFolder;

extern int numOfEntry;	//hotel 40, GN 100
extern int Blocksize;	//4K page size

class IRTree
{
	map<int, vector<int> *> leaves;	
	int *N2SG;
	int nodeNum;
	vector<pair<int, vector<int> *>> indexNodes;

public:

	~IRTree()
	{
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

	int ReadLeafNodes();
	void ReadIndexNodes();
	void BuildIRTree();	
	void SplitDoc();
	void ComputeVocabularyLeaf();
	void ComputeVocabularyIndex();
	void BuildBTIndex();
};

#endif