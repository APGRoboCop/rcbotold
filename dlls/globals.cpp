// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
 // globals.cpp
 //
 //////////////////////////////////////////////////
 //
 // Global class holding info required throughout program
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot_const.h"
#include "bot.h"
#include "waypoint.h"
#include "bot_weapons.h"

#include "bot_commands.h"
#include "bot_client.h"
#include "bot_menu.h"

#include "dir.h"

#ifdef _WIN32
#define strcmpi _strcmpi
#endif

extern CWaypointVisibilityTable WaypointVisibility;
extern CWaypointLocations WaypointLocations;

extern int debug_engine;

extern FILE* fpMapConfig;
static FILE* fp;
/*
void ReadRCBotFolder ( void )
{
	FILE *fp = fopen("rcbot.ini","r");

	char rcbot_folder[256];

	if ( fp )
	{
		while ( !feof(fp) && !(fscanf(fp,"rcbot_folder=\"%s\"\n",rcbot_folder) == 1) )
		{
			;
		}
	}
}*/

void CBotGlobals::ReadBotFolder(void)
{
	char filename[256];

	BotMessage(nullptr, 0, "Reading Bot Folder");

	sprintf(filename, "%s/rcbot_folder.ini", m_szModFolder);

	FILE* fp = fopen(filename, "r");

	char rcbot_folder[256];

	//default

	if (fp)
	{
		if (fscanf(fp, "%s\n", rcbot_folder) == 1)
		{
			strncpy(m_szBotFolder, rcbot_folder, 255);
			m_szBotFolder[255] = 0;
		}

		BotMessage(nullptr, 0, "Found Bot Folder file : %s", m_szBotFolder);
		fclose(fp);
	}
	else
	{
		strcpy(m_szBotFolder, "rcbot");
		BotMessage(nullptr, 0, "Bot Folder File not found! using default (half-life/rcbot)");
	}
}

BOOL CBotGlobals::NetMessageStarted(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	if (m_bNetMessageStarted == TRUE)
	{
		// message already started... engine will crash
		assert(!m_bNetMessageStarted);
		//ALERT(at_console,"[RCBOT] NET MESSAGE %d ALREADY STARTED, IGNORING (info lost)\n",msg_type);
		return FALSE;
		//	MESSAGE_END();
	}

	m_CurrentMessage = nullptr;

	m_bNetMessageStarted = TRUE;

	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp = fopen("bot.txt", "a");
			fprintf(fp, "pfnMessageBegin: edict=%p dest=%d type=%d\n", ed, msg_dest, msg_type);
			fclose(fp); }

		m_CurrentMessage = nullptr;
		m_iCurrentMessageState = 0;
		m_iCurrentMessageState2 = 0;
		m_iBotMsgIndex = -1;

		if (ed)
		{
			int index = UTIL_GetBotIndex(ed);

			// get the message to see if we can do anything right now
			m_CurrentMessage = m_NetEntityMessages.GetMessage(msg_type, nullptr);

			if (m_CurrentMessage && /*m_CurrentMessage->IsMessageName("WeaponList") &&*/ IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
			{
				m_pDebugMessage = m_CurrentMessage;

				ALERT(at_console, "------ MESSAGE_BEGIN(\"%s\") : \"%s\" -------\n", m_CurrentMessage->getMessageName(),
				      STRING(ed->v.netname));
			}

			if (IsNS())
			{
				if (m_CurrentMessage)
				{
					// is an alien info message
					if (m_CurrentMessage->IsMessageName("AlienInfo"))
					{
						// can auto build, alien info can happen when game starts
						m_fAutoBuildTime = gpGlobals->time + 0.1;
					}
				}
			}
			/*else if ( IsMod(MOD_TS) )
			{
				if ( m_CurrentMessage )
				{
					// !!!
					if ( m_CurrentMessage->IsMessageName("ClipInfo") )
					{
						BotMessage(NULL,0,"break here");
						index = 0;
					}
					else if ( m_CurrentMessage->IsMessageName("WeaponInfo") )
					{
						BotMessage(NULL,0,"break here");
						index = 0;
					}
					else if ( m_CurrentMessage->IsMessageName("WeaponList") )
					{
						BotMessage(NULL,0,"break here");
						index = 0;
					}
				}
			}*/

			if (m_CurrentMessage != nullptr)
			{
				m_pMessageEntity = ed;

				if (m_CurrentMessage->isStateMsg())
					static_cast<CBotStatedNetMessage*>(m_CurrentMessage)->init(index);
				// is this message for a bot?
				if (m_CurrentMessage->humansAllowed() || index != -1)
				{
					// keep current message set so it wil be called whan messages are sent
					// and update current bot index being affected by messages.
					m_iBotMsgIndex = index;
				}
				else // dont call a function
				{
					if (!IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
						m_CurrentMessage = nullptr;
				}
			}
		}
		else if (msg_dest == MSG_ALL)
		{
			m_CurrentMessage = m_NetAllMessages.GetMessage(msg_type, nullptr);

			if (m_CurrentMessage && /*m_CurrentMessage->IsMessageName("WeaponList") &&*/ IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
			{
				m_pDebugMessage = m_CurrentMessage;

				ALERT(at_console, "------ MESSAGE_BEGIN(\"%s\") : (MSG_ALL message) -------\n", m_CurrentMessage->getMessageName());
			}
		}
	}

	return TRUE;
}

void CBotGlobals::StartFrame(void)
{
	static int iIndex = 0;
	static CBot* pBot = nullptr;
	static float fPreviousTime = -1.0;
	static int iNumClients = 0;
	static BOOL bUpdateClientData;
	static BOOL bCheckedTeamplay = FALSE;

	if (!bCheckedTeamplay)
	{
		m_bTeamPlay = CVAR_GET_FLOAT("mp_teamplay") > 0 || CVAR_GET_FLOAT("mp_theonemode") > 0;
		bCheckedTeamplay = TRUE;
	}

	// new map?
	if (gpGlobals->time + 0.1 < fPreviousTime)
	{
		bCheckedTeamplay = FALSE;
		//RoundInit();
	}
	else
	{
		///////////////////////////
		// ???
		// braindead check
		///
		if (m_iMaxPathRevs == 0)
			m_iMaxPathRevs = 150;
		///////////////////////////

		if (fpMapConfig != nullptr && m_fMapInitTime + 10.0 < gpGlobals->time && m_fReadConfigTime < gpGlobals->time)
			ReadMapConfig();

		bUpdateClientData = !IsMod(MOD_DMC) && !IsMod(MOD_TS) && m_fClientUpdateTime <= gpGlobals->time;

		if (bUpdateClientData)
		{
			m_fClientUpdateTime = gpGlobals->time + 1.0;
		}

		if (m_bBotCanRejoin == FALSE)
		{
			if (m_fMapInitTime + 10 < gpGlobals->time)
			{
				if (m_fBotRejoinTime < gpGlobals->time)
				{
					m_bBotCanRejoin = TRUE;
				}
			}
		}

		CClient* pClient;
		edict_t* pPlayer;

		// ------------- NS ------------------
		// Check out the hive info
		// check mainly for the available traits aliens can upgrade with.
		//
		// Also check out the auto build stuff
		if (m_iCurrentMod == MOD_NS)
		{
			int iNumHivesUp = 0;
			int iNumUpgrades = 0;
			//hive_info_t *pHiveInfo;

			m_bHasDefTech = FALSE;
			m_bHasMovTech = FALSE;
			m_bHasSensTech = FALSE;

			// update who is commander
			SetCommander(UTIL_GetCommander());

			if (!IsCombatMap() && IsConfigSettingOn(BOT_CONFIG_MARINE_AUTO_BUILD) && (!m_bAutoBuilt && (m_fAutoBuildTime && m_fAutoBuildTime < gpGlobals->time)))
			{
				edict_t* pEntity = nullptr;

				// Find the marine command console

				while ((pEntity = UTIL_FindEntityByClassname(pEntity, "team_command")) != nullptr)
				{
					break;
				}

				if (pEntity)
				{
					// Found a comm console
					Vector vOrigin = pEntity->v.origin;

					// find a nearby waypoint
					int iWpt = WaypointLocations.NearestWaypoint(vOrigin, REACHABLE_RANGE, -1, FALSE);

					if (iWpt == -1)
						BotMessage(nullptr, 0, "No waypoints for auto-build!!!");
					else
					{
						// find another waypoint but ignire the nearest one to comm console
						iWpt = WaypointLocations.NearestWaypoint(WaypointOrigin(iWpt), REACHABLE_RANGE, iWpt, FALSE);

						if (iWpt == -1)
							BotMessage(nullptr, 0, "No waypoints for auto-build!!!");
						else
						{
							TraceResult tr;

							vOrigin = WaypointOrigin(iWpt);

							// get right position for infantry portal
							UTIL_TraceLine(vOrigin, vOrigin - Vector(0, 0, 100), ignore_monsters, pEntity, &tr);

							vOrigin = tr.vecEndPos + Vector(0, 0, 8);

							//build

							BotFunc_NS_MarineBuild(AVH_USER3_INFANTRYPORTAL, "team_infportal", vOrigin);
						}
					}
				}
				else
					BotMessage(nullptr, 0, "No marine spawn found for auto-build!!!");

				m_bAutoBuilt = TRUE;
			}

			/*for ( iIndex = 0; iIndex < BOT_MAX_HIVES; iIndex ++ )
			{
				pHiveInfo = &m_Hives[iIndex];

				// mTechnology wont work :(
				//mTechnology = pHiveInfo->mTechnology;

				if ( IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL) )
				{
					if ( pHiveInfo->pHive && (pHiveInfo->pHive->v.fuser1 > 0) )
					{
						iNumHivesUp++;
					}
				}
				else
				{
					if ( pHiveInfo->pHive && (pHiveInfo->pHive->v.fuser2 > 0) )
					{
						iNumHivesUp++;
					}
				}
			}*/

			// see if we have technology by checking the upgrades, if hives = amount of upgrades
			// else then we have a choice of what to build so we can build anything until it is built

			if (m_bCanUpgradeDef)
				iNumUpgrades++;
			if (m_bCanUpgradeMov)
				iNumUpgrades++;
			if (m_bCanUpgradeSens)
				iNumUpgrades++;

			if (UTIL_GetNumHives() == iNumUpgrades)
			{
				m_bHasDefTech = m_bCanUpgradeDef;
				m_bHasMovTech = m_bCanUpgradeMov;
				m_bHasSensTech = m_bCanUpgradeSens;
			}
			else
			{
				m_bHasDefTech = TRUE;
				m_bHasMovTech = TRUE;
				m_bHasSensTech = TRUE;
			}

			if (IsCombatMap() && m_pMarineStart)
			{
				if (m_CommConsole.IsUnderAttack())
				{
					CBotTask m_NewSchedule[3] = { CBotTask(BOT_TASK_FIND_PATH,0, nullptr,-1,0,m_pMarineStart->v.origin),
						CBotTask(BOT_TASK_SEARCH_FOR_ENEMY),
						CBotTask(BOT_TASK_SENSE_ENEMY) };

					for (int i = 0; i < MAX_PLAYERS; i++)
					{
						pBot = &m_Bots[i];

						if (pBot->IsUsed())
						{
							if (!pBot->m_Tasks.HasSchedule(BOT_SCHED_NS_CHECK_STRUCTURE))
							{
								// get some alien bots to check out the hive
								if (pBot->IsMarine())
								{
									pBot->m_Tasks.FlushTasks();
									pBot->m_Tasks.AddNewSchedule(BOT_SCHED_NS_CHECK_STRUCTURE, m_NewSchedule, 3);
								}
							}
						}
					}
				}
				else
					m_CommConsole.Update();
			}
			int iBuildingPriority = 0;

			edict_t* pBuildingUnderAttack = m_HiveMind.Tick(&iBuildingPriority);

			if (pBuildingUnderAttack)
			{
				CBotTask m_NewSchedule[3] = { CBotTask(BOT_TASK_FIND_PATH,0, nullptr,-1,0,pBuildingUnderAttack->v.origin),
											 CBotTask(BOT_TASK_SEARCH_FOR_ENEMY),
											 CBotTask(BOT_TASK_SENSE_ENEMY) };

				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					pBot = &m_Bots[i];

					if (pBot->IsUsed())
					{
						const int iGotoChance = (int)((float)iBuildingPriority / 6 * 100);

						if (pBuildingUnderAttack->v.iuser3 == AVH_USER3_HIVE)
						{
							if (!pBot->m_Tasks.HasSchedule(BOT_SCHED_NS_CHECK_HIVE))
							{
								// get some alien bots to check out the hive
								if (pBot->IsAlien() && !pBot->IsGorge() && RANDOM_LONG(0, 100) < iGotoChance)
								{
									pBot->m_Tasks.FlushTasks();
									pBot->m_Tasks.AddNewSchedule(BOT_SCHED_NS_CHECK_HIVE, m_NewSchedule, 3);
								}
							}
						}
						else if (!pBot->m_Tasks.HasSchedule(BOT_SCHED_NS_CHECK_STRUCTURE))
						{
							// get some alien bots to check out the hive
							if (pBot->IsAlien() && !pBot->IsGorge() && RANDOM_LONG(0, 100) < iGotoChance)
							{
								pBot->m_Tasks.FlushTasks();
								pBot->m_Tasks.AddNewSchedule(BOT_SCHED_NS_CHECK_STRUCTURE, m_NewSchedule, 3);
							}
						}
					}
				}
			}
		}

		m_iNumBots = 0;

		/*for ( iIndex = 0; iIndex < MAX_PLAYERS; iIndex ++ )
		{
			// do some bot checking
			pBot = &m_Bots[iIndex];

			if ( pBot->IsUsed() )
			{
				m_iNumBots++;
			}
		}*/

		iNumClients = UTIL_GetNumClients() + GetNumJoiningClients();

		for (iIndex = 0; iIndex < MAX_PLAYERS; iIndex++)
		{
			// do some bot checking ( just do ONE loop... jeez)
			pBot = &m_Bots[iIndex];

			if (pBot->IsUsed())
			{
				m_iNumBots++;
			}

			// check for unused clients
			if ((pClient = m_Clients.GetClientByIndex(iIndex)) == nullptr)
				continue;

			if (!pClient->IsUsed())
				continue;

			if ((pPlayer = pClient->GetPlayer()) == nullptr)
				continue;

			if (!*STRING(pPlayer->v.netname))
				m_Clients.ClientDisconnected(pClient);
			else
			{
				pClient->Think();
			}
		}

		BOOL bBotJoin = FALSE;

		if (IsConfigSettingOn(BOT_CONFIG_BOTS_LEAVE_AND_JOIN))
		{
			const int iClientsInGame = iNumClients; // argh, can't debug static variables
			const float val = (float)iClientsInGame / gpGlobals->maxClients * RANDOM_FLOAT(0.9, 1.3);

			bBotJoin = val < 0.75;
		}

		const BOOL bServerFull = iNumClients >= gpGlobals->maxClients;

		for (iIndex = 0; iIndex < MAX_PLAYERS; iIndex++)
		{
			pBot = &m_Bots[iIndex];

			if (!pBot)
				continue;

			if (pBot->m_iRespawnState != RESPAWN_IDLE)
			{
				// ---------------------------------------
				//
				// See if a bot can re-join the game
				//
				// [] check if min or max bots are enough
				//
				// [] check if bots are still to join from
				//    the last game
				//
				// ---------------------------------------
				if (!bServerFull && m_bBotCanRejoin)
				{
					BOOL bAddBot = FALSE;

					// Bot was in last game so is re-connecting
					if (m_iMaxBots == -1 &&
						m_iMinBots == -1)
					{
						if (pBot->m_iRespawnState == RESPAWN_NEED_TO_REJOIN)
							bAddBot = TRUE;
						else
						{
							bAddBot = bBotJoin;
						}
					}

					if (m_iMaxBots != -1)
					{
						if (iNumClients < m_iMaxBots)
							bAddBot = TRUE;
					}

					// Number of clients already maximum so can't add a bot
					if (m_iMinBots != -1)
						// min bots is set
					{
						// number of bots is less than minimum, so add the bot
						if (m_iNumBots < m_iMinBots)
							bAddBot = TRUE;
					}

					if (bAddBot)
					{
						// Call addbot function with no parameters to add a bot
						BotFunc_AddBot(nullptr, nullptr, nullptr, nullptr, nullptr);

						m_bBotCanRejoin = FALSE;
						m_fBotRejoinTime = gpGlobals->time + 2.0;

						bAddBot = FALSE;
					}
				}

				continue;
			}

			if (pBot->IsUsed())
			{
				if (pBot->HasCondition(BOT_CONDITION_WANT_TO_LEAVE_GAME))
				{
					if (!pBot->m_fLeaveTime)
					{
						pBot->m_fLeaveTime = gpGlobals->time + RANDOM_FLOAT(3.0, 6.0);

						pBot->BotEvent(BOT_EVENT_LEAVING);
					}
					else if (pBot->m_fLeaveTime < gpGlobals->time)
					{
						// kick the bot from the game
						pBot->m_bKick = TRUE;
						pBot->m_fLeaveTime = 0;
						pBot->RemoveCondition(BOT_CONDITION_WANT_TO_LEAVE_GAME);
					}
				}

				// if bot has kick flag, kick the bot, and reset kick flag
				if (pBot->m_bKick)
				{
					char cmd[80];

					//int iUserId;

					//iUserId = (*g_engfuncs.pfnGetPlayerUserId)(pBot->m_pEdict);

					sprintf(cmd, "kick \"%s\"\n", pBot->m_szBotName);
					//sprintf(cmd,"kick #%d\n",iUserId);

					SERVER_COMMAND(cmd);

					pBot->m_bKick = FALSE;
				}
				// If the bot is alive and the kill flag is set
				// Kill the bot and put the kill flag off.
				else if (pBot->IsAlive() && pBot->m_bKill)
				{
#ifdef RCBOT_META_BUILD
					MDLL_ClientKill(pBot->m_pEdict);
#else
					ClientKill(pBot->m_pEdict);
#endif
					pBot->m_bKill = FALSE;
				}
				else
				{
					/*	if ( m_iJoiningClients > m_iMaxBots )
							pBot->m_bKick = TRUE;
						else */if (bUpdateClientData)
						{
							// Check to see if the bot has dropped any weapons
							// and update the weapons it is carrying

							// don't do this for aliens, they can't drop weapons and they
							// are used differently
						//	if ( !IsNS() || pBot->IsMarine() )
						//	{
								// see if a weapon was dropped...or added...
							if (pBot->m_iBotWeapons != pBot->pev->weapons)
							{
								int j;

								// weapons(s) were dropped, remove all weapons it has
								// and re-add them according to the new bit mask of weapons!

								if (IsNS() && !IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
								{
									//pBot->m_iBotWeapons = pBot->pev->weapons;

									//pBot->m_Weapons.RemoveWeapons();

									//pBot->m_iBotWeapons = 0;

									for (j = 1; j < MAX_WEAPONS; j++)
									{
										BOOL bHasWeapon = pBot->HasWeapon(j);

										if (pBot->pev->weapons & 1 << j && !bHasWeapon)
										{
											CBotWeapon* pWeapon = pBot->m_Weapons.GetWeapon(j);

											BOOL bWeaponStatus = TRUE;

											if (pWeapon)
												bWeaponStatus = pWeapon->HasWeapon(pBot->m_pEdict);

											pBot->m_Weapons.AddWeapon(j);

											// realiable check
											pWeapon = pBot->m_Weapons.GetWeapon(j);

											if (pWeapon)
												pWeapon->setHasWeapon(bWeaponStatus);

											pBot->m_iBotWeapons |= 1 << j;
										}
										else if (!(pBot->pev->weapons & 1 << j) && bHasWeapon)
										{
											CBotWeapon* pWeapon = pBot->m_Weapons.GetWeapon(j);

											if (pWeapon)
												pWeapon->setHasWeapon(FALSE);

											pBot->m_Weapons.RemoveWeapon(j);
											pBot->m_iBotWeapons &= ~(1 << j);
										}
									}
								}
								else if (IsMod(MOD_TS))
									pBot->m_Weapons.AddWeapon(36);
								else
								{
									pBot->m_iBotWeapons = pBot->pev->weapons;

									pBot->m_Weapons.RemoveWeapons();

									pBot->m_iBotWeapons = 0;

									for (j = 1; j < MAX_WEAPONS; j++)
									{
										if (pBot->pev->weapons & 1 << j)
										{
											pBot->m_Weapons.AddWeapon(j);
										}
									}

									pBot->m_iBotWeapons = pBot->pev->weapons;
								}
							}
						}

						//try
						//{
						// Make the bot run it's thinking procedure
						pBot->Think();
						//}

						//catch (...)
						//{
						//	BugMessage(NULL,"Crash in bot think() index %d",iIndex);
						//}
						// Need to run this function after the bot
						// thinks each frame so it can move.
						pBot->RunPlayerMove();
				}
			}
		}

		// Update squad stuff
		m_Squads.UpdateAngles();

		// Show waypoints to clients with Waypoint Mode On
		// WaypointThink();

		if (m_BotCam.IsWorking())
			m_BotCam.Think();
		else
			m_BotCam.Spawn();
	}

	iIndex = 0; // Reset for next StartFrame()

	fPreviousTime = gpGlobals->time;
}

void CBotGlobals::KeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd)
{
	// If there has been an entity needing a master add it to the
	// global masters list, for use with bots finding out if doors
	// that use this master are open or not (and find buttons etc to open it)
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		m_Masters.AddMaster(pentKeyvalue, pkvd->szValue);
	}
	/*
		if ( IsMod(MOD_TS) )
		{
			char *cname = (char*)STRING(pentKeyvalue->v.classname);
			// weapons

			if ( strstr(cname,"weapon") != NULL )
			{
				// test
				ALERT(at_console,"test");
			}
		}*/
}

void CBotGlobals::MapInit(void)
{
	int i;
	char szTemp[256];

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		m_Bots[i].FreeLocalMemory();
	}

	memset(&m_Squads, 0, sizeof(CBotSquads));
	memset(m_iJoiningClients, 0, sizeof(int) * MAX_PLAYERS);
	memset(m_iTeamScores, 0, sizeof(int) * MAX_TEAMS);
	m_pDebugMessage = nullptr;
	m_pTFCDetect = nullptr;
	m_pTFCGoal = nullptr;
	m_pMarineStart = nullptr;
	
	m_bAutoPathWaypoint = TRUE;
	m_bWaypointsHavePaths = FALSE;
	m_iBotMsgIndex = -1;
	m_iCurrentMessageState = 0;
	m_iCurrentMessageState2 = 0;
	m_bIsFakeClientCommand = FALSE;
	
	m_CurrentMessage = nullptr;
	m_fAutoBuildTime = 0;
	m_bAutoBuilt = FALSE;
	m_bNetMessageStarted = FALSE;
	m_CurrentHandledCvar = nullptr;
	m_fMapInitTime = 0;
	m_fBotRejoinTime = 0;
	m_pCommander.Set(nullptr);

	m_bHasDefTech = FALSE;
	m_bHasSensTech = FALSE;
	m_bHasMovTech = FALSE;

	m_bCanUpgradeDef = FALSE;
	m_bCanUpgradeSens = FALSE;
	m_bCanUpgradeMov = FALSE;

	memset(m_Hives, 0, sizeof(hive_info_t) * BOT_MAX_HIVES);
	//m_iNumHivesAdded = 0;

	m_fClientUpdateTime = 0;
	m_bBotCanRejoin = FALSE;

	m_fNextJoinTeam = 0;
	m_iNumBots = 0;

	m_bTeamPlay = FALSE;

	m_currCapPoint = nullptr;
	m_currFlag = nullptr;

	prevBackPackInvalid = FALSE;
	prevCapturePointInvalid = FALSE;
	prevFlagInvalid = FALSE;

	const char* mapname = static_cast<const char*>(STRING(gpGlobals->mapname));

	if (IsMod(MOD_TS))
	{
		if (strncmp(mapname, "tm_", 3) == 0)
		{
			setMapType(NON_TFC_TS_TEAMPLAY);
			m_bTeamPlay = TRUE;
		}
	}

	PRECACHE_MODEL("models/mechgibs.mdl");

	m_sModelIndexFireball = PRECACHE_MODEL("sprites/zerogxplode.spr");

	// so alines and marines know where default start is for marines on the map
	m_pMarineStart = nullptr;

	// key value stuff
	prevFlagInvalid = FALSE;
	prevBackPackInvalid = FALSE;
	prevCapturePointInvalid = FALSE;
	m_currCapPoint = nullptr;
	m_currFlag = nullptr;
	//m_pTFCGroup = NULL;
	m_pTFCDetect = nullptr;
	m_pTFCGoal = nullptr;
	m_currBackPack = nullptr;

	m_fAutoBuildTime = 0;
	m_bAutoBuilt = FALSE;

	memset(m_Hives, 0, sizeof(hive_info_t) * BOT_MAX_HIVES);
	//	m_iNumHivesAdded = 0;

	m_iWaypointTexture = PRECACHE_MODEL("sprites/lgtning.spr");
	m_bGameRules = TRUE;

	m_fMapInitTime = gpGlobals->time;
	m_fBotRejoinTime = gpGlobals->time;

	SetCommander(nullptr);

	m_bCanUpgradeDef = FALSE;
	m_bCanUpgradeSens = FALSE;
	m_bCanUpgradeMov = FALSE;

	m_bCombatMap = strncmp("co_", STRING(gpGlobals->mapname), 3) == 0;

#ifdef __linux__
	sprintf(szTemp, "map_configs/%s/%s.cfg", m_szModFolder, STRING(gpGlobals->mapname));
#else
	sprintf(szTemp, "map_configs\\%s\\%s.cfg", m_szModFolder, STRING(gpGlobals->mapname));
#endif
	char filename[512];

	UTIL_BuildFileName(filename, szTemp);

	fpMapConfig = fopen(filename, "r");

	if (fpMapConfig == nullptr)
		BotMessage(nullptr, 0, "No map specific config file found (%s)", filename);
	else
	{
		BotMessage(nullptr, 0, "map specific config file found...");

		// Remove all rejoining bots since theres a specific config.

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			m_Bots[i].Init();
			m_Bots[i].m_iRespawnState = RESPAWN_NONE;
		}
	}
	//	m_Locations.Destroy();
}

const char* CBotGlobals::GetModInfo(void)
{
	char game_dir[256];

	GET_GAME_DIR(game_dir);

	int pos = 0;

	if (strchr(game_dir, '/') != nullptr)
	{
		pos = strlen(game_dir) - 1;

		// scan backwards till first directory separator...
		while (pos && game_dir[pos] != '/')
			pos--;

		if (pos == 0)
		{
			// Error getting directory name!

			BotMessage(nullptr, 1, "Error determining MOD directory name!");
		}

		pos++;
	}

	m_szModFolder = m_Strings.GetString(&game_dir[pos]);

	CModInfo* pModInfo = m_Mods.GetModInfo(m_szModFolder);

	if (pModInfo != nullptr)
	{
		m_iCurrentMod = pModInfo->GetModId();

		m_bIsNS = m_iCurrentMod == MOD_NS;

		GameInit();

#ifndef RCBOT_META_BUILD
		return pModInfo->GetDllFileName(m_szModFolder);
#endif
	}
	/// no mod info
	GameInit();

	return nullptr;
}

void CBotGlobals::LoadBotModels(void)
// Load bot model names in bot globals botModels array
// Most done By Botman :-)
{
	char path[MAX_PATH];
	char search_path[MAX_PATH];
	char dirname[MAX_PATH];
	char filename[MAX_PATH];
	//   int index;
	struct stat stat_str;
#ifndef __linux__
	HANDLE directory = nullptr;
#else
	DIR* directory = NULL;
#endif

	// find the directory name of the currently running MOD...
	strcpy(path, m_szModFolder);

#ifdef __linux__
	strcat(path, "/models/player");
#else
	strcat(path, "\\models\\player");
#endif

	if (stat(path, &stat_str) != 0)
	{
		// use the valve/models/player directory if no MOD models/player
#ifdef __linux__
		strcpy(path, "valve/models/player");
#else
		strcpy(path, "valve\\models\\player");
#endif
	}

	strcpy(search_path, path);

#ifndef __linux__
	strcat(search_path, "/*");
#endif

	m_uaBotModels.Init();

	// Looking for model files (.mdl) in player directory
	// search_path = <search folder>/* (in windows)
	// or search_path = <search folder>

	while ((directory = FindDirectory(directory, dirname, search_path)) != nullptr)
	{
		// don't want to get stuck looking in the same directory again and again (".")
		// don't wan't to search parent directories ("..")
		if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)
			continue;

		// looking for .mdl file inside a folder of same name
		strcpy(filename, path);
		strcat(filename, "/");
		strcat(filename, dirname);
		strcat(filename, "/");
		strcat(filename, dirname);
		strcat(filename, ".mdl");

		// seeing if file exists (if foldername = model name)
		if (stat(filename, &stat_str) == 0)
		{
			// ok, we only need to add directory name into list then.
			m_uaBotModels.Add(m_Strings.GetString(dirname));
		}
	}
}

void CBotGlobals::ReadConfig(void)
{
	char filename[256];

	UTIL_BuildFileName(filename, "bot_config.ini", nullptr);

	FILE* fp = fopen(filename, "r");

	if (fp)
	{
		char cmd_line[64];
		char arg1[64];
		char arg2[64];
		char arg3[64];
		char arg4[64];
		char buffer[256];

		while (fgets(buffer, 127, fp) != nullptr)
		{
			int i = 0;

			int length = strlen(buffer);

			if (buffer[0] == '#') // comment
				continue;

			if (buffer[length - 1] == '\n')
			{
				buffer[length - 1] = 0;  // remove '\n'
				length--;
			}

			if (length == 0) // nothing on line
				continue;

			while (i < length && buffer[i] == ' ')
				i++;

			int j = 0;

			while (i < length && buffer[i] != ' ')
				cmd_line[j++] = buffer[i++];
			cmd_line[j] = 0;
			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;

			while (i < length && buffer[i] != ' ')
				arg1[j++] = buffer[i++];
			arg1[j] = 0;
			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;

			while (i < length && buffer[i] != ' ')
				arg2[j++] = buffer[i++];
			arg2[j] = 0;
			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;

			while (i < length && buffer[i] != ' ')
				arg3[j++] = buffer[i++];
			arg3[j] = 0;
			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;

			while (i < length && buffer[i] != ' ')
				arg4[j++] = buffer[i++];
			arg4[j] = 0;

			m_CurrentHandledCvar = m_BotCvars.GetCvar(cmd_line);

			if (m_CurrentHandledCvar)
				m_CurrentHandledCvar->action(nullptr, arg1, arg2, arg3, arg4);
			else
				BotMessage(nullptr, 0, "Error with config, unknown command : %s", cmd_line);
		}

		fclose(fp);
	}
	else
		BotMessage(nullptr, 0, "Error: could not find bot config file (%s)", filename);
}

void CBotGlobals::saveLearnedData()
{
	char szFilename[256];
	char tmpFilename[64];

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		CLearnedHeader header = CLearnedHeader(i);

		sprintf(tmpFilename, "team%d.rld", i);
		UTIL_BuildFileName(szFilename, tmpFilename);

		FILE* bfp = fopen(szFilename, "wb");

		if (bfp)
		{
			fwrite(&header, sizeof(CLearnedHeader), 1, bfp);
			this->m_enemyCostGAsForTeam[i].save(bfp);
			fclose(bfp);
		}

		//this->m_enemyCostGAsForTeam[i].freeLocalMemory();
	}

	sprintf(tmpFilename, "tsweaps.rld");
	UTIL_BuildFileName(szFilename, tmpFilename);

	FILE* bfp = fopen(szFilename, "wb");

	if (bfp)
	{
		this->m_TSWeaponChoices.save(bfp);
		fclose(bfp);
	}

	//this->m_TSWeaponChoices.freeLocalMemory();

	if (IsNS() && IsCombatMap())
	{
		sprintf(tmpFilename, "combat0.rld");
		UTIL_BuildFileName(szFilename, tmpFilename);

		bfp = fopen(szFilename, "wb");

		if (bfp)
		{
			this->m_pCombatGA[0].save(bfp);
			fclose(bfp);
		}

		sprintf(tmpFilename, "combat1.rld");
		UTIL_BuildFileName(szFilename, tmpFilename);

		bfp = fopen(szFilename, "wb");

		if (bfp)
		{
			this->m_pCombatGA[1].save(bfp);
			fclose(bfp);
		}

		//this->m_pCombatGA[0].freeLocalMemory();
		//this->m_pCombatGA[1].freeLocalMemory();
	}
}

void CBotGlobals::loadLearnedData()
{
	char szFilename[256];
	char tmpFilename[64];

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		CLearnedHeader header = CLearnedHeader(i);
		const CLearnedHeader checkheader = CLearnedHeader(i);

		sprintf(tmpFilename, "team%d.rld", i);
		UTIL_BuildFileName(szFilename, tmpFilename);

		FILE* bfp = fopen(szFilename, "rb");

		if (bfp)
		{
			fread(&header, sizeof(CLearnedHeader), 1, bfp);

			if (header == checkheader)
				this->m_enemyCostGAsForTeam[i].load(bfp, 16);
			else
				BotMessage(nullptr, 0, "Team's learned data for %s header mismatch", tmpFilename);

			fclose(bfp);
		}
	}

	sprintf(tmpFilename, "tsweaps.rld");
	UTIL_BuildFileName(szFilename, tmpFilename);

	FILE* bfp = fopen(szFilename, "rb");

	if (bfp)
	{
		this->m_TSWeaponChoices.load(bfp, 16);
		fclose(bfp);
	}

	if (IsNS())
	{
		sprintf(tmpFilename, "combat0.rld");
		UTIL_BuildFileName(szFilename, tmpFilename);
		bfp = fopen(szFilename, "rb");

		if (bfp)
		{
			this->m_pCombatGA[0].setPopType(TYPE_INTGAVALS);
			this->m_pCombatGA[0].load(bfp, 16);
			fclose(bfp);
		}

		sprintf(tmpFilename, "combat1.rld");
		UTIL_BuildFileName(szFilename, tmpFilename);
		bfp = fopen(szFilename, "rb");

		if (bfp)
		{
			this->m_pCombatGA[1].setPopType(TYPE_INTGAVALS);
			this->m_pCombatGA[1].load(bfp, 16);
			fclose(bfp);
		}
	}
}

void CBotGlobals::GameInit(void)
{
	// where things are stored ..DO THIS FIRST !!
	ReadBotFolder();

	m_iForceTeam = -1;

	switch (m_iCurrentMod)
	{
	case MOD_NS:
		// make a new things to build structure
		m_ThingsToBuild = new CThingsToBuild();
		// read the file for input.
		ReadThingsToBuild();
		break;
	default:
		break;
	}

	LoadBotModels();
	////////////////////////////
	// USERS
	ReadBotUsersConfig();
	////////////////////////////
	// NET MESSAGES
	SetupNetMessages();
	////////////////////////////
	// COMMANDS & CVARS
	SetupCommands();
	///////////////////////////
	// BOT CHAT
	SetupBotChat();
	//////////////////////////

	//////////////////////
	// CONFIG
	ReadConfig();
	// WEAPON PRESETS
	m_Weapons.Init();
	m_WeaponPresets.ReadPresets();

	loadLearnedData();

	BotMessage(nullptr, 0, "RCBOT BUILD %s-%s", __DATE__, __TIME__);
}

void CBotGlobals::FreeLocalMemory(void)
{
	int i;

	WaypointInit();

	m_pDebugMessage = nullptr;

	this->m_BotCam.Clear();

	this->m_HiveMind.FreeLocalMemory();
	this->m_Masters.FreeLocalMemory();
	this->m_Squads.FreeMemory();
	this->m_CurrentHandledCvar = nullptr;
	this->m_CurrentMessage = nullptr;
	this->m_iBotMsgIndex = -1;
	this->m_iCurrentMessageState = 0;
	this->m_iCurrentMessageState2 = 0;
	this->m_iFakeArgCount = 0;
	this->m_bIsFakeClientCommand = FALSE;

	//	this->m_EntityMasters.Destroy();
	//	this->m_EntityMasters.Init();

	this->m_bWaypointsHavePaths = FALSE;
	this->m_bBotCanRejoin = FALSE;
	this->m_bCanUpgradeDef = FALSE;
	this->m_bCanUpgradeMov = FALSE;
	this->m_bCanUpgradeSens = FALSE;
	this->m_bHasDefTech = FALSE;
	this->m_bHasSensTech = FALSE;
	this->m_bHasMovTech = FALSE;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		this->m_Bots[i].m_Weapons.RemoveWeapons();
	}

	for (i = 0; i < MAX_TEAMS; i++)
		this->m_TeamTechs[i].freeMemory();
}

void CBotGlobals::ReadThingsToBuild(void) const
{
	char filename[512];
	char szbuffer[256];
	char szline[256];

	int iBuilding = 0;

	UTIL_BuildFileName(filename, "things_to_build.ini", nullptr);
	//	sprintf(szbuffer,"%sthings_to_build.ini",RCBOT_FOLDER);

	FILE* fp = fopen(filename, "r");

	int iNum;
	int iPriority;

	if (fp)
	{
		m_ThingsToBuild->Clear();

		while (fgets(szbuffer, 255, fp) != nullptr)
		{
			szbuffer[255] = 0;

			if (szbuffer[0] == '#')
				continue; // comment

			int ilen = strlen(szbuffer);

			if (ilen == 0)
				continue;

			if (szbuffer[ilen - 1] == '\n')
				szbuffer[--ilen] = 0;

			if (ilen == 0)
				continue;

			if (szbuffer[0] == '[')
			{
				int i = 1;
				int j = 0;

				while (i < ilen && szbuffer[i] != ']')
					szline[j++] = szbuffer[i++];
				szline[j] = 0;

				if (strcmp(szline, "hive") == 0)
					iBuilding = AVH_USER3_HIVE;
				else if (strcmp(szline, "alienrestower") == 0)
					iBuilding = AVH_USER3_ALIENRESTOWER;
				else if (strcmp(szline, "off") == 0)
					iBuilding = AVH_USER3_OFFENSE_CHAMBER;
				else if (strcmp(szline, "def") == 0)
					iBuilding = AVH_USER3_DEFENSE_CHAMBER;
				else if (strcmp(szline, "mov") == 0)
					iBuilding = AVH_USER3_MOVEMENT_CHAMBER;
				else if (strcmp(szline, "sen") == 0)
					iBuilding = AVH_USER3_SENSORY_CHAMBER;

				continue;
			}
			else if (iBuilding)
			{
				CThingToBuild* theThingsToBuild = nullptr;

				switch (iBuilding)
				{
				case AVH_USER3_HIVE:
					theThingsToBuild = &m_ThingsToBuild->m_forHive;
					break;
				case AVH_USER3_ALIENRESTOWER:
					theThingsToBuild = &m_ThingsToBuild->m_forResTower;
					break;
				case AVH_USER3_OFFENSE_CHAMBER:
					theThingsToBuild = &m_ThingsToBuild->m_forOffenseChamber;
					break;
				case AVH_USER3_DEFENSE_CHAMBER:
					theThingsToBuild = &m_ThingsToBuild->m_forDefenseChamber;
					break;
				case AVH_USER3_MOVEMENT_CHAMBER:
					theThingsToBuild = &m_ThingsToBuild->m_forMovementChamber;
					break;
				case AVH_USER3_SENSORY_CHAMBER:
					theThingsToBuild = &m_ThingsToBuild->m_forSensoryChamber;
					break;
				default: ;
				}

				if (theThingsToBuild)
				{
					if (sscanf(szbuffer, "offs=%d,priority=%d", &iNum, &iPriority) == 2)
						theThingsToBuild->m_iOffs = CStructToBuild(iNum, iPriority);
					else if (sscanf(szbuffer, "defs=%d,priority=%d", &iNum, &iPriority) == 2)
						theThingsToBuild->m_iDefs = CStructToBuild(iNum, iPriority);
					else if (sscanf(szbuffer, "movs=%d,priority=%d", &iNum, &iPriority) == 2)
						theThingsToBuild->m_iMovs = CStructToBuild(iNum, iPriority);
					else if (sscanf(szbuffer, "sens=%d,priority=%d", &iNum, &iPriority) == 2)
						theThingsToBuild->m_iSens = CStructToBuild(iNum, iPriority);
				}
			}
		}

		fclose(fp);
	}
	else
		BotMessage(nullptr, 0, "Error: could not find NS aliens build file (%s)", filename);
}

void CBotGlobals::FreeGlobalMemory(void)
{
	/// --- ?? Required ?? we've got all our destructors in
		   // Free ALL memory when quitting HL
	this->FreeLocalMemory();

	int i;

	for (i = 0; i < BOT_MENU_MAX_ITEMS; i++)
		m_Menus[i].DestroyMenu();

	if (this->m_ThingsToBuild)
	{
		delete this->m_ThingsToBuild;
		this->m_ThingsToBuild = nullptr;
	}

	WaypointVisibility.FreeVisibilityTable();

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		this->m_Bots[i].FreeGlobalMemory();
	}

	this->m_BotCvars.Destroy();
	this->m_BotUsers.Destroy();
	this->m_Mods.DestroyInfo();
	this->m_NetAllMessages.Destroy();
	this->m_NetEntityMessages.Destroy();
	this->m_Strings.FreeStrings();
	this->m_WeaponPresets.Destroy();

	this->m_BotChat.m_Greetings[0].Clear();
	this->m_BotChat.m_Greetings[1].Clear();
	this->m_BotChat.m_Greetings[2].Clear();
	this->m_BotChat.m_Help.Clear();
	this->m_BotChat.m_Idle.Clear();
	this->m_BotChat.m_Killed[0].Clear();
	this->m_BotChat.m_Killed[1].Clear();
	this->m_BotChat.m_Killed[2].Clear();
	this->m_BotChat.m_Kills[0].Clear();
	this->m_BotChat.m_Kills[1].Clear();
	this->m_BotChat.m_Kills[2].Clear();
	this->m_BotChat.m_Laugh[0].Clear();
	this->m_BotChat.m_Laugh[1].Clear();
	this->m_BotChat.m_Laugh[2].Clear();
	this->m_BotChat.m_Thanks[0].Clear();
	this->m_BotChat.m_Thanks[1].Clear();
	this->m_BotChat.m_Thanks[2].Clear();
	this->m_BotChat.m_LeaveGame[0].Clear();
	this->m_BotChat.m_LeaveGame[1].Clear();
	this->m_BotChat.m_LeaveGame[2].Clear();

	this->m_BotUsers.Destroy();

	this->m_NetAllMessages.Destroy();
	this->m_NetEntityMessages.Destroy();

	this->m_WeaponPresets.Destroy();
	this->m_uaBotModels.Clear();

	this->m_Clients.FreeGlobalMemory();
	this->m_Strings.FreeStrings();

	this->m_enemyCostGAsForTeam[0].freeLocalMemory();
	this->m_enemyCostGAsForTeam[1].freeLocalMemory();
	this->m_enemyCostGAsForTeam[2].freeLocalMemory();
	this->m_enemyCostGAsForTeam[3].freeLocalMemory();
	this->m_TSWeaponChoices.freeLocalMemory();
	this->m_pCombatGA[0].freeLocalMemory();
	this->m_pCombatGA[1].freeLocalMemory();

	// nullify everything
	memset(this, 0, sizeof(CBotGlobals));
}

void CBotGlobals::SetupBotChat(void)
{
	char filename[512];

	UTIL_BuildFileName(filename, BOT_CHAT_FILE, nullptr);

	FILE* fp = fopen(filename, "r");

	if (fp == nullptr)
	{
		BotMessage(nullptr, 0, "Warning : Could not find bot chat file! (%s)", filename);
		return;
	}

	char buffer[256];

	dataUnconstArray<char*>* chatStack = nullptr;

	m_BotChat.m_Thanks[2].Init();
	m_BotChat.m_Thanks[1].Init();
	m_BotChat.m_Thanks[0].Init();

	m_BotChat.m_Greetings[2].Init();
	m_BotChat.m_Greetings[1].Init();
	m_BotChat.m_Greetings[0].Init();

	m_BotChat.m_Kills[2].Init();
	m_BotChat.m_Kills[1].Init();
	m_BotChat.m_Kills[0].Init();

	m_BotChat.m_Killed[2].Init();
	m_BotChat.m_Killed[1].Init();
	m_BotChat.m_Killed[0].Init();

	m_BotChat.m_Laugh[2].Init();
	m_BotChat.m_Laugh[1].Init();
	m_BotChat.m_Laugh[0].Init();

	m_BotChat.m_LeaveGame[0].Init();
	m_BotChat.m_LeaveGame[1].Init();
	m_BotChat.m_LeaveGame[2].Init();

	m_BotChat.m_Idle.Init();

	m_BotChat.m_Help.Init();

	while (fgets(buffer, 255, fp) != nullptr)
	{
		if (buffer[0] == '#')
			continue;

		int iLength = strlen(buffer);

		if (buffer[iLength - 1] == '\n')
		{
			buffer[--iLength] = 0;
		}

		if (iLength <= 0)
			continue;

		if (buffer[0] == '[')
		{
			if (FStrEq(buffer, "[thanks-friendly]"))
				chatStack = &m_BotChat.m_Thanks[2];
			else if (FStrEq(buffer, "[thanks-neutral]"))
				chatStack = &m_BotChat.m_Thanks[1];
			else if (FStrEq(buffer, "[thanks-unfriendly]"))
				chatStack = &m_BotChat.m_Thanks[0];
			else if (FStrEq(buffer, "[greetings-friendly]"))
				chatStack = &m_BotChat.m_Greetings[2];
			else if (FStrEq(buffer, "[greetings-neutral]"))
				chatStack = &m_BotChat.m_Greetings[1];
			else if (FStrEq(buffer, "[greetings-unfriendly]"))
				chatStack = &m_BotChat.m_Greetings[0];
			else if (FStrEq(buffer, "[kills-friendly]"))
				chatStack = &m_BotChat.m_Kills[2];
			else if (FStrEq(buffer, "[kills-neutral]"))
				chatStack = &m_BotChat.m_Kills[1];
			else if (FStrEq(buffer, "[kills-unfriendly]"))
				chatStack = &m_BotChat.m_Kills[0];
			else if (FStrEq(buffer, "[killed-friendly]"))
				chatStack = &m_BotChat.m_Killed[2];
			else if (FStrEq(buffer, "[killed-neutral]"))
				chatStack = &m_BotChat.m_Killed[1];
			else if (FStrEq(buffer, "[killed-unfriendly]"))
				chatStack = &m_BotChat.m_Killed[0];
			else if (FStrEq(buffer, "[laugh-friendly]"))
				chatStack = &m_BotChat.m_Laugh[2];
			else if (FStrEq(buffer, "[laugh-neutral]"))
				chatStack = &m_BotChat.m_Laugh[1];
			else if (FStrEq(buffer, "[laugh-unfriendly]"))
				chatStack = &m_BotChat.m_Laugh[0];
			else if (FStrEq(buffer, "[leave-friendly]"))
				chatStack = &m_BotChat.m_LeaveGame[2];
			else if (FStrEq(buffer, "[leave-neutral]"))
				chatStack = &m_BotChat.m_LeaveGame[1];
			else if (FStrEq(buffer, "[leave-unfriendly]"))
				chatStack = &m_BotChat.m_LeaveGame[0];
			else if (FStrEq(buffer, "[idle]"))
				chatStack = &m_BotChat.m_Idle;
			else if (FStrEq(buffer, "[help]"))
				chatStack = &m_BotChat.m_Help;
		}
		else if (chatStack != nullptr)
		{
			chatStack->Add(m_Strings.GetString(buffer));
		}
	}

	fclose(fp);
}