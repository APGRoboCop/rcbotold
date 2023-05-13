#ifndef __ACTION_UTILITY_H__
#define __ACTION_UTILITY_H__

#include <vector>

//using namespace std;

class CActionUtility
{
public:
	CActionUtility(eCanDoStuff action, bool utility)
	{
		m_Action = action;
		m_fUtility = utility;
	}

	float getUtility()
	{
		return m_fUtility;
	}

	eCanDoStuff getAction()
	{
		return m_Action;
	}

private:
	eCanDoStuff m_Action;
	bool m_fUtility;
};

class CActionUtilities
{
public:
	void add(eCanDoStuff action, BOOL CanDo, float utility)
	{
		//m_Utilities.emplace_back(CActionUtility(action, (float)CanDo * utility));
		m_Utilities.emplace_back(action, static_cast<float>(CanDo) * utility);
	}

	eCanDoStuff getBestAction()
	{
		float rand = RANDOM_FLOAT(0, getTotalUtility());

		float total = 0.0f;

		for (unsigned int i = 0; i < m_Utilities.size(); i++)
		{
			total += m_Utilities[i].getUtility();

			if (m_Utilities[i].getUtility() && rand < total)
				return m_Utilities[i].getAction();
		}

		return BOT_CAN_NONE;
	}

	float getTotalUtility()
	{
		float total = 0.0f;

		for (unsigned int i = 0; i < m_Utilities.size(); i++)
		{
			total += m_Utilities[i].getUtility();
		}

		return total;
	}
private:
	std::vector<CActionUtility> m_Utilities;
};

#endif