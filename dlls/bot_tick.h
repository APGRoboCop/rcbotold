#ifndef __BOT_TICK_H__
#define __BOT_TICK_H__

#include <list>

class CTickable
{
public:
	virtual ~CTickable() = default;

	CTickable()
	{
		m_bDone = false;
		m_iTicksLeft = 0;
	}

	void reset()
	{
		m_bDone = false;
		m_iTicksLeft = 0; // ?
		//reset();
	}

	virtual void _reset();

	virtual void tick();
	bool isDone() const { return m_bDone; }
	// more priorty with those with more ticks required to complete
	bool operator < (const CTickable* other) const { return m_iTicksLeft > other->ticksLeft(); }
	unsigned int ticksLeft() const { return m_iTicksLeft; }

private:
	bool m_bDone;
	unsigned int m_iTicksLeft;
};

class CTickManager
{
public:
	void add(CTickable* pTickable)
	{
		m_Tickables.emplace_back(pTickable);
	}

	void run()
	{
		unsigned int iTicks = 300;

		// round robin
		m_Tickables.sort();

		for (CTickable* pCurrent : m_Tickables)
		{
			if (pCurrent->isDone())
				continue;

			for (unsigned int i = 0; i < iTicks; i++)
				pCurrent->tick();

			iTicks = iTicks / 2;
		}
	}
private:
	std::list<CTickable*> m_Tickables;
};

#endif