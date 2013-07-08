#include "IRTree.h"


int IRTree::ReadLeafNodes()
{
	int total = 0;
	leaves.clear();
	ifstream leafF(leafnodeFile.c_str());
	while( !leafF.eof())
	{
		string line;
		getline(leafF, line);
		if(line == "")
			continue;			//FUCK, �ְ���һ������ˣ��������һ�����У���������һ���ӽ�������ˣ���
		istringstream iss(line);
		int leafID ;
		char c;
		iss>>leafID>>c;
		vector<int> *p = new vector<int>();
		int nodes;
		int count = 0;
		while(iss>>nodes)
		{
			p->push_back(nodes);
			total ++;			
			iss>>c;
			count ++;
		}		
		leaves[leafID] = p;
	}	

	N2SG = new int[total];
	map<int, vector<int> *>::iterator iter;
	for(iter = leaves.begin(); iter != leaves.end() ; ++ iter)
	{
		vector<int> *p = iter->second;
		vector<int>::iterator iter2;
		for(iter2 = p->begin(); iter2 != p->end(); ++iter2)
		{
			int node = *iter2;
			N2SG[node] = iter->first;
		}
	}
	return total;
}


void IRTree::ReadIndexNodes()
{
	ifstream inodeF(indexnodeFile.c_str());
	string line;
	while( getline( inodeF, line))
	{
		if(line=="")
			continue;
		istringstream iss(line);
		int nid;char c;
		iss>>nid>>c;
		vector<int> *p = new vector<int>();
		int nodes;
		while(iss>>nodes>>c)
		{
			p->push_back(nodes);
		}
		indexNodes.push_back(make_pair<int, vector<int> *>(nid, p));
	}
	inodeF.close();
}



void IRTree::ComputeVocabularyLeaf()
{
	ifstream docvecF(termweightFile.c_str());
	vector<pair<int, double>> ** docvec = new vector<pair<int, double>> *[nodeNum];
	string line;
	int count = 0;
	while( getline(docvecF, line))		//������ȫ�������ڴ�
	{
		if( line == "")
			continue;
		vector<pair<int, double>> *p = new vector<pair<int, double>>();
		
		istringstream iss(line);
		int node; char c;
		double weight;
		while(iss>>node>>c>>weight)
		{
			p->push_back(make_pair<int, double>(node, weight));
			iss>>c;
		}
		docvec[count++] = p;
	}

	map<int, vector<int> *>::iterator iter = leaves.begin();
	for(;iter != leaves.end(); ++iter)
	{
		int leafID = iter->first;			//����ÿ��Ҷ���
		vector<int> *p = iter->second;
		map<int, vector<pair<int, double>> *> inverted;

		vector<int>::iterator vi = p->begin();
		for(;vi != p->end(); ++vi)			//Ҷ������ÿһ������
		{
			int id = *vi;
			vector<pair<int, double>> *words = docvec[id];
			vector<pair<int, double>>::iterator wi = words->begin();
			for(; wi!=words->end();++wi)	//�������ÿ����
			{
				int wordID= wi->first;
				map<int, vector<pair<int, double>> *>::iterator ti = inverted.find(wordID);
				if(ti != inverted.end())
					inverted[wordID]->push_back(make_pair<int, double>(id, wi->second));
				else
				{
					vector<pair<int,double>> *list = new vector<pair<int, double>>();
					list->push_back(make_pair<int, double>(id, wi->second));
					inverted[wordID] = list;
				}
			}
		}

		ofstream outF( (vocabFolder + MyTool::IntToString(leafID)).c_str());

		map<int, vector<pair<int, double>> *>::iterator fi;		
		for(fi = inverted.begin(); fi != inverted.end(); ++fi)
		{
			outF<<fi->first<<"\t";
			
			vector<pair<int,double>> *p = fi->second;
			vector<pair<int, double>>::iterator iter = p->begin();

			int leng = p->size();
			double max = 0;
			double min = 10;	// values not exceed 1

			for(; iter != p->end() ; ++iter)
			{
				outF<<iter->first<<":"<<iter->second<<"-"<<iter->second<<",";
				if(iter->second > max)
					max = iter->second;
				if(iter->second < min)
					min = iter->second;
			}
			outF<<-1<<":"<<max<<"-"<<min<<endl;
		}
		outF.close();
	}

	for(int i=0;i<nodeNum;i++)
		delete docvec[i];
	delete[] docvec;

	docvecF.close();	
}


void IRTree::ComputeVocabularyIndex()
{
	ReadIndexNodes();

	for(int idx = indexNodes.size() - 1; idx >= 0; idx--)		//����ÿһ����Ҷ���
	{
		map<int, vector<pair<int, pair<double, double>>> *> inverted;

		int indexID = indexNodes[idx].first;
		ofstream outF( (vocabFolder + MyTool::IntToString(indexID)).c_str(), ios::binary);

		vector<int> *p = indexNodes[idx].second;
		vector<int>::iterator iter = p->begin();
		for(; iter != p->end(); ++iter)							//������ÿ���ӽ�㣬�������Ӧlist��Ȼ���ҵ�ÿ���ʵ����ֵ������ֵ��
		{
			int id = *iter;			
			ifstream listF( (vocabFolder + MyTool::IntToString(id)).c_str());
			string line;
			while(getline(listF, line))
			{
				int wordID; char c;
				istringstream iss(line);
				iss>>wordID>>c;

				int docID; double maxvalue, minvalue;
				while(iss>>docID>>c>>maxvalue>>c>>minvalue>>c);		//read the last maximum and minimum value;
				

				map<int, vector<pair<int, pair<double, double>>> *>::iterator iter = inverted.find(wordID);
				if(iter != inverted.end())
				{
					iter->second->push_back(make_pair<int, pair<double, double>>(id, make_pair<double, double>(maxvalue, minvalue)));
				}
				else
				{
					vector<pair<int, pair<double, double>>> *p = new vector<pair<int, pair<double, double>>>();
					p->push_back(make_pair<int, pair<double, double>>(id, make_pair<double, double>(maxvalue, minvalue)));
					inverted[wordID] = p;
				}
			}

			listF.close();
		}

		map<int, vector<pair<int, pair<double, double>>> *>::iterator liter = inverted.begin();
		
		for(; liter != inverted.end(); ++liter)
		{
			outF<<liter->first<<"\t";
			vector<pair<int, pair<double, double>>>::iterator ti = liter->second->begin();
			double max = 0, min = 10;

			int leng = liter->second->size();
			for(; ti != liter->second->end(); ++ti)
			{
				outF<<ti->first<<":"<<ti->second.first<<"-"<<ti->second.second<<",";
				if(ti->second.first > max)
					max = ti->second.first;
				if(ti->second.second < min)
					min = ti->second.second;
			}
			outF<<-1<<":"<<max<<"-"<<min<<endl;
		}
		outF.close();
	}
}


/*
void IRTree::SplitDoc()
{
	ifstream textF(termweightFile.c_str());
	map<int, ofstream *> sdhandlers;
	
	string line;
	int count = 0;
	while(!textF.eof())
	{
		getline(textF, line);
		if(line == "")
			continue;

		int nid = N2SG[count];
		map<int, ofstream *>::iterator si = sdhandlers.find(nid);
		ofstream *op;
		if(si == sdhandlers.end())		//�����δ�������ͶԴ��ӽ�㽨��һ���ļ�
		{
			if(sdhandlers.size() == 500)		//��ֹ��Դ���
			{
				for(int k=0;k<50;k++)
				{
					map<int, ofstream *>::iterator replace = sdhandlers.begin();
					replace->second->close();
					sdhandlers.erase(replace);
				}
			}
			op = new ofstream((subdocFolder+MyTool::IntToString(nid)).c_str(), ios::app);
			sdhandlers[nid] = op;
		}					
		else
			op = si->second;

		(*op)<<line<<endl;
		count++;
	}
	
	map<int, ofstream *>::iterator iter = sdhandlers.begin();
	for(;iter != sdhandlers.end(); ++iter)
	{
		iter->second->close();
		delete iter->second;
	}
	textF.close();
}*/


void IRTree::BuildBTIndex()
{		
	map<int, vector<int> *>::iterator iterLeaf = leaves.begin();
	int blocklength = Blocksize;		//block size
	
	for(; iterLeaf != leaves.end(); iterLeaf++)			//����Ҷ����inverted file list
	{
		vector<int> *olist = iterLeaf->second;
		int leafID = iterLeaf->first;

		ifstream leafIFL( (vocabFolder + MyTool::IntToString(leafID)).c_str());
		
		string btFile = btreeFolder + MyTool::IntToString(leafID);
		char *btfname = new char[btFile.size()+1];
		memcpy(btfname, btFile.c_str(), btFile.size());
		btfname[btFile.size()] = '\0';
		BTree* bt=new BTree(btfname,blocklength, 0);			

		string line;
		while( getline(leafIFL, line) )
		{				
			int wordID; char c;
			istringstream iss(line);
			iss>>wordID;
			DATA *p = new DATA();
			p->key = wordID;
			for(int i=0;i<DIMENSION;i++)
				p->data[i] = 0;
			
			for(unsigned int i=0; i<olist->size(); i++)
			{
				double maxvalue, minvalue;
				int docid;
				iss>>docid>>c>>maxvalue>>c>>minvalue>>c;
				while( docid != (*olist)[i] && docid != -1)	i++;
				if(docid == -1)
					break;
				else
				{
					p->data[2*i] = (float)maxvalue;
					p->data[2*i+1] = (float)minvalue;
				}
			}
			bt->insert(p);
			delete p;
		}
		leafIFL.close();
		delete bt;
		delete btfname;
	}
	
	ReadIndexNodes();
	
	vector<pair<int, vector<int> *>>::iterator iterIndex = indexNodes.begin();
	for(; iterIndex != indexNodes.end(); ++iterIndex)
	{
		vector<int> *nlist = iterIndex->second;
		int nid = iterIndex->first;
		ifstream indexIFL( (vocabFolder + MyTool::IntToString(nid)).c_str());
		
		string btFile = btreeFolder + MyTool::IntToString(nid);
		char *btfname = new char[btFile.size()+1];
		memcpy(btfname, btFile.c_str(), btFile.size());
		btfname[btFile.size()] = '\0';
		BTree* bt=new BTree(btfname,blocklength, 0);	
		
		string line;
		while( getline(indexIFL, line))
		{				
			int wordID; char c;
			istringstream iss(line);
			iss>>wordID;
			DATA *p = new DATA();
			p->key = wordID;
			
			for(int i=0;i<DIMENSION;i++)
				p->data[i] = 0;

			for(unsigned int i=0; i<nlist->size(); i++)
			{
				double maxvalue, minvalue;
				int docid;
				iss>>docid>>c>>maxvalue>>c>>minvalue>>c;
				while( docid != (*nlist)[i] && docid != -1)	i++;
				if(docid == -1)
					break;
				else
				{
					p->data[2*i] = (float)maxvalue;
					p->data[2*i+1] = (float)minvalue;
				}
			}
			bt->insert(p);		
			delete p;
		}
		indexIFL.close();
		delete bt;
		delete btfname;
	}
}

void IRTree::BuildIRTree()
{	
	if(!MyTool::FileExist(termweightFile))
	{
		LanguageModel lm(termweightFile, docFile);
		lm.ComputeLanguageModel();
	}

	RTreeIndex rt(locFile, treeFile, numOfEntry);
	if(MyTool::FileExist(treeFile + ".idx"))
		rt.ReadIndex();
	else
		rt.CreateRTree();		
	
	if(!MyTool::FileExist(leafnodeFile))
	{
		WriteNodeStrategy wns;		
		rt.getTree()->queryStrategy(wns);		//д���ļ����ṹ
	}

	nodeNum = ReadLeafNodes();

	/*if(_chdir(subdocFolder.c_str()))
	{
		_mkdir(subdocFolder.c_str());
		SplitDoc();
	}*/

	if(_chdir(vocabFolder.c_str()))
	{
		_mkdir(vocabFolder.c_str());
		ComputeVocabularyLeaf();
		ComputeVocabularyIndex();
	}

	if(_chdir(btreeFolder.c_str()))
	{
		_mkdir(btreeFolder.c_str());
		BuildBTIndex();
	}	
}