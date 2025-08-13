/*
 *    This file is part of RCBot.
 *
 *    RCBot by Paul Murphy adapted from botman's template 3.
 *
 *    RCBot is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    RCBot is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RCBot; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
 //////////////////////////////////////////////////
 // RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
 //
 // (http://www.rcbot.net)
 //
 // Based on botman's High Ping Bastard bot
 //
 // (http://planethalflife.com/botman/)
 //
 // bot_ns.h
 //
 //////////////////////////////////////////////////
 //
 // Natural Selection definitions header
#ifndef __BOT_NS_H__
#define __BOT_NS_H__

// TODO: Experimental and NS AlienAction may need to be tested [APG]RoboCop[CL]
typedef enum : std::uint8_t
{
	ACTION_BUILD_DEF,
	ACTION_BUILD_OFF,
	ACTION_BUILD_SENS,
	ACTION_BUILD_MOV,
	ACTION_BUILD_RESTOWER,
	ACTION_BUILD_HIVE,
	ACTION_HEAL_PLAYER,
	ACTION_MAX
}eAlienAction;

typedef enum : std::uint8_t
{
	ACTION_RES_FASTER_RESOURCES = 1,
	ACTION_RES_MORE_HEALTH = 2,
	ACTION_RES_MORE_ABILITIES = 4,
	ACTION_RES_MORE_SPAWNPOINTS = 8,
	ACTION_RES_MORE_DEFENCES = 16,
	ACTION_RES_MORE_TRAITS = 32,
	ACTION_RES_CLOACKED = 64
}eMaskAlienActionResult;

typedef enum
{
	MASK_EVD_SLOW_RESOURCES = 1, // 1
	MASK_EVD_NORM_RESOURCES = 2, // 2
	MASK_EVD_FAST_RESOURCES = 4, // 3
	MASK_EVD_LOW_RESOURCES = 8, // 4
	MASK_EVD_FULL_RESOURCES = 16, // 5
	MASK_EVD_LOW_HEALTH = 32,
	MASK_EVD_MID_HEALTH = 64,
	MASK_EVD_FULL_HEALTH = 128,
	MASK_EVD_MIN_HIVES_UP = 256,
	MASK_EVD_MAX_HIVES_UP = 512,
	MASK_EVD_LOSING = 1024,
	MASK_EVD_BALANCED = 2048,
	MASK_EVD_WINNING = 4096,
	MASK_EVD_MANY_DEFS = 8192,
	MASK_EVD_MANY_OFFS = 16384,
	MASK_EVD_MANY_SENS = 32768,
	MASK_EVD_MANY_MOVS = 65536
}eAlienMaskEvidence;

class CAlienAction
{
public:
	CAlienAction(const eAlienAction action, const eMaskAlienActionResult result)
	{
		m_action = action;
		m_result = result;
	}

	float Utility() const
	{
		float fUtility = 1.0f;

		if (m_result & ACTION_RES_FASTER_RESOURCES)
			fUtility *= 1.1f;
		if (m_result & ACTION_RES_MORE_HEALTH)
			fUtility *= 0.8f;
		if (m_result & ACTION_RES_MORE_ABILITIES)
			fUtility *= 1.05f;
		if (m_result & ACTION_RES_MORE_SPAWNPOINTS)
			fUtility *= 1.1f;
		if (m_result & ACTION_RES_MORE_DEFENCES)
			fUtility *= 0.9f;
		if (m_result & ACTION_RES_CLOACKED)
			fUtility *= 0.5f;
		return fUtility;
	}

	float ResultProbability(eAlienMaskEvidence evd) const
	{
		float fProbability = 1.0f;

		if (m_result & ACTION_RES_FASTER_RESOURCES)
		{
			if (evd & MASK_EVD_SLOW_RESOURCES)
				fProbability *= 1.0f;
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.6f;
			if (evd & MASK_EVD_FAST_RESOURCES)
				fProbability *= 0.3f;
			if (evd & MASK_EVD_LOSING)
				fProbability *= 0.9f;
		}
		if (m_result & ACTION_RES_MORE_HEALTH)
		{
			if (evd & MASK_EVD_LOW_HEALTH)
				fProbability *= 1.0f;
			if (evd & MASK_EVD_BALANCED)
				fProbability *= 0.6f;
			if (evd & MASK_EVD_LOSING)
				fProbability *= 0.9f;
		}
		if (m_result & ACTION_RES_MORE_ABILITIES)
		{
			if (evd & MASK_EVD_SLOW_RESOURCES)
				fProbability *= 1.0f;
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.6f;
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.3f;
			if (evd & MASK_EVD_LOSING)
				fProbability *= 0.9f;
		}
		if (m_result & ACTION_RES_MORE_SPAWNPOINTS)
		{
			if (evd & MASK_EVD_MIN_HIVES_UP)
				fProbability *= 1.0f;
			else if (evd & MASK_EVD_MAX_HIVES_UP)
				fProbability *= 0.0f;
			else
				fProbability *= 0.5f;

			if (evd & MASK_EVD_FAST_RESOURCES)
				fProbability *= 0.9f;
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.7f;
			if (evd & MASK_EVD_SLOW_RESOURCES)
				fProbability *= 0.2f;
		}
		if (m_result & ACTION_RES_MORE_DEFENCES)
		{
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.6f;
			if (evd & MASK_EVD_NORM_RESOURCES)
				fProbability *= 0.3f;
			if (evd & MASK_EVD_LOSING)
				fProbability *= 0.9f;
		}
		if (m_result & ACTION_RES_CLOACKED)
		{
			if (evd & MASK_EVD_BALANCED)
				fProbability *= 0.7f;
			if (evd & MASK_EVD_WINNING)
				fProbability *= 0.9f;
			if (evd & MASK_EVD_MANY_SENS)
				fProbability *= 0.1f;
		}

		if (m_result & ACTION_RES_MORE_TRAITS)
		{
			if (evd & MASK_EVD_MANY_DEFS)
				fProbability *= 0.66f;
			if (evd & MASK_EVD_MANY_SENS)
				fProbability *= 0.66f;
			if (evd & MASK_EVD_MANY_MOVS)
				fProbability *= 0.66f;
			return fProbability;
		}
		return 0;
	}
private:
	eAlienAction m_action;
	eMaskAlienActionResult m_result;
};

class CAlienActions
{
public:
	void setup()
	{
		m_Actions.emplace_back(ACTION_BUILD_DEF, eMaskAlienActionResult(ACTION_RES_MORE_DEFENCES | ACTION_RES_MORE_TRAITS | ACTION_RES_MORE_HEALTH));
		m_Actions.emplace_back(ACTION_BUILD_OFF, eMaskAlienActionResult(ACTION_RES_MORE_DEFENCES));
		m_Actions.emplace_back(ACTION_BUILD_SENS, eMaskAlienActionResult(ACTION_RES_CLOACKED | ACTION_RES_MORE_TRAITS));
		m_Actions.emplace_back(ACTION_BUILD_MOV, eMaskAlienActionResult(ACTION_RES_MORE_TRAITS));
		m_Actions.emplace_back(ACTION_BUILD_RESTOWER, eMaskAlienActionResult(ACTION_RES_FASTER_RESOURCES));
		m_Actions.emplace_back(ACTION_BUILD_HIVE, eMaskAlienActionResult(ACTION_RES_MORE_SPAWNPOINTS + ACTION_RES_MORE_ABILITIES));
		m_Actions.emplace_back(ACTION_HEAL_PLAYER, eMaskAlienActionResult(ACTION_RES_MORE_HEALTH));
	}

	CAlienAction* getBestAction(eAlienMaskEvidence evd)
	{
		bool gotBest = false;
		float fMax = 0.0f;
		CAlienAction* best = nullptr;

		for (CAlienAction& m_Action : m_Actions)
		{
			const float fCur = m_Action.Utility() * m_Action.ResultProbability(evd);
			if (!gotBest || fCur > fMax)
			{
				best = &m_Action;
				fMax = fCur;
				gotBest = true;
			}
		}
		return best;
	}
private:
	std::vector<CAlienAction> m_Actions;
};

// TODO: may need to be tested [APG]RoboCop[CL]
/*typedef enum
{
	CH_BASE_ATTACK = 1,
	CH_ENEMY_SPOT = 2,
	CH_NO_RESOURCES = 4,
	CH_ENEMY_STRUCT = 8,
	CH_ENEMY_RESOURCE_ON_FOUNTAIN = 16,
	CH_ENEMY_HIVE = 32
}eCommanderHappen;

typedef enum
{
	CE_ENEMY_SPOT = 1,
	CE_MARINE_SPOT_ENEMY = 2,
	CE_MARINE_DEATH = 4,
	CE_ALIEN_SOUND = 8
}eCommanderEvidence;

class CCommEvidence
{
public:

private:
	eCommanderEvidence m_iType;
	ga_value m_fWeight;
};

class CCommanderEvidenceList
{
public:
	void addEvidence();
	void removeEvidence();
	void workProbability(Vector loc, float fRange);
private:
	std::vector<CCommEvidence*> m_evd;
};

void CBot::CommanderStrategy()
{
}

float CBot::CommanderUtility(eCommanderAction action, Vector vLocation)
{
	switch (action)
	{
	case CMD_ACT_BUILD_TURRET_FACTORY:

	case CMD_ACT_BUILD_TURRET:
	case CMD_ACT_UPGRADE_TURRET_FACTORY:
	}
}

float CBot::CommanderProbability(eCommanderHappen happen, std::vector<comm_evid_t*> evd)
{
	switch (happen)
	{
	}
}*/

#endif // BOT_NS_H