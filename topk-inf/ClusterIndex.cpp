#include "ClusterIndex.h"

ClusterIndex::ClusterIndex(int clusternum){
	this -> clusternum = nodenum;
	userGroupId = new int[clusternum];
	cluInfo = new ClusterInfo[clusternum];
}
ClusterIndex::~ClusterIndex(){
	delete[] userGroupId;
	delete[] cluInfo;
}

void ClusterIndex::ReadUserCluster(){
	string line;
	ifstream cluF(clusterFile.c_str());
	int uid = 0;
	int clusterid;
	while( getline(cluF, line) ){
		istringstream iss(line);
		iss >> clusterid;
		userGroupId[id] = clusterid;
		cluInfo[clusterid].userId.push_back(id);
		++uid;
	}
	cluF.close();
}

void ClusterIndex::ReadUserInfo(){
	string line;
	int uid = 0;
	double x, y;
	char c;
	//read location information
	ifstream locF(userloc.c_str());
	while( getline(locF, line) ){
		istringstream iss(line);
		iss >> x >> c >> y;
		ClusterInfo * pClu = &cluInfo[ userGroupId[uid++] ];
		pClu->updatePosition(x, y);
	}
	locF.close();
	int keyword;
	double weight;
	//read document information
	ifstream docF(utermweightFile.c_str());
	while(getline(docF, line)){
		istringstream iss(line);
		iss >> uid;
		ClusterInfo * pClu = &cluInfo[ userGroupId[uid] ];
		while(iss >> c >> keyword >> c >> weight)
			pClu->termWeight[keyword] += weight;
	}
	docF.close();
}