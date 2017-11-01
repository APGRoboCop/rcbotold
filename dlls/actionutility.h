#ifndef __ACTION_UTILITY_H__
#define __ACTION_UTILITY_H__

#include <vector>

using namespace std;

class CActionUtility
{
public:
	CActionUtility(eCanDoStuff action,float utility)
	{
		m_Action = action;
		m_fUtility = utility;
	}

	inline float getUtility ()
	{
		return m_fUtility;
	}

	inline eCanDoStuff getAction ()
	{
		return m_Action;
	}

private:
	eCanDoStuff m_Action;
	float m_fUtility;
};

class CActionUtilities 
{
public:
	void add (eCanDoStuff action,BOOL CanDo, float utility)
	{
		m_Utilities.push_back(CActionUtility(action,(float)CanDo*utility));
	}

	eCanDoStuff getBestAction ()		
	{
		float rand = RANDOM_FLOAT(0,getTotalUtility());

		float total = 0;

		for ( unsigned int i = 0; i < m_Utilities.size(); i ++ )
		{
			total += m_Utilities[i].getUtility();
			
			if ( m_Utilities[i].getUtility() && (rand < total) )
				return m_Utilities[i].getAction();
		}

		return BOT_CAN_NONE;
	}

	float getTotalUtility ()
	{
		float total = 0;

		for ( unsigned int i = 0; i < m_Utilities.size(); i ++ )
		{
			total += m_Utilities[i].getUtility();
		}

		return total;
	}
private:
	vector<CActionUtility> m_Utilities;
};

#endif