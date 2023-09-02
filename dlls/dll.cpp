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
 // dll.cpp
 //
 //////////////////////////////////////////////////
 //
 // engine + initializing functions + bot interface
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD

#include "enginecallback.h"
#include "util.h"
#include "cbase.h"

#else

#include "h_export_meta.h"
#include "dllapi.h"
#include "meta_api.h"

#endif

#include "engine.h"

#include "entity_state.h"

#include "bot_client.h"
#include "bot_const.h"
#include "bot.h"
#include "waypoint.h"
#include "bot_commands.h"

#ifndef RCBOT_META_BUILD
extern GETENTITYAPI other_GetEntityAPI;
extern GETNEWDLLFUNCTIONS other_GetNewDLLFunctions;
#endif

#ifdef _WIN32
#define strdup _strdup
#endif

extern enginefuncs_t g_engfuncs;
extern int debug_engine;
extern globalvars_t* gpGlobals;
extern char* g_argv;

DLL_FUNCTIONS other_gFunctionTable;
DLL_GLOBAL const Vector g_vecZero = Vector(0, 0, 0);

CBotGlobals gBotGlobals;

extern CWaypointVisibilityTable WaypointVisibility;
extern CWaypointLocations WaypointLocations;

cvar_t bot_ver_cvar = { BOT_VER_CVAR,BOT_VER,FCVAR_SERVER };

std::FILE* fpMapConfig = nullptr;

void UpdateClientData(const edict_s* ent, int sendweapons, clientdata_s* cd);

///////////////////////////////////////////////////////////
//
// Allowed players
// checks if this item is for a client
BOOL CAllowedPlayer::IsForClient(CClient* pClient) const
{
	BOOL bSameName = false;

	if (steamID_defined())
	{
		return IsForSteamID(pClient->steamID());
	}

	if (*m_szName) //m_szName always true [APG]RoboCop[CL]
		bSameName = pClient->HasPlayerName(m_szName);

	const edict_t* pEdict = pClient->GetPlayer();

	if (pEdict == nullptr)
		return false;

	const BOOL bSamePass = IsForPass(pClient->GetPass());

	return bSameName && bSamePass;
}
//
///////////////////////////////////////////////////////////
//

////////////////////////////////////////////////////////////////////////
// ENGINE CALLED FUNCTIONS

edict_t* UTIL_GetCommander()
{
	if (gBotGlobals.IsNS())
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			edict_t* pPlayer = INDEXENT(i);

			if (!pPlayer || pPlayer->free)
				continue;

			if (pPlayer->v.netname && *STRING(pPlayer->v.netname))
			{
				if (pPlayer->v.iuser3 == AVH_USER3_COMMANDER_PLAYER)
					return pPlayer;
			}
		}
	}

	return nullptr;
}
//////////////////////////////////////////////////////////////////////
void GameDLLInit()
{
	//	CVAR_REGISTER (&sv_bot);

#ifndef RCBOT_META_BUILD
	CVAR_REGISTER(&bot_ver_cvar); // need to be done here for NON-metamod only...
	(*g_engfuncs.pfnAddServerCommand)(BOT_COMMAND_ACCESS, RCBot_ServerCommand);
#endif

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnGameInit)();
#endif
}

///////////////////////////////////////////////////////////////////
/*

	DispatchSpawn is called when an entity spawns in the game
	the entity that spawns is the *pent argument

*/
///////////////////////////////////////////////////////////////////
int DispatchSpawn(edict_t* pent)
{
	if (gpGlobals->deathmatch)
	{
		char* pClassname = const_cast<char*>(STRING(pent->v.classname));

		if (debug_engine)
		{
			std::FILE* fp = std::fopen("bot.txt", "a");
			std::fprintf(fp, "DispatchSpawn: %p %s\n", pent, pClassname);
			if (pent->v.model != 0)
				std::fprintf(fp, " model=%s\n", STRING(pent->v.model));
			std::fclose(fp);
		}

		if (std::strcmp(pClassname, "worldspawn") == 0)
		{
			// do level initialization stuff here...
			gBotGlobals.MapInit();

			// clear waypoints
			WaypointInit();
			WaypointLoad(nullptr);
		}

		if (gBotGlobals.IsNS())
		{
			if (!gBotGlobals.m_pMarineStart)
			{
				if (std::strcmp(STRING(pent->v.classname), "team_command") == 0)
				{
					gBotGlobals.m_pMarineStart = pent;

					if (gBotGlobals.IsCombatMap())
					{
						// to continously check if comm console is under attack
						gBotGlobals.m_CommConsole = CStructure(pent);
					}
				}
			}
			/*
			doesn't work here... obviously iuser3 isn't set till after DispatchSpawn

			// Use the bots vision to advantage
			// and add ant structures it sees in NS to the hivemind
			if ( EntityIsAlienStruct(pent) )
				gBotGlobals.m_HiveMind.AddStructure(pent,ENTINDEX(pent));*/
		}
	}

#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnSpawn)(pent);
#endif
}

///////////////////////////////////////////////////////////////////////////
/*
	DispatchThink is called when an entity thinks...
*/
///////////////////////////////////////////////////////////////////////////
void DispatchThink(edict_t* pent)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnThink)(pent);
#endif
}
///////////////////////////////////////////////////////////////////////////
/*
	DispatchUse is called when pentOther uses pentUsed
*/
///////////////////////////////////////////////////////////////////////////
void DispatchUse(edict_t* pentUsed, edict_t* pentOther)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnUse)(pentUsed, pentOther);
#endif
}
///////////////////////////////////////////////////////////////////////////
void DispatchTouch(edict_t* pentTouched, edict_t* pentOther)
{
	static CBot* pBot;

	// only don't touch triggers if "no touch" is on the client.
	if (pentTouched->v.solid == SOLID_TRIGGER)
	{
		if (const CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pentOther))
		{
			if (pClient->m_bNoTouch)
			{
#ifdef RCBOT_META_BUILD
				RETURN_META(MRES_SUPERCEDE);
#else
				return;
#endif
			}
		}
	}

	pBot = UTIL_GetBotPointer(pentOther);

	if (pBot)
	{
		// bot code wants to evade engine seeing entities being touched.
		if (pBot->Touch(pentTouched))
		{
#ifdef RCBOT_META_BUILD
			RETURN_META(MRES_SUPERCEDE);
#else
			return;
#endif
		}
	}

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
#endif
}
///////////////////////////////////////////////////////////////////////////
void DispatchBlocked(edict_t* pentBlocked, edict_t* pentOther)
{
	static CBot* pBot = nullptr;

	// Save some time since we use a static variable
	BOOL bFindNewBot = true;

	if (pBot != nullptr)
	{
		if (pBot->m_pEdict == pentOther)
			bFindNewBot = false;
	}

	if (bFindNewBot) // takes a few loops
		pBot = UTIL_GetBotPointer(pentOther);

	if (pBot)
		pBot->Blocked(pentBlocked);

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnBlocked)(pentBlocked, pentOther);
#endif
}
///////////////////////////////////////////////////////////////////////////
void DispatchKeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd)
{
	gBotGlobals.KeyValue(pentKeyvalue, pkvd);

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnKeyValue)(pentKeyvalue, pkvd);
#endif
}
///////////////////////////////////////////////////////////////////////////
void DispatchSave(edict_t* pent, SAVERESTOREDATA* pSaveData)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSave)(pent, pSaveData);
#endif
}
///////////////////////////////////////////////////////////////////////////
int DispatchRestore(edict_t* pent, SAVERESTOREDATA* pSaveData, int globalEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnRestore)(pent, pSaveData, globalEntity);
#endif
}
///////////////////////////////////////////////////////////////////////////
void DispatchObjectCollsionBox(edict_t* pent)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSetAbsBox)(pent);
#endif
}
///////////////////////////////////////////////////////////////////////////
void SaveWriteFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSaveWriteFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
#endif
}
///////////////////////////////////////////////////////////////////////////
void SaveReadFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSaveReadFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
#endif
}
///////////////////////////////////////////////////////////////////////////
void SaveGlobalState(SAVERESTOREDATA* pSaveData)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSaveGlobalState)(pSaveData);
#endif
}
///////////////////////////////////////////////////////////////////////////
void RestoreGlobalState(SAVERESTOREDATA* pSaveData)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnRestoreGlobalState)(pSaveData);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ResetGlobalState()
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnResetGlobalState)();
#endif
}
///////////////////////////////////////////////////////////////////////////
BOOL ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	if (gpGlobals->deathmatch)
	{
		const int iIndex = ENTINDEX(pEntity) - 1;

		CClient* pClient = gBotGlobals.m_Clients.GetClientByIndex(iIndex);

		gBotGlobals.m_iJoiningClients[iIndex] = 0;

		if (pClient)
		{
			pClient->Init();
		}

		if (debug_engine) { std::FILE* fp = std::fopen("bot.txt", "a"); std::fprintf(fp, "ClientConnect: pent=%p name=%s\n", pEntity, pszName); std::fclose(fp); }

		if (!IS_DEDICATED_SERVER())
		{
			if (gBotGlobals.m_pListenServerEdict == nullptr)
			{
				// check if this client is the listen server client
				if (std::strcmp(pszAddress, "loopback") == 0)
				{
					// save the edict of the listen server client...
					gBotGlobals.m_pListenServerEdict = pEntity;

					if (pClient)
						pClient->SetAccessLevel(10);
				}
			}
			else if (pEntity == gBotGlobals.m_pListenServerEdict)
			{
				if (pClient)
					pClient->SetAccessLevel(10);
			}
		}

		if (std::strcmp(pszAddress, "127.0.0.1") != 0) // not a bot connecting
		{
			if (gBotGlobals.m_iMinBots != -1)
			{
				const int iNumPlayerCheck = UTIL_GetNumClients(true) + 1 + gBotGlobals.GetNumJoiningClients();

				if (gBotGlobals.m_iNumBots > gBotGlobals.m_iMinBots && iNumPlayerCheck > gBotGlobals.m_iMaxBots)
					// Can it kick a bot to free a slot?
				{
					for (int i = 0; i < MAX_PLAYERS; i++)
					{
						if (const CBot* pBot = &gBotGlobals.m_Bots[i])  // is this slot used?
						{
							if (pBot->IsUsed())
							{
								char cmd[80];
								//int iUserId;

								//iUserId = (*g_engfuncs.pfnGetPlayerUserId)(pBot->m_pEdict);

								BotMessage(nullptr, 0, "Kicking a bot from server to free a slot...");

								//std::sprintf(cmd, "kick #%d\n", iUserId);
								// kick [# userid] doesnt work on linux ??
								std::sprintf(cmd, "kick \"%s\"\n", pBot->m_szBotName);

								SERVER_COMMAND(cmd);  // kick the bot using kick name //(kick #id)

								gBotGlobals.m_fBotRejoinTime = gpGlobals->time + 10.0f;
								gBotGlobals.m_bBotCanRejoin = false;

								break;
							}
						}
					}
					// kick the bot
				}
				else if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_RESERVE_BOT_SLOTS) && gBotGlobals.m_iNumBots < gBotGlobals.m_iMinBots)
				{
					const int iNumPlayers = UTIL_GetNumClients(true) + 1;
					const int iBotsStillToJoin = gBotGlobals.m_iMinBots - gBotGlobals.m_iNumBots;
					const int iNewSlotsFree = gpGlobals->maxClients - iNumPlayers;
					// dont allow player to connect as the number of bots
					// have not been reached yet.

					BotMessage(nullptr, 0, "Player joining, min_bots not reached, Checking to disallow player...");
					BotMessage(nullptr, 0, "numplayers to be = %d\nmin_bots = %d\nbots still to join = %d\nmax clients = %d\nnew slots = %d\nnum bots = %d", iNumPlayers,
						gBotGlobals.m_iMinBots,
						iBotsStillToJoin,
						gpGlobals->maxClients,
						iNewSlotsFree,
						gBotGlobals.m_iNumBots);

					if (iNewSlotsFree < iBotsStillToJoin)
					{
						BotMessage(nullptr, 0, "Rejecting real player from joining as min_bots has not been reached yet...");

						std::strcpy(szRejectReason, "\nMaximum public slots reached (some slots are reserved)");

#ifdef RCBOT_META_BUILD
						RETURN_META_VALUE(MRES_SUPERCEDE, 0);
#else
						return false;
#endif
					}
				}
			}
		}

		gBotGlobals.m_iJoiningClients[iIndex] = 1;
	}

#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnClientConnect)(pEntity, pszName, pszAddress, szRejectReason);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ClientDisconnect(edict_t* pEntity)
{
	// Is the player that is disconnecting an RCbot?
	CBot* pBot = UTIL_GetBotPointer(pEntity);

	const int iIndex = ENTINDEX(pEntity) - 1;

	if (EntityIsCommander(pEntity))
		gBotGlobals.SetCommander(nullptr);

	if (iIndex < MAX_PLAYERS)
		gBotGlobals.m_iJoiningClients[iIndex] = 0;

	if (pBot)
	{
		//		FILE *fp;
		//		char szFilename[128];
		//		int iNewProfile = 1;

		/*		if ( pBot->m_Profile.m_iProfileId == 0 ) // Bot doesn't have a Profile yet?
				{
					// Create a NEW Profile...

					// find a free id

					dataStack<int> s_iProfiles;

					UTIL_BuildFileName(szFilename,BOT_PROFILES_FILE,NULL);

					fp = std::fopen(szFilename,"r");

					if ( fp )
					{
						int iCurrentId = 0;

						while ( !std::feof(fp) )
						{
							std::fscanf(fp,"%d\n",&iCurrentId);

							s_iProfiles.Push(iCurrentId);
						}

						std::fclose(fp);
					}

					while ( iNewProfile <= MAX_BOT_ID )
					{
						if ( !s_iProfiles.IsMember(&iNewProfile) )
							break;
						else
							iNewProfile ++;
					}

					if ( iNewProfile <= MAX_BOT_ID )
					{
						fp = std::fopen(szFilename,"a"); // Open the "botprofiles.ini" add this profile to the list

						if ( fp )
						{
							std::fprintf(fp,"%d\n",iNewProfile);
							std::fclose(fp);
						}
						else
							BotMessage(NULL,0,"Error creating bot profile!");
					}
				}
				else
					iNewProfile = pBot->m_Profile.m_iProfileId;

				if ( iNewProfile <= MAX_BOT_ID )
				{
					char szBotProfile[8];

					std::sprintf(szBotProfile,"%d.ini",iNewProfile);

					UTIL_BuildFileName(szFilename,"botprofiles",szBotProfile);

					fp = std::fopen(szFilename,"w");

					if ( fp )
					{
						BotFunc_WriteProfile(fp,&pBot->m_Profile);

						std::fclose(fp);
					}
				}
				else
					BotMessage(NULL,0,"Error: Couldn't Create Bot Profile!");*/

		const int iProfileId = pBot->m_Profile.m_iProfileId;
		const int iTeam = pBot->m_Profile.m_iFavTeam;

		SaveHALBrainForPersonality(&pBot->m_Profile); // save this personality's HAL brain

		pBot->saveLearnedData();
		pBot->FreeLocalMemory();

		//if ( pBot->m_iRespawnState != RESPAWN_NEED_TO_REJOIN )
		pBot->m_iRespawnState = RESPAWN_NONE;

		pBot->Init();
		pBot->SetEdict(nullptr);
		pBot->m_bIsUsed = false;
		pBot->m_fKickTime = gpGlobals->time;
		pBot->m_bIsUsed = false;

		pBot->m_Profile.m_iProfileId = iProfileId;
		pBot->m_Profile.m_iFavTeam = iTeam;

		if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_REAL_MODE)) // NOT Realistic mode...
		{
			// Free this bots info and initiliases
			// we'll add a new random bot upon map
			// change to cover this bots slot
		}
		else
		{
			// Keeps this bots info in memory and we'll decide on what the bot
			// wants to do after joining into the next map
			// The bot can then "see" what the next map is and dependant on
			// their favourite map + team they can join + their reputation
			// with others joining the server and how they've done
			// on previous games will decide whether they want to leave or not...

			// Edict info will change so reset it.
		}
	}

	gBotGlobals.m_Clients.ClientDisconnected(pEntity);

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnClientDisconnect)(pEntity);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ClientKill(edict_t* pEntity)
{
	if (debug_engine) {
		std::FILE* fp = std::fopen("bot.txt", "a");
		std::fprintf(fp, "ClientKill: %x\n", reinterpret_cast<unsigned>(pEntity));
		std::fclose(fp);
	}

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnClientKill)(pEntity);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ClientPutInServer(edict_t* pEntity)
{
	if (debug_engine) {
		std::FILE* fp = std::fopen("bot.txt", "a");
		std::fprintf(fp, "ClientPutInServer: %x\n", reinterpret_cast<unsigned>(pEntity));
		std::fclose(fp);
	}

	gBotGlobals.m_Clients.ClientConnected(pEntity);

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnClientPutInServer)(pEntity);
#endif
}
///////////////////////////////////////////////////////////////////////////
void FreeArgs(const char* pcmd,
	const char* arg1,
	const char* arg2,
	const char* arg3,
	const char* arg4,
	const char* arg5)
{
	// free the dupliacted strings when a fake client command
	// is made
	if (gBotGlobals.m_bIsFakeClientCommand)
	{
		if (pcmd)
			std::free((void*)pcmd);
		if (arg1)
			std::free((void*)arg1);
		if (arg2)
			std::free((void*)arg2);
		if (arg3)
			std::free((void*)arg3);
		if (arg4)
			std::free((void*)arg4);
		if (arg5)
			std::free((void*)arg5);
	}
}

void BotFunc_TraceToss(edict_t* ent, edict_t* ignore, TraceResult* tr)
{
	edict_t tempent = *ent;
	Vector gravity = Vector(0, 0, CVAR_GET_FLOAT("sv_gravity"));

	for (int i = 0; i < 4; i++)
	{
		Vector nextvel = tempent.v.velocity + (tempent.v.velocity - gravity) * 0.5f;//Vector(0,0,(CVAR_GET_FLOAT("sv_gravity")/tempent.v.velocity.Length()));
		Vector nextstep = tempent.v.origin + nextvel;

		UTIL_TraceLine(tempent.v.origin, nextstep, dont_ignore_monsters, dont_ignore_glass, ignore, tr);

		//WaypointDrawBeam(gBotGlobals.m_Clients.GetClientByIndex(0)->GetPlayer(),tempent.v.origin,tr->vecEndPos,16,0,255,255,255,200,10);

		tempent.v.origin = tr->vecEndPos;
		tempent.v.velocity = nextvel;
	}

	// final draw to end
	UTIL_TraceLine(tempent.v.origin, tempent.v.origin + tempent.v.velocity * 256, dont_ignore_monsters, dont_ignore_glass, ignore, tr);
	//WaypointDrawBeam(gBotGlobals.m_Clients.GetClientByIndex(0)->GetPlayer(),tempent.v.origin,tr->vecEndPos,16,0,255,255,255,200,10);
}

///////////////////////////////////////////////////////////////////////////
void ClientCommand(edict_t* pEntity)
{
	const char* pcmd;
	const char* arg1;
	const char* arg2;
	const char* arg3;
	const char* arg4;
	const char* arg5;

	if (!gBotGlobals.m_bIsFakeClientCommand)
	{
		pcmd = CMD_ARGV(0);
		arg1 = CMD_ARGV(1);
		arg2 = CMD_ARGV(2);
		arg3 = CMD_ARGV(3);
		arg4 = CMD_ARGV(4);
		arg5 = CMD_ARGV(5);
	}
	else
	{
		// duplicate strings for bot client commands
		pcmd = strdup(pfnCmd_Argv(0));
		arg1 = strdup(pfnCmd_Argv(1));
		arg2 = strdup(pfnCmd_Argv(2));
		arg3 = strdup(pfnCmd_Argv(3));
		arg4 = strdup(pfnCmd_Argv(4));
		arg5 = strdup(pfnCmd_Argv(5));
	}

	eBotCvarState iState = BOT_CVAR_CONTINUE;
	int iAccessLevel = 0;
	BOOL bSayTeamMsg = false;
	BOOL bSayMsg = false;

	CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pEntity);

	if (pClient)
		iAccessLevel = pClient->GetAccessLevel();

	/// available for all :p :)
	if (FStrEq(pcmd, "get_rcbot_ver"))
	{
		BotMessage(pEntity, 0, "Bot version is : %s build %s-%s", BOT_VER, __DATE__, __TIME__);

		iState = BOT_CVAR_ACCESSED;
	}
	// someone said something
	else if ((bSayMsg = FStrEq(pcmd, "say")) == 1 || (bSayTeamMsg = FStrEq(pcmd, "say_team")) == 1)
	{
		BOOL bMadeSquad = false;

		if (bSayTeamMsg)
		{
			// player wants to lead squad?
			if (std::strncmp(arg1, "form", 4) == 0 &&
				(std::strncmp(&arg1[4], " up", 3) == 0 || std::strncmp(arg2, "up", 2) == 0))
			{
				// loop through bots in team
				BotFunc_MakeSquad(pClient);
				bMadeSquad = true;
			}
		}

		// not trying to make a squad (i.e. dont reply to it)...
		if (!bMadeSquad)
		{
			///////
			// see if bot can learn its HAL brain from this person speaking
			const BOOL bSenderIsBot = UTIL_GetBotPointer(pEntity) != nullptr;

			if (!bSenderIsBot || gBotGlobals.IsConfigSettingOn(BOT_CONFIG_CHAT_REPLY_TO_BOTS))
			{
				// team only message?
				const int iTeamOnly = bSayTeamMsg;

				char* szMessage = nullptr;
				//				char *szTempArgument;

				if (arg2 && *arg2)
				{
					// argh! someone said something in series of arguments. work out the message
					int i = 1;
					int iLenSoFar = 0;
					// for concatenating string dynamically
					char* szTemp = nullptr;
					BOOL bWasQuote = false;

					const char* (*CmdArgv_func)(int);
					int iArgCount;

					if (bSenderIsBot)
					{
						CmdArgv_func = pfnCmd_Argv;
						iArgCount = pfnCmd_Argc();
					}
					else
					{
						CmdArgv_func = CMD_ARGV;
						iArgCount = CMD_ARGC();
					}

					while (i < iArgCount)
					{
						const char* szArgument = CmdArgv_func(i);

						if (!szArgument || !*szArgument)
						{
							i++;
							continue;
						}

						iLenSoFar += std::strlen(szArgument) + 1;

						// read a string already
						if (szMessage)
						{
							szTemp = strdup(szMessage);
							std::free(szMessage);
							szMessage = nullptr;
						}

						// 2 extra chars, 1 for terminator and 1 for space
						szMessage = new char[iLenSoFar + 1];
						szMessage[0] = 0;

						// copy old string
						if (szTemp)
						{
							BOOL bIsQuote = false;

							// if not a bot sending message, then the ' quotes can seperate words
							// so can spaces argh :-@
							if (!bSenderIsBot)
							{
								bIsQuote = szArgument[0] == '\'' && szArgument[1] == 0;
							}
							if (bIsQuote || bWasQuote)
							{
								std::sprintf(szMessage, "%s%s", szTemp, szArgument);

								bWasQuote = !bWasQuote;
							}
							else
							{
								// take space into count (thats what is seperating these words)
								std::sprintf(szMessage, "%s %s", szTemp, szArgument);
							}

							std::free(szTemp);
							szTemp = nullptr;
						}
						else
						{
							std::strcat(szMessage, szArgument);
						}
						szMessage[iLenSoFar] = 0;

						i++;
					}
				}
				else
					szMessage = strdup(arg1);

				if (szMessage)
				{
					for (int i = 0; i < MAX_PLAYERS; i++)
					{
						CBot* pBot = &gBotGlobals.m_Bots[i];

						if (pBot && pBot->IsUsed())
						{
							// say message, everyone can see, team message only team mates can see
							if (bSayTeamMsg && UTIL_GetTeam(pEntity) == pBot->GetTeam() || bSayMsg)
							{
								pBot->ReplyToMessage(szMessage, pEntity, iTeamOnly);
							}
						}
					}

					// finished with message
					std::free(szMessage);

					szMessage = nullptr;
				}
			}
		}
	}
	// Let the menuselect command go through
	// make sure the command has "rcbot" in front to call bot commands
	// and make sure the listen server edict will use the ServerCommand function instead
	// of this.
	else if (FStrEq(pcmd, "menuselect"))
	{
		gBotGlobals.m_CurrentHandledCvar = gBotGlobals.m_BotCvars.GetCvar(pcmd);

		if (gBotGlobals.m_CurrentHandledCvar)
		{
			if (pClient == nullptr)
			{
				BugMessage(pEntity, "Your client was not found in list of clients");

				FreeArgs(pcmd, arg1, arg2, arg3, arg4, arg5);

#ifdef RCBOT_META_BUILD
				RETURN_META(MRES_SUPERCEDE);
#else
				return;
#endif
			}

			iState = gBotGlobals.m_CurrentHandledCvar->action(pClient, arg1, arg2, arg3, arg4);
		}
	}
	//////////////////////////////////////////
	// little force grip thing, a bit messy!!!
	else if (FStrEq(pcmd, "+rcbot_force_grip"))
	{
		if (iAccessLevel < RCBOT_ACCESS_FORCE_GRIP)
			iState = BOT_CVAR_NEEDACCESS;
		else
		{
			pClient->m_pForceGripEntity = UTIL_FacingEnt(pClient->GetPlayer());

			iState = BOT_CVAR_ACCESSED;
		}
	}
	else if (FStrEq(pcmd, "-rcbot_force_grip"))
	{
		if (iAccessLevel < RCBOT_ACCESS_FORCE_GRIP)
			iState = BOT_CVAR_NEEDACCESS;
		else
		{
			if (pClient->m_pForceGripEntity)
			{
				pClient->m_pForceGripEntity->v.flags &= ~FL_FROZEN;

				DROP_TO_FLOOR(pClient->m_pForceGripEntity);
			}

			pClient->m_pForceGripEntity = nullptr;

			iState = BOT_CVAR_ACCESSED;
		}
	}
	else if (FStrEq(pcmd, BOT_COMMAND_ACCESS) &&
		(!IS_DEDICATED_SERVER() && pEntity != gBotGlobals.m_pListenServerEdict ||
			IS_DEDICATED_SERVER()))
	{
		gBotGlobals.m_CurrentHandledCvar = gBotGlobals.m_BotCvars.GetCvar(arg1);

		if (gBotGlobals.m_CurrentHandledCvar)
		{
			if (pClient == nullptr)
			{
				BugMessage(pEntity, "Your client was not found in list of clients");

				FreeArgs(pcmd, arg1, arg2, arg3, arg4, arg5);
#ifdef RCBOT_META_BUILD
				RETURN_META(MRES_SUPERCEDE);
#else
				return;
#endif
			}

			//if ( pEntity == gBotGlobals.m_pListenServerEdict )
			//	iAccessLevel = 10;
			//else
			// GET ACCESS LEVEL FROM CLIENTS

			if (gBotGlobals.m_CurrentHandledCvar->needAccess(iAccessLevel))
				iState = BOT_CVAR_NEEDACCESS;
			else
				iState = gBotGlobals.m_CurrentHandledCvar->action(pClient, arg1, arg2, arg3, arg4);
		}
		else
			iState = BOT_CVAR_NOTEXIST;
	}

	FreeArgs(pcmd, arg1, arg2, arg3, arg4, arg5);

	if (iState == BOT_CVAR_ERROR || iState == BOT_CVAR_ACCESSED || iState == BOT_CVAR_NEEDACCESS || iState == BOT_CVAR_NOTEXIST)
	{
		if (iState == BOT_CVAR_NEEDACCESS)
		{
			BotMessage(pEntity, 0, "You do not have access to this command...");

			if (pClient)
			{
				if (!pClient->HasToolTipSent(BOT_TOOL_TIP_ADMIN_HELP))
				{
					gBotGlobals.SayToolTip(pEntity, BOT_TOOL_TIP_ADMIN_HELP);
				}
			}
		}
		else if (iState == BOT_CVAR_ERROR)
			BotMessage(pEntity, 0, "Error accessing command...");
		else if (iState == BOT_CVAR_NOTEXIST)
			BotMessage(pEntity, 0, "No command entered or bot command does not exist!");

#ifdef RCBOT_META_BUILD
		RETURN_META(MRES_SUPERCEDE);
#else
		return;
#endif
	}

	if (pcmd[0] != 0 && std::strstr(pcmd, "addbot") != nullptr)
	{
		BotMessage(pEntity, 0, "Tip: If you want to add an RCBOT use the command \"rcbot addbot\"\n");
	}

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnClientCommand)(pEntity);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ClientUserInfoChanged(edict_t* pEntity, char* infobuffer)
{
	if (debug_engine) {
		std::FILE* fp = std::fopen("bot.txt", "a");
		std::fprintf(fp, "ClientUserInfoChanged: pEntity=%x infobuffer=%s\n", reinterpret_cast<unsigned>(pEntity), infobuffer);
		std::fclose(fp);
	}

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnClientUserInfoChanged)(pEntity, infobuffer);
#endif
}

void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	std::memset(gBotGlobals.m_iJoiningClients, 0, sizeof(int) * MAX_PLAYERS);

	//	gBotGlobals.m_iJoiningClients = 0;
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnServerActivate)(pEdictList, edictCount, clientMax);
#endif
}
///////////////////////////////////////////////////////////////////////////
void ServerDeactivate()
{
	// server has finished (map changed for example) but new map
	// hasn't loaded yet!!

	// free our memory, I call it local meaning not everywhere,
	// since we are only changing maps, not quitting the game
	// usually when this function is called...
	gBotGlobals.FreeLocalMemory();

	gBotGlobals.m_bNetMessageStarted = false;
	gBotGlobals.m_fBotRejoinTime = 0.0f;
	gBotGlobals.m_fClientUpdateTime = 0.0f;
	gBotGlobals.m_bBotCanRejoin = false;
	gBotGlobals.m_fReadConfigTime = 0.0f;
	gBotGlobals.m_Squads.FreeMemory();
	gBotGlobals.m_fNextJoinTeam = 0.0f;

	gBotGlobals.saveLearnedData();

	// mark the bots as needing to be rejoined next game...
	for (int iIndex = 0; iIndex < MAX_PLAYERS; iIndex++)
	{
		CBot* pBot = &gBotGlobals.m_Bots[iIndex];

		// Respawn left game when bot wants to leave game
		// initialize bot so that it wont re-use the profile (hopefully)
		if (pBot->m_iRespawnState == RESPAWN_LEFT_GAME)
		{
			pBot->Init();
			continue;
		}

		// took a very short time for bots to be kicked from server
		// when server was deactivated
		// assume that bots were just kicked because the server changed map
		// and let them reconnect upon map change.
		if (pBot->m_fKickTime + 0.5f >= gpGlobals->time)
			pBot->m_iRespawnState = RESPAWN_NEED_TO_REJOIN;
	}

	WaypointVisibility.ClearVisibilityTable();

	gBotGlobals.m_iNumBots = 0;
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnServerDeactivate)();
#endif
}
///////////////////////////////////////////////////////////////////////////
void PlayerPreThink(edict_t* pEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	try
	{
		(*other_gFunctionTable.pfnPlayerPreThink)(pEntity);
	}

	catch (...)
	{
		return;
	}
#endif
}

void PlayerPostThink(edict_t* pEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnPlayerPostThink)(pEntity);
#endif
}

void StartFrame()
{
	gBotGlobals.StartFrame();

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else

	(*other_gFunctionTable.pfnStartFrame)();

#endif
}

void ParmsNewLevel()
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnParmsNewLevel)();
#endif
}

void ParmsChangeLevel()
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnParmsChangeLevel)();
#endif
}

const char* GetGameDescription()
{
	try
	{
#ifdef RCBOT_META_BUILD
		RETURN_META_VALUE(MRES_IGNORED, NULL);
#else
		return (*other_gFunctionTable.pfnGetGameDescription)();
#endif
	}

	catch (...)
	{
		BotMessage(nullptr, 2, "Error: bot detected you are running the wrong Rcbot DLL file (e.g. metamod instead of standalone)");
		//exit(0);
	}

	return nullptr;
}

void PlayerCustomization(edict_t* pEntity, customization_t* pCust)
{
	if (debug_engine) {
		std::FILE* fp = std::fopen("bot.txt", "a");
		std::fprintf(fp, "PlayerCustomization: %x\n", reinterpret_cast<unsigned>(pEntity));
		std::fclose(fp);
	}

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnPlayerCustomization)(pEntity, pCust);
#endif
}

void SpectatorConnect(edict_t* pEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSpectatorConnect)(pEntity);
#endif
}

void SpectatorDisconnect(edict_t* pEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSpectatorDisconnect)(pEntity);
#endif
}

void SpectatorThink(edict_t* pEntity)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSpectatorThink)(pEntity);
#endif
}

void Sys_Error(const char* error_string)
{
	char szError[1024];

	std::sprintf(szError, "System Error: \"%s\"\n", error_string);
	szError[1023] = 0;

	BotFile_Write(szError);

#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSys_Error)(error_string);
#endif
}

void PM_Move(playermove_s* ppmove, int server)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnPM_Move)(ppmove, server);
#endif
}

void PM_Init(playermove_s* ppmove)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnPM_Init)(ppmove);
#endif
}

char PM_FindTextureType(char* name)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnPM_FindTextureType)(name);
#endif
}

void SetupVisibility(edict_t* pViewEntity, edict_t* pClient, unsigned char** pvs, unsigned char** pas)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnSetupVisibility)(pViewEntity, pClient, pvs, pas);
#endif
}

void UpdateClientData(const edict_s* ent, int sendweapons, clientdata_s* cd)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnUpdateClientData)(ent, sendweapons, cd);
#endif
}

int AddToFullPack(entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnAddToFullPack)(state, e, ent, host, hostflags, player, pSet);
#endif
}

void CreateBaseline(int player, int eindex, entity_state_s* baseline, edict_s* entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnCreateBaseline)(player, eindex, baseline, entity, playermodelindex, player_mins, player_maxs);
#endif
}

void RegisterEncoders()
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnRegisterEncoders)();
#endif
}

int GetWeaponData(edict_s* player, weapon_data_s* info)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnGetWeaponData)(player, info);
#endif
}

void CmdStart(const edict_t* player, const usercmd_s* cmd, unsigned int random_seed)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnCmdStart)(player, cmd, random_seed);
#endif
}

void CmdEnd(const edict_t* player)
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnCmdEnd)(player);
#endif
}

int ConnectionlessPacket(const netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnConnectionlessPacket)(net_from, args, response_buffer, response_buffer_size);
#endif
}

int GetHullBounds(int hullnumber, float* mins, float* maxs)
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnGetHullBounds)(hullnumber, mins, maxs);
#endif
}

void CreateInstancedBaselines()
{
#ifdef RCBOT_META_BUILD
	RETURN_META(MRES_IGNORED);
#else
	(*other_gFunctionTable.pfnCreateInstancedBaselines)();
#endif
}

int InconsistentFile(const edict_t* player, const char* filename, char* disconnect_message)
{
	if (debug_engine) {
		std::FILE* fp = std::fopen("bot.txt", "a");
		std::fprintf(fp, "InconsistentFile: %x filename=%s\n", reinterpret_cast<unsigned>(player), filename);
		std::fclose(fp);
	}

#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnInconsistentFile)(player, filename, disconnect_message);
#endif
}

int AllowLagCompensation()
{
#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else
	return (*other_gFunctionTable.pfnAllowLagCompensation)();
#endif
}

DLL_FUNCTIONS gFunctionTable =
{
   GameDLLInit,               //pfnGameInit
   DispatchSpawn,             //pfnSpawn
   DispatchThink,             //pfnThink
   DispatchUse,               //pfnUse
   DispatchTouch,             //pfnTouch
   DispatchBlocked,           //pfnBlocked
   DispatchKeyValue,          //pfnKeyValue
   DispatchSave,              //pfnSave
   DispatchRestore,           //pfnRestore
   DispatchObjectCollsionBox, //pfnAbsBox

   SaveWriteFields,           //pfnSaveWriteFields
   SaveReadFields,            //pfnSaveReadFields

   SaveGlobalState,           //pfnSaveGlobalState
   RestoreGlobalState,        //pfnRestoreGlobalState
   ResetGlobalState,          //pfnResetGlobalState

   ClientConnect,             //pfnClientConnect
   ClientDisconnect,          //pfnClientDisconnect
   ClientKill,                //pfnClientKill
   ClientPutInServer,         //pfnClientPutInServer
   ClientCommand,             //pfnClientCommand
   ClientUserInfoChanged,     //pfnClientUserInfoChanged
   ServerActivate,            //pfnServerActivate
   ServerDeactivate,          //pfnServerDeactivate

   PlayerPreThink,            //pfnPlayerPreThink
   PlayerPostThink,           //pfnPlayerPostThink

   StartFrame,                //pfnStartFrame
   ParmsNewLevel,             //pfnParmsNewLevel
   ParmsChangeLevel,          //pfnParmsChangeLevel

   GetGameDescription,        //pfnGetGameDescription    Returns string describing current .dll game.
   PlayerCustomization,       //pfnPlayerCustomization   Notifies .dll of new customization for player.

   SpectatorConnect,          //pfnSpectatorConnect      Called when spectator joins server
   SpectatorDisconnect,       //pfnSpectatorDisconnect   Called when spectator leaves the server
   SpectatorThink,            //pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)

   Sys_Error,                 //pfnSys_Error          Called when engine has encountered an error

   PM_Move,                   //pfnPM_Move
   PM_Init,                   //pfnPM_Init            Server version of player movement initialization
   PM_FindTextureType,        //pfnPM_FindTextureType

   SetupVisibility,           //pfnSetupVisibility        Set up PVS and PAS for networking for this client
   UpdateClientData,          //pfnUpdateClientData       Set up data sent only to specific client
   AddToFullPack,             //pfnAddToFullPack
   CreateBaseline,            //pfnCreateBaseline        Tweak entity baseline for network encoding, allows setup of player baselines, too.
   RegisterEncoders,          //pfnRegisterEncoders      Callbacks for network encoding
   GetWeaponData,             //pfnGetWeaponData
   CmdStart,                  //pfnCmdStart
   CmdEnd,                    //pfnCmdEnd
   ConnectionlessPacket,      //pfnConnectionlessPacket
   GetHullBounds,             //pfnGetHullBounds
   CreateInstancedBaselines,  //pfnCreateInstancedBaselines
   InconsistentFile,          //pfnInconsistentFile
   AllowLagCompensation,      //pfnAllowLagCompensation
};

#ifndef RCBOT_META_BUILD

#ifdef __BORLANDC__
int EXPORT GetEntityAPI(DLL_FUNCTIONS* pFunctionTable, int interfaceVersion)
#else
extern "C" EXPORT int GetEntityAPI(DLL_FUNCTIONS * pFunctionTable, int interfaceVersion)
#endif
{
	// check if engine's pointer is valid and version is correct...

	if (!pFunctionTable || interfaceVersion != INTERFACE_VERSION)
		return false;

	// pass engine callback function table to engine...
	std::memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	// pass other DLLs engine callbacks to function table...
	if (!(*other_GetEntityAPI)(&other_gFunctionTable, INTERFACE_VERSION))
	{
		return false;  // error initializing function table!!!
	}

	return true;
}

#ifdef __BORLANDC__
int EXPORT GetNewDLLFunctions(NEW_DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
#else
extern "C" EXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS * pFunctionTable, int* interfaceVersion)
#endif
{
	if (other_GetNewDLLFunctions == NULL)
		return false;

	// pass other DLLs engine callbacks to function table...
	if (!(*other_GetNewDLLFunctions)(pFunctionTable, interfaceVersion))
	{
		return false;  // error initializing function table!!!
	}

	return true;
}

#endif

// Fakeclient Command code
//
// by Pierre-Marie Baty
// racc bot (Check it out! : racc.bots-united.com)

void FakeClientCommand(edict_t* pFakeClient, const char* fmt, ...)
{
	// the purpose of this function is to provide fakeclients (bots) with the same client
	// command-scripting advantages (putting multiple commands in one line between semicolons)
	// as real players. It is an improved version of botman's FakeClientCommand, in which you
	// supply directly the whole string as if you were typing it in the bot's "console". It
	// is supposed to work exactly like the pfnClientCommand (server-sided client command).

	va_list argptr;
	static char command[256];
	int fieldstop, i, stringindex = 0;

	if (!pFakeClient)
	{
		BugMessage(nullptr, "FakeClientCommand : No fakeclient!");
		return; // reliability check
	}

	if (fmt == nullptr)
		return;

	// concatenate all the arguments in one string
	va_start(argptr, fmt);
	std::vsprintf(command, fmt, argptr);
	va_end(argptr);

	if (*command == 0 || *command == '\n')
	{
		BugMessage(nullptr, "FakeClientCommand : No command!");
		return; // if nothing in the command buffer, return
	}

	gBotGlobals.m_bIsFakeClientCommand = true; // set the "fakeclient command" flag
	const int length = std::strlen(command); // get the total length of the command string

	// process all individual commands (separated by a semicolon) one each a time
	while (stringindex < length)
	{
		const int fieldstart = stringindex; // save field start position (first character)
		while (stringindex < length && command[stringindex] != ';')
			stringindex++; // reach end of field
		if (command[stringindex - 1] == '\n')
			fieldstop = stringindex - 2; // discard any trailing '\n' if needed
		else
			fieldstop = stringindex - 1; // save field stop position (last character before semicolon or end)
		for (i = fieldstart; i <= fieldstop; i++)
			g_argv[i - fieldstart] = command[i]; // store the field value in the g_argv global string
		g_argv[i - fieldstart] = 0; // terminate the string
		stringindex++; // move the overall string index one step further to bypass the semicolon

		int index = 0;
		gBotGlobals.m_iFakeArgCount = 0; // let's now parse that command and count the different arguments

		// count the number of arguments
		while (index < i - fieldstart)
		{
			while (index < i - fieldstart && g_argv[index] == ' ')
				index++; // ignore spaces

			// is this field a group of words between quotes or a single word ?
			if (g_argv[index] == '"')
			{
				index++; // move one step further to bypass the quote
				while (index < i - fieldstart && g_argv[index] != '"')
					index++; // reach end of field
				index++; // move one step further to bypass the quote
			}
			else
				while (index < i - fieldstart && g_argv[index] != ' ')
					index++; // this is a single word, so reach the end of field

			gBotGlobals.m_iFakeArgCount++; // we have processed one argument more
		}
#ifdef RCBOT_META_BUILD
		MDLL_ClientCommand(pFakeClient);
#else
		ClientCommand(pFakeClient); // tell now the MOD DLL to execute this ClientCommand...
#endif
	}

	g_argv[0] = 0; // when it's done, reset the g_argv field
	gBotGlobals.m_bIsFakeClientCommand = false; // reset the "fakeclient command" flag
	gBotGlobals.m_iFakeArgCount = 0; // and the argument count
}

void BotFunc_InitProfile(bot_profile_t* bpBotProfile)
// Initalizes a bot profile structure/type
{
	bpBotProfile->m_szModel = nullptr;
	bpBotProfile->m_iClass = -1;
	bpBotProfile->m_FadePercent = 0;
	bpBotProfile->m_GorgePercent = 0;
	bpBotProfile->m_iFavMod = 0;
	bpBotProfile->m_iFavTeam = TEAM_AUTOTEAM;
	bpBotProfile->m_iClass = -1;
	bpBotProfile->m_iProfileId = 0;
	bpBotProfile->m_iSkill = DEF_BOT_SKILL;
	bpBotProfile->m_LerkPercent = 50;
	bpBotProfile->m_OnosPercent = 50;

	bpBotProfile->m_szBotName = gBotGlobals.m_Strings.GetString(BOT_DEFAULT_NAME);
	bpBotProfile->m_szFavMap = nullptr;
	bpBotProfile->m_szSpray = nullptr;
	bpBotProfile->m_iNumGames = 0;

	bpBotProfile->m_HAL = nullptr;
	bpBotProfile->m_szHAL_AuxFile = nullptr;
	bpBotProfile->m_szHAL_BanFile = nullptr;
	bpBotProfile->m_szHAL_PreTrainFile = nullptr;
	bpBotProfile->m_szHAL_SwapFile = nullptr;

	bpBotProfile->m_fAimSpeed = 0.2f;
	bpBotProfile->m_fAimSkill = 0.0f;
	bpBotProfile->m_fAimTime = 1.0f;

	bpBotProfile->m_iBottomColour = RANDOM_LONG(0, 255);
	bpBotProfile->m_iTopColour = RANDOM_LONG(0, 255);

	bpBotProfile->m_iPathRevs = gBotGlobals.m_iMaxPathRevs;
	bpBotProfile->m_iVisionRevs = gBotGlobals.m_iMaxVisUpdateRevs;
	bpBotProfile->m_fVisionTime = gBotGlobals.m_fUpdateVisTime;

	bpBotProfile->m_Rep.Destroy();
}

void BotFunc_WriteProfile(std::FILE* fp, bot_profile_t* bpBotProfile)
// Writes a profile onto file
{
	int i = 0;

	while (i <= 10)
	{
		char* szTag = nullptr;
		char* szToWrite = nullptr;
		const int* iToWrite = nullptr;

		switch (i)
		{
		case 0:
			szTag = BOT_PROFILE_BOT_NAME;
			szToWrite = bpBotProfile->m_szBotName;
			break;
		case 1:
			szTag = BOT_PROFILE_FAVMOD;
			iToWrite = &bpBotProfile->m_iFavMod;
			break;
		case 2:
			szTag = BOT_PROFILE_FAVTEAM;
			iToWrite = &bpBotProfile->m_iFavTeam;
			break;
		case 3:
			szTag = BOT_PROFILE_FAVMAP;
			szToWrite = bpBotProfile->m_szFavMap;
			break;
		case 4:
			szTag = BOT_PROFILE_SKILL;
			iToWrite = &bpBotProfile->m_iSkill;
			break;
		case 5:
			szTag = BOT_PROFILE_SPRAY;
			szToWrite = bpBotProfile->m_szSpray;
			break;
		case 6:
			szTag = BOT_PROFILE_GORGE_PERCENT;
			iToWrite = &bpBotProfile->m_GorgePercent;
			break;
		case 7:
			szTag = BOT_PROFILE_LERK_PERCENT;
			iToWrite = &bpBotProfile->m_LerkPercent;
			break;
		case 8:
			szTag = BOT_PROFILE_FADE_PERCENT;
			iToWrite = &bpBotProfile->m_FadePercent;
			break;
		case 9:
			szTag = BOT_PROFILE_ONOS_PERCENT;
			iToWrite = &bpBotProfile->m_OnosPercent;
			break;
		case 10:
			szTag = BOT_PROFILE_NUMGAMES;
			iToWrite = &bpBotProfile->m_iNumGames;
			break;
		}

		i++;

		if (!szTag)
		{
			BugMessage(nullptr, "Error writing bot profile, tag missing");
			continue;
		}

		if (szToWrite)
			std::fprintf(fp, "%s\"%s\"\n", szTag, szToWrite);
		else if (iToWrite)
			std::fprintf(fp, "%s%d\n", szTag, *iToWrite);
		else
		{
			std::fprintf(fp, "%s", szTag);
		}
	}

	bpBotProfile->m_Rep.SaveAllRep(bpBotProfile->m_iProfileId);

	SaveHALBrainForPersonality(bpBotProfile);
}

void BotFunc_ReadProfile(std::FILE* fp, bot_profile_t* bpBotProfile)
{
	char szBuffer[128];

	int i;

	char szTemp[64];
	int j;

	// true when the bot needs to read pretraining file for megahal

	// read bot profile with bots name etc on it.

	while (std::fgets(szBuffer, 127, fp))
	{
		szBuffer[127] = '\0';

		if (szBuffer[0] == '#')
			continue;

		int iLength = std::strlen(szBuffer);

		if (szBuffer[iLength - 1] == '\n')
			szBuffer[--iLength] = '\0';

		if (std::strncmp(szBuffer, "name=", 5) == 0)
		{
			j = 0;
			i = 6;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szBotName = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "class=", 6) == 0)
		{
			bpBotProfile->m_iClass = std::atoi(&szBuffer[6]);
		}
		else if (std::strncmp(szBuffer, "model=", 6) == 0)
		{
			j = 0;
			i = 6;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szModel = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "favmod=", 7) == 0)
		{
			bpBotProfile->m_iFavMod = std::atoi(&szBuffer[7]);
		}
		else if (std::strncmp(szBuffer, "favteam=", 8) == 0)
		{
			bpBotProfile->m_iFavTeam = std::atoi(&szBuffer[8]);
		}
		else if (std::strncmp(szBuffer, "favmap=", 7) == 0)
		{
			j = 0;
			i = 8;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szFavMap = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "skill=", 6) == 0)
		{
			bpBotProfile->m_iSkill = std::atoi(&szBuffer[6]);
		}
		else if (std::strncmp(szBuffer, "spray=", 6) == 0)
		{
			j = 0;
			i = 7;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szSpray = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "gorge_percent=", 14) == 0)
		{
			bpBotProfile->m_GorgePercent = std::atoi(&szBuffer[14]);
		}
		else if (std::strncmp(szBuffer, "lerk_percent=", 13) == 0)
		{
			bpBotProfile->m_LerkPercent = std::atoi(&szBuffer[13]);
		}
		else if (std::strncmp(szBuffer, "fade_percent=", 13) == 0)
		{
			bpBotProfile->m_FadePercent = std::atoi(&szBuffer[13]);
		}
		else if (std::strncmp(szBuffer, "onos_percent=", 13) == 0)
		{
			bpBotProfile->m_OnosPercent = std::atoi(&szBuffer[13]);
		}
		else if (std::strncmp(szBuffer, "aim_speed=", 10) == 0)
			bpBotProfile->m_fAimSpeed = std::stof(&szBuffer[10]);
		else if (std::strncmp(szBuffer, "aim_skill=", 10) == 0)
			bpBotProfile->m_fAimSkill = std::stof(&szBuffer[10]);
		else if (std::strncmp(szBuffer, "aim_time=", 9) == 0)
			bpBotProfile->m_fAimTime = std::stof(&szBuffer[9]);
		else if (std::strncmp(szBuffer, "bottomcolor=", 12) == 0)
		{
			bpBotProfile->m_iBottomColour = std::atoi(&szBuffer[12]);

			if (bpBotProfile->m_iBottomColour < 0)
				bpBotProfile->m_iBottomColour = 0;
			if (bpBotProfile->m_iBottomColour > 255)
				bpBotProfile->m_iBottomColour = 255;
		}
		else if (std::strncmp(szBuffer, "topcolor=", 9) == 0)
		{
			bpBotProfile->m_iTopColour = std::atoi(&szBuffer[9]);

			if (bpBotProfile->m_iTopColour < 0)
				bpBotProfile->m_iTopColour = 0;
			if (bpBotProfile->m_iTopColour > 255)
				bpBotProfile->m_iTopColour = 255;
		}
		else if (std::strncmp(szBuffer, "numgames=", 9) == 0)
		{
			bpBotProfile->m_iNumGames = std::atoi(&szBuffer[9]);
		}
		else if (std::strncmp(szBuffer, "hal_pretrain_file=", 18) == 0)
		{
			j = 0;
			i = 19;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szHAL_PreTrainFile = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "hal_aux_file=", 13) == 0)
		{
			j = 0;
			i = 14;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szHAL_AuxFile = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "hal_ban_file=", 13) == 0)
		{
			j = 0;
			i = 14;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szHAL_BanFile = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "hal_swap_file=", 14) == 0)
		{
			j = 0;
			i = 15;

			while (i < iLength && szBuffer[i] != '\"')
				szTemp[j++] = szBuffer[i++];

			szTemp[j] = 0;

			bpBotProfile->m_szHAL_SwapFile = gBotGlobals.m_Strings.GetString(szTemp);
		}
		else if (std::strncmp(szBuffer, "max_path_revs=", 14) == 0)
		{
			bpBotProfile->m_iPathRevs = std::atoi(&szBuffer[14]);
		}
		else if (std::strncmp(szBuffer, "max_update_vision_revs=", 23) == 0)
		{
			bpBotProfile->m_iVisionRevs = std::atoi(&szBuffer[23]);
		}
		else if (std::strncmp(szBuffer, "update_vision_time=", 19) == 0)
		{
			bpBotProfile->m_fVisionTime = std::stof(&szBuffer[19]);
		}
	}

	const BOOL bPreTrain = PrepareHALBrainForPersonality(bpBotProfile); // check the bot HAL brain
	LoadHALBrainForPersonality(bpBotProfile, bPreTrain); // wake the bot's HAL brain up

	// Also read bots rep with other players on the server

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		const CClient* pClient = gBotGlobals.m_Clients.GetClientByIndex(i);

		if (pClient->IsUsed())
		{
			bpBotProfile->m_Rep.AddLoadRep(bpBotProfile->m_iProfileId, pClient->GetPlayerRepId());
		}
	}
}

void ReadBotUsersConfig()
// Read the allowed users to use bot commands
{
	char filename[256];

	UTIL_BuildFileName(filename, BOT_USERS_FILE, nullptr);

	std::FILE* fp = std::fopen(filename, "r");

	if (fp != nullptr)
	{
		char buffer[256];

		char szName[64];
		char szPass[BOT_MAX_PASSWORD_LEN];
		char szAccessLevel[8];
		char szSteamId[STEAM_ID_LEN];

		int line = 0;
		int users = 0;

		while (std::fgets(buffer, 255, fp) != nullptr)
		{
			line++;

			buffer[255] = 0;

			int length = std::strlen(buffer);

			if (buffer[0] == '#') // comment
				continue;

			if (buffer[length - 1] == '\n')
			{
				buffer[length - 1] = 0;  // remove '\n'
				length--;
			}

			if (length == 0) // nothing on line
				continue;

			int i = 0;

			while (i < length && buffer[i] != '"')
				i++;
			i++;

			int j = 0;

			while (i < length && buffer[i] != '"' && j < 64)
				szName[j++] = buffer[i++];
			szName[j] = 0;
			i++;

			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;
			while (i < length && buffer[i] != ' ' && j < 16)
				szPass[j++] = buffer[i++];
			szPass[j] = 0;

			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;
			while (i < length && buffer[i] != ' ' && j < 8)
				szAccessLevel[j++] = buffer[i++];
			szAccessLevel[j] = 0;

			// skip spaces...
			while (i < length && buffer[i] == ' ')
				i++;

			j = 0;
			while (i < length && buffer[i] != ' ' && j < STEAM_ID_LEN)
				szSteamId[j++] = buffer[i++];
			szSteamId[j] = 0;

			if (*szName && *szPass && *szAccessLevel || *szSteamId && *szAccessLevel)
			{
				BotMessage(nullptr, 0, "Added: name=\"%s\", pass=%s, accesslev=%s, steamid=%s", szName, szPass, szAccessLevel, szSteamId);

				// Add to allowed users
				gBotGlobals.m_BotUsers.AddPlayer(szName, szPass, std::atoi(szAccessLevel), szSteamId);
				users++;
			}
			else
			{
				BotMessage(nullptr, 0, "Error in bot_users.ini, error on line %d (bad format?)", line);
			}
		}

		BotMessage(nullptr, 0, "%d users added to list of RCBot users", users);

		std::fclose(fp);
	}
	else
		BotMessage(nullptr, 0, "!!! Cannot find bot_users.ini !!!! (trying to look in %s - not found) permissions are bad or rcbot is not installed properly", filename);
}

void ReadMapConfig()
// Reads a config file for the current map
// this function is called many times so that server commands
// are called every so often (only if m_fReadConfigTime < gpGlobals->time )
// this is so that when bots are added it has some time to add another bot
// thus this function only reads one line of the file until the file has reached the end
// and so the file is stored globally.
// To be done properly:

/*

  TODO:

  Make config read whole file and enter commands in a queue for execution (faster??)

  */
{
	char szTemp[256];

	if (std::fgets(szTemp, 255, fpMapConfig) != nullptr)
	{
		if (*szTemp == '#')
			return;

		size_t iLen = std::strlen(szTemp);

		if (iLen > 255)
			szTemp[255] = 0;
		if (szTemp[iLen - 1] != '\n')
		{
			szTemp[iLen] = '\n';
			szTemp[++iLen] = 0;
		}
		if (iLen == 0)
			return;

		if (std::strncmp(szTemp, "rcbot addbot", 12) == 0)
		{
			// dont add bots for another while...
			// Added more delay to prevent possible "Tried to write to uninitialized sizebuf_t" crashes - [APG]RoboCop[CL]
			gBotGlobals.m_fBotRejoinTime = gpGlobals->time + 10.0f;
			gBotGlobals.m_fReadConfigTime = gpGlobals->time + 10.0f;
		}

		// Does the command in the text file
		// -------
		// if its an rcbot command then SERVER COMMAND,
		// will call the rcbot server command function
		// -------
		SERVER_COMMAND(szTemp);
	}
	else
	{
		std::fclose(fpMapConfig);
		fpMapConfig = nullptr;
	}
}

edict_t* BotFunc_NS_CommanderBuild(int iUser3, const char* szClassname, const Vector& vOrigin) //TODO: Experimental [APG]RoboCop[CL]
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (!pPlayer)
			continue;

		if (pPlayer->free)
			continue;

		if (pPlayer->v.iuser3 == iUser3 && std::strcmp(STRING(pPlayer->v.classname), szClassname) == 0)
		{
			pPlayer->v.origin = vOrigin;
			return pPlayer;
		}
	}

	return nullptr;
}
//
// Hack building
edict_t* BotFunc_NS_MarineBuild(int iUser3, const char* szClassname, Vector vOrigin, edict_t* pEntityUser, BOOL bBuilt)
{
	//pfnCreateNamedEntity(MAKE_STRING(pCommBuildent));

	edict_t* pSetgroundentity = nullptr;

	if (iUser3 == AVH_USER3_RESTOWER)
	{
		// find nearest struct resource fountain
		char* classname[1] = { "func_resource" };

		edict_t* pResource = UTIL_FindNearestEntity(classname, 1, vOrigin, 200, false);

		if (pResource)
		{
			if (UTIL_IsResourceFountainUsed(pResource))
			{
				BotMessage(pEntityUser, 0, "Error: Nearest resource fountain is used");
				return nullptr;
			}

			pSetgroundentity = pResource;
			vOrigin = pResource->v.origin + Vector(0, 0, 1);
		}
		else
		{
			BotMessage(pEntityUser, 0, "Error: Can't find a resource fountain for tower");
			return nullptr;
		}
	}

	edict_t* build = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));

	if (build && !FNullEnt(build))
	{
		if (pSetgroundentity)
			build->v.groundentity = pSetgroundentity;

		SET_ORIGIN(build, vOrigin);

		build->v.iuser3 = iUser3;

		if (iUser3 != AVH_USER3_MARINEITEM)
		{
			build->v.iuser4 = MASK_BUILDABLE | MASK_SELECTABLE;
		}
		else
			build->v.iuser4 = MASK_NONE;

		build->v.takedamage = 1;
		build->v.movetype = 6;
		build->v.solid = 2;
		build->v.team = 1;
		build->v.flags = 544;

		if (bBuilt)
		{
			build->v.iuser4 = 536871936;
			build->v.flags &= ~MASK_BUILDABLE;
			build->v.fuser1 = 1000.0f;
			build->v.fuser2 = 1000.0f;

			if (iUser3 == AVH_USER3_INFANTRYPORTAL)
			{
				build->v.health = 2500.0f;
				build->v.max_health = 2500.0f;
			}
		}

		build->v.nextthink = gpGlobals->time + 0.1f;

#ifdef RCBOT_META_BUILD
		MDLL_Spawn(build);
#else
		DispatchSpawn(build);
#endif

		return build;
	}

	return nullptr;
}

/////////////////////////////////////////
// TODO: Make new BOTCAM.CPP soon!!!
CBotCam::CBotCam()
{
	//tr = nullptr;
	//vBotOrigin = nullptr;
	m_iPositionSet = 0;
	m_pCurrentBot = nullptr;
	m_iState = BOTCAM_NONE;
	m_pCameraEdict = nullptr;
	m_fNextChangeBotTime = 0.0f;
	m_fNextChangeState = 0.0f;
	m_bTriedToSpawn = false;
}

BOOL CBotCam::IsWorking() const
{
	return m_pCameraEdict != nullptr && gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ENABLE_BOTCAM);
}

void CBotCam::Spawn()
{
	if (m_bTriedToSpawn || !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ENABLE_BOTCAM))
		return;

	m_bTriedToSpawn = true;

	// Redfox http://www.foxbot.net
	m_pCameraEdict = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
	// /Redfox

	if (!FNullEnt(m_pCameraEdict))
	{
		// Redfox http://www.foxbot.net
		DispatchSpawn(m_pCameraEdict);

		SET_MODEL(m_pCameraEdict, "models/mechgibs.mdl");

		m_pCameraEdict->v.takedamage = DAMAGE_NO;
		m_pCameraEdict->v.solid = SOLID_NOT;
		m_pCameraEdict->v.movetype = MOVETYPE_FLY; //noclip
		m_pCameraEdict->v.classname = MAKE_STRING("entity_botcam");
		m_pCameraEdict->v.nextthink = gpGlobals->time;
		m_pCameraEdict->v.renderamt = 0;
		// /Redfox
	}
}

void CBotCam::Think()
{
	static BOOL bNotAlive;

	if (gBotGlobals.m_iNumBots == 0)
		return;

	bNotAlive = m_pCurrentBot && !m_pCurrentBot->IsAlive();

	if (m_pCurrentBot == nullptr || m_fNextChangeBotTime < gpGlobals->time || !m_pCurrentBot->IsUsed() || bNotAlive)
	{
		int i;
		CBot* pOldBot = m_pCurrentBot;
		CBot* pBot;

		if (bNotAlive)
		{
			float fNearest = 0.0f;

			// think about next bot to view etc
			m_fNextChangeBotTime = gpGlobals->time + RANDOM_FLOAT(7.5f, 10.0f);
			m_pCurrentBot = nullptr;

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				pBot = &gBotGlobals.m_Bots[i];

				if (pBot == pOldBot)
					continue;
				if (!pBot->IsUsed())
					continue;
				if (!pBot->IsAlive())
					continue;

				//if ( pOldBot )
				//	fDistance = pBot->DistanceFrom(pOldBot->GetGunPosition());
				//else
				const float fDistance = pBot->DistanceFrom(m_pCameraEdict->v.origin);

				if (m_pCurrentBot == nullptr || fDistance < fNearest)
				{
					m_pCurrentBot = pBot;
					fNearest = fDistance;

					if (pBot->m_pEnemy != nullptr)
						m_iState = BOTCAM_ENEMY;
					else
						m_iState = BOTCAM_BOT;
				}
			}
		}
		else
		{
			if (m_pCurrentBot && !m_pCurrentBot->IsUsed())
				m_pCurrentBot = nullptr;

			dataUnconstArray<int> iPossibleBots;

			// think about next bot to view etc
			m_fNextChangeBotTime = gpGlobals->time + RANDOM_FLOAT(7.5f, 10.0f);
			m_pCurrentBot = nullptr;

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				pBot = &gBotGlobals.m_Bots[i];
				if (pBot == pOldBot)
					continue;

				if (pBot->IsUsed())
					iPossibleBots.Add(i);
			}

			if (iPossibleBots.IsEmpty())
				m_pCurrentBot = pOldBot;
			else
			{
				m_pCurrentBot = &gBotGlobals.m_Bots[iPossibleBots.Random()];
				iPossibleBots.Clear();
			}
		}
	}

	if (m_pCurrentBot && (m_iState == BOTCAM_NONE || m_fNextChangeState < gpGlobals->time))
	{
		dataUnconstArray<eCamLookState> iPossibleStates;

		iPossibleStates.Add(BOTCAM_BOT);
		iPossibleStates.Add(BOTCAM_FP);

		if (m_pCurrentBot->m_iWaypointGoalIndex != -1)
		{
			if (m_pCurrentBot->DistanceFrom(WaypointOrigin(m_pCurrentBot->m_iWaypointGoalIndex)) < 512)
				iPossibleStates.Add(BOTCAM_WAYPOINT);
		}

		if (m_pCurrentBot->m_pEnemy)
			iPossibleStates.Add(BOTCAM_ENEMY);

		m_iState = iPossibleStates.Random();

		iPossibleStates.Clear();

		m_iPositionSet = -1;

		m_fNextChangeState = gpGlobals->time + RANDOM_FLOAT(4.0f, 6.0f);
	}

	if (!m_pCurrentBot || !m_iState)
		return;

	constexpr BOOL bSetAngle = true;

	vBotOrigin = m_pCurrentBot->pev->origin + m_pCurrentBot->pev->view_ofs;

	const Vector oldOrigin = m_pCameraEdict->v.origin;//

	//Vector vLookAt = vBotOrigin;

	switch (m_iState)
	{
	case BOTCAM_BOT:
	{
		if (m_pCurrentBot->m_pEnemy)
		{
			UTIL_MakeVectors(m_pCurrentBot->pev->v_angle);
			m_pCameraEdict->v.origin = vBotOrigin - gpGlobals->v_forward * 128;

			if (m_iPositionSet == -1)
				m_iPositionSet = RANDOM_LONG(0, 1);

			// looking from the right
			if (m_iPositionSet == 1)
				m_pCameraEdict->v.origin = m_pCameraEdict->v.origin + gpGlobals->v_right * 64.0f;
			else
				m_pCameraEdict->v.origin = m_pCameraEdict->v.origin + -gpGlobals->v_right * 64.0f;

			vBotOrigin = EntityOrigin(m_pCurrentBot->m_pEnemy);
		}
		else
		{
			UTIL_MakeVectors(m_pCurrentBot->pev->angles);
			m_pCameraEdict->v.origin = vBotOrigin - gpGlobals->v_forward * 256;
		}

		UTIL_TraceLine(vBotOrigin, m_pCameraEdict->v.origin, ignore_monsters, ignore_glass, m_pCameraEdict, &tr);

		if (tr.flFraction < 1.0f)
			m_pCameraEdict->v.origin = tr.vecEndPos;
	}
	break;
	case BOTCAM_WAYPOINT:
	{
		if (m_pCurrentBot->m_iWaypointGoalIndex != -1)
		{
			m_pCameraEdict->v.origin = WaypointOrigin(m_pCurrentBot->m_iWaypointGoalIndex);
			m_pCameraEdict->v.origin.z += 16.0f;
		}
		else
			m_fNextChangeState = 0.0f;
	}
	break;
	case BOTCAM_ENEMY:
	{
		if (m_pCurrentBot->m_pEnemy)
		{
			Vector vComp = m_pCurrentBot->m_pEnemy->v.origin + m_pCurrentBot->m_pEnemy->v.view_ofs - vBotOrigin;

			const float fLength = vComp.Length();

			vComp = vComp.Normalize();

			vComp = vComp * (fLength + 128.0f);

			m_pCameraEdict->v.origin = vBotOrigin + vComp;
			m_pCameraEdict->v.origin.z += 24.0f;

			if (m_iPositionSet == -1)
				m_iPositionSet = RANDOM_LONG(0, 1);

			// looking from the right
			if (m_iPositionSet == 1)
				m_pCameraEdict->v.origin = m_pCameraEdict->v.origin + CrossProduct(vComp.Normalize(), Vector(0, 0, 1)) * m_pCurrentBot->m_pEnemy->v.size.Length2D();
			else
				m_pCameraEdict->v.origin = m_pCameraEdict->v.origin + -CrossProduct(vComp.Normalize(), Vector(0, 0, 1)) * m_pCurrentBot->m_pEnemy->v.size.Length2D();

			//----
			UTIL_TraceLine(m_pCurrentBot->m_pEnemy->v.origin, m_pCameraEdict->v.origin, ignore_monsters, ignore_glass, m_pCameraEdict, &tr);

			if (tr.flFraction < 1.0f)
			{
				m_pCameraEdict->v.origin = tr.vecEndPos - vComp.Normalize() * 32.0f;
			}

			//bSetAngle = false;
		}
		else
			m_fNextChangeState = 0.0f;
	}
	break;
	case BOTCAM_FP:
		if (m_pCurrentBot)
		{
			//bSetAngle = false;
			m_pCameraEdict->v.origin = m_pCurrentBot->pev->origin + m_pCurrentBot->pev->view_ofs;

			UTIL_MakeVectors(m_pCurrentBot->pev->v_angle);
			vBotOrigin = m_pCameraEdict->v.origin + gpGlobals->v_forward * 512;

			m_pCameraEdict->v.angles = m_pCurrentBot->pev->angles;
			m_pCameraEdict->v.v_angle = m_pCurrentBot->pev->v_angle;
		}

		break;
	case BOTCAM_NONE:
		break;
	}

	if (bSetAngle)
	{
		Vector ideal = UTIL_VecToAngles(vBotOrigin - m_pCameraEdict->v.origin);
		UTIL_FixAngles(&ideal);

		float fTurnSpeed = std::fabs(180 + ideal.x - (180 + m_pCameraEdict->v.angles.x)) / 10;

		//	ideal.x = -ideal.x;
		BotFunc_ChangeAngles(&fTurnSpeed, &ideal.x, &m_pCameraEdict->v.v_angle.x, &m_pCameraEdict->v.angles.x);

		//	m_pCameraEdict->v.angles.x = -m_pCameraEdict->v.v_angle.x/3;
		//	m_pCameraEdict->v.v_angle.x = -m_pCameraEdict->v.v_angle.x;

		fTurnSpeed = std::fabs(180 + ideal.y - (180 + m_pCameraEdict->v.angles.y)) / 20;
		BotFunc_ChangeAngles(&fTurnSpeed, &ideal.y, &m_pCameraEdict->v.v_angle.y, &m_pCameraEdict->v.angles.y);

		m_pCameraEdict->v.origin = m_pCameraEdict->v.origin - (m_pCameraEdict->v.origin - oldOrigin) * 0.5f;
		/*
		m_pCameraEdict->v.angles = UTIL_VecToAngles(vBotOrigin - m_pCameraEdict->v.origin);

		m_pCameraEdict->v.angles.x = -m_pCameraEdict->v.angles.x;

		m_pCameraEdict->v.v_angle = m_pCameraEdict->v.angles;

		UTIL_FixAngles(&m_pCameraEdict->v.angles);*/
	}

	DispatchThink(m_pCameraEdict);
}

BOOL CBotCam::TuneIn(edict_t* pPlayer) const
{
	if (gBotGlobals.m_iNumBots == 0)
	{
		BotMessage(pPlayer, 0, "No Bots are running for bot cam");
		return false;
	}
	else if (m_pCameraEdict == nullptr)
	{
		BotMessage(pPlayer, 0, "Oops, looks like camera never worked...");
		return false;
	}
	//m_TunedIn[ENTINDEX(pPlayer)-1] = 1;
	SET_VIEW(pPlayer, m_pCameraEdict);

	return true;
}

void CBotCam::TuneOff(edict_t* pPlayer)
{
	//m_TunedIn[ENTINDEX(pPlayer)-1] = 0;
	SET_VIEW(pPlayer, pPlayer);
}

void CBotCam::Clear()
{
	m_pCurrentBot = nullptr;
	m_iState = BOTCAM_NONE;
	m_pCameraEdict = nullptr;
	m_fNextChangeBotTime = 0.0f;
	m_fNextChangeState = 0.0f;
	m_bTriedToSpawn = false;
}

/////////////////////////////////////////////
// TFC Specific

// when noInfo  is true we want to find ...
// any valid capture point for team...
// and haven't given goal or group info
/*edict_t* CTFCCapturePoints::getCapturePoint(int group, int goal, int team, BOOL noInfo)
{
	dataStack<CTFCGoal> tempStack = m_CapPoints;
	CTFCGoal* pGotCap;

	dataUnconstArray<edict_t*> goals;

	while (!tempStack.IsEmpty())
	{
		pGotCap = tempStack.ChoosePointerFromStack();

		// looking for capture point without flag
		if (noInfo)
		{
			if (gBotGlobals.TFC_isValidGoal(pGotCap->getGoal(), team))
				goals.Add(pGotCap->edict());
		}
		else
		{
			if (group && pGotCap->isForGroup(group) || goal && pGotCap->isForGoal(goal) && pGotCap->isForTeam(team))
			{
				if (gBotGlobals.isMapType(TFC_MAP_CAPTURE_FLAG_MULTIPLE))
				{
					goals.Add(pGotCap->edict());
				}
				else
				{
					tempStack.Init();

					return pGotCap->edict();
				}
			}
		}
	}

	if (!goals.IsEmpty())
	{
		edict_t* pGoal = goals.Random();

		goals.Clear();

		return pGoal;
	}

	return NULL;
}*/
//////////////////////////////////////////////////////////////////////////////
// METAMOD REQUIRED...

#ifdef RCBOT_META_BUILD
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	// this is one of the initialization functions hooked by metamod in the gamedll API
	if (!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return false;
	}
	else if (*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return false;
	}

	// gFunctionTable defined in dll.cpp
	// copy the whole table for metamod to know which functions we are using here
	std::memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	return true; // alright
}

#endif