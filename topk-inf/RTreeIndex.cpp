#include "RTreeIndex.h"


RTreeIndex::RTreeIndex(string pFile, string tFile, unsigned int capacity)
{
	pointFile = pFile;
	this->capacity = capacity;
	treeFile = tFile;
}


void RTreeIndex::CreateRTree()
{
	diskfile = StorageManager::createNewDiskStorageManager(treeFile, 4096);
	file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 1000, false);
	tree = RTree::createNewRTree(*file, 0.7, capacity, capacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

	ifstream pFile(pointFile.c_str());
	if (pFile.is_open())
	{		
		int count = 0;
		while (! pFile.eof() )
		{
			string line;
			getline(pFile, line);
			if(line == "")
				continue;
			int id;char c;
			double coor[2];
			istringstream iss(line);
			iss>> id >>c>>coor[0]>>c>>coor[1];
			Point p = Point(coor, 2);			
			tree->insertData(0, 0, p, count);
			count++;
		}
		pFile.close();		
	}
}

void RTreeIndex::ReadIndex()
{
	diskfile = StorageManager::loadDiskStorageManager(treeFile);
	file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 1000, false);
	tree = RTree::loadRTree(*file, 1);
}
