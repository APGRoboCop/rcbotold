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
 // bot_client.cpp
 //
 //////////////////////////////////////////////////
 //
 // Bot net message functions
 //

#include "extdll.h"
#include "enginecallback.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"

#ifdef _WIN32
#define strcmpi _strcmpi
#endif

#define DMG_GENERIC			0			// generic damage was done
#define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FREEZE			(1 << 4)	// frozen
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_CLUB			(1 << 7)	// crowbar, punch, headbutt
#define DMG_SHOCK			(1 << 8)	// electric shock
#define DMG_SONIC			(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.
#define DMG_DROWN			(1 << 14)	// Drowning
// time-based damage
#define DMG_TIMEBASED		(~(0x3fff))	// mask for time-based damage

#define DMG_PARALYZE		(1 << 15)	// slows affected creature down
#define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
#define DMG_POISON			(1 << 17)	// blood poisioning
#define DMG_RADIATION		(1 << 18)	// radiation exposure
#define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
#define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
#define DMG_SLOWBURN		(1 << 21)	// in an oven
#define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#define DMG_MORTAR			(1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)

// these are the damage types that are allowed to gib corpses
#define DMG_GIB_CORPSE		( DMG_CRUSH | DMG_FALL | DMG_BLAST | DMG_SONIC | DMG_CLUB )

// these are the damage types that have client hud art
#define DMG_SHOWNHUD		(DMG_POISON | DMG_ACID | DMG_FREEZE | DMG_SLOWFREEZE | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK)
#endif

#include "generic_class.h"
#include "bot.h"
#include "bot_client.h"
#include "bot_weapons.h"

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_FREEZE | DMG_FALL | \
                       DMG_DROWN | DMG_NERVEGAS | DMG_RADIATION | \
                       DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN | \
                       DMG_SLOWFREEZE | 0xFF000000)

//extern bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions

extern CBotGlobals gBotGlobals;

void SetupNetMessages()
{
#ifdef _DEBUG
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_RoundTime());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_TSMessage());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_PTakeDam());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_Health());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_Frags());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_TeamInfo());
#endif

	gBotGlobals.m_NetAllMessages.AddNetMessage(new BotClient_TS_DelObj());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_Objective());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_State());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_ClipInfo());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_WeaponInfo());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_TS_WStatus());

	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_WeaponList());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_CurrentWeapon());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_AmmoX());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_AmmoPickup());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_WeaponPickup());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_ItemPickup());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_Health());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_Battery());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_Damage());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_ScreenFade());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_TeamScore());
	// DMC Items
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_DMC_QuakeItem());
	// get vgui message
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_VGUIMenu());
	// ns 3.0 tech upgrades??
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_TechSlots());
	// need to catch message to show a menu
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_ShowMenu());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_Commndr());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_HudText());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_SetOrder());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_SetTech());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_NS_AlienInfo());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_BG_MakeMessage());
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_ScreenShake());
	//gBotGlobals.m_NetEntityMessages.AddNetMessage(BotClient_NS_Progress,			"Progress",		MOD_NS);

	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_TextMessage());
	// say to team etc.?
	gBotGlobals.m_NetEntityMessages.AddNetMessage(new BotClient_Generic_SayText());

	// MESSAGES to all players ?
	gBotGlobals.m_NetAllMessages.AddNetMessage(new BotClient_Generic_TeamScore());
	gBotGlobals.m_NetAllMessages.AddNetMessage(new BotClient_All_Generic_SayText());
	gBotGlobals.m_NetAllMessages.AddNetMessage(new BotClient_Generic_DeathMessage());
	gBotGlobals.m_NetAllMessages.AddNetMessage(new BotClient_NS_HudText());
}

void BotClient_TS_Health::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
		break;
	case 1:
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_TeamInfo::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:		// player id ( byte)
		break;
	case 1: // team name (string)
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_State::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
		gBotGlobals.m_Bots[iIndex].m_iTS_State = static_cast<eTS_State>(*static_cast<int*>(p));
		break;
	case 1:
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_Frags::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
		break;
	case 1:
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_RoundTime::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
	{
		//if ( gBotGlobals.m_Bots[iIndex].IsUsed() )
		//	gBotGlobals.m_Bots[iIndex].SpawnInit(false);
	}
	break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_Objective::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	static Vector origin;
	static int id;
	static char* name;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0://x
		origin.x = POINTER_TO_FLOAT(p);
		break;
	case 1://y
		origin.y = POINTER_TO_FLOAT(p);
		break;
	case 2://z
		origin.z = POINTER_TO_FLOAT(p);
		break;
	case 3://index
		id = POINTER_TO_INT(p);
		break;
	case 4://name
		name = static_cast<char*>(p);

		gBotGlobals.m_Bots[iIndex].UpdateCondition(BOT_CONDITION_DONT_CLEAR_OBJECTIVES);
		gBotGlobals.m_Bots[iIndex].m_TSObjectives.Add(TSObjective(id, origin, name));

		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_WStatus::execute(void* p, int iIndex)
{
	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (gBotGlobals.m_iCurrentMessageState++)
	{
	case 1:
		if (!*static_cast<int*>(p))
		{
			CBot* pBot = &gBotGlobals.m_Bots[iIndex];
			pBot->m_pCurrentWeapon = pBot->m_Weapons.GetWeapon(36);
			//ALERT(at_console,"%s lost a weapon!!!\n",gBotGlobals.m_Bots[iIndex].m_szBotName);
		}
		break;
	}
}

void BotClient_TS_ClipInfo::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
	{
		try
		{
			CBot* pBot = &gBotGlobals.m_Bots[iIndex];

			if (pBot->m_pCurrentWeapon)
			{
				pBot->m_pCurrentWeapon->UpdateWeapon(*static_cast<int*>(p));
				pBot->m_fLastBulletFired = gpGlobals->time;

				if (!*static_cast<int*>(p) && pBot->m_pCurrentWeapon->getReserve() <= pBot->m_pCurrentWeapon->getMaxClip())
				{				// drop weapon
					gBotGlobals.m_Bots[iIndex].AddPriorityTask(CBotTask(BOT_TASK_DROP_WEAPON));
				}
			}
		}

		catch (...)
		{
			//blah
			return;
		}
	}
	break;
	case 1:
	{
	}
	break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_DelObj::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr)
	{
		return;
	}

	switch (POINTER_VALUE(state))
	{
	case 0:
	{
		const int id = *static_cast<int*>(p);

		for (int i = 0; i < 32; i++)
		{
			CBot* pBot = &gBotGlobals.m_Bots[i];

			if (pBot->IsUsed())
			{
				//delete gBotGlobals.m_Bots[iIndex].m_TSObjectives.top();
				if (!pBot->m_TSObjectives.IsEmpty())
				{
					for (int j = 0; j < pBot->m_TSObjectives.Size(); j++)
					{
						TSObjective* pObj = pBot->m_TSObjectives.ReturnPointerFromIndex(j);

						if (pObj->getID() == id)
						{
							pBot->m_TSObjectives.RemoveByPointer(pObj);
							pBot->m_FailedGoals.Destroy();
							break;
						}
					}
				}
			}
		}
	}
	break;
	case 1:
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_TS_WeaponInfo::execute(void* p, int iIndex)
{
	static int clip;
	static int ammo;
	static int id;

	if (p == nullptr || iIndex == -1)
	{
		return;
	}
	//from tsxmod
	CBot* pBot = &gBotGlobals.m_Bots[iIndex];
	switch (gBotGlobals.m_iCurrentMessageState)
	{
	case 0:
		id = *static_cast<int*>(p);

		if (!id)
			id = 36; // kung fu

		break;
	case 1:
		clip = *static_cast<int*>(p);
		break;
	case 2:
		ammo = *static_cast<int*>(p);
		break;
	case 3:
	{
		static int wpn;
		int mode = *static_cast<int*>(p);

		gBotGlobals.m_Weapons.AddWeapon(id, "tsweapon", ammo, ammo, 0, 0, 0, 0, 0);

		pBot->m_Weapons.AddWeapon(id);
		pBot->m_pCurrentWeapon = pBot->m_Weapons.GetWeapon(wpn);
		pBot->m_pCurrentWeapon->UpdateWeapon(clip);
		pBot->m_pCurrentWeapon->SetWeapon(id, nullptr);
		pBot->m_iBotWeapons |= id;
		pBot->m_pCurrentWeapon->setHasWeapon(true);
		pBot->m_pCurrentWeapon->setReserve(ammo);
		pBot->m_pCurrentWeapon->checkMaxClip(clip);

		if (!ammo && !clip)
		{				// drop weapon
			gBotGlobals.m_Bots[iIndex].AddPriorityTask(CBotTask(BOT_TASK_DROP_WEAPON));
		}
		//pBot->m_pCurrentWeapon->setAmmoArray(ammo);
	}
	break;
	}
	gBotGlobals.m_iCurrentMessageState++;
}

void BotClient_NS_TechSlots::execute(void* p, int iIndex)
{
	static int iSlots;

	if (p == nullptr || iIndex == -1)
	{
		iSlots = 0;
		return;
	}

	short int* iState = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_TO_INT(iState))
	{
	case 0:
		iSlots = POINTER_TO_INT(p);
		break;
	case 1:
		break;
	default:
		break;
	}

	iSlots = POINTER_TO_INT(p);

	POINTER_INCREMENT_VALUE(iState);
}

void BotClient_BG_MakeMessage::execute(void* p, int iIndex)
{
	static int iSenderId = 0;
	static int iMsg = 0;

	if (p == nullptr || iIndex == -1)
		return;

	short int* iState = &gBotGlobals.m_iCurrentMessageState;

	if (POINTER_TO_INT(iState) == 0)
	{
		iSenderId = POINTER_TO_INT(p);
	}
	else
	{
		edict_t* pSender = INDEXENT(iSenderId);

		// not a bot
		if (pSender && UTIL_GetBotPointer(pSender) == nullptr)
		{
			CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pSender);

			if (pClient)
			{
				switch (POINTER_TO_INT(p))
				{
				case BOT_BG_MSG_YES:
					break;
				case BOT_BG_MSG_NO:
					break;
				case BOT_BG_MSG_FOLLOW:
				case BOT_BG_MSG_RALLY:
					// make squad
					BotFunc_MakeSquad(pClient);
					// follow leader hack :)
					pClient->m_iLastMenuItemSelected = 4;
					BotMenu_Func_SquadMode1(pClient);
					break;
				case BOT_BG_MSG_LEAVE:
					BotMenu_Func_Squad_Leave(pClient);
					break;
				case BOT_BG_MSG_UNDERFIRE:
					break;
				case BOT_BG_MSG_SPREADOUT:
					// enlarge spread
					pClient->m_iLastMenuItemSelected = 1; // hack :)
					BotMenu_Func_Squad_Spread(pClient);
					break;
				case BOT_BG_MSG_BATTLECRY:
					break;
				case BOT_BG_MSG_MEDIC:
					break;
				case BOT_BG_MSG_ADVANCE:
					BotMenu_Func_Squad_Remove(pClient);
					break;
				case BOT_BG_MSG_RETREAT:
					break;
				case BOT_BG_MSG_HALT:
					break;
				case BOT_BG_MSG_LINEUP:
					// make squad
					BotFunc_MakeSquad(pClient);
					// line formation
					pClient->m_iLastMenuItemSelected = 2; // hack >:)
					BotMenu_Func_Squad_Form(pClient);
					break;
				case BOT_BG_MSG_MAKEREADY:
					break;
				case BOT_BG_MSG_PRESENT_ARMS:
					break;
				case BOT_BG_MSG_FIRE:
				case BOT_BG_MSG_CEASEFIRE:
					pClient->m_iLastMenuItemSelected = 1; // hack for open/hold fire
					BotMenu_Func_SquadMode1(pClient);
					break;
				default:
					break;
				}
			}
		}
	}

	POINTER_INCREMENT_VALUE(iState);
}

void BotClient_Generic_TextMessage::execute(void* p, const int iIndex)
{
	CBot* pBot;
	short int* iState = &gBotGlobals.m_iCurrentMessageState;

	/*typedef enum
	{
		TXTMSG_TYPE_NONE;
		TXTMSG_TYPE_BUILD_TELE_ENTRANCE,
		TXTMSG_TYPE_BUILD_TELE_EXIT,
		TXTMSG_TYPE_BUILD_SENTRY
	}eTextMessageType;
	static BOOL bCheckTeleEntrance = false;
	static BOOL bCheckTeleExit = false;
	static BOOL bBuiltSentry = false;*/

	//int iType;

	/*if (p == nullptr)
	{
		bCheckTeleEntrance = false;
		bCheckTeleExit = false;
		bBuiltSentry = false;
		bSentryDestroyed = false;
		TeleExitDestroyed = false;
		bTeleEntranceDestroyed = false;

		return;
	}*/

	switch (POINTER_TO_INT(iState))
	{
	case 0:
		POINTER_INCREMENT_VALUE(iState);
		break;

	case 1:
	{
		// not a MSG_ALL message
		//if ( iIndex != -1 )
		//{
		if (iIndex != -1)
			pBot = &gBotGlobals.m_Bots[iIndex];
		else
			pBot = nullptr;

		/*if (gBotGlobals.IsMod(MOD_TFC))
		{
			const char* szMessage = POINTER_TO_STRING(p);

			CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(gBotGlobals.m_pMessageEntity);

			if (FStrEq(szMessage, "#Teleporter_Entrance_Built"))
			{
				bCheckTeleEntrance = true;
			}
			else if (FStrEq(szMessage, "#Teleporter_Exit_Built"))
			{
				bCheckTeleExit = true;
			}
			else if (FStrEq(szMessage, "#Teleporter_Exit_Destroyed"))
			{
				if (pClient)
					pClient->TFCTeleExitDestroyed();
			}
			else if (FStrEq(szMessage, "#Teleporter_Entrance_Destroyed"))
			{
				if (pClient)
					pClient->TFCTeleEntranceDestroyed();
			}
			else if (FStrEq(szMessage, "#Teleporter_entrance_dismantle"))
			{
				if (pClient)
					pClient->TFCTeleEntranceDestroyed();
			}
			else if (FStrEq(szMessage, "#Teleporter_exit_dismantle"))
			{
				if (pClient)
					pClient->TFCTeleExitDestroyed();
			}
			else if (FStrEq(szMessage, "#Sentry_built"))
			{
				bBuiltSentry = true;
			}
			else if (FStrEq(szMessage, "#Sentry_upgrade"))  // sentry gun upgraded
			{
				if (pClient)
				{
					if (pClient->getTFCSentry())
						pClient->getTFCSentry()->v.iuser1++;
				}

				//pBot->UpgradedSentry();
			}
			else if (FStrEq(szMessage, "#Sentry_destroyed") || FStrEq(szMessage, "#Sentry_dismantle"))  // sentry gun destroyed
			{
				if (pClient)
				{
					pClient->TFCSentryDestroyed();
				}

				//	pBot->SentryDestroyed();
			}
			else if (pBot)
			{
				if ( FStrEq(szMessage, "#Sentry_sbar") )
				{
				ALERT(at_console,"sbar\n");
				}
					else 
				if (FStrEq(szMessage, "#Build_nometal") || FStrEq(szMessage, "#Build_notenoughmetal"))
				{
					pBot->NeedMetal();
				}
				else if (FStrEq(szMessage, "#Build_stop"))
				{
					pBot->StoppedBuilding();
				}
				else if (FStrEq(szMessage, "#Build_air") || FStrEq(szMessage, "#Build_nobuild") || FStrEq(szMessage, "#Build_noroom"))
				{
					pBot->CantBuildHere();
				}
				else if (FStrEq(szMessage, "#Dispenser_used")) // enemy using disp
				{
					pBot->AddPriorityTask(CBotTask(BOT_TASK_TFC_DETONATE_DISPENSER));
				}
				else if (FStrEq(szMessage, "#Dispenser_finish"))  // dispenser built
				{
					pBot->DispenserBuilt();
				}
				else if (FStrEq(szMessage, "#Dispenser_destroyed"))  // dispenser destroyed
				{
					pBot->DispenserDestroyed();
				}
				else if (FStrEq(szMessage, "#Disguise_now"))//Disguise_start") )
				{
					pBot->Disguised();
				}
				else if (FStrEq(szMessage, "#Disguise_stop") || FStrEq(szMessage, "#Disguise_Lost")) // lost disguise
				{
					pBot->LostDisguise();
				}
			}
		}

		//}

		POINTER_INCREMENT_VALUE(iState);
		break;
	}
	case 2:

	{
		// begin- foxbot
		char builder[255];
		strcpy(builder, static_cast<char*>(p));
		// end

		edict_t* pent = gBotGlobals.m_Clients.FindClient(builder);

		if (pent)
		{
			CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pent);

			if (pClient)
			{
				if (bCheckTeleEntrance)
				{
					pClient->setTFCTeleEntrance(UTIL_CheckTeleEntrance(pent->v.origin, pClient->getTFCTeleExit(), pent));
				}
				else if (bCheckTeleExit)
				{
					pClient->setTFCTeleExit(UTIL_CheckTeleExit(pent->v.origin, pent, pClient->getTFCTeleEntrance()));
				}
				else if (bBuiltSentry)
				{
					edict_t* sentry = nullptr;

					pClient->clearIgnoreSentries();

					while ((sentry = UTIL_FindEntityByClassname(sentry, "building_sentrygun")) != nullptr)
					{
						pClient->addIgnoreSentry(sentry);
					}

					pClient->checkBuiltSentry();
				}
			}
		}*/
	}
	POINTER_INCREMENT_VALUE(iState);
	break;
	}
}

//
// global (all)
void BotClient_All_Generic_SayText::execute(void* p, int iIndex)
{
	static int iSenderId;
	//	char *szMsg;
	short int* iState = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr)
		return;

	if (POINTER_VALUE(iState) == 0)
		iSenderId = POINTER_TO_INT(p);
	else
	{
	}

	POINTER_INCREMENT_VALUE(iState);
}
//
// Happens when someone says a message
// ... this does not happen to bots in NS 3.. WHY??!
// (sticking to "say" / "say_team" catching in clientcommand
void BotClient_Generic_SayText::execute(void* p, int iIndex)
{
	static int iSenderId;
	//	char *szMsg;
	short int* iState = &gBotGlobals.m_iCurrentMessageState;

	if (p == nullptr)
		return;

	if (POINTER_VALUE(iState) == 0)
		iSenderId = POINTER_TO_INT(p);
	else
	{
		/*edict_t *pSender;

		pSender = INDEXENT(iSenderId);

		if ( pSender )
		{
			int iLen;
			char *pName;

			pName = (char*)STRING(pSender->v.netname);
			szMsg = POINTER_TO_STRING(p);

			if ( szMsg && *szMsg )
			{
				szMsg = strstr(szMsg,pName);

				if ( szMsg && *szMsg )
				{
					// skip name
					szMsg = szMsg + strlen(pName);

					szMsg = strstr(szMsg,": ");

					if ( szMsg && *szMsg )
					{
						// skip colon
						szMsg = szMsg + 2;

						iLen = strlen(szMsg);

						if ( szMsg[iLen-1] == '\n' )
							szMsg[iLen-1] = 0;
					}

					// do stuff with msg
				}
			}
		}*/
	}

	POINTER_INCREMENT_VALUE(iState);
}

// PM - use QItems!
void BotClient_DMC_QuakeItem::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	// new DMC Weapon info
	CBot* pBot = &gBotGlobals.m_Bots[iIndex];

	pBot->m_iQuakeItems = POINTER_TO_INT(p);

	// remove all weapons
	pBot->m_Weapons.RemoveWeapons();

	for (int i = 0; i <= 7; i++)
	{
		// re add them from the info
		if (pBot->m_iQuakeItems & 1 << i)
			pBot->m_Weapons.AddWeapon(i + 1);	// dmc weapon ids start at 0.. add 1.
	}
}

void BotClient_Generic_VGUIMenu::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
	{
		return;
	}
	/*
	#ifdef _DEBUG

		FILE *fp = fopen ("bot_vgui.txt","a");

		if ( fp )
		{
			fprintf(fp,"\n new Vgui menu : index = %d",iIndex);

			fclose(fp);
		}

	#endif
	*/
	// update bots current vgui menu that is on its screen
	CBot* pBot = &gBotGlobals.m_Bots[iIndex];

	pBot->SetVGUIState(POINTER_TO_INT(p));
}

/*

  Alien Info

  for Natural Selection

  catches new upgrades and hive info

  */
void BotClient_NS_AlienInfo::execute(void* p, int iIndex)
{
	static short int state = 0;
	static int message_type = -1;
	static int num_loops = 0;
	static int got_loops = 0;

	// HiveInfo

	static int theHiveInfoChanged = 0;
	static int theSentCoords = 0;

	static float theX = 0;
	static float theY = 0;
	static float theZ = 0;

	static int mStatus = 0;
	static int mUnderAttack = 0;
	static int mTechnology = 0;

	// End - HiveInfo

	static int read_states = 0;

	if (p == nullptr || iIndex == -1)
	{
		if (!p && gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
			BotMessage(nullptr, 0, "---- END Alien Info Message ----");

		message_type = -1;
		num_loops = 0;
		got_loops = 0;

		theHiveInfoChanged = 0;
		theSentCoords = 0;

		theX = 0;
		theY = 0;
		theZ = 0;

		mStatus = 0;
		mUnderAttack = 0;
		mTechnology = 0;

		state = 0;
		read_states = 0;

		return;
	}

	if (message_type == -1)
	{
		if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
			BotMessage(nullptr, 0, "---- BEGIN Alien Info Message ----");

		message_type = *static_cast<int*>(p);// 0 means upgrades, 1 means hive info

		if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
		{
			if (!message_type)
				BotMessage(nullptr, 0, "message_type : UPGRADE INFO");
			else
				BotMessage(nullptr, 0, "message_type : HIVE INFO");
		}

		num_loops = got_loops = state = 0;

		theHiveInfoChanged = 0;
		theSentCoords = 0;

		theX = 0;
		theY = 0;
		theZ = 0;

		mStatus = 0;
		mUnderAttack = 0;
		mTechnology = 0;

		read_states = 0;
	}
	else
	{
		if (message_type == 0)
		{
			if (state == 0)
			{
				num_loops = *static_cast<int*>(p);
				state++;

				read_states = 0;

				gBotGlobals.m_bCanUpgradeDef = false;
				gBotGlobals.m_bCanUpgradeSens = false;
				gBotGlobals.m_bCanUpgradeMov = false;

				if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
					BotMessage(nullptr, 0, "num upgrades : %d", num_loops);
			}
			else if (got_loops < num_loops)
			{
				if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
				{
					//BotMessage(NULL,0,"reading upgrade info : %d...",got_loops);
					BotMessage(nullptr, 0, "Upgrade Info : %d...", *static_cast<int*>(p));
				}

				switch (*static_cast<int*>(p))
				{
				case 1:
					gBotGlobals.m_bCanUpgradeDef = true;
					break;
				case 2:
					gBotGlobals.m_bCanUpgradeSens = true;
					break;
				case 3:
					gBotGlobals.m_bCanUpgradeMov = true;
					break;
				}

				// READ UPGRADE INFO
				got_loops++;
			}
		}
		else if (message_type == 1)
		{
			if (state == 0)
			{
				num_loops = *static_cast<int*>(p);

				state = 1;

				read_states = 0;

				if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
					BotMessage(nullptr, 0, "num hives : %d", num_loops);
			}
			else if (got_loops < num_loops)
			{
				// hive_info = *(int*)p
				//

				if (read_states == 0)
				{
					theHiveInfoChanged = *static_cast<int*>(p);
					read_states++;

					if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
						BotMessage(nullptr, 0, "reading hive info : %d", got_loops);

					if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
						BotMessage(nullptr, 0, "hive info changed : %d", theHiveInfoChanged);
				}
				else if (read_states == 1)
				{
					if (theHiveInfoChanged)
					{
						theSentCoords = *static_cast<int*>(p);
						read_states++;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "the sent coords : %d", theSentCoords);
					}
					else
					{
						got_loops++;
						read_states = 0;
					}
				}
				else if (read_states == 2)
				{
					if (theSentCoords)
					{
						theX = *static_cast<float*>(p);
						read_states++;

						gBotGlobals.m_Hives[got_loops].vOrigin.x = theX;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "the X coord : %0.2f", theX);
					}
					else
					{
						mStatus = *static_cast<int*>(p);
						read_states = 3;

						gBotGlobals.m_Hives[got_loops].mStatus = mStatus;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mStatus : %d", mStatus);
					}
				}
				else if (read_states == 3)
				{
					if (theSentCoords)
					{
						theY = *static_cast<float*>(p);
						read_states++;

						gBotGlobals.m_Hives[got_loops].vOrigin.y = theY;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "the Y coord : %0.2f", theY);
					}
					else
					{
						mUnderAttack = *static_cast<int*>(p);
						read_states = 4;

						gBotGlobals.m_Hives[got_loops].mUnderAttack = mUnderAttack;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mUnderAttack : %d", mUnderAttack);
					}
				}
				else if (read_states == 4)
				{
					if (theSentCoords)
					{
						edict_t* pHive = nullptr;

						theZ = *static_cast<float*>(p);
						read_states++;

						gBotGlobals.m_Hives[got_loops].vOrigin.z = theZ;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "the Z coord : %0.2f", theZ);

						while ((pHive = UTIL_FindEntityByClassname(pHive, "team_hive")) != nullptr)
						{
							if (pHive->v.origin == gBotGlobals.m_Hives[got_loops].vOrigin)
							{
								gBotGlobals.m_Hives[got_loops].pHive = pHive;
								break;
							}
						}
					}
					else
					{
						mTechnology = *static_cast<int*>(p);

						got_loops++;
						read_states = 0;

						gBotGlobals.m_Hives[got_loops].mTechnology = mTechnology;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mTechnology : %d", mTechnology);
					}
				}
				else if (read_states == 5)
				{
					if (theSentCoords)
					{
						mStatus = *static_cast<int*>(p);
						read_states++;

						gBotGlobals.m_Hives[got_loops].mStatus = mStatus;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mStatus : %d", mStatus);
					}
				}
				else if (read_states == 6)
				{
					if (theSentCoords)
					{
						mUnderAttack = *static_cast<int*>(p);
						read_states++;

						gBotGlobals.m_Hives[got_loops].mUnderAttack = mUnderAttack;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mUnderAttack : %d", mUnderAttack);
					}
				}
				else if (read_states == 7)
				{
					if (theSentCoords)
					{
						mTechnology = *static_cast<int*>(p);

						read_states = 0;

						gBotGlobals.m_Hives[got_loops].mTechnology = mTechnology;

						got_loops++;

						if (gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL))
							BotMessage(nullptr, 0, "mTechnology : %d", mTechnology);
					}
				}
			}
		}
	}
}

void BotClient_Generic_TeamScore::execute(void* p, int iIndex)
{
	static int iTeam = 0;
	static int iScore = 0;

	if (p == nullptr)
	{
		if (iTeam >= 0 && iTeam < MAX_TEAMS)
			gBotGlobals.m_iTeamScores[iTeam] = iScore;
		iTeam = 0;
		iScore = 0;
		return;
	}

	if (gBotGlobals.m_iCurrentMessageState == 0)
	{
		const char* team = POINTER_TO_STRING(p);

		if (strcmp(team, "Blue") == 0)
			iTeam = 0;
		else if (strcmp(team, "Red") == 0)
			iTeam = 1;
		else if (strcmp(team, "Green") == 0)
			iTeam = 2;
		else if (strcmp(team, "Yellow") == 0)
			iTeam = 3;
	}
	else if (gBotGlobals.m_iCurrentMessageState == 1)
	{
		iScore = POINTER_TO_INT(p);
	}

	gBotGlobals.m_iCurrentMessageState++;
}

void BotClient_NS_HudText::execute(void* p, int iIndex)
{
	static short int state = 0;

	if (p == nullptr /*|| (iIndex==-1) */)
	{
		state = 0;
		return;
	}

	if (state == 0)
	{
		const char* msg = static_cast<char*>(p);

		if (msg)
		{
			const int length = strlen(msg);

			// If a team has won (used to say "one" duhhh), round is over
			if (!strcmpi(&msg[length - 3], "Won"))
			{
				BotMessage(nullptr, 0, "-----------\nRound Restart\n---------");

				// reset commander
				gBotGlobals.SetCommander(nullptr);
				gBotGlobals.m_bAutoBuilt = false;
				gBotGlobals.m_fAutoBuildTime = 0.0f;

				/*CBot *pBot = UTIL_GetBotPointer(INDEXENT(iIndex));

				if ( gBotGlobals.IsNS() &&  gBotGlobals.IsCombatMap() )
				{
					if ( pBot )
					{
						pBot->m_pCombatBits->setFitness((float)pBot->iCombatFitness);
						gBotGlobals.m_pCombatGA[pBot->m_iCombatTeam].addToPopulation(pBot->m_pCombatBits->copy());
						delete pBot->m_pCombatBits;
						pBot->m_pCombatBits = NULL;

						if ( gBotGlobals.m_pCombatGA[pBot->m_iCombatTeam].canPick() )
						{
							pBot->m_pCombatBits = (CIntGAValues*)gBotGlobals.m_pCombatGA[pBot->m_iCombatTeam].pick();
							pBot->m_iCombatInfo = pBot->m_pCombatBits->get();
						}
						else
						{
							pBot->m_iCombatInfo = RANDOM_LONG(0,(BOT_COMBAT_WANT_SENUP3*2)-1);
							pBot->m_pCombatBits = new CIntGAValues(pBot->m_iCombatInfo);
						}
					}
				}*/
			}
		}
	}

	state++;
}

// tells who is the commander
void BotClient_NS_Commndr::execute(void* p, int iIndex)
{
	if (!p)
		return;

	const int index = *static_cast<int*>(p);

	if (index > 0 && index <= gpGlobals->maxClients)
	{
		gBotGlobals.SetCommander(INDEXENT(POINTER_TO_INT(p)));
	}
	else
		gBotGlobals.SetCommander(nullptr);
}

void BotClient_NS_SetTech::execute(void* p, int iIndex)
{
	short int* state = &gBotGlobals.m_iCurrentMessageState;

	static AvHMessageID iImpulsemessage = static_cast<AvHMessageID>(0);
	static int iSlot = 0;
	static int iCost = 0;
	static int iRadius = 0;
	/*

	  WRITE_BYTE(43) // message "impulse" (& iuser2) 0
	  WRITE_BYTE(3) // slot? /unique 1
	  WRITE_BYTE(0) // ?? 2
	  WRITE_BYTE(0) // ?? 3
	  WRITE_SHORT(10) // cost 4
	  WRITE_SHORT(2) // build radius? 5
	  WRITE_BYTE(7) // ?? 6
		*/

	CBot* pBot = &gBotGlobals.m_Bots[iIndex];

	if (p == nullptr || iIndex == -1)
		return;

	switch (POINTER_VALUE(state))
	{
	case 0:
		iImpulsemessage = static_cast<AvHMessageID>(POINTER_TO_INT(p));
		break;
	case 1:
		iSlot = POINTER_TO_INT(p);
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		iCost = POINTER_TO_INT(p);
		break;
	case 5:
		iRadius = POINTER_TO_INT(p);
		break;
	case 6:
		const CBotNSTech tech = CBotNSTech(iImpulsemessage, iCost, true, iRadius, iSlot);

		if (pBot)
		{
			const int team = pBot->GetTeam();

			if (team >= 0 && team < MAX_TEAMS)
				gBotGlobals.m_TeamTechs[team].addTech(tech);
		}

		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

#define IS_NS_3_FINAL 1

// When a marine bot receives an order...
void BotClient_NS_SetOrder::execute(void* p, int iIndex)
{
	static Vector				vOrigin;
	static dataStack<int>		iReceiverIndexes;
	static AvHOrderType			iOrderType = ORDERTYPE_UNDEFINED;
	static AvHOrderTargetType	iOrderTargetType = ORDERTARGETTYPE_UNDEFINED;
	static int					iOrderStatus = 0;
	static int                  iReceivers = 0;
	static int					iEntityUser3 = 0;
	static edict_t* pEntity = nullptr;

	short int* state = &gBotGlobals.m_iCurrentMessageState;
	short int* state2 = &gBotGlobals.m_iCurrentMessageState2;

	if (p == nullptr || iIndex == -1)
	{
		if (iIndex == -1)
			return;

		//	   eBotTask GoalTask = BOT_TASK_NONE;
		//	   CBotTask GoalTaskToAdd;
		eBotTask TryOrderTask = BOT_TASK_NONE;
		eBotTask OrderTask = BOT_TASK_NONE;
		eScheduleDesc OrderSched = BOT_SCHED_NONE;
		CBotTask WptObjectiveTask;

		if (pEntity == nullptr && iEntityUser3)
		{
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, vOrigin, 64.0f)) != nullptr)
			{
				if (pEntity->v.iuser3 == iEntityUser3)
					break;
			}
		}
		//eBotTask iTask, edict_t *pInfo = NULL, int iInfo = 0,
		//float fInfo = 0, Vector vInfo = Vector(0,0,0) )
		switch (iOrderType)
		{
		case ORDERTYPE_UNDEFINED:
			OrderTask = BOT_TASK_NONE;
			break;
		case ORDERTYPET_ATTACK:
			// depends on health/skill etc whether we want a range/flank/normal attack...
			OrderTask = BOT_TASK_NORMAL_ATTACK;
			break;
		case ORDERTYPET_BUILD:
			OrderTask = BOT_TASK_BUILD;
			OrderSched = BOT_SCHED_BUILD;
			break;
		case ORDERTYPET_GUARD:
			if (pEntity && pEntity->v.fuser1 != -1000.0f && pEntity->v.fuser2 != 1000.0f)
			{
				TryOrderTask = BOT_TASK_WELD_OBJECT;
			}
			OrderTask = BOT_TASK_DEFEND;
			break;
		case ORDERTYPET_WELD:
			OrderTask = BOT_TASK_WELD_OBJECT;
			break;
		case ORDERTYPET_GET:
			OrderTask = BOT_TASK_PICKUP_ITEM;
			break;
		case ORDERTYPEL_DEFAULT:
		case ORDERTYPEL_MOVE:
		case ORDERTYPEG_HOLD_POSITION:
			OrderTask = BOT_TASK_NONE;

			WptObjectiveTask = CBotTask(BOT_TASK_MOVE_TO_VECTOR, 1, nullptr, 0, 0, vOrigin);
			break;
		case ORDERTYPEG_CODE_DEPLOY_MINES:
			OrderTask = BOT_TASK_DEPLOY_MINES;
			break;
		case ORDERTYPEG_CODE_GREEN:
		case ORDERTYPEG_CODE_YELLOW:
		case ORDERTYPEG_CODE_RED:
		case ORDERTYPE_MAX:
			break;
		default:
			OrderTask = BOT_TASK_NONE;
		}

		dataStack<int> tempStack = iReceiverIndexes;

		CBotTask OrderTaskToAdd = CBotTask(OrderTask, 1, pEntity, iEntityUser3, 0, vOrigin);

		const BOOL bHasOrder = !iOrderStatus;

		while (!tempStack.IsEmpty())
		{
			const int iReceiverIndex = tempStack.ChooseFromStack();

			CBot* pBot = UTIL_GetBotPointer(INDEXENT(iReceiverIndex));

			if (pBot)
			{
				// Flush current tasks
				pBot->ClearTasks();

				pBot->m_bHasOrder = bHasOrder;

				if (bHasOrder)
				{
					switch (TryOrderTask)
					{
						// If the bot has been given an order to weld
						// see if it has the weapon

					case BOT_TASK_WELD_OBJECT:
						// if has the weapon then do the welding
						if (pBot->HasWeapon(NS_WEAPON_WELDER))
							OrderTaskToAdd = CBotTask(TryOrderTask, 1, pEntity, 0, 0, vOrigin);
						break;
					default:
						break;
					}

					pBot->m_OrderTask = OrderTaskToAdd;

					if (OrderTaskToAdd.Task() != BOT_TASK_NONE)
						pBot->AddPriorityTask(OrderTaskToAdd);

					pBot->m_iOrderType = iOrderType;
					pBot->m_FailedGoals.Destroy();

					pBot->AddPriorityTask(CBotTask(BOT_TASK_FIND_PATH, 1, pEntity, 0, 0, vOrigin));

					pBot->m_Tasks.GiveSchedIdDescription(1, OrderSched);
				}
				else
				{
					// finished
					pBot->m_Tasks.RemoveSameTask(pBot->m_OrderTask);
					pBot->m_iOrderType = ORDERTYPE_UNDEFINED;
				}
			}
		}

		iReceiverIndexes.Destroy();
		iOrderType = ORDERTYPE_UNDEFINED;
		iOrderTargetType = ORDERTARGETTYPE_UNDEFINED;
		iOrderStatus = 0;

		pEntity = nullptr;
		iEntityUser3 = 0;
		iReceivers = 0;

		return;
	}

	if (POINTER_VALUE(state) == 0)
	{
		if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
			iReceiverIndexes.Push(POINTER_TO_INT(p));
		else
			iReceivers = POINTER_TO_INT(p);

		POINTER_INCREMENT_VALUE(state);
	}
	else if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL) && POINTER_VALUE(state) <= iReceivers)
	{
		iReceiverIndexes.Push(POINTER_TO_INT(p));

		POINTER_INCREMENT_VALUE(state);
	}
	else
	{
		if (POINTER_VALUE(state2) == 0)
		{
			iOrderType = static_cast<AvHOrderType>(POINTER_TO_INT(p));
		}
		/*else if ( POINTER_VALUE(state2) == 1 )
		{
			iOrderTargetType = (AvHOrderTargetType)POINTER_TO_INT(p);
		}*/
		else if (POINTER_VALUE(state2) >= 1)
		{
			//AvHOrderTargetType t = iOrderTargetType;

			if (iOrderType != ORDERTYPET_GUARD)//(iOrderTargetType == ORDERTARGETTYPE_LOCATION)
			{
				switch (POINTER_VALUE(state2))
				{
				case 1:
					vOrigin.x = POINTER_TO_FLOAT(p);
					break;
				case 2:
					vOrigin.y = POINTER_TO_FLOAT(p);
					break;
				case 3:
					vOrigin.z = POINTER_TO_FLOAT(p);
					break;
				case 4:
					iEntityUser3 = POINTER_TO_INT(p);
					break;
				case 5:
					iOrderStatus = POINTER_TO_INT(p);
					break;
				}
			}
			else// if ( iOrderTargetType == ORDERTARGETTYPE_TARGET )// iOrderType == ORDER_DEFEND )
			{
				switch (POINTER_VALUE(state2))
				{
				case 1:
				{
					const int index = POINTER_TO_INT(p);
					pEntity = INDEXENT(index);
				}
				break;
				case 2:
					iEntityUser3 = POINTER_TO_INT(p);
					break;
				case 3:
					iOrderStatus = POINTER_TO_INT(p);
					break;
				}
			}
		}

		POINTER_INCREMENT_VALUE(state2);
	}
}

void BotClient_Generic_DeathMessage::execute(void* p, int iIndex)
{
	static int killer_index;
	static int victim_index;

	if (p == nullptr)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		killer_index = POINTER_TO_INT(p);
		break;
	case 1:
		victim_index = POINTER_TO_INT(p);
		break;
	case 2:
	{
		// stuff in here to do when a bot is killed by/kills another player/bot
		if (killer_index == victim_index || killer_index == -1)
		{
			const edict_t* victim_edict = INDEXENT(victim_index);
			CBot* pBotVictim = UTIL_GetBotPointer(victim_edict);

			if (pBotVictim)
				pBotVictim->BotEvent(BOT_EVENT_KILL_SELF);

			/*for ( i = 0; i < MAX_PLAYERS; i ++ )
			{
				pBot = &gBotGlobals.m_Bots[i];

				if ( (pBot->DistanceFromEdict(victim_edict) < BOT_HEAR_DISTANCE) && pBot->FVisible(EntityOrigin(victim_edict)) )
				{
					pBot->BotEvent(BOT_EVENT_SEE_TEAMMATE_DIE,victim_edict);
				}
			}*/
			// killed self
		}
		else if (victim_index != -1 && killer_index != -1)
		{
			CBot* pBot;
			int i;

			edict_t* killer_edict = INDEXENT(killer_index);
			edict_t* victim_edict = INDEXENT(victim_index);

			CBot* pBotKiller = UTIL_GetBotPointer(killer_edict);
			CBot* pBotVictim = UTIL_GetBotPointer(victim_edict);

			if (pBotKiller)
			{
				pBotKiller->BotEvent(BOT_EVENT_KILL, victim_edict);
			}

			if (pBotVictim)
			{
				pBotVictim->BotEvent(BOT_EVENT_DIED, killer_edict);
			}

			// fire some bot events

			BOOL iNotHandled[MAX_PLAYERS];

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				pBot = &gBotGlobals.m_Bots[i];

				iNotHandled[i] = false;

				if (pBot == pBotKiller)
					continue;
				if (pBot == pBotVictim)
					continue;
				if (!pBot->IsUsed() || pBot->m_iRespawnState != RESPAWN_IDLE)
					continue;

				iNotHandled[i] = true;

				pBot->PlayerDead(victim_edict);

				if (pBot->IsInVisibleList(victim_edict))
				{
					if (victim_edict && victim_edict->v.team)
					{
						if (!pBot->IsEnemy(victim_edict))
						{
							pBot->BotEvent(BOT_EVENT_SEE_TEAMMATE_DIE, victim_edict, killer_edict);
							iNotHandled[i] = false;
							continue;
						}
						else
						{
							pBot->BotEvent(BOT_EVENT_SEE_ENEMY_DIE, victim_edict, killer_edict);
							iNotHandled[i] = false;
							continue;
						}
					}
					else
					{
						pBot->BotEvent(BOT_EVENT_SEE_PLAYER_DIE, victim_edict, killer_edict);
						iNotHandled[i] = false;
						continue;
					}
				}

				if (pBot->IsInVisibleList(killer_edict))
				{
					if (killer_edict && killer_edict->v.team)
					{
						if (!pBot->IsEnemy(killer_edict))
						{
							pBot->BotEvent(BOT_EVENT_SEE_TEAMMATE_KILL, killer_edict, victim_edict);
							iNotHandled[i] = false;
							continue;
						}
						else if (!pBot->IsEnemy(victim_edict))
						{
							pBot->BotEvent(BOT_EVENT_SEE_ENEMY_KILL, killer_edict, victim_edict);
							iNotHandled[i] = false;
							continue;
						}
					}
					else
					{
						pBot->BotEvent(BOT_EVENT_SEE_PLAYER_DIE, killer_edict, victim_edict);
						iNotHandled[i] = false;
						continue;
					}
				}
			}

			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if (iNotHandled[i])
				{
					pBot = &gBotGlobals.m_Bots[i];

					if (!pBot->IsEnemy(victim_edict)) // team-mate?
					{
						if (pBot->DistanceFromEdict(victim_edict) < BOT_HEAR_DISTANCE && pBot->FVisible(EntityOrigin(victim_edict)))
						{
							pBot->BotEvent(BOT_EVENT_HEAR_TEAMMATE_DIE, victim_edict);
						}
					}
				}
			}
		}
	}
	break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_WeaponList::execute(void* p, const int iIndex)
{
	static char* szClassname;
	static int iAmmo1Max;
	static int iAmmo1;
	static int iAmmo2Max;
	static int iAmmo2;
	static int iSlot;
	static int iPosition;
	static int iId;
	static int iFlags;

	if (p == nullptr || iIndex == -1)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		szClassname = gBotGlobals.m_Strings.GetString(static_cast<char*>(p));
		break;
	case 1:
		iAmmo1 = POINTER_TO_INT(p);  // ammo index 1
		break;
	case 2:
		iAmmo1Max = POINTER_TO_INT(p);  // max ammo1
		break;
	case 3:
		iAmmo2 = POINTER_TO_INT(p);  // ammo index 2
		break;
	case 4:
		iAmmo2Max = POINTER_TO_INT(p);  // max ammo2
		break;
	case 5:
		iSlot = POINTER_TO_INT(p);  // slot for this weapon
		break;
	case 6:
		iPosition = POINTER_TO_INT(p);  // position in slot
		break;
	case 7:
		iId = POINTER_TO_INT(p);  // weapon ID
		break;
	case 8:
	{
		const int iHackedId = iId;

		//BotMessage(NULL,0,"Weapon ID: %d Classname: \"%s\"",iId,szClassname);

		iFlags = POINTER_TO_INT(p);  // flags for weapon

		if (gBotGlobals.IsMod(MOD_DMC))
		{
			iId = BotFunc_GetBitSetOf(iId);
		}

		gBotGlobals.m_Weapons.AddWeapon(iId, szClassname, iAmmo1Max, iAmmo2Max, iSlot, iPosition, iFlags, iAmmo1, iAmmo2);

		if (gBotGlobals.IsMod(MOD_DMC))
		{
			CBot* pBot = &gBotGlobals.m_Bots[iIndex];

			pBot->m_iBotWeapons |= iHackedId;
			pBot->m_Weapons.AddWeapon(iId);
		}
	}
	// got all the info, now...
   // store away this weapon with it's ammo information...
   //weapon_defs[bot_weapon.iId] = bot_weapon;
	break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_CurrentWeapon::execute(void* p, const int iIndex)
{
	static int iState;
	static int iId;
	static int iClip;

	if (p == nullptr || iIndex == -1)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		iState = POINTER_TO_INT(p);  // state of the current weapon
		break;
	case 1:
		iId = POINTER_TO_INT(p);  // weapon ID of current weapon
		break;
	case 2:
		iClip = POINTER_TO_INT(p);  // ammo currently in the clip for this weapon

		CBot* pBot;
		CBotWeapon* pWeapon;

		pBot = &gBotGlobals.m_Bots[iIndex];

		if (gBotGlobals.IsMod(MOD_DMC))
		{
			iId = BotFunc_GetBitSetOf(iId);

			if (iId <= 8)
			{
				if (iState == 1)
				{
					pBot->m_iBotWeapons |= 1 << (iId - 1);

					pBot->m_Weapons.AddWeapon(iId);

					pWeapon = pBot->m_Weapons.GetWeapon(iId);

					if (pWeapon)
						pWeapon->UpdateWeapon(iClip);

					pBot->m_pCurrentWeapon = pWeapon;
				}
			}
		}
		else if (iId < MAX_WEAPONS)
		{
			int st = iState;
			/*WEAPON_ON_TARGET    = 0x01,
			WEAPON_IS_CURRENT    = 0x02,
			WEAPON_IS_ENABLED    = 0x04*/
			if (gBotGlobals.IsNS() && iState & WEAPON_IS_CURRENT || iState == 1)
			{
				pBot->m_Weapons.AddWeapon(iId);
				pWeapon = pBot->m_Weapons.GetWeapon(iId);

				//if ( pWeapon->HasWeapon(NULL) == false )
				//{
					 // try to add
				//}

				if (pWeapon)
					pWeapon->UpdateWeapon(iClip);

				pBot->m_pCurrentWeapon = pWeapon;
				pBot->m_iBotWeapons |= iId;
			}
			if (gBotGlobals.IsNS() && !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
			{
				pBot->m_Weapons.setHasWeapon(iId, (iState & WEAPON_IS_ENABLED) == WEAPON_IS_ENABLED);
			}
		}
		break;
	case 3:
		pBot = &gBotGlobals.m_Bots[iIndex];

		if (gBotGlobals.IsNS() && !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
		{
			const int enabled = POINTER_TO_INT(p);
			pBot->m_Weapons.setHasWeapon(iId, enabled == 1);
		}
	default:
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_AmmoX::execute(void* p, const int iIndex)
{
	static int index;
	static int amount;

	if (p == nullptr || iIndex == -1)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		index = POINTER_TO_INT(p);  // ammo index (for type of ammo)
		break;
	case 1:
		amount = POINTER_TO_INT(p);  // the amount of ammo currently available
		{
			CBot* pBot = &gBotGlobals.m_Bots[iIndex];

			pBot->m_Weapons.UpdateAmmo(index, amount);
		}
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_AmmoPickup::execute(void* p, const int iIndex)
{
	static int index;
	static int amount;

	if (p == nullptr || iIndex == -1)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		index = POINTER_TO_INT(p);
		break;
	case 1:
		amount = POINTER_TO_INT(p);
		{
			CBot* pBot = &gBotGlobals.m_Bots[iIndex];

			pBot->m_Weapons.UpdateAmmo(index, amount);
		}
		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_WeaponPickup::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	const int iWeaponIndex = POINTER_TO_INT(p);

	CBot* pBot = &gBotGlobals.m_Bots[iIndex];

	if (gBotGlobals.IsNS())
	{
		switch (iWeaponIndex)
		{
		case NS_WEAPON_KNIFE:
		case NS_WEAPON_WELDER:
		case NS_WEAPON_MINE:
			pBot->m_bCanUseAmmoDispenser = false;
			break;
		default:
			pBot->m_bCanUseAmmoDispenser = true;
		}
	}
	/*else if (gBotGlobals.IsMod(MOD_SVENCOOP))
	{
		switch (iWeaponIndex)
		{
		case VALVE_WEAPON_RPG:
			// only when the bot picks up rpg for the first time
			if (!pBot->HasWeapon(VALVE_WEAPON_RPG))
			{
				// make bot use laser
				pBot->UseRPGLaser();
			}
			break;
		}
	}*/

	pBot->m_Weapons.AddWeapon(iWeaponIndex);

	pBot->m_iBotWeapons |= 1 << iWeaponIndex;
}

void BotClient_Generic_ItemPickup::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	CBot* pBot = &gBotGlobals.m_Bots[iIndex];

	if (p == pBot->m_pPickupEntity)
		pBot->m_pPickupEntity = nullptr;
}

void BotClient_Generic_Health::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	CBot* pBot = &gBotGlobals.m_Bots[iIndex];
	const int iHealth = POINTER_TO_INT(p);

	if (iHealth > pBot->m_fPrevHealth) // more health
	{
		if (pBot->m_bAcceptHealth)
		{
			if (!pBot->m_Tasks.HasTask(BOT_TASK_ACCEPT_HEALTH))
			{
				edict_t* pSupplier = nullptr;
				const float nearest = 96.0f;

				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					edict_t* pPlayer = INDEXENT(i);

					if (!pPlayer || FNullEnt(pPlayer))
						continue;

					if (!pPlayer->v.netname)
						continue;

					if (pPlayer == pBot->m_pEdict)
						continue;

					const float dist = pBot->DistanceFromEdict(pPlayer);

					if (dist < nearest)
					{
						// crude way of checking player is holding fire
						/*if (pPlayer->v.button & IN_ATTACK && pPlayer->v.weapons & SVEN_WEAPON_MEDKIT)
						{
							nearest = dist;
							pSupplier = pPlayer;
						}*/
					}
				}

				if (pSupplier)
				{
					pBot->AddPriorityTask(CBotTask(BOT_TASK_ACCEPT_HEALTH, 0, pSupplier, 0, RANDOM_FLOAT(1.5, 2.5)));
					pBot->m_bAcceptHealth = false;
				}
			}
		}
	}
}

void BotClient_Generic_Battery::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;
}

void BotClient_TS_PTakeDam::execute(void* p, int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	ALERT(at_console, "PTakeDam message\n");
}

void BotClient_TS_TSMessage::execute(void* p, int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;

	// ALERT(at_console,"TS_TSMessage message (iIndex = %d) message = \"%s\"\n",iIndex,(char*)p);
}

void BotClient_Generic_Damage::execute(void* p, const int iIndex)
{
	static int damage_armor;
	static int damage_taken;
	static int damage_bits;  // type of damage being done
	static Vector damage_origin;

	if (p == nullptr || iIndex == -1)
		return;

	short int* state = &gBotGlobals.m_iCurrentMessageState;

	switch (POINTER_VALUE(state))
	{
	case 0:
		damage_armor = POINTER_TO_INT(p);
		break;
	case 1:
		damage_taken = POINTER_TO_INT(p);
		break;
	case 2:
		damage_bits = POINTER_TO_INT(p);
		break;
	case 3:
		damage_origin.x = POINTER_TO_FLOAT(p);
		break;
	case 4:
		damage_origin.y = POINTER_TO_FLOAT(p);
		break;
	case 5:
		damage_origin.z = POINTER_TO_FLOAT(p);

		if (damage_armor > 0 || damage_taken > 0)
		{
			// ignore certain types of damage...
			if (damage_bits & IGNORE_DAMAGE)
				return;
			/*if (!gBotGlobals.IsMod(MOD_SVENCOOP))
			{
				// can get shocked by some monsters, dont ignore in svencoop
				// only in other mods
				if (damage_bits & DMG_SHOCK)
					return;
			}*/

			CBot* pBot = &gBotGlobals.m_Bots[iIndex];

			pBot->BotEvent(BOT_EVENT_HURT, nullptr, nullptr, damage_origin);

			//if ( damage_taken > (int)(pBot->pev->max_health/4) )
		   //	 pBot->BotEvent(BOT_EVENT_HEAVY_DAMAGE,NULL,NULL,(float*)damage_origin);
			//else
		   //	 pBot->BotEvent(BOT_EVENT_LIGHT_DAMAGE,NULL,NULL,(float*)damage_origin);

			// if the bot doesn't have an enemy and someone is shooting at it then
			// turn in the attacker's direction...
		}

		break;
	}

	POINTER_INCREMENT_VALUE(state);
}

void BotClient_Generic_ScreenFade::execute(void* p, const int iIndex)
{
	if (p == nullptr || iIndex == -1)
		return;
}