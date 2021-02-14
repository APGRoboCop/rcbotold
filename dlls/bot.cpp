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
 // bot.cpp
 //
 //////////////////////////////////////////////////
 //
 // Bot functions
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif
#include "cbase.h" // Need CBASE for classify()
//#include "player.h"
#include "bot_const.h"
#include "bot.h"
#include "waypoint.h"
#include "bot_weapons.h"
#include "perceptron.h"
#include "bot_visibles.h"
//#include "megahal.h"
#include "actionutility.h"

#include "usercmd.h"

#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>

// for logging
#include <ctime>

// BOT GLOBALS

// engine functions
extern DLL_FUNCTIONS gFunctionTable;

// quick waypoint access
extern CWaypointLocations WaypointLocations;

extern CBotGlobals gBotGlobals; // defined in DLL.CPP

// all waypoints
extern WAYPOINTS waypoints;

// safe - freeing... ??
// shove in the POINTER to the pointer to free, so we can (nullify)
// not used yet (I'm scared :P)
// it works though...
void safefree(void** p)
{
	if (!p)
		return;
	if (!*p)
		return;
	free(*p);

	// nullify freed pointer
	*p = nullptr;
}

// BOT CLASS FUNCTIONS
//

// sets bots edict to pEdict, sets pev etc, sets-up so it can work :p
void CBot::SetEdict(edict_t* pSetEdict)
{
	if (pSetEdict == nullptr)
	{
		// initialising bot? / problem?
		// reset pev etc
		m_pEdict = nullptr;
		pev = nullptr;

		m_iRespawnState = RESPAWN_NEED_TO_RESPAWN;
	}
	else
	{
		m_pEdict = pSetEdict;
		pev = &m_pEdict->v;
		pev->flags |= FL_FAKECLIENT;
	}
}

// Commanding can be done when NS 3 is out maybe
// (use hotkeys?)
void CBot::BotCommand(void)
// Commanding not yet complete
{
	//	static BOOL s_bIssueCommand;
	//	static Vector s_vCommandOrigin;
	//	static edict_t *iPlayerToSelect;
	/*
	typedef struct usercmd_s
{
	short	lerp_msec;      // Interpolation time on client
	byte	msec;           // Duration in ms of command
	vec3_t	viewangles;     // Command view angles.

// intended velocities
	float	forwardmove;    // Forward velocity.
	float	sidemove;       // Sideways velocity.
	float	upmove;         // Upward velocity.
	byte	lightlevel;     // Light level at spot where we are standing.
	unsigned short  buttons;  // Attack buttons
	byte    impulse;          // Impulse command issued.
	byte	weaponselect;	// Current weapon id

// Experimental player impact stuff.
	int		impact_index;
	vec3_t	impact_position;
} usercmd_t;
*/
//usercmd_t pCommandMove;

	int iScheduleId;

	// this function is not used yet.. dont even know if it works!!

	pev->impulse = 0;
	pev->button = 0;

	m_CurrentTask = m_Tasks.CurrentTask();

	if (m_fHoldAttackTime > gpGlobals->time)
		PrimaryAttack();

	if (m_CurrentTask)
	{
		switch (m_CurrentTask->Task())
		{
		case BOT_COMMAND_TASK_MOVE_TO_VECTOR:
		{
			Vector vMoveTo = m_CurrentTask->TaskVector();

			SET_ORIGIN(m_pEdict, vMoveTo);

			//pev->flags &= ~FL_ONGROUND;
			//pev->origin.x = vMoveTo.x;
			//pev->origin.y = vMoveTo.y;

			m_Tasks.FinishedCurrentTask();
		}
		break;
		case BOT_COMMAND_TASK_SELECT_POINT:
		{
			PrimaryAttack();

			m_Tasks.FinishedCurrentTask();
		}
		break;
		case BOT_COMMAND_TASK_SELECT_AREA:
		{
			const float fRange = m_CurrentTask->TaskFloat();

			Vector vOrigin = m_CurrentTask->TaskVector();
			vOrigin.z = pev->origin.z;

			Vector vMinOrigin = vOrigin;
			vMinOrigin.x -= fRange;
			vMinOrigin.y -= fRange;

			Vector vMaxOrigin = vOrigin;
			vMaxOrigin.x += fRange;
			vMaxOrigin.y += fRange;

			iScheduleId = m_CurrentTask->GetScheduleId();

			m_Tasks.FinishedCurrentTask();

			m_fHoldAttackTime = gpGlobals->time + 1.0;

			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_MOVE_TO_VECTOR, iScheduleId, nullptr, 0, 0, vMinOrigin));
			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_SELECT_POINT, iScheduleId));
			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_MOVE_TO_VECTOR, iScheduleId, nullptr, 0, 0, vMaxOrigin));
			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_SELECT_POINT, iScheduleId));
		}
		break;
		case BOT_COMMAND_TASK_BUILD_OBJECT:
		{
			Impulse(m_CurrentTask->TaskInt());

			iScheduleId = m_CurrentTask->GetScheduleId();

			m_Tasks.FinishedCurrentTask();

			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_MOVE_TO_VECTOR, iScheduleId, nullptr, 0, 0, m_CurrentTask->TaskVector()));
			AddPriorityTask(CBotTask(BOT_COMMAND_TASK_ISSUE_COMMAND, iScheduleId, nullptr, 0, 0, m_CurrentTask->TaskVector()));
		}
		break;
		case BOT_COMMAND_TASK_ISSUE_COMMAND:
		{
			SecondaryAttack();

			m_Tasks.FinishedCurrentTask();
		}
		break;
		case BOT_TASK_WAIT:
		{
			// let go of buttons & impulse
			pev->button = 0;
			pev->impulse = 0;
			m_Tasks.FinishedCurrentTask();
		}
		break;
		default:
			break;
		}
	}
	else
	{
		static CStructures pBases;

		if (m_bCommInit)
		{
			pBases.FreeLocalMemory();

			m_bCommInit = FALSE;
		}
		else
		{
			int iUnderAttackPriority;

			//			edict_t *pNewbuilding = BotFunc_NS_MarineBuild();

			pBases.Tick(&iUnderAttackPriority);
		}

		// Not got certain buildings built yet......?

		// find out from setTech net message(s) ...??? what happened to em?
	}
}
/*

  typedef enum
  {
	CH_BASE_ATTACK = 1,
	CH_ENEMY_SPOT = 2,
	CH_NO_RESOURCES = 4,
	CH_ENEMY_STRUCT = 8
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
		void addEvidence ();
		void removeEvidence ();
		void workProbability ( Vector loc, float fRange )
	private:
		vector<CCommEvidence*> m_evd;
  };

void CBot :: CommanderStrategy ()
{
}

float CBot :: CommanderUtility ( eCommanderAction action, Vector vLocation )
{
	switch ( action )
	{
	case CMD_ACT_BUILD_TURRET_FACTORY:

	case CMD_ACT_BUILD_TURRET:
	case CMD_ACT_UPGRADE_TURRET_FACTORY:
	}
}

float CBot :: CommanderProbability ( eCommanderHappen happen, vector<comm_evid_t *> evd )
{
	switch ( happen )
	{
	}
}*/

BOOL CBot::FacingIdeal(void) const
{
	// looking within "2.0" degrees of target?
	return std::fabs(UTIL_AngleDiff(pev->ideal_yaw, pev->v_angle.y)) < 2.0 &&
		std::fabs(UTIL_AngleDiff(pev->idealpitch, pev->v_angle.x)) < 2.0;
}

// get distance between edict1 and edict2
float BotFunc_DistanceBetweenEdicts(edict_t* pEdict1, edict_t* pEdict2)
{
	return (pEdict1->v.origin - pEdict2->v.origin).Length();
}

BOOL CBot::IsInVisibleList(edict_t* pEntity) const
{
	return m_pVisibles->isVisible(ENTINDEX(pEntity));
}

void CBot::BotEvent(const eBotEvent iEvent, edict_t* pInfo, edict_t* pExtInfo, float* pFloatInfo)
// a type of event happens on the bot
{
	CClient* pClient = nullptr;
	CClient* pExtClient = nullptr;

	if (pInfo)
		pClient = gBotGlobals.m_Clients.GetClientByEdict(pInfo);
	if (pExtInfo)
		pExtClient = gBotGlobals.m_Clients.GetClientByEdict(pExtInfo);

	switch (iEvent)
	{
	case BOT_EVENT_LEAVING:
		BotChat(BOT_CHAT_LEAVEGAME, nullptr, TRUE);
		break;
	case BOT_EVENT_FAIL_TASK:
		if (m_CurrentTask && gBotGlobals.IsDebugLevelOn(BOT_DEBUG_THINK_LEVEL))
		{
			DebugMessage(BOT_DEBUG_THINK_LEVEL, gBotGlobals.m_pListenServerEdict, 0, "%s failed task \"%s\" part of schedule \"%s\"", m_szBotName, m_CurrentTask->getTaskDescription(), m_CurrentTask->getScheduleDescription());
		}
		break;
	case BOT_EVENT_COMPLETE_TASK:
		if (m_CurrentTask && gBotGlobals.IsDebugLevelOn(BOT_DEBUG_THINK_LEVEL))
		{
			DebugMessage(BOT_DEBUG_THINK_LEVEL, gBotGlobals.m_pListenServerEdict, 0, "%s finished task \"%s\" part of schedule \"%s\"", m_szBotName, m_CurrentTask->getTaskDescription(), m_CurrentTask->getScheduleDescription());
		}
		break;
	case BOT_EVENT_HEAR_TEAMMATE_DIE:
		// Didn't see the team mate but was near by and is visible from bot
	{
		if (!m_pEnemy)
			AddPriorityTask(CBotTask(BOT_TASK_FACE_EDICT, 0, pInfo, 0, 1.0));
	}
	break;
	case BOT_EVENT_SEE_TEAMMATE_DIE:
	{
		// Saw a team-mate die

		// Can I see the guy that killed my teammate?
		if (IsInVisibleList(pExtInfo))
		{
			// is it an enemy?
			if (IsEnemy(pExtInfo))
			{
				// Attack the enemy
				m_pEnemy = pInfo;
				AddPriorityTask(CBotTask(BOT_TASK_NORMAL_ATTACK, m_Tasks.GetNewScheduleId(), pInfo));
			}
			// team kill
			else
			{
				// like the team mate
				if (m_Profile.m_Rep.GetClientRep(pClient) > BOT_MID_REP)
				{
					const int iRep = m_Profile.m_Rep.GetClientRep(pExtClient);

					// like the team killer less
					if (BotFunc_DecreaseRep(iRep))
					{
						m_Profile.m_Rep.DecreaseRep(pExtClient->GetPlayerRepId());
						//BotChat(BOT_CHAT_KILLED,pInfo);
					}
				}
			}
		}
		else
		{
			// cant see the enemy, remember it.
			RememberPosition(EntityOrigin(pExtInfo), pExtInfo, BOT_REMEMBER_LOST_ENEMY);
		}
	}
	break;
	case BOT_EVENT_KILL_SELF:
	{
		if (RANDOM_LONG(0, 100) < gBotGlobals.m_iBotChatPercent)
		{
			// I like a lot of people on the server so lets laugh
			if (m_Profile.m_Rep.AverageRepOnServer() >= BOT_MID_REP)
			{
				BotChat(BOT_CHAT_LAUGH);

				havingFun();
			}
			else // I don't think laughing would help my esteem... curse!
			{
				BotChat(BOT_CHAT_KILLED);

				gettingBored();
			}
		}
	}
	break;
	case BOT_EVENT_SEE_PLAYER_DIE:
	case BOT_EVENT_SEE_ENEMY_DIE:
	{
		if (!pClient || !pExtClient)
			return;

		const int iRep = m_Profile.m_Rep.GetClientRep(pExtClient);

		// dont like this player much
		if (m_Profile.m_Rep.GetClientRep(pClient) < BOT_MID_REP)
		{
			const int iRep = m_Profile.m_Rep.GetClientRep(pExtClient);

			if (BotFunc_IncreaseRep(iRep, BotFunc_DistanceBetweenEdicts(pInfo, pExtInfo), m_Profile.m_iSkill))
			{
				m_Profile.m_Rep.IncreaseRep(pExtClient->GetPlayerRepId());
				BotChat(BOT_CHAT_LAUGH, pInfo);

				// fun fun
				havingFun();
			}
		}
		// like this player a bit
		else if (m_Profile.m_Rep.GetClientRep(pClient) > BOT_MID_REP)
		{
			// decrease rep upon killer
			if (BotFunc_DecreaseRep(iRep, BotFunc_DistanceBetweenEdicts(pInfo, pExtInfo), m_Profile.m_iSkill))
			{
				m_Profile.m_Rep.DecreaseRep(pExtClient->GetPlayerRepId());
				BotChat(BOT_CHAT_LAUGH, pInfo);

				// ya-ya-yay much killage
				havingFun();
			}
		}
	}
	break;
	case BOT_EVENT_SEE_TEAMMATE_KILL:
		break;
	case BOT_EVENT_SEE_ENEMY_KILL:
	{
		// saw an enemy kill another player or one of my team
		// attack this guy

		if (IsEnemy(pInfo))
		{
			m_pEnemy = pInfo;
			AddPriorityTask(CBotTask(BOT_TASK_NORMAL_ATTACK, m_Tasks.GetNewScheduleId(), pInfo));
		}

		if (!pClient || !pExtClient)
			return;

		const int iRep = m_Profile.m_Rep.GetClientRep(pExtClient);

		if (m_Profile.m_Rep.GetClientRep(pClient) > BOT_MID_REP)
		{
			// enemy killed a player i liked
			if (BotFunc_DecreaseRep(iRep))
			{
				m_Profile.m_Rep.DecreaseRep(pExtClient->GetPlayerRepId());

				gettingBored();
			}
		}
	}
	break;
	case BOT_EVENT_KILL:

		// make bot taunt or something

		m_pKilledEdict = pInfo;

		if (RANDOM_LONG(0, 100) < gBotGlobals.m_iBotChatPercent)
		{
			BotChat(BOT_CHAT_KILLS, pInfo);

			havingFun();
		}
		else
		{
			if (m_fNextUseSayMessage > gpGlobals->time)
				return;

			m_fNextUseSayMessage = gpGlobals->time + RANDOM_FLOAT(10, 20);

			switch (gBotGlobals.m_iCurrentMod)
			{
			case MOD_BUMPERCARS:
				// taunt message
				pev->impulse = 108;
				break;
			case MOD_NS:
				if (IsAlien())
					pev->impulse = SAYING_1;
				else if (IsMarine())
					pev->impulse = SAYING_3;
				break;
			default: ;
			}
		}

		if (m_pKilledEdict && pClient && m_Profile.m_Rep.GetClientRep(pClient) < BOT_MID_REP)
		{
			const int iRep = m_Profile.m_Rep.GetClientRep(pClient);
			// killed a player i didn't like, are we even?

			// if i got him (by making him want to decrease his rep)
			if (BotFunc_IncreaseRep(iRep, DistanceFromEdict(m_pKilledEdict), m_Profile.m_iSkill))
			{
				m_Profile.m_Rep.IncreaseRep(pClient->GetPlayerRepId());
			}
		}
		break;
	case BOT_EVENT_DIED:

		m_pKillerEdict = pInfo;

		/*	if ( m_pElectricEnemy )
			{
				dec_attackElectrified->train(1.0f-(m_pElectricEnemy->v.health/m_pElectricEnemy->v.max_health));
			}*/

		gettingBored(); // argh why did i put this in

		if (gBotGlobals.IsNS() && IsGorge())
		{
			// if bot is a gorge, try ignoring part of the path ..
			// as it can dodge areas where it died next time.
			if (m_iCurrentWaypointIndex != -1 && m_iPrevWaypointIndex != -1)
			{
				PATH* pPath = BotNavigate_FindPathFromTo(m_iPrevWaypointIndex, m_iCurrentWaypointIndex, GetTeam());

				if (pPath)
					m_stFailedPaths.AddFailedPath(pPath);
			}
		}

		// got killed off someone
		if (m_pKillerEdict)
		{
			// chat to the guy
			if (RANDOM_LONG(0, 100) < gBotGlobals.m_iBotChatPercent)
				BotChat(BOT_CHAT_KILLED, pInfo);

			CClient* pKillerClient = gBotGlobals.m_Clients.GetClientByEdict(m_pKillerEdict);

			if (pKillerClient)
			{
				if (m_pKillerEdict->v.team != 0 && m_pKillerEdict->v.team == pev->team)
				{
					m_Profile.m_Rep.DecreaseRep(pKillerClient->GetPlayerRepId());
				}
				else
				{
					if (m_pKillerEdict != m_pEdict)
					{
						CBotReputation* pRep = m_Profile.m_Rep.GetRep(pKillerClient->GetPlayerRepId());

						if (pRep)
						{
							const int iRep = pRep->CurrentRep();

							if (BotFunc_DecreaseRep(iRep, DistanceFrom(EntityOrigin(m_pKillerEdict)), m_Profile.m_iSkill))
								m_Profile.m_Rep.DecreaseRep(pKillerClient->GetPlayerRepId());
						}
					}
				}
			}
		}

		break;
	case BOT_EVENT_HURT:

		if (m_fNextUpdateHealth < gpGlobals->time)
		{
			m_fNextUpdateHealth = gpGlobals->time + 1.0;

			const float fHealthLost = m_fPrevHealth - pev->health;

			std::vector<ga_value> inputs;

			// percentage of health now
			inputs.push_back(pev->health / pev->max_health);
			// percentage of health lost
			inputs.push_back(fHealthLost / pev->max_health);

			if (pFloatInfo)
			{
				// distance from
				inputs.push_back(DistanceFrom(Vector(pFloatInfo)));
			}
			else
				inputs.push_back(1);

			dec_runForCover->input(&inputs);
			dec_faceHurtOrigin->input(&inputs);

			std::vector<ga_value> weights;

			//
			// crap way of getting values from an individual !!!
			// (improved in rcbot 2 source!)

			// bias weight
			weights.push_back(m_GASurvival->get(4));
			// health weight
			weights.push_back(m_GASurvival->get(5));
			// health lost weight
			weights.push_back(m_GASurvival->get(6));
			// distance weight
			weights.push_back(m_GASurvival->get(7));

			dec_runForCover->setWeights(weights);

			weights.clear();
			// bias weight
			weights.push_back(m_GASurvival->get(8));
			// health weight
			weights.push_back(m_GASurvival->get(9));
			// health lost weight
			weights.push_back(m_GASurvival->get(10));
			// distance
			weights.push_back(m_GASurvival->get(11));

			dec_faceHurtOrigin->setWeights(weights);

			dec_runForCover->execute();
			dec_faceHurtOrigin->execute();

			m_fPrevHealth = pev->health;

			inputs.clear();
			weights.clear();
		}

		if (dec_runForCover->fired())
		{
			// getting pummelled so keep trying to run for cover somewhere!!
			RunForCover(Vector(pFloatInfo), TRUE);
		}
		// !!!dont break , drop down to the light damage stuff
		if (!dec_faceHurtOrigin->fired())
			break;
		//case BOT_EVENT_LIGHT_DAMAGE:

		if (pFloatInfo)
		{
			BOOL bAdd = FALSE;

			if (!bAdd)
			{
				if (!m_Tasks.HasTask(BOT_TASK_FACE_VECTOR))
				{
					// if no enemy
					if (!m_pEnemy)
						bAdd = TRUE;
					// if shooting a breakable...
					else if (strcmp("func_breakable", STRING(m_pEnemy->v.classname)) == 0)
						bAdd = TRUE;
				}
			}

			// face origin of damage
			if (bAdd)
				AddPriorityTask(CBotTask(BOT_TASK_FACE_VECTOR, 0, nullptr, 0, RANDOM_FLOAT(2.0, 3.0), Vector(pFloatInfo)));
		}

		if (gBotGlobals.IsNS() && IsGorge())
		{
			CBotWeapon* pBileBomb = m_Weapons.GetWeapon(NS_WEAPON_BILEBOMB);

			if (!pBileBomb->HasWeapon(m_pEdict))
			{
				// not got a good weapon to defend with..
				if (m_fChatMessageTime < gpGlobals->time)
				{
					BotChat(BOT_CHAT_HELP);
					// ask for help
					m_fChatMessageTime = gpGlobals->time + RANDOM_FLOAT(10.0, 12.0);
				}
			}
		}
		break;
	}
}

BOOL BotFunc_IncreaseRep(const int iRep, const float fInfo, const float fSkill)
// Basically More chance of increasing rep if current
// rep is good.
{
	if (iRep == -1)
		return FALSE;

	return !BotFunc_DecreaseRep(iRep, fInfo, fSkill);//( RANDOM_LONG(0,100) <= (iRep*10) );
}

BOOL BotFunc_DecreaseRep(const int iRep, float fInfo, const float fSkill)
// more chance of decreasing rep if current rep
// is bad.
// input the distance of enemy
{
	if (iRep == -1)
		return FALSE;

	const BOOL bUseDist = fInfo >= 0;

	if (bUseDist)
	{
		const float iSkill = fSkill / MAX_BOT_SKILL;

		fInfo /= 800.0;

		if (fInfo > 1.0)
			fInfo = 1.0;

		// more chance decresing rep if i have crap skill!!
		if (fInfo >= iSkill)
			return RANDOM_LONG(0, 100) >= iRep * 10;
	}
	else // more chance decreasing rep if it's already low
		return RANDOM_LONG(0, 100) >= iRep * 10;

	return FALSE;
}

void CBot::EnemyDown(edict_t* pEnemy)
{
	//RemoveCondition(BOT_CONDITION_SEE_ENEMY);

	m_vRememberedPositions.removePosition(pEnemy);
}

void CBot::EnemyFound(edict_t* pEnemy)
// called when a new enemy is found
{
	UpdateCondition(BOT_CONDITION_SEE_ENEMY);

	m_pEnemy = pEnemy;

	if (pEnemy == nullptr)
		return;

	m_fFirstSeeEnemyTime = gpGlobals->time;

	if (IsSquadLeader())
	{
		// back to normal
		m_stSquad->ChangeFormation(SQUAD_FORM_WEDGE);
		m_stSquad->SetCombatType(COMBAT_COMBAT);
	}

	RememberPosition(EntityOrigin(pEnemy), pEnemy, BOT_REMEMBER_1ST_SEE_ENEMY);

	m_fInvestigateSoundTime = gpGlobals->time + 2.0;

	if (strstr(STRING(pEnemy->v.classname), "osprey"))
		UpdateCondition(BOT_CONDITION_ENEMY_IS_OSPREY);
	else
		RemoveCondition(BOT_CONDITION_ENEMY_IS_OSPREY);

	if (!gBotGlobals.m_BotCam.BotHasEnemy())
	{
		gBotGlobals.m_BotCam.SetCurrentBot(this);
	}

	if (m_fNextUseSayMessage > gpGlobals->time)
		return;

	m_fNextUseSayMessage = gpGlobals->time + RANDOM_FLOAT(4.0, 8.0);

	// do something when an enemy is found
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_BUMPERCARS:
		// beep the horn!
		Impulse(109);
		break;
	case MOD_NS:
		if (IsMarine() && EntityIsAlien(pEnemy))
			Impulse(SAYING_7);
		else if (IsAlien() && EntityIsMarine(pEnemy))
			Impulse(SAYING_4);
		break;
	case MOD_TS:
		FakeClientCommand(m_pEdict, "usepowerup");

		// more chance of using special if good skilled
		if (m_pCurrentWeapon && m_pCurrentWeapon->GetID() != 36 && DistanceFrom(pEnemy->v.origin) > 200 && RANDOM_LONG(0, 100) < m_Profile.m_iSkill)
		{
			// this won't work anyway.. why was it put in ;P
			pev->button |= IN_ALT1;
		}

		break;
	default: ;
	}
}

BOOL CBot::HasSeenEnemy(edict_t* pEnemy) const
// returns if we have seen this enemy
{
	return m_pLastEnemy == pEnemy;
}

void CBot::ReplyToMessage(char* szMessage, edict_t* pSender, const int iTeamOnly)
// make a reply using megaHAL from a message by another player
{
	char m_TempMessage[BOT_CHAT_MESSAGE_LENGTH];

	if (m_Tasks.HasTask(BOT_TASK_TYPE_MESSAGE))
	{
		return;
	}

	m_TempMessage[0] = 0;

	BotChatReply(this, szMessage, pSender, m_TempMessage);

	// got a reply?
	if (m_TempMessage[0])
	{
		strcpy(m_szChatString, m_TempMessage);
		AddPriorityTask(CBotTask(BOT_TASK_TYPE_MESSAGE, 0, pSender, iTeamOnly));
	}
}

CLearnedHeader::CLearnedHeader(const int iId)
{
	strncpy(this->szBotVersion, BOT_VER, 31);
	szBotVersion[31] = 0;

	iDataVersion = LEARNED_FILE_VER;
	iProfileId = iId;
}

BOOL CLearnedHeader :: operator == (CLearnedHeader const other) const
{
	return !strcmp(szBotVersion, other.szBotVersion) &&
		iProfileId == other.iProfileId &&
		iDataVersion == other.iDataVersion;
}

void CBot::loadLearnedData() const
{
	char tmp_filename[64];
	char filename[256];

	sprintf(tmp_filename, "%d.rld", m_Profile.m_iProfileId);

	UTIL_BuildFileName(filename, BOT_PROFILES_FOLDER, tmp_filename);

	FILE* bfp = fopen(filename, "rb"); //Possible Memory Leak? [APG]RoboCop[CL]

	if (bfp == nullptr)
		return;

	CLearnedHeader header = CLearnedHeader(m_Profile.m_iProfileId);
	const CLearnedHeader checkheader = CLearnedHeader(m_Profile.m_iProfileId);

	fread(&header, sizeof(CLearnedHeader), 1, bfp);

	if (checkheader != header)
	{
		BotMessage(nullptr, 0, "Bots learned data for %s (profile %d) header mismatch", tmp_filename, m_Profile.m_iProfileId);
		return;
	}

	if (feof(bfp))
	{
		fclose(bfp);
		return;
	}
	// add new to the end plz!!!
	if (m_GASurvival != nullptr)
		m_GASurvival->load(bfp, 16);
	if (dec_attackElectrified != nullptr)
		dec_attackElectrified->load(bfp);
	if (dec_flapWings != nullptr)
		dec_flapWings->load(bfp);
	if (dec_runForCover != nullptr)
		dec_runForCover->load(bfp);
	if (dec_faceHurtOrigin != nullptr)
		dec_faceHurtOrigin->load(bfp);
	if (dec_jump != nullptr)
		dec_jump->load(bfp);
	if (dec_duck != nullptr)
		dec_duck->load(bfp);
	if (dec_strafe != nullptr)
		dec_strafe->load(bfp);
	if (m_pPersonalGAVals != nullptr)
		m_pPersonalGAVals->load(bfp, 22);
	if (m_personalGA != nullptr)
		m_personalGA->load(bfp, 22);
	if (m_pFlyGA != nullptr)
		m_pFlyGA->load(bfp, 6);
	if (m_pFlyGAVals != nullptr)
		m_pFlyGAVals->load(bfp, 6);
	if (dec_followEnemy != nullptr)
		dec_followEnemy->load(bfp);
	if (m_pTSWeaponSelect != nullptr)
		m_pTSWeaponSelect->load(bfp, 8/*+MAX_WEAPONS*/);
	if (dec_stunt != nullptr)
		dec_stunt->load(bfp);
	if (m_pCombatBits != nullptr)
		m_pCombatBits->load(bfp, 1);

	//if ( dec_firepercent != NULL )
	//	dec_firepercent->load(bfp);

	fclose(bfp);
}

void CBot::saveLearnedData() const
{
	char tmp_filename[64];
	char filename[256];

	sprintf(tmp_filename, "%d.rld", m_Profile.m_iProfileId);

	UTIL_BuildFileName(filename, BOT_PROFILES_FOLDER, tmp_filename);

	FILE* bfp = fopen(filename, "wb");

	if (bfp == nullptr)
		return;

	CLearnedHeader header = CLearnedHeader(m_Profile.m_iProfileId);

	fwrite(&header, sizeof(CLearnedHeader), 1, bfp);

	// MUST BE IN SAME ORDER AS LOADING!
	if (m_GASurvival != nullptr)
		m_GASurvival->save(bfp);
	if (dec_attackElectrified != nullptr)
		dec_attackElectrified->save(bfp);
	if (dec_flapWings != nullptr)
		dec_flapWings->save(bfp);
	if (dec_runForCover != nullptr)
		dec_runForCover->save(bfp);
	if (dec_faceHurtOrigin != nullptr)
		dec_faceHurtOrigin->save(bfp);
	if (dec_jump != nullptr)
		dec_jump->save(bfp);
	if (dec_duck != nullptr)
		dec_duck->save(bfp);
	if (dec_strafe != nullptr)
		dec_strafe->save(bfp);
	if (m_pPersonalGAVals != nullptr)
		m_pPersonalGAVals->save(bfp);
	if (m_personalGA != nullptr)
		m_personalGA->save(bfp);
	if (m_pFlyGA != nullptr)
		m_pFlyGA->save(bfp);
	if (m_pFlyGAVals != nullptr)
		m_pFlyGAVals->save(bfp);
	if (dec_followEnemy != nullptr)
		dec_followEnemy->save(bfp);
	if (m_pTSWeaponSelect != nullptr)
		m_pTSWeaponSelect->save(bfp);
	if (dec_stunt != nullptr)
		dec_stunt->save(bfp);
	if (m_pCombatBits != nullptr)
		m_pCombatBits->save(bfp);
	//	if ( dec_firepercent != NULL )
		//	dec_firepercent->save(bfp);

	fclose(bfp);
}

void CBot::FreeLocalMemory(void)
{
	// Free all stacks/queues...
	/*
	if ( m_pAimingGA != NULL )
	{
		m_pAimingGA->freeGlobalMemory();
		delete m_pAimingGA;
		m_pAimingGA;
	}
	if ( m_pAimingGAVals != NULL )
	{
		delete m_pAimingGAVals;
		m_pAimingGAVals = NULL;
	}

	if ( m_pAiming != NULL )
	{
		delete m_pAiming;
		m_pAiming = NULL;
	}
	//*/

	if (m_GASurvival != nullptr)
	{
		m_GASurvival->freeMemory();
		delete m_GASurvival;
		m_GASurvival = nullptr;
	}

	if (m_pCombatBits)
	{
		delete m_pCombatBits;
		m_pCombatBits = nullptr;
	}

	if (m_pVisibles != nullptr)
	{
		m_pVisibles->freeMemory();
		delete m_pVisibles;
		m_pVisibles = nullptr;
	}

	if (dec_attackElectrified != nullptr)
	{
		delete dec_attackElectrified;
		dec_attackElectrified = nullptr;
	}
	if (dec_flapWings != nullptr)
	{
		delete dec_flapWings;
		dec_flapWings = nullptr;
	}
	if (dec_runForCover != nullptr)
	{
		delete dec_runForCover;
		dec_runForCover = nullptr;
	}
	if (dec_faceHurtOrigin != nullptr)
	{
		delete dec_faceHurtOrigin;
		dec_faceHurtOrigin = nullptr;
	}
	if (dec_jump != nullptr)
	{
		delete dec_jump;
		dec_jump = nullptr;
	}
	if (dec_duck != nullptr)
	{
		delete dec_duck;
		dec_duck = nullptr;
	}
	if (dec_strafe != nullptr)
	{
		delete dec_strafe;
		dec_strafe = nullptr;
	}

	if (m_pPersonalGAVals != nullptr)
	{
		delete m_pPersonalGAVals;
		m_pPersonalGAVals = nullptr;
	}

	if (m_personalGA != nullptr)
	{
		m_personalGA->freeGlobalMemory();
		delete m_personalGA;
		m_personalGA = nullptr;
	}

	if (m_pFlyGA != nullptr)
	{
		m_pFlyGA->freeGlobalMemory();
		delete m_pFlyGA;
		m_pFlyGA = nullptr;
	}

	if (m_pFlyGAVals != nullptr)
	{
		delete m_pFlyGAVals;
		m_pFlyGAVals = nullptr;
	}

	if (dec_followEnemy != nullptr)
	{
		delete dec_followEnemy;
		dec_followEnemy = nullptr;
	}

	if (m_pTSWeaponSelect != nullptr)
	{
		delete m_pTSWeaponSelect;
		m_pTSWeaponSelect = nullptr;
	}

	if (dec_stunt != nullptr)
	{
		delete dec_stunt;
		dec_stunt = nullptr;
	}

	/*if ( dec_firepercent != NULL )
	{
		delete dec_firepercent;
		dec_firepercent = NULL;
	}*/

	m_stBotPaths.Destroy();
	//m_stBotVisibles.Destroy();
	m_stFailedPaths.ClearFailedPaths();

	m_Tasks.FlushTasks();

	m_Profile.m_Rep.Destroy();

	sOpenList.Clear();
	//sOpenList.Destroy();

	m_FailedGoals.Destroy();

	if (m_Profile.m_HAL)
	{
		FreeHALBrain(&m_Profile);

		//		if (m_Profile.m_HAL != NULL)
		free(m_Profile.m_HAL);

		m_Profile.m_HAL = nullptr;
	}
}

BOOL CBot::WantToLeaveGame(void) const
{
	/*

			Cheeseh's leave the game formula!

			  leave game prob = (255-Boredom)*(m_fJoinGameTime)

		*/
	const float fTimelimit = CVAR_GET_FLOAT("mp_timelimit");
	const float fBoredompercent = (float)(255 - (int)m_iBoredom) / 255;
	const float fLongGameTime = 1.0 - (fTimelimit * 60 - (gpGlobals->time - m_fJoinServerTime)) / (fTimelimit * 60);

	//long game time = 0 to 1 number, 0 meaning 0% of time spent on server,
	//1 means 100% of time spent on server
	return fBoredompercent * fLongGameTime > RANDOM_FLOAT(0.75, 1.0); // 75% or more of wanting to leave
}

void CBot::Init(void)
// Initialise everything required.
{
	memset(this, 0, sizeof(CBot));

	m_iBoredom = 127;

	m_bCommInit = TRUE;

	//m_MegaHALTalkEdict = NULL;

	//m_bSelectedCar = FALSE;

	//m_Profile.m_HAL = NULL;

	//m_iNextWaypoint = -1;

	//	m_GoalTasks.Destroy();

	//m_WptObjectiveTask = CBotTask(BOT_TASK_NONE);
	sOpenList.Clear();

	//sOpenList.Init();

	m_fCombatFitness = 0;
	m_fTurnSpeed = gBotGlobals.m_fTurnSpeed;//BOT_DEFAULT_TURN_SPEED;
	m_fMaxSpeed = 290;

	m_iWaypointGoalIndex = -1;
	m_iCurrentWaypointIndex = -1;

	m_fPrevWaypointDist = 4096;

	m_szBotName = nullptr;

	m_Profile.m_Rep.Init();

	m_Profile.m_iNumGames = 0;
	m_Profile.m_iFavMod = 1;   // any mod
	m_Profile.m_iFavTeam = 5;  // any team
	m_Profile.m_szFavMap = nullptr; // no map
	m_Profile.m_iSkill = DEF_BOT_SKILL; // mid skill
	m_Profile.m_szSpray = nullptr; // no spray
	m_Profile.m_szBotName = BOT_DEFAULT_NAME; // rcbot default name (PREFERRED NAME)

	m_Profile.m_iProfileId = 0;

	////////////////////////////////
	// NS STUFF

	m_Profile.m_GorgePercent = 50;
	m_Profile.m_LerkPercent = 50;
	m_Profile.m_FadePercent = 50;
	m_Profile.m_OnosPercent = 50;

	m_iTeam = TEAM_AUTOTEAM;

	m_fReactionTime = BOT_DEFAULT_REACTION_TIME;
	m_fThinkTime = BOT_DEFAULT_THINK_TIME;

	m_bIsUsed = FALSE;

	m_iRespawnState = RESPAWN_NEED_TO_RESPAWN;

	m_CurrentTask = nullptr;

	m_bHasOrder = FALSE;

	m_fSecDelTime = BOT_DEF_MSECDELTIME;

	m_FailedGoals.Destroy();
	//m_iLastFailedGoalWaypoint = -1;

	m_pVisibles = nullptr;
	dec_flapWings = nullptr;
	m_GASurvival = nullptr;

	m_pCombatBits = nullptr;
	dec_runForCover = nullptr;
	dec_faceHurtOrigin = nullptr;
	dec_followEnemy = nullptr;
	dec_attackElectrified = nullptr;
	dec_jump = nullptr;
	dec_duck = nullptr;
	dec_stunt = nullptr;
	dec_strafe = nullptr;
	m_pPersonalGAVals = nullptr;
	m_personalGA = nullptr;
	m_pFlyGAVals = nullptr;
	m_pFlyGA = nullptr;
	m_pTSWeaponSelect = nullptr;

	SpawnInit(TRUE);
}

void CBot::setupDataStructures()
{
	/*
	if ( m_pAimingGA == NULL )
		m_pAimingGA = new CGA(10);
	if ( m_pAiming == NULL )
		m_pAiming = new NN(3,3,3,2);
	if ( m_pAimingGAVals == NULL )
	{
		vector <ga_value> weights;
		m_pAimingGAVals = new CBotGAValues();
		m_pAiming->getWeights(&weights);
		m_pAimingGAVals->setVector(weights);
	}
	//*/

	if (m_pVisibles == nullptr)
		m_pVisibles = new CBotVisibles();

	if (dec_flapWings == nullptr)
	{
		dec_flapWings = new CPerceptron(4);
		//	dec_flapWings->load("fwp",m_Profile.m_iProfileId);
	}
	// energy & +ve/-ve height & onground & zvelocity

	if (m_GASurvival == nullptr)
	{
		m_GASurvival = new CBotGAValues();
		//	m_GASurvival->loadForBot("gas",m_Profile.m_iProfileId);
	}

	if (!m_pCombatBits)
	{
		m_pCombatBits = new CIntGAValues(0);
	}

	//if ( dec_firepercent == NULL )
//		dec_firepercent = new CPerceptron(MAX_WEAPONS);

	if (dec_runForCover == nullptr)
	{
		dec_runForCover = new CPerceptron(3);
		//	dec_runForCover->load("rfcp",m_Profile.m_iProfileId);
	}

	if (dec_faceHurtOrigin == nullptr)
	{
		dec_faceHurtOrigin = new CPerceptron(3);
		//	dec_faceHurtOrigin->load("fhop",m_Profile.m_iProfileId);
	}

	if (dec_followEnemy == nullptr)
	{
		dec_followEnemy = new CPerceptron(3);
		//	dec_followEnemy->load("fep",m_Profile.m_iProfileId);
	}

	if (dec_attackElectrified == nullptr)
	{
		dec_attackElectrified = new CPerceptron(7);
		//	dec_attackElectrified->load("aep",m_Profile.m_iProfileId);
	}

	if (dec_jump == nullptr)
	{
		dec_jump = new CPerceptron(4);
		//	dec_jump->load("jp",m_Profile.m_iProfileId);
	}

	if (dec_duck == nullptr)
	{
		dec_duck = new CPerceptron(4);
		//dec_duck->load("dp",m_Profile.m_iProfileId);
	}

	if (dec_stunt == nullptr)
	{
		dec_stunt = new CPerceptron(5);
	}

	if (dec_strafe == nullptr)
	{
		dec_strafe = new CPerceptron(4);
		//dec_strafe->load("strp",m_Profile.m_iProfileId);
	}

	if (m_pPersonalGAVals == nullptr)
	{
		m_pPersonalGAVals = new CBotGAValues();
		//m_pPersonalGAVals->loadForBot("pga",m_Profile.m_iProfileId);
	}
	//if ( dec_feign == NULL )
	//	dec_feign = new CPerceptron(3);
	if (m_personalGA == nullptr)
	{
		m_personalGA = new CGA(5);
		//m_personalGA->loadBotGA("perga",m_Profile.m_iProfileId);
	}

	if (m_pFlyGAVals == nullptr)
	{
		m_pFlyGAVals = new CBotGAValues();
	}

	m_pFlyGAVals->clear();
	// custom : lerk hold & flap time
	m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 0
	m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 1

	m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 2
	m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 3
	m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 4
	m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 5

	//m_pFlyGAVals->loadForBot("fgaval",m_Profile.m_iProfileId);

	if (m_pFlyGA == nullptr)
	{
		m_pFlyGA = new CGA(6);
		//m_pFlyGA->loadBotGA("flyga",m_Profile.m_iProfileId);
	}

	if (m_pTSWeaponSelect == nullptr)
	{
		m_pTSWeaponSelect = new CBotGAValues();
	}

	m_pTSWeaponSelect->clear();
	m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1)); //0
	m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1));
	m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1));

	m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 3
	m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1));
	m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1));
	m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1));
	m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1));

	//for ( int i = 0; i < MAX_WEAPONS; i ++ )
		//	m_pTSWeaponSelect->add(RANDOM_FLOAT(0,1)); //7+37 = 44 (45)// fire percent

	/*	if ( m_pAiming == NULL )
	{
		m_pAiming = new NNGATrained (3, 6, 3, 3 );

		vector<CNNTrainSet>
	}*/

	loadLearnedData();
}

void CBot::SpawnInit(const BOOL bInit)
{
	m_fLastPlaceDetpack = 0;
	m_fNextShootButton = 0;
	m_bNearestRememberPointVisible = FALSE;
	m_bZoom = FALSE;
	//m_bSelectedGun = 0;
	RemoveCondition(BOT_CONDITION_SELECTED_GUN);
	m_fPrevFlyHeight = 0;
	m_pTank = nullptr;
	m_pElectricEnemy = nullptr;
	m_bUsedMelee = FALSE;

	m_bPlacedPipes = FALSE;
	m_pSpySpotted = nullptr;
	m_bIsDisguised = FALSE;
	m_fGrenadePrimeTime = 0;
	m_iGrenadeHolding = 0;

	m_bAcceptHealth = FALSE;

	m_fNextUseScientist = 0;
	m_fNextUseBarney = 0;

	m_fNextGetEnemyTime = 0;

	m_vAvoidMoveTo = Vector(0, 0, 0);
	m_fAvoidTime = 0;

	m_fUpgradeTime = 0;

	m_fHoldAttackTime = 0;
	m_iGear = 0;
	m_bNotFollowingWaypoint = FALSE;
	m_fLastNoMove = 0.0;
	m_iVisUpdateRevs = 0.0;
	m_fUpTime = 0.0;
	m_fAttemptEvolveTime = 0;
	m_fLookForBuildableTime = 0;
	//	m_iLastCurrentWaypoint = -1;
	m_fNextBuildTime = 0;
	m_fNextUseVGUI = 0;
	m_bSaidInPosition = FALSE;
	m_bSaidMoveOut = FALSE;
	m_fNextClearFailedGoals = 0.0;

	// bInit is true upon constructors (DONT DESTROY ANYTHING!)
	if (!bInit)
	{
		m_bHasAskedForOrder = FALSE;
		m_fRespawnTime = 0;
		CBotTask TypeMessageTask;

		if (!m_Tasks.NoTasksLeft())
			m_Tasks.GetTask(BOT_TASK_TYPE_MESSAGE, &TypeMessageTask);

		m_Tasks.FlushTasks();

		if (!HasCondition(BOT_CONDITION_DONT_CLEAR_OBJECTIVES))
		{
			m_TSObjectives.Clear();
			/*
			while ( !m_TSObjectives.empty() )
			{
				//	delete m_TSObjectives.top();
				m_TSObjectives.pop();
			}*/
		}

		clearUsedTeleporters();

		m_fChatMessageTime = gpGlobals->time + RANDOM_LONG(10, 20);

		if (gBotGlobals.IsMod(MOD_BG))
		{
			// possible max speed fix.
			if (pev && pev->maxspeed > m_fMaxSpeed)
				m_fMaxSpeed = pev->maxspeed;
		}
		else if (gBotGlobals.IsNS()) // quicker check
		{
			if (m_OrderTask.Task() != BOT_TASK_NONE)
			{
				AddPriorityTask(m_OrderTask);
			}
		}

		if (TypeMessageTask.Task() != BOT_TASK_NONE)
			AddPriorityTask(TypeMessageTask);

		//		m_iNextWaypoint = -1;
		m_stBotPaths.Destroy();     // the bot will be at a different
		// position most likely after spawning so clear its paths
		//m_stBotVisibles.Destroy();  // free the list of visible entities
	//	m_stBotVisibles.Destroy();

		sOpenList.Clear();
		//sOpenList.Destroy();

		int team = GetTeam();

		if (team < 0 || team >= MAX_TEAMS)
			team = 0;

		if (m_personalGA->canPick())
		{
			IIndividual* ind = m_personalGA->pick();

			m_pPersonalGAVals->freeMemory();
			delete m_pPersonalGAVals;

			m_pPersonalGAVals = static_cast<CBotGAValues*>(ind);
			m_pPersonalGAVals->set_fitness(0);
		}
		else
		{
			m_pPersonalGAVals->clear();
			// weights

			// jump
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 0
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 1
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 2
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 3
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 4

			// duck
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 5
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 6
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 7
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 8
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 9

			// strafe
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 10
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 11
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 12
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 13
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 14

			// electric enemy
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 15
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 16
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 17
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 18
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 19
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 20
			m_pPersonalGAVals->add(RANDOM_FLOAT(-1, 1)); // 21
		}

		dec_attackElectrified->setWeights(m_pPersonalGAVals, 15, 7);

		if (gBotGlobals.m_enemyCostGAsForTeam[team].canPick())
		{
			IIndividual* costs = gBotGlobals.m_enemyCostGAsForTeam[team].pick();

			m_GASurvival->freeMemory();
			delete m_GASurvival;

			m_GASurvival = static_cast<CBotGAValues*>(costs);
			m_GASurvival->set_fitness(0);
		}
		else
		{
			m_GASurvival->clear();
			// friendly cost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 0
			// enemy cost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 1
			// inanimate cost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 2
			// grenade cost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 3

			// run cover bias weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 4
			// health lost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 5
			// health now
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 6
			// distance weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 7

			// face hurt origin bias weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 8
			// health lost
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 9
			// health now
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 10
			// distance weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 11

			// follow enemy bias weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 12
			// follow enemy distance
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 13
			// health now weight
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 14
			// enemy size
			m_GASurvival->add(RANDOM_FLOAT(-1, 1)); // 15
		}
	}

	m_iBotWeapons = 0;
	m_pCurrentWeapon = nullptr;

	m_fNextCheckCover = 0;

	m_pLastEnemy = nullptr;

	m_bHasLongJump = FALSE;

	//m_pNearestAmmoDisp = NULL;

	m_iLastFailedWaypoint = -1;

	//m_fMaxDangerFactor = 0;
	//m_fMinDangerFactor = 0;

	m_fListenToSoundTime = 0;

	//m_bGotEnemyPath = FALSE;

	m_fLastCallRunPlayerMove = 0;

	m_pPickupEntity = nullptr;

	m_fLastUpdateConditions = 0;

	m_CurrentTask = nullptr;

	m_fLastThinkTime = 0;
	m_fUpdateWaypointTime = 0;

	//    m_pSound = NULL;
	m_bKill = FALSE;
	//    m_bShotFailed = FALSE;
	m_bCanUseAmmoDispenser = TRUE;

	//  m_bCurrentLookDirIsValid = FALSE;
	m_fLastLookTime = 0;

	m_bMoveToIsValid = FALSE;       // Is FALSE When bot has no waypoint.

	/////////////////////////////
	// NAVIGATION RELATED

	m_iWaypointGoalIndex = -1;
	//	m_iPrevWaypointGoalIndex = -1;
	m_iCurrentWaypointIndex = -1;
	m_iPrevWaypointIndex = -1;
	m_iCurrentWaypointFlags = 0;
	m_fPrevWaypointDist = 4096.0;
	m_fLastSeeWaypoint = 0;

	m_pAvoidEntity = nullptr;

	m_fStuckTime = 0;
	m_fMoveSpeed = 0;
	m_fUpSpeed = 0;
	m_fStrafeSpeed = 0;

	m_fStrafeTime = 0;

	m_fStartJumpTime = 0;
	m_fEndJumpTime = 0;
	m_fStartDuckTime = 0;
	m_fEndDuckTime = 0;

	m_fFindPathTime = 0;

	m_fWallAtLeftTime = 0;
	m_fWallAtRightTime = 0;

	m_fUpdateVisiblesTime = 0;

	////////////////////////////
	// NATURAL SELECTION RELATED

	// Bot died, lost upgrades
	//	m_siLastGestationState = 0;
	//	m_siCurrentUpgrades = 0;

	///////////////////////////
	// TASK RELATED

	// Keep unfinished tasks untouched for now

	////////////////////////////
	// ENEMY RELATED

	m_pEnemy = nullptr;

	m_fFirstSeeEnemyTime = 0;
	m_fGetAimVectorTime = 0;

	m_pEnemyRep = nullptr;

	////////////////////////////
	// MISCELLANEOUS

	m_bNeedToInit = TRUE;

	//	m_siLadderDir = LADDER_UNKNOWN;

	if (HasCondition(BOT_CONDITION_DONT_CLEAR_OBJECTIVES))
		m_ibBotConditions = BOT_CONDITION_DONT_CLEAR_OBJECTIVES;
	else
		m_ibBotConditions = 0;

	//m_Tasks.FlushTasks();
	m_fFindPathTime = 0;

	m_bNeedToInit = FALSE;

	Vector m_vCurrentLookDir = Vector(0, 0, 0);
	//    m_bCurrentLookDirIsValid = FALSE;

	m_vMoveToVector = Vector(0, 0, 0);
	m_bMoveToIsValid = FALSE;

	m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
	m_fLastLookTime = 0.0;

	m_fGetAimVectorTime = 0.0;

	m_fNextUseSayMessage = 0;

	m_fIdealMoveSpeed = m_fMaxSpeed;
}

int CBot::GetTeam(void) const
{
	return UTIL_GetTeam(m_pEdict);
}

void CBot::BotChat(const eBotChatType iChatType, edict_t* pChatEdict, const BOOL bSayNow)
// pChatEdict will be NULL if not directly talking to someone
{
	CClient* pClient = nullptr;
	int iTeamOnly = 0;

	if (!FBitSet(gBotGlobals.m_iConfigSettings, BOT_CONFIG_CHATTING))
		return;

	if (m_Tasks.HasTask(BOT_TASK_TYPE_MESSAGE))
		return; // already trying to talk to someone...

	dataUnconstArray <char*>* pChatArray = nullptr;

	if (pChatEdict)
	{
		if (pChatEdict->v.flags & FL_CLIENT)
		{
			pClient = gBotGlobals.m_Clients.GetClientByEdict(pChatEdict);
		}

		if (pClient == nullptr)
		{
			BugMessage(nullptr, "Caution : BotChat() Can't find pClient to talk to (Probably a bot fell/drowned/died from worldspawn)");
		}
	}

	// lots of repetitive coding here, but I'd like bots to chat as much as possible ;)
	// i.e. even if its not chatting to a client as long as the message it wants to say
	// is kind of generic.

	int iArrayNum = 1;

	if (pClient == nullptr)
		iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.AverageRepOnServer());
	else if (pChatEdict->v.flags & FL_CLIENT)
	{
		iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
	}

	switch (iChatType)
	{
	case BOT_CHAT_LEAVEGAME:
	{
		pChatArray = &gBotGlobals.m_BotChat.m_LeaveGame[iArrayNum];
	}
	break;
	case BOT_CHAT_KILLS:
		//if ( pClient == NULL )
		//	pChatArray = &gBotGlobals.m_BotChat.m_Kills[1];
		//else if ( pChatEdict->v.flags & FL_CLIENT )
	{
		//	iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
		pChatArray = &gBotGlobals.m_BotChat.m_Kills[iArrayNum];
	}
	break;
	case BOT_CHAT_KILLED:
		//if ( pClient == NULL )
		//	pChatArray = &gBotGlobals.m_BotChat.m_Killed[1];
		//else if ( pChatEdict->v.flags & FL_CLIENT )
	{
		//	iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
		pChatArray = &gBotGlobals.m_BotChat.m_Killed[iArrayNum];
	}
	break;
	case BOT_CHAT_IDLE:
		pChatArray = &gBotGlobals.m_BotChat.m_Idle;
		break;
	case BOT_CHAT_THANKS:
		//if ( pClient == NULL )
		//	pChatArray = &gBotGlobals.m_BotChat.m_Thanks[1];
		//else if ( pChatEdict->v.flags & FL_CLIENT )
	{
		//	iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
		pChatArray = &gBotGlobals.m_BotChat.m_Thanks[iArrayNum];
	}
	break;
	case BOT_CHAT_GREETINGS:
		//if ( pClient == NULL )
		//	pChatArray = &gBotGlobals.m_BotChat.m_Greetings[1];
		//else if ( pChatEdict->v.flags & FL_CLIENT )

	{
		const int iIndex = ENTINDEX(pChatEdict) - 1;

		if (!(m_iSaidGreetingsTo & 1 << iIndex))
		{
			m_iSaidGreetingsTo |= 1 << iIndex;

			iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
			pChatArray = &gBotGlobals.m_BotChat.m_Greetings[iArrayNum];
		}
		else
			return; // Already greeted this player!
	}
	break;
	case BOT_CHAT_LAUGH:
		//if ( pClient == NULL )
		//	pChatArray = &gBotGlobals.m_BotChat.m_Laugh[1];
		//else if ( pChatEdict->v.flags & FL_CLIENT )
	{
		//	iArrayNum = BotFunc_GetRepArrayNum(m_Profile.m_Rep.GetClientRep(pClient));
		pChatArray = &gBotGlobals.m_BotChat.m_Laugh[iArrayNum];
	}
	break;
	case BOT_CHAT_HELP:
		iTeamOnly = 1;
		pChatArray = &gBotGlobals.m_BotChat.m_Help;
		break;
	default:
		return;
	}

	if (pChatArray == nullptr)
		return;

	if (pChatArray->IsEmpty())
		return;

	// ahhh!
	assert(pChatArray->Size() > 0);

	char* pChatStr = pChatArray->Random();

	if (pChatStr == nullptr)
		return;

	strncpy(m_szChatString, pChatStr, BOT_CHAT_MESSAGE_LENGTH - 1);
	m_szChatString[BOT_CHAT_MESSAGE_LENGTH - 1] = 0;

	// fill in talking player
	if (strstr(m_szChatString, "%n"))
	{
		if (pChatEdict == nullptr)
			return;
		if (!(pChatEdict->v.flags & FL_CLIENT))
			return;
		char szName[80];

		RemoveNameTags(STRING(pChatEdict->v.netname), szName);

		if (!BotFunc_FillString(m_szChatString, "%n", szName, BOT_CHAT_MESSAGE_LENGTH))
			return;
	}

	// fill in a random player name
	if (strstr(m_szChatString, "%r"))
	{
		const char* szPlayerName;

		if ((szPlayerName = BotFunc_GetRandomPlayerName(this, 0)) == nullptr)
		{
			return;
		}

		char szName[80];

		RemoveNameTags(szPlayerName, szName);

		if (!BotFunc_FillString(m_szChatString, "%r", szName, BOT_CHAT_MESSAGE_LENGTH))
			return;
	}

	// fill in a player name I like
	if (strstr(m_szChatString, "%g"))
	{
		const char* szPlayerName;

		if ((szPlayerName = BotFunc_GetRandomPlayerName(this, 1)) == nullptr)
		{
			return;
		}

		char szName[80];

		RemoveNameTags(szPlayerName, szName);

		if (!BotFunc_FillString(m_szChatString, "%g", szName, BOT_CHAT_MESSAGE_LENGTH))
			return;
	}

	// fill in a player name I don't like
	if (strstr(m_szChatString, "%b"))
	{
		const char* szPlayerName;

		if ((szPlayerName = BotFunc_GetRandomPlayerName(this, -1)) == nullptr)
		{
			return;
		}

		char szName[80];

		RemoveNameTags(szPlayerName, szName);

		if (!BotFunc_FillString(m_szChatString, "%b", szName, BOT_CHAT_MESSAGE_LENGTH))
			return;
	}

	// fill in a random enemy's name
	if (strstr(m_szChatString, "%e"))
	{
		const char* szPlayerName;

		if ((szPlayerName = BotFunc_GetRandomPlayerName(this, 2)) == nullptr)
		{
			return;
		}

		char szName[80];

		RemoveNameTags(szPlayerName, szName);

		if (!BotFunc_FillString(m_szChatString, "%e", szName, BOT_CHAT_MESSAGE_LENGTH))
			return;
	}

	// fill in the map name
	if (!BotFunc_FillString(m_szChatString, "%m", STRING(gpGlobals->mapname), BOT_CHAT_MESSAGE_LENGTH))
		return;

	if (m_szChatString[0])
	{
		// shouldn't happen anyway after adding max len to FillString
		/*if ( strlen(m_szChatString) > (BOT_CHAT_MESSAGE_LENGTH-1) )
		{
			m_szChatString[BOT_CHAT_MESSAGE_LENGTH-1] = 0;

			BotMessage(NULL,1,"\nERROR: Memory Overflow upon filling bot chat message \"%s\"\n[%s] Chat line may be too long on some circumstances",m_szChatString,BOT_TAG);
			BotMessage(NULL,1,"\nGame has ceased, look for the message in the %s file and decrease the length of that line",BOT_CHAT_FILE);
			return;
		}*/
		if (bSayNow)
			AddPriorityTask(CBotTask(BOT_TASK_TYPE_MESSAGE, 0, nullptr, iTeamOnly));
		else
			AddTask(CBotTask(BOT_TASK_TYPE_MESSAGE, 0, nullptr, iTeamOnly));
	}
}

const char* BotFunc_GetRandomPlayerName(CBot* pBot, const int iState)
// Get a random playername, depending on iState...
	// iState 0 will return any random player name
	// iState -1 will return a random bad rep player name
	// iState 1 will return a random good rep player name
	// iState 2 will return a random ENEMY player name
{
	dataUnconstArray<const char*> arrayNames;

	edict_t* pEdict = pBot->m_pEdict;

	CClient* pClient;

	int iGotRep;

	arrayNames.Init();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (FNullEnt(pPlayer))
			continue;
		if (!*STRING(pPlayer->v.netname))
			continue;
		if (pPlayer == pEdict)
			continue;
		switch (iState)
		{
		case -1:
			// Clients stored as ENTINDEX() of Players -1 so
			// this should work to get the correct CClient*
			pClient = gBotGlobals.m_Clients.GetClientByIndex(i - 1);

			if ((iGotRep = pBot->m_Profile.m_Rep.GetClientRep(pClient)) != -1)
			{
				if (iGotRep < 3)
					arrayNames.Add(STRING(pPlayer->v.netname));
			}
			break;
		case 0:
			arrayNames.Add(STRING(pPlayer->v.netname));
			break;
		case 1:
			pClient = gBotGlobals.m_Clients.GetClientByIndex(i - 1);

			if ((iGotRep = pBot->m_Profile.m_Rep.GetClientRep(pClient)) != -1)
			{
				if (iGotRep > 6)
					arrayNames.Add(STRING(pPlayer->v.netname));
			}
			break;
		case 2:
			if (pBot->IsEnemy(pPlayer))
				arrayNames.Add(STRING(pPlayer->v.netname));
			break;
		default: ;
		}
	}

	// no names left to use!?
	if (!arrayNames.IsEmpty())
	{
		assert(arrayNames.Size() > 0);

		const char* pName = arrayNames.Random();

		arrayNames.Clear();

		return pName;
	}

	return nullptr;
}

// Fill string, a neat function I made
// fills the points of a string with %l, %r, %whatever with readable text
BOOL BotFunc_FillString(char* string, const char* fill_point, const char* fill_with, const int max_len)
{
	// keep a big string to make sure everything fits
	static char temp[1024];
	int len = strlen(string) + 1;

	if (fill_with == nullptr)
		return FALSE;

	char* ptr = string;

	// Keep searching for a point in the string
	while ((ptr = strstr(ptr, fill_point)) != nullptr)
	{
		// found a point
		char* before = static_cast<char*>(malloc(sizeof(char) * len));
		char* after = static_cast<char*>(malloc(sizeof(char) * len));

		// initialize them to empty (don't need to null terminate at the right point)
		memset(before, 0, len);
		memset(after, 0, len);

		const int start = (int)ptr - (int)string;
		strncpy(before, string, start);

		// always null terminate the last possible character
		string[start] = 0;

		// 'strlen' function called too many times inside loop [APG]RoboCop[CL]
		const int end = start + strlen(fill_point);
		strncpy(after, &string[end], len - end);
		after[len - end] = 0;

		// fill in new string and..
		// update len (string size may have INCREASED
		// if by more than 2* normal string size, could have overwritten
		// some precious memory!)
		len = sprintf(temp, "%s%s%s", before, fill_with, after);

		// fill string now with maximum length for string
		strncpy(string, temp, max_len - 1);
		string[max_len - 1] = 0;

		// MUST reset ptr incase new ptr is outside the string (if
		// string size was reduced)
		ptr = string;

		// free memory used
//		if (before != NULL)
		free(before);

		before = nullptr;

		//		if (after != NULL)
		free(after);

		after = nullptr;
		/*

				int len = strlen(ptr+2);

				temp = NULL;

				if ( len > 0 )
				{
					// allocate the string size, will use less space and wont crash if too big :p
					temp = (char*)malloc(len + 1);
					strcpy(temp,ptr+2);
				}

				string[(int)ptr - (int)&string[0]] = 0;

				strcat(string,fill_with);

				if ( temp )
				{
					strcat(string,temp);
					free(temp);
				}
		*/
	}

	return TRUE; // no point found (nothing to add) return true (string can be used)
}

BOOL CBot::builtTeleporterEntrance() const
{
	edict_t* pent = nullptr;

	while ((pent = UTIL_FindEntityByClassname(pent, "building_teleporter")) != nullptr)
	{
		if (pent->v.iuser1 == 1 && pent->v.euser2 == m_pEdict)
			return TRUE;
	}

	return FALSE;
}

BOOL CBot::builtTeleporterExit() const
{
	return getTeleporterExit() != nullptr;
}

edict_t* CBot::getTeleporterExit() const
{
	edict_t* pent = nullptr;

	while ((pent = UTIL_FindEntityByClassname(pent, "building_teleporter")) != nullptr)
	{
		if (pent->v.iuser1 == 2 && pent->v.euser2 == m_pEdict)
			return pent;
	}

	return pent;
}

int CBot::GetLadderDir(const BOOL bCheckWaypoint) const
// Get ladder dir, simply find if the bot
// wants to go up or down..
//return -1 (down), 0 (jump off), 1 (up)
{
	const int iNextWaypoint = GetNextWaypoint();

	if (bCheckWaypoint && iNextWaypoint != -1)
	{
		const Vector vNextWptOrigin = waypoints[iNextWaypoint].origin;

		if (vNextWptOrigin.z > pev->origin.z - 8)
			return 1;
		else if (vNextWptOrigin.z < pev->origin.z)
			return -1;
	}
	else
	{
		// keep going up if 8 units from top so we can climb further.
		if (m_vMoveToVector.z > pev->origin.z - 8)
			return 1;
		else if (m_vMoveToVector.z < pev->origin.z)
			return -1;
	}

	return 0;
}

BOOL CBot::NotStartedGame(void)
// return FALSE if bot hasn't been able to join the game yet
// if it cant join, it wont think, if it wont think it wont do anything except keep
// trying to join.
{
	// keep checking in NS if we havent joined a team because
	// a new round could have started and we will be back in the ready room!
	m_iTeam = GetTeam();

	if (m_bStartedGame == FALSE)
		return TRUE;

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_HL_RALLY:
		return !m_bSelectedCar;
	case MOD_NS:

		// Not joined a team yet.
		if (m_iTeam != TEAM_MARINE && m_iTeam != TEAM_ALIEN)
		{
			return TRUE;
		}

		return IsInReadyRoom();
		break;
	case MOD_BG:
		return !pev->playerclass && !pev->team;
	case MOD_TS:
		return FALSE;
	case MOD_COUNTERSTRIKE:
		break;
	default:
		break;
	}

	// a generic check...
	return FBitSet(pev->flags, FL_SPECTATOR);
}

void CBot::StartGame()
// do the neccesary stuff to start the game or join a team
// e.g, select vgui menus etc
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_HL_RALLY:

		/*if ( !m_bSelectedCar )
		{
		FakeClientCommand(m_pEdict,"menuselect 1");
		FakeClientCommand(m_pEdict,"menuselect %d",RANDOM_LONG(1,8));
		FakeClientCommand(m_pEdict,"menuselect 4");

		  FakeClientCommand(m_pEdict,"menuselect 1");
		  FakeClientCommand(m_pEdict,"menuselect 1");

			m_bSelectedCar = TRUE;
		}*/

		break;
		/*case MOD_GEARBOX: // Support for OP4CTF [APG]RoboCop[CL]

			FakeClientCommand(m_pEdict, "jointeam 3");
			FakeClientCommand(m_pEdict, "selectchar 7");
			m_bStartedGame = TRUE;
			break;
			*/
			
	case MOD_BUMPERCARS:
	{
		FakeClientCommand(m_pEdict, "changeclass %d", RANDOM_LONG(1, 20));
		break;
	}
	// natural selection
	case MOD_NS:
	{
		// Find a team to join

		// setup bots final upgrade (e.g. grenade gun/hmg or onos/fade)
		/*m_iCombatInfo = (1<<(RANDOM_LONG(0,2)));

		// setup extra, e.g. wanting jetpack or armor
		switch ( RANDOM_LONG(0,2) )
		{
		case 1:
			SetWantCombatItem(BOT_COMBAT_WANT_JETPACK,TRUE);
			break;
		case 2:
			SetWantCombatItem(BOT_COMBAT_WANT_ARMOR,TRUE);
			break;
		default:
			break;
		}

		if ( RANDOM_LONG(0,1) )
			SetWantCombatItem(BOT_COMBAT_WANT_RESUPPLY,TRUE);

		if ( RANDOM_LONG(0,1) )
			SetWantCombatItem(BOT_COMBAT_WANT_WELDER,TRUE);

		if ( RANDOM_LONG(0,1) )
			SetWantCombatItem(BOT_COMBAT_WANT_MINES,TRUE);*/

		if (gBotGlobals.IsCombatMap())
		{
			if (m_fCombatFitness > 0.0f)
			{
				m_pCombatBits->set_fitness(m_fCombatFitness);
				gBotGlobals.m_pCombatGA[m_iCombatTeam].addToPopulation(m_pCombatBits->copy());
				//m_fCombatFitness = 0; //That line triggers crash [APG]RoboCop[CL]

				if (m_pCombatBits)
				{
					delete m_pCombatBits;
					m_pCombatBits = nullptr;
				}
			}
		}

		// allow time for bots to join after each other
		if (gBotGlobals.m_fNextJoinTeam > gpGlobals->time)
			return;

		if (m_Profile.m_iFavTeam == TEAM_AUTOTEAM)
		{
			FakeClientCommand(m_pEdict, "autoassign");
		}
		else
		{
			switch (m_Profile.m_iFavTeam)
			{
			case TEAM_ALIEN:
				FakeClientCommand(m_pEdict, "jointeamtwo");

				// if balance teams is set for bot then set fav team to other team
				// incase it can't join to balance the teams
				if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_BALANCE_TEAMS))
					m_Profile.m_iFavTeam = TEAM_MARINE;
				break;
			case TEAM_MARINE:
				FakeClientCommand(m_pEdict, "jointeamone");

				if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_BALANCE_TEAMS))
					m_Profile.m_iFavTeam = TEAM_ALIEN;

				break;
			default: ;
			}
		}

		m_iCombatTeam = 0;

		if (IsAlien())
			m_iCombatTeam = 1;

		if (gBotGlobals.IsCombatMap())
		{
			if (gBotGlobals.m_pCombatGA[m_iCombatTeam].canPick())
			{
				//				if (m_pCombatBits)
				delete m_pCombatBits;

				m_pCombatBits = static_cast<CIntGAValues*>(gBotGlobals.m_pCombatGA[m_iCombatTeam].pick());
				m_iCombatInfo = m_pCombatBits->get();
			}
			else
			{
				if (!m_pCombatBits || !m_pCombatBits->get() || m_iCombatInfo == m_pCombatBits->get())
				{
					m_iCombatInfo = RANDOM_LONG(0, BOT_COMBAT_WANT_SENUP3 * 2 - 1);

					//					if (m_pCombatBits)
					delete m_pCombatBits;

					m_pCombatBits = new CIntGAValues(m_iCombatInfo);
				}
				else
					m_iCombatInfo = m_pCombatBits->get();
			}
		}

		gBotGlobals.m_fNextJoinTeam = gpGlobals->time + 1.0;

		SpawnInit(FALSE);

		m_bStartedGame = TRUE;

		return;
	}
	break;
	// international online soccer
	case MOD_IOS:
	{
		// was going to do IOS but bots came out on ios 3.0 :P
		switch (this->m_iVguiMenu)
		{
		case VGUI_MENU_IOS_SELECT_TEAM:
			break;
		case VGUI_MENU_IOS_SELECT_POSITION:
			break;
		}
	}
	break;
	case MOD_TS:
	{
		m_bStartedGame = TRUE;
	}

	break;
	// battle grounds
	case MOD_BG:
	{
		// dont use vgui every frame.
		if (m_fNextUseVGUI > gpGlobals->time)
			break;

		m_fNextUseVGUI = gpGlobals->time + 1.0;

		switch (this->m_iVguiMenu)
		{
		case VGUI_MENU_BG_SELECT_TEAM:
			// select the bots favourite/designated team
		{
			int iTeam = m_Profile.m_iFavTeam;

			// if team isn't valid then make iTeam 5. (keep it 1, 2 or 5 )
			if ( iTeam < 1 || iTeam > 5 || iTeam == 3 || iTeam == 4 )
				iTeam = 5;

			FakeClientCommand(m_pEdict, "jointeam %d", iTeam);
		}
		break;
		case VGUI_MENU_BG_SELECT_AMERICAN_CLASS:
		case VGUI_MENU_BG_SELECT_BRITISH_CLASS:
			// select a random class
			FakeClientCommand(m_pEdict, "joinclass %d", RANDOM_LONG(1, 3));
			// should have started the game now
			m_bStartedGame = TRUE;

			m_fNextUseVGUI = gpGlobals->time + 3.0;
			break;
		}
	}
	break;
	case MOD_DMC:
		FakeClientCommand(m_pEdict, "_firstspawn");
		m_bStartedGame = TRUE;
		return;
		break;
	default:
		break;
	}

	// generic case, just press fire :)
	if (RANDOM_LONG(0, 100) < 50)
	{
		PrimaryAttack();
		m_bStartedGame = TRUE;
		// set the boolean to say we've joined the game.
	}
}

eClimbType CBot::GetClimbType(void) const
// return a type of climbing or flying info
// if it is not BOT_CLIMB_NONE then the bot is
// trying to climb or fly
{
	if (m_pEnemy != nullptr)
		return BOT_CLIMB_NONE;

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		if (IsSkulk())
		{
			// if bot is sticking to a wall
			if (IsWallSticking())
				return BOT_CLIMB_WALL_STICKING;
			if (m_iCurrentWaypointIndex != -1 && m_iCurrentWaypointFlags & W_FL_WALL_STICK)
				return BOT_CLIMB_TRYING_WALL_STICK;
		}
		else if (IsLerk())
		{
			if (m_iCurrentWaypointIndex != -1)
			{
				if (m_iCurrentWaypointFlags & W_FL_LADDER)
					return BOT_CLIMB_FLYING;
				if (m_iCurrentWaypointFlags & W_FL_FLY)
					return BOT_CLIMB_FLYING;
				if (m_iCurrentWaypointFlags & W_FL_WALL_STICK)
					return BOT_CLIMB_FLYING;
			}
		}
		else if (IsFade())
		{
			if (m_iCurrentWaypointIndex != -1)
			{
				if (m_iCurrentWaypointFlags & W_FL_FLY)
					return BOT_CLIMB_FLYING;
				if (m_iCurrentWaypointFlags & W_FL_WALL_STICK)
					return BOT_CLIMB_FLYING;
			}
		}
		break;
	default:
		break;
	}

	// bot is already climbing a ladder
	if (IsOnLadder())
		return BOT_CLIMB_CLIMBING;

	// bot is about to climb a ladder but not on it yet
	if (m_iCurrentWaypointIndex != -1 && m_iCurrentWaypointFlags & W_FL_LADDER)
	{
		if (gBotGlobals.IsNS() && IsSkulk())
		{
			return BOT_CLIMB_TRYING_WALL_STICK;
		}

		return BOT_CLIMB_NOT_TOUCHING;
	}

	return BOT_CLIMB_NONE;
}

void CBot::Think(void)
// Bot think
// without this function... bots will do nothing!
{
	static TraceResult tr;

	m_f3dVelocity = (pev->velocity + pev->basevelocity).Length();
	m_f2dVelocity = (pev->velocity + pev->basevelocity).Length2D();

	m_bLookForNewTasks = TRUE;

	// Reset bots main forward move speed
	m_fMoveSpeed = 0;

	// If stuck code has not overidden upspeed (while swimming)
	// reset it to 0
	if (m_fUpTime < gpGlobals->time)
		m_fUpSpeed = 0;

	// Save last buttons held in incase some need to be held in after
	// resetting (for ladder climbing etc)
	m_iLastButtons = pev->button;

	// Bots must have this flag set at all times
	pev->flags |= FL_FAKECLIENT;

	// Reset the bots squad leader (even if it has a squad it will be updated after)
	m_pSquadLeader = nullptr;

	// If not started the game (e.g. not chosen team or class yet)
	// do it before continuing.
	if (NotStartedGame())
	{
		StartGame();

		return;
	}

	// Haven't added another goal waypoint to our list of failed waypoint goals
	// Clear the list so we have a chance of checking them again to see if we dont
	// fail them next time
	if (!m_FailedGoals.IsEmpty() && m_fNextClearFailedGoals < gpGlobals->time)
	{
		m_FailedGoals.Destroy();
		//m_fNextClearFailedGoals
	}

	// Does bot have a squad?
	if (m_stSquad)
	{
		// If I am not the leader of the squad update the squad leader
		if (!m_stSquad->IsLeader(m_pEdict))
			m_pSquadLeader = static_cast<edict_t*>(m_stSquad->GetLeader());
		else
		{
			// See if I have to say some stuff to my squad if I am leader
			// e.g. "let's move out!"
			if (!m_bSaidMoveOut && m_fNextUseSayMessage < gpGlobals->time)
			{
				switch (gBotGlobals.m_iCurrentMod)
				{
				case MOD_NS:
					Impulse(SAYING_8);
					break;
				default:
					FakeClientCommand(m_pEdict, "say_team \"Let's go!\"");
					break;
				}

				m_bSaidMoveOut = TRUE;
				m_fNextUseSayMessage = gpGlobals->time + 5.0;
			}
		}

		// If I am following a leader, check the conditions
		if (m_pSquadLeader)
		{
			// If the leader is alive update the condition, if it's a commander in NS
			// then we can't follow so assume not alive too...
			if (EntityIsAlive(m_pSquadLeader))
			{
				if (gBotGlobals.IsNS() && EntityIsCommander(m_pSquadLeader) || IsEnemy(m_pSquadLeader))
					UpdateCondition(BOT_CONDITION_SQUAD_LEADER_DEAD);
				else
					RemoveCondition(BOT_CONDITION_SQUAD_LEADER_DEAD);
			}
			else
				UpdateCondition(BOT_CONDITION_SQUAD_LEADER_DEAD);
		}
	}

	// Update condition if bot wants to leave the game
	if (!HasCondition(BOT_CONDITION_WANT_TO_LEAVE_GAME) && gBotGlobals.IsConfigSettingOn(BOT_CONFIG_BOTS_LEAVE_AND_JOIN))
	{
		if (WantToLeaveGame())
			UpdateCondition(BOT_CONDITION_WANT_TO_LEAVE_GAME);
	}

	//	try
	//	{
	m_fCombatFitness = pev->frags / m_iNumDeaths;

	// Not alive anymore
	if (!IsAlive())
	{
		const BOOL feigned = FALSE;

		if (!feigned)
		{
			if (m_bNeedToInit)
			{
				m_iNumDeaths++;
				m_fSurvivalTime = gpGlobals->time - m_fSpawnTime;
				const int fragsSinceDeath = pev->frags - m_iNumFragsSinceDeath;
				int team = GetTeam();

				if (team < 0 || team >= MAX_TEAMS)
					team = 0;

				const int teamScoreSinceDeath = gBotGlobals.m_iTeamScores[team] - m_iPrevTeamScore;
				m_iPrevTeamScore = gBotGlobals.m_iTeamScores[team];

				const float fFitness = m_fSurvivalTime * 0.001 + (float)fragsSinceDeath * 0.2 + (float)teamScoreSinceDeath * 0.2;

				m_GASurvival->set_fitness(fFitness);

				if (gBotGlobals.IsMod(MOD_TS))
				{
					m_pTSWeaponSelect->set_fitness(fFitness);

					gBotGlobals.m_TSWeaponChoices.addToPopulation(m_pTSWeaponSelect->copy());
				}

				CGA* ga = &gBotGlobals.m_enemyCostGAsForTeam[team];

				IIndividual* newCopy = m_GASurvival->copy();

				ga->addToPopulation(newCopy);

				if (m_bUsedMelee)
				{
					ga = m_personalGA;

					m_pPersonalGAVals->set_fitness(fFitness);

					newCopy = m_pPersonalGAVals->copy();

					ga->addToPopulation(newCopy);
					m_bUsedMelee = FALSE;
				}

				SpawnInit(FALSE);

				m_fSpawnTime = 0;

				/// doesn't work... ??
				if (gBotGlobals.IsMod(MOD_TS))
				{
					FakeClientCommand(m_pEdict, "buy");
					FakeClientCommand(m_pEdict, "menuselect 7"); // buy random
				}
			}

			if (gBotGlobals.IsMod(MOD_TS))
			{
				if (m_fRespawnTime < gpGlobals->time)
				{
					FakeClientCommand(m_pEdict, "respawn");
					m_fRespawnTime = gpGlobals->time + 5.0f;
				}
			}

			// keep pressing attack occasionally releasing attack key
			if (RANDOM_LONG(0, 1))
				pev->button = IN_ATTACK;
			else
				pev->button = 0;

			return;
		}
		else
			m_CurrentLookTask = BOT_LOOK_TASK_NONE;
	}

	if (m_fSpawnTime == 0)
	{
		m_fSpawnTime = gpGlobals->time;
		m_iNumFragsSinceDeath = pev->frags;
		m_fPrevHealth = pev->health;

		m_vSpawnPosition.SetVector(pev->origin);
	}

	//	}
	//	catch (...)
	//	{
	//		BugMessage(NULL,"GOTCHA!!!! (respawn)");
	//	}
	//	m_iLastCurrentWaypoint = m_iCurrentWaypointIndex;

	m_bNeedToInit = TRUE;

	if (gBotGlobals.IsMod(MOD_TFC))
		m_fMaxSpeed = pev->maxspeed;

	if (m_stSquad)
	{
		if (m_stSquad->IsStealthMode())
			m_fIdealMoveSpeed = m_fMaxSpeed / 2;
	}

	if (gBotGlobals.IsNS() && IsCommander())
	{
		this->BotCommand();
		return;
	}

	// do some stuck check code...
	// (cant do if bot is an NS commander)
	// A commander still wants to have think time and press buttons

	//////////////////////
	// MOVE/LOOK CODE

	WorkViewAngles();

	WorkMoveDirection();

	//////////////////////////////////////////////
	// Check if stuck and do stuff to get unstuck
	// if need be
	CheckStuck();
	///////////////////////////////////////////

	//// Thought a brief moment ago
	if (m_fLastThinkTime >= gpGlobals->time)
	{
		return;
	}

	m_fLastThinkTime = gpGlobals->time + m_fThinkTime;

	if (!HasCondition(BOT_CONDITION_SELECTED_GUN))
	{
		if (gBotGlobals.IsMod(MOD_TS))
		{
			if (gBotGlobals.m_TSWeaponChoices.canPick())
			{
				IIndividual* costs = gBotGlobals.m_TSWeaponChoices.pick();

				m_pTSWeaponSelect->freeMemory();
				delete m_pTSWeaponSelect;

				m_pTSWeaponSelect = static_cast<CBotGAValues*>(costs);
				m_pTSWeaponSelect->set_fitness(0);

				if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_DISABLE_WEAPON_LEARN))
				{
					m_pTSWeaponSelect->set(0, RANDOM_FLOAT(0, 1));
					m_pTSWeaponSelect->set(1, RANDOM_FLOAT(0, 1));
					m_pTSWeaponSelect->set(2, RANDOM_FLOAT(0, 1));
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						const float fval = m_pTSWeaponSelect->get(i);

						if (fval < 0.0)
							m_pTSWeaponSelect->set(i, 0.0);
						else if (fval > 1.0)
							m_pTSWeaponSelect->set(i, 1.0);
					}
				}
			}
			else
			{
				m_pTSWeaponSelect->clear();

				// max 3 weapons
				m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1)); // 0
				m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1));
				m_pTSWeaponSelect->add(RANDOM_FLOAT(0, 1));

				// stunt (TS)
				m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 3
				m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 4
				m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 5
				m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 6
				m_pTSWeaponSelect->add(RANDOM_FLOAT(-1, 1)); // 7

				//for ( int i = 0; i < MAX_WEAPONS; i ++ )
				//	m_pTSWeaponSelect->add(RANDOM_FLOAT(0,1)); // start 8 num 37, 7+37 = 44 (45)// fire percent
			}

			if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TS_KUNGFU))
			{
				UTIL_makeTSweapon(m_pEdict, (eTSWeaponID)(int)(m_pTSWeaponSelect->get(0) * 35));
				UTIL_makeTSweapon(m_pEdict, (eTSWeaponID)(int)(m_pTSWeaponSelect->get(1) * 35));
				UTIL_makeTSweapon(m_pEdict, (eTSWeaponID)(int)(m_pTSWeaponSelect->get(2) * 35));
			}

			RemoveCondition(BOT_CONDITION_DONT_CLEAR_OBJECTIVES);
			UpdateCondition(BOT_CONDITION_SELECTED_GUN);
		}
	}

	// Reset ideal move speed to max speed, can be changed in some tasks.
	m_fIdealMoveSpeed = m_fMaxSpeed;

	UpdateConditions();

	//try
	//{
	if (m_fUpdateVisiblesTime < gpGlobals->time)
	{
		m_fUpdateVisiblesTime = gpGlobals->time + m_Profile.m_fVisionTime;

		if (!UpdateVisibles())
		{
			if (m_fNextWorkRangeCosts < gpGlobals->time)
				m_fNextWorkRangeCosts = gpGlobals->time + 1.0f;
		}
	}

	//}
	//catch(...)
	//{
	//	BugMessage(NULL,"GOTCHA!!!! (update visibles)");
	//}

	// if no new buttons pressed before this statement
	// reset buttons
	if (m_iLastButtons == pev->button)
		pev->button = 0;

	////////////////////////////////////
	// MOD Specific set-up / per-frame
	//
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
	{
		const int iSpecies = pev->iuser3;

		// clamatius : getting Resources
		m_iResources = pev->vuser4.z / 100;
		// clamatius : getting Experience
		m_iExperience = pev->vuser4.z;
		// ns get level from experience
		m_iLevel = NS_GetPlayerLevel(m_iExperience);

		if (IsGestating())
			return; // dont think if gestating

		if (gBotGlobals.IsNS() && m_iCurrentWaypointIndex != -1 && !(m_iCurrentWaypointFlags & W_FL_WALL_STICK))
		{
			if (IsWallSticking()) // dont want to wall stick if waypoint i want to use is not a wall stick waypoint
				Duck();
		}

		if (IsAlien())
		{
			if (m_iLastSpecies != iSpecies)
			{
				// changed species, might as well flush tasks for bot to get
				// more species specific tasks.
				m_Tasks.FlushTasks();
			}
			m_iLastSpecies = iSpecies;
		}

		edict_t* pCommander;

		if (IsMarine())
		{
			if (!gBotGlobals.IsCombatMap() && (pCommander = gBotGlobals.GetCommander()) != nullptr)
			{
				if (m_fNextUseSayMessage < gpGlobals->time)
				{
					// Shout for medic if less than 50% health
					if (pev->health < pev->max_health * 0.25)
					{
						Impulse(10);
						m_fNextUseSayMessage = gpGlobals->time + RANDOM_FLOAT(18.0, 26.0);

						// check out commander, give him a tip

						CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pCommander);

						if (pClient)
							pClient->AddNewToolTip(BOT_TOOL_TIP_COMMANDER_HEALTH);
					}
				}
			}
			/*
			// keep checking for cover and shout for grenades
			if ( (m_fNextCheckCover < gpGlobals->time) && m_pAvoidEntity )
			{
				char *szClassname = (char*)STRING(m_pAvoidEntity->v.classname);

				// run for cover from grenades and exploding robo grunts
				if ( strcmp(szClassname,"sporegunprojectile") )
				{
					// say "take cover" message
					FakeClientCommand(m_pEdict,"grenade");
					m_fNextUseSayMessage = gpGlobals->time + RANDOM_FLOAT(8.0,12.0);

					int iCoverWpt = WaypointLocations.GetCoverWaypoint(pev->origin,EntityOrigin(m_pAvoidEntity),&m_FailedGoals);

					if ( iCoverWpt != -1 )
					{
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,0,NULL,iCoverWpt,-2));
						m_fNextCheckCover = gpGlobals->time + 1.0;
					}
				}
			}*/
		}
	}
	break;
	case MOD_TS:
	{
		/*if ( gBotGlobals.m_iForceTeam != -1 )
		{
			if ( !m_bSelectedCar ) // "hack"
			{
				FakeClientCommand(m_pEdict,"changeteam");
				FakeClientCommand(m_pEdict,"menuselect %d",gBotGlobals.m_iForceTeam);

				m_bSelectedCar = TRUE;
			}
		}*/

		// keep checking for cover
		if (m_pAvoidEntity && m_fNextCheckCover < gpGlobals->time)
		{
			//char *szClassname = (char*)STRING(m_pAvoidEntity->v.classname);

			// run for cover from grenades and exploding robo grunts
			if (UTIL_IsGrenadeRocket(m_pAvoidEntity))
			{
				// guess final position of grenade

				Vector v_src = EntityOrigin(m_pAvoidEntity);

				BotFunc_TraceToss(m_pAvoidEntity, nullptr, &tr);
				//UTIL_TraceLine(v_src,v_src+m_pAvoidEntity->v.velocity*5.0,ignore_monsters,dont_ignore_glass,m_pAvoidEntity,&tr);

				RunForCover(tr.vecEndPos, TRUE);
				/*
				int iCoverWpt = WaypointLocations.GetCoverWaypoint(pev->origin,tr.vecEndPos,&m_FailedGoals);

				if ( iCoverWpt != -1 )
				{
					AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,0,NULL,iCoverWpt,-2));
					m_fNextCheckCover = gpGlobals->time + 1.0;
				}*/
			}
			// cant get Damage message, do it manually
			if (!m_pEnemy && pev->health < m_fPrevHealth)
			{
				UTIL_MakeVectors(pev->v_angle);

				// make origin behind bot so it looks around
				Vector origin = GetGunPosition() - gpGlobals->v_forward * 128;

				BotEvent(BOT_EVENT_HURT, nullptr, nullptr, static_cast<float*>(origin));
			}
		}
	}
	default:
		break;
	}

	///////////////////////////////////
	// per-frame condition updates
	//

	// Update see Waypoint condition
	if (m_iCurrentWaypointIndex == -1 || !(m_iCurrentWaypointFlags & W_FL_LADDER))
	{
		UTIL_TraceLine(pev->origin, m_vMoveToVector, ignore_monsters, ignore_glass, m_pEdict, &tr);

		if (tr.flFraction >= 1.0)
		{
			UpdateCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT);
			m_fLastSeeWaypoint = 0;
		}
		else
		{
			BOOL bRemove = TRUE;

			if (tr.pHit)
			{
				// waypoint through a door I can walk through
				if (strcmp(STRING(tr.pHit->v.classname), "worldspawn"))
					// not worldspawn
				{
					if (BotFunc_EntityIsMoving(&tr.pHit->v))
						bRemove = FALSE;
					else if (strncmp(STRING(tr.pHit->v.classname), "func_door", 8) == 0)
						bRemove = FALSE;
				}
			}

			if (bRemove)
			{
				if (HasCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT))
					m_fLastSeeWaypoint = gpGlobals->time;

				RemoveCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT);
			}
			else
			{
				UpdateCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT);
				m_fLastSeeWaypoint = 0;
			}
		}
	}
	else if (m_bMoveToIsValid)
		m_fLastSeeWaypoint = 0;

	m_CurrentTask = m_Tasks.CurrentTask();

	BOOL bFollowLeader = TRUE;
	BOOL bCanUpgrade = TRUE;

	if (m_CurrentTask)
	{
		edict_t* pTaskEdict;

		if ((pTaskEdict = m_CurrentTask->TaskEdict()) != nullptr)
		{
			UTIL_TraceLine(pev->origin, EntityOrigin(pTaskEdict), ignore_monsters, ignore_glass, m_pEdict, &tr);

			if (tr.flFraction >= 1.0)
				UpdateCondition(BOT_CONDITION_SEE_TASK_EDICT);
			else
				RemoveCondition(BOT_CONDITION_SEE_TASK_EDICT);
		}

		bFollowLeader = m_CurrentTask->Task() != BOT_TASK_FIND_PATH;

		if (m_CurrentTask->Task() == BOT_TASK_NORMAL_ATTACK)
		{
			if (m_pCurrentWeapon && m_pCurrentWeapon->IsMelee())
				bFollowLeader = FALSE;

			// don't upgrade while attacking
			bCanUpgrade = FALSE;
		}
	}

	///////////////////////////////////////////////////

	// keep checking upgrades
	if (gBotGlobals.IsNS() && bCanUpgrade && gBotGlobals.IsCombatMap() && m_fUpgradeTime < gpGlobals->time)
	{
		AddPriorityTask(CBotTask(BOT_TASK_COMBAT_UPGRADE, 0));
		m_fUpgradeTime = gpGlobals->time + RANDOM_FLOAT(5.0, 10.0);
	}

	if (bFollowLeader && m_stSquad && m_pSquadLeader && HasCondition(BOT_CONDITION_SEE_SQUAD_LEADER) && !HasCondition(BOT_CONDITION_SQUAD_LEADER_DEAD))
	{
		// Dont follow waypoints if following a leader.

		const Vector vMoveTo = m_stSquad->GetFormationVector(m_pEdict);

		if (DistanceFrom(vMoveTo, TRUE) > m_stSquad->GetSpread())
			SetMoveVector(vMoveTo);
		else
		{
			if (!UTIL_OnGround(&m_pSquadLeader->v))
				touchedWpt();
			StopMoving();
		}
	}
	else
	{
		if (IsSquadLeader())
			BotSquadLeaderThink();

		if (m_fUpdateWaypointTime < gpGlobals->time)
		{
			if (!BotNavigate_UpdateWaypoint(this))
			{
				//StopMoving();
				m_fUpdateWaypointTime = gpGlobals->time + 0.5;
			}
		}
	}

	if (m_pEnemy == nullptr)
	{
		// Make sure the bot's not shooting an enemy before listening to sounds.
		edict_t* pPlayer = nullptr;

		if (GetClimbType() == BOT_CLIMB_NONE && (!PlayerStandingOnMe() && !StandingOnPlayer()) && m_fListenToSoundTime < gpGlobals->time && !m_iOrderType)
		{
			m_fListenToSoundTime = gpGlobals->time + m_fReactionTime + RANDOM_FLOAT(0.3, 0.75);

			if ((pPlayer = UTIL_UpdateSounds(pev)) != nullptr)
			{
				if (!m_Tasks.HasTask(BOT_TASK_LISTEN_TO_SOUND))
				{
					AddPriorityTask(CBotTask(BOT_TASK_LISTEN_TO_SOUND, 0, pPlayer, 0, RANDOM_FLOAT(2.0, 3.0)));
				}
			}
		}
		else if (m_fChatMessageTime < gpGlobals->time)
		{
			if (!m_Tasks.HasTask(BOT_TASK_TYPE_MESSAGE))
			{
				m_fChatMessageTime = gpGlobals->time + RANDOM_LONG(10, 20);

				if (RANDOM_LONG(0, 100) < gBotGlobals.m_iBotChatPercent)
				{
					BotChat(BOT_CHAT_IDLE, nullptr);
					gettingBored();
				}
			}
		}
		else
		{
			// safe to reload?
			if (m_fLastSeeEnemyTime && m_fLastSeeEnemyTime + 5.0 < gpGlobals->time)
			{
				// Need a max clip variable in CWeaponPreset, to make this effective
				if (m_pCurrentWeapon && m_pCurrentWeapon->CanReload())
				{
					AddPriorityTask(CBotTask(BOT_TASK_RELOAD));
					m_fLastSeeEnemyTime = 0;
				}
				else
					m_fLastSeeEnemyTime = 0;
			}
		}
	}
	else
	{
		m_fChatMessageTime = gpGlobals->time + 5.0;
		m_fLastSeeEnemyTime = gpGlobals->time;
	}

	if (m_stSquad)
	{
		if (m_stSquad->IsProneMode())
		{
			// if isn't proning then prone , depending on mod
			if (m_iTS_State != TS_State_Stunt && gBotGlobals.IsMod(MOD_TS))
				AltButton();
		}

		if (m_stSquad->IsCrouchMode())
		{
			Duck();
		}

		if (!m_stSquad->SquadCanShoot())
		{
			UpdateCondition(BOT_CONDITION_CANT_SHOOT);
		}
		else
			RemoveCondition(BOT_CONDITION_CANT_SHOOT);
	}

	// Remove this condition, will be updated in tasks
	RemoveCondition(BOT_CONDITION_CANT_SHOOT);
	///////////////////////
	// Reset enemy
	//
	// Reset enemy so we can make sure it loses enemy if normal_attack cant update it
	m_pEnemy = nullptr;
	////////////////////////////////
	// Task Code
	DoTasks();

	// done tasks
	RemoveCondition(BOT_CONDITION_TASK_EDICT_PAIN);

	//try
	//{
	///////////////////////////////
	// Get New Task
	if (m_bLookForNewTasks)
	{
		LookForNewTasks();
	}
	//}

	//catch(...)
	//{
	//	BugMessage(NULL,"GOTCHA!!!! (m_bLookForNewTasks)");
	//}

	/*try
	{*/

	///////////////////////
	// Find an enemy
	if (WantToFindEnemy())
	{
		// we can find an enemy...
		m_pEnemy = FindEnemy();
	}
	else
		m_pEnemy = nullptr;
	//}

	/*catch(...)
	{
		BugMessage(NULL,"GOTCHA!!!! (FindEnemy)");
	}*/

	if (gBotGlobals.IsNS())
	{
		// check my last species in NS
		// this is so we can check if we have evolved successfully.
		m_iLastSpecies = pev->iuser3;
	}

	/*
		if ( bAimStopMoving )
			StopMoving();
	//*/
	return;
}

BOOL CBot::WantToFindEnemy(void) const
{
	if (gBotGlobals.IsMod(MOD_TS))
		return !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_DONT_SHOOT);
	// can I shoot?
	if (HasCondition(BOT_CONDITION_CANT_SHOOT))
		return FALSE;
	// is bot dont shoot setting on?
	else if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_DONT_SHOOT))
		return FALSE;
	// Always find enemy in bumpercars, whatever the problem ;-P
	else if (gBotGlobals.IsMod(MOD_BUMPERCARS))
		return TRUE;
	// Have a weapon?
	else if (!HasCondition(BOT_CONDITION_HAS_WEAPON))
		return FALSE;

	return TRUE;
}
/*
typedef enum
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

typedef enum
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
	CAlienAction ( eAlienAction action, eMaskAlienActionResult result )
	{
		m_action = action;
		m_result = result;
	}

	float Utility ()
	{
		float fUtility = 1;

		if ( m_result & ACTION_RES_FASTER_RESOURCES )
			fUtility *= 1.1;
		if ( m_result & ACTION_RES_MORE_HEALTH )
			fUtility *= 0.8;
		if ( m_result & ACTION_RES_MORE_ABILITIES )
			fUtility *= 1.05;
		if ( m_result & ACTION_RES_MORE_SPAWNPOINTS )
			fUtility *= 1.1;
		if ( m_result & ACTION_RES_MORE_DEFENCES )
			fUtility *= 0.9;
		if ( m_result & ACTION_RES_CLOACKED )
			fUtility *= 0.5;
	}

	float ResultProbability ( eAlienMaskEvidence evd )
	{
		float fProbability = 1.0f;

		if ( m_result & ACTION_RES_FASTER_RESOURCES )
		{
			if ( evd & MASK_EVD_SLOW_RESOURCES )
				fProbability *= 1.0;
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.6;
			if ( evd & MASK_EVD_FAST_RESOURCES )
				fProbability *= 0.3;
			if ( evd & MASK_EVD_LOSING )
				fProbability *= 0.9;
		}
		if ( m_result & ACTION_RES_MORE_HEALTH )
		{
			if ( evd & MASK_EVD_LOW_HEALTH )
				fProbability *= 1.0;
			if ( evd & MASK_EVD_BALANCED )
				fProbability *= 0.6;
			if ( evd & MASK_EVD_LOSING )
				fProbability *= 0.9;
		}
		if ( m_result & ACTION_RES_MORE_ABILITIES )
		{
			if ( evd & MASK_EVD_SLOW_RESOURCES )
				fProbability *= 1.0;
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.6;
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.3;
			if ( evd & MASK_EVD_LOSING )
				fProbability *= 0.9;
		}
		if ( m_result & ACTION_RES_MORE_SPAWNPOINTS )
		{
			if ( evd & MASK_EVD_MIN_HIVES_UP )
				fProbability *= 1.0;
			else if ( evd & MASK_EVD_MAX_HIVES_UP )
				fProbability *= 0.0;
			else
				fProbability *= 0.5;

			if ( evd & MASK_EVD_FAST_RESOURCES )
				fProbability *= 0.9;
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.7;
			if ( evd & MASK_EVD_SLOW_RESOURCES )
				fProbability *= 0.2;
		}
		if ( m_result & ACTION_RES_MORE_DEFENCES )
		{
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.6;
			if ( evd & MASK_EVD_NORM_RESOURCES )
				fProbability *= 0.3;
			if ( evd & MASK_EVD_LOSING )
				fProbability *= 0.9;
		}
		if ( m_result & ACTION_RES_CLOACKED )
		{
			if ( evd & MASK_EVD_BALANCED )
				fProbability *= 0.7;
			if ( evd & MASK_EVD_WINNING )
				fProbability *= 0.9;
			if ( evd & MASK_EVD_MANY_SENS )
				fProbability *= 0.1;
		}

		if ( m_result & ACTION_RES_MORE_TRAITS )
		{
			if ( evd & MASK_EVD_MANY_DEFS )
				fProbability *= 0.66;
			if ( evd & MASK_EVD_MANY_SENS )
				fProbability *= 0.66;
			if ( evd & MASK_EVD_MANY_MOVS )
				fProbability *= 0.66;
		}
	}
private:
	eAlienAction m_action;
	eMaskAlienActionResult m_result;
};

class CAlienActions
{
public:
	void setup ()
	{
		m_Actions.push_back(CAlienAction(ACTION_BUILD_DEF,ACTION_RES_MORE_DEFENCES|ACTION_RES_MORE_TRAITS|ACTION_RES_MORE_HEALTH));
		m_Actions.push_back(CAlienAction(ACTION_BUILD_OFF,ACTION_RES_MORE_DEFENCES));
		m_Actions.push_back(CAlienAction(ACTION_BUILD_SENS,ACTION_RES_CLOACKED|ACTION_RES_MORE_TRAITS));
		m_Actions.push_back(CAlienAction(ACTION_BUILD_MOV,ACTION_RES_MORE_TRAITS));
		m_Actions.push_back(CAlienAction(ACTION_BUILD_RESTOWER,ACTION_RES_FASTER_RESOURCES));
		m_Actions.push_back(CAlienAction(ACTION_BUILD_HIVE,(ACTION_RES_MORE_SPAWNPOINTS + ACTION_RES_MORE_ABILITIES)));
		m_Actions.push_back(CAlienAction(ACTION_HEAL_PLAYER,ACTION_RES_MORE_HEALTH));
	}

	CAlienAction *getBestAction ( eAlienMaskEvidence evd )
	{
		BOOL gotBest = FALSE;
		float fMax = 0;
		float fCur = 0;
		CAlienAction *best = NULL;

		for ( unsigned int i = 0; i < m_Actions.size(); i ++ )
		{
			fCur = m_Actions[i].Utility() * m_Actions[i].ResultProbability(evd);

			if ( !gotBest || (fCur > fMax) )
			{
				best = &(m_Actions[i]);
				fMax = fCur;
				gotBest = TRUE;
			}
		}
	}
private:
	vector<CAlienAction> m_Actions;
};

*/
void CBot::LookForNewTasks(void)
{
	// Do a big check for new tasks
	///////////////////////////////
	BOOL bRoam = FALSE;

	StopMoving();

	// Find a new task for the bot
	//////////////////////////////
	Vector vOrigin;

	int iBestStructureToBuild = 0;

	dataStack<edict_t*> tempStack;
	edict_t* pEntity;
	entvars_t* pEntitypev;

	float fDistance = 0;
	float fNearestBuildableDist = 0;
	float fNearestAmmoDispDist = 0;
	float fNearestWeldableDist = 0;
	float fNearestHealablePlayerDist = 0;
	float fNearestCommandStationDist = 0;
	float fNearestBuildPositionDist = 0;
	float fNearestPickupEntityDist = 0;
	float fNearestHiveDist = 0;
	float fNearestResourceFountainDist = 0;
	float fNearestHEVchargerDist = 0;
	float fNearestHealthchargerDist = 0;
	float fNearestButtonDist = 0;

	edict_t* pNearestHEVcharger = nullptr;
	edict_t* pNearestHealthcharger = nullptr;
	edict_t* pNearestHealablePlayer = nullptr;
	edict_t* pNearestCommandStation = nullptr;
	edict_t* pNearestBuildable = nullptr;
	edict_t* pNearestAmmoDisp = nullptr;
	edict_t* pNearestWeldable = nullptr;
	// Nearest edict that an alien can build nearby
	///////////////////////////////////////////////
	edict_t* pNearestBuildEdict = nullptr;
	edict_t* pNearestPickupEntity = nullptr;
	edict_t* pNearestHive = nullptr;
	edict_t* pNearestResourceFountain = nullptr;
	edict_t* pNearestButton = nullptr;

	edict_t* pNearestScientist = nullptr;
	edict_t* pNearestBarney = nullptr;

	edict_t* pEnemyFlag = nullptr;

	BOOL bIsNS = gBotGlobals.IsNS();
	BOOL bIsMarine = bIsNS && IsMarine();
	BOOL bIsGorge = bIsNS && IsGorge();
	BOOL bCanBuild = bIsMarine || bIsGorge;
	BOOL bCanCommand = bIsMarine && gBotGlobals.GetCommander() == nullptr;
	BOOL bIsAlien = IsAlien();

	BOOL bNeedHealth = pev->health < pev->max_health * 0.75;
	BOOL bNeedArmor = pev->armorvalue < VALVE_MAX_NORMAL_BATTERY / 2;
	BOOL bCanUseScientist = !m_Tasks.HasSchedule(BOT_SCHED_USING_SCIENTIST);
	BOOL bCanUseBarney = !m_Tasks.HasSchedule(BOT_SCHED_USING_BARNEY);

	int iMod = gBotGlobals.m_iCurrentMod;

	char* szClassname;

	if (m_pLastEnemy && EntityIsAlive(m_pLastEnemy))
		m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY;
	else
		m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

	//tempStack = m_stBotVisibles;

	BOOL bCanBuildNearby;

	int iToBuild = 0;

	/////////////////////////////////////
	// Check Visibles for stuff right now
	m_pVisibles->resetIter();

	while ((pEntity = m_pVisibles->nextVisible()) != nullptr)
	{
		if (pEntity == m_pEdict)
			continue;

		bCanBuildNearby = FALSE;
		pEntitypev = &pEntity->v;
		vOrigin = EntityOrigin(pEntity);

		fDistance = DistanceFrom(vOrigin);

		szClassname = const_cast<char*>(STRING(pEntitypev->classname));

		// --- new Tasks ---
		// MOD specific Stuff
		switch (iMod)
		{
		case MOD_DMC:
		{
			if (!pNearestPickupEntity || fDistance < fNearestPickupEntityDist)
			{
				if (CanPickup(pEntity))
				{
					fNearestPickupEntityDist = fDistance;
					pNearestPickupEntity = pEntity;
					continue;
				}
			}
		}
		break;
		case MOD_BUMPERCARS:
			if (!pNearestPickupEntity || fDistance < fNearestPickupEntityDist)
			{
				if (CanPickup(pEntity))
				{
					fNearestPickupEntityDist = fDistance;
					pNearestPickupEntity = pEntity;
					continue;
				}
			}
			break;
		case MOD_TS:
			if (!pNearestPickupEntity || fDistance < fNearestPickupEntityDist)
			{
				if (CanPickup(pEntity))
				{
					fNearestPickupEntityDist = fDistance;
					pNearestPickupEntity = pEntity;
					continue;
				}
			}
			break;
		case MOD_NS:

			if (bCanBuild)
			{
				if (EntityIsBuildable(pEntity))
				{
					if (!pNearestBuildable || fDistance < fNearestBuildableDist)
					{
						pNearestBuildable = pEntity;
						fNearestBuildableDist = fDistance;
					}
				}
			}

			if (bIsMarine)
			{
				if (pEntitypev->iuser3 == AVH_USER3_ARMORY || pEntitypev->iuser3 == AVH_USER3_ADVANCED_ARMORY)
				{
					if (!pNearestAmmoDisp || fDistance < fNearestAmmoDispDist)
					{
						pNearestAmmoDisp = pEntity;
						fNearestAmmoDispDist = fDistance;
						continue;
					}
				}

				if (!pNearestWeldable || fDistance < fNearestWeldableDist)
				{
					if (EntityIsWeldable(pEntity))
					{
						pNearestWeldable = pEntity;
						fNearestWeldableDist = fDistance;
						continue;
					}
				}

				if (bCanCommand && pEntitypev->iuser3 == AVH_USER3_COMMANDER_STATION)
				{
					if (!pNearestCommandStation || fDistance < fNearestCommandStationDist)
					{
						pNearestCommandStation = pEntity;
						fNearestCommandStationDist = fDistance;
						continue;
					}
				}
			}
			else if (bIsAlien)
			{
				if (bIsGorge)
				{
					if (m_fNextBuildTime < gpGlobals->time && (!pNearestBuildEdict || fDistance < fNearestBuildPositionDist))
					{
						iToBuild = BotFunc_GetStructureForGorgeBuild(pev, pEntitypev);

						if (iToBuild)
						{
							iBestStructureToBuild = iToBuild;
							pNearestBuildEdict = pEntity;
							fNearestBuildPositionDist = fDistance;
						}
					}

					if (EntityIsAlive(pEntity) && EntityIsAlien(pEntity) && pEntitypev->health < pEntitypev->max_health * 0.75 && (!pNearestHealablePlayer || fDistance < fNearestHealablePlayerDist))
					{
						// If not a structure, OK
						// if is a structure, must currently be built
						if (!EntityIsAlienStruct(pEntity) || !EntityIsBuildable(pEntity))
						{
							pNearestHealablePlayer = pEntity;
							fNearestHealablePlayerDist = fDistance;
						}
					}
					else if (pEntitypev->iuser3 == AVH_USER3_FUNC_RESOURCE && (!pNearestResourceFountain || fDistance < fNearestResourceFountainDist))
					{
						if (!HasVisitedResourceTower(pEntity))
						{
							pNearestResourceFountain = pEntity;
							fNearestResourceFountainDist = fDistance;
						}
					}
				}

				// let gorge check for buildings it can build near hive too
				if (pEntitypev->iuser3 == AVH_USER3_HIVE && (!pNearestHive || fDistance < fNearestHiveDist))
				{
					pNearestHive = pEntity;
					fNearestHiveDist = fDistance;
				}

				/*if ( ...... pNearestBuildable == NULL ...... )
				{//blah......
				iBestStructureToBuild = AVH_USER3_OFFENSE_CHAMBER;
				//......
			}*/
			}

			break;
	default: ;
		}
	}

	// require new schedule identifier for new task
	int iNewScheduleId = m_Tasks.GetNewScheduleId();

	switch (iMod)
	{
	case MOD_TS:
		if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TS_DONT_PICKUP_WEAPONS) && pNearestPickupEntity)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestPickupEntity));
			AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, iNewScheduleId, pNearestPickupEntity));
			AddTask(CBotTask(BOT_TASK_USE, iNewScheduleId, pNearestPickupEntity));

			break;
		}

		if (gBotGlobals.m_bTeamPlay || gBotGlobals.isMapType(NON_TFC_TS_TEAMPLAY))
		{
			int iWpt;

			if (!m_TSObjectives.IsEmpty())
			{
				TSObjective pObj = m_TSObjectives.Random();//m_TSObjectives.top();

				iWpt = WaypointLocations.NearestWaypoint(pObj.getOrigin(), REACHABLE_RANGE, m_iLastFailedWaypoint, FALSE, FALSE, TRUE, nullptr);

				bRoam = GetTeam() == 1 && !RANDOM_LONG(0, 2) || GetTeam() == 2 && !RANDOM_LONG(0, 10);

				if (!bRoam && iWpt != -1)
				{
					AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));

					edict_t* pUse = nullptr;

					while ((pUse = UTIL_FindEntityInSphere(pUse, WaypointOrigin(iWpt), 100)) != nullptr)
					{
						if (strcmp("ts_hack", STRING(pUse->v.classname)) == 0)
							break;
						else if (strcmp("ts_bomb", STRING(pUse->v.classname)) == 0)
							break;
					}

					if (GetTeam() == 1)
						AddTask(CBotTask(BOT_TASK_DEFEND, iNewScheduleId));
					else if (pUse)
						AddTask(CBotTask(BOT_TASK_USE, iNewScheduleId, pUse, 0, 9, Vector(0, 0, 0)));

					break;
				}
			}
			//else
			//	ALERT(at_console,"WTF??? %s has no objectives\n",m_szBotName);

			iWpt = WaypointFindRandomGoal(pev->origin, m_pEdict, 4096.0, -1, W_FL_ENDLEVEL, &m_FailedGoals);

			if (m_iCurrentWaypointIndex != iWpt && iWpt != -1)
			{
				AddTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iWpt, -1));
				break;
			}

			//W_FL_ENDLEVEL
		}

		bRoam = TRUE;

		break;
	case MOD_BUMPERCARS:
		if (pNearestPickupEntity)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestPickupEntity));
			AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, iNewScheduleId, pNearestPickupEntity));
			break;
		}

		bRoam = TRUE;

		break;
	case MOD_NS:
		if (bIsMarine)
		{
			// can I see a buildable object?
			if (pNearestBuildable)
			{
				// TO:DO:::Use real SCHEDULES.. arghhh!!!
				AddPriorityTask(CBotTask(BOT_TASK_BUILD, iNewScheduleId, pNearestBuildable));
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestBuildable, 0, 0, Vector(0, 0, 0), 30.0));

				m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
				//m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId,60.0);

				break;
			}
			// can I use an ammo disp?
			else if (pNearestAmmoDisp &&
				//m_bCanUseAmmoDispenser &&
				m_pCurrentWeapon &&
				m_pCurrentWeapon->HasWeapon(m_pEdict) &&
				m_pCurrentWeapon->CanGetMorePrimaryAmmo())
			{
				AddPriorityTask(CBotTask(BOT_TASK_USE_AMMO_DISP, iNewScheduleId, pNearestAmmoDisp, 0, 0, EntityOrigin(pNearestAmmoDisp)));
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestAmmoDisp));
				break;
			}
			// Got something to weld?
			else if (pNearestWeldable && HasWeapon(NS_WEAPON_WELDER))
			{
				AddPriorityTask(CBotTask(BOT_TASK_WELD_OBJECT, iNewScheduleId, pNearestWeldable, 0, 0, EntityOrigin(pNearestWeldable)));
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestWeldable));
				break;
			}
			// no commanding yet
			/*else if ( gBotGlobals.IsConfigSettingOn(BOT_CONFIG_COMMANDING) && pNearestCommandStation )
			{
			AddPriorityTask(CBotTask(BOT_TASK_USE,iNewScheduleId,pNearestCommandStation,0,0,EntityOrigin(pNearestCommandStation)));
			AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,iNewScheduleId,pNearestCommandStation));
		}*/
		// Nothing else to do...?
			else if (gBotGlobals.GetCommander())// a commander is in the console
			{
				if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_WAIT_FOR_ORDERS))
				{
					if (!m_bHasAskedForOrder)
					{
						if (m_fNextUseSayMessage < gpGlobals->time)
						{
							// Ask for a new order...
							Impulse((int)ORDER_REQUEST);
							// Asked for an order now, so don't try for another while.
							m_bHasAskedForOrder = TRUE;
							// look around for some thing to do.
							m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

							m_fNextUseSayMessage = gpGlobals->time + RANDOM_FLOAT(8.0, 12.0);

							CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(gBotGlobals.GetCommander());

							if (pClient)
							{
								pClient->AddNewToolTip(BOT_TOOL_TIP_COMMANDER_MARINE_ORDER);
							}
						}
					}
					// Let the bot wait for his next order
					//AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_ORDERS,0));
					StopMoving();

					break;
				}
			}

			bRoam = TRUE;
		}
		else if (IsAlien())
		{
			BOOL bNotCombatMap = !gBotGlobals.IsCombatMap();

			m_bHasGestated = !IsSkulk();

			if (bIsGorge)
			{
				CActionUtilities actions;

				int iNumResourceFountains = UTIL_CountEntities("func_resource");
				int iNumAlienResourceTowers = UTIL_CountEntities("alienresourcetower");
				float fNumResourceTowers = 1.0f;

				if (iNumResourceFountains > 0)
					fNumResourceTowers = 1.0f - (float)iNumAlienResourceTowers / iNumResourceFountains;

				//-au2

				float fHiveResources = (float)m_iResources / NS_HIVE_RESOURCES;
				float fStructResources = (float)m_iResources / (NS_DEFENSE_CHAMBER_RESOURCES * 2);
				float fResTowerResources = (float)m_iResources / NS_RESOURCE_TOWER_RESOURCES;
				float fBuildableHealth = 0.0f;
				float fHealHealthUtility = 0.0f;

				/*
				gBotGlobals.m_fHiveImportance = 1.0f;
				gBotGlobals.m_fResTowerImportance = 0.7f;
				gBotGlobals.m_fHealingImportance = 0.75f;
				gBotGlobals.m_fStructureBuildingImportance = 0.4f;
				gBotGlobals.m_fRefillStructureImportance = 0.5f;
				*/
				if (fStructResources > 1.0f)
					fStructResources = 1.0f;

				fStructResources *= gBotGlobals.m_fStructureBuildingImportance;//0.4f;

				/*if ( iBestStructureToBuild )
				{
					switch ( iBest
					UTIL_CountEntitiesInRange();
				}*/

				if (fResTowerResources > 1.0f)
					fResTowerResources = 1.0f;

				fResTowerResources *= fNumResourceTowers * gBotGlobals.m_fResTowerImportance;//0.7);//0.75f;

				if (fHiveResources > 1.0f)
					fHiveResources = 1.0f;

				fHiveResources *= gBotGlobals.m_fHiveImportance;

				if (pNearestBuildable)
					fBuildableHealth = 1.0f - pNearestBuildable->v.fuser1 / 1000;

				fBuildableHealth *= gBotGlobals.m_fRefillStructureImportance;//0.5;

				if (pNearestHealablePlayer)
					fHealHealthUtility = (1.0f - pNearestHealablePlayer->v.health / pNearestHealablePlayer->v.max_health) * 0.5;

				fHealHealthUtility *= gBotGlobals.m_fHealingImportance;//0.75;

				actions.add(BOT_CAN_HEAL, iNumAlienResourceTowers && pNearestHealablePlayer != nullptr, fHealHealthUtility);
				actions.add(BOT_CAN_BUILD_HIVE, iNumAlienResourceTowers && !gBotGlobals.IsCombatMap() && pNearestHive && UTIL_CanBuildHive(&pNearestHive->v), fHiveResources);
				actions.add(BOT_CAN_REFILL_STRUCT, !gBotGlobals.IsCombatMap() && pNearestBuildable != nullptr, fBuildableHealth);
				actions.add(BOT_CAN_BUILD_STRUCT, iNumAlienResourceTowers && !gBotGlobals.IsCombatMap() && iBestStructureToBuild, fStructResources);
				actions.add(BOT_CAN_BUILD_RESOURCE, !gBotGlobals.IsCombatMap() && pNearestResourceFountain, fResTowerResources);

				eCanDoStuff action = actions.getBestAction();

				/*
				int iRand;
				dataUnconstArray<eCanDoStuff> iThingsICanDo;

				// heal a player
				if ( pNearestHealablePlayer )
				{
					iThingsICanDo.Add(BOT_CAN_HEAL);
				}

				if ( bNotCombatMap )
				{
					//if ( iNumResourceTowers >= 2 )
					//{
					if ( pNearestHive )
					{
						if ( UTIL_CanBuildHive(&pNearestHive->v))
						{
							iThingsICanDo.Add(BOT_CAN_BUILD_HIVE);
						}
					}

					if ( iBestStructureToBuild != 0 )
					{
						iThingsICanDo.Add(BOT_CAN_BUILD_STRUCT);
					}
					//}

					if ( pNearestBuildable )
						// build a non-built offensechamber etc..
					{
						iThingsICanDo.Add(BOT_CAN_REFILL_STRUCT);
					}

					if ( pNearestResourceFountain )
					{
						if ( !UTIL_FuncResourceIsOccupied(pNearestResourceFountain) )
						{
							iThingsICanDo.Add(BOT_CAN_BUILD_RESOURCE);
						}
					}
				}
				*/
				// stop going gorge, look for a new species to evolve into
				if (bNotCombatMap && UTIL_SpeciesOnTeam(AVH_USER3_ALIEN_PLAYER2, TRUE) > (int)(
					UTIL_PlayersOnTeam(TEAM_ALIEN) * gBotGlobals.m_fGorgeAmount))
				{
					m_bHasGestated = FALSE;
				}
				else if (action != BOT_CAN_NONE)//!iThingsICanDo.IsEmpty() )
				{
					switch (action)//iThingsICanDo.Random() )
					{
					case BOT_CAN_BUILD_HIVE:
						if (bNotCombatMap)
						{
							//see if hive is undefended...
							AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_HIVE_RESOURCES));
							AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pNearestHive, ALIEN_BUILD_HIVE));

							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
							m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(55.0, 65.0));
						}
						break;
					case BOT_CAN_BUILD_RESOURCE:
						if (bNotCombatMap)
						{
							AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestResourceFountain, NS_RESOURCE_TOWER_RESOURCES));
							AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_RESOURCE_TOWER_RESOURCES));
							AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pNearestResourceFountain, ALIEN_BUILD_RESOURCES));
							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
						}
						break;
					case BOT_CAN_REFILL_STRUCT: // already built but needs health
						if (bNotCombatMap)
						{
							AddPriorityTask(CBotTask(BOT_TASK_BUILD, iNewScheduleId, pNearestBuildable));
							AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestBuildable));
							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);

							// should be nearby and shouldn't take to long to complete
							// if takes too long then forget it for now (set the time)
							m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(25.0, 35.0));
							break;
						}
					case BOT_CAN_BUILD_STRUCT:
						if (bNotCombatMap)
						{
							if (fNearestBuildPositionDist < 96) // too close
								AddPriorityTask(CBotTask(BOT_TASK_AVOID_OBJECT, iNewScheduleId, pNearestBuildEdict, 0, 2.0));

							AddPriorityTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, nullptr, iBestStructureToBuild));

							switch (iBestStructureToBuild)
							{
							case ALIEN_BUILD_OFFENSE_CHAMBER:
								AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_OFFENSE_CHAMBER_RESOURCES));
								break;
							case ALIEN_BUILD_DEFENSE_CHAMBER:
								AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_DEFENSE_CHAMBER_RESOURCES));
								break;
							case ALIEN_BUILD_SENSORY_CHAMBER:
								AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_SENSORY_CHAMBER_RESOURCES));
								break;
							case ALIEN_BUILD_MOVEMENT_CHAMBER:
								AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_MOVEMENT_CHAMBER_RESOURCES));
							default: ;
							}

							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
							m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(30.0, 40.0));
							break;
						}
					case BOT_CAN_HEAL:
						AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestHealablePlayer));

						if (UTIL_EntityIsHive(pNearestHealablePlayer))
						{
							AddTask(CBotTask(BOT_TASK_MOVE_TO_VECTOR, iNewScheduleId, nullptr, 0, 32.0, UTIL_GetGroundVector(pNearestHealablePlayer) + Vector(0, 0, pev->size.z / 2)));
						}

						AddTask(CBotTask(BOT_TASK_HEAL_PLAYER, iNewScheduleId, pNearestHealablePlayer));
						break;
					}
				}
				else
				{
					CActionUtilities buildactions;

					/*float fNumHives = 1.0f - (float)UTIL_GetNumHives()/3;
					float fHiveUtility = ((float)m_iResources/NS_HIVE_RESOURCES);
					float fResTowerUtility = ((float)m_iResources/NS_RESOURCE_TOWER_RESOURCES);
					float fHealHealthUtility = 0.0f;

					  if ( fResTowerUtility > 1.0f )
					  fResTowerUtility = 1.0f;

						//fResTowerUtility *= 0.75f;

						  if ( fHiveUtility > 1.0f )
						  fHiveUtility = 1.0f;

							fHiveUtility *= fNumHives;
							fResTowerUtility *= (fNumResourceTowers*0.7);
					*/
					if (fHiveResources == 0 && UTIL_GetNumHives() < 3)
						fHiveResources = gBotGlobals.m_fHiveImportance / (UTIL_GetNumHives() + 1);
					if (fResTowerResources == 0)
						fResTowerResources = gBotGlobals.m_fResTowerImportance / (UTIL_CountEntities("alienresourcetower") + 1);

					actions.add(BOT_CAN_BUILD_HIVE, iNumAlienResourceTowers && !gBotGlobals.IsCombatMap(), fHiveResources);//fHiveUtility);
					actions.add(BOT_CAN_BUILD_RESOURCE, !gBotGlobals.IsCombatMap(), fResTowerResources);//fResTowerUtility);

					action = actions.getBestAction();
					int iRand = 0;

					if (action == BOT_CAN_NONE)
						iRand = RANDOM_LONG(1, 2);
					/*
					int iRand;
					// capture 3 resource towers before attempting hive
					if ( iNumResourceTowers >= 3 )
						iRand = RANDOM_LONG(0,1);
					else
						iRand = 1;
*/
// Find un-built Hives/resource fountains
					if (action == BOT_CAN_BUILD_HIVE || iRand == 1)//iRand == 0 )
					{
						if (UTIL_GetNumHives() < 3)
						{
							edict_t* pHive = UTIL_GetRandomUnbuiltHive();

							if (pHive)
							{
								AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pHive));
								AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, pHive, NS_HIVE_RESOURCES));
								AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pHive, ALIEN_BUILD_HIVE));

								m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
								m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(55.0, 65.0));
							}
						}
					}
					else// if ( (action == BOT_CAN_BUILD_RESOURCE) || (iRand == 2) )
					{
						edict_t* pFuncResource = UTIL_FindRandomUnusedFuncResource(this);

						if (pFuncResource)
						{
							AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pFuncResource));
							AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, pFuncResource, NS_RESOURCE_TOWER_RESOURCES));
							AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pFuncResource, ALIEN_BUILD_RESOURCES));

							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
							m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(70.0, 80.0));
						}
					}
				}

				// restore memory
				//iThingsICanDo.Clear();
			}
			else if (IsFade())
			{
				if (pev->health < pev->max_health)
				{
					AddPriorityTask(CBotTask(BOT_TASK_HEAL_PLAYER, 0, m_pEdict));
					break;
				}
			}

			if (!m_pLastEnemy && m_fAttemptEvolveTime < gpGlobals->time)
			{
				if (bNotCombatMap && !m_bHasGestated)
				{
					int iRand = RANDOM_LONG(0, 100);

					// Not enough gorges on team?
					BOOL bGoGorge = TRUE;

					if (UTIL_SpeciesOnTeam(AVH_USER3_ALIEN_PLAYER2) >= (int)(UTIL_PlayersOnTeam(TEAM_ALIEN) *
						gBotGlobals.m_fGorgeAmount))
						bGoGorge = FALSE;
					else if (this->m_Profile.m_GorgePercent > 0)
						bGoGorge = TRUE;

					if (bGoGorge)
					{
						if (m_iResources < NS_GORGE_RESOURCES)
							bGoGorge = FALSE;
					}

					if (bGoGorge /*&& !gBotGlobals.IsCombatMap()*/)
					{
						AddPriorityTask(CBotTask(BOT_TASK_ALIEN_EVOLVE, 0, nullptr, ALIEN_LIFEFORM_TWO));
						m_fAttemptEvolveTime = gpGlobals->time + 1.0;
						break;
					}
					else
					{
						// pick a new species to go..
						if (m_iResources >= NS_ONOS_RESOURCES && iRand <= this->m_Profile.m_OnosPercent)
						{
							AddPriorityTask(CBotTask(BOT_TASK_ALIEN_EVOLVE, 0, nullptr, ALIEN_LIFEFORM_FIVE));
							m_fAttemptEvolveTime = gpGlobals->time + 1.0;
							break;
						}
						else if (m_iResources >= NS_FADE_RESOURCES && iRand <= this->m_Profile.m_FadePercent)
						{
							AddPriorityTask(CBotTask(BOT_TASK_ALIEN_EVOLVE, 0, nullptr, ALIEN_LIFEFORM_FOUR));
							m_fAttemptEvolveTime = gpGlobals->time + 1.0;
							break;
						}
						else if (m_iResources >= NS_LERK_RESOURCES && iRand <= this->m_Profile.m_LerkPercent)
						{
							AddPriorityTask(CBotTask(BOT_TASK_ALIEN_EVOLVE, 0, nullptr, ALIEN_LIFEFORM_THREE));
							m_fAttemptEvolveTime = gpGlobals->time + 1.0;
							break;
						}
					}
				}

				// do some upgrading
				// ...
				if (gBotGlobals.m_bCanUpgradeDef)
				{
					if (!HasUpgraded(BOT_UPGRADE_DEF))
					{
						AddPriorityTask(CBotTask(BOT_TASK_ALIEN_UPGRADE, 0, nullptr, BOT_UPGRADE_DEF));
						m_fAttemptEvolveTime = gpGlobals->time + 1.0;
						break;
					}
				}

				if (gBotGlobals.m_bCanUpgradeMov)
				{
					if (!HasUpgraded(BOT_UPGRADE_MOV))
					{
						AddPriorityTask(CBotTask(BOT_TASK_ALIEN_UPGRADE, 0, nullptr, BOT_UPGRADE_MOV));
						m_fAttemptEvolveTime = gpGlobals->time + 1.0;
						break;
					}
				}

				if (gBotGlobals.m_bCanUpgradeSens)
				{
					if (!HasUpgraded(BOT_UPGRADE_SEN))
					{
						AddPriorityTask(CBotTask(BOT_TASK_ALIEN_UPGRADE, 0, nullptr, BOT_UPGRADE_SEN));
						m_fAttemptEvolveTime = gpGlobals->time + 1.0;
						break;
					}
				}
			}

			bRoam = m_Tasks.NoTasksLeft();
		}
		break;
	case MOD_HL_RALLY:
	{
		int iWpt;

		// get to the end
		if (m_iCurrentWaypointIndex != -1)
		{
			if (WaypointFlags(m_iCurrentWaypointIndex) & W_FL_ENDLEVEL)
			{
				iWpt = WaypointFindNearestGoal(pev->origin, m_pEdict, 4096.0, -1, W_FL_IMPORTANT, &m_FailedGoals);

				if (m_iCurrentWaypointIndex != iWpt && iWpt != -1)
				{
					AddTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iWpt, -1));
					break;
				}
			}
		}

		iWpt = WaypointFindNearestGoal(pev->origin, m_pEdict, 4096.0, -1, W_FL_ENDLEVEL, &m_FailedGoals);

		if (m_iCurrentWaypointIndex != iWpt && iWpt != -1)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iWpt, -1));
			break;
		}
	}
	break;
	case MOD_DMC:
		// need health and a health waypoint nearby ?
		if (bNeedHealth)
		{
			int iWpt = WaypointFindNearestGoal(pev->origin, m_pEdict, 2000.0, -1, W_FL_HEALTH, &m_FailedGoals);

			if (m_iCurrentWaypointIndex != iWpt && iWpt != -1)
			{
				AddTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iWpt, -1));
				break;
			}
		}
		// something I can pick up?
		if (pNearestPickupEntity)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestPickupEntity));
			AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, iNewScheduleId, pNearestPickupEntity));
			break;
		}
		bRoam = TRUE;
		break;
	default:
		bRoam = TRUE;
	}

	//  if ( m_Tasks.NoTasksLeft() )
	//  {
	//      AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,NULL,-1,-1));
	//  }

	// nothing specific I can do... ROAM ... ("rambo"!)
	// If I an roam around...
	if (bRoam)
	{
		Vector vPos;

		// in a squad, follow leader.
		if (m_pSquadLeader)
		{
			AddTask(CBotTask(BOT_TASK_FOLLOW_LEADER, iNewScheduleId));
		}
		else
		{
			//int iIndex;
			m_bLookingForEnemy = FALSE;

			// flip a coin between finding enemy or doing something else constructive (random_long(0,1)
			BOOL bCheckEnemy = RANDOM_LONG(0, 1) && (!IsGorge() && m_vRememberedPositions.gotPosition());

			if (bCheckEnemy)
			{
				if (gBotGlobals.IsMod(MOD_TFC))
				{
					if (m_bHasFlag)
						bCheckEnemy = FALSE;
					else
					{
						switch (pev->playerclass)
						{
						case TFC_CLASS_SNIPER:
							bCheckEnemy = RANDOM_LONG(0, 4) == 0;
							break;
						case TFC_CLASS_ENGINEER:
							if (getSentry() && getSentry()->v.euser1 && UTIL_SentryLevel(getSentry()) < 3)
								bCheckEnemy = FALSE;
							else
								bCheckEnemy = RANDOM_LONG(0, 4) == 0;
							break;
						case TFC_CLASS_SCOUT:
							bCheckEnemy = FALSE;
							break;
						case TFC_CLASS_MEDIC:
							bCheckEnemy = RANDOM_LONG(0, 2) == 0;
							break;
						}
					}
				}
			}

			// iIndex determines the enemy position I have stored
			// if -1 it is invalid

			// dont look for enemy positions as a gorge...
			/*if (( gBotGlobals.IsNS() ) && (IsGorge()) )
			iIndex = -1; // make invalid
			else
			iIndex = RANDOM_LONG(0,(int)m_iEnemyPositions) - 1;
			*/
			if (!bCheckEnemy) //iIndex == -1 )
			{
				// mod specific stuff again...
				switch (gBotGlobals.m_iCurrentMod)
				{
				case MOD_DMC:
				case MOD_HL_DM:
				{
					// go for a new weapon
					int iWpt = WaypointFindRandomGoal(m_pEdict, -1, W_FL_WEAPON, &m_FailedGoals);

					if (iWpt != -1)
					{
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
					}
				}
				break;
				case MOD_BG:
				{
					// go for a capture point
					int iWpt = WaypointFindRandomGoal(m_pEdict, -1, W_FL_ENDLEVEL, &m_FailedGoals);

					bRoam = FALSE;

					if (iWpt != -1)
					{
						// wait for 8 seconds or something for capture
						AddPriorityTask(CBotTask(BOT_TASK_WAIT, iNewScheduleId, nullptr, 0, RANDOM_FLOAT(8.0, 12.0)));
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
					}
				}
				break;
				case MOD_NS:
					// marine specific (go to a hive)
					if (IsMarine())
					{
						if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ABNORMAL_GAME))
						{
							edict_t* pEnt = nullptr;

							while ((pEnt = UTIL_FindEntityByClassname(pEnt, "team_command")) != nullptr)
							{
								if (pEnt->v.team != pev->team)
									break;
							}

							if (pEnt)
							{
								int iWpt = WaypointLocations.NearestWaypoint(EntityOrigin(pEnt), REACHABLE_RANGE, -1, FALSE);

								if (iWpt != -1)
								{
									AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
									break;
								}
							}
						}
						else
						{
							// find a random hive
							edict_t* pHive = BotFunc_FindRandomEntity("team_hive");

							if (pHive)
							{
								int iWpt = WaypointLocations.NearestWaypoint(EntityOrigin(pHive), REACHABLE_RANGE, -1, FALSE);

								if (iWpt != -1)
								{
									AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
								}
							}
						}
					}
					else if (IsAlien())  // go to marine spawn and bite some marine ass :P
					{
						if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ABNORMAL_GAME))
						{
							edict_t* pEnt = nullptr;

							while ((pEnt = UTIL_FindEntityByClassname(pEnt, "team_hive")) != nullptr)
							{
								if (pEnt->v.team != pev->team)
									break;
							}

							if (pEnt)
							{
								int iWpt = WaypointLocations.NearestWaypoint(EntityOrigin(pEnt), REACHABLE_RANGE, -1, FALSE);

								if (iWpt != -1)
								{
									AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
									break;
								}
							}
						}
						else if (gBotGlobals.m_pMarineStart && (!IsGorge() || gBotGlobals.IsCombatMap()) && RANDOM_LONG(0, 100) > 50)
						{
							// goto the marine start to fight some marines...!
							int iWpt = WaypointLocations.NearestWaypoint(EntityOrigin(gBotGlobals.m_pMarineStart), REACHABLE_RANGE, -1, FALSE);

							if (iWpt != -1)
							{
								AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
								break;
							}
						}
					}

					break;
				default:
					break;
				}

				// Nothing else to do.... at all
				if (m_Tasks.NoTasksLeft())
				{
					// Can I make a squad..?
					BOOL bCanMakeSquad = !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_DISABLE_BOT_SQUADS) && (m_fLookForSquadTime < gpGlobals->time&&
						gBotGlobals.m_iCurrentMod != MOD_RC &&
						gBotGlobals.m_iCurrentMod != MOD_RC2 &&
						gBotGlobals.m_iCurrentMod != MOD_BUMPERCARS &&
						gBotGlobals.m_iCurrentMod != MOD_DMC);
						//( gBotGlobals.m_iCurrentMod != MOD_GEARBOX ) && // Support for OP4CTF [APG]RoboCop[CL]

			// if Natural Selction is the MOD..
					if (bCanMakeSquad && gBotGlobals.IsNS())
					{
						if (IsFade())
						{
							// fades with all abilities always attack
							if (pev->iuser4 & MASK_UPGRADE_11 &&
								pev->iuser4 & MASK_UPGRADE_13 &&
								pev->iuser4 & MASK_UPGRADE_15)
							{
								bCanMakeSquad = FALSE;
							}
						}
						else
						{
							// onos' always attack
							// gorges always defend and build
							bCanMakeSquad = !IsOnos() && !IsGorge();
						}
					}

					if (bCanMakeSquad)
					{
						// Get bot to look for other players to group
						AddTask(CBotTask(BOT_TASK_ASSEMBLE_SQUAD, iNewScheduleId));
						m_fLookForSquadTime = gpGlobals->time + RANDOM_FLOAT(10.0, 20.0);
					}
					else
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, -1, -1));
				}
			}
			// Check a remembered position
			else //if ( m_iEnemyPositions > 0 )
			{
				CRememberPosition* position = m_vRememberedPositions.getNewest();
				vPos = position->getVector();
				Vector vSpottedOrigin = vPos;

				int iWpt = WaypointLocations.NearestWaypoint(vPos, REACHABLE_RANGE, -1);

				if (iWpt == -1)
				{
					vPos = position->getVisibleOrigin();
					iWpt = WaypointLocations.NearestWaypoint(vPos, REACHABLE_RANGE, -1);
				}

				if (iWpt != -1)
				{
					BOOL bNormal = TRUE;
					// saw a sentry at this point
					// throw a gren at it from a safe point and attack
					if (position->hasFlags(BOT_REMEMBER_SENTRY))
					{
						//killSentry(position->getEntity());
						// Find nearest cover waypoint to vPOS and from vPOS location
						int iCoverWpt = WaypointLocations.GetCoverWaypoint(vPos, vPos, &m_FailedGoals);

						if (iCoverWpt != -1)
						{
							AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iCoverWpt, -1));
							AddTask(CBotTask(BOT_TASK_THROW_GRENADE, iNewScheduleId, nullptr, 0, 0, vPos));

							if (position->getEntity())
								AddTask(CBotTask(BOT_TASK_NORMAL_ATTACK, iNewScheduleId, position->getEntity()));

							bNormal = FALSE;
						}
					}

					if (bNormal && !m_FailedGoals.IsMember(iWpt))
					{
						// find most recent spot

						if (vSpottedOrigin != vPos)
						{
							AddPriorityTask(CBotTask(BOT_TASK_WAIT_AND_FACE_VECTOR, iNewScheduleId, nullptr, 0, 1.0, vSpottedOrigin));
						}

						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, iWpt, -1));
						m_bLookingForEnemy = TRUE;
					}
				}

				/*
				// Look for a place where we saw an enemy last...
				int iWpt = WaypointLocations.NearestWaypoint(m_vEnemyPositions[iIndex].GetVector(),REACHABLE_RANGE,-1);
				int i = 0;

				  if ( iWpt != -1 )
				  {
				  if ( !m_FailedGoals.IsMember(iWpt) )
				  {
				  AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,iNewScheduleId,NULL,iWpt,-1));
				  m_bLookingForEnemy = TRUE;
				  }
				  }

					i = iIndex;

					  // Remove this position from the list so we dont
					  // keep returning to the same places
					  m_vEnemyPositions[i].SetVector(Vector(0,0,0));

						// so we dont go out bounds of array here
						m_iEnemyPositions--;

						  // moving them down a slot
						  while ( i < m_iEnemyPositions )
						  {
						  m_vEnemyPositions[i].SetVector(m_vEnemyPositions[i+1].GetVector());
						  i++;
						  }

							// free the rest
							while ( i < MAX_REMEMBER_POSITIONS )
				m_vEnemyPositions[i++].SetVector(Vector(0,0,0));*/
			}

			// totally lost now...
			if (m_Tasks.NoTasksLeft())
			{
				// go anywhere, i don't care!
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, -1, -1));
			}

			if (m_bLookingForEnemy)
			{
				// Have a look for an enemy there since we are going to an enemy position
				AddTask(CBotTask(BOT_TASK_SEARCH_FOR_ENEMY, iNewScheduleId));

				if (gBotGlobals.IsNS())
				{
					if (IsMarine() && HasWeapon(NS_WEAPON_MINE))
						AddTask(CBotTask(BOT_TASK_DEPLOY_MINES, iNewScheduleId, nullptr, 0, 0, Vector(0, 0, 0), 10.0));
				}
			}
		}

		if (IsGorge() && hasWeb())
			AddTask(CBotTask(BOT_TASK_WEB, iNewScheduleId)); // do a little webbing
	}
	// End bLookForNewTasks
}

BOOL CBot::UpdateVisibles(void)
//
// Update the bots visible list with the entities it can see
//
{
	unsigned char* pvs;
	unsigned char* pas;

	Vector vOrigin;

	//
	// static variables for concurrency
	//

	static edict_t* pNearestBuildable = nullptr;
	static float fNearestBuildableDist = 0;

	// Lots of stuff for quick bot reactions

	static float fDistance = 0;
	static float fNearestAvoidEntity = 0;
	static float fPlayerAvoidDist = 0;
	static BOOL bLookForBuildable = FALSE;

	static edict_t* pNearestResourceFountain = nullptr;
	static float fNearestResourceFountainDist = 0;

	static edict_t* pNearestBuildEdict = nullptr;
	static float fNearestBuildPositionDist = 0;

	static int iToBuild = 0;
	static int iBestStructureToBuild = 0;
	static edict_t* pTeleporter = nullptr;
	float fNearestTeleporter = 0.0;

	BOOL bIsTFC = gBotGlobals.IsMod(MOD_TFC);
	const BOOL bIsNS = gBotGlobals.IsNS();
	const BOOL bIsGorge = bIsNS && IsGorge();
	BOOL bSeeLeader = FALSE;
	edict_t* pNearestSentry = nullptr;
	float fNearestSentry = 0;
	// dont go back to leader if checking out hive / structure
	const BOOL bNeedToFollowLeader = !m_Tasks.HasTask(BOT_TASK_FOLLOW_LEADER) && m_pSquadLeader &&
		!m_Tasks.HasSchedule(BOT_SCHED_NS_CHECK_HIVE) &&
		!m_Tasks.HasSchedule(BOT_SCHED_NS_CHECK_STRUCTURE);

	static edict_t* pNearestHive = nullptr;
	static float fNearestHiveDist = 0;
	int iNeededmetal = 0;

	// used for setting up engine visibility
	pvs = nullptr;
	pas = nullptr;

	BOOL bWorkEnemyCosts = FALSE;

	// initialise

	// only build if not currently trying to build
	// [update each time]
	bLookForBuildable = !m_Tasks.HasSchedule(BOT_SCHED_BUILD) &&
		(
			(bIsGorge || IsMarine()) && gBotGlobals.IsNS());

	// Trying to do a bit of the list each frame so keep info until we've
	// searched everything...

	// Bot restarting visible check from beginning of entity list
	if (m_iVisUpdateRevs == 0)
	{
		// free all the objects in the current visible list.
		//m_stBotVisibles.Destroy();
		m_iVisUpdateRevs = 1; // start from 1
		fDistance = 0;
		m_pAvoidEntity = nullptr;

		fPlayerAvoidDist = pev->size.Length2D() * 3;

		// Remove conditions related to seeing.
		RemoveCondition(BOT_CONDITION_SEE_ENEMY);
		RemoveCondition(BOT_CONDITION_SEE_ENEMY_HEAD);

		pNearestBuildable = nullptr;
		fNearestBuildableDist = 0;
		pNearestResourceFountain = nullptr;
		fNearestResourceFountainDist = 0;
		pNearestBuildEdict = nullptr;
		fNearestBuildPositionDist = 0;

		pNearestHive = nullptr;
		fNearestHiveDist = 0;

		pTeleporter = nullptr;

		iToBuild = 0;
		iBestStructureToBuild = 0;

		if (bLookForBuildable)
			m_fLookForBuildableTime = gpGlobals->time + 1.0;

		TraceResult tr;

		UTIL_MakeVectors(pev->v_angle);

		const Vector v_src = GetGunPosition();

		UTIL_TraceLine(v_src, v_src + gpGlobals->v_forward * 768, dont_ignore_monsters, dont_ignore_glass, m_pEdict, &tr);

		m_fDistanceFromWall = (tr.vecEndPos - v_src).Length();

		bWorkEnemyCosts = TRUE;

		clearEnemyCosts();
	}

	m_CurrentTask = m_Tasks.CurrentTask();

	if (bIsNS)
	{
		// If NS, is a marine and standing on an infantry portal, move
		// out of its way, marines will spawn here...
		if (IsMarine() && pev->groundentity)
		{
			if (pev->groundentity->v.iuser3 == AVH_USER3_INFANTRYPORTAL)
				m_pAvoidEntity = pev->groundentity;
		}
	}

	// Setup the bots visibility, for quick vis checking
	(*gFunctionTable.pfnSetupVisibility)(nullptr, m_pEdict, &pvs, &pas);

	// the maximum entity index we will look at this run
	int iMax = m_iVisUpdateRevs + m_Profile.m_iVisionRevs;//gBotGlobals.m_iMaxVisUpdateRevs;

	// if it's bigger than the max allowed then cap it.
	if (iMax > gpGlobals->maxEntities)
		iMax = gpGlobals->maxEntities;

	BOOL bFinished = FALSE;
	//return TRUE;

	CRememberPosition* pos = m_vRememberedPositions.positionNearest(m_vGoalOrigin.GetVector(), pev->origin);

	if (pos != nullptr)
	{
		// quick visible checking
		//pvs = ENGINE_SET_PVS ( (float *)&(pos->getVector()) ); // No longer required? [APG]RoboCop[CL]

		if (ENGINE_CHECK_VISIBILITY(m_pEdict, pvs))
		{
			m_vNearestRememberPoint.SetVector(pos->getVector());
			m_bNearestRememberPointVisible = TRUE;
		}

		m_bNearestRememberPointVisible = FALSE;
	}
	else
	{
		m_bNearestRememberPointVisible = FALSE;
	}

	// search through the entities we want
	for (int i = m_iVisUpdateRevs; i <= iMax; i++)
	{
		edict_t* pEntity = INDEXENT(i);

		if (!pEntity)
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}
		if (pEntity->free)
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}

		if (FNullEnt(pEntity))
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}
		// it's invisible?
		if (pEntity->v.effects & EF_NODRAW)
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}

		vOrigin = EntityOrigin(pEntity);

		fDistance = DistanceFrom(vOrigin);

		if (fDistance > BOT_VIEW_DISTANCE)
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}

		BOOL bCheckInFOV = TRUE;

		if (bIsGorge)
		{
			// make gorges always see hive
			bCheckInFOV = pEntity->v.iuser3 != AVH_USER3_HIVE;
		}
		else if (bIsNS && pEntity->v.iuser3 == AVH_USER3_BREAKABLE)
			bCheckInFOV = FALSE;

		if (bCheckInFOV)
		{
			// use FInViewCone to check if the object in the view cone
			if (!FInViewCone(&vOrigin))
			{
				m_pVisibles->setVisible(i, FALSE);
				continue;
			}
		}

		// quick visible checking
		pvs = ENGINE_SET_PVS(reinterpret_cast<float*>(&vOrigin));

		if (!ENGINE_CHECK_VISIBILITY(m_pEdict, pvs))
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}

		// Use the bots vision to advantage
		// and add the structures it sees in NS to the hivemind
		if (bIsNS && EntityIsAlienStruct(pEntity))
			gBotGlobals.m_HiveMind.AddStructure(pEntity, i);

		// finally run a traceline
		if (!FVisible(pEntity))
		{
			m_pVisibles->setVisible(i, FALSE);
			continue;
		}

		// I see my enemy
		if (pEntity == m_pEnemy)
		{
			// update the condition
			UpdateCondition(BOT_CONDITION_SEE_ENEMY);
		}

		if ((!m_pAvoidEntity || fDistance < fNearestAvoidEntity) && !(m_iCurrentWaypointFlags & W_FL_JUMP))
		{
			if (CanAvoid(pEntity, fDistance, fPlayerAvoidDist) && m_fAvoidTime < gpGlobals->time)
			{
				fNearestAvoidEntity = fDistance;
				// something to avoid walking into
				m_pAvoidEntity = pEntity;
			}
		}

		// Only search for things to do in this function if bot already has a task
		// if it doesnt have a task it will check when tasks are empty anyway
		if (m_CurrentTask && bLookForBuildable)
		{
			// update entity pev in here, not used elsewhere
			entvars_t* pEntitypev = &pEntity->v;

			if (CanBuild(pEntity, &iNeededmetal))
			{
				if (!pNearestBuildable || fDistance < fNearestBuildableDist)
				{
					fNearestBuildableDist = fDistance;
					pNearestBuildable = pEntity;
				}
			}
			// don't build in combat map, man
			else if (bIsGorge && !gBotGlobals.IsCombatMap())
			{
				// Check buildings I see
				if (m_fNextBuildTime < gpGlobals->time && (!pNearestBuildEdict || fDistance < fNearestBuildPositionDist))
				{
					iToBuild = BotFunc_GetStructureForGorgeBuild(pev, &pEntity->v);

					if (iToBuild)
					{
						iBestStructureToBuild = iToBuild;
						pNearestBuildEdict = pEntity;
						fNearestBuildPositionDist = fDistance;
					}
				}

				if (pEntitypev->iuser3 == AVH_USER3_HIVE && (!pNearestHive || fDistance < fNearestHiveDist))
				{
					pNearestHive = pEntity;
					fNearestHiveDist = fDistance;
				}

				if (pEntity->v.iuser3 == AVH_USER3_FUNC_RESOURCE && (!pNearestResourceFountain || fDistance < fNearestResourceFountainDist))
				{
					if (!HasVisitedResourceTower(pEntity))
					{
						pNearestResourceFountain = pEntity;
						fNearestResourceFountainDist = fDistance;
					}
				}
			}
			else if (IsMarine() && !gBotGlobals.IsCombatMap() && gBotGlobals.IsConfigSettingOn(BOT_CONFIG_WAIT_AT_RESOURCES))
			{
				if (pEntity->v.iuser3 == AVH_USER3_FUNC_RESOURCE && (!pNearestResourceFountain || fDistance < fNearestResourceFountainDist))
				{
					pNearestResourceFountain = pEntity;
					fNearestResourceFountainDist = fDistance;
				}
			}
		}

		// Update the fact that I saw my squad leader
		if (!bSeeLeader && m_pSquadLeader == pEntity)
			bSeeLeader = TRUE;

		m_pVisibles->setVisible(i, TRUE);

		//m_stBotVisibles.Push(pEntity);

		if (bWorkEnemyCosts)
			workEnemyCosts(pEntity, vOrigin, fDistance);
	}

	// Update the start of the entitiy list check for next run
	if (iMax >= gpGlobals->maxEntities)
	{
		m_iVisUpdateRevs = 0;
		bFinished = TRUE;
	}
	else
		m_iVisUpdateRevs = iMax + 1;

	//int iNumResourceTowers = UTIL_CountEntities("alienresourcetower");

	// possible tasks we found by seeing stuff
	if (bLookForBuildable && !gBotGlobals.IsCombatMap())
	{
		if (gBotGlobals.IsNS())
		{
			CActionUtilities actions;

			const int iNumResourceFountains = UTIL_CountEntities("func_resource");

			const int iNumAlienResourceTowers = UTIL_CountEntities("alienresourcetower");
			float fNumResourceTowers = 1.0f;

			if (iNumResourceFountains > 0)
				fNumResourceTowers = 1.0f - (float)iNumAlienResourceTowers / iNumResourceFountains;
			/*
			gBotGlobals.m_fHiveImportance = 1.0f;
			gBotGlobals.m_fResTowerImportance = 0.7f;
			gBotGlobals.m_fHealingImportance = 0.75f;
			gBotGlobals.m_fStructureBuildingImportance = 0.4f;
			gBotGlobals.m_fRefillStructureImportance = 0.5f;
			*/
			//-au1
			float fHiveResources = (float)m_iResources / NS_HIVE_RESOURCES;
			float fStructResources = (float)m_iResources / (NS_DEFENSE_CHAMBER_RESOURCES * 2);
			float fResTowerResources = (float)m_iResources / NS_RESOURCE_TOWER_RESOURCES;
			float fBuildableHealth = 0.0f;

			if (fStructResources > 1.0f)
				fStructResources = 1.0f;

			fStructResources *= gBotGlobals.m_fStructureBuildingImportance;//0.4f;

			if (fResTowerResources > 1.0f)
				fResTowerResources = 1.0f;

			fResTowerResources *= fNumResourceTowers * gBotGlobals.m_fResTowerImportance;//0.6;//0.75f;

			if (fHiveResources > 1.0f)
				fHiveResources = 1.0f;

			fHiveResources *= gBotGlobals.m_fHiveImportance;

			if (pNearestBuildable)
				fBuildableHealth = 1.0f - pNearestBuildable->v.fuser1 / 1000;

			fBuildableHealth *= gBotGlobals.m_fRefillStructureImportance;

			actions.add(BOT_CAN_BUILD_HIVE, iNumAlienResourceTowers && !gBotGlobals.IsCombatMap() && bIsGorge && pNearestHive && UTIL_CanBuildHive(&pNearestHive->v), fHiveResources);
			actions.add(BOT_CAN_REFILL_STRUCT, !gBotGlobals.IsCombatMap() && pNearestBuildable != nullptr, fBuildableHealth);
			actions.add(BOT_CAN_BUILD_STRUCT, iNumAlienResourceTowers && !gBotGlobals.IsCombatMap() && bIsGorge && iBestStructureToBuild, fStructResources);
			actions.add(BOT_CAN_BUILD_RESOURCE, !gBotGlobals.IsCombatMap() && bIsGorge && pNearestResourceFountain, fResTowerResources);

			const eCanDoStuff action = actions.getBestAction();
			/*
						dataUnconstArray<eCanDoStuff> iThingsICanDo;

						if ( bIsGorge && pNearestHive )
						{
							if ( UTIL_CanBuildHive(&pNearestHive->v) )
							{
								iThingsICanDo.Add(BOT_CAN_BUILD_HIVE);
							}
							//see if hive is undefended...
						}

						if ( pNearestBuildable )
						{
							iThingsICanDo.Add(BOT_CAN_REFILL_STRUCT);
							//m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId,60.0);
						}
						else if ( bIsGorge )
						{
							//			if ( iNumResourceTowers >= 2 )
							//			{
							if ( iBestStructureToBuild != 0 )
							{
								iThingsICanDo.Add(BOT_CAN_BUILD_STRUCT);
							}
							//			}

							if ( pNearestResourceFountain )
							{
								if ( !UTIL_FuncResourceIsOccupied(pNearestResourceFountain) )
								{
									iThingsICanDo.Add(BOT_CAN_BUILD_RESOURCE);
								}
							}
						}

						if ( !iThingsICanDo.IsEmpty() )
						{*/
			const int iNewScheduleId = m_Tasks.GetNewScheduleId();

			switch (action)// iThingsICanDo.Random() )
			{
			case BOT_CAN_BUILD_HIVE:
				AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_HIVE_RESOURCES));
				AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pNearestHive, ALIEN_BUILD_HIVE));

				m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
				m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(55.0, 65.0));
				break;
			case BOT_CAN_BUILD_RESOURCE:
				AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestResourceFountain, NS_RESOURCE_TOWER_RESOURCES));
				AddTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_RESOURCE_TOWER_RESOURCES));
				AddTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, pNearestResourceFountain, ALIEN_BUILD_RESOURCES));

				m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
				break;
			case BOT_CAN_REFILL_STRUCT: // already built but needs health
				m_Tasks.AddTaskToFront(CBotTask(BOT_TASK_BUILD, iNewScheduleId, pNearestBuildable));
				m_Tasks.AddTaskToFront(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestBuildable));
				m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
				break;
			case BOT_CAN_BUILD_STRUCT:
				if (fNearestBuildPositionDist < 85) // too close
				{
					// avoid the object first
					AddPriorityTask(CBotTask(BOT_TASK_AVOID_OBJECT, iNewScheduleId, pNearestBuildEdict, 0, 2.0));
				}

				AddPriorityTask(CBotTask(BOT_TASK_BUILD_ALIEN_STRUCTURE, iNewScheduleId, nullptr, iBestStructureToBuild));

				switch (iBestStructureToBuild)
				{
				case ALIEN_BUILD_OFFENSE_CHAMBER:
					AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_OFFENSE_CHAMBER_RESOURCES));
					break;
				case ALIEN_BUILD_DEFENSE_CHAMBER:
					AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_DEFENSE_CHAMBER_RESOURCES));
					break;
				case ALIEN_BUILD_SENSORY_CHAMBER:
					AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_SENSORY_CHAMBER_RESOURCES));
					break;
				case ALIEN_BUILD_MOVEMENT_CHAMBER:
					AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCES, iNewScheduleId, nullptr, NS_MOVEMENT_CHAMBER_RESOURCES));
				default: ;
				}

				m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_BUILD);
				m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(55.0, 65.0));
				break;
			default:
				if (!EntityIsCommander(nullptr) && pNearestResourceFountain && IsMarine() && !gBotGlobals.IsCombatMap() && gBotGlobals.IsConfigSettingOn(BOT_CONFIG_WAIT_AT_RESOURCES))
				{
					if (!UTIL_FuncResourceIsOccupied(pNearestResourceFountain) && !m_Tasks.HasTask(BOT_TASK_WAIT_FOR_RESOURCE_TOWER_BUILD))
					{
						//AddPriorityTask(CBotTask(BOT_TASK_IMPULSE,iNewScheduleId,NULL,(int)ORDER_REQUEST));
						AddPriorityTask(CBotTask(BOT_TASK_WAIT_FOR_RESOURCE_TOWER_BUILD, iNewScheduleId, pNearestResourceFountain));
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pNearestResourceFountain));
					}
				}
			}
			/*	}

				// Free memory used
				iThingsICanDo.Clear();*/
		}
	}

	// Ohh... can I see my squad leader and I lost sight of him earlier
	// begin to follow him again.
	if (bSeeLeader && bNeedToFollowLeader)
	{
		// make sure he didn't suddenly turn against him
		if (!IsEnemy(m_pSquadLeader))
		{
			AddPriorityTask(CBotTask(BOT_TASK_FOLLOW_LEADER, m_Tasks.GetNewScheduleId(), m_pSquadLeader));

			// re-say "in position" when he gets back in position of the squad
			m_bSaidInPosition = FALSE;
		}
	}

	if (pTeleporter)
	{
		const int iSched = m_Tasks.GetNewScheduleId();

		AddPriorityTask(CBotTask(BOT_TASK_USE_TELEPORTER, iSched, pTeleporter->v.euser1, (int)pTeleporter, 0,
			pTeleporter->v.origin));
		AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iSched, pTeleporter));

		m_Tasks.SetTimeToCompleteSchedule(iSched, 15.0);
	}

	if (pNearestSentry && !m_Tasks.HasTask(BOT_TASK_TFC_DISCARD))
	{
		AddPriorityTask(CBotTask(BOT_TASK_TFC_DISCARD, 0, nullptr, 0, 0, pNearestSentry->v.origin));
	}

	return bFinished;
}

BOOL CBot::CanAvoid(edict_t* pEntity, const float fDistanceToEntity, const float fAvoidDistance) const
// return
{
	assert(pEntity != NULL);

	// if the bot is walking along a func_wall or something, ignore it.
	if (pev->groundentity == pEntity)
		return FALSE;

	// Reliability checking...
	if (pEntity == nullptr)
		return FALSE;
	// dont avoid yourself!
	if (pEntity == m_pEdict)
		return FALSE;
	// cant see it...
	if (pEntity->v.effects & EF_NODRAW)
		return FALSE;
	if (m_stSquad)
	{
		// dont avoid squad members
		if (m_stSquad->IsMember(pEntity))
		{
			return FALSE;
		}
	}
	if (pEntity == m_pEnemy && m_pCurrentWeapon && m_pCurrentWeapon->IsMelee())
	{
		// trying to melee an enemy (Don't avoid, I want to hit it, man!)
		return FALSE;
	}

	char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));

	const BOOL bIsSvenCoop = gBotGlobals.IsMod(MOD_SVENCOOP);
	// check for grenades.

	if (bIsSvenCoop || gBotGlobals.IsMod(MOD_HL_DM))
	{
		//	if ( bIsSvenCoop )
		//	{
				//if ( pEntity->v.movetype != MOVETYPE_WALK ) // ? ?why was this here?
				//{
				//BOOL bAvoid = TRUE;

				// NOT a weapon or ammo (strncmp will return -1 or 1)... save time!
		if (strncmp(szClassname, "weapon_", 7) &&
			strncmp(szClassname, "ammo_", 5))
		{
			// oh crap, a grenade!
			if (UTIL_IsGrenadeRocket(pEntity))//strstr(szClassname,"grenade") != NULL )
			{
				if (fDistanceToEntity > 512)
					return FALSE;

				TraceResult tr;

				UTIL_TraceLine(pEntity->v.origin, pEntity->v.origin + pEntity->v.velocity * 3 - Vector(0, 0, 200), ignore_monsters, dont_ignore_glass, m_pEdict, &tr);

				if (DistanceFrom(tr.vecEndPos) < EXPLODE_RADIUS)
				{
					if (bIsSvenCoop && pEntity->v.owner)
					{
						// Dont avoid a grenade if a player threw it (doesnt work?!)
						return pEntity->v.owner == m_pEdict || !(pEntity->v.owner->v.flags & FL_CLIENT);
					}
				}

				return TRUE;
			}
		}
		//}
//	}

	// NOT a weapon or ammo (strncmp will return -1 or 1)
		if (strncmp(szClassname, "rpg_rocket", 10))
		{
			if (BotFunc_EntityIsMoving(&pEntity->v))
			{
				// rocket coming this way?
				if (DistanceFrom(pEntity->v.origin + pEntity->v.velocity * DistanceFromEdict(pEntity)) < EXPLODE_RADIUS)
				{
					if (bIsSvenCoop && pEntity->v.owner)
					{
						// Dont avoid a grenade if a player threw it (doesnt work?!)
						return (pEntity->v.owner->v.flags & FL_CLIENT) != FL_CLIENT;
					}

					return TRUE;
				}
			}
		}
		//}
	}
	// Too far from me
	if (fDistanceToEntity > fAvoidDistance)
		return FALSE;

	// dont avoid on a ladder
	if (IsOnLadder())
		return FALSE;
	if (m_iCurrentWaypointFlags & W_FL_LADDER)
		return FALSE;

	if (m_CurrentTask)
	{
		if (pEntity == m_CurrentTask->TaskEdict())
			return FALSE;
	}

	// MOD specific...

	// bumper cars stuff atm.
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_BUMPERCARS:
	{
		// hurts and crashes.. TRY to avoid them
		if (strncmp("trigger_", szClassname, 8) == 0)
		{
			if (strcmp("crash", &szClassname[8]) == 0)
				return TRUE;
			else if (strcmp("hurt", &szClassname[8]) == 0)
				return TRUE;
		}
	}
	break;
	case MOD_TS:
	case MOD_TFC:
	{
		if (UTIL_IsGrenadeRocket(pEntity))//strstr(szClassname,"grenade") )
			return TRUE;
	}
	break;
	default:
		break;
	}

	//  is the entity solid or not?
	if (pEntity->v.solid == SOLID_NOT)
		return FALSE;
	// not solid again, can walk through it
	if (pEntity->v.solid == SOLID_TRIGGER)
		return FALSE;

	if (strncmp(szClassname, "func_door", 9) == 0)
		return FALSE; // we want to walk INTO doors

	if (pEntity->v.flags & FL_WORLDBRUSH)
	{
		if (m_iCurrentWaypointIndex != -1)
		{
			if (WaypointDistance(EntityOrigin(pEntity), m_iCurrentWaypointIndex) < 100)
			{
				return FALSE;
			}
		}

		Vector vEntOrigin = EntityOrigin(pEntity);
		// only if in front of me
		return DotProductFromOrigin(&vEntOrigin) > 0.9;
	}

	return TRUE;
}

void CBot::BotSquadLeaderThink()
// what does a bot want to do when leading
// a squad.. hmm.
{
	if (m_CurrentTask)
	{
		// get task
		switch (m_CurrentTask->Task())
		{
			// listening to sound
		case BOT_TASK_LISTEN_TO_SOUND:
		{
			edict_t* pEntity = m_CurrentTask->TaskEdict();

			if (pEntity && IsEnemy(pEntity))
			{
				// go stealth
				m_stSquad->ChangeFormation(SQUAD_FORM_VEE);
				//m_stSquad->SetCombatType(COMBAT_STEALTH);
			}
		}
		break;
		default:
			break;
		}
	}
	return;
}
/*
float CBot :: timeToPositionCrossHair ( Vector vTarget )
{
	float a;
	float b;
	float A;
	float W;

	UTIL_MakeVectors(pev->v_angle);

	Vector ideal_aim = vTarget - GetGunPosition();

	Vector angles = UTIL_VecToAngles(ideal_aim);

	UTIL_MakeVectors();

	a = m_fReactionTime;
	b = 0.96;
	A = Length2D()

	float MT = a + (b*log((2*A)/W));

	return MT;
}*/

Vector CBot::GetAimVector(edict_t* pBotEnemy)
//
// Get the vector we want to aim at to attack the enemy
//
{
	Vector vEnemyOrigin;

	if (gBotGlobals.IsMod(MOD_SVENCOOP) && pBotEnemy->v.flags & FL_MONSTER)
	{
		// shoot cockpit of the osprey

		if (HasCondition(BOT_CONDITION_ENEMY_IS_OSPREY))
			vEnemyOrigin = pBotEnemy->v.origin + pBotEnemy->v.view_ofs;
		else if (!HasCondition(BOT_CONDITION_SEE_ENEMY_BODY) && HasCondition(BOT_CONDITION_SEE_ENEMY_HEAD))
			vEnemyOrigin = pBotEnemy->v.origin + pBotEnemy->v.view_ofs;
		else if (HasCondition(BOT_CONDITION_SEE_ENEMY_HEAD))
		{
			vEnemyOrigin = pBotEnemy->v.origin + pBotEnemy->v.view_ofs;
			vEnemyOrigin.z -= pBotEnemy->v.size.z * 0.1;
		}
		else
			vEnemyOrigin = pBotEnemy->v.origin + pBotEnemy->v.view_ofs / 2;

		if (IsCurrentWeapon(VALVE_WEAPON_RPG))
		{
			UTIL_MakeVectors(pev->v_angle);

			vEnemyOrigin = vEnemyOrigin + pBotEnemy->v.velocity / 2 + gpGlobals->v_forward * RANDOM_FLOAT(0, 16);
		}
	}
	else
	{
		vEnemyOrigin = EntityOrigin(pBotEnemy);
	}

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_RC:
	case MOD_RC2:
		if (m_pCurrentWeapon)
		{
			if (m_pCurrentWeapon->GetID() == VALVE_WEAPON_HANDGRENADE)
			{
				const float zComp = (vEnemyOrigin.z - pev->origin.z) / 2;
				vEnemyOrigin = EntityOrigin(pBotEnemy);
				vEnemyOrigin = vEnemyOrigin + (vEnemyOrigin - pev->origin).Length() / 320 * pBotEnemy->v.velocity;
				vEnemyOrigin.z += zComp;

				// higher than jump distance
				// jump to throw grenade higher
				if (zComp > MAX_JUMP_HEIGHT)
				{
					Jump();
				}
			}
		}
		break;
	case MOD_DMC:
		if (m_pCurrentWeapon)
		{
			// aim at feet
			if (m_pCurrentWeapon->GetID() == DMC_WEAPON_ROCKET1)
			{
				TraceResult tr;

				const float fFootZ = vEnemyOrigin.z - pBotEnemy->v.size.z / 2 + 1.0;

				UTIL_TraceLine(GetGunPosition(), Vector(vEnemyOrigin.x, vEnemyOrigin.y, fFootZ), ignore_monsters, m_pEdict, &tr);

				if (tr.flFraction >= 1.0)
					vEnemyOrigin.z = fFootZ;
			}
		}
		break;
	default:
		break;
	}

	// time to update aim offset
	if (m_fGetAimVectorTime < gpGlobals->time)
	{
		float fSpeed;

		// Store the enemy details, instead of working them out each time
		// Get bot's reputation with this guy...

		if (m_pEnemyRep == nullptr) // Isn't a player (monster or breakable...)
		{
			// Aim factor dependant on skill
			m_fAimFactor = MAX_BOT_SKILL - m_Profile.m_iSkill;
			// get a 0 to 1 representation factor.
			m_fAimFactor /= MAX_BOT_SKILL;

			fSpeed = 0.5;
		}
		else
		{
			const int iRep = m_pEnemyRep->CurrentRep();
			// Aim factor dependant on skill and reputation.
			m_fAimFactor = MAX_BOT_SKILL - m_Profile.m_iSkill + (BOT_MAX_REP - iRep);
			// get a 0 to 1 representation factor.
			m_fAimFactor /= MAX_BOT_SKILL + BOT_MAX_REP;

			fSpeed = (float)iRep / BOT_MAX_REP;

			if (fSpeed == 0)
				fSpeed = 0.1;
		}

		Vector vEnemyFactors = pBotEnemy->v.size / 2 + pBotEnemy->v.velocity;

		// snipers better aim,
		if (gBotGlobals.IsMod(MOD_TFC) && pev->playerclass == TFC_CLASS_SNIPER)
			vEnemyFactors = vEnemyFactors / 3;

		m_fAimFactor *= 1 - m_Profile.m_fAimSkill;

		const float fleX = m_fAimFactor * vEnemyFactors.x;
		const float fleY = m_fAimFactor * vEnemyFactors.y;
		const float fleZ = m_fAimFactor * vEnemyFactors.z;

		const float flX = RANDOM_FLOAT(-fleX, fleX);
		const float flY = RANDOM_FLOAT(-fleY, fleY);
		const float flZ = RANDOM_FLOAT(-fleZ, fleZ);

		m_vOffsetVector = Vector(flX, flY, flZ);

		if (m_bZoom)
			m_vOffsetVector = m_vOffsetVector * 0.5;

		// Wait another while before picking up a new offset vector
		m_fGetAimVectorTime = gpGlobals->time + RANDOM_FLOAT(m_Profile.m_fAimTime - 0.2, m_Profile.m_fAimTime + 0.2) * fSpeed;
	}

	return vEnemyOrigin + m_vOffsetVector;
}

float CBot::DistanceFrom(const Vector& vOrigin, const BOOL twoD) const
{
	// get distance from origin

	if (twoD) // 2 d distance (x and y) only?
		return (vOrigin - GetGunPosition()).Length2D();

	return (vOrigin - GetGunPosition()).Length();
}

float CBot::DistanceFromEdict(edict_t* pEntity) const
{
	// edict distance this time
	return DistanceFrom(EntityOrigin(pEntity));
}

BOOL CBot::FVisible(edict_t* pEntity)
{
	if (pEntity == m_pEnemy)
	{
		UpdateCondition(BOT_CONDITION_SEE_ENEMY_BODY);
		RemoveCondition(BOT_CONDITION_SEE_ENEMY_HEAD);
	}

	const Vector vOrigin = EntityOrigin(pEntity);

	TraceResult tr;

	// draw a Traceline
	UTIL_TraceLine(GetGunPosition(), vOrigin, dont_ignore_monsters, pev->pContainingEntity, &tr);

	// Hit something before the edict
	if (tr.flFraction < 1.0)
	{
		if (tr.pHit == pEntity)
		{
			return TRUE;
		}
		else
		{
			// this is my enemy, and i probably cant see his body area
			if (pEntity == m_pEnemy)
			{
				// condition...
				RemoveCondition(BOT_CONDITION_SEE_ENEMY_BODY);
			}

			// find out if we can see its head then

			// svencoop only
			/*if( (gBotGlobals.IsMod(MOD_SVENCOOP)) && (pEntity->v.flags & FL_MONSTER) )*/
			{
				BOOL bCanSeeHead = TRUE;

				UTIL_TraceLine(GetGunPosition(), pEntity->v.origin + pEntity->v.view_ofs, dont_ignore_monsters, pev->pContainingEntity, &tr);

				if (tr.flFraction < 1.0)
					bCanSeeHead = tr.pHit == pEntity;

				if (bCanSeeHead)
				{
					if (m_pEnemy == pEntity)
					{
						UpdateCondition(BOT_CONDITION_SEE_ENEMY_HEAD);
					}

					return TRUE;
				}
			}
		}
	}
	else
		return TRUE;

	if (pEntity == m_pEnemy)
	{
		// see everything!
		RemoveCondition(BOT_CONDITION_SEE_ENEMY_BODY);
		RemoveCondition(BOT_CONDITION_SEE_ENEMY_HEAD);
		RemoveCondition(BOT_CONDITION_SEE_ENEMY);
	}

	return FALSE;
}

BOOL CBot::FVisible(const Vector& vecOrigin) const
{
	// see if vector is visible, simple traceline ..
	TraceResult tr;

	UTIL_TraceLine(GetGunPosition(), vecOrigin, ignore_monsters, pev->pContainingEntity, &tr);

	return tr.flFraction >= 1.0;
}

BOOL CBot::FInViewCone(Vector* pOrigin) const
{
	if (gBotGlobals.IsMod(MOD_SVENCOOP))
	{
		// Use 2d LOS for svencoop for seeing ospreys etc easier
		return BotFunc_FInViewCone(pOrigin, m_pEdict);
	}

	return DotProductFromOrigin(pOrigin) > 0.5; // 60 degree field of view
}

float CBot::DotProductFromOrigin(Vector* pOrigin) const
{
	static Vector vecLOS;
	static float flDot;

	// in fov? Check angle to edict
	UTIL_MakeVectors(pev->v_angle);

	vecLOS = *pOrigin - GetGunPosition();
	vecLOS = vecLOS.Normalize();

	flDot = DotProduct(vecLOS, gpGlobals->v_forward);

	return flDot;
}

// Update conditions that can be updated regularly
// cannot update enemy damage or heavy damage conditions etc.
void CBot::UpdateConditions(void)
{
	int iWpn;

	//////////////////
	// Squad conditions - do every time a bot thinks...
	if (m_pSquadLeader)
	{
		TraceResult tr;

		UTIL_TraceLine(GetGunPosition(), EntityOrigin(m_pSquadLeader), ignore_monsters, ignore_glass, m_pEdict, &tr);

		if (tr.flFraction >= 1.0)
			UpdateCondition(BOT_CONDITION_SEE_SQUAD_LEADER);
		else
			RemoveCondition(BOT_CONDITION_SEE_SQUAD_LEADER);
	}

	if (m_fLastUpdateConditions > gpGlobals->time)
		return;

	RemoveCondition(BOT_CONDITION_HAS_WEAPON);
	RemoveCondition(BOT_CONDITION_ENEMY_OCCLUDED);
	RemoveCondition(BOT_CONDITION_TASK_EDICT_NA);
	RemoveCondition(BOT_CONDITION_HAS_WEAPON);

	if ((iWpn = m_Weapons.GetBestWeaponId(this, nullptr)) > 0)
	{
		//m_pBestWeaponId = m_Weapons.GetWeapon(iWpn);

		// bot has a weapon!
		UpdateCondition(BOT_CONDITION_HAS_WEAPON);
	}
	//m_pBestWeaponId = NULL;

	if (gBotGlobals.IsNS() && IsMarine())
	{
		m_iPrimaryWeaponId = m_Weapons.GetPrimaryWeaponId();
		m_iSecondaryWeaponId = m_Weapons.GetSecondaryWeaponId();
	}

	// Do i have an enemy
	if (m_pEnemy)
	{
		// can i see it?
		if (!HasCondition(BOT_CONDITION_SEE_ENEMY))
		{
			UpdateCondition(BOT_CONDITION_ENEMY_OCCLUDED);
		}
		else
		{
			// yep? is it dead?
			if (!EntityIsAlive(m_pEnemy))
			{
				// Enemy dead...
				// update time...

				UpdateCondition(BOT_CONDITION_ENEMY_DEAD);
			}
		}
	}

	// Have i got a task//?
	if ((m_CurrentTask = m_Tasks.CurrentTask()) != nullptr)
	{
		// check if task edict is valid
		edict_t* pTaskEdict;

		if ((pTaskEdict = m_CurrentTask->TaskEdict()) != nullptr)
		{
			if (FNullEnt(pTaskEdict))
			{
				// task edict not available anymore
				// i.e. edict died/exploded/whatever a while back
				// and doesnt exist anymore
				UpdateCondition(BOT_CONDITION_TASK_EDICT_NA);
				m_CurrentTask->SetEdict(nullptr);
			}
		}
	}

	// don't update conditions for another while
	m_fLastUpdateConditions = gpGlobals->time + m_fReactionTime;
}

// changes the bot's weapon
BOOL CBot::SwitchWeapon(const int iId)
{
	// already using this weapon?
	// nothing to do
	if (IsCurrentWeapon(iId))
		return TRUE; // "successful"

	if (gBotGlobals.IsMod(MOD_TS) || m_Weapons.HasWeapon(m_pEdict, iId))
	{
		if (gBotGlobals.IsMod(MOD_TS))
		{
			usercmd_t user;

			user.lerp_msec = 0;
			user.msec = 0;
			user.viewangles = pev->v_angle;
			user.forwardmove = 0;
			user.sidemove = 0;
			user.upmove = 0;
			user.lightlevel = 127;
			user.buttons = 0;
			user.impulse = 0;
			user.weaponselect = iId;
			user.impact_index = 0;
			user.impact_position = Vector(0, 0, 0);
			// send a client update command
#ifndef RCBOT_META_BUILD
			CmdStart(m_pEdict, &user, 0);
			CmdEnd(m_pEdict);
#else
			MDLL_CmdStart(m_pEdict, &user, 0);
			MDLL_CmdEnd(m_pEdict);
#endif
			if (gBotGlobals.IsMod(MOD_TS))
			{
				// hack
				m_pCurrentWeapon = m_Weapons.GetWeapon(iId);
				if (m_pCurrentWeapon->GetID() == 0)
					m_pCurrentWeapon->SetWeapon(iId, nullptr);
			}
		}
		// DMC weapon switching
		// MUCH weirder than normal mods.
		else if (gBotGlobals.IsMod(MOD_DMC))
			// Different method of choosing weapon in DMC.
		{
			usercmd_t user;

			user.lerp_msec = 0;
			user.msec = 0;
			user.viewangles = pev->v_angle;
			user.forwardmove = 0;
			user.sidemove = 0;
			user.upmove = 0;
			user.lightlevel = 127;
			user.buttons = 0;
			user.impulse = 0;
			user.weaponselect = iId;
			user.impact_index = 0;
			user.impact_position = Vector(0, 0, 0);
			// send a client update command
#ifndef RCBOT_META_BUILD
			CmdStart(m_pEdict, &user, 0);
			CmdEnd(m_pEdict);
#else
			MDLL_CmdStart(m_pEdict, &user, 0);
			MDLL_CmdEnd(m_pEdict);
#endif
			// hack
			m_pCurrentWeapon = m_Weapons.GetWeapon(iId);
		}
		else
		{
			CBotWeapon* pWeapon;

			// simply see if the weapon exists...
			if ((pWeapon = m_Weapons.GetWeapon(iId)) != nullptr)
			{
				// get its class name from our list
				char* szClassname = pWeapon->GetClassname();

				// crap.. something wrong
				if (szClassname == nullptr)
					BugMessage(nullptr, "CBot::SwitchWeapon() : szClassname NULL (CBotWeapon::GetClassname() returned NULL)");
				else // yay, select our weapon!
					FakeClientCommand(m_pEdict, "%s\n", szClassname);
			}
		}
	}
	else
		return FALSE;

	m_bZoom = FALSE;

	return TRUE;
}

void CBot::WorkViewAngles(void)
{
	m_CurrentTask = m_Tasks.CurrentTask();

	// We have a task for looking
	// what do we want to look at?
	switch (m_CurrentLookTask)
	{
		// nothing...
	case BOT_LOOK_TASK_NONE:
		break;
	case BOT_LOOK_TASK_FACE_NEAREST_REMEMBER_POS:

		if (m_bNearestRememberPointVisible && !m_pEnemy && m_bLookingForEnemy)
		{
			SetViewAngles(m_vNearestRememberPoint.GetVector());
		}
		else
			m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;

		break;
	case BOT_LOOK_TASK_FACE_GROUND:
		SetViewAngles(pev->origin - Vector(0, 0, 4096.0));
		break;
		// our next waypoint?
	case BOT_LOOK_TASK_NEXT_WAYPOINT:
	{
		/*	BOOL bLookAtCurrent = TRUE;
			int iNextwpt;

			if ( (m_iCurrentWaypointIndex != -1) &&
				 ((iNextwpt=GetNextWaypoint()) != -1) )
			{
				if ( (WaypointFlags(m_iCurrentWaypointIndex) == 0) &&
					 (WaypointFlags(iNextwpt) == 0))
				{
					bLookAtCurrent = FALSE;

					SetViewAngles(WaypointOrigin(iNextwpt));
				}
			}

			if ( bLookAtCurrent )*/
		SetViewAngles(m_vMoveToVector);
	}
	break;
	// look around for fun..
	case BOT_LOOK_TASK_LOOK_AROUND:
	{
		// slow looking speed
		//m_fTurnSpeed = 0.8;

		if (m_fLastLookTime < gpGlobals->time)
		{
			m_fLastLookTime = gpGlobals->time + RANDOM_FLOAT(2.0, 5.0);

			float fIdealYaw = pev->ideal_yaw + (RANDOM_FLOAT(0, 180) - 90);

			UTIL_FixFloatAngle(&fIdealYaw);

			UTIL_MakeVectors(Vector(0, fIdealYaw, 0));

			m_vCurrentLookDir = GetGunPosition() + gpGlobals->v_forward * 64;
		}

		SetViewAngles(m_vCurrentLookDir);
	}
	break;
	// look about the same area we saw an enemy
	case BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY:

		if (m_fSearchEnemyTime < gpGlobals->time)
		{
			m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
			break;
		}

		if (m_pLastEnemy == nullptr)
		{
			m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_ENEMY;
		}
		else
		{
			SetViewAngles(m_vLastSeeEnemyPosition.GetVector());
		}
		break;
		// look around-ish
	case BOT_LOOK_TASK_SEARCH_FOR_ENEMY:

		if (m_fSearchEnemyTime < gpGlobals->time)
		{
			m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
			break;
		}

		//		m_fTurnSpeed = 0.4; // Slow searching speed

		if (m_fLastLookTime < gpGlobals->time)
		{
			// Don't want to jitter all the time!
			// only set the view direction every few seconds
			// mainly dependant on bots skill.

			Vector vBaseLookDir, vBaseLookOrigin;

			if (m_pLastEnemy != nullptr && EntityIsAlive(m_pLastEnemy))
			{
				//				m_fTurnSpeed = 20.0;

				vBaseLookOrigin = m_vLastSeeEnemyPosition.GetVector();
			}
			else if (m_bMoveToIsValid)
			{
				vBaseLookOrigin = m_vMoveToVector;
			}
			else
			{
				m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;
				break;
			}

			Vector vRandom = Vector(RANDOM_FLOAT(0.0, 128.0) - 64.0, RANDOM_FLOAT(0.0, 128.0) - 64.0,
			                        RANDOM_FLOAT(0.0, 128.0) - 64.0);
			vRandom = vRandom.Normalize();
			vRandom = vRandom * 64;

			m_vCurrentLookDir = vBaseLookOrigin + vRandom;
			m_fLastLookTime = gpGlobals->time + RANDOM_FLOAT(2.0, 4.0);
		}

		SetViewAngles(m_vCurrentLookDir);

		break;
		// gotta face the enemy to shoot the thing
	case BOT_LOOK_TASK_FACE_ENEMY:
		// erratic look behaviour so we sometimes miss
	{
		const float fTempspeed = m_fTurnSpeed;

		//m_fTurnSpeed = 10.0*m_Profile.m_fAimSpeed;
		m_fTurnSpeed = 15 * (1 - m_Profile.m_fAimSpeed);
		if (m_fTurnSpeed <= 0)
			m_fTurnSpeed = 1;

		if (m_pEnemy)
		{
			// Get the aim vector of the enemy dependant
			// on the bots skill, enemy's size and speed.
			SetViewAngles(GetAimVector(m_pEnemy));
		}
		else
		{
			// Had an enemy but lost it or killed it
			// search for another/same enemy again
			m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY;
			m_fSearchEnemyTime = gpGlobals->time + RANDOM_FLOAT(5.0, 8.0);
		}

		m_fTurnSpeed = fTempspeed;
	}
	break;

	case BOT_LOOK_TASK_FACE_TEAMMATE:
		//SetViewAngles(m_CurrentLookTask.vLookVec);
		break;
	case BOT_LOOK_TASK_FACE_TASK_VECTOR:
		if (m_CurrentTask)
			SetViewAngles(m_CurrentTask->TaskVector());
		else
			m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
		break;
		//try to face sound
	case BOT_LOOK_TASK_FACE_GOAL_OBJECT:
		//		if ( m_GoalTask.Task() != BOT_TASK_NONE )
		//		{
		//			SetViewAngles(m_GoalTask.TaskVector());
		//		}
		m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
		break;
		// got a task with an edict stored that we need
		// to face? (e.g. a button to press it)
	case BOT_LOOK_TASK_FACE_TASK_EDICT:

		m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

		if (m_CurrentTask)
		{
			edict_t* pTaskEdict;

			if ((pTaskEdict = m_CurrentTask->TaskEdict()) != nullptr)
			{
				Vector vOrigin = AbsOrigin(pTaskEdict); // EntityOrigin

				// could have problems for somethings
				if (IsMarine())
				{
					switch (m_CurrentTask->Task())
					{
						// if I want to build, don't face exact origin
					case BOT_TASK_BUILD:
					case BOT_TASK_USE_AMMO_DISP:
						// face origin of buildable but same height as my self
						vOrigin = pTaskEdict->v.origin;
						vOrigin.z = pev->origin.z;
						break;
					default:
						break;
					}
				}

				SetViewAngles(vOrigin);
				m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;
			}
		}
		break;
	}
}

// ChangeAngles - adapted by Whistler (http://forums.bots-united.com/member.php?u=786)

void BotFunc_ChangeAngles(float* fSpeed, const float* fIdeal, float* fCurrent, float* fUpdate)
{
	float fCurrent180;  // current +/- 180 degrees

	// turn from the current v_angle yaw to the ideal_yaw by selecting
	// the quickest way to turn to face that direction

	// find the difference in the current and ideal angle
	const float fDiff = fabs(*fCurrent - *fIdeal);

	// check if the bot is already facing the ideal_yaw direction...
	if (fDiff <= 0.1)
	{
		*fSpeed = fDiff;

		return;
	}

	// check if difference is less than the max degrees per turn
	if (fDiff < *fSpeed)
		*fSpeed = fDiff;  // just need to turn a little bit (less than max)

	 // here we have four cases, both angle positive, one positive and
	 // the other negative, one negative and the other positive, or
	 // both negative.  handle each case separately...

	if (*fCurrent >= 0 && *fIdeal >= 0)  // both positive
	{
		if (*fCurrent > * fIdeal)
			*fCurrent -= *fSpeed;
		else
			*fCurrent += *fSpeed;
	}
	else if (*fCurrent >= 0 && *fIdeal < 0)
	{
		fCurrent180 = *fCurrent - 180;

		if (fCurrent180 > * fIdeal)
			*fCurrent += *fSpeed;
		else
			*fCurrent -= *fSpeed;
	}
	else if (*fCurrent < 0 && *fIdeal >= 0)
	{
		fCurrent180 = *fCurrent + 180;
		if (fCurrent180 > * fIdeal)
			*fCurrent += *fSpeed;
		else
			*fCurrent -= *fSpeed;
	}
	else  // (current < 0) && (ideal < 0)  both negative
	{
		if (*fCurrent > * fIdeal)
			*fCurrent -= *fSpeed;
		else
			*fCurrent += *fSpeed;
	}

	UTIL_FixFloatAngle(fCurrent);

	*fUpdate = *fCurrent;
}

void CBot::ChangeAngles(float* fSpeed, float* fIdeal, float* fCurrent, float* fUpdate)
{
	BotFunc_ChangeAngles(fSpeed, fIdeal, fCurrent, fUpdate);
	/*
   float fCurrent180;  // current +/- 180 degrees
   float fDiff;

   // turn from the current v_angle yaw to the ideal_yaw by selecting
   // the quickest way to turn to face that direction

   // find the difference in the current and ideal angle
   fDiff = fabs(*fCurrent - *fIdeal);

   // check if the bot is already facing the ideal_yaw direction...
   if (fDiff <= 0.1)
   {
	*fSpeed = fDiff;

	  return;
   }

   // check if difference is less than the max degrees per turn
   if (fDiff < *fSpeed)
	  *fSpeed = fDiff;  // just need to turn a little bit (less than max)

   // here we have four cases, both angle positive, one positive and
   // the other negative, one negative and the other positive, or
   // both negative.  handle each case separately...

   if ((*fCurrent >= 0) && (*fIdeal >= 0))  // both positive
   {
	  if (*fCurrent > *fIdeal)
		 *fCurrent -= *fSpeed;
	  else
		 *fCurrent += *fSpeed;
   }
   else if ((*fCurrent >= 0) && (*fIdeal < 0))
   {
	  fCurrent180 = *fCurrent - 180;

	  if (fCurrent180 > *fIdeal)
		 *fCurrent += *fSpeed;
	  else
		 *fCurrent -= *fSpeed;
   }
   else if ((*fCurrent < 0) && (*fIdeal >= 0))
   {
	  fCurrent180 = *fCurrent + 180;
	  if (fCurrent180 > *fIdeal)
		 *fCurrent += *fSpeed;
	  else
		 *fCurrent -= *fSpeed;
   }
   else  // (current < 0) && (ideal < 0)  both negative
   {
	  if (*fCurrent > *fIdeal)
		 *fCurrent -= *fSpeed;
	  else
		 *fCurrent += *fSpeed;
   }

   UTIL_FixFloatAngle(fCurrent);

   *fUpdate = *fCurrent;*/
}
/*

void CBot :: ChangeAngles ( float *fSpeed, float *fIdeal, float *fCurrent, float *fUpdate )
{
   float fCurrent180;  // current +/- 180 degrees
   float fDiff;

   // turn from the current v_angle yaw to the ideal_yaw by selecting
   // the quickest way to turn to face that direction

   // find the difference in the current and ideal angle
   fDiff = fabs(*fCurrent - *fIdeal);

   // check if the bot is already facing the ideal_yaw direction...
   if (fDiff <= 0.1)
   {
	   *fSpeed = fDiff;

	  return;  // return number of degrees turned
   }

   // check if difference is less than the max degrees per turn
   if (fDiff < *fSpeed)
	  *fSpeed = fDiff;  // just need to turn a little bit (less than max)

   // here we have four cases, both angle positive, one positive and
   // the other negative, one negative and the other positive, or
   // both negative.  handle each case separately...

   if ((*fCurrent >= 0) && (*fIdeal >= 0))  // both positive
   {
	  if (*fCurrent > *fIdeal)
		 *fCurrent -= *fSpeed;
	  else
		 *fCurrent += *fSpeed;
   }
   else if ((*fCurrent >= 0) && (*fIdeal < 0))
   {
	  fCurrent180 = *fCurrent - 180;

	  if (fCurrent180 > *fIdeal)
		 *fCurrent += *fSpeed;
	  else
		 *fCurrent -= *fSpeed;
   }
   else if ((*fCurrent < 0) && (*fIdeal >= 0))
   {
	  fCurrent180 = *fCurrent + 180;
	  if (fCurrent180 > *fIdeal)
		 *fCurrent += *fSpeed;
	  else
		 *fCurrent -= *fSpeed;
   }
   else  // (current < 0) && (ideal < 0)  both negative
   {
	  if (*fCurrent > *fIdeal)
		 *fCurrent -= *fSpeed;
	  else
		 *fCurrent += *fSpeed;
   }

   UTIL_FixFloatAngle(fCurrent);

   *fUpdate = *fCurrent;
}*/

void CBot::SetViewAngles(const Vector& pOrigin)
// make the bot face pOrigin.
{
	/*
	vector<ga_value> inputs;
	vector<ga_value> outputs;

	UTIL_MakeVectors(pev->v_angle);

	Vector v_comp = ((pOrigin-GetGunPosition()).Normalize() - gpGlobals->v_forward).Normalize();

	inputs.push_back(v_comp.x);
	inputs.push_back(v_comp.y);
	inputs.push_back(v_comp.z);

	vector <ga_value> weights;

	BOOL bGiveUpdate = FALSE;

	BOOL bExecute = TRUE;

	if ( bSetPrevAimOrigin == FALSE )
	{
		bSetPrevAimOrigin = TRUE;
		vPrevAimOrigin = pOrigin;
	}

	if ( m_fAimLearnTime < gpGlobals->time )
	{
		float fFitness = 8192.0f / ( (GetGunPosition()+(gpGlobals->v_forward*DistanceFrom(vPrevAimOrigin)))-pOrigin).Length();

		bAimStopMoving = (fFitness<750.0f);

		bGiveUpdate = TRUE;

		m_pAimingGAVals->setFitness(fFitness);

		BotMessage(NULL,0,"Fitness : %0.2f",fFitness);

		m_pAimingGA->addToPopulation(m_pAimingGAVals->copy());

		if ( m_pAimingGA->canPick() )
		{
			IIndividual *ind = m_pAimingGA->pick();

			m_pAimingGAVals->freeMemory();
			delete m_pAimingGAVals;

			m_pAimingGAVals = (CBotGAValues*)ind;
			m_pAimingGAVals->setFitness(0);

			m_pAiming->trainOutputs(m_pAimingGAVals->returnVector());
		}
		else
		{
			vector<ga_value> weights;
			m_pAiming->randomize();

			m_pAimingGAVals->setVector(weights);
			m_pAimingGAVals->setFitness(0);
			m_pAiming->execute(&outputs,&inputs);

			m_pAiming->getOutputs(m_pAimingGAVals->returnVector());

			bExecute = FALSE; // done already
		}

		//m_pAimingGAVals->getVector(&weights);

		//m_pAiming->setWeights(weights);

		vPrevAimOrigin = pOrigin;

		m_fAimLearnTime = gpGlobals->time + 0.333f;
	}

	if ( bExecute )
		m_pAiming->execute(&outputs,&inputs);

	pev->v_angle.x = (outputs[0]*360)-180.0f;
	pev->v_angle.y = (outputs[1]*360)-180.0f;

	if ( pev->v_angle.x < -90.0f )
		pev->v_angle.x = -90.0f;
	if ( pev->v_angle.x > 90.0f )
		pev->v_angle.x = 90.0f;

	if ( bGiveUpdate )
	{
		BotMessage(NULL,0,"x: %0.2f, y: %0.2f",pev->v_angle.x,pev->v_angle.y);
	}

	pev->angles.x = -pev->v_angle.x/3;
	pev->angles.y = pev->v_angle.y;

	UTIL_FixAngles(&pev->v_angle);
	UTIL_FixAngles(&pev->angles);//*/

	///*

	Vector vComponent;
	Vector vAngles;

	float fTurnSpeed = m_fTurnSpeed;

	BOOL bUsePitch = FALSE;
	const eClimbType iClimbType = GetClimbType();
	const BOOL bCanClimb = iClimbType != BOT_CLIMB_NONE;

	// if possible, stay looking one direction when climbing ladder ("ladder angles")
	// best results is 0
	if (gBotGlobals.m_fUpdateLadderTime != -1 && m_fUpdateLadderAngleTime < gpGlobals->time)
	{
		UnSetLadderAngleAndMovement();
		m_fUpdateLadderAngleTime = gpGlobals->time + gBotGlobals.m_fUpdateLadderTime;
	}

	if (IsSkulk() && (iClimbType == BOT_CLIMB_WALL_STICKING || // current wall-sticking
		iClimbType == BOT_CLIMB_TRYING_WALL_STICK) &&
		m_CurrentLookTask != BOT_LOOK_TASK_FACE_ENEMY) // trying to wall-stick
	{
		// stay facing the wall

		// let "move up" and "move down" do the work
		vAngles.x = 0;

		vComponent = m_vMoveToVector - GetGunPosition();
		vAngles = UTIL_VecToAngles(vComponent);
		UTIL_FixAngles(&vAngles);
	}
	else if (bCanClimb && LadderAnglesSet())
	{
		vAngles = GetLadderAngles();

		bUsePitch = TRUE;
		//		m_fTurnSpeed = 12.0;
	}
	else
	{
		vComponent = pOrigin - GetGunPosition();

		vAngles = UTIL_VecToAngles(vComponent);

		UTIL_FixAngles(&vAngles);

		// ladder angles
		if (bCanClimb && m_CurrentLookTask == BOT_LOOK_TASK_NEXT_WAYPOINT)
		{
			const int iLadderDir = GetLadderDir();

			if (iLadderDir == 1)// && ((vAngles.x < 0)||(vAngles.x > 60)))
				vAngles.x = 60;
			else if (iLadderDir == -1)// && ((vAngles.x > 0)||(vAngles.x < -60)) )
				vAngles.x = -60;

			SetLadderAngles(vAngles);
			bUsePitch = TRUE;
			m_fTurnSpeed = MAX_JUMP_HEIGHT;
		}
		else if (LadderAnglesSet())
			UnSetLadderAngleAndMovement();
	}

	m_CurrentTask = m_Tasks.CurrentTask();

	// get the current task name
	if (m_CurrentTask != nullptr)
		eBotTask iCurrentTask = m_CurrentTask->Task();

	vAngles.z = 0;
	pev->v_angle.z = 0;

	float zComp = m_vMoveToVector.z - pev->origin.z;

	if (zComp < 0)
		zComp = -zComp;

	if (bUsePitch == FALSE)
	{
		if (m_CurrentLookTask)
		{
			switch (m_CurrentLookTask)
			{
			case BOT_LOOK_TASK_SEARCH_FOR_ENEMY:
			case BOT_LOOK_TASK_FACE_ENEMY:
			case BOT_LOOK_TASK_FACE_TEAMMATE:
			case BOT_LOOK_TASK_FACE_GOAL_OBJECT:
			case BOT_LOOK_TASK_FACE_ENTITY:
			case BOT_LOOK_TASK_FACE_TASK_VECTOR:
			case BOT_LOOK_TASK_FACE_TASK_EDICT:
			case BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY:
			case BOT_LOOK_TASK_FACE_GROUND:
			case BOT_LOOK_TASK_LOOK_AROUND:
				bUsePitch = TRUE;
				break;
			default:
				bUsePitch = FALSE;
				break;
			}
		}
	}

	if (bUsePitch == FALSE)
	{
		// Bot is swimming need to look up/down.
		if (pev->waterlevel > 0)
		{
			bUsePitch = TRUE;
		}
		else if (zComp > MAX_JUMP_HEIGHT)
			bUsePitch = TRUE;
	}

	if (bUsePitch)
	{
		pev->idealpitch = -vAngles.x;
	}
	else
		pev->idealpitch = 0;

	pev->ideal_yaw = vAngles.y;

	// change angles smoothly

	//temp = 1/1+exp(-fabs((pev->ideal_yaw+180.0f)-(pev->v_angle.y+180.0f))/180);
	float temp = fabs(pev->ideal_yaw + 180.0f - (pev->v_angle.y + 180.0f));

	fTurnSpeed = temp / m_fTurnSpeed;//fabs((pev->ideal_yaw+180.0f)-(pev->v_angle.y+180.0f))/20;//m_fTurnSpeed;
	// change yaw
	ChangeAngles(&fTurnSpeed, &pev->ideal_yaw, &pev->v_angle.y, &pev->angles.y); // 5 degrees

	//temp = 1/1+exp(-fabs((pev->idealpitch+180.0f)-(pev->v_angle.x+180.0f))/180);
	temp = fabs(pev->idealpitch + 180.0f - (pev->v_angle.x + 180.0f));

	// set by ChangeAngles... remove this functionality soon...
	fTurnSpeed = temp / m_fTurnSpeed;
	//fTurnSpeed = fabs((pev->idealpitch+180.0f)-(pev->v_angle.x+180.0f))/20;//m_fTurnSpeed;

	// change pitch
	ChangeAngles(&fTurnSpeed, &pev->idealpitch, &pev->v_angle.x, &pev->angles.x);

	// reset turn speed
	m_fTurnSpeed = gBotGlobals.m_fTurnSpeed;//BOT_DEFAULT_TURN_SPEED;
	//pev->v_angle.x = -pev->v_angle.x;
	pev->angles.x = -pev->v_angle.x / 3;

	pev->angles.y = pev->v_angle.y;//*/
}

void CBot::touchedWpt()
{
	if (gBotGlobals.IsNS() && m_bFlappedWings)
	{
		BOOL bLeaderWalking = FALSE;

		if (m_pSquadLeader)
			bLeaderWalking = UTIL_OnGround(&m_pSquadLeader->v);
		//dec_flapWings->train(1.0f);
		m_bFlappedWings = FALSE;

		if (!bLeaderWalking && DistanceFrom(m_vMoveToVector) < BOT_WAYPOINT_TOUCH_DIST
			|| m_iCurrentWaypointFlags & (W_FL_WALL_STICK | W_FL_FLY | W_FL_LADDER))
		{
			//BotMessage(NULL,0,"%s touched ma wpt",m_szBotName);

			m_pFlyGAVals->set_fitness(1000.0f);

			m_pFlyGA->addToPopulation(m_pFlyGAVals->copy());

			if (m_pFlyGA->canPick())
			{
				IIndividual* ind = m_pFlyGA->pick();

				dec_flapWings->setTrained();

				m_pFlyGAVals->freeMemory();
				delete m_pFlyGAVals;

				m_pFlyGAVals = static_cast<CBotGAValues*>(ind);
				m_pFlyGAVals->set_fitness(0);

				float x1 = m_pFlyGAVals->get(0);
				float x2 = m_pFlyGAVals->get(1);

				if (x1 < 0.01)
					x1 = 0.01;
				if (x2 < 0.01)
					x2 = 0.01;
				if (x1 > 0.2)
					x1 = 0.2;
				if (x2 > 0.2)
					x2 = 0.2;

				m_pFlyGAVals->set(0, x1);
				m_pFlyGAVals->set(1, x2);

				//BotMessage(NULL,0,"hold time : %0.4f",m_pFlyGAVals->get(0));
				//BotMessage(NULL,0,"let go time : %0.4f",m_pFlyGAVals->get(1));
			}
			else
			{
				m_pFlyGAVals->clear();
				// custom : lerk hold & flap time
				m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 0
				m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 1

				m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 2
				m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 3
				m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 4
				m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 5
			}
		}
	}
}

void CBot::gotStuck()
{
}

void CBot::WorkMoveDirection(void)
{
	// Move Direction Related To View Direction!!!

	float fAngle;

	Vector vAngle;
	BOOL bNullMove = FALSE;

	// climbing/trying to climb/flying etc..?
	const eClimbType iCurrentClimbState = GetClimbType();

	if (!m_bNotFollowingWaypoint && iCurrentClimbState == BOT_CLIMB_NONE)
	{
		if (m_iCurrentWaypointIndex == -1)
		{
			/*	if ( !m_bMoveToIsValid )
				{
					if ( !HasCondition(BOT_CONDITION_SEE_SQUAD_LEADER) && DistanceFrom(m_vLowestEnemyCostVec) > BOT_WAYPOINT_TOUCH_DIST )
					{
						if ( UTIL_PointContents(m_vMoveToVector) == CONTENTS_SOLID )
							Duck();

						SetMoveVector(m_vLowestEnemyCostVec);
					}
				}*/
			if (DistanceFrom(m_vMoveToVector) < BOT_WAYPOINT_TOUCH_DIST)
			{
				// reached target, need to work out next move.
				StopMoving();
			}
		}
	}

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
	{
		const int iPrevWaypointFlags = WaypointFlags(m_iPrevWaypointIndex);

		BOOL bLeaderWalking = TRUE;

		if (m_pSquadLeader && HasCondition(BOT_CONDITION_SEE_SQUAD_LEADER))
			bLeaderWalking = UTIL_OnGround(&m_pSquadLeader->v);

		if (m_iCurrentWaypointFlags & W_FL_FLY || iPrevWaypointFlags & W_FL_FLY ||
			m_iCurrentWaypointFlags & W_FL_LADDER || iPrevWaypointFlags & W_FL_LADDER ||
			m_iCurrentWaypointFlags & W_FL_WALL_STICK || iPrevWaypointFlags & W_FL_WALL_STICK ||
			!bLeaderWalking)
		{
			if (m_vMoveToVector.z > pev->absmin.z)
			{
				if (IsMarine() && HasJetPack() && !(m_iCurrentWaypointFlags & W_FL_LADDER) || IsLerk())
				{
					if (!onGround())
					{
						if (pev->absmin.z - m_fStartFlyHeight > m_fPrevFlyHeight)
							m_fPrevFlyHeight = pev->absmin.z - m_fStartFlyHeight;

						m_bFlappedWings = TRUE;
					}
					else
					{
						m_fStartFlyHeight = pev->absmin.z;

						if (m_bFlappedWings)
						{
							m_pFlyGAVals->set_fitness(m_fPrevFlyHeight);

							IIndividual* newCopy = m_pFlyGAVals->copy();

							m_pFlyGA->addToPopulation(newCopy);

							if (m_pFlyGA->canPick())
							{
								IIndividual* ind = m_pFlyGA->pick();

								dec_flapWings->setTrained();

								m_pFlyGAVals->freeMemory();
								delete m_pFlyGAVals;

								m_pFlyGAVals = static_cast<CBotGAValues*>(ind);
								m_pFlyGAVals->set_fitness(0);

								float x1 = m_pFlyGAVals->get(0);
								float x2 = m_pFlyGAVals->get(1);

								if (x1 < 0.01)
									x1 = 0.01;
								if (x2 < 0.01)
									x2 = 0.01;
								if (x1 > 0.2)
									x1 = 0.2;
								if (x2 > 0.2)
									x2 = 0.2;

								m_pFlyGAVals->set(0, x1);
								m_pFlyGAVals->set(1, x2);
							}
							else
							{
								m_pFlyGAVals->clear();
								// custom : lerk hold & flap time
								m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 0
								m_pFlyGAVals->add(RANDOM_FLOAT(0, 0.2)); // 1

								m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 2
								m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 3
								m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 4
								m_pFlyGAVals->add(0.3 - RANDOM_FLOAT(0, 6)); // 5
							}
						}

						m_bFlappedWings = FALSE;
						m_fPrevFlyHeight = 0;
					}

					std::vector<ga_value> inputs;

					const float fEnergy = NS_AmountOfEnergy();

					inputs.push_back(fEnergy * 0.01f);
					inputs.push_back(m_vMoveToVector.z - pev->absmin.z);///(pev->size.z/2));
					inputs.push_back(pev->velocity.z);///(pev->maxspeed*0.33));
					inputs.push_back(m_pFlyGAVals->get(1));

					dec_flapWings->setWeights(m_pFlyGAVals, 2, 4);

					dec_flapWings->input(&inputs);
					dec_flapWings->execute();

					if (fEnergy == 100 || (!dec_flapWings->trained() || dec_flapWings->fired()))//(NS_AmountOfEnergy()*0.01 > m_pFlyGAVals->get(2)) )//!dec_flapWings->trained() || dec_flapWings->fired () )
					{
						FlapWings();
						//BotMessage(NULL,0,"%s flappin ma wings",m_szBotName);
					}
					else
					{
						//BotMessage(NULL,0,"%s havin' a rest",m_szBotName);
						StopMoving();

						if (m_CurrentLookTask == BOT_LOOK_TASK_NEXT_WAYPOINT)
							m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;
					}

					inputs.clear();
				}
				// check again
				/*else if ( !(m_iCurrentWaypointFlags & W_FL_LADDER) && (IsMarine() && HasJetPack()) )
				{
					// enough fuel?
					if ( NS_AmountOfEnergy() > 75 )
					{
						// hold in jump most of the time (use jetpack)
						if ( RANDOM_LONG(0,50) > 1 )
							Jump();
					}
					// Don't stop moving If I am trying to shoot an enemy with melee weapon
					else if ( !m_pEnemy || !m_pCurrentWeapon || !m_pCurrentWeapon->IsMelee() )
					{
						// get some jet pack fuel back first
						StopMoving();

						if ( m_CurrentLookTask == BOT_LOOK_TASK_NEXT_WAYPOINT )
							m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;
					}
				}*/
			}
		}
	}
	break;
	default:
		break;
	}

	if (!m_bMoveToIsValid)
	{
		if (iCurrentClimbState == BOT_CLIMB_CLIMBING)
			bNullMove = FALSE;
		else
		{
			// Hasn't got a waypoint or desired vector to move to
			bNullMove = TRUE;

			if (IsMarine() && pev->groundentity)
			{
				if (pev->groundentity->v.iuser3 == AVH_USER3_INFANTRYPORTAL)
					bNullMove = FALSE;
			}
		}
	}

	if ((m_vMoveToVector - pev->origin).Length() < 4) // close enough
		bNullMove = TRUE;
	else if (m_bNotFollowingWaypoint)
		bNullMove = FALSE;
	if (m_pAvoidEntity && m_bMoveToIsValid) // trying to avoid something
		bNullMove = FALSE;

	if (bNullMove || pev->flags & FL_FROZEN)
	{
		m_fMoveSpeed = 0.0;
		m_fStrafeSpeed = 0.0;

		if (pev->waterlevel == 3)
			Jump();

		return;
	}

	if (iCurrentClimbState != BOT_CLIMB_NONE)
	{
		if (m_iCurrentWaypointIndex != -1)
		{
			m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;

			if (!IsSkulk())
				m_vMoveToVector = WaypointOrigin(m_iCurrentWaypointIndex);
		}
	}
	/*else if ( IsSkulk() && ((m_vMoveToVector.z - pev->origin.z) > MAX_JUMP_HEIGHT) )
	{
		m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
		m_vMoveToVector = WaypointOrigin(m_iCurrentWaypointIndex);

		pev->button |= IN_FORWARD;
	}*/

	const BOOL bOnLadder = m_iCurrentWaypointFlags & W_FL_LADDER;
	// better ladder handling, man...
	if (m_iCurrentWaypointIndex != -1 && bOnLadder && IsLerk())
	{
		if (WaypointOrigin(m_iCurrentWaypointIndex).z > pev->origin.z)
		{
			m_fMoveSpeed = 0.0;
			m_fStrafeSpeed = 0.0;

			return;
		}
	}

	/*	else if ( bOnLadder && (m_iPrevWaypointIndex != -1) )
		{
		// better ladder climbing???
			Vector vMoveTo;
			Vector vComp;
			Vector vWptOrigin = WaypointOrigin(m_iCurrentWaypointIndex);
			Vector vTemp = WaypointOrigin(m_iPrevWaypointIndex);

			vTemp.z = vWptOrigin.z;
			vComp = (vWptOrigin - vTemp).Normalize();

			// make sure we WALK INTO the ladder ALWAYS
			// by finding the component from our previous waypoint.
			vMoveTo = (vWptOrigin+(vComp*32));

			fAngle = UTIL_YawAngleBetweenOrigin(pev,vMoveTo);
		}
		else*/

	if (m_pCurrentWeapon && m_pCurrentWeapon->IsMelee())
		m_pAvoidEntity = nullptr;

	if (m_pAvoidEntity)
	{
		BOOL bEnemy = FALSE;

		if (m_fAvoidTime <= gpGlobals->time)
		{
			if ( /*gBotGlobals.IsMod(MOD_TFC) ||*/ (bEnemy = IsEnemy(m_pAvoidEntity)) == TRUE)
			{
				if (m_pAvoidEntity->v.velocity.x || m_pAvoidEntity->v.velocity.y)
				{
					// leave some space to walk by (my size plus a bit)
					m_vAvoidMoveTo = ((m_vMoveToVector - pev->origin).Normalize() + (pev->origin - EntityOrigin(m_pAvoidEntity)).Normalize()) * (pev->size.Length2D() * 1.1);
					m_vAvoidMoveTo = pev->origin + m_vAvoidMoveTo;
					m_vAvoidMoveTo.z = pev->origin.z;

					// check again in 0.2 seconds...
					m_fAvoidTime = gpGlobals->time + 0.2;
				}
			}
		}

		// avoiding enemy?
		if (m_fAvoidTime > gpGlobals->time)
			fAngle = UTIL_YawAngleBetweenOrigin(pev, m_vAvoidMoveTo); // move around enemy related to waypoint
		else
			fAngle = UTIL_GetAvoidAngle(m_pEdict, EntityOrigin(m_pAvoidEntity));
	}
	else
	{
		fAngle = UTIL_YawAngleBetweenOrigin(pev, m_vMoveToVector);
	}

	if (pev->waterlevel > 0)
	{
		if (m_fUpTime < gpGlobals->time)
		{
			if (m_vMoveToVector.z < pev->origin.z)
			{
				m_fUpSpeed = -m_fMaxSpeed;
			}
			else if (m_vMoveToVector.z > pev->origin.z)
			{
				m_fUpSpeed = m_fMaxSpeed;
			}
		}

		// If bot is trying to get back on land, decrease the up speed to water accelerate
		if (m_fUpSpeed > 0)
		{
			if (UTIL_PointContents(m_vMoveToVector) != CONTENTS_WATER)
			{
				pev->button &= ~IN_JUMP;
				m_fUpSpeed = m_vMoveToVector.z - pev->origin.z;

				if (m_fUpSpeed > m_fMaxSpeed)
					m_fUpSpeed = m_fMaxSpeed;
			}
		}
	}
	// skulk wanting to climb?
	else if (IsSkulk() && iCurrentClimbState != BOT_CLIMB_NONE)
	{
		// use up-speed
		if (m_vMoveToVector.z < pev->origin.z)
		{
			m_fUpSpeed = -m_fMaxSpeed;
		}
		else if (m_vMoveToVector.z + pev->size.z / 2 > pev->origin.z)
		{
			m_fUpSpeed = m_fMaxSpeed;
		}
	}

	UTIL_FixFloatAngle(&fAngle);

	// botmans code! (me maths suxorz...sorta)
	const float radians = fAngle * M_PI / 180.f; // degrees to radians
	const float flMove = cos(radians);
	const float flSide = sin(radians);
	//

	/* // <-- add extra slash to uncomment this code
	// Spread the angle onto a scale from -1 to 1 (-1 being left, 1 being right)
	flSide = fAngle;

	if ( flSide < -90.0 )
	{
		while (flSide < -90.0)
			flSide+=90.0;

		flSide = -90.0 - flSide;
	}

	if ( flSide > 90.0 )
	{
		while (flSide > 90.0)
			flSide-=90.0;

		flSide = 90.0 - flSide;
	}

	flSide/=90.0;

	// wrap angle onto a scale for -1 being backwards and 1 being forwards
	// Flip scale for forward/back

	if( fAngle >= 0.0 )
		flMove = 1 - (fAngle/90.0);
	else
		flMove = 1 + (fAngle/90.0);

	if( flMove < -1.0 )
		flMove += 1.0;
	else if ( flMove  > 1.0 )
		flMove -= 1.0;

	//*/

	m_fMoveSpeed = m_fIdealMoveSpeed * flMove;

	// dont want this to override strafe speed if we're trying
	// to strafe to avoid a wall for instance.
	if (m_fStrafeTime < gpGlobals->time)
		m_fStrafeSpeed = m_fIdealMoveSpeed * flSide;

	if (iCurrentClimbState == BOT_CLIMB_CLIMBING && !IsSkulk())//|| (IsSkulk()&&(iCurrentClimbState == BOT_CLIMB_NOT_TOUCHING)) )
	{
		// need to hold in forward button to make bot move on ladder
		pev->button |= IN_FORWARD;
	}

	float fMoveSpeed = m_fMoveSpeed;
	float fStrafeSpeed = m_fStrafeSpeed;

	if (fMoveSpeed < 0)
		fMoveSpeed = -fMoveSpeed;
	if (fStrafeSpeed < 0)
		fStrafeSpeed = -fStrafeSpeed;

	// moving less than 1.0 units/sec? just stop to
	// save bot jerking around..
	if (fMoveSpeed < 1.0)
		m_fMoveSpeed = 0.0;
	if (fStrafeSpeed < 1.0)
		m_fStrafeSpeed = 0.0;

	if (gBotGlobals.IsMod(MOD_HL_RALLY))
	{
		const float fSpeed = pev->velocity.Length2D();

		if (m_iGear < 5)
		{
			if (fSpeed < m_fIdealMoveSpeed)
			{
				FakeClientCommand(m_pEdict, "gearup");
				m_iGear++;
			}
		}

		if (m_iGear > 2)
		{
			if (fSpeed < m_fIdealMoveSpeed)
			{
				FakeClientCommand(m_pEdict, "geardown");
				m_iGear--;
			}
		}

		// go into reverse!!!
		if (m_iGear > -1)
		{
			if (m_fMoveSpeed < 0)
			{
				FakeClientCommand(m_pEdict, "geardown");
				m_iGear--;

				m_fMoveSpeed = -m_fMoveSpeed;
			}
		}
	}
}

BOOL CBot::CanPickup(edict_t* pPickup) const
{
	// if entity can be picked up by player return true,
	if (pPickup == nullptr)
		return FALSE;

	if (m_fPickupItemTime > gpGlobals->time)
		return FALSE;

	if (pPickup->v.effects & EF_NODRAW)
		return FALSE;

	if (pPickup->v.origin.z > pev->origin.z + MAX_JUMP_HEIGHT)
		return FALSE;
	// too high up

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		if (IsMarine())
		{
			return pPickup->v.iuser3 == AVH_USER3_MARINEITEM;
		}
		break;
	case MOD_BUMPERCARS:
		if (strcmp("item_powerup", STRING(pPickup->v.netname)) == 0)
			return TRUE;
		break;
	case MOD_RC:
	case MOD_RC2:
	case MOD_DMC:
	{
		char* szClassname = const_cast<char*>(STRING(pPickup->v.classname));

		// quad damage etc..
		if (strncmp(szClassname, "item_artifact", 13) == 0)
			return TRUE;
	}
	break;
	case MOD_TS:
	{
		char* szClassname = const_cast<char*>(STRING(pPickup->v.classname));

		// quad damage etc..
		if (strncmp(szClassname, "ts_powerup", 12) == 0)
			return TRUE;
		else if (strcmp(szClassname, "ts_groundweapon") == 0)
			return TRUE;
	}
	break;
	default:
		return FALSE;
	}

	return FALSE;
}

BOOL CBot::Touch(edict_t* pentTouched)
{
	BOOL bIsMoving = FALSE;

	entvars_t* pentTouchedpev = &pentTouched->v;

	char* szClassname = const_cast<char*>(STRING(pentTouched->v.classname));

	// to avoid sticking to other bots..
	if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_UNSTICK))
	{
		// dont jump while holding minigun (forces bot to drop minigun)
		if (/*!IsHoldingMiniGun() &&*/ m_fMoveSpeed && pentTouchedpev->flags & FL_FAKECLIENT && pev->flags & FL_ONGROUND && pentTouchedpev->flags & FL_ONGROUND && !IsOnLadder() && pev->groundentity != pentTouched)
		{
			if (!m_CurrentTask || m_CurrentTask->Task() != BOT_TASK_HUMAN_TOWER)
			{
				if (RANDOM_LONG(0, 1))
				{
					JumpAndDuck();
				}
				else
				{
					pev->button &= ~IN_JUMP;
					pev->button &= ~IN_DUCK;
				}
			}
		}
	}

	if (pentTouched->v.solid == SOLID_TRIGGER)
	{
		// update our long jump state so we know we have it
		if (strcmp(szClassname, "item_longjump") == 0)
			m_bHasLongJump = TRUE;
		else if (!m_bHasFlag && strcmp(szClassname, "item_tfgoal") == 0)
		{
#ifdef RCBOT_META_BUILD
			MDLL_Touch(pentTouched, m_pEdict);
#else
			extern DLL_FUNCTIONS other_gFunctionTable;

			(*other_gFunctionTable.pfnTouch)(pentTouched, m_pEdict);
#endif
			if (pentTouched->v.owner == m_pEdict)
			{
				m_Tasks.FlushTasks();
				m_bHasFlag = TRUE;
				m_stBotPaths.Destroy();

				m_pFlag = pentTouched;
			}

			return TRUE;
		}
	}

	if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_TOUCH_LEVEL))
		DebugMessage(BOT_DEBUG_TOUCH_LEVEL, nullptr, 0, "Touch() %s touched %s", m_szBotName, szClassname);

	// touched the thing i want to pick up
	if (m_pPickupEntity == pentTouched)
	{
		if ((m_CurrentTask = m_Tasks.CurrentTask()) != nullptr)
		{
			if (m_CurrentTask->Task() == BOT_TASK_PICKUP_ITEM &&
				m_CurrentTask->TaskEdict() == m_pPickupEntity)
			{
				m_Tasks.FinishedCurrentTask();
			}
		}

		m_fPickupItemTime = gpGlobals->time + RANDOM_FLOAT(5.0, 10.0);

		m_pPickupEntity = nullptr;

		return FALSE;
	}

	if (pentTouched->v.solid != SOLID_TRIGGER)
	{
		const BOOL bIsDoor = strncmp(szClassname, "func_door", 9) == 0 || strncmp(szClassname, "func_plat", 9) == 0;

		if (bIsDoor)
		{
			if ((bIsMoving = BotFunc_EntityIsMoving(pentTouchedpev)) == TRUE)
				m_fLastSeeWaypoint = 0;
		}

		const BOOL bUsingLift = m_Tasks.HasSchedule(BOT_SCHED_USE_LIFT);

		if (bIsDoor && m_iCurrentWaypointIndex != -1)
		{
			// not still trying to use a lift
			if (pentTouched->v.movedir.z)
			{
				// traceline spammage
				if (WaypointOrigin(m_iCurrentWaypointIndex).z > GetGunPosition().z + MAX_JUMP_HEIGHT || !FVisible(m_vMoveToVector))
				{
					edict_t* pBestTarget = BotFunc_FindNearestButton(pev->origin, &pentTouched->v);

					if (pBestTarget)
					{
						const int iNewScheduleId = m_Tasks.GetNewScheduleId();

						if (bIsMoving && bUsingLift)
							// lift is already moving, dont need to press the button anymore
							// remove the lift schedule
						{
							m_Tasks.FinishSchedule(BOT_SCHED_USE_LIFT);
						}
						else if (!bUsingLift)
							// Add the lift schedule
						{
							AddPriorityTask(CBotTask(BOT_TASK_USE, iNewScheduleId, pBestTarget));
							AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pBestTarget));
							m_Tasks.GiveSchedIdDescription(iNewScheduleId, BOT_SCHED_USE_LIFT);
							m_Tasks.SetTimeToCompleteSchedule(iNewScheduleId, RANDOM_FLOAT(10.0, 15.0));
						}
					}
					else
					{
						const float fRange = pentTouched->v.size.Length2D();

						const int iWpt = WaypointFindNearestGoal(GetGunPosition(), m_pEdict, fRange, -1, W_FL_LIFT, &m_FailedGoals);

						if (iWpt != -1)
						{
							char* szClassnames[3] = { "func_button","button_target","func_rot_button" };

							edict_t* pButton = UTIL_FindNearestEntity(szClassnames, 3, WaypointOrigin(iWpt), fRange, TRUE);

							if (pButton)
							{
								const int iScheduleId = m_Tasks.GetNewScheduleId();

								CBotTask* pCurrentTask = m_Tasks.CurrentTask();
								CBotTask CurrentTask = CBotTask(BOT_TASK_NONE);

								if (pCurrentTask)
									CurrentTask = *pCurrentTask;

								m_Tasks.FlushTasks();

								//AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,
								AddTask(CBotTask(BOT_TASK_FIND_PATH, iScheduleId, pButton));
								AddTask(CBotTask(BOT_TASK_USE, iScheduleId, pButton));

								if (CurrentTask.Task() != BOT_TASK_NONE)
									AddTask(CurrentTask);

								m_Tasks.GiveSchedIdDescription(iScheduleId, BOT_SCHED_USE_LIFT);
							}
						}
					}
				}
			}
		}

		if (bIsDoor)
		{
			if (bIsMoving)
			{
				BOOL bWait = TRUE;

				if (m_iCurrentWaypointIndex != -1)
				{
					if (FVisible(WaypointOrigin(m_iCurrentWaypointIndex)))
					{
						float fZcomp = m_vMoveToVector.z - pev->origin.z;

						if (fZcomp < 0)
							fZcomp = -fZcomp;

						bWait = fZcomp >= 36;
					}
				}

				if (bWait)
				{
					const Vector vPentOrigin = EntityOrigin(pentTouched);

					if ((EntityOrigin(pentTouched) - pev->origin).Length2D() > pentTouchedpev->size.Length2D() / 3)
						SetMoveVector(vPentOrigin);
					else
						StopMoving();
				}
			}
			else
			{
				// Use only door
				if (gBotGlobals.IsMod(MOD_DMC) && pentTouched->v.health > 0 || pentTouched->v.spawnflags & 256)
				{
					const Vector vOrigin = EntityOrigin(pentTouched);

					// If the door is blocking a path I can't walk by..
					if (vOrigin.z > pev->absmin.z + 16)
					{
						const CBotTask UseTask = CBotTask(BOT_TASK_USE, 0, pentTouched, -1);

						if (!m_Tasks.HasTask(UseTask))
							AddPriorityTask(UseTask);
					}
				}
			}
		}
	}

	// touching a hurt that hurst me (doesnt give health...)
	/*if ( (strcmp(szClassname,"trigger_hurt") == 0) && (pentTouched->v.dmg > 0) && (!m_pEnemy))
	{
		m_pAvoidEntity = pentTouched; // avoid it...
	}*/

	return FALSE;
}

BOOL BotFunc_EntityIsMoving(entvars_t* pev)
{
	const Vector velocity = pev->velocity;
	const Vector avelocity = pev->avelocity;

	return velocity.x || velocity.y || velocity.z ||
		avelocity.x || avelocity.y || avelocity.z;
}

void CBot::Blocked(edict_t* pentBlocked)
{
	char* szClassname = const_cast<char*>(STRING(pentBlocked->v.classname));

	entvars_t* pentBlockedpev = &pentBlocked->v;

	if (pentBlockedpev->flags & FL_WORLDBRUSH && pentBlockedpev->velocity.z)
	{
		this->m_bKill = TRUE;
	}

	if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_BLOCK_LEVEL))
		DebugMessage(BOT_DEBUG_BLOCK_LEVEL, nullptr, 0, "Block() %s blocked by %s", szClassname, m_szBotName);
}

/*void CBot :: workButtons ()
{
}*/

BOOL CBot::hasBlink() const
{
	return m_Weapons.HasWeapon(m_pEdict, NS_WEAPON_BLINK);
}

void CBot::RunPlayerMove(void)
{
	UpdateMsec();

	m_fLastCallRunPlayerMove = gpGlobals->time;

	UTIL_FixAngles(&pev->angles);
	UTIL_FixAngles(&pev->v_angle);

	if (m_iCurrentWaypointFlags & W_FL_STAY_NEAR)
	{
		m_fIdealMoveSpeed = m_fMaxSpeed / 2;
	}
	else
		m_fIdealMoveSpeed = m_fMaxSpeed;

	//
	// If duck and jump times are in range then duck or jump!
	//{{
	// dont want skulks to accidently fall off walls (crouch lets them unwall stick)
	if (!IsSkulk() && m_iCurrentWaypointFlags & W_FL_CROUCH || m_fStartDuckTime <= gpGlobals->time && m_fEndDuckTime > gpGlobals->time)
	{
		Duck();
	}

	if (m_fStartJumpTime <= gpGlobals->time && m_fEndJumpTime > gpGlobals->time)
		Jump();
	//}}

	if (m_fHoldAttackTime > gpGlobals->time)
		PrimaryAttack();

	(*g_engfuncs.pfnRunPlayerMove)(m_pEdict, pev->angles, m_fMoveSpeed, m_fStrafeSpeed, m_fUpSpeed, pev->button,
		pev->impulse, (byte)m_iMsecVal);
}

void CBot::ThrowGrenade(edict_t* pEnemy, int preference, const BOOL bDontPrime)
{
	// choose error
	const float gren_speed = RANDOM_FLOAT(330.0, 390.0);
	const float dist = DistanceFromEdict(pEnemy);
	float ftime = dist / gren_speed;

	// wont reach before exploding
	if (!bDontPrime && ftime > 3.909)
		return;

	if (!bDontPrime)
	{
		if (ftime < 0.4) // too close
			ftime = 0.4;

		ftime = 3.909 - ftime;
	}
	else
		ftime = 0.1;

	// time to hold grenade before releasing
	m_fGrenadePrimeTime = gpGlobals->time + ftime;

	if (preference == 0)
		preference = RANDOM_LONG(1, 2);

	/// TFC
	if (preference == 1)
	{
		FakeClientCommand(m_pEdict, "+gren1");
		m_iGrenadeHolding = 1;
	}
	else
	{
		FakeClientCommand(m_pEdict, "+gren2");
		m_iGrenadeHolding = 2;
	}

	if (dist < 200.0)
	{
		RunForCover(EntityOrigin(pEnemy), TRUE);
	}
}

BOOL CBot::ThrowingGrenade() const
{
	return m_iGrenadeHolding && m_fGrenadePrimeTime > gpGlobals->time;
}

void CBot::UpdateMsec(void)
{
	m_iMsecVal = (int)((gpGlobals->time - m_fLastCallRunPlayerMove) * 1000);

	if (m_iMsecVal > 255)
		m_iMsecVal = 255;
}

edict_t* CBot::FindEnemy(void)
// Search the bot visibles for a good enemy
// ie. nearby and isn't running fast..
{
	edict_t* pOldEnemy = m_pEnemy;
	// If bot is shooting a structure in NS
	// keep looking for more improtant structures to shoot
	// OR more importantly other players
	if (m_pEnemy && m_fNextGetEnemyTime < gpGlobals->time)
	{
		if (gBotGlobals.IsNS())
		{
			if (EntityIsAlienStruct(m_pEnemy) || EntityIsMarineStruct(m_pEnemy))
			{
				pOldEnemy = m_pEnemy;
				m_pEnemy = nullptr;

				m_fNextGetEnemyTime = gpGlobals->time + RANDOM_FLOAT(0.75, 1.25);
			}
		}
		else if (gBotGlobals.IsMod(MOD_TS))
		{
			pOldEnemy = m_pEnemy;
			m_pEnemy = nullptr;

			m_fNextGetEnemyTime = gpGlobals->time + RANDOM_FLOAT(0.75, 1.25);
		}
	}

	if (m_pEnemy == nullptr)
	{
		// Set up priorities for enemies
		// i.e. which enemies to choose first
		int iBestPriority = 0;

		float fBestFitness = 0;

		//dataStack<edict_t*> tempStack;

		edict_t* pEntity;

		// search list of visibles
		//tempStack = m_stBotVisibles;

		m_pVisibles->resetIter();
		//return NULL;

		while ((pEntity = m_pVisibles->nextVisible()) != nullptr)//!tempStack.IsEmpty() )
		{
			//pEntity = tempStack.ChooseFromStack();

			if (pEntity == pOldEnemy)
				continue;

			if (IsEnemy(pEntity))
			{
				const float fDistance = DistanceFromEdict(pEntity);

				// default enemy priority
				int iPriority = 10;

				entvars_t* pEnemypev = &pEntity->v;

				CBotReputation* pRep = nullptr;

				if (pEnemypev->flags & FL_CLIENT)
				{
					CClient* pEnemyClient = gBotGlobals.m_Clients.GetClientByEdict(pEntity);

					if (pEnemyClient)
					{
						pRep = this->m_Profile.m_Rep.GetRep(pEnemyClient->GetPlayerRepId());

						if (pRep)
						{
							const int iRep = pRep->CurrentRep();

							// add higher priority depending on bad reputation
							iPriority = BOT_MAX_REP - iRep;
						}
					}
				}

				const float fFitness = fDistance + pEnemypev->velocity.Length();

				char* szClassname = const_cast<char*>(STRING(pEnemypev->classname));

				if (strcmp("func_breakable", szClassname) == 0)
				{
					// breakables are less priority than enemy monsters/players etc.
					iPriority = 5;
				}

				switch (gBotGlobals.m_iCurrentMod)
				{
				case MOD_NS:
					// lower priorities for structures.
					if (EntityIsAlienStruct(pEntity))
					{
						// always shoot hive regardless of other players
						// (hive has higher priority than players : 10)
						if (pEntity->v.iuser3 == AVH_USER3_HIVE)
							iPriority = 11;
						else
							iPriority = 8;
					}
					else if (EntityIsMarineStruct(pEntity))
						iPriority = 8;
					break;
				default:
					break;
				}

				if (iPriority >= iBestPriority && (!m_pEnemy || fFitness < fBestFitness))
				{
					m_pEnemy = pEntity;
					fBestFitness = fFitness;
					m_pEnemyRep = pRep;

					// update best priority
					iBestPriority = iPriority;
				}
			}
		}

		if (m_pEnemy)
		{
			m_Tasks.RemoveSimilarTask(BOT_TASK_NORMAL_ATTACK);
			AddPriorityTask(CBotTask(BOT_TASK_NORMAL_ATTACK, m_Tasks.GetNewScheduleId(), m_pEnemy));

			EnemyFound(m_pEnemy);
		}
	}

	if (m_pEnemy)
	{
		if (gBotGlobals.IsMod(MOD_TFC) && pev->playerclass == TFC_CLASS_DEMOMAN && m_bPlacedPipes)
		{
			if ((EntityOrigin(m_pEnemy) - m_vPipeLocation).Length() < 200)
			{
				SecondaryAttack();
				m_bPlacedPipes = FALSE;
			}
		}
	}
	/*
		// if something mucked up
		if ( gBotGlobals.IsNS() )
		{
			if ( m_pEnemy )
			{
				m_CurrentTask = m_Tasks.CurrentTask();

				if ( !m_CurrentTask )
					AddPriorityTask(CBotTask(BOT_TASK_NORMAL_ATTACK,m_Tasks.GetNewScheduleId(),m_pEnemy));
			}
		}
	*/
	return m_pEnemy;
}

BOOL EntityIsMarine(edict_t* pEdict)
{
	switch (pEdict->v.iuser3)
	{
	case AVH_USER3_MARINE_PLAYER:
		return TRUE;
	default:
		break;
	}

	return EntityIsMarineStruct(pEdict);
}

BOOL EntityIsMarineStruct(edict_t* pEdict)
{
	switch (pEdict->v.iuser3)
	{
	case AVH_USER3_COMMANDER_STATION:
	case AVH_USER3_TURRET_FACTORY:
	case AVH_USER3_ARMORY:
	case AVH_USER3_ADVANCED_ARMORY:
	case AVH_USER3_ARMSLAB:
	case AVH_USER3_PROTOTYPE_LAB:
	case AVH_USER3_OBSERVATORY:
	case AVH_USER3_CHEMLAB:
	case AVH_USER3_MEDLAB:
	case AVH_USER3_NUKEPLANT:
	case AVH_USER3_TURRET:
	case AVH_USER3_SIEGETURRET:
	case AVH_USER3_RESTOWER:
	case AVH_USER3_PLACEHOLDER:
	case AVH_USER3_INFANTRYPORTAL:
	case AVH_USER3_NUKE:
	case AVH_USER3_PHASEGATE:
	case AVH_USER3_ADVANCED_TURRET_FACTORY:
		return TRUE;
	default:
		break;
	}

	return FALSE;
}

BOOL EntityIsAlien(edict_t* pEdict)
{
	switch (pEdict->v.iuser3)
	{
	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
	case AVH_USER3_ALIEN_PLAYER4:
	case AVH_USER3_ALIEN_PLAYER5:
	case AVH_USER3_ALIEN_EMBRYO:
		return TRUE;
	default:
		break;
	}
	return EntityIsAlienStruct(pEdict);
}

BOOL EntityIsAlienStruct(edict_t* pEdict)
{
	switch (pEdict->v.iuser3)
	{
	case AVH_USER3_HIVE:
	case AVH_USER3_DEFENSE_CHAMBER:
	case AVH_USER3_MOVEMENT_CHAMBER:
	case AVH_USER3_OFFENSE_CHAMBER:
	case AVH_USER3_SENSORY_CHAMBER:
	case AVH_USER3_ALIENRESTOWER:
	{
		return TRUE;
	}
	default:
		break;
	}
	return FALSE;
}

//
// IS ENEMY
//
// return TRUE if pEntity is an enemy and can be damaged
// return FALSE if not.
BOOL CBot::IsEnemy(edict_t* pEntity)
{
	//	int iUser3 = pEntity->v.iuser3;

	if (pEntity == m_pEdict)
		return FALSE; // can't shoot self...!

	if (pEntity->v.takedamage == DAMAGE_NO)
		return FALSE; // can't shoot it...

	if (pEntity->v.effects & EF_NODRAW)
		return FALSE; // can't see it

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_HL_RALLY:
		return FALSE;
	case MOD_NS:
	{
		const int iBestWeaponId = m_Weapons.GetBestWeaponId(this, pEntity);

		if (iBestWeaponId <= 0)
			return FALSE;

		if (pEntity->v.iuser3 == AVH_USER3_BREAKABLE)
		{
			CBotWeapon* pBestWeapon = m_Weapons.GetWeapon(iBestWeaponId);

			if (pBestWeapon)
			{
				if (pBestWeapon->IsMelee())
				{
					if (EntityOrigin(pEntity).z > pev->origin.z + MAX_JUMP_HEIGHT)
						return FALSE;
				}
			}

			return BotFunc_BreakableIsEnemy(pEntity, m_pEdict);
		}

		if (!EntityIsAlive(pEntity))
			return FALSE;

		///////////////////////////
		//
		// For marine VS marines / aliens VS aliens
		if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ABNORMAL_GAME))
		{
			return (EntityIsMarine(pEntity) || EntityIsAlien(pEntity)) && (pEntity->v.team && pEntity->v.team != pev->team);
		}
		//
		///////////////////////////

		if (IsAlien())
		{
			return EntityIsMarine(pEntity);
		}
		else if (IsMarine())
		{
			if (EntityIsAlien(pEntity))
			{
				if (!(pEntity->v.iuser4 & MASK_VIS_SIGHTED))
					return FALSE;
				if (pEntity->v.iuser4 & MASK_VIS_DETECTED)
				{
					// detected but not seen
					//
				}
				return TRUE;
			}
		}
	}

	break;
	case MOD_DMC:
	{
		if (!EntityIsAlive(pEntity))
			return FALSE;

		return pEntity->v.flags & FL_CLIENT;
	}
	break;
	case MOD_BUMPERCARS:
		if (!EntityIsAlive(pEntity))
			return FALSE;

		if (pEntity->v.flags & FL_MONSTER && !strcmp("monster_human_grunt", STRING(pEntity->v.classname)))
			return TRUE;

		return pEntity->v.flags & FL_CLIENT;
		break;
	case MOD_TS:
		if (!EntityIsAlive(pEntity))
			return FALSE;

		if (pEntity->v.flags & FL_CLIENT)
		{
			const float team = gBotGlobals.isMapType(NON_TFC_TS_TEAMPLAY) || gBotGlobals.m_bTeamPlay;

			if (team)
			{
				char* infobuffer1 = (*g_engfuncs.pfnGetInfoKeyBuffer)(pEntity);
				char* infobuffer2 = (*g_engfuncs.pfnGetInfoKeyBuffer)(m_pEdict);
				char model1[64];
				char model2[64];

				strcpy(model1, g_engfuncs.pfnInfoKeyValue(infobuffer1, "model"));
				strcpy(model2, g_engfuncs.pfnInfoKeyValue(infobuffer2, "model"));

				return strcmp(model1, model2) != 0;
			}

			return TRUE;
		}
		else
		{
			char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));

			if (strcmp(szClassname, "func_breakable") == 0)
			{
				edict_t* pPlayer = nullptr;
				const Vector origin = EntityOrigin(pEntity);

				while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, origin, 200)) != nullptr)
				{
					if (pPlayer->v.flags & FL_CLIENT)
					{
						if (pPlayer == m_pEdict)
						{
							if (DistanceFrom(origin) < 80)
								break;
						}
						else
							break;
					}
				}

				return (m_iTS_State != TS_State_Stunt && pPlayer == m_pEdict || pPlayer != m_pEdict && pPlayer != nullptr && IsInVisibleList(pPlayer) && IsEnemy(pPlayer)) && BotFunc_BreakableIsEnemy(pEntity, m_pEdict);
			}
		}

		break;
	case MOD_RC:
	case MOD_RC2:
		if (!EntityIsAlive(pEntity))
			return FALSE;

		if (pEntity->v.flags & FL_CLIENT)
			return TRUE;

		if (pEntity->v.flags & FL_MONSTER)
		{
			char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));

			if (FStrEq(szClassname, "monster_barney"))
				return FALSE;
			else if (FStrEq(szClassname, "monster_scientist"))
				return FALSE;
			else if (FStrEq(szClassname, "monster_gman"))
				return FALSE;
			else if (FStrEq(szClassname, "monster_furniture"))
				return FALSE;
			else if (FStrEq(szClassname, "monster_tentacle"))
				return FALSE;

			return TRUE;
		}
		break;
	case MOD_HL_DM:
	{
		char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));

		if (strcmp(szClassname, "func_breakable") == 0)
		{
			return BotFunc_BreakableIsEnemy(pEntity, m_pEdict);
		}
		if (!EntityIsAlive(pEntity))
			return FALSE;
		if (!gBotGlobals.m_bTeamPlay)
			return pEntity->v.flags & FL_CLIENT;
		else if (pEntity->v.flags & FL_CLIENT)  // different model for team play
		{
			char* infobuffer1 = (*g_engfuncs.pfnGetInfoKeyBuffer)(m_pEdict);
			char* infobuffer2 = (*g_engfuncs.pfnGetInfoKeyBuffer)(pEntity);

			const char* model1 = static_cast<const char*>(g_engfuncs.pfnInfoKeyValue(infobuffer1, "model"));
			const char* model2 = static_cast<const char*>(g_engfuncs.pfnInfoKeyValue(infobuffer2, "model"));

			return !FStrEq(model1, model2);
		}
	}
	break;
	case MOD_BG:
	{
		char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));

		// bot cant see next waypoint, breakable could be blocking it
		if (!HasCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT))
		{
			if (strcmp(szClassname, "func_breakable") == 0)
			{
				return BotFunc_BreakableIsEnemy(pEntity, m_pEdict);
			}
		}
		// different teams are enemies
		return pEntity->v.flags & FL_CLIENT && GetTeam() != UTIL_GetTeam(pEntity);
	}
	break;
	/*case MOD_SVENCOOP:
	{
		char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));
		int iBestWeapon;
		CBotWeapon* pWeapon;
		entvars_t* pEnemypev = &pEntity->v;

		if (pEnemypev->flags & FL_CLIENT)
			return FALSE;

		if (!EntityIsAlive(pEntity))
			return FALSE;

		if ((iBestWeapon = m_Weapons.GetBestWeaponId(this, pEntity)) <= 0)
			return FALSE;

		pWeapon = m_Weapons.GetWeapon(iBestWeapon);

		if (pEnemypev->flags & FL_MONSTER)
		{
			CBaseEntity* pEnt = static_cast<CBaseEntity*>(GET_PRIVATE(pEntity));

			int iClass = pEnt->Classify();

			switch (iClass)
			{
			case CLASS_NONE:
			case CLASS_PLAYER:
			case CLASS_HUMAN_PASSIVE:
			case CLASS_INSECT:
			case CLASS_PLAYER_ALLY:
				//if ( UTIL_FriendlyHatesPlayer(pEntity,m_pEdict) )
				//	return TRUE;
			case CLASS_PLAYER_BIOWEAPON: // hornets and snarks.launched by players
			case CLASS_ALIEN_BIOWEAPON:
			{
				return FALSE;
			}
			case CLASS_MACHINE:
			{
				if (FStrEq("monster_turret", szClassname))
				{
					if (!pEnemypev->sequence)
						return FALSE;
				}
				return TRUE;
			}
			case CLASS_ALIEN_PASSIVE:
			case CLASS_BARNACLE:
			case CLASS_ALIEN_MILITARY:
			case CLASS_ALIEN_MONSTER:
			case CLASS_ALIEN_PREY:
			case CLASS_ALIEN_PREDATOR:
			case CLASS_HUMAN_MILITARY:
				return TRUE;		
			default:
				break;
			}

			if (FStrEq(szClassname, "monster_generic"))
				return FALSE;
			else if (FStrEq(szClassname, "monster_tentacle")) // tentacle things dont die
				return FALSE;

			return TRUE;
		}
		else if (strcmp(szClassname, "func_breakable") == 0)
		{
			return BotFunc_BreakableIsEnemy(pEntity, m_pEdict);
			
			// i. explosives required to blow breakable
			// ii. OR is not a world brush (non breakable) and can be broken by shooting
			if ( !(pEnemypev->flags & FL_WORLDBRUSH) && !(pEntity->v.spawnflags & SF_BREAK_TRIGGER_ONLY) )
			{
				Vector vSize = pEnemypev->size;

				if ( (*STRING(pEnemypev->target)) ||
					 (vSize.x > 24) ||
					 (vSize.y > 24) ||
					 (vSize.z > 36) )
				{
					// Only shoot breakables that are bigger than me (crouch size)
					// or that target something...
					return TRUE;
				}
			}
		}
		else if (strcmp(szClassname, "func_button") == 0)
		{
			if (m_fNextShootButton < gpGlobals->time)
			{
				if (!pEntity->v.frame && pEntity->v.health > 0)
				{
					//m_fNextShootButton = gpGlobals->time + RANDOM_FLOAT(1.0,2.0);

					return TRUE;
				}
			}
		}
	}
	break;*/
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// NON-CLASS RELATED FUNCTIONS

BOOL BotFunc_IsLongRangeWeapon(const int iId)
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		switch (iId)
		{
		case NS_WEAPON_PISTOL:
		case NS_WEAPON_MG:
		case NS_WEAPON_SONIC:
		case NS_WEAPON_HMG:
		case NS_WEAPON_GRENADE_GUN:
		case NS_WEAPON_SPORES:
		case NS_WEAPON_METABOLIZE:
		case NS_WEAPON_PARASITE:
		case NS_WEAPON_DIVINEWIND:
		case NS_ABILITY_LEAP:
		case NS_WEAPON_UMBRA:
		case NS_WEAPON_PRIMALSCREAM:
		case NS_WEAPON_BILEBOMB:
		case NS_WEAPON_ACIDROCKET:
		case NS_WEAPON_HEALINGSPRAY:
		case NS_WEAPON_BABBLER:
		case NS_WEAPON_STOMP:
		case NS_WEAPON_DEVOUR:
			return TRUE;
		default:
			return FALSE;
		}
	default:
		return FALSE;
	}

	return FALSE;
}

// crap function, doesn't work argh!
// USE STRDUP() INSTEAD!!!
/*
void BotFunc_StringCopy(char *szCopyTo, const char *szCopyFrom)
// Copy a string to an uninitialised string pointer
// First initialise pointer, then copy.
{
	szCopyTo = (char*)malloc((sizeof(char)*strlen(szCopyFrom))+1);
	strcpy(szCopyTo,szCopyFrom);
}
*/

BOOL EntityIsBuildable(edict_t* pEdict)
{
	if (pEdict->v.iuser3 == AVH_USER3_HIVE)
		return FALSE;

	return pEdict->v.iuser4 & MASK_BUILDABLE;
}

BOOL CBot::CanBuild(edict_t* pEdict, int* metal)
{
	if (gBotGlobals.IsNS())
	{
		return EntityIsBuildable(pEdict);
	}

	return TRUE;
}

BOOL EntityIsAlive(edict_t* pEdict)
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		if (pEdict->v.iuser3 == AVH_USER3_HIVE)
			return !UTIL_CanBuildHive(&pEdict->v);
		break;
	default:
		break;
	}

	return pEdict->v.deadflag == DEAD_NO &&
		pEdict->v.health > 0;
}

void BugMessage(edict_t* pEntity, char* fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	BotMessage(pEntity, 0, "%s%s%s%s", "BUG: ", string, " Report bugs to : ", BOT_AUTHOR);

	UTIL_LogPrintf("%s%s%s%s", "BUG: ", string, " Report bugs to : ", BOT_AUTHOR);
}

void AssertMessage(const BOOL bAssert, char* fmt, ...)
{
	if (!bAssert)
	{
		va_list argptr;
		static char string[1024];

		va_start(argptr, fmt);
		vsprintf(string, fmt, argptr);
		va_end(argptr);

		BugMessage(nullptr, "Assertion Failed : %s", string);
	}
}

void DebugMessage(int iDebugLevel, edict_t* pEntity, int errorlevel, char* fmt, ...)
{
	va_list argptr;
	static char string[1024];
	char szDebugMsg[32];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	switch (iDebugLevel)
	{
	case BOT_DEBUG_TOUCH_LEVEL:
		// Bot touched object
		sprintf(szDebugMsg, "%s:TOUCH]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_THINK_LEVEL:
		// Bot thinks
		sprintf(szDebugMsg, "%s:THINK]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_HEAR_LEVEL:
		// Bot hears a sound
		sprintf(szDebugMsg, "%s:HEAR]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_MESSAGE_LEVEL:
		// Bot recieves net message
		sprintf(szDebugMsg, "%s:MESSAGE]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_BLOCK_LEVEL:
		// Bot blocks object
		sprintf(szDebugMsg, "%s:BLOCK]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_MOVE_LEVEL:
		// Bot moves somewhere
		sprintf(szDebugMsg, "%s:MOVE]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_AIM_LEVEL:
		// Bot aims at something
		sprintf(szDebugMsg, "%s:AIM]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_NAV_LEVEL:
		// Bot touches/finds waypoints
		sprintf(szDebugMsg, "%s:NAV]=>", BOT_DEBUG_TAG);
		break;
	case BOT_DEBUG_SEE_LEVEL:
		// Bot touches/finds waypoints
		sprintf(szDebugMsg, "%s:SEE]=>", BOT_DEBUG_TAG);
		break;
	default: ;
	}

	BotMessage(pEntity, errorlevel, "%s%s", szDebugMsg, string);
}

void BotPrintTalkMessage(char* fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	edict_t* pPlayer;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CClient* pClient = gBotGlobals.m_Clients.GetClientByIndex(i);

		if (!pClient->IsUsed())
			continue;

		if ((pPlayer = pClient->GetPlayer()) == nullptr)
			continue;

		BotPrintTalkMessageOne(pPlayer, string);
	}
}

void BotPrintTalkMessageOne(edict_t* pClient, char* fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	if (pClient == nullptr)
		return;

	if (pClient->v.flags & FL_FAKECLIENT)
		return;

	ClientPrint(pClient, HUD_PRINTTALK, string);
}

void BotFile_Write(char* string)
{
	FILE* fp = fopen(BOT_CRASHLOG_FILE, "a");

	if (fp)
	{
		char time_str[512];
		time_t ltime = time(nullptr);
		struct tm* today = localtime(&ltime);

		strftime(time_str, sizeof time_str, "%m/%d/%Y %H:%M:%S", today);

		fprintf(fp, "%s => ", time_str);
		fprintf(fp, "%s\n", string);

		fclose(fp);
	}
}

void BotMessage(edict_t* pEntity, int errorlevel, char* fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	if (pEntity != nullptr)
	{
		if (IS_DEDICATED_SERVER())
		{
			printf("%s%s Message : %s\n", BOT_DBG_MSG_TAG, STRING(pEntity->v.netname), string);
		}

		if (!gBotGlobals.m_bNetMessageStarted)
			// cant do a message at the moment
		{
			ClientPrint(pEntity, HUD_PRINTCONSOLE, BOT_DBG_MSG_TAG);
			ClientPrint(pEntity, HUD_PRINTCONSOLE, string);
			ClientPrint(pEntity, HUD_PRINTCONSOLE, "\n");
		}
		else
			ALERT(at_console, string);
	}
	else
	{
		if (errorlevel)
		{
			switch (errorlevel)
			{
			case 1:
			{
				UTIL_LogPrintf("!!!!ERROR : %s%s\n", BOT_DBG_MSG_TAG, string);

				ALERT(at_console, BOT_DBG_MSG_TAG);

				BotFile_Write(string);
				ALERT(at_error, string);

#ifndef __linux__
				MessageBox(nullptr, string, "RCBot Error", MB_OK);
#endif
				exit(0);
			}
			break;
			case 2:
				UTIL_LogPrintf("!!!!ERROR : %s%s\n", BOT_DBG_MSG_TAG, string);

				ALERT(at_console, BOT_DBG_MSG_TAG);

				BotFile_Write(string);
				ALERT(at_error, string);

#ifndef __linux__
				MessageBox(nullptr, string, "RCBot Error", MB_OK);
#endif
				exit(0);

				break;
			default:
				ALERT(at_console, "BotMessage() : errorlevel invalid\n");
			}
		}
		else
		{
			g_engfuncs.pfnServerPrint(BOT_DBG_MSG_TAG);
			g_engfuncs.pfnServerPrint(string);
			g_engfuncs.pfnServerPrint("\n");
			/*
			if ( IS_DEDICATED_SERVER() )
			{
			//	g_engfuncs.pfnServerPrint( string );
			printf("%s",BOT_DBG_MSG_TAG);
				printf("%s",string);
			}
			else
			{
				ALERT(at_console,BOT_DBG_MSG_TAG);
				ALERT(at_console,string);
			}				*/
		}

		/*if ( IS_DEDICATED_SERVER() )
			printf("\n");
		else
			ALERT(at_console,"\n");*/
	}
}

///////////////////////////////////////////////////////////////////////////
// BOT TASK EQUAL OPERATOR...
BOOL CBotTask :: operator == (const CBotTask& Task) const
{
	return m_Task == Task.Task() &&
		m_iInfo == Task.TaskInt() &&
		m_fInfo == Task.TaskFloat() &&
		m_vInfo == Task.TaskVector() &&
		TaskEdict() == Task.TaskEdict();
}

BOOL EntityIsSelectable(edict_t* pEdict)
// useful for bot commanders, if ever, they will know what they can select
{
	return pEdict->v.iuser4 & MASK_SELECTABLE;
}

BOOL EntityIsCommander(edict_t* pEdict)
{
	return pEdict == gBotGlobals.GetCommander();
}

BOOL EntityIsWeldable(edict_t* pEntity)
// returns true if an entity can be welded using the welder in NS
{
	if (EntityIsBuildable(pEntity))
		return FALSE; // gotta build it first
	if (EntityIsAlien(pEntity))
		return FALSE;
	if (pEntity->v.iuser3 == AVH_USER3_WELD)
	{
		if (pEntity->v.fuser1 != -1000.0 && pEntity->v.fuser2 != 1000.0)
			return TRUE;
	}
	if (!EntityIsSelectable(pEntity))
		return FALSE;
	if (EntityIsMarineStruct(pEntity) && pEntity->v.fuser2 < 1000)
		return TRUE;
	if (pEntity->v.iuser3 == AVH_USER3_MARINE_PLAYER)
	{
		if (EntityIsCommander(pEntity))
			return FALSE;

		return pEntity->v.velocity.Length() < 1 && EntityIsAlive(pEntity) && pEntity->v.armorvalue < UTIL_NS_GetMaxArmour(pEntity);
	}

	if (strcmp("team_webstrand", STRING(pEntity->v.classname)) == 0)
		return TRUE;

	return FALSE;
}

void CBot::HearSound(const eSoundType iSound, Vector const vOrigin, edict_t* pEdict)
// bot hears a type of sound
{
	CBotTask TaskToAdd = CBotTask(BOT_TASK_NONE);

	switch (iSound)
	{
	case SOUND_UNKNOWN:
		break;
	case SOUND_PLAYER_PAIN:
	{
		CBotTask* currentTask = m_Tasks.CurrentTask();

		if (currentTask && currentTask->TaskEdict())
		{
			if (currentTask->TaskEdict() == pEdict)
				UpdateCondition(BOT_CONDITION_TASK_EDICT_PAIN);
		}
	}
	break;
	case SOUND_PLAYER:
		if (IsEnemy(pEdict) && !m_pEnemy)
		{
			// Dont hear stuff when trying to concentrate on the human tower :p
			if (!m_CurrentTask || m_CurrentTask->Task() != BOT_TASK_HUMAN_TOWER)
				TaskToAdd = CBotTask(BOT_TASK_LISTEN_TO_SOUND, m_Tasks.GetNewScheduleId(), nullptr, 0, 0, EntityOrigin(pEdict));
		}
		break;
	case SOUND_DOOR:
		// if ever, you can make the bot react to doors moving.
		break;
	case SOUND_WEAPON:
		break;
	case SOUND_FOLLOW:
	case SOUND_NEEDBACKUP:
		if (!m_stSquad && !IsEnemy(pEdict))
		{
			m_stSquad = gBotGlobals.m_Squads.AddSquadMember(pEdict, m_pEdict);
			if (m_stSquad)
				TaskToAdd = CBotTask(BOT_TASK_FOLLOW_LEADER, m_Tasks.GetNewScheduleId(), pEdict);
		}
		break;
	case SOUND_NEEDHEALTH:
	{
		if (pEdict && pEdict != m_pEdict && IsInVisibleList(pEdict))
		{
			if (!m_pEnemy && !m_Tasks.HasTask(CBotTask(BOT_TASK_HEAL_PLAYER, 0, pEdict)))
			{
				BOOL bCanHeal = FALSE;

				if (gBotGlobals.IsNS())
				{
					bCanHeal = IsGorge() && HasWeapon(NS_WEAPON_HEALINGSPRAY);
				}

				if (bCanHeal)
				{
					const int iNewScheduleId = m_Tasks.GetNewScheduleId();

					AddPriorityTask(CBotTask(BOT_TASK_HEAL_PLAYER, iNewScheduleId, pEdict));
					AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pEdict));

					break;
				}
			}
		}
	}
	break;
	case SOUND_TAKE_COVER:
	{//moomoo2
		if (pEdict && pEdict != m_pEdict && IsInVisibleList(pEdict))
		{
			if (!m_Tasks.HasSchedule(BOT_SCHED_RUN_FOR_COVER))
			{
				RunForCover(vOrigin);
				break;
			}
		}
	}
	break;
	case SOUND_NEEDAMMO:
		break;
	case SOUND_COVERING:
		break;
	case SOUND_TAUNT:
		break;
	case SOUND_INPOSITION:
		break;
	case SOUND_INCOMING:
		if (!IsEnemy(pEdict))
		{
			if (gBotGlobals.m_iCurrentMod != MOD_NS || !IsGorge())
				TaskToAdd = CBotTask(BOT_TASK_FIND_PATH, m_Tasks.GetNewScheduleId(), pEdict);
		}
		//else
		//	TaskToAdd = CBotTask(BOT_TASK_LISTEN_TO_SOUND,pEdict,iSound,0,vOrigin);
		break;
	case SOUND_MOVEOUT:
		// NS player radio
		break;
	case SOUND_ALLCLEAR:
		// NS player radio
		break;
	case SOUND_HUD:
		break;
	case SOUND_BUTTON:
		// someone (or something!) pressed a button
		break;
	case SOUND_TURRETS:
		// turrets firing in NS
		break;
	case SOUND_MISC:
		break;
	case SOUND_ATTACK:
		// NS alien 'radio' message
		break;
	case SOUND_BUILDINGHERE:
		// NS alien 'radio' message
		break;
	}

	if (TaskToAdd.Task() != BOT_TASK_NONE)
		AddPriorityTask(TaskToAdd);
}

edict_t* BotFunc_FindNearestButton(Vector vOrigin, entvars_t* pDoor, Vector* vFoundOrigin)
//
// finds the nearest button to the door pDoor
{
	float fNearest = 0;

	edict_t* pBestTarget = nullptr;

	char* szTargetname = const_cast<char*>(STRING(pDoor->targetname));

	TraceResult tr;

	// check the masters list first
	edict_t* pTarget = gBotGlobals.m_Masters.GetButtonForEntity(ENT(pDoor), vOrigin);

	if (pTarget)
		return pTarget;

	pTarget = nullptr;

	while ((pTarget = UTIL_FindEntityByString(pTarget, "target", szTargetname)) != nullptr)
	{
		Vector vEntityOrigin = EntityOrigin(pTarget);

		const float fDist = (vEntityOrigin - vOrigin).Length();

		if (pBestTarget == nullptr || fDist < fNearest)
		{
			// if it's svencoop, a lot of doors are opened by buttons through obstacles
			// so don't check if it is visible or not.
			if (pTarget->v.solid != SOLID_TRIGGER)
			{
				UTIL_TraceLine(vOrigin, vEntityOrigin, ignore_monsters, nullptr, &tr);

				if (tr.pHit == pTarget || tr.flFraction >= 1.0)
				{
					pBestTarget = pTarget;
					fNearest = fDist;

					if (vFoundOrigin)
						*vFoundOrigin = vEntityOrigin;

					break;
				}

				// try absmin/absmax for those tedious maps
				UTIL_TraceLine(vOrigin, vEntityOrigin + pTarget->v.size, ignore_monsters, nullptr, &tr);

				if (tr.pHit == pTarget || tr.flFraction >= 1.0)
				{
					pBestTarget = pTarget;
					fNearest = fDist;

					if (vFoundOrigin)
						*vFoundOrigin = vEntityOrigin + pTarget->v.size;

					break;
				}

				UTIL_TraceLine(vOrigin, vEntityOrigin - pTarget->v.size, ignore_monsters, nullptr, &tr);

				if (tr.pHit == pTarget || tr.flFraction >= 1.0)
				{
					pBestTarget = pTarget;
					fNearest = fDist;

					if (vFoundOrigin)
						*vFoundOrigin = vEntityOrigin - pTarget->v.size;

					break;
				}
			}
			else
			{
				pBestTarget = pTarget;
				fNearest = fDist;
			}
		}
	}

	if (pBestTarget)
	{
		// sometimes we might get things that are not buttons, like triggers
		if (!UTIL_IsButton(pBestTarget))
			return nullptr;
	}

	return pBestTarget;
}

BOOL CBot::SentryNeedsRepaired() const
{
	CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(m_pEdict);

	if (pClient)
	{
		edict_t* pSentry = pClient->getTFCSentry();

		if (pSentry == nullptr)
		{
			return FALSE;
		}

		return pSentry->v.health < pSentry->v.max_health * 0.75;
	}
	return FALSE;
}

BOOL CBot::HasUpgraded(int iUpgrade) const
{
	// returns TRUE if this alien bot in NS has upgraded to a specific trait yet
	switch (iUpgrade)
	{
	case BOT_UPGRADE_DEF:
		return pev->iuser4 & MASK_UPGRADE_1 ||
			pev->iuser4 & MASK_UPGRADE_2 || // Needs a def upgrade?
			pev->iuser4 & MASK_UPGRADE_3;
	case BOT_UPGRADE_SEN:
		return pev->iuser4 & MASK_UPGRADE_4 || // Needs movement upgrade?
			pev->iuser4 & MASK_UPGRADE_5 ||
			pev->iuser4 & MASK_UPGRADE_6;
		break;
	case BOT_UPGRADE_MOV:
		return pev->iuser4 & MASK_UPGRADE_7 ||
			pev->iuser4 & MASK_UPGRADE_8 ||
			pev->iuser4 & MASK_UPGRADE_9;
		break;
	default:
		break;
	}

	return FALSE;
}

void CBot::DuckAndJump(void)
// Makes bot duck first and then jump
// if the bot has a long jump module
// then it will do a long jump.
{
	m_fStartJumpTime = gpGlobals->time + 0.1;
	m_fEndJumpTime = gpGlobals->time + 0.2;

	m_fStartDuckTime = gpGlobals->time;
	m_fEndDuckTime = gpGlobals->time + 0.2;
}

void CBot::JumpAndDuck(void)
// makes bot jump and crouch to jump a bit
// higher
{
	if (m_fEndJumpTime < gpGlobals->time)
	{
		/*if (gBotGlobals.IsMod(MOD_SVENCOOP))
		{
			if (m_pCurrentWeapon && m_pCurrentWeapon->GetID() == SVEN_WEAPON_MINIGUN)
				FakeClientCommand(m_pEdict, "drop");
		}*/

		m_fStartJumpTime = gpGlobals->time;
		m_fEndJumpTime = gpGlobals->time + 0.1;
	}
	if (m_fEndDuckTime < gpGlobals->time)
	{
		m_fStartDuckTime = gpGlobals->time + 0.1;
		m_fEndDuckTime = gpGlobals->time + 0.7;
	}
}

// NATURAL SELECTION
//------------------------
// HasVisitedResourceTower
// AddVisitedResourceTower.
BOOL CBot::HasVisitedResourceTower(edict_t* pEdict)
{
	const int iMax = (int)m_iVisitedFuncResources;

	for (int i = 0; i < iMax; i++)
	{
		if (m_pVisitedFuncResources[i] == pEdict)
			return TRUE;
	}

	return FALSE;
}

// Make gorges hold an array of resource fountains/towers theyve already visited
// so they know not to visit them again for a while.
void CBot::AddVisitedResourceTower(edict_t* pEdict)
{
	const int iMax = (int)m_iVisitedFuncResources;

	for (int i = 0; i < iMax; i++)
	{
		if (m_pVisitedFuncResources[i] == pEdict)
			return;
	}

	if (iMax > MAX_REMEMBER_POSITIONS)
	{
		m_pVisitedFuncResources[RANDOM_LONG(0, MAX_REMEMBER_POSITIONS - 1)];
	}
	else
	{
		m_pVisitedFuncResources[m_iVisitedFuncResources++] = pEdict;
	}

	return;
}

// Remember Position of enemy or something else important
void CBot::RememberPosition(const Vector vOrigin, edict_t* pEntity, int flags)
{
	m_vRememberedPositions.addPosition(vOrigin, pEntity, flags, pev->origin);
}

void CBot::AddTask(const CBotTask& Task)
{
	m_Tasks.AddTaskToEnd(Task);
}

void CBot::AddPriorityTask(const CBotTask& Task)
{
	m_Tasks.RemovePaths();
	m_Tasks.AddTaskToFront(Task);
}

void CBot::StopMoving(void)
{
	m_bMoveToIsValid = FALSE;
}

void CBot::SetMoveVector(const Vector& vOrigin)
{
	m_vMoveToVector = vOrigin;
	m_bMoveToIsValid = TRUE;
}

// Get proper Id value for weapon, DMC mucks things up
int BotFunc_GetBitSetOf(const int iId)
{
	int weapon_index = 0;
	int value = iId;

	while (value)
	{
		weapon_index++;
		value = value >> 1;
	}

	return weapon_index;
}

BOOL CBot::HasWeapon(const int iWeapon) const
{
	//	if ( gBotGlobals.IsMod(MOD_DMC) )
	//		return ((m_iBotWeapons) & (1<<(iWeapon-1))) != 0;

	return (m_iBotWeapons & 1 << iWeapon) != 0;
}

// Change a leader of a squad, this can cause lots of effects
void CBotSquads::ChangeLeader(CBotSquad* theSquad)
{
	// first change leader to next squad member
	theSquad->ChangeLeader();

	// if no leader anymore/no members in group
	if (theSquad->IsLeader(nullptr))
	{
		// make sure any bots who has this squad set
		// has their squad removed/cleared.
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CBot* pBot = &gBotGlobals.m_Bots[i];

			if (pBot->InSquad(theSquad))
			{
				pBot->ClearSquad();
			}
		}

		// must also remove from stack of available squads.
		m_theSquads.Remove(theSquad);
	}
}

Vector CBotSquad::GetFormationVector(edict_t* pEdict) const
{
	Vector vBase;

	edict_t* pLeader = GetLeader();

	const int iPosition = GetFormationPosition(pEdict);
	const Vector vLeaderOrigin = EntityOrigin(pLeader);

	const int iMod = iPosition % 2;

	UTIL_MakeVectors(m_vLeaderAngle); // leader body angles as base

	// going to have members on either side.
	switch (m_theDesiredFormation)
	{
	case SQUAD_FORM_VEE:
	{
		if (iMod)
			vBase = gpGlobals->v_forward - gpGlobals->v_right;
		else
			vBase = gpGlobals->v_forward + gpGlobals->v_right;
	}
	break;
	case SQUAD_FORM_WEDGE:
	{
		if (iMod)
			vBase = -(gpGlobals->v_forward - gpGlobals->v_right);
		else
			vBase = -(gpGlobals->v_forward + gpGlobals->v_right);
	}
	break;
	case SQUAD_FORM_LINE:
	{
		// have members on either side of leader

		if (iMod)
			vBase = gpGlobals->v_right;
		else
			vBase = -gpGlobals->v_right;
	}
	break;
	case SQUAD_FORM_COLUMN:
	{
		vBase = -gpGlobals->v_forward;
	}
	break;
	case SQUAD_FORM_ECH_LEFT:
	{
		vBase = -gpGlobals->v_forward - gpGlobals->v_right;
	}
	break;
	case SQUAD_FORM_ECH_RIGHT:
	{
		vBase = -gpGlobals->v_forward + gpGlobals->v_right;
	}
	break;
default: ;
	}

	vBase = vBase * m_fDesiredSpread * iPosition;

	TraceResult tr;

	UTIL_TraceLine(vLeaderOrigin, vLeaderOrigin + vBase, ignore_monsters, dont_ignore_glass, pLeader, &tr);

	vBase = tr.vecEndPos;

	if (tr.flFraction < 1.0)
	{
		const float fDist = vBase.Length();
		vBase = vBase.Normalize();

		vBase = vBase * (fDist - 32);
	}

	return vBase;
}

// AddSquadMember can have many effects
// 1. scenario: squad leader exists as squad leader
//              assign bot to squad
// 2. scenario: 'squad leader' exists as squad member in another squad
//              assign bot to 'squad leaders' squad
// 3. scenario: no squad has 'squad leader'
//              make a new squad
CBotSquad* CBotSquads::AddSquadMember(edict_t* pLeader, edict_t* pMember)
{
	dataStack<CBotSquad*> tempStack = m_theSquads;
	CBotSquad* theSquad;
	CBot* pBot;

	char msg[120];

	if (!pLeader)
		return nullptr;

	if (UTIL_GetTeam(pLeader) != UTIL_GetTeam(pMember))
		return nullptr;

	CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pLeader);

	if (pClient)
	{
		pClient->AddNewToolTip(BOT_TOOL_TIP_SQUAD_HELP);
	}

	sprintf(msg, "%s %s has joined your squad", BOT_DBG_MSG_TAG, STRING(pMember->v.netname));
	ClientPrint(pLeader, HUD_PRINTTALK, msg);

	while (!tempStack.IsEmpty())
	{
		theSquad = tempStack.ChooseFromStack();

		if (theSquad->IsLeader(pLeader))
		{
			theSquad->AddMember(pMember);
			tempStack.Init();
			return theSquad;
		}
		else if (theSquad->IsMember(pLeader))
		{
			theSquad->AddMember(pMember);
			tempStack.Init();
			return theSquad;
		}
	}

	// no squad with leader, make one

	theSquad = new CBotSquad(pLeader, pMember);

	if (theSquad != nullptr)
	{
		m_theSquads.Push(theSquad);

		if ((pBot = UTIL_GetBotPointer(pLeader)) != nullptr)
			pBot->SetSquad(theSquad);
	}

	return theSquad;
}

void CBot::AddToSquad(edict_t* pLeader)
{
	m_stSquad = gBotGlobals.m_Squads.AddSquadMember(pLeader, m_pEdict);
	if (m_stSquad)
		AddPriorityTask(CBotTask(BOT_TASK_FOLLOW_LEADER, m_Tasks.GetNewScheduleId(), pLeader));
}

BOOL CBot::CanAddToSquad(edict_t* pLeader)
{
	return !m_stSquad && !IsEnemy(pLeader) && GetTeam() == UTIL_GetTeam(pLeader);
}

void CBotSquads::RemoveSquad(CBotSquad* pSquad)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBot* pBot = &gBotGlobals.m_Bots[i];

		if (pBot->InSquad(pSquad))
			pBot->ClearSquad();
	}

	m_theSquads.Remove(pSquad);

	//	if (pSquad != NULL)
	delete pSquad;
}

BOOL CBot::RemoveMySquad(void)
// return TRUE if a squad was removed
{
	if (!m_stSquad)
		return FALSE;

	if (m_stSquad->IsLeader(m_pEdict))
	{
		gBotGlobals.m_Squads.RemoveSquad(m_stSquad);
		m_stSquad = nullptr;
		return TRUE;
	}

	return FALSE;
}
/*
void CBot :: BotOnLadder ( void )
{
	Vector v_src, v_dest, view_angles;
	TraceResult tr;
	float angle = 0.0;
	bool done = FALSE;

	// check if the bot has JUST touched this ladder...
	if (m_siLadderDir == LADDER_UNKNOWN)
	{
		// try to square up the bot on the ladder...
		while ((!done) && (angle < 180.0))
		{
			// try looking in one direction (forward + angle)
			view_angles = pev->v_angle;
			view_angles.y = pev->v_angle.y + angle;

			if (view_angles.y < 0.0)
				view_angles.y += 360.0;
			if (view_angles.y > 360.0)
				view_angles.y -= 360.0;

			UTIL_MakeVectors( view_angles );

			v_src = pev->origin + pev->view_ofs;
			v_dest = v_src + gpGlobals->v_forward * 30;

			UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
				pev->pContainingEntity, &tr);

			if (tr.flFraction < 1.0)  // hit something?
			{
				if (strcmp("func_wall", STRING(tr.pHit->v.classname)) == 0)
				{
					// square up to the wall...
					view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);

					// Normal comes OUT from wall, so flip it around...
					view_angles.y += 180;

					if (view_angles.y > 180)
						view_angles.y -= 360;

					pev->ideal_yaw = view_angles.y;

					UTIL_FixFloatAngle(&pev->ideal_yaw);

					done = TRUE;
				}
			}
			else
			{
				// try looking in the other direction (forward - angle)
				view_angles = pev->v_angle;
				view_angles.y = pev->v_angle.y - angle;

				if (view_angles.y < 0.0)
					view_angles.y += 360.0;
				if (view_angles.y > 360.0)
					view_angles.y -= 360.0;

				UTIL_MakeVectors( view_angles );

				v_src = pev->origin + pev->view_ofs;
				v_dest = v_src + gpGlobals->v_forward * 30;

				UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
					pev->pContainingEntity, &tr);

				if (tr.flFraction < 1.0)  // hit something?
				{
					if (strcmp("func_wall", STRING(tr.pHit->v.classname)) == 0)
					{
						// square up to the wall...
						view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);

						// Normal comes OUT from wall, so flip it around...
						view_angles.y += 180;

						if (view_angles.y > 180)
							view_angles.y -= 360;

						pev->ideal_yaw = view_angles.y;

						UTIL_FixFloatAngle(&pev->ideal_yaw);

						done = TRUE;
					}
				}
			}

			angle += 10;
		}

		if (!done)  // if didn't find a wall, just reset ideal_yaw...
		{
			// set ideal_yaw to current yaw (so bot won't keep turning)
			pev->ideal_yaw = pev->v_angle.y;

			UTIL_FixFloatAngle(&pev->ideal_yaw);
		}
	}

	// moves the bot up or down a ladder.  if the bot can't move
	// (i.e. get's stuck with someone else on ladder), the bot will
	// change directions and go the other way on the ladder.

	if (m_siLadderDir == LADDER_UP)  // is the bot currently going up?
	{
		pev->v_angle.x = -60;  // look upwards

		// check if the bot hasn't moved much since the last location...
		if ((moved_distance <= 1) && (prev_speed >= 1.0))
		{
			// the bot must be stuck, change directions...

			pev->v_angle.x = 60;  // look downwards
			m_siLadderDir = LADDER_DOWN;
		}
	}
	else if (m_siLadderDir == LADDER_DOWN)  // is the bot currently going down?
	{
		pev->v_angle.x = 60;  // look downwards

		// check if the bot hasn't moved much since the last location...
		if ((moved_distance <= 1) && (prev_speed >= 1.0))
		{
			// the bot must be stuck, change directions...

			pev->v_angle.x = -60;  // look upwards
			m_siLadderDir = LADDER_UP;
		}
	}
	else  // the bot hasn't picked a direction yet, try going up...
	{
		pev->v_angle.x = -60;  // look upwards
		m_siLadderDir = LADDER_UP;
	}

	// move forward (i.e. in the direction the bot is looking, up or down)
	pev->button |= IN_FORWARD;
}
*/

eMasterType CMasterEntity::CanFire(edict_t* pActivator) const
{
	edict_t* pMaster = FIND_ENTITY_BY_TARGETNAME(nullptr, m_szMasterName);

	if (pMaster)
	{
		CBaseEntity* pentMaster = static_cast<CBaseEntity*>(GET_PRIVATE(pMaster));

		if (pentMaster)
		{
			CBaseEntity* pentActivator = static_cast<CBaseEntity*>(GET_PRIVATE(pActivator));

			if (pentActivator)
			{
				if (!pentMaster->IsTriggered(pentActivator))
					return MASTER_NOT_TRIGGERED;
				else
					return MASTER_TRIGGERED;
			}
		}
	}

	return MASTER_FAULT;
}

BOOL CBot::WantToFollowEnemy(edict_t* pEnemy) const
// return TRUE if bot wants to follow an enemy once out of sight
// might want to add a few more things to it though
{
	std::vector<ga_value> weights;

	weights.push_back(m_GASurvival->get(12));
	weights.push_back(m_GASurvival->get(13));
	weights.push_back(m_GASurvival->get(14));
	weights.push_back(m_GASurvival->get(15));

	std::vector<ga_value> inputs;

	dec_followEnemy->setWeights(weights);

	inputs.push_back(pev->health / pev->max_health);
	inputs.push_back(DistanceFromEdict(pEnemy) * 0.001);
	inputs.push_back(pev->size.Length() / pEnemy->v.size.Length());

	dec_followEnemy->input(&inputs);

	dec_followEnemy->execute();

	weights.clear();
	inputs.clear();

	return dec_followEnemy->fired();
	/*

	if ( pEnemy == NULL )
		return FALSE;

	switch ( gBotGlobals.m_iCurrentMod )
	{
	case MOD_NS:
		// always follow enemy in NS except gorge
		if ( IsGorge() )
			return FALSE;

		return TRUE;
		break;
	case MOD_DMC:
		if ( DMC_HasInvisibility() || DMC_HasInvulnerability() )
			return TRUE;
		break;
	}

	return (pev->health > (pev->max_health * 0.3));*/
}

edict_t* CMasterEntity::FindButton(Vector vOrigin) const
{
	edict_t* pButton = nullptr;
	edict_t* pStart = nullptr;

	//edict_t *pBestButton = NULL;

	edict_t* pNearestVis = nullptr;
	edict_t* pNearest = nullptr;

	float fNearestVisDist = 0.0;
	float fNearestDist = 0.0;

	TraceResult tr;

	while ((pButton = FIND_ENTITY_BY_TARGET(pButton, m_szMasterName)) != nullptr)
	{
		if (pStart == nullptr)
			pStart = pButton;
		else if (pButton == pStart)
			break; // finished

		Vector vButtonOrigin = EntityOrigin(pButton);

		float fDist = (vOrigin - vButtonOrigin).Length();

		if (UTIL_IsButton(pButton))
		{
			UTIL_TraceLine(vOrigin, vButtonOrigin, ignore_monsters, dont_ignore_glass, nullptr, &tr);

			if (tr.flFraction >= 1.0)
			{
				if (!pNearestVis || fDist < fNearestVisDist)
				{
					fNearestVisDist = fDist;
					pNearestVis = pButton;
				}
			}
			else
			{
				if (!pNearest || fDist < fNearestDist)
				{
					fNearestDist = fDist;
					pNearest = pButton;
				}
			}
		}
		else
		{
			char* szRelayName = const_cast<char*>(STRING(pButton->v.targetname));

			edict_t* pButton2 = nullptr;
			edict_t* pStart2 = nullptr;

			while ((pButton2 = FIND_ENTITY_BY_TARGET(pButton, szRelayName)) != nullptr)
			{
				if (pStart2 == nullptr)
					pStart2 = pButton2;
				else if (pButton2 == pStart2)
					break;// finished

				vButtonOrigin = EntityOrigin(pButton2);

				fDist = (vOrigin - vButtonOrigin).Length();

				if (UTIL_IsButton(pButton2))
				{
					UTIL_TraceLine(vOrigin, vButtonOrigin, ignore_monsters, dont_ignore_glass, nullptr, &tr);

					if (tr.flFraction >= 1.0)
					{
						if (!pNearestVis || fDist < fNearestVisDist)
						{
							fNearestVisDist = fDist;
							pNearestVis = pButton2;
						}
					}
					else
					{
						if (!pNearest || fDist < fNearestDist)
						{
							fNearestDist = fDist;
							pNearest = pButton2;
						}
					}
				}
			}
		}
	}

	if (pNearestVis)
		return pNearestVis;

	return pNearest;
}

edict_t* CBot::PlayerStandingOnMe(void) const
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (pPlayer == nullptr)
			continue;

		if (!*STRING(pPlayer->v.netname))
			continue;

		if (pPlayer->v.groundentity == m_pEdict)
			return pPlayer;
	}

	return nullptr;
}

edict_t* CBot::StandingOnPlayer(void) const
{
	edict_t* pStandingOn = pev->groundentity;

	if (pStandingOn)
	{
		if (pStandingOn->v.flags & FL_CLIENT)
			return pStandingOn;
	}

	return nullptr;
}

void BotFunc_MakeSquad(CClient* pClient)
// Make a squad onto client
{
	edict_t* pEntity = pClient->GetPlayer();

	if (pEntity == nullptr)
		return;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBot* pBot = &gBotGlobals.m_Bots[i];

		if (!pBot || !pBot->IsUsed())
			continue;
		if (pBot->DistanceFromEdict(pEntity) > 512)
			continue;

		if (pBot->CanAddToSquad(pEntity))
		{
			pBot->AddToSquad(pEntity);
		}
	}
}

void CBotSquad::ReturnAllToFormation(void)
{
	dataStack<MyEHandle> tempStack = this->m_theSquad;

	while (!tempStack.IsEmpty())
	{
		edict_t* pMember = tempStack.ChooseFromStack().Get();

		if (pMember->v.flags & FL_FAKECLIENT)
		{
			CBot* pBot = UTIL_GetBotPointer(pMember);

			if (pBot)
			{
				pBot->m_Tasks.FlushTasks();
				pBot->AddPriorityTask(CBotTask(BOT_TASK_FOLLOW_LEADER, pBot->m_Tasks.GetNewScheduleId()));
				pBot->SetSquad(this);
			}
		}
	}
}

BOOL BotFunc_BreakableIsEnemy(edict_t* pBreakable, edict_t* pEdict)
{
	entvars_t* pEnemypev = &pBreakable->v;
	// i. explosives required to blow breakable
	// ii. OR is not a world brush (non breakable) and can be broken by shooting
	if (!(pEnemypev->flags & FL_WORLDBRUSH) && !(pEnemypev->spawnflags & SF_BREAK_TRIGGER_ONLY))
	{
		Vector* vSize = &pEnemypev->size;
		Vector* vMySize = &pEdict->v.size;

		if (*STRING(pEnemypev->target) ||
			vSize->x >= vMySize->x ||
			vSize->y >= vMySize->y ||
			vSize->z >= vMySize->z / 2)
		{
			// Only shoot breakables that are bigger than me (crouch size)
			// or that target something...
			return pEnemypev->health < 1000 && !(pEnemypev->effects & EF_NODRAW); // breakable still visible (not broken yet)
		}
	}

	return FALSE;
}

edict_t* BotFunc_FindRandomEntity(char* szClassname)
// find a random entity with classname
{
	dataUnconstArray<edict_t*> theEntities;

	edict_t* pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, szClassname)) != nullptr)
	{
		theEntities.Add(pEntity);
	}

	if (theEntities.IsEmpty())
		return nullptr;

	pEntity = theEntities.Random();

	theEntities.Clear();

	return pEntity;
}

BOOL BotFunc_GetStructuresToBuildForEntity(AvHUser3 iBuildingType, int* iDefs, int* iOffs, int* iSens, int* iMovs)
{
	CThingToBuild* theThingsToBuild = nullptr;

	switch (iBuildingType)
	{
	case AVH_USER3_HIVE:
		// lets say put a couple of offenses
		// def chambers first etc.
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forHive;
		break;
	case AVH_USER3_ALIENRESTOWER:
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forResTower;
		break;
	case AVH_USER3_OFFENSE_CHAMBER:
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forOffenseChamber;
		break;
	case AVH_USER3_DEFENSE_CHAMBER:
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forDefenseChamber;
		break;
	case AVH_USER3_MOVEMENT_CHAMBER:
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forMovementChamber;
		break;
	case AVH_USER3_SENSORY_CHAMBER:
		theThingsToBuild = &gBotGlobals.m_ThingsToBuild->m_forSensoryChamber;
		break;
	default:
		break;
	}

	if (theThingsToBuild)
		theThingsToBuild->GetThingsToBuild(iOffs, iDefs, iMovs, iSens);

	return *iOffs || *iDefs || *iSens || *iMovs;
}

void CBot::FlapWings(void)
{
	Jump();

	if (m_fEndJumpTime + m_pFlyGAVals->get(0) < gpGlobals->time)
	{
		m_fStartJumpTime = 0;
		m_fEndJumpTime = gpGlobals->time + m_pFlyGAVals->get(1);
	}
}

int BotFunc_GetStructureForGorgeBuild(entvars_t* pGorge, entvars_t* pEntitypev)
// look for things that a gorge might want to build nearby the pEntitypev Entity.
{
	int iDefs = 0;
	int iOffs = 0;
	int iMovs = 0;
	int iSens = 0;

	const AvHUser3 iBuildingType = (AvHUser3)pEntitypev->iuser3;

	const Vector vOrigin = pEntitypev->origin;

	// 2d distance
	const float fRange = (vOrigin - pGorge->origin).Length2D();

	// dont build out too far
	if (fRange > MAX_BUILD_RANGE)
		return 0;

	const BOOL bCanBuildNearby = BotFunc_GetStructuresToBuildForEntity(iBuildingType, &iDefs, &iOffs, &iSens, &iMovs);

	if (bCanBuildNearby)
	{
		if (pEntitypev->iuser3 == AVH_USER3_HIVE && UTIL_CanBuildHive(pEntitypev))
		{
			// unbuilt hive
			// build half of the stuff only so we can build the hive quicker
			iDefs = iDefs / 2;
			iOffs = iOffs / 2;
			iMovs = iMovs / 2;
			iSens = iSens / 2;
		}
		// get the buildings we still need to build
		UTIL_CountBuildingsInRange(pGorge->origin, fRange, &iDefs, &iOffs, &iSens, &iMovs);

		dataUnconstArray<int> iThingsToBuild;

		// got to build offense chambers?
		if (iOffs > 0)// can always build offense chambers
		{
			while (iOffs > 0)
			{
				iThingsToBuild.Add(ALIEN_BUILD_OFFENSE_CHAMBER);
				iOffs--;
			}
		}
		if (gBotGlobals.m_bHasDefTech && iDefs > 0)
		{
			while (iDefs > 0)
			{
				iThingsToBuild.Add(ALIEN_BUILD_DEFENSE_CHAMBER);
				iDefs--;
			}
		}
		if (gBotGlobals.m_bHasMovTech && iMovs > 0)
		{
			while (iMovs > 0)
			{
				iThingsToBuild.Add(ALIEN_BUILD_MOVEMENT_CHAMBER);
				iMovs--;
			}
		}
		if (gBotGlobals.m_bHasSensTech && iSens > 0)
		{
			while (iSens > 0)
			{
				iThingsToBuild.Add(ALIEN_BUILD_SENSORY_CHAMBER);
				iSens--;
			}
		}

		if (!iThingsToBuild.IsEmpty())
		{
			const int iToBuild = iThingsToBuild.Random();

			iThingsToBuild.Clear();

			return iToBuild;
		}
	}

	return 0;
}

void CBot::SayInPosition(void)
{
	if (!m_bSaidInPosition && m_fNextUseSayMessage < gpGlobals->time)
	{
		switch (gBotGlobals.m_iCurrentMod)
		{
		case MOD_NS:
			Impulse(SAYING_6);
			break;
		default:
			FakeClientCommand(m_pEdict, "say_team \"In Position!\"");
			break;
		}

		m_bSaidInPosition = TRUE;
		m_fNextUseSayMessage = gpGlobals->time + 5.0;
	}
}

/*BOOL CBot::IsHoldingMiniGun(void)
{
	return gBotGlobals.IsMod(MOD_SVENCOOP) && m_pCurrentWeapon && m_pCurrentWeapon->GetID() == SVEN_WEAPON_MINIGUN;
}*/

void CBot::RunForCover(Vector const vOrigin, const BOOL bDoItNow, int iScheduleId)
{
	const BOOL bCovering = m_Tasks.HasSchedule(BOT_SCHED_RUN_FOR_COVER);

	if (bDoItNow || !bCovering)
	{
		// already got a cover chedule but want to override the old one
		if (bCovering) // remove it
			m_Tasks.FinishSchedule(BOT_SCHED_RUN_FOR_COVER);

		if (!iScheduleId)
			iScheduleId = m_Tasks.GetNewScheduleId();

		int iCoverWpt = WaypointLocations.GetCoverWaypoint(pev->origin, vOrigin, &m_FailedGoals);

		BOOL bReloading = FALSE;

		// always try to crouch

		// ----------------------.....task......,.....ID....,..edict..,.int.,.........float .......
		AddPriorityTask(CBotTask(BOT_TASK_CROUCH, iScheduleId, nullptr, 0, RANDOM_FLOAT(1.0, 2.0)));

		if (iCoverWpt == -1)
		{
			// no cover point

			// use lowest cost techniques
			iCoverWpt = WaypointLocations.NearestWaypoint(m_vLowestEnemyCostVec, 320.0, -1);

			// not found
			if (iCoverWpt == -1)
				return;
		}

		// Reload at the cover point in battlegrounds (slows you down)
		if (gBotGlobals.IsMod(MOD_BG))
		{
			//                                                        int = 1, "Don't wait for reload to complete"
			AddPriorityTask(CBotTask(BOT_TASK_RELOAD, iScheduleId, nullptr, 1));
			bReloading = TRUE;
		}

		// cover waypoint valid and not already heading to this waypoint...?
		if (m_iWaypointGoalIndex != iCoverWpt)
		{
			if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_NAV_LEVEL) && gBotGlobals.m_pListenServerEdict)
				WaypointDrawBeam(gBotGlobals.m_pListenServerEdict, pev->origin, WaypointOrigin(iCoverWpt), 16, 5, 255, 0, 0, 127, 10);

			AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iScheduleId, nullptr, iCoverWpt, -2));
		}

		// reload NOW (if not battlegrounds)
		if (!bReloading)
		{
			if (m_pCurrentWeapon && m_pCurrentWeapon->NeedToReload())
			{
				//                                                        int = 1, "Don't wait for reload to complete"
				AddPriorityTask(CBotTask(BOT_TASK_RELOAD, iScheduleId, nullptr, 1));
			}
		}

		m_Tasks.GiveSchedIdDescription(iScheduleId, BOT_SCHED_RUN_FOR_COVER);
		// 7.5 seconds avg to run for cover
		m_Tasks.SetTimeToCompleteSchedule(iScheduleId, RANDOM_FLOAT(5.0, 10.0));
	}
}

///-------- NS Stuff -----------
BOOL CBot::IsInReadyRoom(void) const { return gBotGlobals.IsNS() && GetTeam() == TEAM_NONE; }
BOOL CBot::IsCommander(void) const { return gBotGlobals.IsNS() && EntityIsCommander(m_pEdict); }
BOOL CBot::IsMarine(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_MARINE_PLAYER; }
BOOL CBot::IsGestating(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_EMBRYO; }
BOOL CBot::IsAlien(void) const { return gBotGlobals.IsNS() && GetTeam() == TEAM_ALIEN; }
BOOL CBot::IsSkulk(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_PLAYER1; }
BOOL CBot::IsGorge(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_PLAYER2; }
BOOL CBot::IsLerk(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_PLAYER3; }
BOOL CBot::IsFade(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_PLAYER4; }
BOOL CBot::IsOnos(void) const { return gBotGlobals.IsNS() && pev->iuser3 == AVH_USER3_ALIEN_PLAYER5; }

void CBot::NeedMetal(const BOOL flush, const BOOL priority, const int iSched)
{
	if (flush)
		m_Tasks.FlushTasks();

	UpdateCondition(BOT_CONDITION_TASKS_CORRUPTED);

	m_fNextBuildTime = gpGlobals->time + 2.0;

	edict_t* pAmmo = gBotGlobals.findBackpack(pev->origin, pev->team, 0, 50, 0, 0);

	int sched = iSched;

	if (iSched == 0)
		sched = m_Tasks.GetNewScheduleId();

	if (priority)
	{
		AddPriorityTask(CBotTask(BOT_TASK_PICKUP_ITEM, sched, pAmmo));
		AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, sched, pAmmo));
	}
	else
	{
		AddTask(CBotTask(BOT_TASK_FIND_PATH, sched, pAmmo));
		AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, sched, pAmmo));
	}

	//Find ammo
}

void CBot::FindBackPack(const int health, const int cells, const int armour, const int ammo, const BOOL flush,
	const BOOL priority, const int iSched)
{
	if (flush)
		m_Tasks.FlushTasks();

	UpdateCondition(BOT_CONDITION_TASKS_CORRUPTED);

	m_fNextBuildTime = gpGlobals->time + 2.0;

	edict_t* pAmmo = gBotGlobals.findBackpack(pev->origin, pev->team, health, cells, armour, ammo);

	int sched = iSched;

	if (iSched == 0)
		sched = m_Tasks.GetNewScheduleId();

	if (priority)
	{
		AddPriorityTask(CBotTask(BOT_TASK_PICKUP_ITEM, sched, pAmmo));
		AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, sched, pAmmo));
	}
	else
	{
		AddTask(CBotTask(BOT_TASK_FIND_PATH, sched, pAmmo));
		AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, sched, pAmmo));
	}

	//Find ammo
}

///
// Repair bots sentry, given a schedule id for tasks
/*void CBot::RepairSentry(int iNewScheduleId)
{
	// get number of cells
	CBotWeapon* pSpanner = m_Weapons.GetWeapon(TF_WEAPON_SPANNER);
	int cells;

	if (pSpanner && (cells = pSpanner->PrimaryAmmo()) < 130)
	{
		//Vector location, int team, int min_health, int min_cells, int min_armor, int min_ammo )

		edict_t* pAmmopack = gBotGlobals.findBackpack(pev->origin, pev->team, 0, 50, 0, 0);

		if (pAmmopack)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, pAmmopack));
			AddTask(CBotTask(BOT_TASK_PICKUP_ITEM, iNewScheduleId, pAmmopack));
		}
	}
	else
	{
		edict_t* sentry = getSentry();

		if ( sentry == NULL )
		{
			char *classname[1] = {"building_sentrygun"};

			// get nearest sentry
			sentry = UTIL_FindNearestEntity(classname,1,m_vSentry,200.0,FALSE);
		}

		if (sentry)
		{
			AddTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, sentry));
		}
	}
}*/

edict_t* CBot::getSentry() const
{
	CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(m_pEdict);

	if (pClient)
		return pClient->getTFCSentry();

	return nullptr;
}

BOOL CBot::ThinkSpyOnTeam(edict_t* pSpy)
{
	if (pSpy->v.team == pev->team)
		return TRUE;

	if (pSpy == m_pSpySpotted)
	{
		// been away too long
		if (m_fSeeSpyTime < gpGlobals->time)
		{
			m_pSpySpotted = nullptr;
		}
		else
			return FALSE;
	}

	char color[32];
	int color_bot, color_player;

	char* infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)(m_pEdict);
	strcpy(color, g_engfuncs.pfnInfoKeyValue(infobuffer, "topcolor"));
	sscanf(color, "%d", &color_bot);

	infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)(pSpy);
	strcpy(color, g_engfuncs.pfnInfoKeyValue(infobuffer, "topcolor"));
	sscanf(color, "%d", &color_player);

	if ((color_bot == 140 || color_bot == 148 || color_bot == 150 || color_bot == 153) &&
		(color_player == 140 || color_player == 148 || color_player == 150 || color_player == 153))
		return TRUE;

	if ((color_bot == 5 || color_bot == 250 || color_bot == 255) &&
		(color_player == 5 || color_player == 250 || color_player == 255))
		return TRUE;

	if (color_bot == 45 && color_player == 45)
		return TRUE;

	if ((color_bot == 80 || color_bot == 100) &&
		(color_player == 80 || color_player == 100))
		return TRUE;

	return FALSE;
}

void CBot::DoTasks()
{
	m_CurrentTask = m_Tasks.CurrentTask();

	if (m_CurrentTask != nullptr)// Check if the current task is timed out
	{
		if (m_CurrentTask->TimedOut())
		{
			// fail it
			m_Tasks.FinishSchedule(m_CurrentTask->GetScheduleId());
			m_CurrentTask = m_Tasks.CurrentTask();
		}
		else // fail other schedules we havent done yet that we wanted to do a while back !
		{
			m_Tasks.RemoveTimedOutSchedules();
			m_CurrentTask = m_Tasks.CurrentTask();
		}
	}

	if (m_CurrentTask != nullptr)
	{
		BOOL bDone = FALSE;

		// PathFound will be true when a BOT_FIND_PATH is done sucessfully.
		// this allows other tasks to know a path has been worked out to go to
		// a task, so we don't keep looking for one.
		BOOL bPathFound = FALSE;
		BOOL bTaskFailed = FALSE;

		//		BOOL bGotPath = m_CurrentTask->HasPath();

				// May need to create a new task in some cases
				// a bit crap, should just add them on the fly (do this sometimes)
		CBotTask TaskToAdd = CBotTask(BOT_TASK_NONE);
		eBotTask iTask = m_CurrentTask->Task();

		m_bLookForNewTasks = FALSE;
		m_bHasAskedForOrder = FALSE;

		RemoveCondition(BOT_CONDITION_TASKS_CORRUPTED);

		// TASK_CODE
		switch (iTask)
		{

		case BOT_TASK_PUSH_PUSHABLE:
		{
			// get State first
			int iState = m_CurrentTask->TaskInt();
			// pushable object
			edict_t* pPushable = m_CurrentTask->TaskEdict();

			if (!pPushable || FNullEnt(pPushable))
			{
				// something wrong
				bTaskFailed = TRUE;
				break;
			}

			// origin of where we want to put the pushable
			Vector vOrigin = m_CurrentTask->TaskVector();
			entvars_t* pPushablepev = &pPushable->v;
			float fSize = UTIL_GetBestPushableDistance(pPushable);//pev->size.x+pPushablepev->size.y)/3;

			if (UTIL_AcceptablePushableVector(pPushable, vOrigin))
			{
				// close enough
				bDone = TRUE;
				break;
			}

			if (iState == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(6.0, 8.0));
				m_CurrentTask->SetInt(1);
			}
			else if (iState == 1)
			{
				Vector vDesiredPushOrigin;

				vDesiredPushOrigin = UTIL_GetDesiredPushableVector(vOrigin, pPushable);

				if (DistanceFrom(vDesiredPushOrigin) > fSize / 2)
				{
					SetMoveVector(vDesiredPushOrigin);
				}
				else
					m_CurrentTask->SetInt(2);
			}
			else if (iState == 2)
			{
				// line up
				if (!UTIL_IsFacingEntity(pev, pPushablepev))
				{
					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;
				}
				else
					m_CurrentTask->SetInt(3); // next state

				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(6.0, 8.0));
			}
			else
			{
				if (RANDOM_LONG(0, 100))
					Use();

				SetMoveVector(vOrigin);
			}

			if (m_CurrentTask->TaskFloat() < gpGlobals->time)
			{
				// time out
				bTaskFailed = TRUE;
				break;
			}
		}
		break;
		case BOT_TASK_WAIT_FOR_FLAG:
			if (m_bHasFlag)
				bDone = TRUE;
			if (DistanceFrom(m_CurrentTask->TaskVector()) > 128)
			{
				bTaskFailed = TRUE;
				break;
			}

			StopMoving();
			m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

			Duck();
			break;
		case BOT_TASK_WAIT_FOR_LIFT:
			// Bot waiting for an entity nearby that could be a lift to stop.
		{
			edict_t* pLift = nullptr;

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;
			StopMoving();

			while ((pLift = UTIL_FindEntityInSphere(pLift, m_CurrentTask->TaskVector(), 72)) != nullptr)
			{
				// Look for potential lifts or doors that are moving nearby and set it to the lift
				if (pLift->v.effects & EF_NODRAW)
					continue;
				if (pLift == m_pEdict)
					continue;
				if (strncmp("func_door", STRING(pLift->v.classname), 9) == 0)
				{
					bDone = TRUE;
					break;
				}
				if (strncmp("func_plat", STRING(pLift->v.classname), 9) == 0)
				{
					bDone = TRUE;
					break;
				}
				if (strncmp("func_train", STRING(pLift->v.classname), 10) == 0)
				{
					bDone = TRUE;
					break;
				}
			}

			if (bDone == FALSE)
			{
				if (m_CurrentTask->TaskInt() == 0)
				{
					// Wait ten seconds max
					m_CurrentTask->SetFloat(gpGlobals->time + 10.0);
					m_CurrentTask->SetInt(1);
				}
				else if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					bDone = TRUE;
				}
			}
		}
		break;
		case BOT_TASK_RELOAD:
			UpdateCondition(BOT_CONDITION_CANT_SHOOT);

			if (m_pCurrentWeapon == nullptr)
			{
				bTaskFailed = TRUE;
			}
			else
			{
				if (m_pCurrentWeapon->HasWeapon(m_pEdict) == FALSE)
				{
					bTaskFailed = TRUE;
					m_pCurrentWeapon = nullptr;
					break;
				}

				if (m_pLastEnemy)
				{
					m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY;
					m_fSearchEnemyTime = gpGlobals->time + 0.5;
				}

				if (RANDOM_LONG(0, 100) > 0)
					Reload();

				if (m_CurrentTask->TaskInt() == 0)
				{
					BOOL bSetTime = FALSE;

					if (gBotGlobals.IsMod(MOD_BG))
					{
						bSetTime = TRUE;
						m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(2.0, 3.0));
					}

					if (!bSetTime)
						m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(0.5, 1.5));

					m_CurrentTask->SetInt(1);
				}
				else
				{
					if (m_CurrentTask->TaskFloat() < gpGlobals->time)
						bDone = TRUE;
				}

				if (gBotGlobals.IsMod(MOD_BG))
				{
					// dont crouch as it will make ou move real slow
					pev->button &= ~IN_DUCK;
				}
			}

			break;
		case BOT_TASK_LISTEN_TO_SOUND:
			// Make bot pause for a bit (if no waypoint (done auto))
			// and face sound or if its sound entity is a player on team
			// and shooting then face to what they are shooting if they can
			// see what they're shooting at.
		{
			float fSoundTime = m_CurrentTask->TaskFloat();
			int iState = m_CurrentTask->TaskInt();
			edict_t* pSound = m_CurrentTask->TaskEdict();
			Vector vOrigin;

			if (m_pEnemy && HasCondition(BOT_CONDITION_SEE_ENEMY) || IsOnLadder())
			{
				bTaskFailed = TRUE;
				break;
			}
			if (m_iOrderType)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (pSound == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (iState == 0)
			{
				fSoundTime = gpGlobals->time + m_CurrentTask->TaskFloat();
				m_CurrentTask->SetFloat(fSoundTime);
				iState = 1;

				vOrigin = pSound->v.origin;
				m_CurrentTask->SetVector(vOrigin);
			}
			else if (iState == 1)
			{
				BOOL bIsGorge = gBotGlobals.IsNS() && IsGorge();
				vOrigin = pSound->v.origin;

				if (pSound->v.button & IN_ATTACK)
				{
					if (IsInVisibleList(pSound))
					{
						Vector vOrigin;
						Vector vSrc;
						TraceResult tr;

						if (!IsEnemy(pSound))
						{
							// state 2, looking where the player making the sound is looking
							iState = 2;
						}
					}
					else if (!bIsGorge && !m_bHasFlag && m_fInvestigateSoundTime < gpGlobals->time)
					{
						int iGotoWpt = -1;

						m_fInvestigateSoundTime = gpGlobals->time + RANDOM_FLOAT(8.0, 16.0);

						// investigate!

						switch (gBotGlobals.m_iCurrentMod)
						{
						case MOD_TFC:
							break;
						default:
							break;
						}

						if (iGotoWpt != -1)
							TaskToAdd = CBotTask(BOT_TASK_FIND_PATH, m_CurrentTask->GetScheduleId(), pSound, iGotoWpt, -2);
					}
				}

				m_CurrentTask->SetVector(vOrigin);
			}
			else if (iState == 2)
			{
				UTIL_MakeVectors(pSound->v.v_angle);
				vOrigin = pSound->v.origin + pSound->v.view_ofs + gpGlobals->v_forward * 8192;
				m_CurrentTask->SetVector(vOrigin);
				iState = 3; // look dead ahead
			}

			m_CurrentTask->SetInt(iState);
			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

			if (fSoundTime <= gpGlobals->time)
			{
				bDone = TRUE;

				// more frantic in TS!!!
				if (gBotGlobals.IsMod(MOD_TS))
				{
					if (!m_Tasks.HasTask(BOT_TASK_USE))
						m_fListenToSoundTime = gpGlobals->time + m_fReactionTime * 4.0;
					else
						m_fListenToSoundTime = gpGlobals->time + RANDOM_FLOAT(4.5, 10.0);
				}
				else
				{
					// wait another while before listening to another sound
					m_fListenToSoundTime = gpGlobals->time + RANDOM_FLOAT(3.0, 5.0);
				}

				break;
			}
		}
		break;
		case BOT_TASK_USE_TELEPORTER:
			// set float:- size of teleporter 2d
			// set entity:- destination teleporter
			// set vector:- source teleporter origin
		{
			edict_t* pDestinationTeleporter = m_CurrentTask->TaskEdict();
			edict_t* pSourceTeleporter = reinterpret_cast<edict_t*>(m_CurrentTask->TaskInt());

			Vector vSourceTeleporter = m_CurrentTask->TaskVector();

			if (pDestinationTeleporter == nullptr || FNullEnt(pDestinationTeleporter) || pDestinationTeleporter->v.effects & EF_NODRAW)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (pSourceTeleporter == nullptr || FNullEnt(pSourceTeleporter) || pSourceTeleporter->v.effects & EF_NODRAW || pSourceTeleporter->v.origin != vSourceTeleporter)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (!FVisible(pSourceTeleporter))
			{
				bTaskFailed = TRUE;
				break;
			}

			if (!RANDOM_LONG(0, 100) && UTIL_PlayerStandingOnEntity(pSourceTeleporter, m_iTeam, m_pEdict))
			{
				bTaskFailed = TRUE;
				break;
			}

			float fDistTeleporterSrc = DistanceFrom(vSourceTeleporter, TRUE);
			float fDistTeleporterDest = DistanceFrom(pDestinationTeleporter->v.origin);

			if (fDistTeleporterSrc > 50 || pev->groundentity != pSourceTeleporter)
			{
				if (fDistTeleporterDest < fDistTeleporterSrc)
				{
					if (gBotGlobals.IsNS())
					{
						if (pev->groundentity->v.iuser3 == AVH_USER3_PHASEGATE && pev->groundentity != pDestinationTeleporter)
							TaskToAdd = CBotTask(BOT_TASK_USE, m_CurrentTask->GetScheduleId(), pev->groundentity);
					}

					SetMoveVector(vSourceTeleporter);

					if (fDistTeleporterSrc < 100)
						m_fIdealMoveSpeed = m_fMaxSpeed / 2;
				}
			}
			else
			{
				if (gBotGlobals.IsNS())
				{
					if (pev->groundentity->v.iuser3 == AVH_USER3_PHASEGATE)
						TaskToAdd = CBotTask(BOT_TASK_USE, m_CurrentTask->GetScheduleId(), pev->groundentity);
					else
						bTaskFailed = TRUE;
				}
			}
			break;
		}
		case BOT_TASK_WAIT_AND_FACE_VECTOR:

			if (m_pEnemy || HasCondition(BOT_CONDITION_SEE_ENEMY))
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

			StopMoving();

			if (m_CurrentTask->TaskInt() == 1)
			{
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
					bDone = TRUE;
			}
			else
			{
				// init.
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1);
			}

			break;
		case BOT_TASK_FACE_VECTOR:

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

			if (m_CurrentTask->TaskInt() == 1)
			{
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
					bDone = TRUE;
			}
			else
			{
				// init.
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1);
			}

			break;
		case BOT_TASK_FACE_EDICT:
		{
			if (!m_CurrentTask->TaskEdict())
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

			if (m_CurrentTask->TaskInt() == 1) // is "state 1"
			{
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
					bDone = TRUE;
			}
			else
			{
			   // look at entity for ... taskfloat() seconds
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1); // set "state 1"
			}

			break;
		}
		case BOT_TASK_GOTO_OBJECT:
			break;
		case BOT_TASK_FIND_ENEMY_PATH:

			m_fIdealMoveSpeed = m_fMaxSpeed / 2;

			UpdateCondition(BOT_CONDITION_CANT_SHOOT);

			break;
		case BOT_TASK_ALIEN_EVOLVE:
			// Evolve to another alien species in NS
			// TaskInt holds the species type.
			if (m_CurrentTask->TaskInt() == 0)
			{
				bTaskFailed = TRUE;
				break;
			}

			Impulse(m_CurrentTask->TaskInt());

			bDone = TRUE;
			break;
		case BOT_TASK_FOLLOW_ENEMY:
			break;
		case BOT_TASK_AVOID_OBJECT:
			// A task that sets the "avoid entity" to task edict.
			// avoid entity is handled in movement code.
		{
			int iState = m_CurrentTask->TaskInt();
			m_pAvoidEntity = m_CurrentTask->TaskEdict();

			if (m_pAvoidEntity == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (iState == 0)
			{
				//
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());

				// update state
				m_CurrentTask->SetInt(1);
			}
			else if (m_CurrentTask->TaskFloat() <= gpGlobals->time)
			{
				bDone = TRUE;
				break;
			}
		}
		break;
		case BOT_TASK_BUILD_ALIEN_STRUCTURE:
			// in NS, make gorge build a structure held in taskInt (iuser3)
		{
			int iToBuild = m_CurrentTask->TaskInt();
			BOOL bBuild = TRUE;

			if (!iToBuild)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (iToBuild == ALIEN_BUILD_HIVE)
			{
				edict_t* pHive = m_CurrentTask->TaskEdict();

				if (pHive == nullptr)
				{
					bTaskFailed = TRUE;
					break;
				}

				if (!UTIL_CanBuildHive(&pHive->v))//pHive->v.iuser4 & MASK_BUILDABLE)
				{
					bDone = TRUE;
					break;
				}

				m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

				Vector vOrigin = EntityOrigin(pHive);

				if ((vOrigin - pev->origin).Length2D() > 320)
				{
					SetMoveVector(vOrigin);
					bBuild = FALSE;
				}
				else
				{
					StopMoving();

					FakeClientCommand(m_pEdict, "say_team \"Building Hive\"");
				}
			}
			else if (iToBuild == ALIEN_BUILD_RESOURCES)
			{
				edict_t* pFuncResource = m_CurrentTask->TaskEdict();

				if (pFuncResource == nullptr)
				{
					bTaskFailed = TRUE;
					break;
				}

				if (UTIL_FuncResourceIsOccupied(pFuncResource))
				{
					// Already a resource tower here
					bTaskFailed = TRUE;
					break;
				}

				// Need to face the right direction
				m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

				Vector vOrigin = EntityOrigin(pFuncResource);

				if (!UTIL_IsFacingEntity(pev, &pFuncResource->v))
				{
					bBuild = FALSE;
				}
				else
				{
					TraceResult tr;
					bBuild = TRUE;

					UTIL_TraceLine(GetGunPosition(), vOrigin, dont_ignore_monsters, ignore_glass, m_pEdict, &tr);

					if (tr.pHit)
					{
						if (tr.pHit->v.iuser3 == AVH_USER3_RESTOWER ||
							tr.pHit->v.iuser3 == AVH_USER3_ALIENRESTOWER)
						{
							bBuild = FALSE;
						}
					}

					if (bBuild)
					{
						AddVisitedResourceTower(pFuncResource);
						FakeClientCommand(m_pEdict, "say_team \"Building Resource Tower\"");
					}
				}
			}

			if (bBuild)
			{
				// set next build time so we dont keep trying to build...
				m_fNextBuildTime = gpGlobals->time + 1.0;
				Impulse(iToBuild);
				// only try once
				//bDone = TRUE;
				m_Tasks.FinishedCurrentTask();

				if (iToBuild != ALIEN_BUILD_HIVE)
				{
					UTIL_MakeVectors(pev->v_angle);
					AddPriorityTask(CBotTask(BOT_TASK_FACE_VECTOR, 0, nullptr, 0, 0.75, pev->origin + gpGlobals->v_forward * 64));
				}
			}
		}
		break;
		case BOT_TASK_WAIT_FOR_BOT_AT_WPT:
		{
			int iWpt = m_CurrentTask->TaskInt();
			edict_t* pBotEdict = m_CurrentTask->TaskEdict();

			if (iWpt == -1 || !pBotEdict)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (m_CurrentTask->TaskFloat() == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_LONG(4.0, 6.0));
			}

			CBot* pBot = UTIL_GetBotPointer(pBotEdict);

			if (!pBot)
			{
				bTaskFailed = TRUE;
				break;
			}

			Vector vWptOrigin = WaypointOrigin(iWpt);

			StopMoving();

			bDone = m_CurrentTask->TaskFloat() < gpGlobals->time || DistanceFrom(vWptOrigin) <= pBot->DistanceFrom(vWptOrigin);
		}
		break;
		case BOT_TASK_DROP_WEAPON:
			FakeClientCommand(m_pEdict, "drop");
			bDone = TRUE;
			break;
		case BOT_TASK_SECONDARY_ATTACK:

			SecondaryAttack();

			bDone = TRUE;
			break;
		case BOT_TASK_ALIEN_UPGRADE:
		{
			int iDesiredUpgrade = m_CurrentTask->TaskInt();

			switch (iDesiredUpgrade)
			{
			case BOT_UPGRADE_DEF:
				// TO_DO : allow bots to choose upgrades depenging on how well they did with them.
				Impulse(RANDOM_LONG((int)ALIEN_EVOLUTION_ONE, (int)ALIEN_EVOLUTION_THREE));
				break;
			case BOT_UPGRADE_MOV:
				Impulse(RANDOM_LONG((int)ALIEN_EVOLUTION_TEN, (int)ALIEN_EVOLUTION_TWELVE));
				break;
			case BOT_UPGRADE_SEN:
				Impulse(RANDOM_LONG((int)ALIEN_EVOLUTION_SEVEN, (int)ALIEN_EVOLUTION_NINE));
				break;
			default: ;
			}

			bDone = TRUE;
		}
		break;
		case BOT_TASK_FOLLOW_LEADER:
		{
			Vector vLeaderOrigin;

			//m_pSquadLeader = m_CurrentTask->TaskEdict();

			if (m_pSquadLeader == nullptr || m_stSquad == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

			if (gBotGlobals.IsNS())
			{
				if (IsLerk())
				{
					if (!onGround()) // flying
					{
						m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;
					}
				}
				// can become gorge on squad if gestating to gorge in a squad
				else if (IsGorge())
				{
					gBotGlobals.m_Squads.removeSquadMember(m_stSquad, m_pEdict);
					m_pSquadLeader = nullptr;
					bTaskFailed = TRUE;
					break;
				}
			}

			vLeaderOrigin = EntityOrigin(m_pSquadLeader);

			if (HasCondition(BOT_CONDITION_SQUAD_LEADER_DEAD))
			{
				// Pick a new leader or become new leader or stop following

				gBotGlobals.m_Squads.ChangeLeader(m_stSquad);

				if (m_stSquad)
				{
					m_CurrentTask->SetEdict(m_pSquadLeader);
				}
				else
				{
					// stop following
					bTaskFailed = TRUE;
					break;
				}
			}
			else
			{
				BOOL bFindPath = FALSE;
				Vector vMoveTo = m_stSquad->GetFormationVector(m_pEdict);

				if (gBotGlobals.m_pListenServerEdict && !IS_DEDICATED_SERVER() && gBotGlobals.IsDebugLevelOn(BOT_DEBUG_NAV_LEVEL))
				{
					WaypointDrawBeam(gBotGlobals.m_pListenServerEdict, m_pSquadLeader->v.origin, vMoveTo, 16, 0, 255, 255, 255, 255, 10);
				}

				if (DistanceFrom(vMoveTo, TRUE) > m_stSquad->GetSpread()) // 2d distance (true for twoD)
					bFindPath = TRUE;
				else
				{
					SayInPosition();

					StopMoving();
				}

				if (!bFindPath || HasCondition(BOT_CONDITION_SEE_SQUAD_LEADER))
					// set up bot formation position
				{
					if (bFindPath)
						SetMoveVector(vMoveTo);

					if (m_pSquadLeader->v.flags & FL_DUCKING)
						Duck();

					m_CurrentTask->SetPathInfo(FALSE);

					m_iCurrentWaypointIndex = -1;
					m_iWaypointGoalIndex = -1;
					m_iPrevWaypointIndex = -1;
				}
				else if (bFindPath)
				{
					// look for squad leader

					if (!m_CurrentTask->HasPath())
					{
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, m_CurrentTask->GetScheduleId(), m_pSquadLeader, -1));
						m_CurrentTask->SetPathInfo(TRUE);
					}
					else
						bTaskFailed = TRUE;
				}
			}
		}
		break;
		case BOT_TASK_TYPE_MESSAGE:
			// Stop and simulate typing for some time and 'plop out' message
		{
			m_CurrentLookTask = BOT_LOOK_TASK_NONE;

			StopMoving();

			if (m_CurrentTask->TaskFloat() != 0.0)
			{
				if (m_CurrentTask->TaskFloat() <= gpGlobals->time)
				{
					HumanizeString(m_szChatString);

					if (m_CurrentTask->TaskInt() == 1) // team only
						FakeClientCommand(m_pEdict, "say_team \"%s\"", m_szChatString);
					else
						FakeClientCommand(m_pEdict, "say \"%s\"", m_szChatString);

					m_szChatString[0] = '\0';

					bDone = TRUE;
				}
			}
			else
			{
				if (m_szChatString[0])
				{
					float fTimeToComplete = (float)strlen(m_szChatString) / BOT_CHAT_TYPE_SPEED_SEC;

					m_CurrentTask->SetFloat(gpGlobals->time + fTimeToComplete);
				}
				else
				{
					bTaskFailed = TRUE;
					m_szChatString[0] = 0;
				}
			}
		}
		break;
		case BOT_TASK_FIND_WEAPON:
			// crap task :p
			break;
		case BOT_TASK_THROW_GRENADE:
			switch (gBotGlobals.m_iCurrentMod)
			{
			case MOD_HL_DM:

				if (HasWeapon(VALVE_WEAPON_HANDGRENADE))
				{
					if (IsCurrentWeapon(VALVE_WEAPON_HANDGRENADE))
					{
						if (m_CurrentTask->TaskInt() == 0)
						{
							Vector vOrigin, vAngle;

							vOrigin = m_CurrentTask->TaskVector();
							vAngle = UTIL_VecToAngles(vOrigin - GetGunPosition());
							vAngle.x -= RANDOM_FLOAT(20.0, 30.0); // add extra height
							if (vAngle.x < -89.0)
								vAngle.x = -89.0;

							UTIL_FixAngles(&vAngle);

							UTIL_MakeVectors(vAngle);

							m_CurrentTask->SetInt(1);

							m_CurrentTask->SetVector(GetGunPosition() + gpGlobals->v_forward * 320);

							// gren hold time
							m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(0.5, 1.0));
						}
						else
						{
							UpdateCondition(BOT_CONDITION_CANT_SHOOT);

							if (m_CurrentTask->TaskFloat() < gpGlobals->time)
							{
								bDone = TRUE;
								TaskToAdd = CBotTask(BOT_TASK_WAIT, m_CurrentTask->GetScheduleId(), nullptr, 0, RANDOM_FLOAT(2.1, 3.5));
							}
							else
							{
								// prime grenade
								PrimaryAttack();
							}
						}

						StopMoving();
					}
					else
						TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, VALVE_WEAPON_HANDGRENADE, 0.0, Vector(0, 0, 0), m_CurrentTask->TimeToComplete());

					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;
				}
				else
					bTaskFailed = TRUE;

				break;
			default:
				bTaskFailed = TRUE;
			}
			break;
		case BOT_TASK_RUN_PATH:
			// was going to use, never did
			break;
		case BOT_TASK_FIND_PATH:
		{
			edict_t* pTaskEdict = m_CurrentTask->TaskEdict();

			m_fIdealMoveSpeed = m_fMaxSpeed;

			int iPathResult;

			if (m_CurrentTask->HasPath() == FALSE) // if we haven't found a path yet
			{
				// if current paths to go to are still valid
				// keep walking through them else stop moving
				if (m_stBotPaths.IsEmpty())
				{
					if (!m_bNotFollowingWaypoint)
					{
						StopMoving();
					}
				}

				// dont muck up the tasks
				//UpdateCondition(BOT_CONDITION_CANT_SHOOT);

				if (m_CurrentTask->TaskFloat() == -3) // Pending path...
				{
					// WTF???
					if (m_iCurrentWaypointIndex == -1)
						m_iCurrentWaypointIndex = WaypointLocations.NearestWaypoint(pev->origin, REACHABLE_RANGE, m_iLastFailedWaypoint, TRUE, FALSE, TRUE);

					if ((iPathResult = BotNavigate_AStarAlgo(this, m_iCurrentWaypointIndex, m_iWaypointGoalIndex, TRUE)) < 0)
					{
						if (iPathResult == -1) // No path found
						{
							bTaskFailed = TRUE;
						}
						else if (iPathResult == -2) // Still pending path
							break;

						break;
					}
					else
					{
						if (m_stBotPaths.IsEmpty() || *m_stBotPaths.GetHeadInfoPointer() != m_iCurrentWaypointIndex)
							m_stBotPaths.Push(m_iCurrentWaypointIndex);

						m_bMoveToIsValid = TRUE;

						bPathFound = TRUE;

						m_CurrentTask->SetPathInfo(TRUE);
						m_FailedGoals.Destroy();

						if (m_CurrentTask->TaskFloat() == -2)
						{
							// Just find the path and bail out
							bDone = TRUE;
						}

						m_CurrentTask->SetFloat(1.0);
					}
				}
				else if (m_CurrentTask->TaskFloat() == -2 || m_fFindPathTime < gpGlobals->time)
				{
					// Update waypoint goal index
					// "BotNavigate_NextWaypoint" will know it
					// has changed by looking at PrevWaypointGoalIndex
					// and work out a new path.

					Vector vOrigin;

					BOOL bFindPath = TRUE;

					// Don't want to keep doing this each frame
					m_fFindPathTime = gpGlobals->time + 0.5;

					if (m_CurrentTask->TaskFloat() == -1 && m_CurrentTask->TaskInt() == -1)
					{
						m_CurrentTask->SetInt(WaypointFindRandomGoal(m_pEdict, m_iTeam, &m_FailedGoals));
						m_iWaypointGoalIndex = m_CurrentTask->TaskInt();

						if (m_iWaypointGoalIndex != -1)
							m_CurrentTask->SetVector(WaypointOrigin(m_iWaypointGoalIndex));
						else
						{
							bTaskFailed = TRUE;
							break;
						}
					}
					else
					{
						if (pTaskEdict)
						{
							vOrigin = EntityOrigin(pTaskEdict);
							m_CurrentTask->SetVector(vOrigin);

							m_iWaypointGoalIndex = NearestWaypointToOrigin(vOrigin, m_iLastFailedWaypoint, &m_FailedGoals);
						}
						else if (m_CurrentTask->TaskFloat() == 0)
						{
							m_CurrentTask->SetFloat(1.0);

							vOrigin = m_CurrentTask->TaskVector();

							m_iWaypointGoalIndex = NearestWaypointToOrigin(vOrigin, m_iLastFailedWaypoint, &m_FailedGoals);
						}
						else
						{
							m_iWaypointGoalIndex = m_CurrentTask->TaskInt();
							//m_CurrentTask->SetVector(WaypointOrigin(m_iWaypointGoalIndex));
						}
					}

					// No waypoint at goal?? can't find path
					if (m_iWaypointGoalIndex == -1)
					{
						bTaskFailed = TRUE;
						break;
					}

					m_iCurrentWaypointIndex = WaypointLocations.NearestWaypoint(pev->origin, REACHABLE_RANGE, m_iLastFailedWaypoint);

					m_CurrentTask->SetInt(m_iWaypointGoalIndex);

					if (m_iCurrentWaypointIndex == -1)
					{
						bTaskFailed = TRUE;
						break;
					}

					if (bFindPath)
					{
						if (m_CurrentTask->TaskVector() == Vector(0, 0, 0))
							m_CurrentTask->SetVector(WaypointOrigin(m_iWaypointGoalIndex));

						m_vGoalOrigin.SetVector(m_CurrentTask->TaskVector());

						if (m_iWaypointGoalIndex == m_iCurrentWaypointIndex)
						{
							m_stBotPaths.Push(m_iCurrentWaypointIndex);

							m_bMoveToIsValid = TRUE;

							bPathFound = TRUE;

							//m_fFindPathTime = gpGlobals->time + 1.0;

							break;
						}
						else if ((iPathResult = BotNavigate_AStarAlgo(this, m_iCurrentWaypointIndex, m_iWaypointGoalIndex, FALSE)) < 0)
						{
							if (iPathResult == -1) // No path found
								bTaskFailed = TRUE;
							else if (iPathResult == -2) // Still pending path
							{
								m_CurrentTask->SetFloat(-3);
								break;
							}

							break;
						}
					}
					else
					{
						m_CurrentTask->SetPathInfo(TRUE);
						m_FailedGoals.Destroy();
					}

					if (m_stBotPaths.IsEmpty() || *m_stBotPaths.GetHeadInfoPointer() != m_iCurrentWaypointIndex)
						m_stBotPaths.Push(m_iCurrentWaypointIndex);

					m_bMoveToIsValid = TRUE;

					bPathFound = TRUE;

					if (m_CurrentTask->TaskFloat() == -2)
					{
						// Just find the path and bail out
						bDone = TRUE;
					}
				}
			}
			else
			{
				if (m_bNearestRememberPointVisible && !m_pEnemy && m_bLookingForEnemy)
				{
					m_CurrentLookTask = BOT_LOOK_TASK_FACE_NEAREST_REMEMBER_POS;
				}
				else if (m_pLastEnemy && EntityIsAlive(m_pLastEnemy) && m_fLastSeeEnemyTime + 6.0 > gpGlobals->time)
				{
					m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY;
					m_fSearchEnemyTime = gpGlobals->time + 0.5;
				}
				else
					m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;

				if (m_iCurrentWaypointIndex == -1)
				{
					m_CurrentTask->SetPathInfo(FALSE);

					break;
				}

				if (m_iWaypointGoalIndex != m_CurrentTask->TaskInt())
				{
					m_CurrentTask->SetPathInfo(FALSE);

					//bTaskFailed = TRUE;
					break;
				}

				m_iWaypointGoalIndex = m_CurrentTask->TaskInt();

				if (m_iWaypointGoalIndex == -1)
				{
					m_CurrentTask->SetPathInfo(FALSE);
					break;
				}

				if (pTaskEdict && m_iWaypointGoalIndex == m_iCurrentWaypointIndex)
				{
					float fDist = DistanceFrom(EntityOrigin(pTaskEdict));

					if (fDist < WaypointDistance(GetGunPosition(), m_iWaypointGoalIndex))
						/*(fDist < WaypointDistance(GetGunPosition(),m_iCurrentWaypointIndex)) )*/
					{
						bDone = TRUE;
					}
				}

				m_bMoveToIsValid = TRUE;
			}
		}
		break;
		case BOT_TASK_HUMAN_TOWER:

			// Look over this again, seems to be a bit buggy...
		{
			Vector vTowerOrigin = m_CurrentTask->TaskVector();

			edict_t* pNearbyPlayer = nullptr;
			edict_t* pStandingOnPlayer = nullptr;
			edict_t* pPlayerStandingOn = nullptr;

			float fDist2d = DistanceFrom(vTowerOrigin, TRUE);

			pNearbyPlayer = PlayerNearVector(vTowerOrigin, 36);

			if (m_CurrentTask->TaskFloat() == 0.0)
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(8.0, 10.0));
			else
			{
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					bDone = TRUE;
					break;
				}
			}

			if (fDist2d > 36)
			{
				SetMoveVector(vTowerOrigin);

				if (fDist2d < 48)
				{
					if (pNearbyPlayer != nullptr && pNearbyPlayer != m_pEdict)
					{
						if (m_fEndJumpTime < gpGlobals->time)
							JumpAndDuck();
					}
				}
			}
			else
			{
				StopMoving();

				pStandingOnPlayer = StandingOnPlayer();

				if (pStandingOnPlayer)
				{
					// Within jump distance on next waypoint
					if (m_vMoveToVector.z - pev->origin.z < MAX_JUMP_HEIGHT)
						bDone = TRUE;
					else
						JumpAndDuck();
				}
				else
				{
					int iState = m_CurrentTask->TaskInt();

					pPlayerStandingOn = PlayerStandingOnMe();

					if (pPlayerStandingOn)
					{
						// stop ducking
						pev->button &= ~IN_DUCK;

						// state 1, stand and jump
						m_CurrentTask->SetInt(1);
					}
					else if (iState == 1)
					{							// jump
						// stop ducking
						pev->button &= ~IN_DUCK;

						if (m_fEndJumpTime + 0.5 < gpGlobals->time)
							Jump();
						else
							m_CurrentTask->SetInt(0); // assume I jumped, back to ducking
					}
					else
						Duck();
				}
			}
		}

		break;
		case BOT_TASK_MOVE_TO_VECTOR:
		{
			edict_t* pTaskEdict;
			Vector vOrigin;

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

			if ((pTaskEdict = m_CurrentTask->TaskEdict()) != nullptr)
			{
				float fDist;
				vOrigin = EntityOrigin(pTaskEdict);

				if ((fDist = (vOrigin - pev->origin).Length2D()) <= pTaskEdict->v.size.Length2D())
				{
					bDone = TRUE;
					break;
				}

				if (fDist < DistanceFrom(m_CurrentTask->TaskVector()))
				{
					bDone = TRUE;
					break;
				}
			}
			else
			{
				vOrigin = m_CurrentTask->TaskVector();

				// if within given range
				if (DistanceFrom(vOrigin) < m_CurrentTask->TaskFloat())
				{
					bDone = TRUE;
					break;
				}
			}

			if ((vOrigin - pev->origin).Length2D() <= 100)
			{
				StopMoving();

				bDone = TRUE;
			}
			else if (HasCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT))
			{
				if (vOrigin.z > pev->origin.z + MAX_JUMP_HEIGHT)
				{
					bTaskFailed = TRUE;
				}

				SetMoveVector(vOrigin);
			}
			else
				bTaskFailed = TRUE;
		}

		break;
		case BOT_TASK_USE_AMMO_DISP:
		{
			// Make marine use an ammo dispenser
			// change weapon after filling one weapon up.
			// hold use to use ammo disp

			edict_t* pAmmoDisp = m_CurrentTask->TaskEdict();
			int iWeaponId;

			// Must have a waypoint path to it or we could
			// be walking into a wall trying to get to it...
			if (m_CurrentTask->HasPath() == FALSE)
			{
				bTaskFailed = TRUE;
				break;
			}

			// ammo disp not defined
			if (pAmmoDisp == nullptr)
			{
				bTaskFailed = FALSE;
				break;
			}

			// no weapon to fill ammo
			if (m_pCurrentWeapon == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			// get current weapon were filling ammo with
			// initially 0.
			if ((iWeaponId = m_CurrentTask->TaskInt()) == 0)
			{
				if (m_iPrimaryWeaponId)
					iWeaponId = m_iPrimaryWeaponId;
				else if (m_iSecondaryWeaponId)
					iWeaponId = m_iSecondaryWeaponId;

				if (iWeaponId)
				{
					m_CurrentTask->SetInt(iWeaponId);

					if (!IsCurrentWeapon(iWeaponId))
					{
						TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, iWeaponId);
						break;
					}
				}
				else
					bDone = TRUE;
			}
			else
			{
				CBotWeapon* pWeapon = m_Weapons.GetWeapon(iWeaponId);

				if (pWeapon == nullptr)
				{
					bTaskFailed = TRUE;
					break;
				}

				// No more ammo to fill
				if (!pWeapon->CanGetMorePrimaryAmmo())
				{
					// change weapon, or finish.
					if (iWeaponId == m_iPrimaryWeaponId)
					{
						iWeaponId = m_iSecondaryWeaponId;

						m_CurrentTask->SetInt(iWeaponId);
						TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, iWeaponId, 0.0, Vector(0, 0, 0), m_CurrentTask->TimeToComplete());
					}
					else
						bDone = TRUE;
				}
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

			float fDistance = (EntityOrigin(pAmmoDisp) - pev->origin).Length2D();//m_CurrentTask->TaskDistanceFrom(m_CurrentTask->TaskVector());

			if (fDistance > pAmmoDisp->v.size.Length2D())
			{
				SetMoveVector(m_CurrentTask->TaskVector());
			}
			else
			{
				StopMoving();
				if (RANDOM_LONG(0, 100) > 1)
				{
					Use();
				}
			}
		}
		break;
		case BOT_TASK_COMBAT_UPGRADE:
		{
			dataUnconstArray<int> m_iPossibleUpgrades;

			if (IsMarine())
			{
				if (BotWantsCombatItem(BOT_COMBAT_WANT_RESUPPLY) /*&& m_pCurrentWeapon && m_pCurrentWeapon->LowOnAmmo()*/)
				{
					m_iPossibleUpgrades.Add(RESEARCH_RESUPPLY);
				}

				// WEAPONS
				if (!HasUser4Mask(MASK_UPGRADE_1))
				{
					m_iPossibleUpgrades.Add(RESEARCH_WEAPONS_ONE);
				}
				else
				{
					// make sure we only get one of these weapons, the one we want
					if (!HasWeapon(NS_WEAPON_SONIC) && !HasWeapon(NS_WEAPON_GRENADE_GUN) && !HasWeapon(NS_WEAPON_HMG))
						m_iPossibleUpgrades.Add(BUILD_SHOTGUN);
					else
					{
						if (BotWantsCombatItem(BOT_COMBAT_WANT_GRENADE_GUN) && !HasWeapon(NS_WEAPON_GRENADE_GUN))
							m_iPossibleUpgrades.Add(BUILD_GRENADE_GUN);

						if (BotWantsCombatItem(BOT_COMBAT_WANT_HMG) && !HasWeapon(NS_WEAPON_HMG))
							m_iPossibleUpgrades.Add(BUILD_HMG);
					}

					if (BotWantsCombatItem(BOT_COMBAT_WANT_WELDER) && !HasWeapon(NS_WEAPON_WELDER))
						m_iPossibleUpgrades.Add(BUILD_WELDER);

					if (BotWantsCombatItem(BOT_COMBAT_WANT_MINES) && !HasWeapon(NS_WEAPON_MINE))
						m_iPossibleUpgrades.Add(BUILD_MINES);

					if (!HasUser4Mask(MASK_UPGRADE_2))
					{
						m_iPossibleUpgrades.Add(RESEARCH_WEAPONS_TWO);
					}
					else
					{
						if (!HasUser4Mask(MASK_UPGRADE_3))
						{
							m_iPossibleUpgrades.Add(RESEARCH_WEAPONS_THREE);
						}
					}
				}
				// ARMOR

				// first armor
				if (!HasUser4Mask(MASK_UPGRADE_4))
				{
					m_iPossibleUpgrades.Add(RESEARCH_ARMOR_ONE);
				}
				else
				{
					// second
					if (!HasUser4Mask(MASK_UPGRADE_5))
					{
						m_iPossibleUpgrades.Add(RESEARCH_ARMOR_TWO);
					}
					else
					{
						// want a jetpack
						if (BotWantsCombatItem(BOT_COMBAT_WANT_JETPACK) && !HasJetPack())
						{
							m_iPossibleUpgrades.Add(BUILD_JETPACK);
						}
						// want heavy armor
						else if (BotWantsCombatItem(BOT_COMBAT_WANT_ARMOR) && !HasUser4Mask(MASK_UPGRADE_13))
						{
							m_iPossibleUpgrades.Add(BUILD_HEAVY);
						}
						// third armor research
						else if (!HasUser4Mask(MASK_UPGRADE_6))
						{
							m_iPossibleUpgrades.Add(RESEARCH_ARMOR_THREE);
						}
					}
				}
			}
			else if (IsAlien())
			{
				if (IsSkulk())
				{
					// want to go onos
					if (BotWantsCombatItem(BOT_COMBAT_WANT_ONOS))
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FIVE); // onos

					else if (BotWantsCombatItem(BOT_COMBAT_WANT_FADE))
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FOUR); // fade

					else  //( BotWantsCombatItem(BOT_COMBAT_WANT_LERK) )
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_THREE); // lerk

					if (UTIL_SpeciesOnTeam(AVH_USER3_ALIEN_PLAYER2) == 0 || UTIL_SpeciesOnTeam(AVH_USER3_ALIEN_PLAYER2) < (int)(UTIL_PlayersOnTeam(TEAM_ALIEN) *
						gBotGlobals.m_fGorgeAmount / 2))
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_TWO); //gorge
				}
				else if (IsFade())
				{
					// want to go onos
					if (BotWantsCombatItem(BOT_COMBAT_WANT_ONOS))
					{
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FIVE); //onos
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FIVE); // more chance!
					}
				}
				else if (IsGorge())
				{
					if (BotWantsCombatItem(BOT_COMBAT_WANT_LERK))
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_THREE); //lerk
					else
					{
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FOUR); // fade
						m_iPossibleUpgrades.Add(ALIEN_LIFEFORM_FOUR); // more chance of going fade
					}
				}

				if (!IsSkulk())
				{
					// How do we find out we already have the ability???
					m_iPossibleUpgrades.Add(118); //ability 1

					// leave some room for onos resources
					if (!BotWantsCombatItem(BOT_COMBAT_WANT_ONOS))
						m_iPossibleUpgrades.Add(126); //ability 2
				}

				if (!HasUser4Mask(MASK_UPGRADE_1) && BotWantsCombatItem(BOT_COMBAT_WANT_DEFUP1))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_ONE);// Carapace
				if (!HasUser4Mask(MASK_UPGRADE_2) && BotWantsCombatItem(BOT_COMBAT_WANT_DEFUP2))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_TWO);// Regeneration
				if (!HasUser4Mask(MASK_UPGRADE_3) && BotWantsCombatItem(BOT_COMBAT_WANT_DEFUP3))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_THREE);// Redemption

				if (!HasUser4Mask(MASK_UPGRADE_4) && BotWantsCombatItem(BOT_COMBAT_WANT_MOVUP1))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_SEVEN);// Celerity
				if (!HasUser4Mask(MASK_UPGRADE_5) && BotWantsCombatItem(BOT_COMBAT_WANT_MOVUP2))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_EIGHT);// Adrenaline
				if (!HasUser4Mask(MASK_UPGRADE_6) && BotWantsCombatItem(BOT_COMBAT_WANT_MOVUP3))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_NINE);// Silence

				if (!HasUser4Mask(MASK_UPGRADE_7) && BotWantsCombatItem(BOT_COMBAT_WANT_SENUP1))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_TEN);// Cloaking
				if (!HasUser4Mask(MASK_UPGRADE_8) && BotWantsCombatItem(BOT_COMBAT_WANT_SENUP2))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_ELEVEN);// Pheromones
				if (!HasUser4Mask(MASK_UPGRADE_9) && BotWantsCombatItem(BOT_COMBAT_WANT_SENUP3))
					m_iPossibleUpgrades.Add(ALIEN_EVOLUTION_TWELVE);// Scent of fear
			}

			if (!m_iPossibleUpgrades.IsEmpty())
			{
				Impulse(m_iPossibleUpgrades.Random());
				m_iPossibleUpgrades.Clear();
				bDone = TRUE;
			}
			else
				bTaskFailed = TRUE;
		}
		break;
		case BOT_TASK_WELD_OBJECT:
			// NS only... bring out a welder and 'attack' the
			// object with the welder
		{
			edict_t* pWeld = m_CurrentTask->TaskEdict();

			if (pWeld == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			// not weldable anymore (finished)?
			if (!EntityIsWeldable(pWeld))
				bDone = TRUE;

			if (!bDone)
			{
				if (!HasWeapon(NS_WEAPON_WELDER))
				{
					//TaskToAdd = CBotTask(BOT_TASK_PICKUP_ITEM,..."weapon_welder"); try looking for a welder
					bTaskFailed = TRUE; // no welder / can't weld
				}
				else if (!IsCurrentWeapon(NS_WEAPON_WELDER))
				{
					TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, NS_WEAPON_WELDER);
				}
				else
				{
					Vector vOrigin = EntityOrigin(pWeld);

					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

					if (DistanceFrom(vOrigin) < 100)
					{
						//m_vMoveToVector =
						PrimaryAttack();
						StopMoving();
					}
					else
					{
						SetMoveVector(vOrigin);
					}
				}
			}
		}
		break;
		case BOT_TASK_PICKUP_ITEM:
			// walk towards the item to pickup
			// and keep checking to see if its still possible
			// to pick it up.
		{
			if (m_CurrentTask->TaskInt() == 0)
			{
				m_pPickupEntity = m_CurrentTask->TaskEdict();
				m_CurrentTask->SetInt(1);
				m_CurrentTask->SetTimeToComplete(10.0);
			}

			if (m_pPickupEntity == nullptr)
			{
				bTaskFailed = TRUE;
			}
			else
			{
				if (!CanPickup(m_pPickupEntity))
					bTaskFailed = TRUE;
				else
				{
					/*if ( !FVisible(m_pPickupEntity->v.origin) )
					{
						bTaskFailed = TRUE;
						break;
					}*/

					SetMoveVector(EntityOrigin(m_pPickupEntity));

					if (gBotGlobals.IsMod(MOD_TS))
					{
						if (DistanceFromEdict(m_pPickupEntity) < 64)
							Use();
					}
				}
			}
		}
		break;
		case BOT_TASK_GOTO_FLANK_POSITION:
			break;
		case BOT_TASK_CHANGE_WEAPON:

			// dont let attack enemy task take over this or we are in deadlock
			UpdateCondition(BOT_CONDITION_CANT_SHOOT);

			if (!SwitchWeapon(m_CurrentTask->TaskInt()))
				bTaskFailed = TRUE;

			bDone = TRUE;//done
			break;
		case BOT_TASK_HEAL_PLAYER:
		{
			if (m_pEnemy || HasCondition(BOT_CONDITION_SEE_ENEMY))
			{
				bTaskFailed = TRUE;
				break;
			}

			edict_t* pPlayer = m_CurrentTask->TaskEdict();

			if (pPlayer == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}
			else if (!EntityIsAlive(pPlayer))
				bDone = TRUE;
			else if (pev->waterlevel > 1 || pPlayer->v.waterlevel > 1)
				bTaskFailed = TRUE;
			else
			{
				switch (gBotGlobals.m_iCurrentMod)
				{
				case MOD_NS:
					if (pPlayer == m_pEdict)
					{
						if (!IsFade())
						{
							bTaskFailed = TRUE;
							break;
						}
						else
						{
							if (!IsCurrentWeapon(NS_WEAPON_METABOLIZE))
							{
								if (m_iLastFailedTask == BOT_TASK_CHANGE_WEAPON)
									bTaskFailed = TRUE;
								else
									TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, NS_WEAPON_METABOLIZE, 0.0, Vector(0, 0, 0), m_CurrentTask->TimeToComplete());
							}
							else
							{
								if (pev->health < pev->max_health)
								{
									if (RANDOM_LONG(0, 100) < 50)
										PrimaryAttack();
								}
								else
									bDone = TRUE;
							}
						}
					}
					else
					{
						if (!IsGorge())
						{
							bTaskFailed = TRUE;
							break;
						}
						// dont 'heal' an enemy? should be attack..
						else if (IsEnemy(pPlayer))
						{
							bTaskFailed = TRUE;
							break;
						}
						else
						{
							if (!IsCurrentWeapon(NS_WEAPON_HEALINGSPRAY))
							{
								if (m_iLastFailedTask == BOT_TASK_CHANGE_WEAPON)
									bTaskFailed = TRUE;
								else
									TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, NS_WEAPON_HEALINGSPRAY, 0.0, Vector(0, 0, 0), m_CurrentTask->TimeToComplete());
							}
							else
							{
								float max_health = pPlayer->v.max_health * 0.95;

								if (UTIL_EntityIsHive(pPlayer))
									max_health = pPlayer->v.max_health * 0.8;

								if (pPlayer->v.health < max_health)
								{
									if (NS_AmountOfEnergy() > 45)
									{
										m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

										if (RANDOM_LONG(0, 100) < 50)
											PrimaryAttack();
									}
									else
										m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;
								}
								else
									bDone = TRUE;
							}
						}
					}
					break;
				}
			}
		}
		break;
		case BOT_TASK_WAIT_FOR_RESOURCES:
			// make bot wait until resources in NS >= taskInt
		{
			int iResources = m_CurrentTask->TaskInt();

			edict_t* pStructure = m_CurrentTask->TaskEdict();

			if (pStructure)
			{
				if (pStructure->v.iuser3 == AVH_USER3_HIVE)
				{
					if (!UTIL_CanBuildHive(&pStructure->v))
						//if ( pStructure->v.iuser4 & MASK_BUILDABLE )
					{
						bDone = TRUE;
						break;
					}
				}
				else if (pStructure->v.iuser3 == AVH_USER3_FUNC_RESOURCE)
				{
					if (UTIL_FuncResourceIsOccupied(pStructure))
					{
						bDone = TRUE;
						break;
					}
				}
			}

			if (iResources > 100)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (iResources <= m_iResources)
				bDone = TRUE;
			else
			{
				// look around for enemies
				m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;
				// stay still
				StopMoving();
			}
		}
		break;
		case BOT_TASK_SENSE_ENEMY:
		{
			edict_t* pEnt = nullptr;

			if (m_pEnemy)
			{
				bDone = TRUE;
				break;
			}

			if (!m_CurrentTask->TaskFloat())
			{
				m_CurrentTask->SetFloat(gpGlobals->time + 2.0);
			}

			while ((pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin, 512)) != nullptr)
			{
				if (pEnt == m_pEdict || pEnt->v.owner == m_pEdict || pEnt->free || !pEnt->v.classname || !*STRING(pEnt->v.classname))
					continue;

				if (pEnt->v.velocity.Length() > 100 || pEnt->v.button & IN_ATTACK)
				{
					if (IsEnemy(pEnt))
					{
						break;
					}
				}
			}

			bDone = m_CurrentTask->TaskFloat() < gpGlobals->time;

			if (pEnt)
			{
				m_Tasks.FinishedCurrentTask();
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, 0, pEnt));
				bDone = FALSE;
			}
		}
		break;
		case BOT_TASK_RANGE_ATTACK:
			break;
		case BOT_TASK_NORMAL_ATTACK:

			m_pEnemy = m_CurrentTask->TaskEdict();

			if (m_pEnemy == nullptr)
			{
				m_pLastEnemy = nullptr;
				bDone = TRUE;
				break;
			}

			if (!IsEnemy(m_CurrentTask->TaskEdict()))
			{
				/*	if ( m_pEnemy )
					{
						if ( m_pEnemy == m_pElectricEnemy )
							dec_attackElectrified->train(1.0f-(m_pElectricEnemy->v.health/m_pElectricEnemy->v.max_health));
					}*/

					//enemy not alive anymore
				m_pLastEnemy = nullptr;
				m_pEnemy = nullptr;

				bDone = TRUE;

				EnemyDown(m_pEnemy);

				//				m_Tasks.RemoveSchedule(BOT_SCHED_RUN_FOR_COVER);
				break;
			}

			if (m_CurrentTask->TaskFloat())
			{
				float fShootTime = m_CurrentTask->TaskFloat();

				StopMoving();

				if (fShootTime < gpGlobals->time)
				{
					bDone = TRUE;
					break;
				}
			}

			if (HasCondition(BOT_CONDITION_SEE_ENEMY))
			{
				// Do attacking stuff in HERE!!!
				Vector vEnemyOrigin = EntityOrigin(m_pEnemy);

				m_CurrentLookTask = BOT_LOOK_TASK_FACE_ENEMY;

				float fEnemyDist = (vEnemyOrigin - GetGunPosition()).Length();

				m_pLastEnemy = m_pEnemy;
				m_vLastSeeEnemyPosition.SetVector(vEnemyOrigin);

				if (m_pCurrentWeapon && m_pCurrentWeapon->IsMelee())
				{
					if (pev->movetype == MOVETYPE_FLY)
					{
						if (RANDOM_LONG(0, 1))
							Jump();
					}
				}

				if (IsUsingTank())
				{
					edict_t* pUsingTank = GetUsingTank();

					if (pUsingTank)
					{
						// just shoot if using tank
						if (RANDOM_LONG(0, 100))
							PrimaryAttack();

						// dont change weapon or anything
						break;
					}
				}

				switch (gBotGlobals.m_iCurrentMod)
				{
				case MOD_NS:
				{
					BOOL bChangeWeapon = FALSE;

					if (m_pCurrentWeapon == nullptr || m_pCurrentWeapon->HasWeapon(m_pEdict) == FALSE)
					{
						//Get weapon
						bChangeWeapon = TRUE;
					}

					if (!bChangeWeapon)
					{
						if (m_CurrentTask->TaskInt() == 0) // not chosen weapon yet?
						{
							int iWeaponId = m_Weapons.GetBestWeaponId(this, m_pEnemy);

							if (iWeaponId != 0)
							{
								m_CurrentTask->SetInt(iWeaponId);
								bChangeWeapon = TRUE;
							}
							else
							{
								m_iLastFailedWaypoint = m_iPrevWaypointIndex;

								if (WantToFollowEnemy(m_pEnemy))
								{
									int iScheduleId = m_CurrentTask->GetScheduleId();
									m_Tasks.FinishSchedule(iScheduleId);
									AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iScheduleId, m_pEnemy));
									break;
								}
								else
									bTaskFailed = TRUE;

								break;
							}
						}
						else if (m_CurrentTask->TaskInt() != m_pCurrentWeapon->GetID())
						{
							bChangeWeapon = TRUE;
						}
						else if (!m_pCurrentWeapon->IsMelee() && !m_pCurrentWeapon->CanShootPrimary(m_pEdict, fEnemyDist, m_fDistanceFromWall))
						{
							m_CurrentTask->SetInt(m_Weapons.GetBestWeaponId(this, m_pEnemy));
							bChangeWeapon = TRUE;
						}
					}

					if (bChangeWeapon)
					{
						if (!IsCurrentWeapon(m_CurrentTask->TaskInt()))
						{
							TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, m_CurrentTask->TaskInt());

							break;
						}
					}

					if (m_pCurrentWeapon->IsMelee())
					{
						if (fEnemyDist > pev->size.Length2D())
						{
							SetMoveVector(vEnemyOrigin);
						}
						else
							StopMoving();

						decideJumpDuckStrafe(fEnemyDist, vEnemyOrigin);

						/*
						if ( IsSkulk() )
						{
							float m_fSkillJumpTime = ((float)(MAX_BOT_SKILL-m_Profile.m_iSkill)/MAX_BOT_SKILL)*1.0;

							if ( (m_f2dVelocity > 200.0) && (fEnemyDist < 400.0) && (m_fStartJumpTime < gpGlobals->time && (m_fEndJumpTime + 1.0) < gpGlobals->time) )
							{
								m_fStartJumpTime = gpGlobals->time + 0.5;
								m_fEndJumpTime = gpGlobals->time + RANDOM_FLOAT(0.75,1.5) + m_fSkillJumpTime;;
							}

							if ( (m_fStrafeTime + 0.75) < gpGlobals->time )
							{
								m_fStrafeTime = gpGlobals->time + (1.0 + RANDOM_FLOAT(m_fSkillJumpTime-0.25,m_fSkillJumpTime+0.25));
								m_fStrafeSpeed = (RANDOM_FLOAT(0,2.0) - 1.0) * m_fMaxSpeed;
							}
						}*/
					}
					else if (m_pCurrentWeapon->PrimaryInRange(fEnemyDist) == -1)
					{
						// naive "is melee" check
						if (fEnemyDist > m_pCurrentWeapon->PrimMinRange())
						{
							//float fMinRange = m_pCurrentWeapon->PrimMinRange();

							//						m_GoalTasks.Push(CBotTask(BOT_TASK_MOVE_TO_VECTOR,NULL,0,0,EntityOrigin(m_pEnemy)));

							//TaskToAdd = CBotTask(BOT_TASK_FIND_PATH,m_pEnemy);
							SetMoveVector(vEnemyOrigin);
						}
					}
					else if (IsMarine() && HasJetPack())
					{
						if (m_fEndJumpTime + 0.5 < gpGlobals->time)
						{
							m_fStartJumpTime = 0;
							m_fEndJumpTime = gpGlobals->time + RANDOM_FLOAT(1.5, 3.0);
						}
					}
					else if (IsLerk())
					{
						if (HasCondition(BOT_CONDITION_SEE_NEXT_WAYPOINT) && m_vMoveToVector.z + 128.0 > pev->origin.z)
						{
							// make sure I got enough energy to actually shoot my enemy!!
							if (NS_AmountOfEnergy() > 40)
								FlapWings();
						}
					}

					if (IsMarine())
					{
						if (!m_pCurrentWeapon->IsMelee())
						{
							if (m_pCurrentWeapon->OutOfAmmo())
							{
								// Ammo all out of current weapon.
								// Find a new weapon to switch to.
								int iChangeWeaponTo;

								if ((iChangeWeaponTo = m_Weapons.GetBestWeaponId(this, m_pEnemy)) != 0)
								{
									TaskToAdd = CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, iChangeWeaponTo);
								}
							}
							else if (m_pCurrentWeapon->NeedToReload())
							{
								RunForCover(vEnemyOrigin);
							}
							else // not melee weapon...
							{
								if (fEnemyDist < 256.0)
								{
									//------------------
									SetMoveVector(m_vLowestEnemyCostVec);
									// stay away from enemy
									//m_pAvoidEntity = m_pEnemy;
								}
								// let GA handle this
								/*if ( fEnemyDist < 128.0 )
								{
									// stay away from enemy
									m_pAvoidEntity = m_pEnemy;
									//SetMoveVector(((m_vMoveToVector-pev->origin).Normalize() + (pev->origin-vEnemyOrigin).Normalize())*128.0);
								}*/
							}
						}
					}

					//if ( m_fNextAttackTime < gpGlobals->time )
					//{
					/*weapon_data_t pCurWeapon;

					  #ifdef RCBOT_META_BUILD
					  if ( MDLL_GetWeaponData(m_pEdict,&pCurWeapon) )
					  #else
					  if ( GetWeaponData(m_pEdict,&pCurWeapon) )
					  #endif
					  {
					  m_fNextAttackTime = pCurWeapon.m_flNextPrimaryAttack + (m_fReactionTime * (1 - (m_Profile.m_iSkill / 100)));
					  }
					else*/

					pev->button &= ~IN_ATTACK;
					pev->button &= ~IN_ATTACK2;

					if (RANDOM_LONG(0, 100) < 50)
					{
						//	if ( DotProductFromOrigin(&vEnemyOrigin) > 0.8 )
						//	{
								//m_fNextAttackTime = gpGlobals->time + m_fReactionTime;
						PrimaryAttack();
						//	}
					}

					//}
				}
				break;
				case MOD_BUMPERCARS:
					// bump into em!!!

					if (fEnemyDist < REACHABLE_RANGE)
					{
						SetMoveVector(vEnemyOrigin);

						//rendermode 0
						//renderamt 30
						//rendercolor 255 255 255
												//renderfx 19

												// nearby an enemy?
						if (fEnemyDist < 150)
						{
							// Does bot have a bomb?
							if (pev->rendermode == 0 &&
								pev->renderamt == 30 &&
								pev->rendercolor == Vector(255, 255, 255) &&
								pev->renderfx == 19)
							{
								// set it off
								if (RANDOM_LONG(0, 1))
									PrimaryAttack();
							}
						}
					}
					else if (m_CurrentTask->HasPath() == FALSE)
					{
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, m_CurrentTask->GetScheduleId(), m_pEnemy, -2, -2));
						m_CurrentTask->SetPathInfo(TRUE);
					}
					else if (m_iWaypointGoalIndex == -1)
					{
						SetMoveVector(vEnemyOrigin);
					}
					else if (m_iCurrentWaypointIndex == m_iWaypointGoalIndex)
					{
						SetMoveVector(vEnemyOrigin);
					}

					break;
				case MOD_TS:
				{
					/*BOOL bChangeWeapon = FALSE;

					if ( (m_pCurrentWeapon == NULL) || (m_pCurrentWeapon->HasWeapon(m_pEdict)==FALSE)&&(m_pCurrentWeapon->GetID()!=36) )
					{
						//Get weapon
						bChangeWeapon = TRUE;
					}

					// lets say that an int of 0 means not chosen a weapon...
					if ( m_CurrentTask->TaskInt() == 0 ) // not chosen weapon yet?
					{
						bChangeWeapon = TRUE;
					}
					else if ( m_CurrentTask->TaskInt() != m_pCurrentWeapon->GetID() )
					{
						bChangeWeapon = TRUE;
					}

					if ( bChangeWeapon )
					{
						int iWeaponId;

						iWeaponId = m_Weapons.GetBestWeaponId(this,m_pEnemy);

						m_CurrentTask->SetInt(iWeaponId);

						if ( !IsCurrentWeapon(m_CurrentTask->TaskInt()) )
						{
							AddPriorityTask(CBotTask(BOT_TASK_CHANGE_WEAPON,m_CurrentTask->GetScheduleId(),NULL,m_CurrentTask->TaskInt()));

							break;
						}
						else if ( m_pCurrentWeapon && m_pCurrentWeapon->OutOfAmmo() )
						{
							bTaskFailed = TRUE;
							break;
						}
					}
					else if ( !m_pCurrentWeapon || m_pCurrentWeapon->IsMelee() || (m_pCurrentWeapon->GetID() == 36) )
					{*/

					/*
												SetMoveVector(vEnemyOrigin);

												if ( !m_pCurrentWeapon || m_pCurrentWeapon->GetID() == 36 )
												{
													if ( fEnemyDist < 100 )
													{
														if ( RANDOM_LONG(0,100) > 75 )
															Jump();
													}

													if ( fEnemyDist < 72 )
													{
														if ( RANDOM_LONG(0,100) > 90 )
															PrimaryAttack();
														else
														{
															SecondaryAttack();
															pev->button |= IN_USE;
														}
													}
												}

												// low?
												if ( pev->groundentity == m_pEnemy )
													Duck();
												// too high?
												else if ( vEnemyOrigin.z > (pev->origin.z + MAX_JUMP_HEIGHT) )
												{
													bTaskFailed = TRUE;
													break;
												}
												break;
											}*/
											/*else if ( m_pCurrentWeapon->NeedToReload() )
											{
												RunForCover(vEnemyOrigin);

												// battlegrounds, MUST reload, does not do it automatically.
												if ( gBotGlobals.IsMod(MOD_BG) )
													TaskToAdd = CBotTask(BOT_TASK_RELOAD);
											}
											else if ( !m_pCurrentWeapon->CanShootPrimary(m_pEdict,fEnemyDist,m_fDistanceFromWall) )
											{
												int iWeaponId = m_Weapons.GetBestWeaponId(this,m_pEnemy);

												if ( iWeaponId )
												{
													m_CurrentTask->SetInt(m_Weapons.GetBestWeaponId(this,m_pEnemy));
													bChangeWeapon = TRUE;
												}
												else
												{
													m_iLastFailedWaypoint = m_iPrevWaypointIndex;

													if ( WantToFollowEnemy(m_pEnemy) )
													{
														int iScheduleId = m_CurrentTask->GetScheduleId();
														m_Tasks.FinishSchedule(iScheduleId);
														AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,iScheduleId,m_pEnemy));
														break;
													}
													else
														bTaskFailed = TRUE;

													break;
												}
											}
											// let GA do this
											else if ( fEnemyDist < 256.0 )
											{
												//------------------
												SetMoveVector(m_vLowestEnemyCostVec);
												// stay away from enemy
												//m_pAvoidEntity = m_pEnemy;
											}*/

											/*if ( RANDOM_LONG(0,1) )
												PrimaryAttack();*/

					BOOL bMelee = FALSE;

					float weap_id = 0.0f;
					int iweapid = 0;

					if (m_pCurrentWeapon)
						iweapid = m_pCurrentWeapon->GetID();

					weap_id = (float)iweapid / (float)MAX_WEAPONS;

					if (!m_pCurrentWeapon || (m_pCurrentWeapon->GetID() == 34 || m_pCurrentWeapon->GetID() == 35 || m_pCurrentWeapon->GetID() == 36 || m_pCurrentWeapon->GetID() == 25))
					{
						/*	if ( fEnemyDist < 100 )
							{
								if ( RANDOM_LONG(0,100) > 75 )
									Jump();
						}*/
						if (!WaypointReachable(pev->origin, EntityOrigin(m_pEnemy), FALSE))// fEnemyDist > 512.0 )
						{
							//------------------
							SetMoveVector(m_vLowestEnemyCostVec);
						}
						else
							SetMoveVector(vEnemyOrigin);

						bMelee = TRUE;

						SetMoveVector(vEnemyOrigin);

						if (fEnemyDist < 256)
							decideJumpDuckStrafe(fEnemyDist, vEnemyOrigin);

						if (fEnemyDist < 72)
						{
							if (RANDOM_LONG(0, 100) > 90)
								PrimaryAttack();
							else
							{
								SecondaryAttack();

								if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TS_DONT_STEAL_WEAPONS))
									Use();
							}
						}
					}

					if (!bMelee)
					{
						dec_stunt->setWeights(m_pTSWeaponSelect, 3, 5);

						std::vector<ga_value> inputs;

						inputs.push_back((float)m_iTS_State / 2);
						inputs.push_back(pev->velocity.Length() / m_fMaxSpeed);
						inputs.push_back(m_pEnemy->v.velocity.Length() / m_pEnemy->v.maxspeed);
						inputs.push_back(UTIL_YawAngleBetweenOrigin(pev, vEnemyOrigin) / 180);
						inputs.push_back(weap_id);

						dec_stunt->input(&inputs);
						dec_stunt->execute();

						// replace with perceptron
						if (dec_stunt->fired())
						{
							//m_vLowestEnemyCostVec.
							//SetMoveVector(m_vLowestEnemyCostVec);
							AltButton();
						}

						//float fire;

						//dec_firepercent->setWeights(m_pTSWeaponSelect,8,MAX_WEAPONS);
						//inputs.clear();

						//for ( int j = 0; j < MAX_WEAPONS; j ++ )
						//	inputs.push_back((float)(iweapid==j));

						//dec_firepercent->execute();
						/*fire = m_pTSWeaponSelect->get(8+iweapid);//dec_firepercent->getOutput();

						if ( (fire < 0.001) || (fire > 0.999) )
							m_pTSWeaponSelect->set(8+iweapid,0.5);*/

						if (m_pCurrentWeapon->NeedToReload())
						{
							RunForCover(vEnemyOrigin);
						}
						else if (m_fHoldAttackTime > gpGlobals->time || RANDOM_LONG(0, 100) < 90)
						{
							//	if ( DotProductFromOrigin(&vEnemyOrigin) > 0.8 )
							//	{
							//m_fNextAttackTime = gpGlobals->time + m_fReactionTime;

							//if ( bAttack )
							PrimaryAttack();
							//	}
						}

						if (fEnemyDist < 256.0)
						{
							//------------------
							SetMoveVector(m_vLowestEnemyCostVec);
						}
						// dont know when we'reout of bullets, so it's random for now
					//	else if ( !RANDOM_LONG(0,10) )
						//	Reload();
							/*else if ( m_pCurrentWeapon->SecondaryInRange(fEnemyDist) && m_pCurrentWeapon->CanShootSecondary() && (RANDOM_LONG(0,100) < 50) )
							{
							SecondaryAttack();
					}*/
					}
				}
				break;
				case MOD_RC:
				case MOD_RC2:
				case MOD_HL_DM:
				default:
				{
					BOOL bChangeWeapon = FALSE;

					if (m_pCurrentWeapon == nullptr || m_pCurrentWeapon->HasWeapon(m_pEdict) == FALSE)
					{
						//Get weapon
						bChangeWeapon = TRUE;
					}

					// lets say that an int of 0 means not chosen a weapon...
					if (m_CurrentTask->TaskInt() == 0) // not chosen weapon yet?
					{
						bChangeWeapon = TRUE;
					}
					else if (m_CurrentTask->TaskInt() != m_pCurrentWeapon->GetID())
					{
						bChangeWeapon = TRUE;
					}
					else if (m_pCurrentWeapon->IsMelee())
					{
						// need to get closer to enemy
						if (m_pCurrentWeapon->PrimaryInRange(fEnemyDist) != 0)
						{
							SetMoveVector(vEnemyOrigin);
						}

						// low?
						if (pev->groundentity == m_pEnemy)
							Duck();
						// too high?
						else if (vEnemyOrigin.z > pev->origin.z + MAX_JUMP_HEIGHT)
						{
							bTaskFailed = TRUE;
							break;
						}

						decideJumpDuckStrafe(fEnemyDist, vEnemyOrigin);
					}
					else if (m_pCurrentWeapon->NeedToReload())
					{
						RunForCover(vEnemyOrigin);

						// battlegrounds, MUST reload, does not do it automatically.
						if (gBotGlobals.IsMod(MOD_BG))
							TaskToAdd = CBotTask(BOT_TASK_RELOAD);
					}
					else if (!m_pCurrentWeapon->CanShootPrimary(m_pEdict, fEnemyDist, m_fDistanceFromWall))
					{
						int iWeaponId = m_Weapons.GetBestWeaponId(this, m_pEnemy);

						if (iWeaponId)
						{
							m_CurrentTask->SetInt(m_Weapons.GetBestWeaponId(this, m_pEnemy));
							bChangeWeapon = TRUE;
						}
						else
						{
							m_iLastFailedWaypoint = m_iPrevWaypointIndex;

							if (WantToFollowEnemy(m_pEnemy))
							{
								int iScheduleId = m_CurrentTask->GetScheduleId();
								m_Tasks.FinishSchedule(iScheduleId);
								AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iScheduleId, m_pEnemy));
								break;
							}
							else
								bTaskFailed = TRUE;

							break;
						}
					}
					// let GA do this
					else if (fEnemyDist < 256.0)
					{
						//------------------
						SetMoveVector(m_vLowestEnemyCostVec);
						// stay away from enemy
						//m_pAvoidEntity = m_pEnemy;
					}

					if (bChangeWeapon)
					{
						int iWeaponId;

						iWeaponId = m_Weapons.GetBestWeaponId(this, m_pEnemy);

						m_CurrentTask->SetInt(iWeaponId);

						if (!IsCurrentWeapon(m_CurrentTask->TaskInt()))
						{
							AddPriorityTask(CBotTask(BOT_TASK_CHANGE_WEAPON, m_CurrentTask->GetScheduleId(), nullptr, m_CurrentTask->TaskInt()));

							break;
						}
						else if (m_pCurrentWeapon && m_pCurrentWeapon->OutOfAmmo())
						{
							bTaskFailed = TRUE;
							break;
						}
					}

					int iFirePercent = 75;
					BOOL bAttack = TRUE;

					if (gBotGlobals.IsMod(MOD_DMC))
					{
						if (m_stBotPaths.IsEmpty() && !WantToFollowEnemy(m_pEnemy))
						{
							int iCoverWpt = WaypointLocations.GetCoverWaypoint(pev->origin, vEnemyOrigin, &m_FailedGoals);

							if (iCoverWpt != -1)
								AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iCoverWpt, -2));
						}

						iFirePercent = 90;
					}

					if (gBotGlobals.IsMod(MOD_BG) && !m_pCurrentWeapon->IsMelee())
					{
						// crouch if possible for best aim
						m_fEndDuckTime = gpGlobals->time + 2.0;

						if (m_fFirstSeeEnemyTime + 0.5 < gpGlobals->time)
						{
							if (RANDOM_LONG(0, 1))
								PrimaryAttack();
						}
					}
					else
					{
						if (m_fHoldAttackTime > gpGlobals->time || RANDOM_LONG(0, 100) < iFirePercent)
						{
							//	if ( DotProductFromOrigin(&vEnemyOrigin) > 0.8 )
							//	{
							//m_fNextAttackTime = gpGlobals->time + m_fReactionTime;

							if (bAttack)
							{
								PrimaryAttack();

								m_fNextShootButton = gpGlobals->time + RANDOM_FLOAT(1.0, 2.0);
							}
							//	}
						}
						else if (m_pCurrentWeapon->SecondaryInRange(fEnemyDist) && m_pCurrentWeapon->CanShootSecondary() && RANDOM_LONG(0, 100) < 50)
						{
							SecondaryAttack();
						}
					}
				}
				break;
				}
			}
			else
			{
				if (HasSeenEnemy(m_pEnemy)) // Bot has seen this enemy
				{
					Vector vLastEnemyPosition;

					vLastEnemyPosition = m_vLastSeeEnemyPosition.GetVector();

					m_pLastEnemy = m_pEnemy;
					m_fSearchEnemyTime = gpGlobals->time + RANDOM_LONG(5, 10);

					RememberPosition(vLastEnemyPosition, m_pEnemy, BOT_REMEMBER_LOST_ENEMY);
					// used to check if enemy has just gone out of FOV
					if (FVisible(vLastEnemyPosition))
					{
						UpdateCondition(BOT_CONDITION_SEE_ENEMY);
						m_CurrentLookTask = BOT_LOOK_TASK_FACE_ENEMY;
					}
					else
					{
						if (!WantToFollowEnemy(m_pEnemy))
						{
							bTaskFailed = TRUE;
						}
						else
						{
							if (DistanceFrom(vLastEnemyPosition) > 100)
							{
								m_Tasks.FinishedCurrentTask();

								int iNewScheduleId = this->m_Tasks.GetNewScheduleId();

								if (!m_Tasks.HasTask(BOT_TASK_SEARCH_FOR_ENEMY))
								{
									if (gBotGlobals.IsNS() && IsMarine() && HasWeapon(NS_WEAPON_MINE))
										AddPriorityTask(CBotTask(BOT_TASK_DEPLOY_MINES, iNewScheduleId, nullptr, 0, 0, Vector(0, 0, 0), 10.0));
									else if (IsGorge() && hasWeb())
										AddPriorityTask(CBotTask(BOT_TASK_WEB, iNewScheduleId)); // do a little webbing
									AddPriorityTask(CBotTask(BOT_TASK_SEARCH_FOR_ENEMY, iNewScheduleId, nullptr, 0, RANDOM_FLOAT(3.0, 6.0)));
									AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, iNewScheduleId, nullptr, 0, 0, vLastEnemyPosition, RANDOM_FLOAT(10.0, 20.0)));
								}
							}
							else
							{
								m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_ENEMY;
								bTaskFailed = TRUE;
								break;
							}
						}

						if (bTaskFailed)
						{
							if (DistanceFrom(vLastEnemyPosition) < 400 && HasWeapon(VALVE_WEAPON_HANDGRENADE))
								TaskToAdd = CBotTask(BOT_TASK_THROW_GRENADE, m_Tasks.GetNewScheduleId(), nullptr, 0, 0, vLastEnemyPosition);
						}
					}
				}
				else
				{
					// what can this bot do now????
					// has been told to attack but has no idea where the enemy is?
					// lets wander off?
					// set bDone to TRUE for now
					//int iNewScheduleId = m_Tasks.GetNewScheduleId();

					/*if ( !m_Tasks.HasTask(BOT_TASK_SEARCH_FOR_ENEMY) )
					{
						AddPriorityTask(CBotTask(BOT_TASK_SEARCH_FOR_ENEMY,iNewScheduleId,NULL,0,RANDOM_FLOAT(3.0,6.0)));
						AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH,iNewScheduleId,NULL,0,0,m_vLastSeeEnemyPosition.GetVector(),RANDOM_FLOAT(10.0,20.0)));
					}*/

					m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY;
					m_pEnemy = nullptr;
					m_pLastEnemy = nullptr;

					bDone = TRUE;
				}
			}

			break;
		case BOT_TASK_ACCEPT_HEALTH:
		{
			edict_t* pSupplier = m_CurrentTask->TaskEdict();

			if (m_CurrentTask->TaskInt() == 0) // use as initializer
			{
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(1.0, 2.0));
				m_CurrentTask->SetInt(pev->health);
			}

			int iPrevHealth = m_CurrentTask->TaskInt();

			if (pev->health > iPrevHealth) // wait a bit longer
			{
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(1.0, 2.0));
				m_CurrentTask->SetVector(Vector(1.0, 0, 0));
				m_CurrentTask->SetInt(pev->health);
			}

			if (pev->health >= pev->max_health || m_CurrentTask->TaskFloat() < gpGlobals->time)
			{
				// got more health
				if (m_CurrentTask->TaskVector().x == 1.0)
					BotChat(BOT_CHAT_THANKS, pSupplier);

				m_bAcceptHealth = FALSE;
				bDone = TRUE;
			}
			else
			{
				StopMoving();
				m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

				/*if (!m_pEnemy && pSupplier)
				{
					if (pSupplier->v.health < pSupplier->v.max_health)
					{
						if (m_pCurrentWeapon && m_pCurrentWeapon->GetID() == SVEN_WEAPON_MEDKIT)
						{
							if (RANDOM_LONG(0, 100) < 10)
								PrimaryAttack();
						}
						else if (HasWeapon(SVEN_WEAPON_MEDKIT) && !m_pCurrentWeapon || m_pCurrentWeapon->GetID() != SVEN_WEAPON_MEDKIT)
						{
							SwitchWeapon(SVEN_WEAPON_MEDKIT);
						}
					}
				}*/
			}
		}
		break;
		case BOT_TASK_WAIT_FOR_ENTITY:
		{
			edict_t* pTaskEdict = m_CurrentTask->TaskEdict();

			if (pTaskEdict != nullptr)
			{
				if (BotFunc_EntityIsMoving(&pTaskEdict->v))
				{
					StopMoving();
					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;
				}
				else
					bDone = TRUE;
			}
			else
				bTaskFailed = TRUE;
		}
		break;
		case BOT_TASK_HIDE:

			Duck();
			StopMoving();

			if (m_CurrentTask->TaskInt() == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1);
			}
			else
			{
				bDone = m_CurrentTask->TaskFloat() < gpGlobals->time;
			}

			break;
		case BOT_TASK_ASSEMBLE_SQUAD:
		{
			m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

			if (gBotGlobals.IsNS())
			{
				if (IsGorge())
				{
					bTaskFailed = TRUE;
					break;
				}
			}

			if (m_CurrentTask->TaskInt() == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(4.0, 6.0));
				m_CurrentTask->SetInt(1);
			}
			else
			{
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					bDone = TRUE;
					break;
				}
			}

			if (m_stSquad == nullptr)
			{
				// not in squad
				int i;
				edict_t* pEntity;

				dataStack<edict_t*> theTeamList;

				for (i = 1; i <= gpGlobals->maxClients; i++)
				{
					pEntity = INDEXENT(i);

					if (pEntity == nullptr)
						continue;
					if (pEntity == m_pEdict)
						continue;
					if (!EntityIsAlive(pEntity))
						continue;
					if (UTIL_GetTeam(pEntity) != GetTeam())
						continue;

					if (IsInVisibleList(pEntity))
					{
						if (!IsEnemy(pEntity))
						{
							// If a combat map
							if (gBotGlobals.IsNS() && gBotGlobals.IsCombatMap())
							{
								// dont group up with gorges if not a gorge (gorges tend to defend..)
								if (!IsGorge() && pEntity->v.iuser3 == AVH_USER3_ALIEN_PLAYER2)
									continue;
							}

							theTeamList.Push(pEntity);
						}
					}
				}

				if (!theTeamList.IsEmpty())
				{
					// pick best to join

					dataStack <edict_t*> tempStack = theTeamList;
					edict_t* pBest = nullptr;
					float fFactor;
					float fBestFactor = 0;

					while (!tempStack.IsEmpty())
					{
						pEntity = tempStack.ChooseFromStack();

						fFactor = DistanceFromEdict(pEntity);

						if (!pBest || fFactor < fBestFactor)
						{
							pBest = pEntity;
							fBestFactor = fFactor;
						}
					}

					if (pBest != nullptr)
						m_stSquad = gBotGlobals.m_Squads.AddSquadMember(pBest, m_pEdict);

					theTeamList.Destroy();
				}
			}
		}
		break;
		case BOT_TASK_CROUCH:
		{
			// Crouch for a while
			// ...

			Duck();

			if (!m_CurrentTask->TaskInt())
			{
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1);
			}
			else if (m_CurrentTask->TaskFloat() < gpGlobals->time)
			{
				// fini

				bDone = TRUE;
			}
		}
		break;
		case BOT_TASK_WAIT_FOR_ORDERS:
			// UNUSED - had it in for a while
			// but better to leave bots with no task so they
			// check for buildings to build & weld etc.
			//
			// got an order or no commander anymore, its done
			if (m_bHasOrder || EntityIsCommander(nullptr))
				bDone = TRUE;

			m_CurrentLookTask = BOT_LOOK_TASK_LOOK_AROUND;

			// keep looking for new tasks
			// bLookForNewTasks = TRUE;

			StopMoving();

			break;
		case BOT_TASK_SOLO_RUN:
			break;
		case BOT_TASK_BLINK:
		{
			if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_BLINKING))
			{
				bTaskFailed = TRUE;
				break;
			}

			if (IsOnLadder() || !hasBlink())
			{
				bTaskFailed = TRUE;
				break;
			}
			if (m_pCurrentWeapon == nullptr || m_pCurrentWeapon->GetID() != NS_WEAPON_BLINK)
			{
				if (m_iLastFailedTask == BOT_TASK_CHANGE_WEAPON)
				{
					bTaskFailed = TRUE;
					break;
				}

				AddPriorityTask(CBotTask(BOT_TASK_CHANGE_WEAPON, 0, nullptr, NS_WEAPON_BLINK));

				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

			m_CurrentTask->SetVector(m_vMoveToVector + Vector(0, 0, pev->size.z / 2));

			bDone = DistanceFrom(m_vMoveToVector, TRUE) < pow(pev->size.x + pev->size.y, 0.5f) && m_vMoveToVector.z < pev->origin.z;

			//if ( RANDOM_LONG(0,50) )
			PrimaryAttack();

			if (!onGround())
				Duck();
		}
		break;
		case BOT_TASK_BUILD:
			// Might be same as USE, but requires the bot to wait until it is built.
		{
			edict_t* pTaskEdict = m_CurrentTask->TaskEdict();

			//if ( HasCondition(BOT_CONDITION_SEE_TASK_EDICT) )

			if (pTaskEdict == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}
			//{
			if (EntityIsBuildable(pTaskEdict))
			{
				Vector vPlayer = pev->origin;
				Vector vBuilding = EntityOrigin(pTaskEdict);
				float fDistance = (vBuilding - vPlayer).Length2D();
				float fSize = pTaskEdict->v.size.Length2D();

				m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

				//					float fHealth = m_CurrentTask->TaskFloat();

				if (fDistance < fSize)
				{
					if (RANDOM_LONG(0, 100) > 0)
						Use();

					StopMoving();

					// building not getting built?
					/*if ( fHealth == pTaskEdict->v.health )
					{
						// move closer only if a bit far away
						if ( fDistance > fSize/1.5 )
							SetMoveVector(vBuilding);
					}*/
				}
				else
				{
					SetMoveVector(vBuilding);
				}

				// store previouse building health
				//m_CurrentTask->SetFloat(pTaskEdict->v.health);
			}
			else
				bDone = TRUE;

			//}
		}
		break;
		case BOT_TASK_USE_DOOR_BUTTON:
		{
			edict_t* pDoor = m_CurrentTask->TaskEdict();

			if (pDoor == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			edict_t* pButton = BotFunc_FindNearestButton(pev->origin, &pDoor->v);

			if (pButton != nullptr)
			{
				m_Tasks.FinishedCurrentTask();
				AddPriorityTask(CBotTask(BOT_TASK_USE, m_CurrentTask->GetScheduleId(), pButton));
				AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, m_CurrentTask->GetScheduleId(), pButton));
			}
			else
			{
				bTaskFailed = TRUE;
				break;
			}
		}
		break;
		case BOT_TASK_USE:   // USE object
		{
			int iState = m_CurrentTask->TaskInt();

			if (iState != -2 && iState != -1 && !m_CurrentTask->HasPath())
			{
				bTaskFailed = TRUE;
				break;
			}

			edict_t* pToUse = m_CurrentTask->TaskEdict();

			if (pToUse == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			if (gBotGlobals.IsMod(MOD_DMC) && pToUse->v.health > 0)
			{
				// shoot it to open it

				m_Tasks.FinishedCurrentTask();
				UpdateCondition(BOT_CONDITION_SEE_ENEMY);
				AddPriorityTask(CBotTask(BOT_TASK_NORMAL_ATTACK, 0, pToUse, 0, gpGlobals->time + 2.0));
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;
			StopMoving();

			if (iState == 1)
			{
				float fUseTime = m_CurrentTask->TaskVector().x;

				if (fUseTime == 0)
				{
					fUseTime = gpGlobals->time + m_CurrentTask->TaskFloat();

					m_CurrentTask->SetVector(Vector(fUseTime, 0, 0));
				}

				Use();

				if (fUseTime > gpGlobals->time)
				{
					// if its moving (momentary) keep holding it.
					if (!strncmp(STRING(pToUse->v.classname), "momentary", 9))// !BotFunc_EntityIsMoving(&pToUse->v) )
					{
						bDone = TRUE;

						if (*STRING(pToUse->v.target))
						{
							edict_t* pDoor = nullptr;

							//if it targets a moving momentary door, keep holding it
							while ((pDoor = UTIL_FindEntityByTargetname(pDoor, STRING(pToUse->v.target))) != nullptr)
							{
								if (BotFunc_EntityIsMoving(&pDoor->v))
								{
									bDone = FALSE;
									break;
								}
							}
						}
					}
				}
				else
					bDone = fUseTime <= gpGlobals->time;
			}
			else
			{
				BOOL bUse = FALSE;

				if (pToUse)
				{/*
				 if ( !HasCondition(BOT_CONDITION_SEE_TASK_EDICT) )
				 {
				 TaskToAdd =
				 }
					*/
					Vector vEntityOrigin = EntityOrigin(pToUse);

					float fDistance2d = (vEntityOrigin - pev->origin).Length2D();

					if (fDistance2d < 24)
					{
						float fzComp = fabs(vEntityOrigin.z - pev->origin.z);

						if (fzComp > MAX_JUMP_HEIGHT)
						{
							bTaskFailed = TRUE;
							break;
						}
					}

					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

					if (iState == -1 || DistanceFrom(vEntityOrigin) < 72)
					{
						if (UTIL_IsFacingEntity(pev, &pToUse->v))
						{
							if (m_CurrentTask->TaskInt() == -1)
							{
								if (RANDOM_LONG(0, 1))
								{
									Use();
									bDone = TRUE;
								}
								break;
							}
							bUse = TRUE;
						}
					}
					else
						SetMoveVector(vEntityOrigin);
				}
				else
					bUse = TRUE;

				if (bUse)
				{
					m_CurrentTask->SetInt(1); // Set int to state 1, using
					Use();
				}
			}
		}
		break;
		case BOT_TASK_USE_HEV_CHARGER:
			// Use an hev charger, hold in the use key if close enough
			// if not move closer to it, if it doesnt have a path set
			// to it then fail the task.
		{
			if (!m_CurrentTask->HasPath())
			{
				bTaskFailed = TRUE;
				break;
			}

			edict_t* pCharger = m_CurrentTask->TaskEdict();

			if (pCharger == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

			// It is done if the HEV charger has no energy left (Found from it's texture frame)
			// or the bot has max armor.
			bDone = pev->armorvalue >= VALVE_MAX_NORMAL_BATTERY || pCharger->v.frame != 0;

			if (bDone == FALSE)
			{
				Vector vOrigin = EntityOrigin(pCharger);

				if (DistanceFrom(vOrigin) > 72)
				{
					SetMoveVector(vOrigin);
				}
				else
				{
					if (RANDOM_LONG(0, 100))
						Use();

					StopMoving();
				}
			}
		}
		break;
		case BOT_TASK_USE_HEALTH_CHARGER:
		{
			if (!m_CurrentTask->HasPath())
			{
				bTaskFailed = TRUE;
				break;
			}

			edict_t* pCharger = m_CurrentTask->TaskEdict();

			if (pCharger == nullptr)
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

			bDone = pev->health >= pev->max_health || pCharger->v.frame != 0;

			if (bDone == FALSE)
			{
				Vector vOrigin = EntityOrigin(pCharger);

				if (RANDOM_LONG(0, 100))
					Use();

				if (DistanceFrom(vOrigin) > 72)
				{
					SetMoveVector(vOrigin);
				}
				else
				{
					if (RANDOM_LONG(0, 100))
						Use();

					StopMoving();
				}
			}
		}
		break;
		case BOT_TASK_DEFEND: // DEFEND object

			if (m_CurrentTask->TaskFloat() == 0)
				m_CurrentTask->SetFloat(gpGlobals->time + RANDOM_FLOAT(20.0, 80.0));

			if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				bDone = TRUE;

			if (gBotGlobals.IsMod(MOD_TS))
			{
				if (m_iTS_State != TS_State_Stunt)
					AltButton();
				else
					StopMoving();
			}
			else
				StopMoving();

			break;
		case BOT_TASK_USE_TANK: // like wait except dont look away from tank
		{
			edict_t* pTank = m_CurrentTask->TaskEdict();

			if (pTank != nullptr)
			{
				float fDist;

				fDist = DistanceFromEdict(pTank);

				if (!CanUseTank(pTank))
				{
					bTaskFailed = TRUE;
					break;
				}

				if (!IsUsingTank())
				{
					UpdateCondition(BOT_CONDITION_CANT_SHOOT);

					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_EDICT;

					if (pev->origin.x < pTank->v.absmin.x || pev->origin.x > pTank->v.absmax.x ||
						(pev->origin.y < pTank->v.absmin.y || pev->origin.y > pTank->v.absmax.y) ||
						(pev->origin.z < pTank->v.absmin.z || pev->origin.z > pTank->v.absmax.z))
					{
						SetMoveVector(EntityOrigin(pTank));
					}
					else
					{
						if (UTIL_IsFacingEntity(pev, &pTank->v))
						{
							if (RANDOM_LONG(0, 1))
							{
								Use();
								m_pTank = pTank;
							}
						}
					}
				}
				else
				{
					m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;

					if (m_CurrentTask->TaskInt() == 0)
					{
						pTank = nullptr;

						while ((pTank = UTIL_FindEntityInSphere(pTank, pev->origin, 72)) != nullptr)
						{
							if (strcmp("func_tank", STRING(pTank->v.classname)) == 0 ||
								strcmp("func_tankmortar", STRING(pTank->v.classname)) == 0 ||
								strcmp("func_tankrocket", STRING(pTank->v.classname)) == 0)
							{
								break;
							}
						}

						m_fNextUseTank = m_CurrentTask->TaskFloat() + RANDOM_FLOAT(5.0, 20.0);

						if (pTank == nullptr)
						{
							bTaskFailed = TRUE;
							break;
						}

						m_CurrentTask->SetVector(pev->origin + (AbsOrigin(pTank) - pev->origin).Normalize() * 256);
						m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
						m_CurrentTask->SetInt(1);

						// dont use tank again for a while
					}

					StopMoving();

					if (m_CurrentTask->TaskFloat() < gpGlobals->time)
					{
						bDone = TRUE;
						break;
					}
				}
			}
			else
			{
				bTaskFailed = TRUE;
				break;
			}
		}
		break;
		case BOT_TASK_WAIT:
			// stop moving completly...
			// for certain time made when making task

			if (m_CurrentTask->TaskInt() == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_CurrentTask->SetInt(1);
			}

			if (m_CurrentTask->TaskFloat() < gpGlobals->time)
			{
				bDone = TRUE;
				break;
			}
			else
				StopMoving();

			break;
		case BOT_TASK_WEB:
		{
			if (!hasWeb() || m_iCurrentWaypointIndex == -1 || m_iPrevWaypointIndex == -1 || m_iCurrentWaypointIndex == m_iPrevWaypointIndex)
			{
				if (m_iLastFailedTask != BOT_TASK_WEB && hasWeb())
				{
					m_iCurrentWaypointIndex = WaypointLocations.NearestWaypoint(pev->origin, REACHABLE_RANGE, m_iPrevWaypointIndex, TRUE, FALSE, TRUE);
					m_iLastFailedTask = BOT_TASK_WEB;
					break;
				}

				bTaskFailed = TRUE;
				break;
			}

			if (m_pCurrentWeapon == nullptr || m_pCurrentWeapon->GetID() != NS_WEAPON_WEBSPINNER)
			{
				if (m_iLastFailedTask == BOT_TASK_CHANGE_WEAPON)
				{
					bTaskFailed = TRUE;
					break;
				}

				AddPriorityTask(CBotTask(BOT_TASK_CHANGE_WEAPON, 0, nullptr, NS_WEAPON_WEBSPINNER));

				break;
			}

			StopMoving();

			switch (m_CurrentTask->TaskInt())
			{
			case 0:
			{
				Vector wallLeft = CrossProduct((WaypointOrigin(m_iCurrentWaypointIndex) - WaypointOrigin(m_iPrevWaypointIndex)).Normalize(), Vector(0, 0, 1));

				m_CurrentTask->SetVector(pev->origin + wallLeft * 512.0);
				m_CurrentTask->SetFloat(gpGlobals->time + 1.5);
				m_CurrentTask->SetInt(1);
			}
			break;
			case 1:
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					Vector wallRight = CrossProduct((WaypointOrigin(m_iCurrentWaypointIndex) - WaypointOrigin(m_iPrevWaypointIndex)).Normalize(), Vector(0, 0, 1));

					PrimaryAttack();

					m_CurrentTask->SetVector(pev->origin - wallRight * 512.0);
					m_CurrentTask->SetFloat(gpGlobals->time + 1.5);
					m_CurrentTask->SetInt(2);
				}
				break;
			case 2:
				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					PrimaryAttack();

					bDone = TRUE;
				}
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_FACE_TASK_VECTOR;
		}
		break;
		case BOT_TASK_DEPLOY_MINES:
			if (m_pEnemy != nullptr)
			{
				bDone = TRUE;
				break;
			}

			if (gBotGlobals.IsNS())
			{
				CBotWeapon* pMines = m_Weapons.GetWeapon(NS_WEAPON_MINE);

				if (m_CurrentTask->TaskFloat() == 0)
					m_CurrentTask->SetFloat(gpGlobals->time + 0.5);

				if (!HasWeapon(NS_WEAPON_MINE))
				{
					bDone = TRUE;
					break;
				}

				if (pMines->OutOfAmmo())
				{
					bDone = TRUE;
					break;
				}

				if (!pMines->HasWeapon(m_pEdict))
				{
					bDone = TRUE;
					break;
				}

				if (m_pCurrentWeapon == nullptr || m_pCurrentWeapon != pMines)
				{
					if (m_iLastFailedTask == BOT_TASK_CHANGE_WEAPON)
					{
						bTaskFailed = TRUE;
						break;
					}

					AddPriorityTask(CBotTask(BOT_TASK_CHANGE_WEAPON, 0, nullptr, NS_WEAPON_MINE));

					break;
				}

				m_CurrentLookTask = BOT_LOOK_TASK_FACE_GROUND;

				if (m_CurrentTask->TaskFloat() < gpGlobals->time)
				{
					PrimaryAttack();
					m_CurrentTask->SetFloat(gpGlobals->time + 0.75);

					if (m_CurrentTask->TaskInt() > 3) // 3 attempts
						bDone = TRUE;

					m_CurrentTask->SetInt(m_CurrentTask->TaskInt() + 1);
				}
				else if (m_CurrentTask->TaskFloat() + 10.0 < gpGlobals->time)
				{
					bTaskFailed = TRUE;
					break;
				}
			}
			else
				bTaskFailed = TRUE;
			break;
		case BOT_TASK_SEARCH_FOR_ENEMY:
			// Stop for some time and look around (use search for enemy look task)
			// if see an enemy then complete.
			if (m_pEnemy || HasCondition(BOT_CONDITION_SEE_ENEMY))
			{
				bTaskFailed = TRUE;
				break;
			}

			m_CurrentLookTask = BOT_LOOK_TASK_SEARCH_FOR_ENEMY;

			StopMoving();

			if (m_CurrentTask->TaskInt() == 0)
			{
				m_CurrentTask->SetFloat(gpGlobals->time + m_CurrentTask->TaskFloat());
				m_fSearchEnemyTime = m_CurrentTask->TaskFloat();
				m_CurrentTask->SetInt(1);
			}
			else
			{
				if (m_CurrentTask->TaskFloat() <= gpGlobals->time)
					bDone = TRUE;
			}

			break;
		default:
			break;
		}

		m_CurrentTask = m_Tasks.CurrentTask();

		if (m_CurrentTask) // reliablity check
		{
			// Failed task?
			if (bTaskFailed)
			{
				BotEvent(BOT_EVENT_FAIL_TASK);

				if (m_CurrentTask->Task() == BOT_TASK_FIND_PATH)
				{
					m_iLastFailedWaypoint = -1;
				}

				m_iLastFailedTask = m_CurrentTask->Task();
				m_iLastFailedSchedule = m_CurrentTask->GetScheduleDesc();
				m_fFailedTaskTime = gpGlobals->time;

				bDone = TRUE;
			}

			if (m_CurrentTask->Task() == BOT_TASK_FIND_PATH)
			{
				if (!m_Tasks.NoTasksLeft())
				{
					if (m_CurrentTask->HasPath() == FALSE)
					{
						if (bPathFound)
						{
							m_bMoveToIsValid = TRUE;
						}

						m_CurrentTask->SetPathInfo(bPathFound);
					}
				}
			}

			// Finished Task OK or FAILED?
			if (m_CurrentTask && bDone)
			{
				CBotTask LastTask = *m_CurrentTask;
				BotEvent(BOT_EVENT_COMPLETE_TASK);

				m_iLastFailedTask = BOT_TASK_NONE;
				m_iLastBotTask = m_CurrentTask->Task();
				m_Tasks.FinishedCurrentTask(bTaskFailed); // done

				if (LastTask.Task() == BOT_TASK_NORMAL_ATTACK)
					m_pEnemy = nullptr;
			}
		}

		if (TaskToAdd.Task() != BOT_TASK_NONE)
			AddPriorityTask(TaskToAdd);

		// end TASK CODE
	}
}

BOOL CBot::PrimaryAttack(void) const
{
	/*if ( HasCondition(BOT_CONDITION_CANT_SHOOT) )
	{
	return FALSE;
	}*/
	// Can't shoot while reloading
	if (pev->button & IN_RELOAD)
		return FALSE;

	if (m_pCurrentWeapon && gBotGlobals.IsNS())
	{
		switch (m_pCurrentWeapon->GetID())
		{
			// clamitius (whichbot)
		case NS_ABILITY_LEAP:
			Impulse(ALIEN_ABILITY_LEAP);
			break;
		case NS_WEAPON_BLINK:
			Impulse(ALIEN_ABILITY_BLINK);
			break;
		case NS_ABILITY_CHARGE:
			Impulse(ALIEN_ABILITY_CHARGE);
			break;
		default:
			break;
		}
	}

	pev->button |= IN_ATTACK;
	return TRUE;
}

void CBot::CheckStuck(void)
{
	const eClimbType iCurrentClimbState = GetClimbType();

	BOOL bCheckIfStuck = FALSE;

	///////////////
	// Check Stuck

	// Stuck speed not invalid
	if (gBotGlobals.m_fBotStuckSpeed != -1.0)
	{
		// check current task incase we dont want to check if stuck
		// i.e. dont want to kill ourselves if gestating in NS like what used to happen O_o
		m_CurrentTask = m_Tasks.CurrentTask();

		bCheckIfStuck = m_fIdealMoveSpeed > 0 && m_bMoveToIsValid && pev->waterlevel < 3 && (!m_CurrentTask || (m_CurrentTask->Task() == BOT_TASK_FIND_PATH || m_CurrentTask->Task() == BOT_TASK_MOVE_TO_VECTOR));

		if (bCheckIfStuck)
		{
			switch (gBotGlobals.m_iCurrentMod)
			{
			case MOD_NS:
				if (IsAlien())
				{
					bCheckIfStuck = !IsGestating();
				}
				else if (IsMarine())
				{
					// if bot does not have a task, he could be just waiting for orders
					if (!m_CurrentTask) // if commander is not null then he wont check if stuck
						bCheckIfStuck = EntityIsCommander(nullptr);
					if (bCheckIfStuck) // digested and cant move
						bCheckIfStuck = (pev->flags & MASK_DIGESTING) != MASK_DIGESTING;
				}
				break;
			default:
				break;
			}
		}
	}

	// Check if bot is REALLY stuck!
	if (bCheckIfStuck)//m_fIdealMoveSpeed > 1.0 )
	{
		// stuck inside something...
		if (pev->pContainingEntity != m_pEdict)
			m_bKill = TRUE;
		// Last non move time not set
		if (m_fLastNoMove == 0.0)
		{
			if (m_f3dVelocity <= gBotGlobals.m_fBotStuckSpeed)
				m_fLastNoMove = gpGlobals->time;
		}
		else
		{
			// bot already thinks its stuck
			if (m_f3dVelocity > gBotGlobals.m_fBotStuckSpeed)
				m_fLastNoMove = 0.0;
			else if (m_fLastNoMove + 8.0 < gpGlobals->time)
				// Stuck , non moving for 8 seconds
			{
				// Kill self
				m_fLastNoMove = 0.0;
				m_bKill = TRUE;
			}
		}
	}
	else
		m_fLastNoMove = 0.0;

	//if ( m_fLastNoMove == 0.0 )
	//{
	/////////////////////////
	// STUCK CODE          //
	/////////////////////////
	if (m_fStuckTime == 0)
	{
		// Check if Stuck

		if (pev->waterlevel != 3)
			// not in water
		{
			if (iCurrentClimbState != BOT_CLIMB_NONE)
				// climbing or flying
			{
				// get direction
				const int iLadderDir = GetLadderDir();

				// DONT jump off
				pev->button &= ~IN_JUMP;

				if (iLadderDir == 1)
					// moving up
				{
					// If not moving very fast up the way we are stuck
					if (pev->velocity.z < 10.0)
						m_fStuckTime = gpGlobals->time;
				}
				else if (iLadderDir == -1)
					// going down
				{
					// If not moving very fast down the way we are stuck
					if (pev->velocity.z > -10.0)
						m_fStuckTime = gpGlobals->time;
				}
			}
			else if (pev->flags & FL_DUCKING)
			{
				// if crouching, we move slower so stuck speed is real slow
				if (m_f2dVelocity < m_fMoveSpeed / 4)
					m_fStuckTime = gpGlobals->time;
			}
			// only stuck if we move less than half speed
			else if (m_f2dVelocity < m_fMoveSpeed / 2)
				m_fStuckTime = gpGlobals->time;
		}
		else
		{
			// swimming

			// moving slow (3d velocity since we are swimming)
			if (m_f3dVelocity < 32)
			{
				m_fStuckTime = gpGlobals->time;
			}
		}
	}
	else if (m_fStuckTime + 2.0 < gpGlobals->time)
	{
		if (pev->waterlevel == 3)
		{
			TraceResult tr;

			m_fUpTime = gpGlobals->time + RANDOM_FLOAT(2.0, 3.0);

			UTIL_TraceLine(GetGunPosition(), GetGunPosition() + Vector(0, 0, 64.0), ignore_monsters, dont_ignore_glass, m_pEdict, &tr);

			if (tr.flFraction < 1.0)
				m_fUpSpeed = -m_fMaxSpeed;
			else
				m_fUpSpeed = m_fMaxSpeed;
		}
		else
		{
			/// Bot is STUCK.

			// Climbing.. reset view angle
			if (iCurrentClimbState != BOT_CLIMB_NONE)
			{
				UnSetLadderAngleAndMovement();
			}
			else // jump (maybe hit an obstacle)
				JumpAndDuck();

			if (IsSkulk())
				pev->button |= IN_FORWARD; // try walking up a wall if possible.

			m_iLastFailedWaypoint = m_iCurrentWaypointIndex;

			// got stuck trying to go from one waypoint to another
			// find the path we tried to travel and mark it as failed so
			// we don't go there again next time.
			if (m_iCurrentWaypointIndex != -1 && m_iPrevWaypointIndex != -1)
			{
				BOOL m_bFailPath = TRUE;
				// Has a previous waypoint
				// we can work out the path the bot tried to use.

				if (gBotGlobals.IsNS() && (IsMarine() && m_iOrderType != ORDERTYPE_UNDEFINED))
					m_bFailPath = FALSE;

				if (m_bFailPath)
				{
					// found the path
					PATH* pPath = BotNavigate_FindPathFromTo(m_iPrevWaypointIndex, m_iCurrentWaypointIndex, m_iTeam);

					// add it to failed paths
					if (pPath)
						m_stFailedPaths.AddFailedPath(pPath);
				}
			}

			// remove path info (don't know where we are going now...)
			m_CurrentTask = m_Tasks.CurrentTask();

			if (m_CurrentTask)
				m_CurrentTask->SetPathInfo(FALSE);

			// NOT Strafing?
			if (m_fStrafeTime < gpGlobals->time)
			{
				// wall on the left of me so we'll strafe right for approx 2 seconds
				if (BotCheckWallOnLeft(this))
				{
					m_fStrafeTime = gpGlobals->time + RANDOM_FLOAT(1.5, 2.5);
					m_fStrafeSpeed = m_fMaxSpeed; // going right (positive speed)
				}
				// wall on the right so we'll strafe left for approx 2 seconds
				else if (BotCheckWallOnRight(this))
				{
					m_fStrafeTime = gpGlobals->time + RANDOM_FLOAT(1.5, 2.5);
					m_fStrafeSpeed = -m_fMaxSpeed; // going left (negative speed)
				}
			}
		}

		// reset stuck time for another check
		m_fStuckTime = 0;
	}
	else if (m_fStuckTime + 1.0 < gpGlobals->time)
	{
		if (iCurrentClimbState == BOT_CLIMB_NONE)
			JumpAndDuck();

		gotStuck();
	}
	else
	{
		if (pev->waterlevel == 3)
			// swimming
		{
			// we're going faster than stuck speed, reset stuck time
			if (m_f3dVelocity >= 32)
			{
				m_fStuckTime = 0;
			}
		}
		else if (iCurrentClimbState != BOT_CLIMB_NONE)
			// climbing or flying
		{
			const int iLadderDir = GetLadderDir();

			if (iLadderDir == 1)
				// going up
			{
				// we're going faster than stuck speed, reset stuck time.
				if (pev->velocity.z >= 10.0)
					m_fStuckTime = 0;
			}
			else if (iLadderDir == -1)
				// going down
			{
				// we're going faster than stuck speed, reset stuck time..
				if (pev->velocity.z <= -10.0)
					m_fStuckTime = 0;
			}
		}
		else if (pev->flags & FL_DUCKING)
		{
			// we're going faster than stuck speed, reset stuck time...
			if (m_f2dVelocity >= m_fMoveSpeed / 4)
				m_fStuckTime = 0;
		}
		else if (m_f2dVelocity >= m_fMoveSpeed / 2)
			m_fStuckTime = 0;
	}
	//}

	// END STUCK CODE
	/////////////////////////
}

void CBotReputation::printRep(CBot* forBot, edict_t* pPrintTo) const
{
	CClient* pClient = gBotGlobals.m_Clients.GetClientByRepId(m_iPlayerRepId);
	const char* szName = STRING(pClient->GetPlayer()->v.netname);
	char* szBotName = forBot->m_szBotName;

	switch (m_iRep)
	{
	case 0:
		BotMessage(pPrintTo, 0, "%s really hates %s (%d)", szBotName, szName, m_iRep);
		break;
	case 1:
		BotMessage(pPrintTo, 0, "%s hates %s (%d)", szBotName, szName, m_iRep);
		break;
	case 2:
		BotMessage(pPrintTo, 0, "%s kind of hates %s (%d)", szBotName, szName, m_iRep);
		break;
	case 3:
		BotMessage(pPrintTo, 0, "%s doesn't like %s (%d)", szBotName, szName, m_iRep);
		break;
	case 4:
	case 5:
	case 6:
		BotMessage(pPrintTo, 0, "%s likes %s (%d)", szBotName, szName, m_iRep);
		break;
	case 7:
		BotMessage(pPrintTo, 0, "%s really likes %s (%d)", szBotName, szName, m_iRep);
		break;
	case 8:
		BotMessage(pPrintTo, 0, "%s loves %s (%d)", szBotName, szName, m_iRep);
		break;
	case 9:
		BotMessage(pPrintTo, 0, "%s really loves %s (%d)", szBotName, szName, m_iRep);
		break;
	case 10:
		BotMessage(pPrintTo, 0, "%s LURRVESS %s !!! (%d)", szBotName, szName, m_iRep);
		break;
	}
}

void CBot::UseTank(edict_t* pTank)
{
	const int iSchedId = m_Tasks.GetNewScheduleId();

	m_pTank = pTank;

	AddPriorityTask(CBotTask(BOT_TASK_USE_TANK, iSchedId, pTank, 0, RANDOM_FLOAT(12.0, 22.0)));

	//	CBotTask Tasks[] =
	//	{
	//		CBotTask(BOT_TASK_USE,0,pTank,-2),
	//	};

	m_Tasks.GiveSchedIdDescription(iSchedId, BOT_SCHED_USE_TANK);
}

BOOL CBot::IsUsingTank(void) const
{
	if (m_pTank)
	{
		if (strcmp(STRING(m_pTank->v.netname), STRING(m_pEdict->v.netname)) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CBot::workEnemyCosts(edict_t* pEntity, Vector vOrigin, const float fDistance)
{
	if (m_fNextWorkRangeCosts > gpGlobals->time)
		return;

	if (fDistance > 768)
		return;

	Vector lowest;
	const int mid = BOT_COST_BUCKETS / 2;
	int enemyState = 0;

	if (isInAnimate(pEntity))
		enemyState = 2;
	else if (IsEnemy(pEntity))
		enemyState = 1;
	else
	{
		char* szClassname = const_cast<char*>(STRING(pEntity->v.classname));
		// grenade
		if (pEntity->v.deadflag != DEAD_NO && strcmp(szClassname, "monster_robogrunt") == 0 ||
			UTIL_IsGrenadeRocket(pEntity))
			enemyState = 3;
		else if (EntityIsAlive(pEntity))
			enemyState = 0;
		else
			enemyState = 2; // inanimate
	}

	for (int x = 0; x < BOT_COST_BUCKETS; x++)
	{
		const int posx = x - mid;

		for (int y = 0; y < BOT_COST_BUCKETS; y++)
		{
			const int posy = y - mid;

			Vector pos = Vector(posx * BOT_COST_RANGE, posy * BOT_COST_RANGE, pev->origin.z);

			float fCost = fRangeCosts[x][y];

			fCost += m_GASurvival->get(enemyState) / (fDistance * fDistance) * (pEntity->v.size.Length() / pev->size.Length());

			fRangeCosts[x][y] = fCost;

			if (fCost < m_fLowestEnemyCost)
			{
				m_fLowestEnemyCost = fCost;
				m_vLowestEnemyCostVec = pev->origin + pos;
			}
		}
	}
}

void CBot::decideJumpDuckStrafe(const float fEnemyDist, const Vector vEnemyOrigin)
{
	std::vector<ga_value> inputs;

	dec_jump->setWeights(m_pPersonalGAVals, 0, 5);
	dec_duck->setWeights(m_pPersonalGAVals, 5, 5);
	dec_strafe->setWeights(m_pPersonalGAVals, 10, 5);

	inputs.push_back(pev->velocity.Length() / pev->size.Length());
	inputs.push_back(fEnemyDist * 0.01);
	inputs.push_back((vEnemyOrigin.z - pev->origin.z) * 0.1);
	//inputs.push_back(pev->size.Length());
	if (m_pEnemy)
		inputs.push_back(pev->size.Length() / m_pEnemy->v.size.Length());
	else
		inputs.push_back(1);

	inputs.push_back(pev->flags & FL_ONGROUND && pev->movetype == MOVETYPE_WALK);

	dec_jump->input(&inputs);
	dec_duck->input(&inputs);
	dec_strafe->input(&inputs);

	dec_jump->execute();
	dec_duck->execute();
	dec_strafe->execute();

	if (dec_jump->fired())
		Jump();
	if (dec_duck->fired())
		JumpAndDuck(); // try this
//		Duck();
	if (m_fStrafeTime + 0.75 < gpGlobals->time)
	{
		const float fout = dec_strafe->getOutput();

		if (fout >= 0.75 || fout <= 0.25)
			m_fStrafeSpeed = fout * m_fMaxSpeed;
	}

	inputs.clear();

	m_bUsedMelee = TRUE;
}

BOOL CBot::hasWeb() const
{
	return m_Weapons.HasWeapon(m_pEdict, NS_WEAPON_WEBSPINNER);
}

BOOL CBot::isInAnimate(edict_t* pEntity)
{
	return (pEntity->v.flags & (FL_WORLDBRUSH | FL_DORMANT | FL_KILLME)) > 0;
}

void CBot::clearEnemyCosts()
{
	memset(fRangeCosts, 0, sizeof(float) * (BOT_COST_BUCKETS * BOT_COST_BUCKETS));
	m_fLowestEnemyCost = 99999;
	m_vLowestEnemyCostVec = pev->origin;
}