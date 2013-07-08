#include "config.h"
#include "Propagation.h"
#include "ContriList.h"
#include <iostream>
#include <sstream>
#include <fstream>

/*
string basepath = "C:\\Users\\darkgt\\Desktop\\topk-inf\\GN dataset\\";

string locFile = basepath + "loc";		//location
string treeFile = basepath + "rtree";	//rtree index
int numOfEntry = 100;
int Blocksize = 4 * 1024;		//4K page size
*/

using namespace std;

double teleport = 0.5;
double tolerance = 0.001;

void precompute(){
	cerr<<"Working Folder:"<< baseDir <<endl;
	BookmarkColor bca;
	bca.setUserNum(nodenum);
	ifstream ifs(edgeFile);
	while(!ifs.eof()){
		string line;
		getline(ifs, line);
		if(line == "") continue;
		int u, v;
		istringstream iss(line);
		iss>>u>>v;
		bca.addEdge(u, v);
		bca.addEdge(v, u);
	}
	ifs.close();

	bca.setMaxLevel(hublevel);
	ifs.open(hubNodesFile);
	while(!ifs.eof()){
		string line;
		getline(ifs, line);
		if(line == "") continue;
		int id, level, cluster;
		istringstream iss(line);
		iss>>id>>level>>cluster;
		bca.setHubnode(id, level);
	}
	ifs.close();

	bca.setTeleport(teleport);
	bca.setTolerance(tolerance);
	double tot = 0;
	double num = 0;
	//for(int i = 0; i < nodenum; i+= 1000){
		//map<unsigned, double>* mp = bca.singleNodePropagation(i);
		//tot += mp->size();
		//num ++;
		//cout<<mp->size()<<endl;
	//}
	//cout<<tot / num <<endl;
	bca.precomputation();
	//cout<<bca. getHubnodeNum()<<endl;

	/*
	//test correctness
	unsigned id = 0;
	map<unsigned, double> mp1 = *bca.singleNodePropagation(id);
	map<unsigned, double>initInf;
	initInf[id] = 1.0;
	map<unsigned, double> mp2 = *bca.hubPropagation(&initInf);
	map<unsigned, double>::iterator it;
	for(it = mp1.begin();it != mp1.end(); it++)
		cout<<it->first<<" "<<it->second<<endl;
	cout<<" "<<endl;
	for(it = mp2.begin();it != mp2.end(); it++)
		cout<<it->first<<" "<<it->second<<endl;
	*/
}

void loadBCA(){
	BookmarkColor bca;
	bca.setUserNum(nodenum);
	bca.loadVectors();
}

int main(){
	/*
	RTreeIndex rt(locFile, treeFile, numOfEntry, Blocksize);
	//rt.CreateRTree();
	rt.ReadIndex();

	MyQueryStrategy mqs;

	ISpatialIndex* tree = rt.getTree();
	//id_type next = tree->SpatialIndex::RTree::m_rootID;

	id_type next = -1;
	while( tree->queryStrategy2(mqs, next) );
	*/
	//precompute();
	//loadBCA();
	getchar();
	return 0;
}