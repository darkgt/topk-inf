#ifndef _CLUSTERINDEX_H_
#define _CLUSTERINDEX_H_

#include <map>

extern string clusterFile;
extern string utermweightFile;
extern string userloc;
extern int nodenum;

class ClusterInfo{
private:
	bool initialized;
public:
	double l, d; // (l, d) left-down position
	double r, u; // (r, u) right-up position
	map<int, double> termWeight;
	vector<int>userId;
	ClusterInfo(){
		initialized = false;
	}
	~ClusterInfo(){}

	void updatePosition(double x, double y){
		if(!initialized){
			initialized = true;
			l = r = x;
			u = d = y;
		}
		else{
			if(x < l)
				l = x;
			if(x > r)
				r = x;
			if(y < d)
				d = y;
			if(y > u)
				u = y;
		}
	}
};

class ClusterIndex{
protected:
	int clusternum;
	int *userGroupId;
	ClusterInfo *cluInfo;
public:
	ClusterIndex(int clusternum);
	virtual ~ClusterIndex();

	void ReadUserCluster();
	void ReadUserInfo();
};

#endif