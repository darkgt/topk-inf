#ifndef _RTREE_H_
#define _RTREE_H_

#include <SpatialIndex.h>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;
using namespace SpatialIndex;



class RTreeIndex
{
private:
	string pointFile;
	string treeFile;
	IStorageManager* diskfile;
	StorageManager::IBuffer* file;
	ISpatialIndex* tree;
	unsigned int capacity;
	id_type indexIdentifier;
	//MyVisitor vis;
public:
	RTreeIndex(string pFile, string treeFile, unsigned int capacity);
	~RTreeIndex()
	{
		delete tree;
		delete file;		
		delete diskfile;		
	}
	void CreateRTree();
	void ReadIndex();
	
	ISpatialIndex* getTree()
	{return tree;}
};

#endif