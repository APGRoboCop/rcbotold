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
 /***
 *
 *  Copyright (c) 1999, Valve LLC. All rights reserved.
 *
 *  This product contains software technology licensed from Id
 *  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
 *  All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC.  All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 ****/

 //////////////////////////////////////////////////
 // RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
 //
 // (http://www.rcbot.net)
 //
 // Based on botman's High Ping Bastard bot
 //
 // (http://planethalflife.com/botman/)
 //
 // util.cpp
 //
 //////////////////////////////////////////////////
 //
 // Bot & engine utility functions
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "engine.h"
#else
#include "h_export_meta.h"
#include "dllapi.h"
#include "meta_api.h"
#endif
#include "cbase.h"
#include "bot_const.h"
#include "bot.h"

#include "waypoint.h"

extern CBotGlobals gBotGlobals;
extern enginefuncs_t g_engfuncs;
extern CWaypointLocations WaypointLocations;

#define PI 3.141592654

BOOL UTIL_TankUsed(edict_t* pTank)
{
	return pTank->v.netname && *STRING(pTank->v.netname);
}

BOOL UTIL_EntityHasClassname(edict_t* pEntity, char* classname)
{
	return strcmp(STRING(pEntity->v.classname), classname) == 0;
}

BOOL UTIL_IsGrenadeRocket(edict_t* pEntity)
{
	const char* szClassname = static_cast<const char*>(STRING(pEntity->v.classname));

	return strstr(szClassname, "grenade") != NULL || strstr(szClassname, "rpg_rocket") != NULL;
}
// from old RCBot
void UTIL_BotScreenShake(const Vector& center, float amplitude, float frequency, float duration, float radius)
{
	int			i;
	float		localAmplitude;
	ScreenShake	shake;

	shake.duration = FixedUnsigned16(duration, 1 << 12);		// 4.12 fixed
	shake.frequency = FixedUnsigned16(frequency, 1 << 8);	// 8.8 fixed

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (!pPlayer)	// still shake if not onground
			continue;

		localAmplitude = 0;

		if (radius <= 0)
			localAmplitude = amplitude;
		else
		{
			Vector delta = center - pPlayer->v.origin;
			float distance = delta.Length();

			// Had to get rid of this falloff - it didn't work well
			if (distance < radius)
				localAmplitude = amplitude;//radius - distance;
		}

		if (localAmplitude)
		{
			int iMsg = GetMessageID("ScreenShake");

			if (iMsg)
			{
				shake.amplitude = FixedUnsigned16(localAmplitude, 1 << 12);		// 4.12 fixed

				MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, iMsg, NULL, pPlayer);		// use the magic #1 for "one client"

				WRITE_SHORT(shake.amplitude);				// shake amount
				WRITE_SHORT(shake.duration);				// shake lasts this long
				WRITE_SHORT(shake.frequency);				// shake noise frequency

				MESSAGE_END();
			}
		}
	}
}
/*
BOOL UTIL_FriendlyHatesPlayer ( edict_t *pEntity, edict_t *pPlayer )
{
	if ( !gBotGlobals.IsMod(MOD_SVENCOOP) )
		return FALSE;

	CBaseMonster *pEnt = (CBaseMonster*)((CBaseEntity*)GET_PRIVATE(pEntity));
	CBaseEntity *pPlayerEnt = (CBaseEntity*)GET_PRIVATE(pPlayer);

	if ( pEnt )
	{
		if ( pPlayerEnt )
		{
			// test to see what rel is
			int rel = pEnt->IRelationship(pPlayerEnt);

			// rel was some large number, 20000 odd, should only be like -2 to 3 or something?
			return (rel == R_HT);

			// afMemory was always 0.. :/
//		return (( pEnt->m_afMemory & bits_MEMORY_PROVOKED ) == bits_MEMORY_PROVOKED); //This monster hates the player
		}
	}

	return FALSE;
}
*/

Vector UTIL_GetGroundVector(edict_t* pEdict)
{
	Vector vOrigin = EntityOrigin(pEdict);
	TraceResult tr;

	UTIL_TraceLine(vOrigin, vOrigin - Vector(0, 0, 4096.0), ignore_monsters, dont_ignore_glass, pEdict, &tr);

	return tr.vecEndPos;
}

BOOL UTIL_EntityIsHive(edict_t* pEdict)
{
	return pEdict->v.iuser3 == AVH_USER3_HIVE;
}
/*
typedef struct
{
	char				label[32];	// sequence label

	float				fps;		// frames per second
	int					flags;		// looping/non-looping flags

	int					activity;
	int					actweight;

	int					numevents;
	int					eventindex;

	int					numframes;	// number of frames per sequence

	int					numpivots;	// number of foot pivots
	int					pivotindex;

	int					motiontype;
	int					motionbone;
	vec3_t				linearmovement;
	int					automoveposindex;
	int					automoveangleindex;

	vec3_t				bbmin;		// per sequence bounding box
	vec3_t				bbmax;

	int					numblends;
	int					animindex;		// mstudioanim_t pointer relative to start of sequence group data
										// [blend][bone][X, Y, Z, XR, YR, ZR]

	int					blendtype[2];	// X, Y, Z, XR, YR, ZR
	float				blendstart[2];	// starting value
	float				blendend[2];	// ending value
	int					blendparent;

	int					seqgroup;		// sequence group for demand loading

	int					entrynode;		// transition node at entry
	int					exitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	int					nextseq;		// auto advancing sequences
} mstudioseqdesc_t;

typedef struct
{
	int					id;
	int					version;

	char				name[64];
	int					length;

	vec3_t				eyeposition;	// ideal eye position
	vec3_t				min;			// ideal movement hull size
	vec3_t				max;

	vec3_t				bbmin;			// clipping bounding box
	vec3_t				bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;

	int					numhitboxes;			// complex bounding boxes
	int					hitboxindex;

	int					numseq;				// animation sequences
	int					seqindex;

	int					numseqgroups;		// demand loaded sequences
	int					seqgroupindex;

	int					numtextures;		// raw textures
	int					textureindex;
	int					texturedataindex;

	int					numskinref;			// replaceable textures
	int					numskinfamilies;
	int					skinindex;

	int					numbodyparts;
	int					bodypartindex;

	int					numattachments;		// queryable attachable points
	int					attachmentindex;

	int					soundtable;
	int					soundindex;
	int					soundgroups;
	int					soundgroupindex;

	int					numtransitions;		// animation node to animation node transition graph
	int					transitionindex;
} studiohdr_t;
*/
//mahnsawce : To get the player's energy
float UTIL_GetPlayerEnergy(entvars_t* pev)
{
	return pev->fuser3 / 10.0;   // This returns the percentage (0%-100%)
}

int NS_GetPlayerLevel(int exp)
{
	exp /= 100;

	if (exp >= 2700)
		return 10;
	if (exp >= 2200)
		return 9;
	if (exp >= 1750)
		return 8;
	if (exp >= 1350)
		return 7;
	if (exp >= 1000)
		return 6;
	if (exp >= 700)
		return 5;
	if (exp >= 450)
		return 4;
	if (exp >= 250)
		return 3;
	if (exp >= 100)
		return 2;

	return 1;
}

int RoundToNearestInteger(float fVal)
{
	int loVal = (int)fVal;

	fVal -= (float)loVal;

	if (fVal >= 0.5)
		return loVal + 1;

	return loVal;
}

int Ceiling(float fVal)
{
	int loVal = (int)fVal;

	fVal -= (float)loVal;

	if (fVal == 0.0)
		return loVal;

	return loVal + 1;
}

Vector UTIL_VecToAngles(const Vector& vec)
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t* pentIgnore, TraceResult* ptr)
{
	TRACE_LINE(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass ? 0x100 : 0), pentIgnore, ptr);
}

void UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, edict_t* pentIgnore, TraceResult* ptr)
{
	TRACE_LINE(vecStart, vecEnd, igmon == ignore_monsters ? TRUE : FALSE, pentIgnore, ptr);
}

void UTIL_MakeVectors(const Vector& vecAngles)
{
	MAKE_VECTORS(vecAngles);
}

void strlow(char* str)
// lower a string to make it lower case.
{
	int len = strlen(str);

	int i;

	for (i = 0; i < len; i++)
	{
		str[i] = tolower(str[i]);
	}
}

void strhigh(char* str)
// higher a string to make it upper case.
{
	int len = strlen(str);

	int i;

	for (i = 0; i < len; i++)
	{
		str[i] = toupper(str[i]);
	}
}

edict_t* UTIL_FindPlayerByTruncName(const char* name)
// find a player by a truncated name "name".
// e.g. name = "Jo" might find a player called "John"
{
	edict_t* pent = NULL;

	int i;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pent = INDEXENT(i);

		if (pent != NULL)
		{
			if (!pent->free)
			{
				// 'strlen' function called too many times inside loop [APG]RoboCop[CL]
				const int length = strlen(name);

				char arg_lwr[80];
				char pent_lwr[80];

				strcpy(arg_lwr, name);
				strcpy(pent_lwr, STRING(pent->v.netname));

				strlow(arg_lwr);
				strlow(pent_lwr);

				if (strncmp(arg_lwr, pent_lwr, length) == 0)
				{
					return pent;
				}
			}
		}
	}

	return NULL;
}

edict_t* UTIL_FindEntityInSphere(edict_t* pentStart, const Vector& vecCenter, float flRadius)
{
	edict_t* pentEntity;

	pentEntity = FIND_ENTITY_IN_SPHERE(pentStart, vecCenter, flRadius);

	if (!FNullEnt(pentEntity))
		return pentEntity;

	return NULL;
}
/*
int LookupActivity( void *pmodel, entvars_t *pev, int activity )
{
	if( !pmodel )
		return 0;

	studiohdr_t *pstudiohdr;

	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t *pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	if( !pseqdesc )
		return 0;

	int weighttotal = 0;
	int seq = -1;
	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (pseqdesc[i].activity == activity)
		{
			weighttotal += pseqdesc[i].actweight;
			if (!weighttotal || RANDOM_LONG(0,weighttotal-1) < pseqdesc[i].actweight)
				seq = i;
		}
	}

	return seq;
}
*/

edict_t* UTIL_FindEntityByString(edict_t* pentStart, const char* szKeyword, const char* szValue)
{
	edict_t* pentEntity;

	pentEntity = FIND_ENTITY_BY_STRING(pentStart, szKeyword, szValue);

	if (!FNullEnt(pentEntity))
	{
		if (pentEntity == pentStart)
			return NULL; // loop check

		return pentEntity;
	}

	return NULL;
}

edict_t* UTIL_FindEntityByTarget(edict_t* pentStart, const char* szName)
{
	return UTIL_FindEntityByString(pentStart, "target", szName);
}

edict_t* UTIL_FindEntityByTargetname(edict_t* pentStart, const char* szName)
{
	return UTIL_FindEntityByString(pentStart, "targetname", szName);
}

edict_t* UTIL_FindEntityByClassname(edict_t* pentStart, const char* szName)
{
	return UTIL_FindEntityByString(pentStart, "classname", szName);
}

int UTIL_PointContents(const Vector& vec)
{
	return POINT_CONTENTS(vec);
}

void UTIL_SetSize(entvars_t* pev, const Vector& vecMin, const Vector& vecMax)
{
	SET_SIZE(ENT(pev), vecMin, vecMax);
}

void UTIL_SetOrigin(entvars_t* pev, const Vector& vecOrigin)
{
	SET_ORIGIN(ENT(pev), vecOrigin);
}

void ClientPrint(edict_t* pEntity, int msg_dest, const char* msg_name)
{
	const char* szMsg = { "TextMsg" };

	int msg_id;

	// net message already started, can't start another one
	if (gBotGlobals.m_bNetMessageStarted)
	{
		//ALERT(at_console,msg_name);
		return;
	}

	msg_id = GetMessageID(szMsg);

	if (msg_id > 0)
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, msg_id, NULL, pEntity);

		g_engfuncs.pfnWriteByte(msg_dest);
		g_engfuncs.pfnWriteString(msg_name);
		g_engfuncs.pfnMessageEnd();
	}
}

void UTIL_SayText(const char* pText, edict_t* pEdict)
{
	const char* szMsg = { "SayText" };

	int msg_id = GetMessageID(szMsg);

	if (msg_id > 0)
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, msg_id, NULL, pEdict);
		g_engfuncs.pfnWriteByte(ENTINDEX(pEdict));
		g_engfuncs.pfnWriteString(pText);
		g_engfuncs.pfnMessageEnd();
	}
}

int GetMessageID(const char* szMsg)
{
	int msg_id = -1;

#ifndef RCBOT_META_BUILD
	CBotNetMessage* msg = NULL;
	msg = gBotGlobals.m_NetEntityMessages.GetMessage(0, szMsg);
	if (msg)
		msg_id = msg->MessageNum();
#else
	extern plugin_info_t Plugin_info;

	msg_id = GET_USER_MSG_ID(&Plugin_info, szMsg, 0);
#endif

	return msg_id;
}

float UTIL_AnglesBetweenEdictOrigin(edict_t* pEdict, Vector const origin)
{
	Vector v_enemy = origin - pEdict->v.origin + pEdict->v.view_ofs;
	Vector angles = v_enemy;
	//angles = UTIL_VecToAngles(angles);
	Vector v_viewpoint = pEdict->v.v_angle;

	UTIL_MakeVectors(v_viewpoint);
	v_viewpoint = gpGlobals->v_forward * 8192.0f;

	//v_viewpoint = GetGunPosition(pEdict);

	//v_viewpoint = UTIL_VecToAngles(v_viewpoint);
	//v_viewpoint = UTIL_FixAngles(v_viewpoint);

	return UTIL_AngleBetweenVectors(v_viewpoint, angles);
}

edict_t* UTIL_getEntityInFront(edict_t* pEntity)
{
	//angles = UTIL_VecToAngles(angles);
	Vector v_viewpoint = pEntity->v.v_angle;

	UTIL_MakeVectors(v_viewpoint);
	v_viewpoint = gpGlobals->v_forward * 8192.0f;
	Vector v_src = pEntity->v.origin + pEntity->v.view_ofs;

	TraceResult tr;

	UTIL_TraceLine(v_src, v_src + v_viewpoint, dont_ignore_monsters, dont_ignore_glass, pEntity, &tr);

	return tr.pHit;
}

float UTIL_AngleBetweenVectors(Vector const vec1, Vector const vec2)
{
	//vec1 = UTIL_FixAngles(vec1);
	//vec2 = UTIL_FixAngles(vec2);

	double vec1Dotvec2 = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	double veclengths = vec1.Length() * vec2.Length();

	return acos(vec1Dotvec2 / veclengths) * (180 / PI);
}

float UTIL_YawAngleBetweenOrigin(entvars_t* pev, Vector const vOrigin)
{
	float fAngle;
	Vector vBotAngles = pev->v_angle;
	Vector vAngles;

	UTIL_MakeVectors(vBotAngles);

	vAngles = vOrigin - pev->origin;
	vAngles = UTIL_VecToAngles(vAngles);

	fAngle = vBotAngles.y - vAngles.y;

	UTIL_FixFloatAngle(&fAngle);

	return fAngle;
}

Vector UTIL_AngleBetweenOrigin(entvars_t* pev, Vector const vOrigin)
{
	Vector vAngle;
	Vector vBotAngles = pev->v_angle;
	Vector vAngles;

	UTIL_MakeVectors(vBotAngles);

	vAngles = vOrigin - (pev->origin + pev->view_ofs);
	vAngles = UTIL_VecToAngles(vAngles);

	vAngle = vBotAngles - vAngles;

	UTIL_FixAngles(&vAngle);

	return vAngle;
}

BOOL UTIL_IsFacingEntity(entvars_t* pev, entvars_t* pevEntity)
{
	float fDistance;

	UTIL_MakeVectors(pev->v_angle);

	Vector vSrc = GetGunPosition(ENT(pev));

	fDistance = (EntityOrigin(ENT(pevEntity)) - vSrc).Length();

	Vector vDst = vSrc + gpGlobals->v_forward * fDistance;

	return vDst <= pevEntity->absmax && vDst >= pevEntity->absmin;
}
/*

float UTIL_PitchAngleBetweenOrigin(entvars_t *pev,Vector vOrigin)
{
// UTIL_IsFacingEntity(entvars_t *pev, entvars_t *pevEntity)
	//{
	/*
	Vector vMaxAngles;
	Vector vMinAngles;
	Vector vPevOrigin;

	vPevOrigin = pev->origin + pev->view_ofs;
	vMaxAngles = UTIL_VecToAngles(pev*/
	/*
	float fAngle;
	Vector vBotAngles = pev->v_angle;
	Vector vAngles;

	UTIL_MakeVectors(vBotAngles);

	vAngles = vOrigin - pev->origin;
	vAngles = UTIL_VecToAngles(vAngles);

	fAngle = -vBotAngles.x - (vAngles.x*3);

	UTIL_FixFloatAngle(&fAngle);
*/
//	return 1;
//}

// from old bot code
float UTIL_GetAvoidAngle(edict_t* pEdict, Vector const origin)
{
	Vector v_enemy = origin - pEdict->v.origin;

	float angles;

	Vector v_viewpoint = pEdict->v.v_angle;
	UTIL_MakeVectors(v_viewpoint);
	v_viewpoint = gpGlobals->v_forward * 4096.0;

	v_viewpoint = UTIL_VecToAngles(v_viewpoint);

	UTIL_FixAngles(&v_viewpoint);

	v_enemy = UTIL_VecToAngles(v_enemy);

	UTIL_FixAngles(&v_enemy);

	angles = v_viewpoint.y - v_enemy.y;

	if (angles > 180.0)
		angles -= 360.0;

	if (angles < -180.0)
		angles += 360.0;

	return -angles;
}

edict_t* UTIL_GetPlayerByPlayerId(int id)
{
	int i;
	edict_t* pPlayer;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		pPlayer = INDEXENT(i);

		if (!pPlayer || FNullEnt(pPlayer))
			continue;

		if (id == GETPLAYERUSERID(pPlayer))
			return pPlayer;
	}

	return NULL;
}

void UTIL_HostSay(edict_t* pEntity, int teamonly, char* message)
{
	int   j;
	char  text[128];
	char* pc;
	int   sender_team, player_team;
	edict_t* client;

	// make sure the text has content
	for (pc = message; pc != NULL && *pc != 0; pc++)
	{
		if (isprint(*pc) && !isspace(*pc))
		{
			pc = NULL;   // we've found an alphanumeric character,  so text is valid
			break;
		}
	}

	if (pc != NULL)
		return;  // no character found, so say nothing

	 // turn on color set 2  (color on,  no sound)
	if (teamonly)
		sprintf(text, "%c(TEAM) %s: ", 2, STRING(pEntity->v.netname));
	else
		sprintf(text, "%c%s: ", 2, STRING(pEntity->v.netname));

	j = sizeof text - 2 - strlen(text);  // -2 for /n and null terminator
	if ((int)strlen(message) > j)
		message[j] = 0;

	strcat(text, message);
	strcat(text, "\n");

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

	const char* szMsg = { "SayText" };

	int msg_id = GetMessageID(szMsg);

	if (msg_id == -1)
		return;

	sender_team = UTIL_GetTeam(pEntity);

	client = NULL;
	while ((client = UTIL_FindEntityByClassname(client, "player")) != NULL &&
		!FNullEnt(client))
	{
		if (client == pEntity)  // skip sender of message
			continue;

		player_team = UTIL_GetTeam(client);

		if (teamonly && sender_team != player_team)
			continue;

		g_engfuncs.pfnMessageBegin(MSG_ONE, msg_id, NULL, client);
		g_engfuncs.pfnWriteByte(ENTINDEX(pEntity));
		g_engfuncs.pfnWriteString(text);
		g_engfuncs.pfnMessageEnd();
	}

	// print to the sending client
	g_engfuncs.pfnMessageBegin(MSG_ONE, msg_id, NULL, pEntity);
	g_engfuncs.pfnWriteByte(ENTINDEX(pEntity));
	g_engfuncs.pfnWriteString(text);
	g_engfuncs.pfnMessageEnd();

	// echo to server console
	g_engfuncs.pfnServerPrint(text);
}

int UTIL_GetNumClients(BOOL bReport)
{
	int i = 0;

	edict_t* pPlayer;
	int iNum;
	//int iUserId;

	iNum = 0;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (pPlayer == NULL)
			continue;
		if (pPlayer->free)
			continue;
		if (!pPlayer->v.netname)
			continue;
		if (!*STRING(pPlayer->v.netname))
			continue;
		//	if ( bReport )
		//		BotMessage(NULL,0,"Report, player: %d, user id: %d",i,iUserId);
		iNum++;
	}

	if (bReport)
		BotMessage(NULL, 0, "Num clients = %d", iNum);

	return iNum;
}

#ifdef   DEBUG
edict_t* DBG_EntOfVars(const entvars_t* pev)
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;
	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine");
	edict_t* pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t*)pev);
	if (pent == NULL)
		ALERT(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!");
	((entvars_t*)pev)->pContainingEntity = pent;
	return pent;
}
#endif //DEBUG

edict_t* UTIL_TFC_PlayerHasFlag(edict_t* pPlayer)
{
	edict_t* pent = NULL;

	while ((pent = UTIL_FindEntityByClassname(pent, "item_tfgoal")) != NULL)
	{
		if (pent->v.owner == pPlayer)
			return pent;
	}

	return NULL;
}

// return team number 0 through 3 based what MOD uses for team numbers
int UTIL_GetTeam(edict_t* pEntity)
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		return pEntity->v.team;
	case MOD_TS:
	{
		char* infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)(pEntity);

		char model[64];

		strcpy(model, g_engfuncs.pfnInfoKeyValue(infobuffer, "model"));

		if (strncmp("tm_", STRING(gpGlobals->mapname), 3) == 0)
		{
			if (FStrEq(model, "merc"))
				return 0;
			else if (FStrEq(model, "seal"))
				return 1;
		}
		else if (gBotGlobals.m_bTeamPlay)
		{
			const char* teamlist = CVAR_GET_STRING("mp_teamlist");

			const char* pos = strstr(teamlist, model);
			char* sofar = const_cast<char*>(teamlist);

			int team = 0;

			if (sofar == NULL)
				team = -1;
			else
			{
				// count ";"s
				while (sofar < pos)
				{
					if (*sofar == ';')
						team++;

					sofar = sofar + 1;
				}
			}
		}

		return -1;
	}
	case MOD_BG:
		return pEntity->v.team;
	case MOD_TFC:
		return pEntity->v.team - 1;
	case MOD_SVENCOOP:
		return -1;
	default:
		break;
	}
	// UPDATE
	return -1;
}

void BotFunc_KickBotFromTeam(int iTeam)
{
	int i;

	// list of possible bots to kick
	dataUnconstArray<CBot*> theBots;
	CBot* pBot;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		pBot = &gBotGlobals.m_Bots[i];

		if (pBot && pBot->IsUsed())
		{
			if (iTeam == -1 || pBot->pev->team == iTeam)
				theBots.Add(pBot);
		}
	}

	if (!theBots.IsEmpty())
	{
		pBot = theBots.Random();

		if (pBot)
			pBot->m_bKick = TRUE;

		theBots.Clear();
	}

	return;
}

void UTIL_GetArgFromString(char** szString, char* szArg)
{
	*szArg = 0;

	while (**szString && **szString == ' ')
		szString = szString + 1;

	while (**szString && **szString != ' ')
	{
		*szArg = **szString;
		szString = szString + 1; // add size of char (i know it's 1 for a fact :p)
		szArg = szArg + 1;
	}
}

void UTIL_GetArgsFromString(char* szString, char* szArg1, char* szArg2, char* szArg3, char* szArg4)
{
	UTIL_GetArgFromString(&szString, szArg1);
	UTIL_GetArgFromString(&szString, szArg2);
	UTIL_GetArgFromString(&szString, szArg3);
	UTIL_GetArgFromString(&szString, szArg4);
}

// return class number 0 through N
int UTIL_GetClass(edict_t* pEntity)
{
	char* infobuffer;
	char model_name[32];

	infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)(pEntity);
	strcpy(model_name, g_engfuncs.pfnInfoKeyValue(infobuffer, "model"));

	return 0;
}

int UTIL_GetBotIndex(const edict_t* pEdict)
{
	int index;
	CBot* pBot;

	//assert(pEdict != NULL);

	for (index = 0; index < MAX_PLAYERS; index++)
	{
		pBot = &gBotGlobals.m_Bots[index];

		if (!pBot)
			continue;

		if (!pBot->IsUsed())
			continue;

		if (pBot->m_pEdict == pEdict)
		{
			return index;
		}
	}
	return -1;  // return -1 if edict is not a bot
}

CBot* UTIL_GetBotPointer(const edict_t* pEdict)
{
	int index;
	CBot* pBot;

	//assert(pEdict != NULL);

	for (index = 0; index < MAX_PLAYERS; index++)
	{
		pBot = &gBotGlobals.m_Bots[index];

		/// holy crap, this check needed to be in
		if (!pBot->IsUsed())
			continue;

		if (pBot->m_pEdict == pEdict)
		{
			return pBot;
		}
	}

	return NULL;  // return NULL if edict is not a bot
}

float UTIL_EntityAnglesToVector2D(entvars_t* pev, const Vector* pOrigin) // For 2d Movement
{
	Vector2D vec2LOS;
	float    flDot;

	UTIL_MakeVectors(pev->v_angle);

	vec2LOS = (*pOrigin - pev->origin).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct(vec2LOS, gpGlobals->v_forward.Make2D());

	return (float)(acos(flDot) / 3.141592654 * 180);
}

float UTIL_EntityAnglesToVector3D(entvars_t* pev, const Vector* pOrigin) // For 3d Movement (e.g. swimming)
{
	Vector vecLOS;
	float  flDot;

	UTIL_MakeVectors(pev->v_angle);

	vecLOS = *pOrigin - pev->origin;
	vecLOS = vecLOS.Normalize();

	flDot = DotProduct(vecLOS, gpGlobals->v_forward);

	return (float)(acos(flDot) / 3.141592f) * 180.0f;
}

int UTIL_ClassOnTeam(int iClass, int iTeam)
{
	int i;
	edict_t* pPlayer;
	int iPlayers = 0;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (pPlayer == NULL)
			continue;
		if (FNullEnt(pPlayer))
			continue;
		if (!pPlayer->v.classname || !*STRING(pPlayer->v.classname))
			continue;
		if (pPlayer->v.team == iTeam && pPlayer->v.playerclass == iClass)
			iPlayers++;
	}

	return iPlayers;
}

int UTIL_PlayersOnTeam(int iTeam)
{
	int i;
	edict_t* pPlayer;
	int iPlayers = 0;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (pPlayer == NULL)
			continue;
		if (FNullEnt(pPlayer))
			continue;
		if (!pPlayer->v.classname || !*STRING(pPlayer->v.classname))
			continue;
		if (pPlayer->v.team == iTeam)
			iPlayers++;
	}

	return iPlayers;
}

BOOL UTIL_CanEvolveInto(int iSpecies)
{
	switch (iSpecies)
	{
	case ALIEN_LIFEFORM_ONE:
	case ALIEN_LIFEFORM_TWO:
		return TRUE;
	case ALIEN_LIFEFORM_THREE:
	case ALIEN_LIFEFORM_FOUR:
	case ALIEN_LIFEFORM_FIVE:
		return UTIL_GetNumHives() > 1;
	}

	return FALSE;
}

int UTIL_GetNumHives(void)
{
	edict_t* pHive = NULL;
	int iNum = 0;

	while ((pHive = UTIL_FindEntityByClassname(pHive, "team_hive")) != NULL)
	{
		if (pHive->v.fuser1 > 0 && !(pHive->v.iuser4 & MASK_BUILDABLE))//EntityIsAlive(pHive) )
			iNum++;
	}

	return iNum;
}

float UTIL_AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = destAngle - srcAngle;
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

void UTIL_CountBuildingsInRange(Vector const vOrigin, float fRange, int* iDefs, int* iOffs, int* iSens, int* iMovs)
{
	edict_t* pEnt = NULL;

	while ((pEnt = UTIL_FindEntityInSphere(pEnt, vOrigin, fRange)) != NULL)
	{
		switch (pEnt->v.iuser3)
		{
		case AVH_USER3_OFFENSE_CHAMBER:
			*iOffs = *iOffs - 1;
			break;
		case AVH_USER3_DEFENSE_CHAMBER:
			*iDefs = *iDefs - 1;
			break;
		case AVH_USER3_SENSORY_CHAMBER:
			*iSens = *iSens - 1;
			break;
		case AVH_USER3_MOVEMENT_CHAMBER:
			*iMovs = *iMovs - 1;
			break;
		default:
			break;
		}
	}
}

// finding if a resource fountain is occupied
BOOL UTIL_FuncResourceIsOccupied(edict_t* pFuncResource)
{
	edict_t* pResourceTower = NULL;

	Vector vOrigin;

	while ((pResourceTower = UTIL_FindEntityByClassname(pResourceTower, "alienresourcetower")) != NULL)
	{
		if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
		{
			if (pResourceTower->v.groundentity == pFuncResource)
				return TRUE;
		}
		else
		{
			if (pResourceTower->v.origin.x == pFuncResource->v.origin.x &&
				pResourceTower->v.origin.y == pFuncResource->v.origin.y &&
				fabs(pResourceTower->v.origin.x - pFuncResource->v.origin.x) <= 1.0)
			{
				return TRUE;
			}
		}
	}

	pResourceTower = NULL;

	// check marine resource towers
	while ((pResourceTower = UTIL_FindEntityByClassname(pResourceTower, "resourcetower")) != NULL)
	{
		if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
		{
			if (pResourceTower->v.groundentity == pFuncResource)
				return TRUE;
		}
		else
		{
			if (pResourceTower->v.origin.x == pFuncResource->v.origin.x &&
				pResourceTower->v.origin.y == pFuncResource->v.origin.y &&
				fabs(pResourceTower->v.origin.x - pFuncResource->v.origin.x) <= 1.0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// from old RCBOT
Vector UTIL_LengthFromVector(Vector const relation, float length)
{
	return relation / relation.Length() * length;
}

BOOL UTIL_IsResourceFountainUsed(edict_t* pFountain)
{
	edict_t* pTower;

	pTower = NULL;

	while ((pTower = UTIL_FindEntityByClassname(pTower, "resourcetower")) != NULL)
	{
		if (pTower->v.groundentity == pFountain)
			return TRUE;
	}

	pTower = NULL;

	while ((pTower = UTIL_FindEntityByClassname(pTower, "alienresourcetower")) != NULL)
	{
		if (pTower->v.groundentity == pFountain)
			return TRUE;
	}

	return FALSE;
}

edict_t* UTIL_FindRandomUnusedFuncResource(CBot* pBot)
{
	edict_t* pFuncResource = NULL;

	dataUnconstArray<edict_t*> theResources;
	//	int iMem;

	Vector vOrigin = pBot->pev->origin;

	while ((pFuncResource = UTIL_FindEntityByClassname(pFuncResource, "func_resource")) != NULL)
	{
		if (pBot->HasVisitedResourceTower(pFuncResource))
			continue;
		if (UTIL_FuncResourceIsOccupied(pFuncResource))
			continue;

		theResources.Add(pFuncResource);
	}

	if (theResources.IsEmpty())
		return NULL;

	edict_t* pResource = theResources.Random();

	theResources.Clear();

	return pResource;
}

int UTIL_CountBuiltEntities(char* classname)
{
	edict_t* pEnt = NULL;
	int iTotal = 0;

	while ((pEnt = UTIL_FindEntityByClassname(pEnt, classname)) != NULL)
	{
		if (!EntityIsBuildable(pEnt))
			iTotal++;
	}

	return iTotal;
}

int UTIL_CountEntities(char* classname)
{
	edict_t* pEnt = NULL;
	int iTotal = 0;

	while ((pEnt = UTIL_FindEntityByClassname(pEnt, classname)) != NULL)
	{
		iTotal++;
	}

	return iTotal;
}

int UTIL_CountEntitiesInRange(char* classname, Vector vOrigin, float fRange)
{
	edict_t* pEnt = NULL;
	int iTotal = 0;

	while ((pEnt = UTIL_FindEntityByClassname(pEnt, classname)) != NULL)
	{
		if ((EntityOrigin(pEnt) - vOrigin).Length() <= fRange)
			iTotal++;
	}

	return iTotal;
}

BOOL UTIL_CanBuildHive(entvars_t* pev)
{
	return pev->fuser1 == 0.0f;
}

BOOL UTIL_OnGround(entvars_t* pev)
{
	return pev->flags & FL_ONGROUND && pev->movetype == MOVETYPE_WALK;
}

edict_t* UTIL_GetRandomUnbuiltHive(void)
{
	dataUnconstArray<edict_t*> m_Hives;
	edict_t* pEnt = NULL;

	m_Hives.Init();

	while ((pEnt = UTIL_FindEntityByClassname(pEnt, "team_hive")) != NULL)
	{
		if (UTIL_CanBuildHive(&pEnt->v))
			m_Hives.Add(pEnt);
	}

	if (m_Hives.IsEmpty())
		return NULL;

	assert(m_Hives.Size() > 0);

	pEnt = m_Hives.Random();

	m_Hives.Clear();

	return pEnt;
}

int UTIL_GetBuildWaypoint(Vector const vSpawn, dataStack<int>* iFailedGoals)
{
	dataStack<int> iWaypoints;

	WaypointLocations.FillNearbyWaypoints(vSpawn, &iWaypoints, iFailedGoals);

	dataStack<int> tempStack = iWaypoints;
	dataUnconstArray<int> iCandidates;

	edict_t* pent;
	int iWpt;

	BOOL bAdd = TRUE;

	while (!tempStack.IsEmpty())
	{
		pent = NULL;
		bAdd = TRUE;

		iWpt = tempStack.ChooseFromStack();

		if (UTIL_PointContents(WaypointOrigin(iWpt)) == CONTENTS_WATER)
		{
			bAdd = FALSE;
			break;
		}

		while ((pent = UTIL_FindEntityInSphere(pent, WaypointOrigin(iWpt), 32)) != NULL)
		{
			if (FStrEq(STRING(pent->v.classname), "func_nobuild"))
			{
				bAdd = FALSE;
				break;
			}
		}

		if (bAdd)
			iCandidates.Add(iWpt);
	}

	if (iCandidates.IsEmpty())
		return -1;

	iWpt = iCandidates.Random();
	iWaypoints._delete();

	iCandidates.Clear();

	return iWpt;
}

int UTIL_SpeciesOnTeam(int iSpecies, BOOL bIgnoreEmbryos)
{
	int i;
	edict_t* pPlayer;
	int iPlayers = 0;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (FNullEnt(pPlayer))
			continue;
		if (pPlayer->v.iuser3 == iSpecies)
			iPlayers++;
		// Add gestating players to it, since we dont know what they're
		// transforming into
		if (!bIgnoreEmbryos && pPlayer->v.iuser3 == AVH_USER3_ALIEN_EMBRYO)
			iPlayers++;
	}
	return iPlayers;
}

int UTIL_TFC_getMaxArmor(edict_t* pEdict)
{
	switch (pEdict->v.playerclass)
	{
	case TFC_CLASS_SCOUT:
		return 50;
	case TFC_CLASS_SNIPER:
		return 50;
	case TFC_CLASS_SOLDIER:
		return 200;
	case TFC_CLASS_DEMOMAN:
		return 120;
	case TFC_CLASS_MEDIC:
		return 100;
	case TFC_CLASS_HWGUY:
		return 300;
	case TFC_CLASS_PYRO:
		return 150;
	case TFC_CLASS_SPY:
		return 100;
	case TFC_CLASS_ENGINEER:
		return 50;
	case TFC_CLASS_CIVILIAN:
		return 0;
	}

	return 0;
}

BOOL BotFunc_FInViewCone(Vector* pOrigin, edict_t* pEdict)
{
	Vector2D vec2LOS;
	float    flDot;

	UTIL_MakeVectors(pEdict->v.angles);

	vec2LOS = (*pOrigin - pEdict->v.origin).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct(vec2LOS, gpGlobals->v_forward.Make2D());

	return flDot > 0.50;  // 60 degree field of view
}

BOOL BotFunc_FVisible(const Vector& vecOrigin, edict_t* pEdict)
{
	TraceResult tr;
	Vector      vecLookerOrigin;

	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;

	int bInWater = UTIL_PointContents(vecOrigin) == CONTENTS_WATER;
	int bLookerInWater = UTIL_PointContents(vecLookerOrigin) == CONTENTS_WATER;

	// don't look through water
	if (bInWater != bLookerInWater)
		return FALSE;

	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);

	if (tr.flFraction != 1.0)
	{
		return FALSE;  // Line of sight is not established
	}
	else
	{
		return TRUE;  // line of sight is valid.
	}
}

Vector GetGunPosition(edict_t* pEdict)
{
	return pEdict->v.origin + pEdict->v.view_ofs;
}

void UTIL_SelectItem(edict_t* pEdict, char* item_name)
{
	//   FakeClientCommand(pEdict, item_name, NULL, NULL);
}

//#ifndef RCBOT_META_BUILD

void ExplosionCreate(const Vector& center, const Vector& angles, edict_t* pOwner, int magnitude, BOOL doDamage)
{
	/*if ( gBotGlobals.IsMod(MOD_SVENCOOP) )
	{
		KeyValueData	kvd;
		char			buf[128];

		edict_t *pExplosion = CREATE_NAMED_ENTITY(MAKE_STRING("env_explosion"));
		sprintf( buf, "%3d", magnitude );
		kvd.szKeyName = "iMagnitude";
		kvd.szValue = buf;

		DispatchKeyValue(pExplosion,&kvd);
		if ( !doDamage )
			pExplosion->v.spawnflags |= (1<<0);//SF_ENVEXPLOSION_NODAMAGE;

		pExplosion->v.origin = center;
		pExplosion->v.angles = pExplosion->v.v_angle = angles;
		pExplosion->v.owner = pOwner;

		DispatchSpawn(pExplosion);//->Spawn();
		DispatchUse(pExplosion,pOwner);//->Use( (CBaseEntity*)GET_PRIVATE(pOwner), (CBaseEntity*)GET_PRIVATE(pOwner), USE_TOGGLE, 0 );
	}
	else if ( !gBotGlobals.IsMod(MOD_SVENCOOP) )
	{
		KeyValueData	kvd;
		char			buf[128];

		CBaseEntity *pExplosion = CreateEnt( "env_explosion", center, angles, pOwner );
		sprintf( buf, "%3d", magnitude );
		kvd.szKeyName = "iMagnitude";
		kvd.szValue = buf;
		pExplosion->KeyValue( &kvd );
		if ( !doDamage )
			pExplosion->pev->spawnflags |= (1<<0);//SF_ENVEXPLOSION_NODAMAGE;

		pExplosion->Spawn();
		pExplosion->Use( (CBaseEntity*)GET_PRIVATE(pOwner), (CBaseEntity*)GET_PRIVATE(pOwner), USE_TOGGLE, 0 );
	}*/
	/*
	#define	TE_EXPLOSION		3		// additive sprite, 2 dynamic lights, flickering particles, explosion sound, move vertically 8 pps
	// coord coord coord (position)
	// short (sprite index)
	// byte (scale in 0.1's)
	// byte (framerate)
	// byte (flags)	*/
	//	else
	{
		int byteMag = magnitude;

		if (byteMag > 255)
			byteMag = 255;

		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, center);
		WRITE_BYTE(TE_EXPLOSION);
		WRITE_COORD(center.x);
		WRITE_COORD(center.y);
		WRITE_COORD(center.z);
		WRITE_SHORT(gBotGlobals.m_sModelIndexFireball);
		WRITE_BYTE((BYTE)byteMag); // scale * 10
		WRITE_BYTE(15); // framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);
		MESSAGE_END();

		float mag = magnitude * 4;

		if (doDamage)
		{
			//g_engfuncs.
			//extern void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
			RadiusDamage(center, &pOwner->v, &pOwner->v, magnitude, magnitude / 2, 0, 1 << 6);
		}
	}
}

void RadiusDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	CBaseEntity* pEntity = NULL;
	edict_t* pEdict = NULL;

	TraceResult	tr;
	float		flAdjustedDamage, falloff;
	Vector		vecSpot;

	if (flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1.0;

	int bInWater = UTIL_PointContents(vecSrc) == CONTENTS_WATER;

	vecSrc.z += 1;// in case grenade is lying on the ground

	if (!pevAttacker)
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEdict = UTIL_FindEntityInSphere(pEdict, vecSrc, flRadius)) != NULL)
	{
		pEntity = static_cast<CBaseEntity*>(GET_PRIVATE(pEdict));

		if (pEntity && pEntity->pev && pEntity->edict() && !pEntity->edict()->free)
		{
			if (pEntity->pev->takedamage != DAMAGE_NO)
			{
				// UNDONE: this should check a damage mask, not an ignore
				if (iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore)
				{// houndeyes don't hurt other houndeyes with their attack
					continue;
				}

				// blast's don't tavel into or out of water
				if (bInWater && pEntity->pev->waterlevel == 0)
					continue;
				if (!bInWater && pEntity->pev->waterlevel == 3)
					continue;

				vecSpot = pEntity->BodyTarget(vecSrc);

				UTIL_TraceLine(vecSrc, vecSpot, dont_ignore_monsters, ENT(pevInflictor), &tr);

				if (tr.flFraction == 1.0 || tr.pHit == pEntity->edict())
				{// the explosion can 'see' this entity, so hurt them!
					if (tr.fStartSolid)
					{
						// if we're stuck inside them, fixup the position and distance
						tr.vecEndPos = vecSrc;
						tr.flFraction = 0.0;
					}

					// decrease damage for an ent that's farther from the bomb.
					flAdjustedDamage = (vecSrc - tr.vecEndPos).Length() * falloff;
					flAdjustedDamage = flDamage - flAdjustedDamage;

					if (flAdjustedDamage < 0)
					{
						flAdjustedDamage = 0;
					}
					/*
						// ALERT( at_console, "hit %s\n", STRING( pEntity->pev->classname ) );
						if (tr.flFraction != 1.0)
						{
							//ClearMultiDamage( );
							pEntity->TraceAttack( pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize( ), &tr, bitsDamageType );
							//ApplyMultiDamage( pevInflictor, pevAttacker );
						}
						else
						{*/
					pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);

					pEntity->pev->velocity = (pEntity->pev->origin - vecSrc).Normalize() * flAdjustedDamage;
					//}
				}
			}
		}
	}
}

CBaseEntity* CreateEnt(char* szName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner)
{
	edict_t* pent;
	CBaseEntity* pEntity;

	pent = CREATE_NAMED_ENTITY(MAKE_STRING(szName));
	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in Create!\n");
		return NULL;
	}

	if (!pent)
		pent = ENT(0);
	pEntity = static_cast<CBaseEntity*>(GET_PRIVATE(pent));

	pEntity->pev->owner = pentOwner;
	pEntity->pev->origin = vecOrigin;
	pEntity->pev->angles = vecAngles;
#ifdef RCBOT_META_BUILD
	MDLL_Spawn(pEntity->edict());
#else
	DispatchSpawn(pEntity->edict());
#endif
	return pEntity;
}
//#endif

Vector EntityOrigin(edict_t* pEdict)
{
	//if ( pEdict->v.flags & FL_WORLDBRUSH )

	if (gBotGlobals.m_iCurrentMod == MOD_SVENCOOP && pEdict->v.flags & FL_MONSTER)
		return pEdict->v.origin + pEdict->v.view_ofs / 2;

	return pEdict->v.absmin + pEdict->v.size / 2;

	//return pEdict->v.origin;
}

Vector AbsOrigin(edict_t* pEdict)
{
	return (pEdict->v.absmin + pEdict->v.absmax) / 2;
}

void UTIL_ShowMenu(edict_t* pEdict, int slots, int displaytime, BOOL needmore, char* pText)
{
	const char* szMsg = { "ShowMenu" };

	int msg_id = GetMessageID(szMsg);

	if (msg_id > 0)
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, msg_id, NULL, pEdict);

		g_engfuncs.pfnWriteShort(slots);
		g_engfuncs.pfnWriteChar(displaytime);
		g_engfuncs.pfnWriteByte(needmore);
		g_engfuncs.pfnWriteString(pText);

		g_engfuncs.pfnMessageEnd();
	}
}

BOOL UTIL_CanStand(Vector const origin, Vector* v_floor)
{
	TraceResult tr;

	Vector v_src = origin;

	UTIL_TraceLine(v_src, v_src - Vector(0, 0, 144), ignore_monsters, ignore_glass, NULL, &tr);

	*v_floor = tr.vecEndPos;
	float len = v_src.z - tr.vecEndPos.z;

	UTIL_TraceLine(v_src, v_src + Vector(0, 0, 144), ignore_monsters, ignore_glass, NULL, &tr);

	len += tr.vecEndPos.z - v_src.z;

	return len > 72;
}

BOOL UTIL_makeTSweapon(edict_t* pOwner, eTSWeaponID weaponid)
{
	if (weaponid == 0)
		return FALSE;

	edict_t* pWeapon = CREATE_NAMED_ENTITY(MAKE_STRING("ts_groundweapon"));

	if (pWeapon == NULL || FNullEnt(pWeapon))
		return FALSE;

	char* keyname = "tsweaponid";
	char keyvalue[8];

	KeyValueData key;

	sprintf(keyvalue, "%d", weaponid);

	key.szClassName = "ts_groundweapon";
	key.szKeyName = keyname;
	key.szValue = keyvalue;
	key.fHandled = 0;

#ifndef RCBOT_META_BUILD
	DispatchKeyValue(pWeapon, &key);
#else
	MDLL_KeyValue(pWeapon, &key);
#endif

	key.szKeyName = "wextraclip";
	key.szValue = "200";
	key.fHandled = 0;

#ifndef RCBOT_META_BUILD
	DispatchKeyValue(pWeapon, &key);
#else
	MDLL_KeyValue(pWeapon, &key);
#endif

	key.szKeyName = "wduration";
	key.szValue = "-1";
	key.fHandled = 0;

#ifndef RCBOT_META_BUILD
	DispatchKeyValue(pWeapon, &key);
#else
	MDLL_KeyValue(pWeapon, &key);
#endif

	key.szKeyName = "spawnflags";
	key.szValue = "1073741824";
	key.fHandled = 0;

#ifndef RCBOT_META_BUILD
	DispatchKeyValue(pWeapon, &key);
#else
	MDLL_KeyValue(pWeapon, &key);
#endif

	pWeapon->v.spawnflags = 1 << 30;

#ifndef RCBOT_META_BUILD
	DispatchSpawn(pWeapon);
#else
	MDLL_Spawn(pWeapon);
#endif

	//pWeapon->v.owner = pOwner;
	UTIL_SetOrigin(&pWeapon->v, pOwner->v.origin);

#ifndef RCBOT_META_BUILD
	//DispatchTouch(pWeapon,pOwner);
	DispatchUse(pWeapon, pOwner);
#else
	MDLL_Use(pWeapon, pOwner);
#endif

	pWeapon->v.flags |= FL_KILLME;

	return TRUE;
}

void UTIL_BuildFileName(char* filename, char* arg1, char* arg2)
{
	// Build file name will set up the directoy for the filename
	strcpy(filename, gBotGlobals.botFolder());
#ifndef __linux__
	strcat(filename, "\\");
#else
	strcat(filename, "/");
#endif

	if (arg1 && *arg1 && arg2 && *arg2)
	{
		strcat(filename, arg1);

#ifndef __linux__
		strcat(filename, "\\");
#else
		strcat(filename, "/");
#endif

		strcat(filename, arg2);
	}
	else if (arg1 && *arg1)
	{
		strcat(filename, arg1);
	}
}

edict_t* UTIL_FacingEnt(edict_t* pPlayer, BOOL any)
{
	TraceResult tr;
	entvars_t* pev = &pPlayer->v;

	Vector vSrc;
	Vector vEnd;

	UTIL_MakeVectors(pev->v_angle);

	vSrc = pev->origin + pev->view_ofs;
	vEnd = vSrc + gpGlobals->v_forward * 4096.0;

	UTIL_TraceLine(vSrc, vEnd, dont_ignore_monsters, dont_ignore_glass, pPlayer, &tr);

	if (tr.pHit)
	{
		if (any || (tr.pHit->v.flags & FL_CLIENT || tr.pHit->v.flags & FL_MONSTER))
			return tr.pHit;
	}

	return NULL;
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf(char* fmt, ...)
{
	va_list        argptr;
	static char    string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "%s", string);
}

void UTIL_FixAngles(Vector* vAngles)
{
	UTIL_FixFloatAngle(&vAngles->x);
	UTIL_FixFloatAngle(&vAngles->y);
	UTIL_FixFloatAngle(&vAngles->z);
}

void UTIL_FixFloatAngle(float* fAngle)
{
	short int iLoops; // safety

	iLoops = 0;

	if (*fAngle < -180)
	{
		while (iLoops < 4 && *fAngle < -180)
		{
			*fAngle += 360.0;
			iLoops++;
		}
	}
	else if (*fAngle > 180)
	{
		while (iLoops < 4 && *fAngle > 180)
		{
			*fAngle -= 360.0;
			iLoops++;
		}
	}

	if (iLoops >= 4)
		*fAngle = 0; // reset
}

void UTIL_PlaySoundToAll(const char* szSound)
{
	int i;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pEdict = INDEXENT(i);

		if (FNullEnt(pEdict))
			continue;
		if (!*STRING(pEdict->v.netname))
			continue;

		UTIL_PlaySound(pEdict, szSound);
	}
}

void UTIL_PlaySound(edict_t* pPlayer, const char* szSound)
{
	char szCommand[128];

	if (pPlayer->v.flags & FL_FAKECLIENT)
		return;

	sprintf(szCommand, "play %s\n", szSound);

	CLIENT_COMMAND(pPlayer, szCommand);
}

float UTIL_EntityDistance(entvars_t* pev, Vector vOrigin)
{
	return (pev->origin - vOrigin).Length();
}

float UTIL_EntityDistance2D(entvars_t* pev, Vector vOrigin)
{
	return (pev->origin - vOrigin).Length2D();
}

float UTIL_GetBestPushableDistance(edict_t* pPushable)
{
	return (pPushable->v.size.x + pPushable->v.size.y) / 3;
}

Vector UTIL_GetDesiredPushableVector(Vector vOrigin, edict_t* pPushable)
{
	Vector vPushable = EntityOrigin(pPushable);

	return vPushable - (vOrigin - vPushable).Normalize() * UTIL_GetBestPushableDistance(pPushable);
}

BOOL UTIL_AcceptablePushableVector(edict_t* pPushable, Vector const vOrigin)
{
	return UTIL_EntityDistance2D(&pPushable->v, vOrigin) <= UTIL_GetBestPushableDistance(pPushable);
}

void HudText::SayMessage(const char* message, Vector const colour1, Vector const colour2, edict_t* pPlayer)
{
	SetColour1(colour1, 75);
	SetColour2(colour2, 75);

	SayMessage(message, pPlayer);
}

void HudText::SayMessage(const char* message, edict_t* pPlayer)
{
	InitMessage(message);

	if (pPlayer)
		UTIL_BotHudMessage(pPlayer, m_textParms, m_sMessage);
	else
		UTIL_BotHudMessageAll(m_textParms, m_sMessage);
}

void HudText::InitMessage(const char* message)
{
	short int length, i;

	strncpy(m_sMessage, message, HUD_TEXT_LENGTH - 1);

	m_sMessage[HUD_TEXT_LENGTH - 1] = 0;

	length = strlen(m_sMessage);

	i = 0;

	while (i < length)
	{
		if (m_sMessage[i] == '|')
			m_sMessage[i] = ' ';

		if (m_sMessage[i] == '^')
			m_sMessage[i] = '\n';

		i++;
	}
}

void HudText::Initialise(void)
{
	m_textParms.channel = 1;
	m_textParms.x = -1;
	m_textParms.y = -1;
	m_textParms.effect = 2;
	m_textParms.r1 = 140;
	m_textParms.g1 = 140;
	m_textParms.b1 = 140;
	m_textParms.a1 = 255;
	m_textParms.r2 = 255;
	m_textParms.g2 = 255;
	m_textParms.b2 = 255;
	m_textParms.a2 = 200;
	m_textParms.fadeinTime = 0.2;
	m_textParms.fadeoutTime = 0.5;
	m_textParms.holdTime = 4;
	m_textParms.fxTime = 0.5;
}

void UTIL_BotHudMessageAll(const hudtextparms_t& textparms, const char* pMessage)
{
	int	i;

	CClient* pClient;

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		pClient = gBotGlobals.m_Clients.GetClientByIndex(i);

		if (pClient && pClient->IsUsed())
		{
			UTIL_BotHudMessage(pClient->GetPlayer(), textparms, pMessage);
		}
	}
}

void UTIL_BotHudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage)
{
	if (!pEntity || !(pEntity->v.flags & FL_CLIENT) || pEntity->v.flags & FL_FAKECLIENT)
		return;

	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE(TE_TEXTMESSAGE);
	WRITE_BYTE(textparms.channel & 0xFF);

	WRITE_SHORT(FixedSigned16(textparms.x, 1 << 13));
	WRITE_SHORT(FixedSigned16(textparms.y, 1 << 13));
	WRITE_BYTE(textparms.effect);

	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(textparms.a1);

	WRITE_BYTE(textparms.r2);
	WRITE_BYTE(textparms.g2);
	WRITE_BYTE(textparms.b2);
	WRITE_BYTE(textparms.a2);

	WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1 << 8));

	if (textparms.effect == 2)
		WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1 << 8));

	if (strlen(pMessage) < 512)
	{
		WRITE_STRING(pMessage);
	}
	else
	{
		char tmp[512];
		strncpy(tmp, pMessage, 511);
		tmp[511] = 0;
		WRITE_STRING(tmp);
	}
	MESSAGE_END();
}

unsigned short FixedUnsigned16(float value, float scale)
{
	int output;

	output = value * scale;
	if (output < 0)
		output = 0;
	if (output > 0xFFFF)
		output = 0xFFFF;

	return static_cast<unsigned short>(output);
}

short FixedSigned16(float value, float scale)
{
	int output;

	output = value * scale;

	if (output > 32767)
		output = 32767;

	if (output < -32768)
		output = -32768;

	return (short)output;
}

/*
	BOT_TOOL_TIP_SERVER_WELCOME = 0,
	BOT_TOOL_TIP_CLIENT_WELCOME,
	BOT_TOOL_TIP_SQUAD_HELP,
	BOT_TOOL_TIP_ADMIN_HELP,
	BOT_TOOL_TIP_CPU_HELP,
	BOT_TOOL_TIP_NO_WAYPOINTS,
	BOT_TOOL_TIP_CREATE_SQUAD,
	BOT_TOOL_TIP_COMMANDER_WELCOME,
	BOT_TOOL_TIP_COMMANDER_MARINE_ORDER,
	BOT_TOOL_TIP_COMMANDER_SQUAD,
	BOT_TOOL_TIP_COMMANDER_SQUAD2,
	BOT_TOOL_TIP_COMMANDER_HEALTH,
	BOT_TOOL_TIP_BOTCAM_HELP,
	BOT_TOOL_TIP_BOTCAM_ON,
	BOT_TOOL_TIP_MAX
*/

void UTIL_BotToolTip(edict_t* pEntity, eLanguage iLang, eToolTip iTooltip)
//
// Show a Tooltip (message) to the pEntity
//
{
	static HudText hudmessage = HudText(TRUE);
	static char* tooltips[BOT_LANG_MAX][BOT_TOOL_TIP_MAX] =
	{
		//---------------------------------------------------------------------------------------------------------------------<MAX
		{"Welcome %n\nUse the command \"rcbot addbot\" to add a bot\nor use the bot menu (\"rcbot bot_menu\")",
		 "Welcome %n\nServer running RCBot\n(http://rcbot.bots-united.com)",
		 "You can use the squad menu\n(\"rcbot squad_menu\" command) to edit bot squads\ne.g. formation and mode",
		 "You can gain access to rcbot commands if you have\na password on the server\nuse \"rcbot set_pass <password>\"",
		 "CPU Usage of the bot can be decreased by changing some\ncommands like: \"rcbot config max_vision_revs\"",
		 "There are no waypoints on this map\nThis will impair bots navigation\nYou can make your own, (use \"rcbot waypoint\")",
		 "You can create a squad of bots as the leader\nUse the squad menu (rcbot squad_menu)\nselect \"more options\", \"squad\"",
		 "You are now commander\nSelect marines (mouse1) and give orders (mouse2)\nOr select buildings to build (mouse1)",
		 "A marine is looking for orders\nSelect the marine with the marine icon (left) and\ngive orders with mouse2 button",
		 "As commander you can create a squad of marines\nSelect all marines you want\nand hold CTRL then a number 1-5",
		 "You can select the squad of marines you made by\npressing the number of the squad you made",
		 "A marine needs health\nSelect the health pack on the bottom right\nand place it above the marine in need",
		 "You can turn on the bot cam\nto see all the bots in action\nUse the command \"rcbot botcam\"",
		 "Bot cam is now ON\nYou are now watching the bot cam\nUse the command \"rcbot botcam off\" to switch off"
		}
	};

	// check it tooltips are on
	// check if array positions are in bounds
	if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TOOLTIPS) && gBotGlobals.IsConfigSettingOn(iLang < BOT_LANG_MAX) && iTooltip < BOT_TOOL_TIP_MAX)
	{
		char final_message[1024];

		sprintf(final_message, "%s %s", BOT_DBG_MSG_TAG, tooltips[iLang][iTooltip]);

		// name in message
		if (strstr(final_message, "%n") != NULL)
		{
			char* szName = const_cast<char*>(STRING(pEntity->v.netname));
			int iLen = strlen(szName);
			char* szNewName = static_cast<char*>(malloc(sizeof(char) * (iLen + 1)));

			RemoveNameTags(szName, szNewName);

			BotFunc_FillString(final_message, "%n", szNewName, 1024);

			free(szNewName);
			szNewName = NULL;
		}

		//if ( strstr(final_message,"%v") != NULL )
		//{
		//BotFunc_FillString(final_message,"%v",BOT_VER,1024);
		//}

		hudmessage.SayMessage(final_message, pEntity);
	}
}

edict_t* UTIL_UpdateSounds(entvars_t* pev)
//
// Return the entity that could be making a sound
// nearest to "pev"
{
	int i;
	edict_t* pPlayer;
	entvars_t* pPlayerpev;

	float fNearest = 750;
	float fDistance;
	edict_t* pNearest = NULL;

	BOOL bSameTeam;

	edict_t* pEdict = ENT(pev);

	BOOL bAdd;

	int iMyTeam = UTIL_GetTeam(pEdict);

	// listen to footsteps if:
	// i. footsteps are on AND..
	// ii. mod isnt svencoop (dont listen to teammates... (except when shooting)
	// iii. OR mod is bumpercars (hear engine always)
	BOOL bListenToFootSteps = CVAR_GET_FLOAT("mp_footsteps") > 0 && (gBotGlobals.m_iCurrentMod != MOD_SVENCOOP || gBotGlobals.m_iCurrentMod == MOD_BUMPERCARS);

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (FNullEnt(pPlayer))
			continue;

		if (pPlayer == pEdict)
			continue;

		pPlayerpev = &pPlayer->v;

		if (!EntityIsAlive(pPlayer))
			continue;
		if (gBotGlobals.IsNS() && EntityIsCommander(pPlayer))
			continue;

		if (!*STRING(pPlayerpev->netname))
			continue;

		bSameTeam = UTIL_GetTeam(pPlayer) == iMyTeam;

		bAdd = FALSE;

		if (bListenToFootSteps && !bSameTeam)
		{
			if (pPlayerpev->velocity.Length2D() > 120)
				bAdd = TRUE;

			if (gBotGlobals.IsNS())
			{
				// silence upgrade? can't hear
				if (EntityIsAlien(pPlayer) && pPlayerpev->iuser4 & MASK_UPGRADE_6)
					bAdd = FALSE;
				if (pPlayerpev->iuser4 & MASK_VIS_DETECTED) // detected though...?
					bAdd = TRUE;
			}
		}
		else if (pPlayerpev->button & IN_ATTACK)
			bAdd = TRUE;

		if (bAdd)
		{
			fDistance = (pev->origin - pPlayerpev->origin).Length();

			bAdd = FALSE;

			if (fDistance < fNearest)
			{
				if (bSameTeam && fDistance > 128)
					bAdd = TRUE;
				else if (!bSameTeam && fDistance > 64)
					bAdd = TRUE;
			}

			if (bAdd)
			{
				fNearest = fDistance;
				pNearest = pPlayer;
			}
		}
	}

	return pNearest;
}

BOOL UTIL_IsButton(edict_t* pButton)
{
	char* szClassname = NULL;

	if (pButton == NULL)
		return FALSE;

	szClassname = const_cast<char*>(STRING(pButton->v.classname));

	return strncmp(szClassname, "func_", 5) == 0;
}

edict_t* UTIL_FindNearestEntity(char** szClassnames, int iNames, Vector vOrigin, float fRange, BOOL bVisible, edict_t* pIgnore)
{
	edict_t* pEntity;
	edict_t* pNearest;

	int i;

	char* szClassname;

	float fDistance;

	TraceResult tr;

	BOOL bAdd;

	Vector vEntOrigin;

	pNearest = NULL;

	for (i = 0; i < iNames; i++)
	{
		szClassname = szClassnames[i];

		pEntity = NULL;

		while ((pEntity = UTIL_FindEntityByClassname(pEntity, szClassname)) != NULL)
		{
			if (pEntity == pIgnore)
				continue;

			vEntOrigin = EntityOrigin(pEntity);
			fDistance = (vEntOrigin - vOrigin).Length();

			if (fDistance < fRange)
			{
				bAdd = !bVisible;

				if (!bAdd)
				{
					UTIL_TraceLine(vOrigin, vEntOrigin, ignore_monsters, ignore_glass, NULL, &tr);

					bAdd = tr.flFraction >= 1.0 || tr.pHit == pEntity;
				}

				if (bAdd)
				{
					fRange = fDistance;
					pNearest = pEntity;
				}
			}
		}
	}

	return pNearest;
}

int UTIL_NS_GetMaxArmour(edict_t* pEntity)
{
	int armor = 25;

	if (pEntity->v.iuser4 & MASK_UPGRADE_4)
		armor += 20;	// Marine armor 1
	if (pEntity->v.iuser4 & MASK_UPGRADE_5)
		armor += 15;	// Marine armor 2
	if (pEntity->v.iuser4 & MASK_UPGRADE_6)
		armor += 10;	// Marine armor 3

	return armor;
}

Vector UTIL_FurthestVectorAroundYaw(CBot* pBot)
// Called when bot can't find a nearby waypoint
// instead fire tracelines in FOV and move to the furthest
// point it can find

// Thanks PM's racc bot source for some info pointers. (racc.bots-united.com)
{
	const float fFov = 180.0;
	int iStep = 0;

	entvars_t* pev = pBot->pev;

	//float fStartAngle = pev->angles.y - fFov;
	float fStartAngle = pev->v_angle.y - fFov;
	float fAngle;

	int iMinStep = -180;
	int iMaxStep = 180;

	float fMaxDistance = BOT_WAYPOINT_TOUCH_DIST * 2;
	float fDistance;

	dataUnconstArray<Vector> vPositions;

	Vector vStart = pBot->GetGunPosition();
	Vector vEnd;
	Vector vAngles;

	TraceResult tr;

	UTIL_FixFloatAngle(&fStartAngle);

	fAngle = fStartAngle;

	for (iStep = iMinStep; iStep <= iMaxStep; iStep += 20)
	{
		fAngle = (float)iStep;

		vAngles = pBot->m_pEdict->v.v_angle;

		vAngles.y = vAngles.y + fAngle;
		vAngles.x = 0;

		UTIL_FixFloatAngle(&vAngles.y);

		UTIL_MakeVectors(vAngles);

		vEnd = vStart + gpGlobals->v_forward * 4096;

		UTIL_TraceLine(vStart, vEnd, ignore_monsters, ignore_glass, pBot->m_pEdict, &tr);

		vEnd = tr.vecEndPos;

		fDistance = pBot->DistanceFrom(vEnd);

		if (fDistance > fMaxDistance)
		{
			vPositions.Clear();

			fMaxDistance = fDistance;

			vPositions.Add(vEnd);
		}
		else if (!vPositions.IsEmpty() && fDistance == fMaxDistance)
		{
			vPositions.Add(vEnd);
		}
	}

	if (vPositions.IsEmpty())
	{
		// turn around

		pBot->m_CurrentLookTask = BOT_LOOK_TASK_NEXT_WAYPOINT;

		UTIL_MakeVectors(Vector(0, pev->angles.y, 0));

		UTIL_TraceLine(vStart, vStart + -gpGlobals->v_forward * REACHABLE_RANGE, ignore_monsters, ignore_glass, pev->pContainingEntity, &tr);

		return tr.vecEndPos;
	}

	Vector vPosition = vPositions.Random();

	vPositions.Clear();

	return vPosition;
}

edict_t* UTIL_CheckTeleEntrance(Vector const vOrigin, edict_t* pExit, edict_t* pOwner)
{
	edict_t* pent = NULL;

	if (pExit != NULL)
	{
		if (FNullEnt(pExit) || !FStrEq(STRING(pExit->v.classname), "building_teleporter"))
			pExit = NULL;
	}

	while ((pent = UTIL_FindEntityInSphere(pent, vOrigin, 80)) != NULL)
	{
		if (FStrEq(STRING(pent->v.classname), "building_teleporter"))
		{
			if (pent->v.framerate == 0)
			{
				pent->v.euser2 = pOwner;
				pent->v.iuser1 = 1;
				pent->v.euser1 = pExit;
				return pent;
			}
		}
	}

	return NULL;
}

BOOL UTIL_PlayerStandingOnEntity(edict_t* pEntity, int team, edict_t* pIgnore)
{
	edict_t* pPlayer = NULL;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = INDEXENT(i);

		if (pPlayer == NULL)
			continue;
		if (pPlayer->free)
			continue;
		if (!pPlayer->v.netname)
			continue;
		if (!*STRING(pPlayer->v.netname))
			continue;
		if (pPlayer == pIgnore)
			continue;
		if (pPlayer->v.groundentity == pEntity)
		{
			if (UTIL_GetTeam(pPlayer) == team)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

edict_t* UTIL_CheckTeleExit(Vector const vOrigin, edict_t* pOwner, edict_t* pEntrance)
{
	edict_t* pent = NULL;

	CBot* pBot = UTIL_GetBotPointer(pOwner);

	if (pEntrance != NULL)
	{
		if (FNullEnt(pEntrance) || !FStrEq(STRING(pEntrance->v.classname), "building_teleporter"))
			pEntrance = NULL;
	}

	while ((pent = UTIL_FindEntityInSphere(pent, vOrigin, 80)) != NULL)
	{
		if (FStrEq(STRING(pent->v.classname), "building_teleporter"))
		{
			if (pent->v.framerate == 0)
			{
				pent->v.euser2 = pOwner;
				pent->v.iuser1 = 2;

				if (pEntrance)
					pEntrance->v.euser1 = pent;

				if (pBot)
				{
					pBot->m_vTeleporter = pent->v.origin;
					pBot->m_fNextCheckTeleporterExitTime = gpGlobals->time + 10.0f;
				}

				return pent;
			}
		}
	}
	return NULL;
}

int UTIL_SentryLevel(edict_t* pEntity)
{
	char* model = const_cast<char*>(STRING(pEntity->v.model));

	if (model[13] == '1')
		return 1;
	else if (model[13] == '2')
		return 2;
	else if (model[13] == '3')
		return 3;

	return 0;
}