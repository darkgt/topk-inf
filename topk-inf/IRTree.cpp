#include "IURTree.h"

void IURTree::ReadLeafNodes()
{
	nodeNum = 0;
	leaves.clear();
	ifstream leafF(leafnodeFile.c_str());
	string line;
	while( getline( leafF, line) )
	{
		if(line == "")
			continue;
		istringstream iss(line);
		int leafID;
		char c;
		iss>>leafID>>c;
		vector<int> *p = new vector<int>();
		int nodes;
		while(iss>>nodes>>c)
		{
			p->push_back(nodes);
			nodeNum ++;
		}		
		leaves[leafID] = p;
	}
	leafF.close();
}

void IURTree::ReadIndexNodes()
{
	ifstream inodeF(indexnodeFile.c_str());
	string line;
	while( getline( inodeF, line) )
	{
		if(line == "")
			continue;
		istringstream iss(line);
		int nid;
		char c;
		iss>>nid>>c;
		vector<int> *p = new vector<int>();
		int nodes;
		while(iss>>nodes>>c)
		{
			p->push_back(nodes);
		}
		indexNodes.push_back(make_pair(nid, p));
	}
	inodeF.close();
}

void IURTree::ComputeVocabularyLeaf()
{
	ReadLeafNodes();
	ifstream docvecF(termweightFile.c_str());
	vector<pair<int, double>> ** docvec = new vector<pair<int, double>> *[nodeNum];
	string line;
	int count = 0;
	while( getline(docvecF, line) )		//假设能全部读入内存
	{
		if( line == "")
			continue;
		vector<pair<int, double>> *p = new vector<pair<int, double>>();
		
		istringstream iss(line);
		int keyword; char c;
		double weight;
		while(iss>>keyword>>c>>weight)
		{
			p->push_back(make_pair(keyword, weight));
			iss>>c;
		}
		docvec[count++] = p;
	}
	docvecF.close();

	map<int, vector<int> *>::iterator iter = leaves.begin();
	for(;iter != leaves.end(); ++iter)
	{
		int leafID = iter->first;			//对于每个叶结点
		vector<int> *p = iter->second;
		map<int, vector<pair<int, double>> *> inverted;

		vector<int>::iterator vi = p->begin();
		for(;vi != p->end(); ++vi)			//叶结点里的每一个对象
		{
			int id = *vi;
			vector<pair<int, double>> *words = docvec[id];
			vector<pair<int, double>>::iterator wi = words->begin();
			for(; wi!=words->end();++wi)	//对象里的每个词
			{
				int wordID= wi->first;
				map<int, vector<pair<int, double>> *>::iterator ti = inverted.find(wordID);
				if(ti != inverted.end())
					inverted[wordID]->push_back(make_pair(id, wi->second));
				else
				{
					vector<pair<int,double>> *list = new vector<pair<int, double>>();
					list->push_back(make_pair(id, wi->second));
					inverted[wordID] = list;
				}
			}
		}

		ofstream outF( (vocabFolder + MyTool::IntToString(leafID)).c_str());

		map<int, vector<pair<int, double>> *>::iterator fi;		
		for(fi = inverted.begin(); fi != inverted.end(); ++fi)
		{
			vector<pair<int,double>> *p = fi->second;
			vector<pair<int, double>>::iterator iter = p->begin();
			double maxWeight = iter->second;
			double minWeight = iter->second;

			for(; iter != p->end() ; ++iter)
			{
				outF<<iter->first<<":"<<iter->second<<",";
				if(iter->second > maxWeight)
					maxWeight = iter->second;
				if(iter->second < minWeight)
					minWeight = iter->second;
			}
			outF<<fi->first<<"\t";
			outF<<UNION<<":"<<maxWeight<<",";
			outF<<INTERSECTION<<":"<<minWeight<<endl;
			//output:
			//keywordId \t nodeId : weight , nodeid : weight , ... , UNION : maxWeight , INTERSECTION : minWeight
			delete p;
		}
		outF.close();
	}

	for(int i=0;i<nodeNum;i++)
		delete docvec[i];
	delete[] docvec;
}

void IURTree::ComputeVocabularyIndex()
{
	ReadIndexNodes();
	for(int idx = indexNodes.size() - 1; idx >= 0; idx--)		//对于每一个非叶结点
	{
		map<int, vector<pair<int, double>> *> unionInverted;
		map<int, vector<pair<int, double>> *> intersectionInverted;

		int indexID = indexNodes[idx].first;
		ofstream outF( (vocabFolder + MyTool::IntToString(indexID)).c_str(), ios::binary);

		vector<int> *p = indexNodes[idx].second;
		vector<int>::iterator iter = p->begin();
		for(; iter != p->end(); ++iter)							//对于其每个子结点，读入其对应list，然后找到每个词的最大值（或总值）
		{
			int id = *iter;			
			ifstream listF( (vocabFolder + MyTool::IntToString(id)).c_str());
			string line;
			while(getline(listF, line))
			{
				int wordID; char c;
				istringstream iss(line);
				iss>>wordID>>c;

				int docID; double value;
				double unionValue, intersectionValue;
				while(iss>>docID>>c>>value>>c){
					if(docID == UNION)
						unionValue = value;
					if(docID == INTERSECTION)
						intersectionValue = value;
				}

				if(unionInverted.count(wordID) == 0){
					unionInverted[wordID]        = new vector<pair<int, double>>;
					intersectionInverted[wordID] = new vector<pair<int, double>>;
				}
				unionInverted[wordID].push_back(make_pair(id, unionValue));
				intersectionInverted[wordID].push_back(make_pair(id, intersectionValue));
			}

			listF.close();
		}

		map<int, vector<pair<int, double>> *>::iterator liter = unionInverted.begin();
		
		for(; liter != unionInverted.end(); ++liter)
		{
			int id = liter->first;
			outF<<id<<"\t";
			vector<pair<int, double>>::iterator ti = liter->second->begin();
			double maxWeight = ti->second;
			for(; ti != liter->second->end(); ++ti)
			{
				outF<<ti->first<<":"<<ti->second<<","; //Here record the unionValue of each subnode, useless..
				if(ti->second > maxWeight)
					maxWeight = ti->second;
				delete ti->second;
			}

			ti = intersectionInverted[id].begin();
			double minWeight = ti->second;
			for(; ti != intersectionInverted[id].end(); ++ti)
			{
				if(ti->second < minWeight)
					minWeight = ti->second;
				delete ti->second;
			}
			outF<<UNION<<":"<<maxWeight<<",";
			outF<<INTERSECTION<<":"<<minWeight<<endl;
			//output:
			//keywordId \t nodeId : submaxWeight , nodeid : submaxWeight , ... , UNION : maxWeight , INTERSECTION : minWeight
		}
		outF.close();
	}
}

void IURTree::BuildIRTree()
{	
	if(!MyTool::FileExist(termweightFile))
	{
		//LanguageModel lm(termweightFile, docFile);
		//lm.ComputeLanguageModel();

		//VectorSpaceModel vsm(termweightFile, docFile);
		//vsm.ComputeVectorSpace();
	}

	RTreeIndex rt(locFile, treeFile, numOfEntry);
	if(MyTool::FileExist(treeFile + ".idx"))
		rt.ReadIndex();
	else
		rt.CreateRTree();		
	
	if(!MyTool::FileExist(leafnodeFile))
	{
		WriteNodeStrategy wns;	
		rt.getTree()->queryStrategy(wns);		//写入文件树结构
	}

	if(_chdir(vocabFolder.c_str()))
	{
		_mkdir(vocabFolder.c_str());
		ComputeVocabularyLeaf();
		ComputeVocabularyIndex();
	}

	/*
	if(_chdir(btreeFolder.c_str()))
	{
		_mkdir(btreeFolder.c_str());
		BuildBTIndex();
	}
	*/	
}