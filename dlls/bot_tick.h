#ifndef __BOT_TICK_H__
#define __BOT_TICK_H__

#include <list>
using namespace std;

class CTickable
{
public:
	CTickable()
	{
		m_bDone = false;
	}

	void reset () 
	{ 
		m_bDone = false;
		m_iTicksLeft = 0; // ?
		reset();
	}

	virtual void _reset ();

	virtual void tick ();
	inline bool isDone () { return m_bDone; }
	// more priorty with those with more ticks required to complete
	bool operator < ( CTickable *other ) { return m_iTicksLeft > other->ticksLeft(); }
	inline unsigned int ticksLeft () { return m_iTicksLeft; }

private:
	bool m_bDone;
	unsigned int m_iTicksLeft;
};

class CTickManager
{
public:
	void add ( CTickable *pTickable )
	{
		m_Tickables.push_back(pTickable);
	}

	void run ()
	{
		unsigned int iTicks = 300;

		// round robin
		list<CTickable*>::const_iterator it;

		m_Tickables.sort();

		for ( it = m_Tickables.begin(); it != m_Tickables.end(); ++it )
		{
			CTickable *pCurrent = *it;

			if ( pCurrent->isDone() )
				continue;
			
			for ( register unsigned int i = 0; i < iTicks; i ++ )
				pCurrent->tick();

			iTicks = iTicks/2;
		}
	}
private:
	list<CTickable*> m_Tickables;
};

#endif