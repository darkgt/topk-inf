#ifndef CONTRILIST
#define CONTRILIST

#include <vector>

struct Topk{ 
	double upperBound;
	double lowerBound;
	int id;
	unsigned nodeNum;
};

class ContributionList{
protected:
	int ksize;
	std::vector<Topk>list;
	int elementNum;
public:
	void setKsize(int k){
		ksize = k;
		elementNum = 0;
	}

	void addEstimation(double u, double l, int id, unsigned nodeNum){
		Topk top = {u, l, id, nodeNum};
		std::vector<Topk>::iterator it = list.begin();
		for(; it != list.end(); it++)
			if( (*it).upperBound < u )
				break;
		list.insert(it, top);
		elementNum += nodeNum;
	}

	bool eraseEstimation(int id){
		std::vector<Topk>::iterator it = list.begin();
		for(; it != list.end(); it++)
			if( (*it).id == id ){
				elementNum += (*it).nodeNum;
				list.erase(it);
				return true;
			}
		return false;
	}
};

class ReverseTopk: public ContributionList{
protected:
	double threshold;
public:
	double setThreshold(double s){
		threshold = s;
	}
	
	bool canStop(){
		return elementNum < ksize;
	}
};
#endif