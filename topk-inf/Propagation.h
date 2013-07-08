#ifndef PROPAGATION
#define PROPAGATION

#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <io.h>
#include "config.h"
//using std::vector;

struct EdgeInfo{
	unsigned neighbor;
	double weight;
};

class UserGraph{
protected:
	unsigned userNum;
	std::vector< std::vector<EdgeInfo> > edge;
	void addEdge (int u, EdgeInfo info){
		edge[u].push_back(info);
	}

public:
	UserGraph(){}
	~UserGraph(){}

	std::vector<EdgeInfo>* getNeighbors (int userID){
		return &(edge[userID]);
	}

	//directed edge
	
	void addEdge (int u, int v){
		struct EdgeInfo info;
		info.neighbor = v;
		edge[u].push_back(info);
	}

	virtual void setUserNum (unsigned userNum){
		this->userNum = userNum;
		edge = std::vector< std::vector<EdgeInfo> > (userNum);
	}
};

class HierarchicalUserGraph : public UserGraph{
protected:
	std::vector<int> hubnodeLevel; // -1 means not hubnode
	std::vector<std::vector<unsigned> > hubnodeByLevel;

public:
	HierarchicalUserGraph(){}
	~HierarchicalUserGraph(){}

	void setUserNum (unsigned userNum){
		this->userNum = userNum;
		edge = std::vector< std::vector<EdgeInfo> > (userNum);
		hubnodeLevel = std::vector<int> (userNum, -1);
	}

	void setMaxLevel(unsigned level){
		hubnodeByLevel = std::vector<std::vector<unsigned> > (level);
	}

	int getHubnodeNum(){
		int ret = 0;
		for(unsigned  i = 0;i < hubnodeByLevel.size(); i++)
			ret += hubnodeByLevel[i].size();
		return ret;
	}

	void setHubnode(unsigned nodeId, int level = -1){
		if(hubnodeLevel[nodeId] != -1)
			return;
		hubnodeLevel[nodeId] = level;
		if(level != -1){
			hubnodeByLevel[level].push_back(nodeId);
		}
	}

	inline int getHubnodeLevel(unsigned nodeId){
		return hubnodeLevel[nodeId];
	}
};

class BookmarkColor : public HierarchicalUserGraph{
protected:
	std::vector<double> vecInfScore;
	std::map<unsigned, double> mapInfScore;
	std::map<unsigned, int> hubLevel;
	std::map<unsigned, std::map<unsigned, double> > bcaVector;
	double teleportation; //Should be normalized to [0,1]
	double tolerance;     //a very small float number

public:
	void setTeleport(double teleportation){
		this->teleportation = teleportation;
	}

	void setTolerance(double tolerance){
		this->tolerance = tolerance;
	}
	/*
	std::vector<double>* singleNodePropagation(unsigned bookmarkNodeId, double inf = 1.0){
		vecInfScore = std::vector<double> (userNum, 0.0);
		std::map<unsigned, pair<unsigned, double>* >inQueue;//mark whether a node is in the queue
		std::queue<pair<unsigned, double> >myQueue;

		myQueue.push(make_pair(bookmarkNodeId, inf));
		inQueue[bookmarkNodeId] = &myQueue.back();
		while(!myQueue.empty()){
			unsigned curNode = myQueue.front().first;
			double  transInf = myQueue.front().second;
			myQueue.pop();
			vecInfScore[curNode] += transInf * teleportation;
			inQueue.erase(curNode);
			if( transInf < tolerance)
				continue;
			std::vector<EdgeInfo>* pEdges = getNeighbors(curNode);
			unsigned neighborsNum = pEdges->size(); //degree of the node
			std::vector<EdgeInfo>::iterator it = pEdges->begin();
			for(; it != pEdges -> end(); it++){
				unsigned nextNode = (*it).neighbor;
				double propInf = transInf * (1 - teleportation) / neighborsNum;
				if(inQueue.count(nextNode) > 0){
					//in the queue
					inQueue[nextNode]->second += propInf;
				}
				else{
					myQueue.push(make_pair(nextNode, propInf));
					inQueue[bookmarkNodeId] = &myQueue.back();
				}
			}
		}
		return &vecInfScore;
	}
	*/
	std::map<unsigned, double>* singleNodePropagation(unsigned bookmarkNodeId, double inf = 1.0, std::map<unsigned, int>* hubLevel = NULL){
		mapInfScore.clear();
		std::map<unsigned, double>unpropagatedInf;
		std::map<unsigned, double>::reverse_iterator iter;

		unpropagatedInf[bookmarkNodeId] = inf;
		while(unpropagatedInf.size()){
			iter = unpropagatedInf.rbegin();//rbegin
			unsigned curNode = iter->first;
			double  transInf = iter->second;
			unpropagatedInf.erase(curNode);
			//std::cout<<curNode<<" "<<transInf<<std::endl;

			if(transInf < tolerance)
				continue;

			//store to hubnodes
			if(hubLevel != NULL && hubLevel -> count(curNode) > 0){
				mapInfScore[curNode] += transInf;
				continue;
			}

			//leave teleportation percent
			mapInfScore[curNode] += transInf * teleportation;

			std::vector<EdgeInfo>* pEdges = getNeighbors(curNode);
			unsigned neighborsNum = pEdges->size(); //degree of the node
			std::vector<EdgeInfo>::iterator it = pEdges->begin();
			for(; it != pEdges -> end(); it++){
				//separate to neighbors
				unsigned nextNode = (*it).neighbor;
				double propInf = transInf * (1 - teleportation) / neighborsNum;
				unpropagatedInf[nextNode] += propInf;
			}
		}
		return &mapInfScore;
	}

	void saveVector(unsigned id){
		char fileName[MAX_FILENAME_LEN];
		sprintf_s(fileName, MAX_FILENAME_LEN, "%s%u", bcaVectorDir.c_str(), id); //should "try" here...

		FILE *pFile = fopen(fileName, "w");
		if(NULL == pFile)
			std::cerr<<"Failed to open"<<fileName<<"\n";
		std::map<unsigned, double>::iterator iter = mapInfScore.begin();
		for(;iter != mapInfScore.end(); iter++)
			fprintf(pFile, "%u\t%lf\n", iter->first, iter->second);
		fclose(pFile);
	}

	void loadVectors(){
		_finddata_t file;
		long lf;
		std::string bcaVectorFolder = bcaVectorDir + "*";
		if((lf = _findfirst(bcaVectorFolder.c_str(), &file)) == -1)
			std::cerr << "Folder Not Found!" << std::endl;
		else{
			//std::cerr << "File name list:" << std::endl;
			while(_findnext(lf, &file) == 0){
				//ignore the current folder and the parent folder
				if(!strcmp(file.name, ".") || !strcmp(file.name, ".."))
					continue;
				//std::cerr<< file.name << std::endl;
				char fileName[MAX_FILENAME_LEN];
				sprintf_s(fileName, MAX_FILENAME_LEN, "%s%s", bcaVectorDir.c_str(), file.name); //should "try" here...
				FILE *pFile = fopen(fileName, "r");
				unsigned hubId = atoi(file.name);
				unsigned id;
				double 	inf;
				while(fscanf(pFile, "%u %lf", &id, &inf) != EOF){
					bcaVector[hubId][id] = inf;
				}
				fclose(pFile);
			}
		}
		_findclose(lf);
	}

	void precomputation(bool saveInMemory = false, bool saveOnDisk = true){
		hubLevel.clear();
		for(unsigned level = 0; level < hubnodeByLevel.size(); level++){
			for(unsigned j = 0; j < hubnodeByLevel[level].size(); j++){
				unsigned id = hubnodeByLevel[level][j];
				singleNodePropagation(id, 1.0, &hubLevel);//record in disk
				
				if(saveInMemory)
					bcaVector[ id ] = mapInfScore;

				if(saveOnDisk)
					saveVector(id);
			}
			for(unsigned j = 0; j < hubnodeByLevel[level].size(); j++)
				hubLevel[ hubnodeByLevel[level][j] ] = level;
		}
	}

	std::map<unsigned, double>* hubPropagation(std::map<unsigned, double>* initInf){ //initial score
		mapInfScore.clear();//result
		std::map<unsigned, double> unpropagatedInf;//always from lower level node to higher level nodes
		std::map<unsigned, double>::reverse_iterator iter;

		while(initInf -> size()){
			iter = initInf -> rbegin();//rbegin
			unsigned curNode = iter->first;
			double  transInf = iter->second;
			initInf -> erase(curNode);

			if(transInf < tolerance)
				continue;
			
			if(hubnodeLevel[curNode] != -1){
				unpropagatedInf[curNode] += transInf;
				continue;
			}

			mapInfScore[curNode] += transInf * teleportation;

			std::vector<EdgeInfo>* pEdges = getNeighbors(curNode);
			unsigned neighborsNum = pEdges->size(); //degree of the node
			std::vector<EdgeInfo>::iterator it = pEdges->begin();
			for(; it != pEdges -> end(); it++){
				//separate to neighbors
				unsigned nextNode = (*it).neighbor;
				double propInf = transInf * (1 - teleportation) / neighborsNum;
				initInf -> at(nextNode) += propInf;
			}
		}

		for(int level = hubnodeByLevel.size() - 1; level >= 0; level--){
			for(iter = unpropagatedInf.rbegin(); iter != unpropagatedInf.rend(); iter++){
				unsigned curNode = iter->first;
				double  transInf = iter->second;
				if(hubnodeLevel[curNode] != level)
					continue;
				std::map<unsigned, double>::iterator it = bcaVector[curNode].begin();
				for(; it != bcaVector[curNode].end(); it++){
					unsigned nextNode = it -> first;
					double    percent = it -> second;
					if(hubnodeLevel[nextNode] != -1 && hubnodeLevel[nextNode] < level){
						unpropagatedInf[nextNode] += transInf * percent;
					}
					else{
						mapInfScore[nextNode] += transInf * percent;
					}
				}
			}
		}
		return &mapInfScore;
	}
};
#endif
