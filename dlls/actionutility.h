#ifndef __ACTION_UTILITY_H__
#define __ACTION_UTILITY_H__

#include <vector>

//using namespace std;

class CActionUtility
{
public:
	CActionUtility(eCanDoStuff action, const float utility)
	{
		m_Action = action;
		m_fUtility = utility;
	}

	float getUtility() const
	{
		return m_fUtility;
	}

	eCanDoStuff getAction() const
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
	void add(eCanDoStuff action, const bool CanDo, const float utility)
	{
		//m_Utilities.emplace_back(CActionUtility(action, (float)CanDo * utility));
		m_Utilities.emplace_back(action, static_cast<float>(CanDo) * utility);
	}

	eCanDoStuff getBestAction() const
	{
		const float rand = RANDOM_FLOAT(0, getTotalUtility());

		float total = 0.0f;

		for (CActionUtility m_Utility : m_Utilities)
		{
			total += m_Utility.getUtility();

			if (m_Utility.getUtility() && rand < total)
				return m_Utility.getAction();
		}

		return BOT_CAN_NONE;
	}

	float getTotalUtility() const
	{
		float total = 0.0f;

		for (CActionUtility m_Utility : m_Utilities)
		{
			total += m_Utility.getUtility();
		}

		return total;
	}
private:
	std::vector<CActionUtility> m_Utilities;
};

#endif