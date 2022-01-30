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
// bot_commands.cpp
//
//////////////////////////////////////////////////
//
// Bot commands & cvars
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

#include "bot_commands.h"
#include "bot_menu.h"

extern CBotGlobals gBotGlobals;
extern CWaypointLocations WaypointLocations;

#ifndef RCBOT_META_BUILD

// this is the LINK_ENTITY_TO_CLASS function that creates a player (bot)
void player(entvars_t* pev)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
	extern HINSTANCE h_Library;
#else
	extern void* h_Library;
#endif
	static LINK_ENTITY_FUNC otherClassName = NULL;
	if (otherClassName == NULL)
		otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, "player");
	if (otherClassName != NULL)
	{
		(*otherClassName)(pev);
	}
}

#endif // END RCBOT_META_BUILD

#ifdef _WIN32
#define strdup _strdup
#endif

eBotCvarState CMenuSelectCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (pClient == nullptr)
	{
		return BOT_CVAR_CONTINUE; // engine command, so continue anyway.
	}

	if (pClient->m_pMenu == nullptr)
	{
		return BOT_CVAR_CONTINUE; // no menu, continue
	}

	if (!arg1)
		return BOT_CVAR_ERROR;
	if (!*arg1)
		return BOT_CVAR_ERROR;

	const int iMenu = atoi(arg1);

	if (iMenu < 10)
	{
		pClient->m_iLastMenuItemSelected = iMenu;
		pClient->m_pMenu->Activate(iMenu, pClient);
	}

	return BOT_CVAR_CONTINUE;
}

eBotCvarState CSetPassCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEdict;

	if (!arg1)
		return BOT_CVAR_ERROR;
	if (!*arg1)
		return BOT_CVAR_ERROR;
	if (!pClient)
		return BOT_CVAR_ERROR;
	if ((pEdict = pClient->GetPlayer()) != nullptr)
	{
		pClient->SetPass(arg1);
		BotMessage(pEdict, 0, "Password Set");
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CWaypointMenuCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	//edict_t *pEntity;

	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "This command can only be used on the client");
		return BOT_CVAR_ERROR;
	}

	//pEntity = pClient->GetPlayer();

	pClient->m_iCurrentWaypoint = WaypointLocations.NearestWaypoint(EntityOrigin(pClient->GetPlayer()), 50.0, -1, FALSE, TRUE);

	if (pClient->m_iCurrentWaypoint != -1)
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].Render(pClient);
	else
	{
		BotMessage(pClient->GetPlayer(), 0, "Error: You are not standing close enough to a waypoint!");
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CBotMenuCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	//edict_t *pEntity;

	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "This command can only be used on the client");
		return BOT_CVAR_ERROR;
	}

	//pEntity = pClient->GetPlayer();

	gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].Render(pClient);

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CUtilCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity;

	pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (!arg1 || !*arg1)
	{
		BotMessage(pEntity, 0, "no command entered");
		return BOT_CVAR_ERROR;
	}

	/*if ( FStrEq("tsweapon",arg1) )
	{
		if ( pEntity && (arg2 && *arg2) )
		{
			GiveTSWeapon(pEntity,atoi(arg2));
		}
	}
	else */
	if (FStrEq("showboredom", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.flags & FL_FAKECLIENT)
				{
					CBot* pBot = UTIL_GetBotPointer(pPlayer);

					if (pBot)
					{
						pBot->printBoredoms(pEntity);
					}
				}
			}
		}
	}
	else if (FStrEq("bot_goto", arg1))
	{
		if (pClient && (arg2 && *arg2))
		{
			CBot* pBot;

			pBot = UTIL_GetBotPointer(UTIL_FindPlayerByTruncName(arg2));

			if (pBot != nullptr)
			{
				int iWpt = -1;

				if (arg3 && *arg3)
					iWpt = atoi(arg3);
				else
					iWpt = WaypointLocations.NearestWaypoint(pClient->GetPlayer()->v.origin, 100, -1);

				pBot->ClearTasks();

				pBot->AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, 0, nullptr, iWpt, -2));
			}
		}
	}
	else if (FStrEq("version", arg1))
	{
		BotMessage(pEntity, 0, "Bot Version : %s", BOT_VER);
	}
	else if (FStrEq("givetsweapon", arg1))
	{
		if (pEntity && arg2 && *arg2)
			UTIL_makeTSweapon(pEntity, static_cast<eTSWeaponID>(atoi(arg2)));
	}
	else if (FStrEq("numclients", arg1))
	{
		int loopcount = UTIL_GetNumClients(TRUE);

		BotMessage(pEntity, 0, "Loop count = %d", loopcount);
		//      BotMessage(pEntity,0,"unreliable count (joining) = %d",gBotGlobals.m_iJoiningClients);
		//      BotMessage(pEntity,0,"unreliable count (joined) = %d",gBotGlobals.m_iNumClients);
	}
	else if (FStrEq("hudtext", arg1))
	{
		Vector colour = Vector(255, 255, 255);

		if (arg2 && *arg2)
		{
			char msg[256];
			strncpy(msg, arg2, 255);
			msg[255] = 0;

			BotFunc_FillString(msg, "^", "\n", 255);
			BotFunc_FillString(msg, "_", " ", 255);

			if (arg3 && *arg3)
			{
				if (FStrEq(arg3, "blue"))
					colour = Vector(0, 0, 255);
				else if (FStrEq(arg3, "green"))
					colour = Vector(0, 255, 0);
				else if (FStrEq(arg3, "red"))
					colour = Vector(255, 0, 0);
				else if (FStrEq(arg3, "yellow"))
					colour = Vector(255, 255, 0);
				else if (FStrEq(arg3, "purple"))
					colour = Vector(255, 0, 255);
				else if (FStrEq(arg3, "cyan"))
					colour = Vector(0, 255, 255);
			}

			HudText hudmessage;

			hudmessage.SetChannel(3);
			hudmessage.SetEffect(HUD_EFFECT_FADE_IN_OUT);
			hudmessage.SetFadeTime(0.75, 0.75);
			hudmessage.SetEffectTime(0.75);
			hudmessage.SetHoldTime(2.0);
			hudmessage.SetPosition(-1, -1);
			hudmessage.SetColour1(colour, 200);
			hudmessage.SetColour2(colour, 200);
			hudmessage.SayMessage(msg, nullptr); // all
		}
	}
	else if (FStrEq("explo", arg1))
	{
		if (pEntity == nullptr)
		{
			BotMessage(nullptr, 0, "can't use on dedicated server");
			return BOT_CVAR_ERROR;
		}

		int magnitude = 400;
		bool dodamage = TRUE;

		if (arg2 != nullptr && *arg2 != 0)
			magnitude = atoi(arg2);
		if (arg3 != nullptr && *arg3 != 0)
		{
			int temp = atoi(arg3);

			if (temp > 0)
				dodamage = FALSE;
		}

		TraceResult tr;
		entvars_t* pev = &pEntity->v;

		Vector vSrc;
		Vector vEnd;

		UTIL_MakeVectors(pev->v_angle);

		vSrc = pev->origin + pev->view_ofs;
		vEnd = vSrc + gpGlobals->v_forward * 4096.0;

		UTIL_TraceLine(vSrc, vEnd, dont_ignore_monsters, dont_ignore_glass, pEntity, &tr);

		WaypointDrawBeam(pEntity, vSrc, tr.vecEndPos, 20, 10, 200, 1, 1, 160, 10);

		ExplosionCreate(tr.vecEndPos - gpGlobals->v_forward * 4, pEntity->v.v_angle, pEntity, magnitude, dodamage);
		UTIL_BotScreenShake(tr.vecEndPos, 16, 100, 2, magnitude);
	}
	else if (FStrEq("showreps", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.flags & FL_FAKECLIENT)
				{
					CBot* pBot = UTIL_GetBotPointer(pPlayer);

					if (pBot)
					{
						pBot->m_Profile.m_Rep.printReps(pBot, pEntity);
					}
				}
			}
		}
	}
	else if (FStrEq("killbot", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.flags & FL_FAKECLIENT)
				{
					CBot* pBot = UTIL_GetBotPointer(pPlayer);

					if (pBot)
					{
						pBot->m_bKill = TRUE;

						if (pEntity)
							BotPrintTalkMessage("%s is killing bot \"%s\"...\n", STRING(pEntity->v.netname), STRING(pPlayer->v.netname));
						else
							BotPrintTalkMessage("Server is killing bot \"%s\"...\n", STRING(pPlayer->v.netname));
					}
				}
			}
			else
				BotMessage(pEntity, 0, "bot name not found");
		}
		else
			BotMessage(pEntity, 0, "usage rcbot util killbot <botname>");
	}
	else if (FStrEq("set_teleport", arg1))
	{
		if (pEntity)
			pClient->SetTeleportVector(pEntity->v.origin);
		/*else
		{
		if ( arg2 && *arg2 )
		{
		edict_t *pPlayer = UTIL_FindPlayerByTruncName(arg2);

		  if ( pPlayer )
		  pClient->SetTeleportVector(pPlayer->v.origin);
		  }
		  //    else
		  //    BotMessage(NULL,0,"Server usage, set_teleport <player name>, sets the origin of the teleport to the players origin");
}*/
	}
	else if (FStrEq("projectile_rep", arg1))
	{
		if (pClient)
		{
			pClient->toggleCmd(CLIENT_CMD_PROJ_REPEL);
			BotMessage(pEntity, 0, "%s : %d", arg1, pClient->hasCmd(CLIENT_CMD_PROJ_REPEL));
		}
	}
	else if (FStrEq("grenade_deact", arg1))
	{
		if (pClient)
		{
			pClient->toggleCmd(CLIENT_CMD_GREN_DEACT);
			BotMessage(pEntity, 0, "%s : %d", arg1, pClient->hasCmd(CLIENT_CMD_GREN_DEACT));
		}
	}
	else if (FStrEq("teleport_ent", arg1))
	{
		if (pClient)
		{
			const Vector* p_vTeleport = pClient->GetTeleportVector();

			if (p_vTeleport)
			{
				edict_t* pTeleportEntity = UTIL_getEntityInFront(pEntity);

				if (ENTINDEX(pTeleportEntity) && !(pTeleportEntity->v.flags & FL_WORLDBRUSH))
				{
					SET_ORIGIN(pTeleportEntity, *p_vTeleport);
				}
			}
			else
				BotMessage(nullptr, 0, "no teleport vector set yet");
		}
	}
	else if (gBotGlobals.IsNS() && FStrEq("buildres", arg1))
	{
		BotFunc_NS_MarineBuild(AVH_USER3_RESTOWER, "resourcetower", pEntity->v.origin);
	}
	else if (gBotGlobals.IsNS() && FStrEq("buildcomm", arg1))
	{
		BotFunc_NS_MarineBuild(AVH_USER3_COMMANDER_STATION, "team_command", pEntity->v.origin);
	}
	else if (pClient && FStrEq("teleport", arg1))
	{
		const Vector* p_vTeleport = pClient->GetTeleportVector();

		if (p_vTeleport)
		{
			if (arg2 && *arg2)
			{
				edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

				if (pPlayer != nullptr)
				{
					UTIL_SetOrigin(&pPlayer->v, *p_vTeleport);

					if (pEntity)
						BotPrintTalkMessage("%s teleported %s to another location.\n", STRING(pEntity->v.netname), STRING(pPlayer->v.netname));
				}
			}
			else
			{
				if (pEntity)
				{
					UTIL_SetOrigin(&pEntity->v, *p_vTeleport);

					BotPrintTalkMessage("%s teleported self to another location\n", STRING(pEntity->v.netname));
				}
			}
		}
		else
			BotMessage(pEntity, 0, "Error: No teleport destination set, use \"set_teleport\"");
	}
	else if (pEntity && FStrEq("notouch", arg1))
	{
		pClient->m_bNoTouch = !pClient->m_bNoTouch;

		BotPrintTalkMessage("%s used notouch %d on self\n", STRING(pEntity->v.netname), pClient->m_bNoTouch);
	}
	else if (FStrEq("notarget_mode", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.flags & FL_NOTARGET)
					pPlayer->v.flags &= ~FL_NOTARGET;
				else
					pPlayer->v.flags |= FL_NOTARGET;

				BotPrintTalkMessage("%s used notarget_mode %d on %s\n", STRING(pEntity->v.netname), (pPlayer->v.flags & FL_NOTARGET) == FL_NOTARGET, STRING(pPlayer->v.netname));
			}
		}
		else if (pEntity)
		{
			if (pEntity->v.flags & FL_NOTARGET)
				pEntity->v.flags &= ~FL_NOTARGET;
			else
				pEntity->v.flags |= FL_NOTARGET;

			BotPrintTalkMessage("%s used notarget_mode %d on self\n", STRING(pEntity->v.netname), (pEntity->v.flags & FL_NOTARGET) == FL_NOTARGET);
		}
	}
	else if (FStrEq("send_sound", arg1))
	{
		if (arg2 && *arg2)
		{
			UTIL_PlaySoundToAll(arg2);
		}
	}
	else if (FStrEq("noclip_mode", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.movetype != MOVETYPE_NOCLIP)
					pPlayer->v.movetype = MOVETYPE_NOCLIP;
				else
					pPlayer->v.movetype = MOVETYPE_WALK;

				BotPrintTalkMessage("%s used no_clip %d on %s\n", STRING(pEntity->v.netname), pPlayer->v.movetype == MOVETYPE_NOCLIP, STRING(pPlayer->v.netname));
			}
		}
		else if (pEntity)
		{
			if (pEntity->v.movetype != MOVETYPE_NOCLIP)
				pEntity->v.movetype = MOVETYPE_NOCLIP;
			else
				pEntity->v.movetype = MOVETYPE_WALK;

			BotPrintTalkMessage("%s used no_clip %d on self\n", STRING(pEntity->v.netname), pEntity->v.movetype == MOVETYPE_NOCLIP);
		}
	}
	else if (FStrEq("godmode", arg1))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer != nullptr)
			{
				if (pPlayer->v.takedamage != DAMAGE_NO)
					pPlayer->v.takedamage = DAMAGE_NO;
				else
					pPlayer->v.takedamage = DAMAGE_YES;

				BotPrintTalkMessage("%s used godmode %d on %s\n", STRING(pEntity->v.netname), pPlayer->v.takedamage == DAMAGE_NO, STRING(pPlayer->v.netname));
			}
		}
		else if (pEntity)
		{
			if (pEntity->v.takedamage != DAMAGE_NO)
				pEntity->v.takedamage = DAMAGE_NO;
			else
				pEntity->v.takedamage = DAMAGE_YES;

			BotPrintTalkMessage("%s used godmode %d on self\n", STRING(pEntity->v.netname), pEntity->v.takedamage == DAMAGE_NO);
		}
	}
	else if (pEntity && FStrEq("search", arg1))
	{
		edict_t* pEnt = nullptr;
		edict_t* p_entity = pClient->GetPlayer();

		float range = 100;

		if (arg2 && *arg2)
			range = atof(arg2);

		//FILE *fp;

		//fp = fopen("botlog.txt","a");

		while ((pEnt = UTIL_FindEntityInSphere(pEnt, p_entity->v.origin, range)) != nullptr)
		{
			BotMessage(p_entity, 0, "Found: id: %d, classname:%s distance:%0.2f iuser3:%d pclass:%d\niuser1 %d, iuser2 %d, iuser4 %d, euser1 0x%x, euser2 0x%x", ENTINDEX(pEnt), STRING(pEnt->v.classname), (EntityOrigin(pEnt) - p_entity->v.origin).Length(), pEnt->v.iuser3, pEnt->v.playerclass, pEnt->v.iuser1, pEnt->v.iuser2, pEnt->v.iuser4, pEnt->v.euser1, pEnt->v.euser2);
		}

		//fclose(fp);
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CUsersCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (FStrEq(arg1, "readusers"))
	{
		gBotGlobals.m_BotUsers.Destroy();

		BotMessage(pEntity, 0, "re-reading bot users...");
		ReadBotUsersConfig();

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CClient* pPlayerClient = gBotGlobals.m_Clients.GetClientByIndex(i);

			if (pPlayerClient->IsUsed())
			{
				pPlayerClient->SetAccessLevel(0);
				pPlayerClient->m_bRecheckAuth = TRUE;
			}
		}
	}
	else if (FStrEq(arg1, "adduser"))
	{
		if (arg2 && *arg2 && (arg3 && *arg3))
		{
			edict_t* pPlayer = UTIL_FindPlayerByTruncName(arg2);

			if (pPlayer == pEntity)
			{
				if (IS_DEDICATED_SERVER())
					BotMessage(pEntity, 0, "Player not found!!");
				else
					BotMessage(pEntity, 0, "Can't give yourself different access!!");

				return BOT_CVAR_ERROR;
			}

			if (pPlayer)
			{
				CClient* pPlayerClient = gBotGlobals.m_Clients.GetClientByEdict(pPlayer);

				const int iLev = atoi(arg3);

				gBotGlobals.m_BotUsers.AddPlayer(STRING(pPlayer->v.netname), "***", iLev, pPlayerClient->steamID());

				pPlayerClient->m_bRecheckAuth = TRUE;

				BotMessage(pEntity, 0, "Gave %s access level of %d", STRING(pPlayer->v.netname), iLev);
			}
			else
			{
				BotMessage(pEntity, 0, "Player not found!!");

				return BOT_CVAR_ERROR;
			}
		}
		else
		{
			BotMessage(pEntity, 0, "Usage: rcbot readusers add <player name> <access level>");
		}
	}
	else if (FStrEq(arg1, "removeuser"))
	{
		if (arg2 && *arg2)
		{
			edict_t* pPlayer;

			if (*arg2 != '#')
				pPlayer = UTIL_FindPlayerByTruncName(arg2);
			else
			{
				pPlayer = UTIL_GetPlayerByPlayerId(atoi(&arg2[1]));
			}

			if (pPlayer)
			{
				if (pPlayer == pEntity)
				{
					BotMessage(pEntity, 0, "Cannot remove yourself!!!");
					return BOT_CVAR_ERROR;
				}

				CClient* pPlayerClient = gBotGlobals.m_Clients.GetClientByEdict(pPlayer);

				gBotGlobals.m_BotUsers.RemovePlayer(STRING(pPlayer->v.netname), pPlayerClient->GetPass(), pPlayerClient->steamID());

				BotMessage(pEntity, 0, "Removed %s's access", STRING(pPlayer->v.netname));
			}
			else
			{
				BotMessage(pEntity, 0, "Player not found!!");
				return BOT_CVAR_ERROR;
			}
		}
		else
		{
			BotMessage(pEntity, 0, "Usage: rcbot readusers add <player name>");
		}
	}
	else if (FStrEq("showusers", arg1))
	{
		gBotGlobals.m_BotUsers.ShowUsers(pEntity);
	}
	else
	{
		BotMessage(pEntity, 0, "Error: No command issued or command does not exist!");
		showHelp(pEntity);
		return BOT_CVAR_ERROR;
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CSquadMenuCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "This command can only be used on the client");
		return BOT_CVAR_ERROR;
	}

	gBotGlobals.m_Menus[BOT_MENU_SQUAD].Render(pClient);

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CDebugEntCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (FStrEq(arg1, "off"))
	{
		pClient->m_pDebugEnt = nullptr;
	}
	else
	{
		if (pClient)
		{
			edict_t* pEntity = pClient->GetPlayer();
			edict_t* pOther = UTIL_FacingEnt(pEntity, TRUE);

			if (pOther)
				pClient->m_pDebugEnt = pOther;
			else
				BotMessage(pClient->GetPlayer(), 0, "No entity found");
		}
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CRemoveBotCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	int iTeam = -1;

	if (arg1 && *arg1)
		iTeam = atoi(arg1);

	BotFunc_KickBotFromTeam(iTeam);

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CDebugBotCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (FStrEq(arg1, "bot"))
	{
		if (!arg2 || !*arg2)
		{
			showHelp(pClient->GetPlayer());
			return BOT_CVAR_ERROR;
		}

		pClient->m_iDebugBot = UTIL_GetBotIndex(UTIL_FindPlayerByTruncName(arg2));

		if (pClient->m_iDebugBot == -1)
		{
			BotMessage(pClient->GetPlayer(), 0, "Bot not found (not found/ not an RCBot/ or is a human player)");
			return BOT_CVAR_ERROR;
		}
	}
	else if (FStrEq(arg1, "facing"))
	{
		if (pClient)
		{
			edict_t* pEntity = pClient->GetPlayer();

			edict_t* pOther = UTIL_FacingEnt(pEntity);

			if (pOther)
			{
				pClient->m_iDebugBot = UTIL_GetBotIndex(pOther);

				if (pClient->m_iDebugBot == -1)
				{
					BotMessage(pClient->GetPlayer(), 0, "Facing entity is not a bot");
					return BOT_CVAR_ERROR;
				}
			}
			else
				BotMessage(pClient->GetPlayer(), 0, "No entity found");
		}
	}
	else if (FStrEq(arg1, "off"))
	{
		pClient->m_iDebugBot = -1;
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CDebugCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	BOOL bError;

	int iDebugLevel = 0;

	if (IS_DEDICATED_SERVER())
	{
		bError = pClient != nullptr;
	}
	else
		bError = pClient->GetPlayer() != gBotGlobals.m_pListenServerEdict;

	if (bError)
	{
		BotMessage(nullptr, 0, "You can only use this command on the SERVER");
		return BOT_CVAR_ERROR;
	}

	BOOL bMsg = FALSE;

	if (!arg1 || !*arg1 || (!arg2 || !*arg2))
	{
		BotMessage(nullptr, 0, "Usage: rcbot_debug <level> <1 : on, 0 : off>");
		return BOT_CVAR_ERROR;
	}

	if (FStrEq(arg1, "touch"))
		iDebugLevel = BOT_DEBUG_TOUCH_LEVEL;
	else if (FStrEq(arg1, "think"))
		iDebugLevel = BOT_DEBUG_THINK_LEVEL;
	else if (FStrEq(arg1, "hear"))
		iDebugLevel = BOT_DEBUG_HEAR_LEVEL;
	else if (FStrEq(arg1, "message"))
	{
		iDebugLevel = BOT_DEBUG_MESSAGE_LEVEL;
		bMsg = TRUE;
	}
	else if (FStrEq(arg1, "block"))
		iDebugLevel = BOT_DEBUG_BLOCK_LEVEL;
	else if (FStrEq(arg1, "move"))
		iDebugLevel = BOT_DEBUG_MOVE_LEVEL;
	else if (FStrEq(arg1, "aim"))
		iDebugLevel = BOT_DEBUG_AIM_LEVEL;
	else if (FStrEq(arg1, "nav"))
		iDebugLevel = BOT_DEBUG_NAV_LEVEL;
	else if (FStrEq(arg1, "see"))
		iDebugLevel = BOT_DEBUG_SEE_LEVEL;

	if (iDebugLevel)
	{
		if (atoi(arg2) > 0)
		{
			BotMessage(nullptr, 0, "debugging \"%s\" is now ON", arg1);
			gBotGlobals.m_iDebugLevels |= iDebugLevel;
		}
		else
		{
			BotMessage(nullptr, 0, "debugging \"%s\" is now OFF", arg1);
			gBotGlobals.m_iDebugLevels &= ~iDebugLevel;

			if (bMsg)
				gBotGlobals.m_pDebugMessage = nullptr;
		}
	}
	else
	{
		BotMessage(nullptr, 0, "Debug level not found");
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CConfigCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (!arg1 || !*arg1)
	{
		BotMessage(pEntity, 0, "Error: no sub-command issued");
		showHelp(pEntity);
		return BOT_CVAR_ERROR;
	}

	const BOOL bSetting = arg2 && *arg2; // is player setting the value or wanting details?
	float fSetVal = 0;

	const int iState = atoi(arg2);
	int iConfig = 0;

	BOOL bSuccess = TRUE;

	if (FStrEq("ts_dont_steal_weapons", arg1))
		iConfig = BOT_CONFIG_TS_DONT_STEAL_WEAPONS;
	else if (FStrEq("ts_dont_pickup_weapons", arg1))
		iConfig = BOT_CONFIG_TS_DONT_PICKUP_WEAPONS;
	else if (FStrEq("disable_bot_squads", arg1))
		iConfig = BOT_CONFIG_DISABLE_BOT_SQUADS;
	else if (FStrEq("ts_kungfu", arg1))
		iConfig = BOT_CONFIG_TS_KUNGFU;
	else if (FStrEq("disable_weapon_learning", arg1))
		iConfig = BOT_CONFIG_DISABLE_WEAPON_LEARN;
	else if (FStrEq("tooltips", arg1))
		iConfig = BOT_CONFIG_TOOLTIPS;
	else if (FStrEq("abnormal_game", arg1))
		iConfig = BOT_CONFIG_ABNORMAL_GAME;
	else if (FStrEq("autowaypoint_humans", arg1))
		iConfig = BOT_CONFIG_AUTOWAYPOINT_HUMANS;
	else if (FStrEq("allow_botcam", arg1))
		iConfig = BOT_CONFIG_ENABLE_BOTCAM;
	else if (FStrEq("balance_teams", arg1))
		iConfig = BOT_CONFIG_BALANCE_TEAMS;
	else if (FStrEq("chat_dont_learn", arg1))
		iConfig = BOT_CONFIG_CHAT_DONT_LEARN;
	else if (FStrEq("marine_auto_build", arg1))
		iConfig = BOT_CONFIG_MARINE_AUTO_BUILD;
	else if (FStrEq("chat_reply_to_bots", arg1))
		iConfig = BOT_CONFIG_CHAT_REPLY_TO_BOTS;
	else if (FStrEq("wait_for_orders", arg1))
		iConfig = BOT_CONFIG_WAIT_FOR_ORDERS;
	else if (FStrEq("reserve_bot_slots", arg1))
		iConfig = BOT_CONFIG_RESERVE_BOT_SLOTS;
	else if (FStrEq("dont_shoot", arg1))
		iConfig = BOT_CONFIG_DONT_SHOOT;
	else if (FStrEq("chatting", arg1))
		iConfig = BOT_CONFIG_CHATTING;
	else if (FStrEq("real_mode", arg1))
		iConfig = BOT_CONFIG_REAL_MODE;
	else if (FStrEq("commanding", arg1))
		iConfig = BOT_CONFIG_COMMANDING;
	else if (FStrEq("unstick", arg1))
		iConfig = BOT_CONFIG_UNSTICK;
	else if (FStrEq("bots_leave_and_join", arg1))
		iConfig = BOT_CONFIG_BOTS_LEAVE_AND_JOIN;
	else if (FStrEq("bots_wait_for_bots", arg1))
		iConfig = BOT_CONFIG_BOTS_WAIT_FOR_BOTS;
	else if (FStrEq("not_ns3final", arg1))
		iConfig = BOT_CONFIG_NOT_NS3_FINAL;
	else if (FStrEq("blinking", arg1))
		iConfig = BOT_CONFIG_BLINKING;
	else if (FStrEq("wait_at_res", arg1))
		iConfig = BOT_CONFIG_WAIT_AT_RESOURCES;
	else if (FStrEq("turnspeed", arg1))
	{
		fSetVal = gBotGlobals.m_fTurnSpeed;

		if (arg2 && *arg2)
		{
			const float fspeed = atof(arg2);

			if (fspeed > 0)
			{
				for (int i = 0; i < MAX_PLAYERS; i++)
					gBotGlobals.m_Bots[i].m_fTurnSpeed = fspeed;
				gBotGlobals.m_fTurnSpeed = fspeed;
				fSetVal = gBotGlobals.m_fTurnSpeed;
			}
			else
				BotMessage(pEntity, 0, "Error, must be > 0");
		}
	}
	else if (FStrEq("force_team", arg1))
	{
		if (bSetting)
			gBotGlobals.m_iForceTeam = atoi(arg2);
		else
		{
			fSetVal = static_cast<float>(gBotGlobals.m_iForceTeam);
		}
	}
	/*else if (FStrEq("tfc_map_type", arg1))
	{
		if (bSetting)
			gBotGlobals.setMapType(static_cast<eTFCMapType>(atoi(arg2)));
		else
			fSetVal = static_cast<float>(gBotGlobals.getMapType());
	}*/
	else if (FStrEq("bot_stuck_speed", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fBotStuckSpeed = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fBotStuckSpeed;
	}
	else if (FStrEq("update_ladder_time", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fUpdateLadderTime = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fUpdateLadderTime;
	}
	else if (FStrEq("min_bots", arg1))
	{
		if (bSetting)
			gBotGlobals.m_iMinBots = atoi(arg2);
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iMinBots);
	}
	else if (FStrEq("max_bots", arg1))
	{
		if (bSetting)
			gBotGlobals.m_iMaxBots = atoi(arg2);
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iMaxBots);
	}
	/*
			m_fHiveImportance = 1.0f;
		m_fResTowerImportance = 0.7f;
		m_fHealingImportance = 0.75f;
		m_fStructureBuildingImportance = 0.4f;
		m_fRefillStructureImportance = 0.5f;
		*/
	else if (FStrEq("u_hive", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fHiveImportance = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fHiveImportance;
	}
	else if (FStrEq("u_restower", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fResTowerImportance = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fResTowerImportance;
	}
	else if (FStrEq("u_healing", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fHealingImportance = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fHealingImportance;
	}
	else if (FStrEq("u_structure", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fStructureBuildingImportance = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fStructureBuildingImportance;
	}
	else if (FStrEq("u_refill", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fRefillStructureImportance = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fRefillStructureImportance;
	}
	else if (FStrEq("gorge_amount", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fGorgeAmount = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fGorgeAmount;
	}
	else if (FStrEq("chat_percent", arg1))
	{
		if (bSetting)
			gBotGlobals.m_iBotChatPercent = atoi(arg2);
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iBotChatPercent);
	}
	else if (FStrEq("chat_reply_percent", arg1))
	{
		if (bSetting)
			gBotGlobals.m_iBotChatReplyPercent = atoi(arg2);
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iBotChatReplyPercent);
	}
	else if (FStrEq("wall_stick_tolerance", arg1))
	{
		if (bSetting)
			gBotGlobals.m_fWallStickTol = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fGorgeAmount;
	}
	/*else if ( FStrEq("lerk_hold_time",arg1) )
	{
		if ( bSetting )
			gBotGlobals.m_fLerkHoldTime = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fLerkHoldTime;
	}
	else if ( FStrEq("lerk_flap_time",arg1) )
	{
		if ( bSetting )
			gBotGlobals.m_fLerkFlapTime = atof(arg2);
		else
			fSetVal = gBotGlobals.m_fLerkFlapTime;
	}*/
	else if (FStrEq("max_path_revs", arg1))
	{
		if (bSetting)
		{
			const int iRevs = atoi(arg2);

			bSuccess = iRevs > 0;

			if (bSuccess)
			{
				gBotGlobals.m_iMaxPathRevs = iRevs;

				// update each bot currently being used
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					CBot* pBot = &gBotGlobals.m_Bots[i];

					if (pBot->IsUsed())
					{
						pBot->m_Profile.m_iPathRevs = iRevs;
					}
				}
			}
		}
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iMaxPathRevs);
	}
	else if (FStrEq("max_update_vision_revs", arg1))
	{
		if (bSetting)
		{
			const int iRevs = atoi(arg2);

			bSuccess = iRevs > 0;

			if (bSuccess)
			{
				gBotGlobals.m_iMaxVisUpdateRevs = iRevs;

				// update each bot currently being used
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					CBot* pBot = &gBotGlobals.m_Bots[i];

					if (pBot->IsUsed())
					{
						pBot->m_Profile.m_iVisionRevs = iRevs;
					}
				}
			}
		}
		else
			fSetVal = static_cast<float>(gBotGlobals.m_iMaxVisUpdateRevs);
	}
	else if (FStrEq("update_vision_time", arg1))
	{
		if (bSetting)
		{
			const float fTime = atof(arg2);

			bSuccess = fTime >= 0;

			if (bSuccess)
			{
				gBotGlobals.m_fUpdateVisTime = fTime;

				// update each bot currently being used
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					CBot* pBot = &gBotGlobals.m_Bots[i];

					if (pBot->IsUsed())
					{
						pBot->m_Profile.m_fVisionTime = fTime;
					}
				}
			}
		}
		else
			fSetVal = gBotGlobals.m_fUpdateVisTime;
	}
	else
	{
		// show help

		BotMessage(pEntity, 0, "Error: Command does not exist/command not issued!");
		BotMessage(pEntity, 0, "-------------------------------------------------");
		showHelp(pEntity);

		return BOT_CVAR_ACCESSED;
	}

	if (iConfig)
	{
		if (bSetting)
		{
			gBotGlobals.ChangeConfigSetting(iConfig, iState);

			if (bSuccess)
				BotMessage(pEntity, 0, "%s is now %d", arg1, iState);
		}
		else
			BotMessage(pEntity, 0, "\"%s\" is currently \"%d\"", arg1, gBotGlobals.IsConfigSettingOn(iConfig));
	}
	else if (!bSetting)
	{
		BotMessage(pEntity, 0, "\"%s\" is currently \"%0.2f\"", arg1, fSetVal);
	}
	else if (bSuccess)
		BotMessage(pEntity, 0, "%s set to %s", arg1, arg2);

	else
		BotMessage(pEntity, 0, "Error with command, wrong argument");

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CBotSquadCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (!pClient)
		return BOT_CVAR_ERROR;
	if (!arg1 || !*arg1)
		return BOT_CVAR_ERROR;
	if (!arg2 || !*arg2)
		return BOT_CVAR_ERROR;

	edict_t* pEntity = pClient->GetPlayer();

	CBotSquad* theSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if (theSquad == nullptr)
	{
		ClientPrint(pEntity, HUD_PRINTTALK, "You are not leader of a squad\n");
		return BOT_CVAR_ERROR;
	}

	if (FStrEq(arg1, "form"))
	{
		if (FStrEq(arg2, "line"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_LINE);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now LINE\n");
		}
		else if (FStrEq(arg2, "vee"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_WEDGE);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now VEE\n");
		}
		else if (FStrEq(arg2, "wedge"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_WEDGE);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now WEDGE\n");
		}
		else if (FStrEq(arg2, "column"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_COLUMN);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now COLUMN\n");
		}
		else if (FStrEq(arg2, "echleft"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_ECH_LEFT);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now ECHELON LEFT\n");
		}
		else if (FStrEq(arg2, "echright"))
		{
			theSquad->ChangeFormation(SQUAD_FORM_ECH_RIGHT);
			ClientPrint(pEntity, HUD_PRINTTALK, "Squad formation is now ECHELON RIGHT\n");
		}
	}
	else if (FStrEq(arg1, "spread"))
	{
		const float fNewSpread = atof(arg2);

		if (fNewSpread)
		{
			theSquad->ChangeSpread(fNewSpread);

			ClientPrint(pEntity, HUD_PRINTTALK, "Squad spread changed\n");
		}
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CPathWaypointCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "You can only use this command on the client");
		return BOT_CVAR_ERROR;
	}

	edict_t* pEntity = pClient->GetPlayer();

	if (FStrEq(arg1, "on"))
	{
		pClient->m_bWaypointPathsOn = TRUE;
		BotMessage(pClient->GetPlayer(), 0, "Waypoint paths are now being shown");
	}
	else if (FStrEq(arg1, "off"))
	{
		pClient->m_bWaypointPathsOn = FALSE;
		BotMessage(pClient->GetPlayer(), 0, "Waypoints paths are not being shown");
	}
	else if (strncmp(arg1, "create", 6) == 0)
	{
		pClient->m_bWaypointPathsOn = TRUE;

		if (arg1[6] == '1')
		{
			WaypointCreatePath(pClient, 1);
		}
		else if (arg1[6] == '2')
		{
			WaypointCreatePath(pClient, 2);
		}
	}
	else if (strncmp(arg1, "remove", 6) == 0)
	{
		pClient->m_bWaypointPathsOn = TRUE;

		if (arg1[6] == '1')
		{
			WaypointRemovePath(pClient, 1);
		}
		else if (arg1[6] == '2')
		{
			WaypointRemovePath(pClient, 2);
		}
	}
	else if (FStrEq(arg1, "enable"))
	{
		gBotGlobals.m_bAutoPathWaypoint = TRUE;
		BotMessage(pClient->GetPlayer(), 0, "automatic path generation is now enabled");
	}
	else if (FStrEq(arg1, "disable"))
	{
		gBotGlobals.m_bAutoPathWaypoint = FALSE;
		BotMessage(pClient->GetPlayer(), 0, "automatic path generation is now disabled");
	}
	else
	{
		// show help
		/////////////////////
		BotMessage(pEntity, 0, "Error: You must issue a command!");
		BotMessage(pEntity, 0, "--------------------------------");
		showHelp(pEntity);
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CAutoWaypointCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity = nullptr;
	edict_t* pPlayer;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (pEntity == nullptr && (!arg1 || !*arg1))
	{
		BotMessage(nullptr, 0, "You must issue a player name");
		return BOT_CVAR_ERROR;
	}

	if (!arg1 || !*arg1)
		pPlayer = pEntity;
	else
		pPlayer = UTIL_FindPlayerByTruncName(arg1);

	if (pPlayer)
	{
		CClient* pWantedClient = gBotGlobals.m_Clients.GetClientByEdict(pPlayer);

		if (pWantedClient)
		{
			const int state = atoi(arg2);

			if (arg2 && *arg2)
				pWantedClient->AutoWaypoint(state);
			else
				pWantedClient->AutoWaypoint();

			if (pEntity)
			{
				if (state == 2)
				{
					BotPrintTalkMessageOne(pEntity, "autowaypoint set to 2 (debug) on you, you will see beams which are check points");

					if (pEntity != pPlayer)
						BotPrintTalkMessageOne(pPlayer, "autowaypoint set to 2 (debug) on you, you will see beams which are check points");
				}
				else
				{
					BotPrintTalkMessageOne(pEntity, "autowaypoint set to %d on %s", pWantedClient->IsAutoWaypointOn(), STRING(pPlayer->v.netname));
				}
			}
			else
				BotMessage(nullptr, 0, "autowaypoint set to %d on %s", pWantedClient->IsAutoWaypointOn(), STRING(pPlayer->v.netname));
		}
	}
	else
	{
		BotMessage(pEntity, 0, "Player not found");
		return BOT_CVAR_ERROR;
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CBotCamCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (pEntity)
	{
		if (FStrEq("off", arg1))
		{
			gBotGlobals.m_BotCam.TuneOff(pEntity);
			BotMessage(pEntity, 0, "Finished using botcam");
		}
		else
		{
			if (gBotGlobals.m_BotCam.TuneIn(pEntity))
			{
				BotMessage(pEntity, 0, "Using botcam");

				pClient->AddNewToolTip(BOT_TOOL_TIP_BOTCAM_ON);
			}
			else
				BotMessage(pEntity, 0, "Camera never worked or is not enabled (see command \"rcbot config allow_botcam\")");
		}

		return BOT_CVAR_ACCESSED;
	}
	else
		BotMessage(nullptr, 0, "Can't use botcam on dedicated server");

	return BOT_CVAR_ERROR;
}

void CWaypointCommand::showHelp(edict_t* pEntity)
{
	BotMessage(pEntity, 0, "waypoint command help");
	BotMessage(pEntity, 0, "---------------------");
	BotMessage(pEntity, 0, "Waypoints are for bots to navigate, if there are no");
	BotMessage(pEntity, 0, "waypoints on the map then the bots won't move around");
	BotMessage(pEntity, 0, "Waypoints are files stored in the rcbot/waypoints/%s folder", gBotGlobals.m_szModFolder);
	BotMessage(pEntity, 0, "\"waypoint on\" (displays waypoints)");
	BotMessage(pEntity, 0, "\"waypoint off\" (stops displaying waypoints)");
	BotMessage(pEntity, 0, "\"waypoint load\" (loads waypoints for current map)");
	BotMessage(pEntity, 0, "\"waypoint save\" (saves waypoints to disk for future use)");
}

eBotCvarState CWaypointCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "This command can only be used on the client");
		return BOT_CVAR_ERROR;
	}

	edict_t* pEntity = pClient->GetPlayer();

	if (FStrEq(arg1, "on"))
	{
		pClient->m_bWaypointOn = TRUE;
		BotMessage(pEntity, 0, "Waypoints are now being shown");
	}
	else if (FStrEq(arg1, "off"))
	{
		pClient->m_bWaypointOn = FALSE;
		BotMessage(pEntity, 0, "Waypoints are not being shown");
	}
	else if (FStrEq(arg1, "load"))
	{
		WaypointLoad(nullptr);
		BotMessage(pEntity, 0, "Waypoints have been loaded");
	}
	else if (FStrEq(arg1, "save"))
	{
		CWaypointConversion* theConverter = nullptr;

		if (arg2 && *arg2)
		{
			if (FStrEq(arg2, "wb"))
				theConverter = new CWhichbotConvert();
		}

		if (WaypointSave(FALSE, theConverter))
		{
			BotMessage(pEntity, 0, "Waypoints have been saved");

			if (theConverter != nullptr)
				BotMessage(pEntity, 0, "(Saved waypoints as %s format)", theConverter->getName());
		}
		else
			BotMessage(pEntity, 0, "ERROR: CANNOT save waypoints! (File cannot be opened for writing : missing directory/bad permissions)");

		if (theConverter)
		{
			theConverter->FreeMemory();
			delete theConverter;
		}
	}
	else if (FStrEq(arg1, "add"))
	{
		pClient->m_bWaypointOn = TRUE;
		WaypointAdd(pClient);
	}
	else if (FStrEq(arg1, "delete"))
	{
		pClient->m_bWaypointOn = TRUE;
		WaypointDelete(pClient);
	}
	else if (FStrEq(arg1, "info"))
	{
		WaypointPrintInfo(pEntity);
	}
	else if (FStrEq(arg1, "teleport"))
	{
		if (arg2 && *arg2)
		{
			const int iWpt = atoi(arg2);

			SET_ORIGIN(pClient->GetPlayer(), WaypointOrigin(iWpt));
		}
	}
	else
	{
		BotMessage(pEntity, 0, "Error: No command issued or command does not exist!");
		showHelp(pEntity);

		return BOT_CVAR_ERROR;
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState BotFunc_AddBot(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	// If numbots > maxbots dont create more

	edict_t* pEntity = nullptr;

	if (!gpGlobals->deathmatch)
	{
		BotMessage(pEntity, 0, "ERROR: Game not started yet!");
		return BOT_CVAR_ERROR;
	}

	if (pClient)
		pEntity = pClient->GetPlayer();

	int iTeam = -1;
	int iClass = -1;

	const char* szName = nullptr;

	int iSkill;

	int iBotIndex = 0;

	while (iBotIndex < MAX_PLAYERS && gBotGlobals.m_Bots[iBotIndex].IsUsed())
		iBotIndex++;

	if (iBotIndex >= 32)
	{
		BotMessage(pEntity, 0, "Error : Max Bots Already Added!!!");
		return BOT_CVAR_ERROR;
	}

	CBot* pBot = &gBotGlobals.m_Bots[iBotIndex];

	if (gBotGlobals.m_iMaxBots != -1 && gBotGlobals.m_iNumBots >= gBotGlobals.m_iMaxBots)
	{
		if (pBot)
		{
			if (pBot->m_Profile.m_iProfileId != 0)
				pBot->Init();
		}

		BotMessage(pEntity, 0, "Can't add bot, Number of bots reached max_bots");
		BotMessage(pEntity, 0, "max bots = %d, numbots = %d", gBotGlobals.m_iMaxBots, gBotGlobals.m_iNumBots, UTIL_GetNumClients());

		return BOT_CVAR_ERROR;
	}

	FILE* pProfileToOpen = nullptr;
	char szBotProfile[10]; // Store integer value of bot profile in string as filename
	int iBotProfile = -1; // Store integer value of bot profile

	const BOOL arg1valid = arg1 && *arg1;
	const BOOL arg2valid = arg2 && *arg2;
	const BOOL arg3valid = arg3 && *arg3;

	///////////////////////////////////
	// get bots previous team / class
	if (!arg1valid)
	{
		iTeam = gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iFavTeam;
	}

	if (!arg2valid)
		iClass = gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iClass;

	if (!arg3valid)
	{
		//...
	}

	//gBotGlobals.m_Bots[iBotIndex].FreeGlobalMemory();
	gBotGlobals.m_Bots[iBotIndex].Init();

	if (arg1valid)
	{
		iTeam = atoi(arg1);

		if (iTeam <= 0 || iTeam > 5)
			iTeam = 5;

		gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iFavTeam = iTeam;
	}
	if (arg2valid)
	{
		iClass = atoi(arg2);

		if (iClass < 0 /*|| iClass > TFC_MAX_CLASSES/*/)
			iClass = RANDOM_LONG(0, 9);

		gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iClass = iClass;
	}

	if (pBot->m_Profile.m_iProfileId == 0)
	{
		/*
		if ( arg2valid )
		{
		szName = gBotGlobals.m_Strings.GetString(arg2);
		}
		else
		szName = gBotGlobals.m_Strings.GetString("RCBot");

		  gBotGlobals.m_Bots[iBotIndex].m_Profile.m_szBotName = szName;

			if ( arg3valid )
			{
			iSkill = atoi(arg3);

			  if ( iSkill < MIN_BOT_SKILL )
			  iSkill = MIN_BOT_SKILL;
			  if ( iSkill > MAX_BOT_SKILL )
			  iSkill = MAX_BOT_SKILL;

				gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iSkill = iSkill;
		}*/

		BotMessage(pEntity, 0, "Adding Bot...");

		// Gather a list of id's in use...

		dataStack<int> l_IdsInUse;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CBot* tempBot = &gBotGlobals.m_Bots[i];

			if (tempBot && tempBot->m_bIsUsed && tempBot->m_pEdict)
				l_IdsInUse.Push(tempBot->m_Profile.m_iProfileId);
		}

		//bot_profile_t *bpBotProfile = &gBotGlobals.m_Bots[iBotIndex].m_Profile;

		char filename[512];
		UTIL_BuildFileName(filename, BOT_PROFILES_FILE);
		FILE* fp1 = fopen(filename, "r");
		/*
		if ( fp1 )
		{
		while ( !feof(fp1) && !pProfileToOpen )
		{
		// pickup a string first (safer)
		fscanf(fp1,"%s\n",szBotProfile);

		  // use atoi to get the integer value
		  // it will be 0 if it is invalid
		  // thus ,if iBotProfile = 0 then
		  // the profile will be ignored
		  iBotProfile = atoi(szBotProfile);

			if ( !iBotProfile )
			{
			// Assume invalid bot profile
			continue;
			}

			  if ( l_IdsInUse.IsMember(&iBotProfile) )
			  {
			  // Id in use
			  continue;
			  }

				// Got the profile number
				// now setup a string to hold the filename to the .ini
				// file for this profile
				strcat(szBotProfile,".ini");

				  UTIL_BuildFileName(szBotProfilePath,"botprofiles",szBotProfile);

					fp2 = fopen(szBotProfilePath,"r");

					  if ( fp2 )
					  {
					  //CanUseIds.Add(iBotProfile);
					  pProfileToOpen = fp2;
					  }
					  else
					  BotMessage(pEntity,0,"Error: Bot Profile : %d.ini Expected, Profile Not Found",iBotProfile);
					  }

						fclose(fp1);
						}
		*/
		if (fp1)
		{
			char szBotProfilePath[128];
			dataUnconstArray<int> CanUseIds;
			while (!feof(fp1) && !pProfileToOpen)
			{
				// pickup a string first (safer)
				fscanf(fp1, "%s\n", szBotProfile);

				// use atoi to get the integer value
				// it will be 0 if it is invalid
				// thus ,if iBotProfile = 0 then
				// the profile will be ignored
				iBotProfile = atoi(szBotProfile);

				if (!iBotProfile)
				{
					// Assume invalid bot profile
					continue;
				}

				if (l_IdsInUse.IsMember(iBotProfile))
				{
					// Id in use
					continue;
				}

				// Got the profile number
				// now setup a string to hold the filename to the .ini
				// file for this profile
				strcat(szBotProfile, ".ini");

				UTIL_BuildFileName(szBotProfilePath, "botprofiles", szBotProfile);

				FILE* fp2 = fopen(szBotProfilePath, "r");

				if (fp2)
				{
					CanUseIds.Add(iBotProfile);

					fclose(fp2);
				}
				else
					BotMessage(pEntity, 0, "Error: Bot Profile : %d.ini Expected, Profile Not Found", iBotProfile);
			}

			fclose(fp1);

			if (!CanUseIds.IsEmpty())
			{
				iBotProfile = CanUseIds.Random();

				sprintf(szBotProfile, "%d.ini", iBotProfile);

				UTIL_BuildFileName(szBotProfilePath, "botprofiles", szBotProfile);

				pProfileToOpen = fopen(szBotProfilePath, "r");

				CanUseIds.Clear();
			}
		}

		l_IdsInUse.Destroy();

		if (pProfileToOpen == nullptr)
		{
			BotMessage(pEntity, 0, "Error: No Bot Profiles have been found that are free...");
			return BOT_CVAR_ERROR;
		}
		//}
		//else
		//{
		//  BotMessage(pEntity,0,"Error: No Bot Profiles have been found that are free...");
		//  return BOT_CVAR_ERROR;
		//Create profile??
		//}
	}
	else
	{
		char szProfileToOpen[128];

		iBotProfile = pBot->m_Profile.m_iProfileId;
		iTeam = pBot->m_Profile.m_iFavTeam;
		iClass = pBot->m_Profile.m_iClass;

		sprintf(szBotProfile, "%d.ini", iBotProfile);

		UTIL_BuildFileName(szProfileToOpen, "botprofiles", szBotProfile);

		pProfileToOpen = fopen(szProfileToOpen, "r");
	}

	if (pProfileToOpen && iBotProfile != -1)
	{
		BotFunc_InitProfile(&gBotGlobals.m_Bots[iBotIndex].m_Profile);
		gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iProfileId = iBotProfile;
		BotFunc_ReadProfile(pProfileToOpen, &gBotGlobals.m_Bots[iBotIndex].m_Profile);
		fclose(pProfileToOpen);

		if (iTeam != -1)
			gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iFavTeam = iTeam;
		if (iClass != -1)
			gBotGlobals.m_Bots[iBotIndex].m_Profile.m_iClass = iClass;
	}
	else
	{
		BotMessage(nullptr, 0, "Error: Can't read bot's profile!!!");
		return BOT_CVAR_ERROR;
	}

	edict_t* pEdict = (*g_engfuncs.pfnCreateFakeClient)(gBotGlobals.m_Bots[iBotIndex].m_Profile.m_szBotName);

	if (FNullEnt(pEdict))
	{
		BotMessage(pEntity, 0, "Maximum Players Reached, Can't Create Another Bot!");
		return BOT_CVAR_ERROR;
	}

	char ptr[128];  // allocate space for message from ClientConnect
	char* sInfoBuffer; // Bots infobuffer
	int index; // bots edict index

	pEdict->v.frags = 0; // reset his frag count

#ifdef RCBOT_META_BUILD
	CALL_GAME_ENTITY(PLID, "player", VARS(pEdict)); // Olo
#else
	// create the player entity by calling MOD's player function
	// (from LINK_ENTITY_TO_CLASS for player object)

	player(VARS(pEdict));
#endif

	index = ENTINDEX(pEdict);

#ifdef RCBOT_META_BUILD
	sInfoBuffer = GET_INFOKEYBUFFER(pEdict);
#else
	sInfoBuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)(pEdict);
#endif

	// pointer to new bot
	pBot = &gBotGlobals.m_Bots[iBotIndex];

	char* c_skin = nullptr;

	char newteam[64];

	newteam[0] = 0;

	if (gBotGlobals.IsMod(MOD_TS) && gBotGlobals.m_bTeamPlay)
	{
		dataUnconstArray<char*> teams;

		const char* teamlist = CVAR_GET_STRING("mp_teamlist");

		int i = 0;
		int j = 0;

		const int len = strlen(teamlist);

		while (i < len)
		{
			if (i == len - 1 || teamlist[i] == ';')
			{
				if (i == len - 1)
				{
					newteam[j++] = teamlist[i];
					newteam[j] = 0;
				}
				teams.Add(strdup(newteam));
				j = 0;
			}
			else
			{
				newteam[j++] = teamlist[i];
				newteam[j] = 0;
			}

			i++;
		}

		newteam[0] = 0;

		if (gBotGlobals.m_iForceTeam > 0)
		{
			if (gBotGlobals.m_iForceTeam <= teams.Size())
				strcpy(newteam, teams.ReturnValueFromIndex(gBotGlobals.m_iForceTeam - 1));
		}

		if (newteam[0] == 0)
			strcpy(newteam, teams.Random());

		for (i = 0; i < teams.Size(); i++)
			free(teams.ReturnValueFromIndex(i));

		teams.Clear();

		c_skin = newteam;
	}
	else if (pBot->m_Profile.m_szModel)
	{
		c_skin = pBot->m_Profile.m_szModel;
	}
	else if (!gBotGlobals.m_uaBotModels.IsEmpty())
	{
		c_skin = gBotGlobals.m_uaBotModels.Random();
	}

	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "rate", "5000");
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "cl_updaterate", "30");
	//(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "cl_lw", "1"); // jeefo
	//(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "cl_lc", "1");
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "cl_dlmax", "128");

	if (!gBotGlobals.IsMod(MOD_TS)) //No longer required for TS 3.0? [APG]RoboCop[CL]
		(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "_vgui_menus", "0");
	else
		(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "_vgui_menus", "1");

	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "_ah", "0");
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "dm", "0");
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "tracker", "0");
	//(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "name", gBotGlobals.m_Bots[iBotIndex].m_Profile.m_szBotName);

	if (c_skin)
	{
		(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "model", c_skin);
	}

	pBot->SetEdict(pEdict);

	pBot->setupDataStructures();

	char szColour[5];

	sprintf(szColour, "%d", pBot->m_Profile.m_iTopColour);
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "topcolor", szColour);
	sprintf(szColour, "%d", pBot->m_Profile.m_iBottomColour);
	(*g_engfuncs.pfnSetClientKeyValue)(index, sInfoBuffer, "bottomcolor", szColour);

#ifdef RCBOT_META_BUILD
	// update joining clients because metamod wont call engine client connect?
	//gBotGlobals.m_iJoiningClients++;
	MDLL_ClientConnect(pEdict, szName, "127.0.0.1", ptr);
	ClientConnect(pEdict, szName, "127.0.0.1", ptr);

	MDLL_ClientPutInServer(pEdict);
	ClientPutInServer(pEdict);

	gBotGlobals.m_Clients.ClientConnected(pEdict);
#else
	ClientConnect(pEdict, szName, "127.0.0.1", ptr);
	// Pieter van Dijk - use instead of DispatchSpawn() - Hip Hip Hurray!
	ClientPutInServer(pEdict);
#endif

	// dont forget to increment number of bots!!!
	gBotGlobals.m_iNumBots++;

	// give some time for next client command
	gBotGlobals.m_fNextJoinTeam = gpGlobals->time + 0.2;

	iSkill = pBot->m_Profile.m_iSkill;

	// Workout reaction time.
	//pBot->m_fReactionTime = BOT_DEFAULT_REACTION_TIME;

	pBot->m_bIsUsed = TRUE;
	pBot->m_fJoinServerTime = gpGlobals->time;

	pBot->m_iRespawnState = RESPAWN_IDLE;

	pBot->SpawnInit(FALSE);

	// Get the bots name, could be duplicated in-game and changed by the engine
	pBot->m_szBotName = gBotGlobals.m_Strings.GetString(STRING(pEdict->v.netname));

	// Try and greet someone or everyone

	if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_CHATTING))
	{
		CClient* greetClient;
		if ((greetClient = pBot->m_Profile.m_Rep.GetRandomClient(1)) != nullptr)
		{
			pBot->BotChat(BOT_CHAT_GREETINGS, greetClient->GetPlayer());
		}
		else if (RANDOM_LONG(0, 10) < pBot->m_Profile.m_Rep.AverageRepOnServer())
		{
			pBot->BotChat(BOT_CHAT_GREETINGS);
		}
	}

	extern int num_waypoints;

	if (num_waypoints == 0)
	{
		if (pClient)
		{
			pClient->AddNewToolTip(BOT_TOOL_TIP_NO_WAYPOINTS);
		}
	}

	return BOT_CVAR_ACCESSED;
}

eBotCvarState CAddBotCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	return BotFunc_AddBot(pClient, arg1, arg2, arg3, arg4);
}

void CAddBotCommand::showHelp(edict_t* pEntity)
{
	BotMessage(pEntity, 0, "Add bot command help");
	BotMessage(pEntity, 0, "--------------------");
	BotMessage(pEntity, 0, "You can add a bot by typing...");
	BotMessage(pEntity, 0, "\"rcbot addbot\" (creates a random bot)");

	if (gBotGlobals.IsNS())
	{
		BotMessage(pEntity, 0, "\"rcbot addbot 1\" (creates a marine bot)");
		BotMessage(pEntity, 0, "\"rcbot addbot 2\" (creates an alien bot)");
	}
}

eBotCvarState CHelpCommand::action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	edict_t* pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	//find command in list
	if (!arg1 || !*arg1)
	{
		BotMessage(pEntity, 0, "No command specified for help");
		BotMessage(pEntity, 0, "Usage: rcbot help <command name>");

		return BOT_CVAR_ERROR;
	}
	else
	{
		CBotCvar* theCommand = gBotGlobals.m_BotCvars.GetCvar(arg1);

		if (theCommand == nullptr)
		{
			BotMessage(pEntity, 0, "command specified does not exist!\nAvailable commands are");

			gBotGlobals.m_BotCvars.PrintCommands(pEntity);
		}
		else
		{
			theCommand->showHelp(pEntity);
		}

		return BOT_CVAR_ACCESSED;
	}
}

//////////////////////////////////////////////////////////////////////////////
// CVAR HANDLING

void CBotCvar::setupCommand(const char* szCvarName, const int iAccessLevel, const BOOL bCanUseOnDedicatedServer)
{
	m_szCvarName = gBotGlobals.m_Strings.GetString(szCvarName);

	//m_fpCvarFunction = fpCvarFunction;

	m_bCanUseOnDedicatedServer = bCanUseOnDedicatedServer;
	m_iAccessLevel = iAccessLevel;
}

void CBotCvarList::AddCvar(CBotCvar* theCommand)
{
	m_Cvars.Push(theCommand);
}

void SetupCommands()
{
	/////////////////////
	// CVARS & COMMANDS

	gBotGlobals.m_BotCvars.AddCvar(new CAddBotCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CWaypointCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CPathWaypointCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CDebugCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CWaypointMenuCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CBotMenuCommand());
	// engine command also, so requires 0 access level
	gBotGlobals.m_BotCvars.AddCvar(new CMenuSelectCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CConfigCommand());
	// obviously requires 0 access to get higher access!
	gBotGlobals.m_BotCvars.AddCvar(new CSetPassCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CUtilCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CBotSquadCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CSquadMenuCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CBotCamCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CAutoWaypointCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CHelpCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CUsersCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CDebugBotCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CDebugEntCommand());
	gBotGlobals.m_BotCvars.AddCvar(new CRemoveBotCommand());
}

void RCBot_ServerCommand()
{
	const char* pcmd = CMD_ARGV(1);
	const char* arg1 = CMD_ARGV(2);
	const char* arg2 = CMD_ARGV(3);
	const char* arg3 = CMD_ARGV(4);
	const char* arg4 = CMD_ARGV(5);

	eBotCvarState iState = BOT_CVAR_NOTEXIST;

	gBotGlobals.m_CurrentHandledCvar = gBotGlobals.m_BotCvars.GetCvar(pcmd);

	if (gBotGlobals.m_CurrentHandledCvar)
	{
		// use access level max
		if (IS_DEDICATED_SERVER())
		{
			if (gBotGlobals.m_CurrentHandledCvar->error())
			{
				BotMessage(nullptr, 0, "Error: Can't use this command on the dedicated server\n");
				return;
			}

			BotMessage(nullptr, 0, "Accessing debug commands : detected dedicated server");

			iState = gBotGlobals.m_CurrentHandledCvar->action(nullptr, arg1, arg2, arg3, arg4);
		}
		else
		{
			CClient* pListenServerClient = gBotGlobals.m_Clients.GetClientByEdict(gBotGlobals.m_pListenServerEdict);

			if (pListenServerClient)
				iState = gBotGlobals.m_CurrentHandledCvar->action(pListenServerClient, arg1, arg2, arg3, arg4);
			else
			{
				BotMessage(nullptr, 0, "Access to debug commands failed : Detected listen server with no listenserver client");
			}
		}
	}

	if (iState == BOT_CVAR_ACCESSED)
	{
		return;
	}
	else if (iState == BOT_CVAR_ERROR)
	{
		BotMessage(gBotGlobals.m_pListenServerEdict, 0, "The bot command %s returned an error!\n", pcmd);
	}
	else
	{
		if (*pcmd)
			BotMessage(gBotGlobals.m_pListenServerEdict, 0, "The bot command %s does not exist!\n", pcmd);
		else
			BotMessage(gBotGlobals.m_pListenServerEdict, 0, "No command issued!\n");

		BotMessage(gBotGlobals.m_pListenServerEdict, 0, "Available SUB-commands are:\n");

		gBotGlobals.m_BotCvars.PrintCommands(gBotGlobals.m_pListenServerEdict);
	}
}

void GiveTSWeapon(edict_t* pEdict, int id)
{
	edict_t* weapon = g_engfuncs.pfnCreateNamedEntity(MAKE_STRING("weapon_tsgun"));

	weapon->v.spawnflags = 1073741824;
	weapon->v.movetype = 12;
	weapon->v.effects = 128;

	weapon->v.aiment = pEdict;
	weapon->v.owner = pEdict;

	DispatchTouch(pEdict, weapon);
}