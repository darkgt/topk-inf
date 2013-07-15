#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <map>
#include <cmath>
#include <vector>
#include <string>
#include <SpatialIndex.h>
using namespace std;
using namespace SpatialIndex;


class Vertex
{
public:
	Vertex(string text, double x , double y)
	{
		this->text = text;
		this->x = x;
		this->y = y;
	}
	string text;
	double x, y;
};


class MyTool
{

public:

	static string IntToString(int d)
	{
		ostringstream oss;
		oss<<d;
		return oss.str();
	}

	static int parseStr(string s, map<int, double> & res)
	{
		vector<int> words;
		SplitStr(s, words);
		for(unsigned int i=0;i<words.size();i++)
		{
			int w = words[i];
			map<int, double>::iterator iter = res.find(w);
			if( iter == res.end())
			{
				res[w] = 1.0;
			}
			else
			{
				double tmp = res[w];
				res[w] = tmp + 1;
			}
		}
		return words.size();
	}

	static void SplitStr(string s, vector<int> & res)			//分词，重复词会出现
	{
		istringstream iss(s);
		int d; char c;
		iss>>d;
		res.push_back(d);
		while(iss>>c>>d)
		{
			res.push_back(d);
		}
	}

	static double CosSim(string s1, string s2, map<int, vector<int> *> & index)
	{
		int total = (*index[-1])[0];
		map<int, double> v1, v2;
		int numWords1 = parseStr(s1, v1);
		int numWords2 = parseStr(s2, v2);
		if(numWords1 == 0 || numWords2 == 0)		//doc为空
			return 0.0;
		
		map<int, double>::iterator iter = v1.begin();
		double Len1 = 0.0;
		for(;iter != v1.end(); ++ iter)
		{
			double tmp = iter->second / numWords1;		//tf			
			tmp *= log( (double)total/(double)index[iter->first]->size());	//idf
			Len1 += tmp*tmp;
			v1[iter->first] = tmp;
		}
		Len1 = sqrt(Len1);	//

		iter = v2.begin();
		double Len2 = 0.0;
		double sim = 0.0;
		for(;iter != v2.end(); ++ iter)
		{
			double tmp = iter->second/ numWords2;	//tf			
			tmp *= log( (double)total/(double)index[iter->first]->size());
			Len2 += tmp*tmp;			
			map<int, double>::iterator rf = v1.find(iter->first);
			if(rf != v1.end())
			{
				sim += tmp * v1[iter->first];
			}
		}
		Len2 = sqrt(Len2);

		double result = sim / (Len1 * Len2);
		return result;		
	}

	static double DocVector(string & doc, map<int, double> & vec, map<int, vector<int> *> * index)
	{		 
		int total = (*(*index)[-1])[0];	

		int numWords = MyTool::parseStr(doc, vec);
		map<int, double>::iterator iter = vec.begin();
		double Len = 0.0;
		for(;iter != vec.end(); ++ iter)
		{
			double tmp = iter->second / numWords;		//tf			
			tmp *= log( (double)total/(double)(*index)[iter->first]->size());	//idf
			Len += tmp*tmp;
			iter->second = tmp;
		}
		Len = sqrt(Len);	
		return Len;
	}

	static bool FileExist(string &filename)
	{
		fstream _file;
		_file.open(filename.c_str(),ios::in);
		if(!_file)
			return false;
		else
			return true;
		_file.close();
	}

};

#endif