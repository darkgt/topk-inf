/*
 *	the interface for searching in IR-tree, utilize	the IQueryStrategy class in the R-tree library
 */

#ifndef _SEARCHIBRTREE_
#define _SEARCHIBRTREE_

#include "Tool.h"
#include "data.h"
#include "btree.h"

extern string subdocFolder;
extern string btreeFolder;

class SearchIBRTree : public SpatialIndex::IQueryStrategy
{
private:
	//priority_queue<RTreeNode *, deque<RTreeNode *>, NodeValueLess<RTreeNode> > *queueNode;
	queue<RTreeNode *> *queueNode;
	Query *Q;
	int nodeNum;
	queue<id_type> ids;
	vector<int> queryvec;
	
public:
	SearchIBRTree(Query *Q, int nodeNum)
	{
		this->Q = Q;
		this->nodeNum = nodeNum;

		istringstream iss(Q->text);
		int wid;
		char c;
		while(iss>>wid)
		{
			queryvec.push_back(wid);
			iss>>c;
		}
		queueNode = new queue<RTreeNode *>;
	}

	~SearchIBRTree(){
		while(!queueNode->empty()){
			RTreeNode *p = queueNode->back();
			queueNode->pop();
			delete p;
		}
		delete queueNode;
	}

	void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext)
	{
		const INode* n = dynamic_cast<const INode*>(&entry);
		if (n != 0)
		{
			int pid = n->getIdentifier();
			string btFile = btreeFolder + MyTool::IntToString(pid);
			char *btfname = new char[btFile.size()+1];
			memcpy(btfname, btFile.c_str(), btFile.size());		
			btfname[btFile.size()] = '\0';
			BTree *bt = new BTree(btfname, 0);
			
			vector<int>::iterator iter = queryvec.begin();
			set<int> indexID;	//the id of child nodes that contain query keywords
			for(;iter != queryvec.end(); ++iter)
			{
				int wordID = *iter;
				VECTYPE *data = new VECTYPE[DIMENSION];
				bool flag = bt->search(wordID, &data);
				if(flag)
				{
					for(int i=0;i<DIMENSION;i++)
					{
						if(data[i] > 0)							
						{
							unsigned char mask = 1;
							for(int j=0;j<8;j++)
							{
								if((data[i] & mask) > 0){
									indexID.insert(i*8+j);
								}
								mask = mask << 1;
							}
						}
					}
				}
				delete[] data;					
			}
			delete bt;
			delete[] btfname;

			if(n->isIndex())		
			{
				set<int>::iterator si = indexID.begin();
				for(;si != indexID.end(); ++si)		//compute the distance between child nodes and the query point
				{
					uint32_t cChild = *si;
					RTreeNode *rtp = new RTreeNode;
					IShape *out;
					n->getChildShape(cChild, &out);
					Region* pr = dynamic_cast<Region *>(out);	
					rtp->identifier = n->getChildIdentifier(cChild);
					rtp->pr = pr;

					double coor[2];
					coor[0] = Q->x; coor[1] = Q->y;
					Point pt(coor, 2);

					double distScore = MyTool::ComputeMinPossible(pr, &pt);

					rtp->minValue = distScore;
					queueNode->push(rtp);
				}
			}			
			
			if(n->isLeaf())
			{
				set<int>::iterator si = indexID.begin();
				for(;si != indexID.end(); ++si)
				{
					uint32_t cChild = *si;
					int cid = n->getChildIdentifier(cChild);
					
					IShape *out;
					n->getChildShape(cChild, &out);
					Region* pr = dynamic_cast<Region *>(out);	

					double dx = Q->x - pr->m_pHigh[0];
					double dy = Q->y - pr->m_pHigh[1];											

					double dist = sqrt(dx * dx + dy * dy);
					
					Object cur;
					cur.x = pr->m_pHigh[0], cur.y = pr->m_pHigh[1];
					cur.oid = cid;
					cur.mask = keywordsmask[cChild];

					if(dist < maxlen){
						//topk[dist] = cur;
						res.push_back(cur);
					}
					/*
					if(topk.size() > K)
						topk.erase(topk.begin());
					*/
				}		
			}	
		}

		if (!queueNode->empty())		
		{
			RTreeNode *p = queueNode->front();
			//if(!topk.empty() && p->minValue > topk.begin()->first)	//check if can terminate
			if(p->minValue > maxlen)
			{
				hasNext = false;
			}
			else
			{
				nextEntry = p->identifier;
				hasNext = true;
			}

			queueNode->pop();
			delete p;
		}
	
		else 
			hasNext = false;
	}
};


#endif