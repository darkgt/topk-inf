#ifndef _MYSTRATEGY_
#define _MYSTRATEGY_

#include <SpatialIndex.h>
#include <iostream>
#include <queue>

// example of a Strategy pattern.
// traverses the tree by level.

using namespace SpatialIndex;
using namespace std;

class MyQueryStrategy : public SpatialIndex::IQueryStrategy
{
private:
	queue<id_type> ids;

public:
	void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext)
	{
		IShape* ps;
		entry.getShape(&ps);
		Region* pr = dynamic_cast<Region*>(ps);

		//cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl;
		//cout << pr->m_pHigh[0] << " " << pr->m_pLow[1] << endl;
		//cout << pr->m_pHigh[0] << " " << pr->m_pHigh[1] << endl;
		//cout << pr->m_pLow[0] << " " << pr->m_pHigh[1] << endl;
		//cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl << endl << endl;
			// print node MBRs gnuplot style!

		delete ps;

		const INode* n = dynamic_cast<const INode*>(&entry);

		// traverse only index nodes at levels 2 and higher.
		if (n != 0 && n->getLevel() > 1)
		{
			for (uint32_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
			{
				ids.push(n->getChildIdentifier(cChild));
			}
		}

		if (! ids.empty())
		{
			nextEntry = ids.front(); ids.pop();
			hasNext = true;
		}
		else
		{
			hasNext = false;
		}
	}
};
#endif