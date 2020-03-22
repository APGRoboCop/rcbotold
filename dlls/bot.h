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
 // bot.h
 //
 //////////////////////////////////////////////////
 //
 // Bot definitions header
 //
 /*
 * All CLASSES have been made
 *
 *
 *
 *
 *
 */
#ifndef __BOT_H__
#define __BOT_H__

#include "generic_class.h"
#include "bot_const.h"
#include "bot_menu.h"
#include "bot_weapons.h"
#include "bot_client.h"
#include "megahal.h"
#include "bot_ga.h"
#include "bits.h"
#include "nn.h"

#include <vector>
#include <queue>
using namespace std;

#ifndef RCBOT_META_BUILD

// stuff for Win32 vs. Linux builds

#ifndef linux
typedef int (FAR* GETENTITYAPI)(DLL_FUNCTIONS*, int);
typedef int (FAR* GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS*, int*);
typedef void (DLLEXPORT* GIVEFNPTRSTODLL)(enginefuncs_t*, globalvars_t*);
typedef void (FAR* LINK_ENTITY_FUNC)(entvars_t*);
#else

#include <dlfcn.h>
#define GetProcAddress dlsym

typedef int BOOL;

typedef int (*GETENTITYAPI)(DLL_FUNCTIONS*, int);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS*, int*);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t*, globalvars_t*);
typedef void (*LINK_ENTITY_FUNC)(entvars_t*);

#endif

#endif // RCBOT META BUILD

class AvHBaseBuildable;

typedef void (*CONSTRUCT_USE)(AvHBaseBuildable*, CBaseEntity*, int);

/////////////////////
// my Safe-free function.. wonder if it works :o
void safefree(void** p);
//////////////////////////////
// CLIENT FUNCTIONS
int GetMessageID(const char* szMsg);

#ifdef RCBOT_META_BUILD
void GetGameDirectory(char* szDir);
#endif

void DebugMessage(int iDebugLevel, edict_t* pEntity, int errorlevel, char* fmt, ...);
void BotMessage(edict_t* pEntity, int errorlevel, char* fmt, ...);
void BugMessage(edict_t* pEntity, char* fmt, ...);

int GetPlayerRepId(const char* szPlayerName);
int GetPlayerEdictRepId(edict_t* pEdict);

//////////////////////////////
// BOT_H DEFINED FUNCTIONS
void BotFunc_ChangeAngles(float* fSpeed, float* fIdeal, float* fCurrent, float* fUpdate);
BOOL UTIL_FriendlyHatesPlayer(edict_t* pEntity, edict_t* pPlayer);
edict_t* UTIL_getEntityInFront(edict_t* pEntity);
void ExplosionCreate(const Vector& center, const Vector& angles, edict_t* pOwner, int magnitude, BOOL doDamage);
Vector UTIL_GetGroundVector(edict_t* pEdict);
BOOL UTIL_EntityIsHive(edict_t* pEdict);
int UTIL_CountEntitiesInRange(char* classname, Vector vOrigin, float fRange);
int UTIL_SentryLevel(edict_t* pEntity);
void RadiusDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
void UTIL_BotScreenShake(const Vector& center, float amplitude, float frequency, float duration, float radius);
edict_t* UTIL_GetPlayerByPlayerId(int id);
Vector UTIL_FurthestVectorAroundYaw(CBot* pBot);
int UTIL_ClassOnTeam(int iClass, int iTeam);
BOOL UTIL_EntityHasClassname(edict_t* pEntity, char* classname);
float UTIL_GetPlayerEnergy(entvars_t* pev);
int UTIL_NS_GetMaxArmour(edict_t* pEntity);
edict_t* UTIL_GetCommander(void);
int UTIL_CountEntities(char* classname);
edict_t* BotFunc_FindRandomEntity(char* szClassname);
BOOL UTIL_IsGrenadeRocket(edict_t* pEntity);
//void BotFunc_StringCopy(char *, const char *);
BOOL BotFunc_IsLongRangeWeapon(int iId);
BOOL BotFunc_IncreaseRep(int iRep, float fInfo = 0, float fSkill = 0);
BOOL BotFunc_DecreaseRep(int iRep, float fInfo = 0, float fSkill = 0);
int BotFunc_GetRepArrayNum(int iRep);
void BotFunc_TraceToss(edict_t* ent, edict_t* ignore, TraceResult* tr);

CBaseEntity* CreateEnt(char* szName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

BOOL UTIL_PlayerStandingOnEntity(edict_t* pEntity, int team, edict_t* pIgnore = NULL);

BOOL BotFunc_FillString(char* string, const char* fill_point, const char* fill_with, int max_len);

BOOL	EntityIsBuildable(edict_t* pEdict);
BOOL    EntityIsWeldable(edict_t* pEdict);
BOOL	EntityIsAlive(edict_t* pEdict);
BOOL	EntityIsMarine(edict_t* pEdict);
BOOL	EntityIsAlien(edict_t* pEdict);
BOOL    EntityIsCommander(edict_t* pEdict);
BOOL    EntityIsSelectable(edict_t* pEdict);
BOOL	EntityIsMarineStruct(edict_t* pEdict);
BOOL	EntityIsAlienStruct(edict_t* pEdict);

edict_t* BotFunc_NS_MarineBuild(int iUser3, const char* szClassname, Vector vOrigin, edict_t* pEntityUser = NULL, BOOL bBuilt = FALSE);
edict_t* BotFunc_NS_CommanderBuild(int iUser3, const char* szClassname, Vector vOrigin);
BOOL UTIL_IsResourceFountainUsed(edict_t* pFountain);
//////////////////////////////
// UTIL FUNCTIONS

#ifndef RCBOT_META_BUILD
unsigned short FixedUnsigned16(float value, float scale);
short FixedSigned16(float value, float scale);
#endif
//int LookupActivity( void *pmodel, entvars_t *pev, int activity );
Vector AbsOrigin(edict_t* pEdict);
BOOL UTIL_TankUsed(edict_t* pTank);
void UTIL_BotHudMessageAll(const hudtextparms_t& textparms, const char* pMessage);
void UTIL_BotHudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage);
//unsigned short FixedUnsigned16( float value, float scale );
//short FixedSigned16( float value, float scale );
void UTIL_BotToolTip(edict_t* pEntity, eLanguage iLang, eToolTip iTooltip);

int strpos(char* pos, char* start);
void ReadMapConfig(void);
int RoundToNearestInteger(float fVal);
int Ceiling(float fVal);
BOOL UTIL_IsButton(edict_t* pButton);
BOOL UTIL_CanStand(Vector origin, Vector* v_floor);

void UTIL_GetArgFromString(char** szString, char* szArg);

void UTIL_GetArgsFromString(char* szString, char* szArg1, char* szArg2, char* szArg3, char* szArg4);

edict_t* UTIL_FindEntityInSphere(edict_t* pentStart, const Vector& vecCenter,
	float flRadius);

edict_t* UTIL_FindEntityByString(edict_t* pentStart, const char* szKeyword,
	const char* szValue);

BOOL UTIL_AcceptablePushableVector(edict_t* pPushable, Vector vOrigin);
float UTIL_EntityDistance(entvars_t* pev, Vector vOrigin);
float UTIL_EntityDistance2D(entvars_t* pev, Vector vOrigin);
float UTIL_GetBestPushableDistance(edict_t* pPushable);
Vector UTIL_GetDesiredPushableVector(Vector vOrigin, edict_t* pPushable);

edict_t* UTIL_TFC_PlayerHasFlag(edict_t* pPlayer);

void UTIL_PlaySoundToAll(const char* szSound);
void UTIL_PlaySound(edict_t* pPlayer, const char* szSound);

void BotPrintTalkMessageOne(edict_t* pClient, char* fmt, ...);

BOOL UTIL_FuncResourceIsOccupied(edict_t* pFuncResource);
int NS_GetPlayerLevel(int exp);
int UTIL_GetNumClients(BOOL bReport = FALSE);
edict_t* UTIL_FindEntityByTarget(edict_t* pentStart, const char* szName);
edict_t* UTIL_FindEntityByClassname(edict_t* pentStart, const char* szName);
edict_t* UTIL_FindEntityByTargetname(edict_t* pentStart, const char* szName);
void		UTIL_SayText(const char* pText, edict_t* pEdict);
void		UTIL_HostSay(edict_t* pEntity, int teamonly, char* message);
int		UTIL_GetTeam(edict_t* pEntity);
int	    UTIL_GetClass(edict_t* pEntity);
int		UTIL_GetBotIndex(const edict_t* pEdict);
int		UTIL_MasterTriggered(string_t sMaster, CBaseEntity* pActivator);
void		UTIL_ShowMenu(edict_t* pEdict, int slots, int displaytime, BOOL needmore, char* pText);
void		UTIL_BuildFileName(char* filename, char* arg1, char* arg2 = NULL);

edict_t* UTIL_FacingEnt(edict_t* pPlayer, BOOL any = FALSE);

void     UTIL_FixAngles(Vector* vAngles);
void     UTIL_FixFloatAngle(float* fAngle);
BOOL		UTIL_CanBuildHive(entvars_t* pev);

float UTIL_EntityAnglesToVector2D(entvars_t* pev, const Vector* pOrigin); // For 2d Movement
float UTIL_EntityAnglesToVector3D(entvars_t* pev, const Vector* pOrigin);
edict_t* UTIL_FindNearestEntity(char** szClassnames, int iNames, Vector vOrigin, float fRange, BOOL bVisible, edict_t* pIgnore = NULL);

Vector UTIL_LengthFromVector(Vector relation, float length);

BOOL	BotFunc_FInViewCone(Vector* pOrigin, edict_t* pEdict);
BOOL	BotFunc_FVisible(const Vector& vecOrigin, edict_t* pEdict);
Vector	Center(edict_t* pEdict);
Vector	GetGunPosition(edict_t* pEdict);
Vector	EntityOrigin(edict_t* pEdict);

edict_t* UTIL_CheckTeleEntrance(Vector vOrigin, edict_t* pExit, edict_t* pOwner);
edict_t* UTIL_CheckTeleExit(Vector vOrigin, edict_t* pOwner, edict_t* pEntrance);

Vector UTIL_AngleBetweenOrigin(entvars_t* pev, Vector vOrigin);
BOOL UTIL_OnGround(entvars_t* pev);
int     UTIL_TFC_getMaxArmor(edict_t* pEdict);
void	ClientPrint(edict_t* pEdict, int msg_dest, const char* msg_name);
float   UTIL_YawAngleBetweenOrigin(entvars_t* pev, Vector vOrigin);
BOOL	UTIL_IsFacingEntity(entvars_t* pev, entvars_t* pevEntity);
float   UTIL_AngleBetweenVectors(Vector vec1, Vector vec2);
float	UTIL_AnglesBetweenEdictOrigin(edict_t* pEdict, Vector origin);

int		UTIL_PlayersOnTeam(int iTeam);
int		UTIL_SpeciesOnTeam(int iSpecies, BOOL bIgnoreEmbryos = FALSE);
int		UTIL_GetNumHives(void);
int		UTIL_CountBuiltEntities(char* classname);
edict_t* UTIL_GetRandomUnbuiltHive(void);
edict_t* UTIL_FindRandomUnusedFuncResource(CBot* pBot);
edict_t* UTIL_UpdateSounds(entvars_t* pev);
edict_t* UTIL_FindPlayerByTruncName(const char* name);
void strlow(char* str);
void strhigh(char* str);
void UTIL_CountBuildingsInRange(Vector vOrigin, float fRange, int* iDefs, int* iOffs, int* iSens, int* iMovs);
float BotFunc_DistanceBetweenEdicts(edict_t* pEdict1, edict_t* pEdict2);

float UTIL_AngleDiff(float destAngle, float srcAngle);

void BotPrintTalkMessage(char* fmt, ...);

int BotFunc_GetBitSetOf(int iBits);
void BotFile_Write(char* string);

//////////////////////////////////////////////////////////////////////////////////
// END : FUNCTION PROTOTYPES
/*
typedef struct
{
   int  iId;     // weapon ID
   int  iClip;   // amount of ammo in the clip
   int  iAmmo1;  // amount of ammo in primary reserve
   int  iAmmo2;  // amount of ammo in secondary reserve
} bot_current_weapon_t;*/
/*
class CBotExp
{
public:
private:
};*/

typedef struct
{
	unsigned short	amplitude;		// FIXED 4.12 amount of shake
	unsigned short 	duration;		// FIXED 4.12 seconds duration
	unsigned short	frequency;		// FIXED 8.8 noise frequency (low frequency is a jerk,high frequency is a rumble)
} ScreenShake;

/////////////////////////////////////////////
// REPUTATION  STORAGE  CLASSES

class CBotReputation
{
public:
	CBotReputation()
	{
		m_iRep = 5;
		m_iPlayerRepId = -1;
	}

	CBotReputation(int iPlayerRepId, int iRep);

	BOOL operator == (CBotReputation Rep)
	{
		return Rep.IsForPlayer(m_iPlayerRepId);
	}

	inline void UpdateRep(int iRep)
	{
		m_iRep = iRep;
	}

	inline int CurrentRep(void)
	{
		return m_iRep;
	}

	inline int GetRepId(void)
	{
		return m_iPlayerRepId;
	}

	inline BOOL IsForPlayer(int iPlayerRepId)
	{
		return (m_iPlayerRepId == iPlayerRepId);
	}

	void printRep(CBot* forBot, edict_t* pPrintTo);

private:
	int m_iPlayerRepId;
	int m_iRep;
};

class CBotReputations
{
public:

	void printReps(CBot* forBot, edict_t* pPrintTo)
	{
		dataStack<CBotReputation> tempStack = m_RepList;
		CBotReputation* pRep;

		while (!tempStack.IsEmpty())
		{
			pRep = tempStack.ChoosePointerFromStack();

			pRep->printRep(forBot, pPrintTo);
		}
	}

	void Destroy(void)
	{
		m_RepList.Destroy();
	}

	CClient* GetRandomClient(int iRep);

	void WriteToFile(int iBotProfile, CBotReputation* pRep);

	void RemoveSaveRep(int iBotProfile, int iPlayerRepId);

	void SaveAllRep(int iBotProfile);

	void AddLoadRep(int iBotProfile, int iPlayerRepId);

	void NewRep(int iPlayerRepId)
		// Create a NEW Reputation value for player name szPlayerName
		// With a default Rep of a Mid way rep.
	{
		AddRep(iPlayerRepId, BOT_MID_REP);
	}

	int AverageRepOnServer(void)
	{
		int iTotal = 0;
		int iNum = 0;

		if (m_RepList.IsEmpty())
			return 0;

		dataStack<CBotReputation> tempStack = m_RepList;
		CBotReputation* pRep;

		while (!tempStack.IsEmpty())
		{
			pRep = tempStack.ChoosePointerFromStack();

			// bug fixed... didn't add :-P
			iTotal += pRep->CurrentRep();
			iNum++;
		}

		return (int)(iTotal / iNum);
	}

	void AddRep(int iPlayerRepId, int iRep)
		// Add a NEW Reputation WITH a reputation value
	{
		CBotReputation* l_Rep;

		l_Rep = GetRep(iPlayerRepId);

		if (l_Rep == NULL) // Don't want to add a duplicate
			m_RepList.Push(CBotReputation(iPlayerRepId, iRep));
	}

	CBotReputation* GetCreateRep(int iPlayerRepId)
	{
		dataStack<CBotReputation> tempStack = m_RepList;
		CBotReputation* l_Rep;

		while (!tempStack.IsEmpty())
		{
			l_Rep = tempStack.ChoosePointerFromStack();

			if (l_Rep->IsForPlayer(iPlayerRepId))
			{
				// Fool pointer, dont free memory
				tempStack.Init();

				return l_Rep;
			}
		}

		m_RepList.Push(CBotReputation(iPlayerRepId, BOT_MID_REP));

		return m_RepList.GetHeadInfoPointer();
	}

	CBotReputation* GetRep(int iPlayerRepId)
	{
		dataStack<CBotReputation> tempStack = m_RepList;
		CBotReputation* l_Rep;

		while (!tempStack.IsEmpty())
		{
			l_Rep = tempStack.ChoosePointerFromStack();

			if (l_Rep->IsForPlayer(iPlayerRepId))
			{
				// Fool pointer, dont free memory
				tempStack.Init();

				return l_Rep;
			}
		}

		return NULL;
	}

	int GetClientRep(CClient* pClient);

	void IncreaseRep(int iPlayerRepId)
	{
		if (iPlayerRepId == -1)
			return;

		CBotReputation* l_Rep = GetRep(iPlayerRepId);

		if (l_Rep)
		{
			int iNewRep = l_Rep->CurrentRep() + 1;

			if (iNewRep > BOT_MAX_REP)
				return;

			l_Rep->UpdateRep(iNewRep);
		}
		else
		{
			// oh crap wth?

			BugMessage(NULL, "bad rep data... (try deleting all botprofiles/*.rcr files :-( )");
		}
	}

	void DecreaseRep(int iPlayerRepId)
	{
		if (iPlayerRepId == -1)
			return;

		CBotReputation* l_Rep = GetRep(iPlayerRepId);

		if (l_Rep)
		{
			int iNewRep = l_Rep->CurrentRep() - 1;

			if (iNewRep < BOT_MIN_REP)
				return;

			l_Rep->UpdateRep(iNewRep);
		}
		else
		{
			// oh crap wth?

			BugMessage(NULL, "bad rep data... (try deleting all botprofiles/*.rcr files :-( )");
		}
	}

	void Init(void)
	{
		m_RepList.Init();
	}

private:

	dataStack<CBotReputation> m_RepList;
};
/*

// LOCATIONS : NOT REQUIRED IN NS 2.0 + :D
(Use say command and it'll tell the location)

class CLocation
{
public:
	~CLocation()
	{
		//this->Free();
	}

	CLocation()
	{
		this->Init();
	}

	void Init ( void )
	{
		m_szLocationName = NULL;
		m_pEntity = NULL;
	}

	CLocation(const char *szLocationName,edict_t *pEntity);

	void Free ( void )
	{
		if ( m_szLocationName)
		{
			free(m_szLocationName);
		}

		this->Init();
	}

	edict_t *GetEntity ( void )
	{
		return m_pEntity;
	}

	BOOL HasEntity ( edict_t *pEntity )
	{
		return m_pEntity == pEntity;
	}

	const char *GetLocationName(void)
	{
		return (const char*)m_szLocationName;
	}

	void GiveName ( const char *szLocationName );

private:
	char *m_szLocationName;
	edict_t *m_pEntity;
};
*/

//#define MAX_LOCATION_HASHES 10
/*
class CLocations
{
public:
	CLocations()
	{
		this->Init();
	}

	~CLocations()
	{
		this->Destroy();
	}

	void Init ( void )
	{
		int i;

		for ( i = 0; i < MAX_LOCATION_HASHES; i ++ )
		{
			m_Locations[i].Init();
		}
	}

	void Destroy (void)
	{
		int i;
		dataStack<CLocation> tempStack;
		CLocation *pLocation;

		for ( i = 0; i < MAX_LOCATION_HASHES; i ++ )
		{
			tempStack = m_Locations[i];

			while ( !tempStack.IsEmpty() )
			{
				pLocation = tempStack.ChoosePointerFromStack();

				pLocation->Free();
			}

			m_Locations[i].Destroy();
			m_Locations[i].Init();
		}
	}

	void AddLocation ( char *szLocationName, edict_t *pLocation )
	{
		int i = ((int)(pLocation)) % MAX_LOCATION_HASHES;

		m_Locations[i].Push(CLocation(szLocationName,pLocation));
	}

	CLocation *GetLocation ( edict_t *pentLocation )
	{
		int i;
		CLocation *pLocation;
		dataStack<CLocation> tempStack;

		i = ((int)(pentLocation)) % MAX_LOCATION_HASHES;

		tempStack = m_Locations[i];

		while ( !tempStack.IsEmpty() )
		{
			pLocation = tempStack.ChoosePointerFromStack();

			if ( pLocation->HasEntity(pentLocation) )
			{
				tempStack.Init();
				return pLocation;
			}
		}

		return NULL;
	}

private:
	dataStack<CLocation> m_Locations[MAX_LOCATION_HASHES];
};
*/

//////////////////////////////////////////////////////////////////
// BOT TASKS

// use MyEHandle for storing edicts in tasks
// incase edict changes when the task eventually gets to work.
class MyEHandle
{
private:
	int m_iSerialNumber;
	edict_t* m_pEdict;
public:
	edict_t* Get() const
	{
		try
		{
			if (m_pEdict)
			{
				if (m_iSerialNumber == m_pEdict->serialnumber)
					return m_pEdict;
			}
		}

		catch (...)
		{
			return NULL;
		}

		return NULL;
	}

	void Set(edict_t* pEdict)
	{
		try
		{
			m_pEdict = pEdict;

			if (pEdict)
			{
				m_iSerialNumber = m_pEdict->serialnumber;
			}
			else
				m_iSerialNumber = 0;
		}

		catch (...)
		{
			m_pEdict = NULL;
			m_iSerialNumber = 0;
		}
	}
};

class CBotTask
{
public:
	CBotTask()
	{
		memset(this, 0, sizeof(CBotTask));
	}

	CBotTask(eBotTask iTask, int iScheduleId = 0, edict_t* pInfo = NULL, int iInfo = 0, float fInfo = 0, 
		Vector vInfo = Vector(0, 0, 0), float fTimeToComplete = -1.0/*, CBotTask *GoalTask = NULL */)
	{
		// cheap way of adding schedules.. ;)
		// means if this task fails we can fail every other task with the same
		// schedule id
		m_iScheduleId = iScheduleId;

		m_Task = iTask;
		m_iInfo = iInfo;
		m_vInfo = vInfo;
		m_pInfo.Set(pInfo);
		m_fInfo = fInfo;
		m_bFoundPath = FALSE;

		m_iScheduleDescription = BOT_SCHED_NONE;

		if (fTimeToComplete == -1.0)
			m_fTimeToComplete = -1.0;
		else
			m_fTimeToComplete = gpGlobals->time + fTimeToComplete;
		//m_pGoalTask = GoalTask;
	}

	inline BOOL TimedOut(void)
	{
		if (m_fTimeToComplete != -1.0)
		{
			return (m_fTimeToComplete < gpGlobals->time);
		}

		return FALSE;
	}

	inline float TimeToComplete()
	{
		return (m_fTimeToComplete - gpGlobals->time);
	}

	inline void SetVector(Vector const vNewVector)
	{
		m_vInfo = vNewVector;
	}

	inline void SetFloat(float fNewFloat)
	{
		m_fInfo = fNewFloat;
	}

	inline void ChangeScheduleDesc(eScheduleDesc iSchedDesc)
	{
		m_iScheduleDescription = iSchedDesc;
	}

	inline BOOL IsOfSchedule(eScheduleDesc iSchedDesc)
	{
		return (m_iScheduleDescription == iSchedDesc);
	}

	inline void SetInt(int iNewInt)
	{
		m_iInfo = iNewInt;
	}

	inline eBotTask Task(void) const
	{
		return m_Task;
	}

	inline edict_t* TaskEdict(void) const
	{
		return m_pInfo.Get();
	}

	inline void SetEdict(edict_t* pEdict)
	{
		m_pInfo.Set(pEdict);
	}

	Vector TaskVector(void) const
	{
		return m_vInfo;
	}

	inline int TaskInt(void) const
	{
		return m_iInfo;
	}

	inline float TaskFloat(void) const
	{
		return m_fInfo;
	}

	inline BOOL HasPath(void) const
	{
		return m_bFoundPath;
	}

	inline void SetPathInfo(BOOL bPathFound)
	{
		m_bFoundPath = bPathFound;
	}

	inline void SetScheduleId(int iScheduleId)
	{
		m_iScheduleId = iScheduleId;
	}

	inline int GetScheduleId(void) const
	{
		return m_iScheduleId;
	}

	void SetTimeToComplete(float fTime)
	{
		//////// BIGGG Problem fixed (must add gpGlobals time, duuhhhh!!)
		m_fTimeToComplete = gpGlobals->time + fTime;
	}

	BOOL operator == (const CBotTask& Task);

	char* getTaskDescription()
	{
		return CBotTask::_getTaskDescription(m_Task);
	}

	static char* _getTaskDescription(eBotTask itask)
	{
		switch (itask)
		{
		case BOT_TASK_NONE:
			return "BOT_TASK_NONE";
		case BOT_TASK_USE_TANK:				// currently using a turret
			return "BOT_TASK_USE_TANK";
		case BOT_TASK_WAIT_FOR_LIFT:
			return "BOT_TASK_WAIT_FOR_LIFT";
		case BOT_TASK_RELOAD:
			return "BOT_TASK_RELOAD";
		case BOT_TASK_LISTEN_TO_SOUND:
			return "BOT_TASK_LISTEN_TO_SOUND";
		case BOT_TASK_FIND_ENEMY_PATH:
			return "BOT_TASK_FIND_ENEMY_PATH";
		case BOT_TASK_FOLLOW_ENEMY:
			return "BOT_TASK_FOLLOW_ENEMY";
		case BOT_TASK_FOLLOW_LEADER:
			return "BOT_TASK_FOLLOW_LEADER";
		case BOT_TASK_FIND_WEAPON:
			return "BOT_TASK_FIND_WEAPON";
		case BOT_TASK_FIND_PATH:
			return "BOT_TASK_FIND_PATH";
		case BOT_TASK_RUN_PATH:
			return "BOT_TASK_RUN_PATH";
		case BOT_TASK_PICKUP_ITEM:
			return "BOT_TASK_PICKUP_ITEM";
		case BOT_TASK_WAIT_FOR_RESOURCES:
			return "BOT_TASK_WAIT_FOR_RESOURCES";
		case BOT_TASK_GOTO_FLANK_POSITION:
			return "BOT_TASK_GOTO_FLANK_POSITION";
		case BOT_TASK_RANGE_ATTACK:
			return "BOT_TASK_RANGE_ATTACK";
		case BOT_TASK_NORMAL_ATTACK:
			return "BOT_TASK_NORMAL_ATTACK";
		case BOT_TASK_HIDE:
			return "BOT_TASK_HIDE";
		case BOT_TASK_ASSEMBLE_SQUAD:
			return "BOT_TASK_ASSEMBLE_SQUAD";
		case BOT_TASK_WAIT_FOR_ORDERS:
			return "BOT_TASK_WAIT_FOR_ORDERS";
		case BOT_TASK_SOLO_RUN:
			return "BOT_TASK_SOLO_RUN";
		case BOT_TASK_ATTACK_ENEMY:
			return "BOT_TASK_ATTACK_ENEMY";
		case BOT_TASK_CHANGE_WEAPON:
			return "BOT_TASK_CHANGE_WEAPON";
		case BOT_TASK_MOVE_TO_VECTOR:
			return "BOT_TASK_MOVE_TO_VECTOR";
		case BOT_TASK_WELD_OBJECT:
			return "BOT_TASK_WELD_OBJECT";
		case BOT_TASK_GOTO_OBJECT:
			return "BOT_TASK_GOTO_OBJECT";
		case BOT_TASK_HEAL_PLAYER:
			return "BOT_TASK_HEAL_PLAYER";
		case BOT_TASK_ALIEN_UPGRADE:
			return "BOT_TASK_ALIEN_UPGRADE";
		case BOT_TASK_BUILD_ALIEN_STRUCTURE:
			return "BOT_TASK_BUILD_ALIEN_STRUCTURE";
		case BOT_TASK_FACE_VECTOR:
			return "BOT_TASK_FACE_VECTOR";
		case BOT_TASK_FACE_EDICT:
			return "BOT_TASK_FACE_EDICT";
		case BOT_TASK_WAIT_AND_FACE_VECTOR:
			return "BOT_TASK_WAIT_AND_FACE_VECTOR";
		case BOT_TASK_AVOID_OBJECT:
			return "BOT_TASK_AVOID_OBJECT";
		case BOT_TASK_BUILD:				// Might be same as USE: but requires the bot to wait until it is built.
			return "BOT_TASK_BUILD";
		case BOT_TASK_USE:				// USE object
			return "BOT_TASK_USE";
		case BOT_TASK_DEFEND:			// DEFEND object
			return "BOT_TASK_DEFEND";
		case BOT_TASK_WAIT:				// WAIT at object (for team mates for example)
			return "BOT_TASK_WAIT";
		case BOT_TASK_DEPLOY_MINES:
			return "BOT_TASK_DEPLOY_MINES";
		case BOT_TASK_USE_AMMO_DISP:
			return "BOT_TASK_USE_AMMO_DISP";
		case BOT_TASK_WALK_PATH:
			return "BOT_TASK_WALK_PATH";
		case BOT_TASK_TYPE_MESSAGE:
			return "BOT_TASK_TYPE_MESSAGE";
		case BOT_TASK_WAIT_FOR_ENTITY:
			return "BOT_TASK_WAIT_FOR_ENTITY";
		case BOT_TASK_USE_DOOR_BUTTON:
			return "BOT_TASK_USE_DOOR_BUTTON";
		case BOT_TASK_SEARCH_FOR_ENEMY:
			return "BOT_TASK_SEARCH_FOR_ENEMY";
		case BOT_TASK_ALIEN_EVOLVE:
			return "BOT_TASK_ALIEN_EVOLVE";
		case BOT_TASK_USE_HEV_CHARGER:
			return "BOT_TASK_USE_HEV_CHARGER";
		case BOT_TASK_USE_HEALTH_CHARGER:
			return "BOT_TASK_USE_HEALTH_CHARGER";
		case BOT_TASK_HUMAN_TOWER:
			return "BOT_TASK_HUMAN_TOWER";
		case BOT_TASK_THROW_GRENADE:
			return "BOT_TASK_THROW_GRENADE";
		case BOT_TASK_PUSH_PUSHABLE:
			return "BOT_TASK_PUSH_PUSHABLE";
		case BOT_TASK_SECONDARY_ATTACK:
			return "BOT_TASK_SECONDARY_ATTACK";
		case BOT_COMMAND_TASK_MOVE_TO_VECTOR:
			return "BOT_COMMAND_TASK_MOVE_TO_VECTOR";
		case BOT_COMMAND_TASK_SELECT_PLAYERS:
			return "BOT_COMMAND_TASK_SELECT_PLAYERS";
		case BOT_COMMAND_TASK_BUILD_OBJECT:
			return "BOT_COMMAND_TASK_BUILD_OBJECT";
		case BOT_COMMAND_TASK_ISSUE_ORDER:
			return "BOT_COMMAND_TASK_ISSUE_ORDER";
		case BOT_COMMAND_TASK_SELECT_POINT:
			return "BOT_COMMAND_TASK_SELECT_POINT";
		case BOT_COMMAND_TASK_SELECT_AREA:
			return "BOT_COMMAND_TASK_SELECT_AREA";
		case BOT_COMMAND_TASK_ISSUE_COMMAND:
			return "BOT_COMMAND_TASK_ISSUE_COMMAND";
		case BOT_TASK_TFC_BUILD_SENTRY:
			return "BOT_TASK_TFC_BUILD_SENTRY";
		case BOT_TASK_TFC_REPAIR_BUILDABLE:
			return "BOT_TASK_TFC_REPAIR_BUILDABLE";
		case BOT_TASK_TFC_BUILD_TELEPORT_ENTRANCE:
			return "BOT_TASK_TFC_BUILD_TELEPORT_ENTRANCE";
		case BOT_TASK_TFC_BUILD_TELEPORT_EXIT:
			return "BOT_TASK_TFC_BUILD_TELEPORT_EXIT";
		case BOT_TASK_TFC_BUILD_DISPENSER:
			return "BOT_TASK_TFC_BUILD_DISPENSER";
		case BOT_TASK_TFC_DETONATE_DISPENSER:
			return "BOT_TASK_TFC_DETONATE_DISPENSER";
		case BOT_TASK_TFC_FEIGN_DEATH:
			return "BOT_TASK_TFC_FEIGN_DEATH";
		case BOT_TASK_TFC_SNIPE:
			return "BOT_TASK_TFC_SNIPE";
		case BOT_TASK_TFC_PLACE_DETPACK:
			return "BOT_TASK_TFC_PLACE_DETPACK";
		case BOT_TASK_TFC_CONC_JUMP:
			return "BOT_TASK_TFC_CONC_JUMP";
		case BOT_TASK_TFC_ROCKET_JUMP:
			return "BOT_TASK_TFC_ROCKET_JUMP";
		case BOT_TASK_TFC_PLACE_PIPES:
			return "BOT_TASK_TFC_PLACE_PIPES";
		case BOT_TASK_TFC_DETONATE_PIPES:
			return "BOT_TASK_TFC_DETONATE_PIPES";
		case BOT_TASK_TFC_DISGUISE:
			return "BOT_TASK_TFC_DISGUISE";
		case BOT_TASK_TFC_DETONATE_ENTRY_TELEPORT:
			return "BOT_TASK_TFC_DETONATE_ENTRY_TELEPORT";
		case BOT_TASK_TFC_DETONATE_EXIT_TELEPORT:
			return "BOT_TASK_TFC_DETONATE_EXIT_TELEPORT";
		case BOT_TASK_TFC_ROTATE_SENTRY:
			return "BOT_TASK_TFC_ROTATE_SENTRY";
		case BOT_TASK_FIND_COVER_POS:
			return "BOT_TASK_FIND_COVER_POS";
		case BOT_TASK_USE_TELEPORT:
			return "BOT_TASK_USE_TELEPORT";
		case BOT_TASK_COMBAT_UPGRADE:
			return "BOT_TASK_COMBAT_UPGRADE";
		case BOT_TASK_CROUCH:
			return "BOT_TASK_CROUCH";
		case BOT_TASK_ACCEPT_HEALTH:
			return "BOT_TASK_ACCEPT_HEALTH";
		case BOT_TASK_BLINK:
			return "BOT_TASK_BLINK";
		case BOT_TASK_WEB:
			return "BOT_TASK_WEB";
		case BOT_TASK_WAIT_FOR_FLAG:
			return "BOT_TASK_WAIT_FOR_FLAG";
		case BOT_TASK_USE_TELEPORTER:
			return "BOT_TASK_USE_TELEPORTER";
		}

		return "Unknown";
	}

	inline eScheduleDesc GetScheduleDesc()
	{
		return m_iScheduleDescription;
	}

	char* getScheduleDescription()
	{
		switch (m_iScheduleDescription)
		{
		case BOT_SCHED_NONE:
			return "BOT_SCHED_NONE";
		case BOT_SCHED_USE_LIFT:
			return "BOT_SCHED_USE_LIFT";
		case BOT_SCHED_WELD:
			return "BOT_SCHED_WELD";
		case BOT_SCHED_BUILD:
			return "BOT_SCHED_BUILD";
		case BOT_SCHED_NS_CHECK_STRUCTURE:
			return "BOT_SCHED_NS_CHECK_STRUCTURE";
		case BOT_SCHED_NS_CHECK_HIVE:
			return "BOT_SCHED_NS_CHECK_HIVE";
		case BOT_SCHED_RUN_FOR_COVER:
			return "BOT_SCHED_RUN_FOR_COVER";
		case BOT_SCHED_LOOK_FOR_WEAPON:
			return "BOT_SCHED_LOOK_FOR_WEAPON";
		case BOT_SCHED_USE_TANK:
			return "BOT_SCHED_USE_TANK";
		case BOT_SCHED_FOLLOW_LEADER:
			return "BOT_SCHED_FOLLOW_LEADER";
		case BOT_SCHED_USING_SCIENTIST:
			return "BOT_SCHED_USING_SCIENTIST";
		case BOT_SCHED_USING_BARNEY:
			return "BOT_SCHED_USING_BARNEY";
		case BOT_SCHED_PICKUP_FLAG:
			return "BOT_SCHED_PICKUP_FLAG";
		}

		return "Unknown";
	}

private:
	// Current Task ID
	eBotTask	m_Task;

	//////////////////////
	// task variables
	//
	int			m_iInfo;
	float		m_fInfo;
	Vector		m_vInfo;
	MyEHandle	m_pInfo;
	//
	//////////////////////

	/////////////////////////////////////
	// tasks schedule info
	int			m_iScheduleId;
	eScheduleDesc m_iScheduleDescription;
	//
	/////////////////////////////////////

	/////////////////////////////////////
	// Path to task has been determined?
	BOOL        m_bFoundPath;
	//
	/////////////////////////////////////

	//////////////////////////////
	// Incase task takes too long
	float       m_fTimeToComplete;
};

class CBotTasks
{
public:

	void SetTimeToCompleteSchedule(int iScheduleId, float fTime)
	{
		dataQueue<CBotTask> tempStack;
		CBotTask* tempTask;

		tempStack = m_Tasks;

		fTime += gpGlobals->time;

		while (!tempStack.IsEmpty())
		{
			tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->GetScheduleId() == iScheduleId)
			{
				tempTask->SetTimeToComplete(fTime);
			}
		}
	}

	// add a schedule (a list of tasks) of certain schedule type
	// this was added quite recently so ins't used a lot, even though it's quite neat.
	void AddNewSchedule(eScheduleDesc iScheduleDescription, CBotTask* pTasks, int iNumTasks)
	{
		int iNewScheduleId = GetNewScheduleId();

		int i;

		for (i = (iNumTasks - 1); i >= 0; i--)
		{
			pTasks[i].SetScheduleId(iNewScheduleId);
			pTasks[i].ChangeScheduleDesc(iScheduleDescription);

			AddTaskToFront(pTasks[i]);
		}
	}

	void RemoveTimedOutSchedules(void)
	{
		CBotTask* pCurrentTask = this->CurrentTask();
		int iSchedIgnore;
		int iFailSchedule;

		dataQueue<CBotTask> tempStack;
		CBotTask* tempTask;

		if (pCurrentTask == NULL)
			return;

		iSchedIgnore = pCurrentTask->GetScheduleId();

		tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			tempTask = tempStack.ChoosePointerFrom();

			iFailSchedule = tempTask->GetScheduleId();

			if (tempTask->TimedOut())
			{
				if (iFailSchedule != iSchedIgnore)
				{
					// returns true if schedule was definitly removed.
					if (FinishSchedule(tempTask->GetScheduleId()))
					{
						// structure changed, update
						tempStack = m_Tasks;
					}
				}
			}
		}
	}

	void GiveSchedIdDescription(int iSchedId, eScheduleDesc iSchedDesc)
	{
		dataQueue<CBotTask> tempStack;
		CBotTask* tempTask;

		tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->GetScheduleId() == iSchedId)
			{
				tempTask->ChangeScheduleDesc(iSchedDesc);
			}
		}
	}

	//
	// Check if some tasks are part of the schedule input
	// into this function.

	// This function is useful to make sure we dont add lots
	// of the same tasks.
	BOOL HasSchedule(eScheduleDesc iSchedDesc)
	{
		dataQueue<CBotTask> tempStack;
		CBotTask* tempTask;

		tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->IsOfSchedule(iSchedDesc))
			{
				tempStack.Init();
				return TRUE;
			}
		}

		return FALSE;
	}

	/*	void RemoveSchedule ( eScheduleDesc iSched )
		{
			dataQueue<CBotTask> tempStack = m_Tasks;

			CBotTask *pTask;

			int iScheduleId = 0;

			while ( !tempStack.IsEmpty() )
			{
				pTask = tempStack.ChoosePointerFrom();

				if ( pTask->IsOfSchedule(iSched) )
				{
					tempStack.Init();

					iScheduleId = pTask->GetScheduleId();

					m_Tasks.RemoveByPointer(pTask);

					if ( iScheduleId )
					{
						FinishSchedule(iScheduleId);
					}

					// structure changed, restart
					tempStack = m_Tasks;
				}
			}
		}
	*/
	void RemoveSameTask(const CBotTask& Task)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		CBotTask* pTask;

		int iScheduleId = 0;

		while (!tempStack.IsEmpty())
		{
			try
			{
				pTask = tempStack.ChoosePointerFrom();

				if (*pTask == Task)
				{
					tempStack.Init();

					iScheduleId = pTask->GetScheduleId();

					m_Tasks.RemoveByPointer(pTask);

					if (iScheduleId)
					{
						FinishSchedule(iScheduleId);
					}

					// structure changed, restart
					tempStack = m_Tasks;
				}
			}
			catch (...)
			{
				tempStack.Init();
			}
		}
	}

	void RemoveSimilarTask(const eBotTask iTask)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		CBotTask* pTask;

		int iScheduleId = 0;

		while (!tempStack.IsEmpty())
		{
			pTask = tempStack.ChoosePointerFrom();

			if (pTask->Task() == iTask)
			{
				tempStack.Init();

				iScheduleId = pTask->GetScheduleId();

				m_Tasks.RemoveByPointer(pTask);

				if (iScheduleId)
				{
					FinishSchedule(iScheduleId);
				}

				// structure changed, restart
				tempStack = m_Tasks;
			}
		}
	}
	/*void RemoveTask ( const eBotTask iTask )
	{
		tempStack =
	}*/

	void FlushTasks(void)
	{
		m_Tasks.Destroy();
	}

	BOOL HasTask(eBotTask iTask)
	{
		dataQueue<CBotTask> tempStack;
		CBotTask* tempTask;

		tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->Task() == iTask)
			{
				tempStack.Init();

				return TRUE;
			}
		}

		return FALSE;
	}

	// get a actual task  from a task type
	void GetTask(eBotTask iTask, CBotTask* GotTask)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		CBotTask* pTask;

		while (!tempStack.IsEmpty())
		{
			pTask = tempStack.ChoosePointerFrom();

			if (pTask->Task() == iTask)
			{
				*GotTask = *pTask;
				tempStack.Init();
				return;
			}
		}

		*GotTask = CBotTask(BOT_TASK_NONE);
	}

	BOOL HasTask(CBotTask const Task)
	{
		return m_Tasks.IsMember(Task);//changed
	}

	CBotTask* CurrentTask(void)
	{
		return m_Tasks.GetFrontPointer();
	}

	void AddTaskToEnd(const CBotTask& Task)
	{
		// Remove any task that is the same first
		//RemoveSameTask(Task);

		m_Tasks.Add(Task);
	}

	void AddTaskToFront(const CBotTask& Task)
	{
		// Problems occur while using a Task queue/stack...
		// There may be one already the same in the queue,
		// Remove any task that is the same first, so it's effectively moved
		RemoveSameTask(Task);

		m_Tasks.AddFront(Task);
	}

	int GetNewScheduleId(void)
		// Return an unused Schedule Id...
	{
		int iScheduleId = 1;

		dataQueue<CBotTask> tempStack = m_Tasks;
		CBotTask* pTask;

		// search through list to find a unique Id.
		while (!tempStack.IsEmpty())
		{
			pTask = tempStack.ChoosePointerFrom();

			if (iScheduleId == pTask->GetScheduleId())
			{
				tempStack = m_Tasks;
				iScheduleId++;
			}
		}

		return iScheduleId;
	}

	//
	// Removes all tasks from a schedule id number
	BOOL FinishSchedule(int iScheduleId)
		// Finish all tasks with ScheduleId as its schedule
	{
		dataQueue<CBotTask> tempStack = m_Tasks;
		CBotTask* pTask;
		BOOL bRemoved = FALSE;

		while (!tempStack.IsEmpty())
		{
			pTask = tempStack.ChoosePointerFrom();

			if (pTask->GetScheduleId() == iScheduleId)
			{
				m_Tasks.RemoveByPointer(pTask);
				bRemoved = TRUE;
				tempStack = m_Tasks;
			}
		}

		return bRemoved;
	}

	//
	// Removes all tasks from a schedule description
	BOOL FinishSchedule(eScheduleDesc iSchedDesc)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;
		CBotTask* pTask;
		BOOL bRemoved = FALSE;

		while (!tempStack.IsEmpty())
		{
			pTask = tempStack.ChoosePointerFrom();

			if (pTask->IsOfSchedule(iSchedDesc))
			{
				m_Tasks.RemoveByPointer(pTask);
				bRemoved = TRUE;
				tempStack = m_Tasks;
			}
		}

		return bRemoved;
	}

	void FinishedCurrentTask(BOOL bFailed = FALSE)
	{
		CBotTask* pTask = CurrentTask();

		if (pTask == NULL)
			return;

		eBotTask iTask = pTask->Task();
		BOOL bPathInfo = pTask->HasPath();
		int iScheduleId = pTask->GetScheduleId();

		// Current task will be at the top of
		// the queue, so remove the first node.
		m_Tasks.RemoveFront();

		if (bFailed && iScheduleId)
		{
			FinishSchedule(iScheduleId);
		}

		if (iTask == BOT_TASK_FIND_PATH)
		{
			if (!NoTasksLeft())
			{
				this->RemovePaths();

				pTask = CurrentTask();

				if (pTask)
					pTask->SetPathInfo(bPathInfo);
			}
		}
	}

	BOOL NoTasksLeft(void)
	{
		return m_Tasks.IsEmpty();
	}

	void RemovePaths(void)
	{
		// Altered bot's path info

		dataQueue<CBotTask> tempStack = m_Tasks;
		CBotTask* pPrevTask;

		// will increment head, we've already set that to bPathFound
		if (tempStack.IsEmpty())
			return;

		if (tempStack.ChoosePointerFrom())
		{
			while (!tempStack.IsEmpty())
			{
				pPrevTask = tempStack.ChoosePointerFrom();

				if (pPrevTask)
				{
					// Changed path info to false for these tasks
					pPrevTask->SetPathInfo(FALSE);
				}
			}
		}
	}

private:
	dataQueue<CBotTask> m_Tasks;
};

// define the structure for waypoint paths (paths are connections between
// two waypoint nodes that indicates the bot can get from point A to point B.
// note that paths DON'T have to be two-way.  sometimes they are just one-way
// connections between two points.  There is an array called "paths" that
// contains head pointers to these structures for each waypoint index.
typedef struct path {
	short int index[MAX_PATH_INDEX];  // indexes of waypoints (index -1 means not used)
	struct path* next;   // link to next structure
} PATH;

////////////////////////////
// Bot Profile Info

typedef struct bot_profile_s
{
	int m_iProfileId;

	int m_iFavMod;
	int m_iFavTeam;
	int m_iSkill;

	char* m_szFavMap;
	char* m_szSpray;
	char* m_szBotName;

	//////////////////////
	// NS STUFF

	int m_GorgePercent;
	int m_LerkPercent;
	int m_FadePercent;
	int m_OnosPercent;

	int m_iNumGames;

	int m_iBottomColour;
	int m_iTopColour;

	float m_fAimSpeed;
	float m_fAimSkill;
	float m_fAimTime;

	/////////////////////////////////////
	// now made Individual for each bot
	int m_iPathRevs;
	int m_iVisionRevs;
	float m_fVisionTime;
	/////////////////////////////////////

	char* m_szHAL_PreTrainFile;
	char* m_szHAL_AuxFile;
	char* m_szHAL_BanFile;
	char* m_szHAL_SwapFile;

	HAL_bot_t* m_HAL;

	CBotReputations m_Rep;

	int m_iClass;

	char* m_szModel;
}bot_profile_t;

class CBotFailedPath
{
public:

	CBotFailedPath()
	{
		// nothing added
		m_pPath = NULL;
		// in case this is added remove it later
		// by showing that its violated
		m_bViolated = TRUE;
	}

	CBotFailedPath(PATH* pPath)
	{
		m_pPath = pPath;
		m_bViolated = FALSE;
	}

	inline BOOL IsForPath(PATH* pPath) const
	{
		return (m_pPath == pPath);
	}

	inline void Violate(void)
	{
		m_bViolated = TRUE;
	}

	inline BOOL IsViolated(void) const
	{
		return m_bViolated;
	}

private:
	PATH* m_pPath; // the actual path
	BOOL m_bViolated; // the path is going to be removed if true
};

class CBotFailedPaths
{
public:

	BOOL Violate(PATH* pPath)
		// violate path, make it unavailable.
	{
		dataStack<CBotFailedPath> tempStack = m_FailedPaths;
		CBotFailedPath* pFailedPath;

		while (!tempStack.IsEmpty())
		{
			pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsForPath(pPath))
			{
				pFailedPath->Violate();

				// MUST reset temporary stack to a NULL Head!!!
				tempStack.Init();

				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL ClearViolatedPaths(void)
		// Clear paths that have been checked and dealt with in path finding
	{
		dataStack<CBotFailedPath> tempStack = m_FailedPaths;
		CBotFailedPath* pFailedPath;

		BOOL bRemoved = FALSE;

		while (!tempStack.IsEmpty())
		{
			pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsViolated())
			{
				m_FailedPaths.RemoveByPointer(pFailedPath);

				bRemoved = TRUE;
				// keep checking for more

				// structure changed
				tempStack = m_FailedPaths;
			}
		}

		return bRemoved;
	}

	// adds a new path which can't be traversed
	void AddFailedPath(PATH* pPath)
	{
		dataStack<CBotFailedPath> tempStack = m_FailedPaths;
		CBotFailedPath* pFailedPath;

		while (!tempStack.IsEmpty())
		{
			pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsForPath(pPath))
			{
				tempStack.Init();
				// can't add , already in
				return;
			}
		}

		m_FailedPaths.Push(pPath);
	}

	void ClearFailedPaths(void)
	{
		m_FailedPaths.Destroy();
	}

private:
	dataStack<CBotFailedPath> m_FailedPaths;
};

// Store a vector as short integers and return as
// normal floats for less space usage.
template <class T>
class CTypeVector
{
public:

	CTypeVector()
	{
		memset(this, 0, sizeof(CTypeVector<T>));
	}

	void SetVector(Vector vVec)
	{
		m_x = (T)vVec.x;
		m_y = (T)vVec.y;
		m_z = (T)vVec.z;

		m_bVectorSet = TRUE;
	}

	inline Vector GetVector(void) const
	{
		return Vector((float)m_x, (float)m_y, (float)m_z);
	}

	inline BOOL IsVectorSet()
	{
		return m_bVectorSet;
	}

	inline void UnSet()
	{
		m_bVectorSet = FALSE;
	}
protected:
	T m_x, m_y, m_z;

	BOOL m_bVectorSet;
};

class CAutoWaypointCheck : public CTypeVector<float>
{
public:
	void SetPoint(Vector const vec, int iFlags)
	{
		m_iFlags = iFlags;

		SetVector(vec);
	}

	int getFlags()
	{
		return m_iFlags;
	}

	inline void UnSetPoint()
	{
		m_bVectorSet = FALSE;
		m_iFlags = 0;
	}
private:
	int m_iFlags;
};

#define BOT_REMEMBER_POSITION			0
#define BOT_REMEMBER_SENTRY			(1<<0)
#define BOT_REMEMBER_LOST_ENEMY		(1<<1)
#define BOT_REMEMBER_SOUND			(1<<2)
#define BOT_REMEMBER_1ST_SEE_ENEMY	(1<<3)

class CRememberPosition
{
public:

	CRememberPosition()
	{
		memset(this, 0, sizeof(CRememberPosition));
	}

	CRememberPosition(Vector vOrigin, edict_t* pEntity)
	{
		m_vOrigin = vOrigin;
		setEntity(pEntity);
	}

	void setFlags(int flags)
	{
		m_iFlags = flags;
	}

	BOOL hasFlags(int flags)
	{
		return ((m_iFlags & flags) > 0);
	}

	inline void setEntity(edict_t* pEntity)
	{
		m_pEntity.Set(pEntity);
	}

	inline edict_t* getEntity(void)
	{
		return m_pEntity.Get();
	}

	inline BOOL isEntity(edict_t* pEntity)
	{
		if (pEntity == NULL)
			return FALSE;

		return (pEntity == getEntity());
	}

	inline Vector getVector(void)
	{
		return m_vOrigin;
	}

	inline void setVisibleOrigin(Vector const vVis)
	{
		m_vVisibleOrigin = vVis;
	}

	inline Vector getVisibleOrigin()
	{
		return m_vVisibleOrigin;
	}

	inline BOOL operator == (CRememberPosition other)
	{
		return (other.getEntity() == getEntity());
	}

private:
	Vector m_vOrigin;
	Vector m_vVisibleOrigin;
	MyEHandle m_pEntity;
	int m_iFlags;
};

class CRememberedPositions
{
public:

	CRememberedPositions()
	{
		Init();
	}

	void Init(void)
	{
		memset(this, 0, sizeof(CRememberedPositions));

		//		m_iOldest = -1;
			//	m_iNewest = -1;
	}

	/*	void addPosition ( Vector vOrigin, edict_t *pEntity )
		{
			if ( m_iOldestPosition >= m_iPositions )
				m_iOldestPosition = 0;

			m_Positions[m_iOldestPosition] = new CRememberPosition(vOrigin,pEntity);

			m_iOldestPosition ++;

			if ( m_iPositions < MAX_REMEMBER_POSITIONS )
				m_iPositions++;
		}

		void fallDown ( int iLocation )
		{
			int i;

			i = iLocation;

			while ( i < (MAX_REMEMBER_POSITIONS-1) )
				m_Positions[i] = m_Positions[i++];

			if ( (m_iOldestPosition != 0) && (m_iOldestPosition > i) )
				m_iOldestPosition = i - 1;
		}

		BOOL gotPosition ()
		{
			return (m_iPositions > 0);
		}

		Vector getNewest ()
		{
			return m_Positions[getNewestLocation()].getVector();
		}

		Vector getPosition ()
		{
			return m_Positions[0].getVector();
		}

		void removeNewest ()
		{
			fallDown(getNewestLocation());
		}

		int getNewestLocation ()
		{
			int iNew = (m_iOldestPosition + 1);

			if ( iNew > m_iPositions )
				iNew = 0;

			return iNew;
		}

		void removePosition ( edict_t *pEntity )
		{
			int i;

			i = 0;

			while ( i < m_iPositions )
			{
				if ( m_Positions[i].isEntity(pEntity) )
				{
					fallDown(i);

					// restart search
					i = 0;

					continue;
				}

				i++;
			}
		}*/

	void addPosition(Vector const vOrigin, edict_t* pEntity, int flags, Vector vVisibleOrigin)
	{
		CRememberPosition newPosition = CRememberPosition(vOrigin, pEntity);
		newPosition.setFlags(flags);
		newPosition.setVisibleOrigin(vVisibleOrigin);

		CRememberPosition* e = m_Positions.getExisting(newPosition);

		if (e != NULL)
			m_Positions.Remove(*e);
		else if (m_Positions.Size() > MAX_REMEMBER_POSITIONS)
			m_Positions.Remove(m_Positions[0]);

		m_Positions.Add(newPosition);

		/*
		int iExist = -1;

		// check if entity already remembered
		for ( int i = 0; i < m_iPositions; i ++ )
		{
			if ( pEntity && (m_Positions[i].getEntity()) && (m_Positions[i].getEntity() == pEntity) )
			{
				iExist = i;
				break;
			}
		}

		CRememberPosition newPosition = CRememberPosition(vOrigin,pEntity);

		newPosition.setFlags(flags);
		newPosition.setVisibleOrigin(vVisibleOrigin);

		if ( iExist != -1 )
		{
			// overwrite existing entity position
			m_Positions[iExist] = newPosition;
			m_iNewest = iExist;
		}
		else if ( m_iPositions == MAX_REMEMBER_POSITIONS )
		{
			// need to over-write a current position
			m_Positions[m_iOldest] = newPosition;
			m_iNewest = m_iOldest;
			// newly overwritten
			m_iCurrent = m_iOldest;

			// next newest
			m_iOldest++;

			// wrap around 0 to MAX_REMEMBER_POSITIONS
			if ( m_iOldest == MAX_REMEMBER_POSITIONS )
				m_iOldest = 0;
		}
		else
		{
			// over-write a non used position
			if ( m_iOldest == -1 )
				m_iOldest = m_iPositions;

			m_iCurrent = m_iPositions;

			m_Positions[m_iPositions] = newPosition;
			m_iNewest = m_iPositions;
			m_iCurrent = m_iPositions;

			m_iPositions ++;
		}*/
	}

	CRememberPosition* getLatestWFlags(int flags)
	{
		for (int i = 0; i < m_Positions.Size(); i++)
		{
			if (m_Positions[i].hasFlags(flags))
				return &(m_Positions[i]);
		}

		return NULL;
	}

	void removePosition(edict_t* pEntity)
	{
		int i;
		//		int j;

		for (i = 0; i < m_Positions.Size(); i++)
		{
			if (m_Positions[i].isEntity(pEntity))//|| (m_Positions[i].getVector() == vOrigin))
			{
				m_Positions.Remove(m_Positions[i]);
				/*
				for ( j = i; j < (MAX_REMEMBER_POSITIONS-1); j ++ )
				{
					m_Positions[j] = m_Positions[j+1];
				}

				m_iPositions--;

				if ( m_iNewest == i )
				{
					if ( i > 0 )
						m_iNewest = (i-1);
					else
						m_iNewest = 0;
				}
*/
				return;
			}
		}
	}

	CRememberPosition* getSentryPosition()
	{
		for (int i = 0; i < m_Positions.Size(); i++)
		{
			if (m_Positions[i].hasFlags(BOT_REMEMBER_SENTRY))
				return &(m_Positions[i]);
		}

		return NULL;
	}

	BOOL gotPosition()
	{
		return (!m_Positions.IsEmpty());
	}

	CRememberPosition* getNewest()
	{
		return &(m_Positions[m_Positions.Size() - 1]);//[m_iNewest]);//.getVector();
	}

	CRememberPosition* positionNearest(Vector vOrigin, Vector vFrom)
	{
		CRememberPosition* nearest = NULL;
		float fNearest = 0;
		float fDistance = 0;

		for (int i = 0; i < m_Positions.Size(); i++)
		{
			fDistance = ((m_Positions[i].getVector() - vOrigin).Length()) + ((m_Positions[i].getVector() - vFrom).Length());

			if (!nearest || (fDistance < fNearest))
			{
				nearest = &(m_Positions[i]);
				fNearest = fDistance;
			}
		}

		return nearest;
	}

private:
	dataUnconstArray<CRememberPosition> m_Positions;//[MAX_REMEMBER_POSITIONS];
	/*
	int m_iCurrent;
	int m_iNewest;
	int m_iOldest;
	int m_iPositions;*/
};

/*

#define BOT_RES_FLAG_VISITED (1<<0)
#define BOT_RES_FLAG_HOSTILE (1<<1)
#define BOT_RES_FLAG_NOGOAL  (1<<2)

class CVisitedFuncResource
{
public:

	CVisitedFuncResource ( edict_t *pEdict )
	{
		m_pFuncResource = pEdict;
		m_iFlags = 0;
	}

	edict_t *GetFuncResource ( void ) const
	{
		return m_pFuncResource;
	}

	BOOL HasVisited ( void )
	{
		return ((m_iFlags & BOT_RES_FLAG_VISITED)==BOT_RES_FLAG_VISITED);
	}

	BOOL IsHostile ( void )
	{
		return ((m_iFlags & BOT_RES_FLAG_HOSTILE)==BOT_RES_FLAG_HOSTILE);
	}

	BOOL HasFailedGoal ( void )
	{
		return ((m_iFlags & BOT_RES_FLAG_NOGOAL)==BOT_RES_FLAG_NOGOAL);
	}
private:
	edict_t *m_pFuncResource;
	unsigned char m_iFlags;
	float m_fVisitTime;
};

class CVisitedFuncResources
{
public:

	CVisitedFuncResources ( )
	{
		m_FuncResources.Init();
	}

	edict_t *GetBestResource

private:
	dataUnconstArray<CVisitedFuncResource> m_FuncResources;
};*/
/////////////////////////////////////////////////////
// BOT CLASS

#define SQUAD_DEFAULT_SPREAD 50.0// say 50 units between each member...?

typedef enum eSquadForm
{
	SQUAD_FORM_NONE = 0,
	SQUAD_FORM_WEDGE,
	SQUAD_FORM_LINE,
	SQUAD_FORM_ECH_LEFT,
	SQUAD_FORM_ECH_RIGHT,
	SQUAD_FORM_COLUMN,
	SQUAD_FORM_VEE
};

typedef enum eCombatType
{
	COMBAT_NONE = 0,
	COMBAT_STEALTH,
	COMBAT_COMBAT,
	COMBAT_CROUCH,
	COMBAT_PRONE
};

class CBotSquad
{
public:

	CBotSquad(edict_t* pLeader, edict_t* pMember)
	{
		SetLeader(pLeader);
		AddMember(pMember);

		Init();
	}

	~CBotSquad()
	{
		m_theSquad.Destroy();
	}

	void Init()
	{
		edict_t* pLeader;

		m_theDesiredFormation = SQUAD_FORM_WEDGE; // default wedge formation
		m_fDesiredSpread = SQUAD_DEFAULT_SPREAD;

		m_CombatType = COMBAT_COMBAT;

		bCanFire = TRUE;

		if ((pLeader = GetLeader()) != NULL)
			m_vLeaderAngle = pLeader->v.angles;
	}

	void ReturnAllToFormation(void);

	inline void SetLeader(edict_t* pLeader)
	{
		m_pLeader.Set(pLeader);
	}

	void ToggleFireMode(void)
	{
		bCanFire = !bCanFire;

		if (bCanFire)
			BotPrintTalkMessageOne(m_pLeader.Get(), "Squad can now OPEN FIRE");
		else
			BotPrintTalkMessageOne(m_pLeader.Get(), "Squad is now HOLDING FIRE");
	}

	inline BOOL SquadCanShoot(void)
	{
		return bCanFire;
	}

	inline BOOL IsStealthMode(void)
	{
		return (m_CombatType == COMBAT_STEALTH);
	}

	inline BOOL IsProneMode(void)
	{
		return (m_CombatType == COMBAT_PRONE);
	}

	inline BOOL IsCrouchMode(void)
	{
		return (m_CombatType == COMBAT_CROUCH);
	}

	inline edict_t* GetLeader(void) const
	{
		return static_cast<edict_t*>(m_pLeader.Get());
	}

	void SetCombatType(eCombatType iCombatType)
	{
		edict_t* pLeader = GetLeader();

		m_CombatType = iCombatType;

		if (!pLeader /*|| (pLeader->v.flags & FL_FAKECLIENT)*/)
			return;

		char szCombatType[16];

		szCombatType[0] = 0;

		switch (m_CombatType)
		{
		case COMBAT_STEALTH:
			strcpy(szCombatType, "STEALTH");
			break;
		case COMBAT_PRONE:
			strcpy(szCombatType, "PRONE");
			break;
		case COMBAT_CROUCH:
			strcpy(szCombatType, "CROUCH");
			break;
		case COMBAT_NONE:
		case COMBAT_COMBAT:
			strcpy(szCombatType, "NORMAL");
			break;
		}

		BotPrintTalkMessageOne(pLeader, "Combat mode is now %s\n", szCombatType);
	}

	void ChangeLeader(void)
	{
		if (m_theSquad.IsEmpty())
		{
			SetLeader(NULL);
		}
		else
		{
			m_pLeader = m_theSquad.Pop();

			if (m_theSquad.IsEmpty())
				SetLeader(NULL);
			else
			{
				Init(); // new squad init
			}
		}
	}

	void removeMember(edict_t* pMember)
	{
		dataStack<MyEHandle> tempStack;
		MyEHandle* temp;

		tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			temp = tempStack.ChoosePointerFromStack();

			if (temp->Get() == pMember)
			{
				m_theSquad.RemoveByPointer(temp);
				tempStack.Init();
				return;
			}
		}
	}

	inline BOOL IsLeader(edict_t* pLeader) const
	{
		return (GetLeader() == pLeader);
	}

	void AddMember(edict_t* pEdict)
	{
		if (!IsMember(pEdict))
		{
			MyEHandle newh;

			newh.Set(pEdict);

			m_theSquad.Push(newh);
		}
	}

	int numMembers()
	{
		dataStack<MyEHandle> tempStack;
		int num = 0;

		tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			tempStack.ChooseFromStack();

			num++;
		}

		return num;
	}

	BOOL IsMember(edict_t* pEdict)
	{
		dataStack<MyEHandle> tempStack;
		MyEHandle temp;

		tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			temp = tempStack.ChooseFromStack();

			if (temp.Get() == pEdict)
			{
				tempStack.Init();
				return TRUE;
			}
		}

		return FALSE;
	}

	inline BOOL isFormation(eSquadForm theFormation)
	{
		return m_theDesiredFormation == theFormation;
	}

	inline void ChangeFormation(eSquadForm theNewFormation)
	{
		m_theDesiredFormation = theNewFormation;
	}

	inline float GetSpread(void) const
	{
		return m_fDesiredSpread;
	}

	inline void ChangeSpread(float fNewSpread)
	{
		m_fDesiredSpread = fNewSpread;
	}

	int GetFormationPosition(edict_t* pEdict)
	{
		int iPosition = 0;

		dataStack<MyEHandle> tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			iPosition++;

			if (pEdict == tempStack.ChooseFromStack().Get())
			{
				/// !! musssst init
				tempStack.Init();
				return iPosition;
			}
		}

		return 0;
	}

	Vector GetFormationVector(edict_t* pEdict);

	void UpdateAngles(void)
	{
		const edict_t* pLeader = GetLeader();

		if (pLeader->v.velocity.Length2D() != 0)
		{
			m_vLeaderAngle = UTIL_VecToAngles(pLeader->v.velocity);
		}
	}

private:
	// use 'EHandles' for squads
	// as players might leave and stuff...
	MyEHandle m_pLeader;
	dataStack<MyEHandle> m_theSquad;

	eSquadForm m_theDesiredFormation;
	float m_fDesiredSpread;
	BOOL bCanFire;

	eCombatType m_CombatType;

	Vector m_vLeaderAngle;
};

class CTFCBackpack
{
public:
	CTFCBackpack()
	{
		memset(this, 0, sizeof(CTFCBackpack));
	}

	CTFCBackpack(edict_t* pEdict)
	{
		memset(this, 0, sizeof(CTFCBackpack));

		m_pEdict = pEdict;
	}

	BOOL givesHealth(int val)
	{
		return (m_iHealth >= val);
	}

	BOOL givesAmmo(int val)
	{
		return (m_iAmmo >= val);
	}

	BOOL givesArmor(int val)
	{
		return (m_iArmor >= val);
	}

	BOOL givesCells(int val)
	{
		return (m_iCells >= val);
	}

	edict_t* getEdict()
	{
		return m_pEdict;
	}

	BOOL isEdict(edict_t* pEdict)
	{
		return (m_pEdict == pEdict);
	}

	void setHealth(int val)
	{
		m_iHealth = val;
	}

	void setAmmo(int val)
	{
		m_iAmmo = val;
	}

	void setArmor(int val)
	{
		m_iArmor = val;
	}

	void setCells(int val)
	{
		m_iCells = val;
	}

	BOOL isForTeam(int team)
	{
		if (m_iTeam == 0)
			return TRUE; // any team

		return (m_iTeam == team);
	}

	void setTeam(int team)
	{
		m_iTeam = team;
	}

	float distanceFrom(Vector vec)
	{
		return (vec - m_pEdict->v.origin).Length();
	}

private:
	int m_iHealth;
	int m_iAmmo;
	int m_iArmor;

	int m_iCells;
	int m_iTeam;

	edict_t* m_pEdict;
};

class CTFCBackpacks
{
public:
	// makes new back pack & returns pointer
	CTFCBackpack* addBackpack(CTFCBackpack const backpack)
	{
		m_Backpacks.Push(backpack);

		return m_Backpacks.GetHeadInfoPointer();
	}

	void removePrevBackpack()
	{
		m_Backpacks.Pop();
	}

	edict_t* findBackpack(Vector const location, int team, int min_health, int min_cells, int min_armor, int min_ammo)
	{
		float fDist;
		float fNearestDist;
		edict_t* pNearest;
		CTFCBackpack* pack;
		edict_t* pent;

		dataStack<CTFCBackpack> tempStack = m_Backpacks;

		pNearest = NULL;
		fNearestDist = 4096.0;

		while (!tempStack.IsEmpty())
		{
			pack = tempStack.ChoosePointerFromStack();

			if (pack->isForTeam(team) &&
				pack->givesHealth(min_health) && pack->givesCells(min_cells) &&
				pack->givesArmor(min_armor) && pack->givesAmmo(min_ammo))
			{
				if ((fDist = pack->distanceFrom(location)) < fNearestDist)
				{
					pent = pack->getEdict();

					if (pent->v.effects & EF_NODRAW)
						continue;

					fNearestDist = fDist;
					pNearest = pent;
				}
			}
		}

		return pNearest;
	}

	void FreeMemory()
	{
		m_Backpacks.Destroy();
		m_Backpacks.Init();
	}

private:
	dataStack<CTFCBackpack> m_Backpacks;
};

#define LEARNED_FILE_VER 1

class CLearnedHeader
{
public:
	CLearnedHeader(int iId);

	BOOL operator == (CLearnedHeader other);
	BOOL operator != (CLearnedHeader const other)
	{
		return !(*this == other);
	}

	char szBotVersion[32];
	int iProfileId;
	int iDataVersion;
};

class CBotSquads
{
public:

	void FreeMemory(void)
	{
		CBotSquad* pSquad;
		dataStack<CBotSquad*> tempStack = m_theSquads;

		while (!tempStack.IsEmpty())
		{
			pSquad = tempStack.ChooseFromStack();

//			if (pSquad)
				delete pSquad;
			pSquad = NULL;
		}

		m_theSquads.Destroy();
	}

	void removeSquadMember(CBotSquad* pSquad, edict_t* pMember)
	{
		pSquad->removeMember(pMember);

		if (pSquad->numMembers() <= 1)
		{
			RemoveSquad(pSquad);
		}
	}

	// AddSquadMember can have many effects
	// 1. scenario: squad leader exists as squad leader
	//              assign bot to squad
	// 2. scenario: 'squad leader' exists as squad member in another squad
	//              assign bot to 'squad leaders' squad
	// 3. scenario: no squad has 'squad leader'
	//              make a new squad
	CBotSquad* AddSquadMember(edict_t* pLeader, edict_t* pMember);

	CBotSquad* FindSquadByLeader(edict_t* pLeader)
	{
		CBotSquad* pSquad;
		dataStack<CBotSquad*> tempStack = m_theSquads;

		while (!tempStack.IsEmpty())
		{
			pSquad = tempStack.ChooseFromStack();

			if (pSquad->IsLeader(pLeader))
			{
				tempStack.Init();
				return pSquad;
			}
		}

		return NULL;
	}

	void RemoveSquad(CBotSquad* pSquad);

	void UpdateAngles(void)
	{
		CBotSquad* pSquad;
		dataStack<CBotSquad*> tempStack = m_theSquads;

		pSquad = NULL;

		while (!tempStack.IsEmpty())
		{
			try
			{
				pSquad = tempStack.ChooseFromStack();

				pSquad->UpdateAngles();
			}

			catch (...)
			{
				// Arghhhhh

				m_theSquads.Remove(pSquad);
				tempStack = m_theSquads;
			}
		}
	}

	void ChangeLeader(CBotSquad* theSquad);

private:
	dataStack<CBotSquad*> m_theSquads;
};

class CBotGAValues;
class CPerceptron;

class CBotVisibles;

#define BOT_COST_RANGE   64
#define BOT_COST_BUCKETS 7

class TSObjective
{
public:
	TSObjective(int id, Vector origin, char* name)
	{
		m_iId = id;
		m_vOrigin = origin;
		m_szName = name;
	}
	/*guurk*/
	bool operator ==(TSObjective& comp) {
		return m_iId == m_iId;
	}
	/*/guurk*/
	inline int getID() { return m_iId; }

	Vector getOrigin() { return m_vOrigin; }
private:
	int m_iId;
	Vector m_vOrigin;
	char* m_szName;
};
/*
class CCompareTSObjective
{
public:
	bool operator () ( TSObjective a, TSObjective b )
	{
		return (a.getID()>b.getID());
	}
};*/

class CBot
{
private:

	//float timeToPositionCrossHair ( Vector vTarget );

	CBotVisibles* m_pVisibles;

	CTypeVector<short int>  m_vNearestRememberPoint;
	BOOL m_bNearestRememberPointVisible;
	BOOL m_bLookingForEnemy;
	CPerceptron* dec_runForCover;
	CPerceptron* dec_faceHurtOrigin;
	CPerceptron* dec_followEnemy;
	CPerceptron* dec_flapWings;

	//CPerceptron *dec_useBlink;
	//CPerceptron *dec_pressBlinkButton;
	BOOL m_bFlappedWings;
	BOOL m_bZoom;
	//	CPerceptron *dec_feign;

	float m_fNextShootButton;

	////////////////////////////////////
	CPerceptron* dec_jump;
	CPerceptron* dec_duck;
	CPerceptron* dec_strafe;
	//CPerceptron *dec_stop_shoot;
	//CPerceptron *dec_firepercent;

	NNGATrained* m_pAiming;
	/*
	NN *m_pAiming; // in composite vector to middle of aiming object
	// out aiming vector x and y (pitch & yaw)
	CGA *m_pAimingGA;
	CBotGAValues *m_pAimingGAVals;
	float m_fAimLearnTime;
	Vector vPrevAimOrigin;
	BOOL bSetPrevAimOrigin;
	BOOL bAimStopMoving;
	//*/

	CBotGAValues* m_pTSWeaponSelect;

	CBotGAValues* m_pPersonalGAVals;
	CGA* m_personalGA;
	CGA* m_pFlyGA;

	CBotGAValues* m_pFlyGAVals;

	float m_fPrevFlyHeight;
	float m_fStartFlyHeight;
	/////////////////////////////////////

	float m_fLastPlaceDetpack;
	/////////////////////////////////////
public:
	dataUnconstArray<int> iDetPackWaypoints; // for TFC Demoman
	float m_fPrevHealth;
	int m_iCombatTeam;
	float m_fCombatFitness;
	int m_iNumDeaths;
	CIntGAValues* m_pCombatBits;
private:
	float m_fNextUpdateHealth;
	int m_iPrevTeamScore;
	float m_fNextCheckFeignTime;

	BOOL m_bUsedMelee;

	unsigned char m_iBoredom; // negative if really bored

	CTypeVector<short int> m_seenFlagPos;

	void decideJumpDuckStrafe(float fEnemyDist, Vector vEnemyOrigin);

	//float utility ( CBotTask eTask );

	void SawSpy(edict_t* pPlayer)
	{
		m_pSpySpotted = pPlayer;
		m_fSeeSpyTime = gpGlobals->time + RANDOM_FLOAT(4.0, 8.0);
	}

	CBotGAValues* m_GASurvival;
	//	CBotGAValues *m_GATFCSpy;
	float m_fLowestEnemyCost;
	Vector m_vLowestEnemyCostVec;

	float fRangeCosts[BOT_COST_BUCKETS][BOT_COST_BUCKETS];

	void clearEnemyCosts();

	void workEnemyCosts(edict_t* pEntity, Vector vOrigin, float fDistance);

	float m_fNextWorkRangeCosts;

	BOOL m_bPlacedPipes;
	Vector m_vPipeLocation;

	// so that the bot knows if hes seen a spy that he knows is the enemy
	edict_t* m_pSpySpotted;
	// time allowed before bot gives up looking for spy
	float m_fSeeSpyTime;

	// so that spys disguise mostly only as classes it has seen recently
	//CRememberedPositions m_ClassesSeen;

	edict_t* m_pTank; // CFuncTank
	float m_fGrenadePrimeTime;
	int m_iGrenadeHolding;

	float m_fUpdateFlagTime;
	BOOL m_bHasFlag; // has a flag or the only flag?
	edict_t* m_pFlag; // holding this flag

	BOOL m_bCommInit;
	// svencoop
	float m_fNextUseScientist;
	float m_fNextUseBarney;

	float m_fNextGetEnemyTime;
	float m_fUpgradeTime;

	// the list of visible entities
//	dataStack<edict_t*>	m_stBotVisibles;

	// Bitmask of conditions the bot has
	int                 m_ibBotConditions;

	// time of last updating the conditions
	float				m_fLastUpdateConditions;

	// amount of entities to update vision of per frame
	int m_iVisUpdateRevs;

	// next time to use a tank in case bot is stuck
	float m_fNextUseTank;

	// Current bot task, pointer to a task in the stack
	CBotTask* m_CurrentTask;

	// temporary chat message storage
	char m_szChatString[BOT_CHAT_MESSAGE_LENGTH];

	//MyEHandle m_pSentry;
	//Vector m_vSentry;

	// -----------HL Rally-------------

	// if bot has selected a car at the start of game
	BOOL m_bSelectedCar;

	//BOOL m_bSelectedGun; // TS

	// current gear
	int m_iGear;

	/////////////////////////////////
	// evloutionary fitness
	float m_fSurvivalTime;
	float m_fSpawnTime;
	int m_iNumFragsSinceDeath;

	// --------------------------------

	void loadLearnedData();

	// current time for reacting to situations
	float m_fReactionTime;

	// period for bot to work without thinking
	float m_fThinkTime;

	// needed to test if bot evolved successfully
	int m_iLastSpecies;

	// last time bot investigated an area of sound somewhere on the map
	float m_fInvestigateSoundTime;

	// last time bot called its think function
	float m_fLastThinkTime;

	// last time bot tried to build something
	float m_fNextBuildTime;

	// last time bot updated it's waypoint
	float m_fUpdateWaypointTime;

	// last time bot listened to a sound
	float m_fListenToSoundTime;

	// last time bot used a button
	float m_fUseButtonTime;

	// last time bot tried to evolve in NS as alien
	float m_fAttemptEvolveTime;

	// last time bot looked for his squad
	float m_fLookForSquadTime;

	// last time the bot changed its ladder angles incase
	// it might be falling off the ladder
	float m_fUpdateLadderAngleTime;

	BOOL m_bStartedGame;
	BOOL m_bChoseClass;

	//int m_iSentryLevel;

	BOOL m_bHasLongJump;

	int m_iLastButtons;

	BOOL m_bSaidInPosition;
	BOOL m_bSaidMoveOut;
	BOOL m_bSaidAllClear;

	int m_iLadderButtons;

	float m_fNextUseVGUI;

	CTypeVector<short int>  m_vLastSeeEnemyPosition;
	// use short ints, only need from -4096 to + 4096,
	// dont need to bother with much precision so use ints for less memory!

	float m_fNextUseSayMessage;
	float m_fNextCheckCover;

	float m_fChatMessageTime;

	float m_fSearchEnemyTime;

	float m_fSayGreetingsTime;

	int m_iVguiMenu;

	CBotSquad* m_stSquad;

	short int m_iWeaponsNeeded[MAX_WEAPONS];

	//unsigned char m_iEnemyPositions;
	//unsigned char m_iLastUsedEnemyPosition;

	//CTypeVector<short int> m_vEnemyPositions[MAX_REMEMBER_POSITIONS];
	CRememberedPositions m_vRememberedPositions;

	// Store the last 5 visited resource fountains
	edict_t* m_pVisitedFuncResources[MAX_REMEMBER_POSITIONS];
	unsigned char m_iVisitedFuncResources;

	BOOL m_bLadderAnglesSet;
	Vector m_vLadderAngles;

	float m_fLastNoMove;
	/*
		/////////////////////////
		// TFC private stuff

		CTypeVector<short int> m_fFeignPosition;
	*/
	float m_fHoldAttackTime;

	void FlapWings(void);

	BOOL m_bBuiltDispenser;
	BOOL m_bIsDisguised;

	// teleport/phase gate
	float m_fUseTeleporterTime;

	//BOOL m_bBuiltTeleportEntrance;
	//BOOL m_bBuiltTeleportExit;

	BOOL SentryNeedsRepaired();

	void RepairSentry(int iNewScheduleId);

	BOOL m_bLookForNewTasks;

	void DoTasks();

	float m_f3dVelocity;
	float m_f2dVelocity;

	CTypeVector<short int> m_vSpawnPosition;

	void CheckStuck(void);

	//void workButtons ();

	dataUnconstArray<edict_t*> m_pTeleportersUsed;

	BOOL hasUsedTeleporter(edict_t* pTeleport)
	{
		return m_pTeleportersUsed.IsMember(pTeleport);
	}

	void usedTeleporter(edict_t* pTeleport)
	{
		m_pTeleportersUsed.Add(pTeleport);
	}

	void clearUsedTeleporters()
	{
		m_pTeleportersUsed.Clear();
	}

public:

	eTS_State m_iTS_State;
	float m_fLastBulletFired;
	dataUnconstArray<TSObjective> m_TSObjectives;

	void saveLearnedData();
	void touchedWpt();
	void gotStuck();

	BOOL hasFlag()
	{
		return (m_bHasFlag && m_pFlag);
	}

	edict_t* getSentry();

	BOOL CanBuild(edict_t* pEdict, int* metal = NULL);

	const char* getLookTaskDescription()
	{
		switch (m_CurrentLookTask)
		{
		case BOT_LOOK_TASK_NONE:
			return "BOT_LOOK_TASK_NONE";
		case BOT_LOOK_TASK_NEXT_WAYPOINT:
			return "BOT_LOOK_TASK_NEXT_WAYPOINT";
		case BOT_LOOK_TASK_SEARCH_FOR_ENEMY:
			return "BOT_LOOK_TASK_SEARCH_FOR_ENEMY";
		case BOT_LOOK_TASK_FACE_ENEMY:
			return "BOT_LOOK_TASK_FACE_ENEMY";
		case BOT_LOOK_TASK_FACE_TEAMMATE:
			return "BOT_LOOK_TASK_FACE_TEAMMATE";
		case BOT_LOOK_TASK_FACE_GOAL_OBJECT:
			return "BOT_LOOK_TASK_FACE_GOAL_OBJECT";
		case BOT_LOOK_TASK_FACE_ENTITY:
			return "BOT_LOOK_TASK_FACE_ENTITY";
		case BOT_LOOK_TASK_FACE_TASK_VECTOR:
			return "BOT_LOOK_TASK_FACE_TASK_VECTOR";
		case BOT_LOOK_TASK_FACE_TASK_EDICT:
			return "BOT_LOOK_TASK_FACE_TASK_EDICT";
		case BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY:
			return "BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY";
		case BOT_LOOK_TASK_LOOK_AROUND:
			return "BOT_LOOK_TASK_LOOK_AROUND";
		case BOT_LOOK_TASK_FACE_GROUND:
			return "BOT_LOOK_TASK_FACE_GROUND";
		}

		return "Unknown";
	}

	void setupDataStructures();
	//void freeDataStructures ();

	CPerceptron* dec_attackElectrified;
	CPerceptron* dec_stunt;
	edict_t* m_pElectricEnemy;

	BOOL isInAnimate(edict_t* pEntity);

	BOOL isFriendly(edict_t* pEntity);

	CBotSquad* getSquad()
	{
		return m_stSquad;
	}

	inline BOOL onGround()
	{
		return UTIL_OnGround(pev);
	}

	void printBoredoms(edict_t* pEdictTo)
	{
		BotMessage(pEdictTo, 0, "%s's boredom = %d (%0.1f percent)", m_szBotName, m_iBoredom, (float)m_iBoredom / 255);
	}

	void havingFun(void)
	{
		// keep boredom within 0 and 255.
		if (m_iBoredom < 255)
			m_iBoredom++;
	}

	void gettingBored(void)
	{
		if (m_iBoredom > 0)
			m_iBoredom--;
	}

	BOOL WantToLeaveGame(void);

	BOOL WantToFindEnemy(void);

	void PlayerDead(edict_t* pPlayer)
	{
		if (pPlayer == m_pSpySpotted)
			m_pSpySpotted = NULL;
	}

	void LookForNewTasks(void);
	/*
		BOOL CanHealPlayer ( edict_t *pPlayer )
		{
			BOOL bCanHeal = FALSE;

			if ( gBotGlobals.IsNS() )
			{
				//if ( pPlayer->v.health < pPlayer->v.max_health )
				//	return
				bCanHeal = IsGorge() && HasWeapon(NS_WEAPON_HEALINGSPRAY);
			}
			else if ( gBotGlobals.IsMod(MOD_SVENCOOP) )
			{
				bCanHeal = HasWeapon(SVEN_WEAPON_MEDKIT);
			}
			else if ( gBotGlobals.IsMod(MOD_TFC) )
			{
				if ( ( pev->playerclass == TFC_CLASS_MEDIC ) && HasWeapon(TF_WEAPON_MEDIKIT) )
					bCanHeal = TRUE;
				else if ( ( pev->playerclass == TFC_CLASS_ENGINEER ) && HasWeapon(TF_WEAPON_SPANNER) )
					bCanHeal = TRUE;
			}
		}*/

	BOOL ThinkSpyOnTeam(edict_t* pSpy);

	void ThrowGrenade(edict_t* pEnemy, int preference = 0, BOOL bDontPrime = FALSE);

	BOOL ThrowingGrenade();

	void LostDisguise()
	{
		m_bIsDisguised = FALSE;
	}

	void Disguised()
	{
		m_bIsDisguised = TRUE;
	}

	void DispenserBuilt()
	{
		m_bBuiltDispenser = TRUE;
	}

	void DispenserDestroyed()
	{
		m_bBuiltDispenser = FALSE;
	}

	void NeedMetal(BOOL flush = TRUE, BOOL priority = FALSE, int iSched = 0);
	void FindBackPack(int health = 0, int cells = 0, int armour = 0, int ammo = 0, BOOL flush = TRUE, BOOL priority = FALSE, int iSched = 0);

	void CantBuildHere()
	{
		m_fNextBuildTime = gpGlobals->time + RANDOM_FLOAT(1.0, 2.0);
	}

	void StoppedBuilding()
	{
		UpdateCondition(BOT_CONDITION_STOPPED_BUILDING);
	}

	BOOL HasBuiltSentry();

	//void setSentry ( edict_t *pEdict )
	//{
	//	m_pSentry.Set(pEdict);
	//}

	BOOL builtTeleporterEntrance();

	BOOL builtTeleporterExit();

	edict_t* getTeleporterExit();

	/*void TeleportEntranceDestroyed ()
	{
		m_bBuiltTeleportEntrance = FALSE;
	}

	void TeleportExitDestroyed ()
	{
		m_bBuiltTeleportExit = FALSE;
		//m_pTeleExit = NULL;
	}*/

	/*void SentryDestroyed ()
	{
		m_iSentryLevel = 0;
		m_pSentry.Set(NULL);
	}*/

	//void UpgradedSentry ()
	//{
	//	m_iSentryLevel ++;
	//}

	edict_t* m_pSentryToKill;

	void TFC_UpdateFlagInfo(void);

	BOOL m_bAcceptHealth; // if TRUE bot will wait for health when he gets it.

	Vector m_vAvoidMoveTo;
	float m_fAvoidTime;

	edict_t* m_MegaHALTalkEdict;

	void SayInPosition(void);

	BOOL IsUsingTank(void);

	edict_t* GetUsingTank(void)
	{
		if (m_pTank)
			return m_pTank;

		return NULL;
	}

	BOOL CanUseTank(edict_t* pTank)
	{
		if (m_fNextUseTank > gpGlobals->time)
			return FALSE;
		// already using one
		if (IsUsingTank())
			return FALSE;
		if (UTIL_TankUsed(pTank))
			return FALSE;
		return TRUE;
	}

	// Use secondary attack for one time only..
	// not a worky??!
	inline void UseRPGLaser(void)
	{
		AddPriorityTask(CBotTask(BOT_TASK_SECONDARY_ATTACK, 0));
	}

	inline BOOL IsSquadLeader(void)
	{
		return (!m_pSquadLeader && m_stSquad);
	}

	BOOL IsHoldingMiniGun(void);

	// temporary storage to later pick up weapons needed
	inline void SetWeaponsNeeded(short int* iArray)
	{
		memcpy(m_iWeaponsNeeded, iArray, sizeof(short int) * MAX_WEAPONS);
	}

	// returns player standing on the bot
	edict_t* PlayerStandingOnMe(void);

	// returns player bot is standing on
	edict_t* StandingOnPlayer(void);

	// store buttons while on ladder in case of think time clearing buttons
	void SetLadderMovement(int iButtons)
	{
		m_iLadderButtons = iButtons;
	}

	// leave squad/clear it completely
	BOOL RemoveMySquad(void);

	// add another player to bots squad
	void AddToSquad(edict_t* pLeader);

	// available to add to a squad?
	BOOL CanAddToSquad(edict_t* pLeader);

	// is used in the game?
	inline BOOL IsUsed(void)
	{
		return (m_bIsUsed && m_pEdict);
	}

	// is in the squad "theSquad"?
	inline BOOL InSquad(CBotSquad* theSquad)
	{
		return (theSquad == m_stSquad);
	}

	// remove squad from bot
	void ClearSquad(void)
	{
		m_stSquad = NULL;
		m_pSquadLeader = NULL;
	}

	void SetSquad(CBotSquad* theSquad)
	{
		ClearSquad();
		m_stSquad = theSquad;
	}

	inline void SetVGUIState(int iVguiMenu)
	{
		m_iVguiMenu = iVguiMenu;
	}

	inline BOOL BotWantsCombatItem(int iCombatItem)
	{
		return (m_iCombatInfo & iCombatItem) > 0;
	}

	inline void SetWantCombatItem(int iCombatItem, BOOL bSet)
	{
		if (bSet)
			m_iCombatInfo |= iCombatItem;
		else
			m_iCombatInfo &= ~iCombatItem;
	}

	// shortcut pointer to bots pEdict pointer, set up on bot create
	// nullified on initialise, bot functions assume this is not NULL.
	struct entvars_s* pev;

	// Returns TRUE when already visited resource tower
	// (already added to list of those visited)
	BOOL HasVisitedResourceTower(edict_t* pEdict);

	// add resource tower to a list of those visited so
	// gorge doesn't go back to ones we've been to already
	void AddVisitedResourceTower(edict_t* pEdict);

	// Make bot remember this vector position, bot will return to it at some point
	void RememberPosition(Vector vOrigin, edict_t* pEntity, int flags = 0);

	// Add a task to the end of the task stack
	void AddTask(const CBotTask& Task);

	// Add a task to the fron of the task stack
	void AddPriorityTask(const CBotTask& Task);

	// stop bot from moving
	void StopMoving(void);

	// change vector bot wants to move to
	void SetMoveVector(const Vector& vOrigin);

	// The bot's task it needs to carry out when reaching its goal
	// in NS when given an order by the commander
	CBotTask m_OrderTask;

	// Goal origin, a somewhat nearby vector to
	// where the bot wants to go
	CTypeVector<short int> m_vGoalOrigin;

	// using crappy non-waypointed navigation?
	BOOL m_bNotFollowingWaypoint;

	// Bots weapons
	CBotWeapons m_Weapons;
	// the pointer to the current drawn weapon
	CBotWeapon* m_pCurrentWeapon;

	/////////////////////////////////////////
	// Navigation

	// open list of waypoints for A*
	priority_queue<AStarNode*, vector<AStarNode*>, CompareAStarNode> sOpenList;//dataStack<int>
	// A* nodes
	AStarNode aPathsFound[MAX_WAYPOINTS];

	/////////////////////////////////////////
	// Nearest buidlable object in NS
	edict_t* m_pNearestBuildable;

	// Bit mask of players bot has greeted.
	int m_iSaidGreetingsTo;

	float m_fKickTime;

	edict_t* m_pEdict;

	edict_t* m_pPickupEntity;
	float m_fPickupItemTime;

	// bitmask of weapons holding.
	int	m_iBotWeapons;

	// is bot being used?
	BOOL m_bIsUsed;

	// the bots actaul name in the game
	char* m_szBotName;

	// the bots "bot profile" info
	bot_profile_t m_Profile;

	// bots current team
	int m_iTeam;

	// If kill is TRUE, bot will kill itself
	BOOL m_bKill;
	// if kick is TRUE, bot will be kicked from the game
	BOOL m_bKick;

	// If last attack failed
	//BOOL m_bShotFailed;

	// does bot want to use an ammo dispenser
	// to get more ammo?
	BOOL m_bCanUseAmmoDispenser;

	// has bot has already gestated in NS aliens?
	BOOL m_bHasGestated;

	//////////////////////////
	// for LOOK_TASKS

	Vector m_vCurrentLookDir;
	float m_fLastLookTime;

	////////////////////////////

	////////////////////////////
	// for Moving

	Vector m_vMoveToVector;		// Is the vector that the bot wants to move to.
	BOOL m_bMoveToIsValid;		// Is FALSE When bot has no waypoint.

	/////////////////////////////
	//

	int m_iRespawnState;		// Is set to RESPAWN_NEED_TO_RESPAWN upon level
	// change so we know to rejoin the bot.
	//float m_fNextCheckWptVis;
	//BOOL m_bWaypointVis;

	/////////////////////////////
	// NAVIGATION RELATED

	int GetNextWaypoint(void)
		// get waypoint AFTER current waypoint,
		// return current if no next one.
	{
		dataStack<int> tempStack;

		int iNextWaypoint;

		tempStack = m_stBotPaths;
		iNextWaypoint = m_iCurrentWaypointIndex;

		while (!tempStack.IsEmpty() && (iNextWaypoint == m_iCurrentWaypointIndex))
		{
			iNextWaypoint = tempStack.ChooseFromStack();
		}

		// INIT TEMP STRUCTURE AFTER USING!!!
		tempStack.Init();

		return iNextWaypoint;
	}

	// keep bot facing one certain angle
	// when climbing ladder so bot doesn't
	// jerk around like crazy
	void SetLadderAngles(Vector const vAngles)
	{
		m_vLadderAngles = vAngles;
		m_bLadderAnglesSet = TRUE;
	}

	// clear the angles
	void UnSetLadderAngleAndMovement(void)
	{
		m_bLadderAnglesSet = FALSE;
		m_iLadderButtons = 0;
	}

	// get the angles....
	inline Vector GetLadderAngles(void)
	{
		return m_vLadderAngles;
	}

	// if we have set ladder angles?
	inline BOOL LadderAnglesSet(void)
	{
		return m_bLadderAnglesSet;
	}

	// >?
	inline BOOL LadderMovementSet(void)
	{
		return (m_iLadderButtons != 0);
	}

	//////////////////////////
	// DMC functions

	// Useful so bot doesn't run away and always follows enemies etc.
	int m_iQuakeItems;

	// returns TRUE if bot picked up invulnerabilty
	inline BOOL DMC_HasInvulnerability(void)
	{
		return ((m_iQuakeItems & (1 << DMC_INVULNERABILITY)) > 0);
	}

	inline BOOL DMC_HasInvisibility(void)
	{
		return ((m_iQuakeItems & (1 << DMC_INVISIBILITY)) > 0);
	}

	///////////////////////////
	// NS COMBAT
	int m_iExperience;
	int m_iLevel;

	///////////////////////////
	//

	// Failed goals are cleared every so often so
	// bots can retry them again
	float m_fNextClearFailedGoals;

	// Going up or down ?
	short int m_siLadderDir;

	// Bots current waypoint goal INDEX
	int m_iWaypointGoalIndex;

	// the waypoint the bot is currently at
	int m_iCurrentWaypointIndex;

	// the waypoint bot was at before going
	// onto the current waypoint
	int m_iPrevWaypointIndex;

	// the flags for the current waypoint (e.g. Jump? / crouch? )
	int m_iCurrentWaypointFlags;

	// Distance from current waypoint last time
	// in case we flew by the waypoint really fast
	float m_fPrevWaypointDist;

	// Time we last saw the current waypoint
	float m_fLastSeeWaypoint;

	// bots entity it wants to avoid
	edict_t* m_pAvoidEntity;

	// time for bot being stuck
	float m_fStuckTime;

	// Bots Current Move Speed
	float m_fMoveSpeed;
	// Max speed the bot can move at
	float m_fMaxSpeed;
	// Speed bot wants to move at
	float m_fIdealMoveSpeed;

	// Moving up/down? this is the speed
	float m_fUpSpeed;
	// Time for it being set
	float m_fUpTime;

	// moving left/right strafe
	float m_fStrafeSpeed;
	// time for it being set so we don't
	// set it by mistake, overwriting our current value.
	float m_fStrafeTime;

	float m_fTurnSpeed;

	float m_fStartJumpTime;
	float m_fEndJumpTime;

	float m_fStartDuckTime;
	float m_fEndDuckTime;

	// wait a while before searching again
	// for another path
	float m_fFindPathTime;

	float m_fWallAtLeftTime;
	float m_fWallAtRightTime;

	float m_fUpdateVisiblesTime;

	float m_fLookForBuildableTime;

	// used for working out MSEC
	float m_fLastCallRunPlayerMove;
	int m_iMsecVal;
	int m_iMsecNum;
	float m_fMsecDel;
	float m_fSecDelTime;

	////////////////////////////
	// NATURAL SELECTION RELATED

	short int m_iResources;
	//		short int m_siLastGestationState;
	//		short int m_siCurrentUpgrades;

	////////////////////////////
	// CHAT RELATED

	edict_t* m_pLaughEdict;
	//float    m_fLaughEdictTime;
	edict_t* m_pKilledEdict;
	edict_t* m_pKillerEdict;

	BOOL m_bSaidGreetings;

	///////////////////////////
	// TASK RELATED

//	BOOL m_bGotEnemyPath;

	eBotTask m_iLastBotTask;
	eBotTask m_iLastFailedTask; // make sure we dont try that last failed task for a while
	eScheduleDesc m_iLastFailedSchedule;

	////////////////////////////
	// What to do when a bot completes an order in NS
	BOOL m_bHasOrder;
	// e.g. Wait, assemble squad, find a new task etc...
	//CBotTask m_OrderCompleteTask;

	AvHOrderType m_iOrderType;

	BOOL m_bHasAskedForOrder;

	float m_fFailedTaskTime; // Try not to re-do last failed task depending on this time

	////////////////////////////
	// ENEMY RELATED

	//float m_fMaxDangerFactor;
	//float m_fMinDangerFactor;

	edict_t* m_pEnemy;

	float	 m_fAimFactor;
	float    m_fFirstSeeEnemyTime;
	float    m_fLastSeeEnemyTime;
	float    m_fGetAimVectorTime;
	Vector   m_vOffsetVector;
	edict_t* m_pLastEnemy;

	CBotReputation* m_pEnemyRep;

	int m_iPrimaryWeaponId;
	int m_iSecondaryWeaponId;

	////////////////////////////
	// MISCELLANEOUS

	short int m_iCombatInfo;
	//short int m_iCombatUpgrades;

	edict_t* m_pSquadLeader;
	float    m_fFollowSquadTime;
	Vector m_vTeleporter;
	float m_fNextCheckTeleporterExitTime;

	BOOL     m_bNeedToInit;

	float    m_fJoinServerTime;
	float    m_fJoinGameTime;
	float    m_fDistanceFromWall;

	int      m_iNumGamesOnServer;

	float m_fLeaveTime; // time to actually leave, say goodbye first!!!

	CBotTasks m_Tasks;//dataQueue<CBotTask>	m_quBotTaskStack;

	eBotLookTask m_CurrentLookTask;

	// a stack of waypoint PATHS that the bot has failed to reach.
	// This can hold up to as many paths as required. Typically some are removed
	// when the bot wants to find a new path.

	CBotFailedPaths m_stFailedPaths;
	int m_iLastFailedWaypoint;
	dataStack<int> m_FailedGoals;
	//int m_iLastFailedGoalWaypoint;

	// the list of waypoint indexes, the bots path.
	dataStack<int>   m_stBotPaths;

	CBot()
	{
		Init();
	}

	CBot(edict_t* newpEdict)
	{
		SetEdict(newpEdict);
	}

	void BotSquadLeaderThink();

	void SetEdict(edict_t* pSetEdict);

	void RunPlayerMove(void);

	void Init(void);

	/////////////////////////
	// VIEWING ANGLES AND MOVEMENT

	void WorkViewAngles(void);
	void WorkMoveDirection(void);
	void SetViewAngles(const Vector& pOrigin);

	////////////////////////

	void EnemyDown(edict_t* pEnemy);
	BOOL IsEnemy(edict_t* pEntity);

	// FREE LOCAL MEMORY
	// free memory of structures inside the bot
	// do NOT call within bot functions
	void FreeLocalMemory(void);

	int GetLadderDir(BOOL bCheckWaypoint = FALSE);

	BOOL NotStartedGame(void);

	void StartGame(void);

	void ReplyToMessage(char* szMessage, edict_t* pSender, int iTeamOnly);

	void FreeGlobalMemory(void)
	{
		this->FreeLocalMemory();

		Init();
	}

	void DuckAndJump(void);
	void JumpAndDuck(void);

	CBotTask* currentTask()
	{
		return m_CurrentTask;
	}

	void ClearTasks(void)
	{
		//m_WptObjectiveTask = CBotTask(BOT_TASK_NONE);

		m_OrderTask = CBotTask(BOT_TASK_NONE);

		m_CurrentTask = NULL;

		m_Tasks.FlushTasks();

		//m_GoalTasks.Destroy();
	}

	void BotCommand(void);

	void UpdateMsec(void);

	void MapInit(void);

	void BotOnLadder(void);

	void UseTank(edict_t* pTank);

	/////////////////////////////////////
	// VISIBLES
	BOOL IsInVisibleList(edict_t* pEntity);

	BOOL HasUpgraded(int iUpgrade);

	BOOL FVisible(edict_t* pEntity);

	BOOL FVisible(const Vector& vecOrigin);

	BOOL FInViewCone(Vector* pOrigin);

	float DotProductFromOrigin(Vector* pOrigin);

	inline Vector GetGunPosition(void)
	{
		return (pev->origin + pev->view_ofs);
	}

	inline BOOL     IsAlive(void)
	{
		return ((pev->deadflag == DEAD_NO) && (pev->health > 0));
	}

	BOOL     SwitchWeapon(int iId);

	BOOL     IsCurrentWeapon(int iId)
	{
		if (m_pCurrentWeapon == NULL)
			return FALSE;
		return (m_pCurrentWeapon->GetID() == iId);
	}
	// ----------------------------------------------------------
	// Flashlight functions for putting a flashlight on or off

	inline BOOL     FlashlightIsOn(void)
	{
		return ((pev->effects & EF_DIMLIGHT) == EF_DIMLIGHT);
	};

	inline void     FlashlightTurnOn(void)
	{
		if (!FlashlightIsOn())  // If flashlight is already on, we dont want to turn it off!
		{
			pev->impulse = 100;
		}
	};

	inline void     FlashlightTurnOff(void)
	{
		if (FlashlightIsOn())
		{
			pev->impulse = 100;
		}
	}
	// ----------------------------------------------------------
	//

	// Returns TRUE if pEntity isn't an entity
	// that the bot doesn't want to avoid.
	BOOL CanAvoid(edict_t* pEntity, float fDistanceToEntity, float fAvoidDistance);

	void ChangeAngles(float* fSpeed, float* fIdeal, float* fCurrent, float* fUpdate);

	inline BOOL IsOnLadder(void)
	{
		return (pev->movetype == MOVETYPE_FLY);
	};

	inline void     Jump(void)
	{
		pev->button |= IN_JUMP;
	}

	// if bot can fly, this will return true
	BOOL CanFly(void)
	{
		return (IsLerk() || (IsMarine() && HasJetPack()));
	}

	// set crouch button
	inline void     Duck(void)
	{
		pev->button |= IN_DUCK;
	}

	// returns bots ideal climb situation (e.g. flying, climbing, wall-sticking)
	eClimbType GetClimbType(void);

	BOOL     PrimaryAttack(void);

	BOOL     SecondaryAttack(void)
	{
		pev->button |= IN_ATTACK2;

		// Can't shoot while reloading
		if (pev->button & IN_RELOAD)
			return FALSE;

		return TRUE;
	};

	inline void Use(void)
	{
		pev->button |= IN_USE;
	};

	inline void AltButton(void)
	{
		pev->button |= IN_ALT1;
	};

	inline void Reload(void)
	{
		pev->button |= IN_RELOAD;
	};

	// -------------------------------------------------------
	// Bot Conditions
	// Functions for setting, checking and removing conditions

	inline void     UpdateCondition(int iCondition)
	{
		m_ibBotConditions |= iCondition;
	}

	inline BOOL     HasCondition(int iCondition)
	{
		return ((m_ibBotConditions & iCondition) == iCondition);
	}

	inline void     RemoveCondition(int iCondition)
	{
		m_ibBotConditions &= ~iCondition;
	}

	// updates some conditions
	void     UpdateConditions(void);

	// -------------------------------------------------------

	//int		BotDanger				( void );

	// does bot want to pursue the enemy or stop and do his own thing?
	BOOL     WantToFollowEnemy(edict_t* pEnemy);

	// returns TRUE when bot is facing the ideal aim position
	BOOL	 FacingIdeal(void);

	// returns TRUE if bot can pickup the entity pPickup
	BOOL     CanPickup(edict_t* pPickup);

	// bot hears a sound type from a specific origin vector
	void     HearSound(eSoundType iSound, Vector vOrigin, edict_t* pEdict);

	// when the bot wants to think this function is called
	void     Think(void);

	// when an entity touches the bot this is called
	BOOL     Touch(edict_t* pentTouched);

	// when an entity blocks the bot, this is called
	void     Blocked(edict_t* pentBlocked);

	// returns bot team
	int      GetTeam(void);

	// a special event
	void     BotEvent(eBotEvent iEvent, edict_t* pInfo = NULL, edict_t* pExtInfo = NULL, float* pFloatInfo = NULL);

	// initialize after dying
	void     SpawnInit(BOOL bInit);

	// look for a new enemy
	edict_t* FindEnemy(void);

	//void	 EnemyLost				 ( edict_t *pEnemy );

	// does stuff when the bot finds an enemy
	void     EnemyFound(edict_t* pEnemy);

	// returns TRUE if bot knows where the enemy went
	BOOL     HasSeenEnemy(edict_t* pEnemy);

	// get bots vector to aim at for shooting the enemy
	Vector   GetAimVector(edict_t* pBotEnemy);

	// get the distance from vector vec
	float    DistanceFrom(const Vector& vec, BOOL twoD = FALSE);

	// set up tasks so bot runs from the origin
	void	 RunForCover(Vector vOrigin, BOOL bDoItNow = FALSE, int iScheduleId = 0);

	/*BOOL     IsReloading ()
	{
		void *pmodel = GET_MODEL_PTR( m_pEdict );
		int iReloadAnim = LookupActivity(pmodel,pev,ACT_RELOAD);

		return ( pev->sequence == iReloadAnim );
	}*/

	void     Impulse(int impulse) { pev->impulse = impulse; };
	// BUTTONS - END

	BOOL     EvolveInto(int species);
	short int SpeciesOnTeam(int species);
	short int EvolvedSpeciesOnTeam(int species);

	void	 BotChat(eBotChatType iChatType, edict_t* pInfo = NULL, BOOL bSayNow = FALSE);

	BOOL     BotCanUseBuiltStructure(edict_t* structure);
	edict_t* BotCheckForWeldables(void);
	float    DistanceFromEdict(edict_t* pEntity);

	//edict_t *NearestStructure		( int structure_type );

	//////////////////
	// NS Stuff

	BOOL     IsInReadyRoom(void);

	inline float NS_AmountOfEnergy(void)
	{
		return (pev->fuser3 / 10.0);
	}

	BOOL     IsCommander(void);
	BOOL     IsMarine(void);

	BOOL     IsGestating(void);
	BOOL     IsAlien(void);
	BOOL     IsSkulk(void);
	BOOL     IsGorge(void);
	BOOL     IsLerk(void);
	BOOL     IsFade(void);
	BOOL     IsOnos(void);
	/////////////////
	// location stuff
	//void     GetLocation             ( void );
	//BOOL     GotLocation             ( void );

	//////////////////////////////////////////////////

	//edict_t* NearestUsableDoor         ( void );
	//edict_t* NearestVisibleEdict       ( const char *classname ); // returns the nearest edict with classname which is visible to the bot.
	//edict_t* NearestVisibleEdict       ( void ); // returns any nearest edict that is visible

	BOOL     UpdateVisibles(void);

	BOOL     BotCanTalk(void);
	//void     BotSayBuild               ( const char *msg );
	void     BotSay(const char* msg);//, const char *arg1, int team_only );

	// returns true when a bot is skulk and is sticking to a wall in NS mod.
	inline BOOL     IsWallSticking(void) { return (((pev->iuser4 & MASK_WALLSTICKING) == MASK_WALLSTICKING) && ((pev->flags & FL_ONGROUND) == 0) && (pev->waterlevel < 2)); };

	BOOL hasWeb();
	BOOL hasBlink();

	// returns TRUE when bot thinks it is low on ammunition on its current weapon.
	BOOL     LowOnAmmo(void);

	inline BOOL     HasJetPack(void) { return (IsMarine() && ((pev->iuser4 & MASK_UPGRADE_7) == MASK_UPGRADE_7)); };
	inline BOOL     HasUser4Mask(int iBits) { return ((pev->iuser4 & iBits) == iBits); };

	/* Commander stuff */

	//void     BotCommander              ( void );
	//void     BotCommanderLogOut        ( void );
	//void     BotCommanderLogIn         ( void );
	//int      BotGetCommand             ( void );
	//BOOL     BotGiveCommand            ( void );
	//BOOL     BotBuild                  ( void );
	//BOOL     SelectPlayersForOrder     ( edict_t *pent );

	// returns TRUE when bot can follow the player pEdict
	BOOL     CanFollow(edict_t* pEdict);

	// Checks bots weapon bitmask
	BOOL     HasWeapon(int iWeapon);

	// Does bot have ANY weapons?
	inline BOOL     HasWeapons(void)
	{
		return (m_iBotWeapons == 0);
	}

	//edict_t *m_pTeleExit;
};

#define HUD_TEXT_LENGTH 256
//	0 : "Fade In/Out"
//	1 : "Credits"
//	2 : "Scan Out"
#define HUD_EFFECT_FADE_IN_OUT		0
#define HUD_EFFECT_CREDITS			1
#define HUD_EFFECT_FADE_SCAN_OUT	2

// HUD Text class
// allow easy sending of hud messages
class HudText
{
public:

	HudText()
	{
		Initialise();
	}

	// setup hud text class for tool tip properties
	HudText(BOOL tooltip)
	{
		Initialise();

		if (tooltip)
		{
			SetColour1(Vector(255, 255, 0), 75);
			SetColour2(Vector(127, 127, 0), 0);

			SetPosition(0.1, 0.6);
			SetEffect(HUD_EFFECT_FADE_SCAN_OUT);
			SetEffectTime(0.1);
			SetHoldTime(3.0);
			SetFadeTime(0.1, 1.0);
			SetChannel(3);
		}
	}

	void SetPosition(float x, float y)
	{
		// x(string) : "X (0 - 1.0 = left to right) (-1 centers)" : "-1"
		// y(string) : "Y (0 - 1.0 = top to bottom) (-1 centers)" : "-1"

		m_textParms.x = x;
		m_textParms.y = y;
	}

	void SetEffect(int effect)
	{
		//	0 : "Fade In/Out"
		//	1 : "Credits"
		//	2 : "Scan Out"

		m_textParms.effect = effect;
	}

	void SetFadeTime(float fadein, float fadeout)
	{
		m_textParms.fadeinTime = fadein;
		m_textParms.fadeoutTime = fadeout;
	}

	void SetHoldTime(float holdtime)
	{
		m_textParms.holdTime = holdtime;
	}

	void SetChannel(int channel)
	{
		m_textParms.channel = channel;
	}

	void SetEffectTime(float time)
	{
		m_textParms.fxTime = time;
	}

	void SayMessage(const char* message, edict_t* pPlayer);
	void SayMessage(const char* message, Vector colour1, Vector colour2, edict_t* pPlayer);
	void Initialise(void);
	void SetColour1(Vector const colours, int alpha) { m_textParms.r1 = (int)colours.x, m_textParms.g1 = (int)colours.y, m_textParms.b1 = (int)colours.z; m_textParms.a1 = alpha; };
	void SetColour2(Vector const colours, int alpha) { m_textParms.r2 = (int)colours.x, m_textParms.g2 = (int)colours.y, m_textParms.b2 = (int)colours.z; m_textParms.a2 = alpha; };
	void InitMessage(const char* message);

private:
	hudtextparms_t m_textParms;
	char m_sMessage[HUD_TEXT_LENGTH];
};

///////////////////////////////////////////////////////////
// STRING STORAGE

#define STRING_HASHES 26

// class of strings
class CStoredStrings
{
public:

	CStoredStrings()
	{
		int i;

		for (i = 0; i < STRING_HASHES; i++)
			szStringsHead[i].Init();
	}

	void FreeStrings(void)
	{
		int i;

		dataStack<char*> s_tempStack;
		void* pFree;

		for (i = 0; i < STRING_HASHES; i++)
		{
			s_tempStack = szStringsHead[i];

			while (!s_tempStack.IsEmpty())
			{
				pFree = static_cast<void*>(s_tempStack.ChooseFromStack());

				if (pFree)
				{
					free(pFree);
					pFree = NULL;
				}
			}

			szStringsHead[i].Destroy();
		}
	}

	char* GetString(const char* szString)
	{
		int iHashNum = (int)szString[0];

		if (iHashNum < 0)
			iHashNum = -iHashNum;

		int iHashValue = iHashNum % STRING_HASHES;
		char* l_RetString;
		char* szNewString = NULL;

		assert(iHashValue >= 0);
		assert(iHashValue < STRING_HASHES);

		if ((iHashValue < 0) || (iHashValue >= STRING_HASHES))
			return NULL; // problem...

		dataStack<char*> s_tempStack = szStringsHead[iHashValue];

		while (!s_tempStack.IsEmpty())
		{
			l_RetString = s_tempStack.ChooseFromStack();

			if (strcmpi(l_RetString, szString) == 0)
			{
				// Fool the pointer in the tempstack so it doesn't free
				// memory we still want to use.
				s_tempStack.Init();

				return l_RetString;
			}
		}

		// Not found in list of strings
		// Create a new string

		//BotFunc_StringCopy(szNewString,szString);

		szNewString = static_cast<char*>(malloc((sizeof(char) * strlen(szString)) + 1));
		strcpy(szNewString, szString);

		szStringsHead[iHashValue].Push(szNewString);

		return szNewString;
	}
private:
	dataStack<char*> szStringsHead[STRING_HASHES];
};

///////////////////////////////////////////////////////
// CLIENTS

class CPendingToolTips
{
public:
	CPendingToolTips()
	{
		m_fNextTime = 0;
	}

	void Clear()
	{
		m_Tooltips.Destroy();
	}

	void Add(eToolTip tooltip)
	{
		m_Tooltips.Add(tooltip);
	}

	void Think(edict_t* pPlayer);

private:
	dataQueue<eToolTip> m_Tooltips;
	float m_fNextTime;
};

class CBotMenu;

#define MAX_STORED_AUTOWAYPOINT 5

#define CLIENT_CMD_PROJ_REPEL 1
#define CLIENT_CMD_GREN_DEACT 2

#define JUMP_TYPE_JUMP 0
#define JUMP_TYPE_STUNT 1

class CClient
{
public:

	CClient()
	{
		Init();
	}

	void Think(void);

	void Init(void)
	{
		memset(this, 0, sizeof(CClient));

		m_iCurrentWaypoint = -1;
		m_iPlayerId = -1;
		m_iPathWaypointCreateIndex = -1;
		m_iPathWaypointRemoveIndex = -1;
		m_iLastMenuItemSelected = -1;
		m_Tooltips.Clear();

		m_iDebugBot = -1;
	}

	char* GetPass(void)
	{
		return m_szPass;
	}

	~CClient()
	{
		m_iWonId = 0;
	}

	char* steamID()
	{
		return m_szSteamId;
	}

	BOOL IsUsed(void)
	{
		return (m_pPlayer != NULL);
	}

	void SetAccessLevel(int iAccessLevel)
	{
		m_iAccessLevel = iAccessLevel;
	}

	int GetAccessLevel(void) const
	{
		return m_iAccessLevel;
	}

	BOOL IsPlayer(edict_t* pPlayer)
	{
		return (m_pPlayer == pPlayer);
	}

	edict_t* GetPlayer(void) const
	{
		return m_pPlayer;
	}

	BOOL HasPlayerName(const char* szPlayerName)
	{
		// Pointers the same somehow?? Well thats the same :)
		if (szPlayerName == const_cast<char*>(STRING(m_pPlayer->v.netname)))
			return TRUE;

		return (strcmp(szPlayerName, STRING(m_pPlayer->v.netname)) == 0);
	}

	unsigned int GetWonId(void)
	{
		return m_iWonId;
	}

	void SetEdict(edict_t* pPlayer);

	void UpdatePlayerRepId(int iId)
	{
		m_iPlayerId = iId;
	}

	// add new tooltip to the queue
	void AddNewToolTip(eToolTip iToolTip);

	int GetPlayerRepId(void) const
	{
		return m_iPlayerId;
	}

	void RemoveTeleportVector(void)
	{
		if (m_vTeleportVector)
		{
			delete m_vTeleportVector;
			m_vTeleportVector = NULL;
		}
	}

	void setSteamID();

	void SetTeleportVector(Vector const vOrigin)
	{
		RemoveTeleportVector();
		m_vTeleportVector = new Vector(vOrigin);
	}

	const Vector* GetTeleportVector(void) const
	{
		return m_vTeleportVector;
	}

	inline void addCmd(int cmd) { m_iCmd |= cmd; }
	inline void remCmd(int cmd) { m_iCmd &= ~cmd; }
	inline BOOL hasCmd(int cmd) { return (m_iCmd & cmd) == cmd; }
	inline void toggleCmd(int cmd) { if (hasCmd(cmd)) remCmd(cmd); else addCmd(cmd); }

	void FreeGlobalMemory(void);

	edict_t* m_pForceGripEntity;

	BOOL m_bNoTouch;

	BOOL m_bWaypointOn;
	BOOL m_bWaypointPathsOn;

	float m_fWaypointDisplayTime;
	int m_iCurrentWaypoint;

	CPendingToolTips m_Tooltips;

	CBotMenu* m_pMenu;

	int m_iPathWaypointCreateIndex;
	int m_iPathWaypointRemoveIndex;

	int m_iLastMenuItemSelected;

	float m_fJoinServerTime;

	BOOL m_bRecheckAuth;

	// the bot the player is debugging
	int m_iDebugBot;
	edict_t* m_pDebugEnt;
	float m_fDebugEntTime;

	// player sets password for access
	void SetPass(const char* szPass)
	{
		strncpy(m_szPass, szPass, BOT_MAX_PASSWORD_LEN - 1);
		m_szPass[BOT_MAX_PASSWORD_LEN - 1] = 0;

		m_bRecheckAuth = TRUE;
	}

	// check if player has already had this tool-tip sent to them
	inline BOOL HasToolTipSent(eToolTip iToolTip)
	{
		return ((m_iToolTipsSent & (1 << (int)iToolTip)) != 0);
	}

	// sets that the tool tip has been sent
	inline void ToolTipSent(eToolTip iToolTip)
	{
		m_iToolTipsSent |= (1 << (int)iToolTip);
	}

	// sets auto waypoint on/off or toggle
	void AutoWaypoint(int state = -1)
	{
		m_bDebugAutoWaypoint = FALSE;
		m_bSetUpAutoWaypoint = FALSE;

		if (state == 2)
			m_bDebugAutoWaypoint = TRUE;

		if (state == -1)
			m_bAutoWaypoint = !m_bAutoWaypoint;
		else
		{
			m_bAutoWaypoint = (state > 0);
		}
	}

	inline BOOL IsAutoWaypointOn()
	{
		return m_bAutoWaypoint;
	}

	void TFCTeleExitDestroyed()
	{
		if (m_pTFCTeleEntrance.Get())
			m_pTFCTeleEntrance.Get()->v.euser1 = NULL;

		m_pTFCTeleExit.Set(NULL);
	}

	void setTFCTeleExit(edict_t* pEntity)
	{
		m_pTFCTeleExit.Set(pEntity);
	}

	edict_t* getTFCTeleExit()
	{
		return m_pTFCTeleExit.Get();
	}

	void TFCTeleEntranceDestroyed()
	{
		m_pTFCTeleEntrance.Set(NULL);
	}

	void setTFCTeleEntrance(edict_t* pEntity)
	{
		m_pTFCTeleEntrance.Set(pEntity);
	}

	edict_t* getTFCTeleEntrance()
	{
		return m_pTFCTeleEntrance.Get();
	}

	void TFCSentryDestroyed()
	{
		m_pSentry.Set(NULL);
	}

	void setTFCSentry(edict_t* pEntity)
	{
		m_pSentry.Set(pEntity);
	}

	edict_t* getTFCSentry()
	{
		return m_pSentry.Get();
	}

	void checkBuiltSentry();

	void clearIgnoreSentries();

	void addIgnoreSentry(edict_t* pEdict);

private:

	int m_iCmd;
	// bit mask of tool tips already sent to the player
	int m_iToolTipsSent;

	// players entered password
	char m_szPass[BOT_MAX_PASSWORD_LEN];

	// players WON ID
	unsigned int m_iWonId;

	// players bit mask of access to bot commands
	int m_iAccessLevel;

	// used as an id for player rep
	int m_iPlayerId;

	// players set_teleport position
	Vector* m_vTeleportVector;
	//********************

		// autowaypointing stuff
	BOOL m_bAutoWaypoint;

	CAutoWaypointCheck m_vLastAutoWaypointCheckPos[MAX_STORED_AUTOWAYPOINT];
	Vector m_vLastAutoWaypointPlacePos;
	float m_fLastAutoWaypointCheckTime;
	BOOL m_bSetUpAutoWaypoint;
	BOOL m_bDebugAutoWaypoint;

	float m_fCanPlaceJump;
	short int m_iJumpType;
	int  m_iLastButtons;

	int m_iLastLadderWaypointIndex;
	int m_iLastJumpWaypointIndex;

	int m_iLastMoveType;

	float m_fCanPlaceLadder;

	int m_iJoinLadderWaypointIndex;

	// time for debug info to show
	float m_fDebugBotTime;

	MyEHandle m_pTFCTeleExit;
	MyEHandle m_pTFCTeleEntrance;
	MyEHandle m_pSentry;

	float m_fCheckSentryTime;

	dataUnconstArray<edict_t*> ignoreSentries;

	char m_szSteamId[STEAM_ID_LEN];
protected:
	edict_t* m_pPlayer;
};

class CClients
{
public:
	CClients()
	{
		memset(m_Clients, 0, sizeof(CClient) * 32);
		//m_Clients.SetSize(32);
	}

	edict_t* FindClient(const char* szPlayerName)
	{
		int i = 0;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if (m_Clients[i].IsUsed())
			{
				if (m_Clients[i].HasPlayerName(szPlayerName))
					return m_Clients[i].GetPlayer();
			}
		}

		return NULL;
	}

	CClient* GetClientByRepId(int iRepId);

	CClient* ClientConnected(edict_t* pPlayer);

	void ClientDisconnected(edict_t* pPlayer);

	void ClientDisconnected(CClient* pClient);

	CClient* GetClientByIndex(int iIndex)
	{
		if ((iIndex >= 0) && (iIndex < MAX_PLAYERS))
			return &m_Clients[iIndex];

		return NULL;
	}

	CClient* GetClientByEdict(edict_t* pPlayer)
	{
		if (pPlayer == NULL)
			return NULL;

		int iIndex = ENTINDEX(pPlayer);

		if (iIndex <= gpGlobals->maxClients)
		{
			iIndex--;

			if (iIndex >= 0)
			{
				if (m_Clients[iIndex].GetPlayer() == NULL)
					return NULL;

				return &m_Clients[iIndex];
			}
		}

		return NULL;

		/*
		int i = 0;

		if ( pPlayer == NULL )
			return NULL;

		for ( i = 0; i < MAX_PLAYERS; i ++ )
		{
			if ( m_Clients[i].IsPlayer(pPlayer) )
				return &m_Clients[i];
		}

		return NULL;*/
	}

	void FreeGlobalMemory(void)
	{
		int i;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			this->m_Clients[i].FreeGlobalMemory();
		}
	}

	//AddClient(
private:
	//  use our dataArray generic class
	//  for easy initialisation etc..
	CClient m_Clients[MAX_PLAYERS];
};

/////////////////////////
// Allowed Users
// users in bot_users.ini allowed to access bot commands
class CAllowedPlayer
{
public:

	// initializer
	CAllowedPlayer()
	{
		memset(this, 0, sizeof(CAllowedPlayer));
	}

	CAllowedPlayer(const char* szName, char* szPass, int iAccessLevel, char* szSteamId)
	{
		strcpy(m_szName, szName);
		strncpy(m_szPass, szPass, BOT_MAX_PASSWORD_LEN - 1);
		m_szPass[BOT_MAX_PASSWORD_LEN - 1] = 0;
		strncpy(m_szSteamId, szSteamId, STEAM_ID_LEN - 1);
		m_szSteamId[STEAM_ID_LEN - 1] = 0;
		m_iAccessLevel = iAccessLevel;
	}

	BOOL steamID_defined()
	{
		return (m_szSteamId[0] != 0);
	}

	BOOL IsForSteamID(char* steamID)
	{
		// gotta take care if no steam id is mentioned, could be WON we are playing
		if ((steamID && *steamID) && (m_szSteamId && *m_szSteamId))
			return (strcmp(m_szSteamId, steamID) == 0);

		return FALSE;
	}

	BOOL operator == (CAllowedPlayer player)
	{
		// = Steam ID , or = name + password
		return (player.IsForSteamID(m_szSteamId) || (player.IsForName(m_szName) && player.IsForPass(m_szPass)));
	}

	BOOL IsForName(const char* szName) const
	{
		return (strcmp(szName, m_szName) == 0);
	}

	BOOL IsForPass(const char* szPass /*const unsigned int iWonId*/) const
	{
		return FStrEq(m_szPass, szPass);//((unsigned int)m_iWonid == (unsigned int)iWonId);
	}

	BOOL IsForClient(CClient* pClient);

	void GiveClientAccess(CClient* pClient)
	{
		pClient->SetAccessLevel(m_iAccessLevel);
	}

	void ShowUser(edict_t* pEntity)
	{
		BotMessage(pEntity, 0, "name=\"%s\", pass=\"%s\", accesslevel=%d, steamID=\"%s\"", m_szName, m_szPass, m_iAccessLevel, m_szSteamId);
	}

	//	unsigned int GetWonId ( void )
	//	{
	//		return m_iWonid;
	//	}

	const char* GetName(void)
	{
		return m_szName;
	}

private:
	char m_szName[64];
	char m_szPass[BOT_MAX_PASSWORD_LEN];
	int m_iAccessLevel;
	char m_szSteamId[STEAM_ID_LEN];
};

// list of bot users
class CAllowedPlayers
{
public:

	CAllowedPlayers()
	{
		memset(this, 0, sizeof(CAllowedPlayers));
	}

	void ShowUsers(edict_t* pEntity)
	{
		dataStack<CAllowedPlayer> tempStack = m_AllowedPlayers;
		CAllowedPlayer* pPlayer;

		while (!tempStack.IsEmpty())
		{
			pPlayer = tempStack.ChoosePointerFromStack();

			pPlayer->ShowUser(pEntity);
		}
	}

	void AddPlayer(const char* szName, char* szPass, int iAccessLevel, char* szSteamId)
	{
		CAllowedPlayer PlayerToAdd = CAllowedPlayer(szName, szPass, iAccessLevel, szSteamId);

		// remove old one if it exists
		m_AllowedPlayers.Remove(PlayerToAdd);

		// add new
		m_AllowedPlayers.Push(PlayerToAdd);
	}

	void RemovePlayer(const char* szName, char* szPass, char* szSteamId)
	{
		m_AllowedPlayers.Remove(CAllowedPlayer(szName, szPass, 0, szSteamId));
	}

	CAllowedPlayer* GetPlayer(CClient* pClient)
	{
		dataStack<CAllowedPlayer> tempStack = m_AllowedPlayers;
		CAllowedPlayer* Player;

		while (!tempStack.IsEmpty())
		{
			Player = tempStack.ChoosePointerFromStack();

			if (Player->IsForClient(pClient))
			{
				tempStack.Init();

				return Player;
			}
		}

		return NULL;
	}

	void Destroy(void)
	{
		//this->WriteToFile();
		m_AllowedPlayers.Destroy();
	}
private:
	dataStack<CAllowedPlayer> m_AllowedPlayers;
};

////////////////////////////////////
// COMMANDS

// A bot command
// each bot command is a subclass of this in bot_commands.h
class CBotCvar
{
public:

	CBotCvar()
	{
		m_szCvarName = NULL;
	}

	inline const char* GetCommandName(void)
	{
		return m_szCvarName;
	}

	virtual eBotCvarState action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
	{
		return BOT_CVAR_ERROR;
	}

	BOOL error()
	{
		return (!m_bCanUseOnDedicatedServer && IS_DEDICATED_SERVER());
	}

	void setupCommand(const char* szCvarName, int iAccessLevel, BOOL bCanUseOnDedicatedServer = TRUE);

	BOOL needAccess(int iAccessLevel)
	{
		if (m_iAccessLevel > 0)
			return ((iAccessLevel & m_iAccessLevel) == 0);

		return FALSE;
	}

	BOOL IsCvar(const char* szCvarName)
	{
		if (szCvarName == m_szCvarName)
			return TRUE; // Same pointers, same string

		// other wise do a string compare
		return (strcmp(szCvarName, m_szCvarName) == 0);
	}

	virtual void showHelp(edict_t* pEntity)
	{
		BotMessage(pEntity, 0, "There is no help for this command");
		return;
	}

private:

	int m_iAccessLevel;
	char* m_szCvarName;
	BOOL m_bCanUseOnDedicatedServer;
};

class CBotCvarList
{
public:

	void Destroy(void)
	{
		dataStack<CBotCvar*> l_tempCvars = m_Cvars;

		CBotCvar* l_Cvar;

		while (!l_tempCvars.IsEmpty())
		{
			l_Cvar = l_tempCvars.ChooseFromStack();

			delete l_Cvar;
		}

		m_Cvars.Destroy();
	}

	void AddCvar(CBotCvar* theCommand);

	/*eBotCvarState CallCvarFunction ( const char *szCvarName, int iAccessLevel, CClient *curClient, const char *arg1,const char *arg2,const char *arg3,const char *arg4 )
	{
		CBotCvar *tempCvar = GetCvar(szCvarName);

		if ( tempCvar )
			return tempCvar->callCommand(curClient,arg1,arg2,arg3,arg4,iAccessLevel);

		return BOT_CVAR_NOTEXIST;
	}*/

	CBotCvar* GetCvar(const char* szCvarName)
	{
		if (szCvarName == NULL)
			return NULL;

		dataStack<CBotCvar*> l_tempCvars = m_Cvars;

		CBotCvar* l_Cvar;

		while (!l_tempCvars.IsEmpty())
		{
			l_Cvar = l_tempCvars.ChooseFromStack();

			if (l_Cvar->IsCvar(szCvarName))
			{
				l_tempCvars.Init();

				return l_Cvar;
			}
		}

		return NULL;
	}

	void PrintCommands(edict_t* pEdict)
	{
		dataStack<CBotCvar*> tempStack = m_Cvars;
		CBotCvar* pCommand;

		while (!tempStack.IsEmpty())
		{
			pCommand = tempStack.ChooseFromStack();

			BotMessage(pEdict, 0, "\"%s\"", pCommand->GetCommandName());
		}
	}

private:
	dataStack<CBotCvar*> m_Cvars;
};

struct SBotChatMessages
{
	/////////////////////////////////
	// REPUTATION CHAT
	dataUnconstArray<char*> m_Laugh[3];		// "hehehe!"
	dataUnconstArray<char*> m_Thanks[3];	// says thanks
	dataUnconstArray<char*> m_Greetings[3]; // welcomes
	dataUnconstArray<char*> m_Killed[3];	// gets killed
	dataUnconstArray<char*> m_Kills[3];		// kills someone
	dataUnconstArray<char*> m_LeaveGame[3]; // leaving game

	/////////////////////////////////
	// NON-REPUTATION CHAT
	dataUnconstArray<char*> m_Idle;			// chit chat
	dataUnconstArray<char*> m_Help;			// asking for help
};

class CModInfo
{
public:

	CModInfo()
	{
		m_szModFolder[0] = 0;
		m_szDllFile[0] = 0;
		m_iModId = 0;
	}

	CModInfo(const char* szModFolder, const char* szDllFile, int iModId)
	{
		strcpy(m_szModFolder, szModFolder);
		strcpy(m_szDllFile, szDllFile);
		m_iModId = iModId;
	}

	BOOL IsForMod(const char* szFolder)
	{
		return (strcmpi(szFolder, m_szModFolder) == 0);
	}

	int GetModId(void)
	{
		return m_iModId;
	}

	const char* GetDllFileName(const char* szFolder);

private:
	char m_szModFolder[16];
	char m_szDllFile[16];
	int m_iModId;
};

class CModInfos
{
public:
	//
	// Add a supported Modification
	//
	// need to specifiy the Mod's folder
	// and the dll file name to hook
	// and the mod id to set after successfully hooking the dll file.
	//
	void AddMod(const char* szModFolder, const char* szDllFile, int iModId)
	{
		m_Mods.Push(CModInfo(szModFolder, szDllFile, iModId));
	}

	// return the CModInfo structure
	// of the given mod folder
	CModInfo* GetModInfo(const char* szModFolder)
	{
		dataStack<CModInfo> tempStack;

		CModInfo* pInfo;

		tempStack = m_Mods;

		while (!tempStack.IsEmpty())
		{
			pInfo = tempStack.ChoosePointerFromStack();

			if (pInfo->IsForMod(szModFolder))
			{
				tempStack.Init();

				return pInfo;
			}
		}

		return NULL;
	}

	// at the end, free memory
	void DestroyInfo(void)
	{
		m_Mods.Destroy();
	}
private:
	dataStack<CModInfo> m_Mods;
};

// ------------ NS HIVE INFO --------------

typedef struct
{
	edict_t* pEntity;
	string_t szMaster;
}entity_master_t;

typedef struct
{
	Vector vOrigin;

	int mStatus;
	int mUnderAttack;
	int mTechnology;

	edict_t* pHive;
}hive_info_t;

#define BOT_MAX_HIVES 3

///////////////////////////
// MASTER STUFF
//
// made this myself to get the state of doors that use masters
//
typedef enum { MASTER_NONE, MASTER_NOT_TRIGGERED, MASTER_TRIGGERED, MASTER_FAULT } eMasterType;

class CMasterEntity
{
public:
	CMasterEntity(edict_t* pEntity, char* szMasterName)
	{
		m_pEntity = pEntity;
		m_szMasterName = new char[strlen(szMasterName) + 1];
		strcpy(m_szMasterName, szMasterName);
	}

	~CMasterEntity()
	{
		delete m_szMasterName;
		m_szMasterName = NULL;
		m_pEntity = NULL;
	}

	BOOL IsForEntity(edict_t* pEntity)
	{
		return (m_pEntity == pEntity);
	}

	eMasterType CanFire(edict_t* pActivator);

	edict_t* FindButton(Vector vOrigin);

private:
	char* m_szMasterName;
	edict_t* m_pEntity;
};

// list of all entities that use masters
class CMasterEntities
{
public:
	void FreeLocalMemory(void)
	{
		dataStack<CMasterEntity*> temp;

		temp = m_Masters;

		while (!temp.IsEmpty())
		{
			CMasterEntity* m = temp.ChooseFromStack();

			delete m;
			m = NULL;
		}

		m_Masters.Destroy();
	}

	void AddMaster(edict_t* pEntity, char* szMasterName)
	{
		m_Masters.Push(new CMasterEntity(pEntity, szMasterName));
	}

	eMasterType EntityCanFire(edict_t* pEntity, edict_t* pActivator)
	{
		CMasterEntity* pMaster;

		pMaster = GetMaster(pEntity);

		if (pMaster)
			return pMaster->CanFire(pActivator);

		return MASTER_NONE;
	}

	edict_t* GetButtonForEntity(edict_t* pEntity, Vector const vOrigin)
	{
		CMasterEntity* pMaster;

		pMaster = GetMaster(pEntity);

		if (pMaster)
			return pMaster->FindButton(vOrigin);

		return NULL;
	}

private:

	CMasterEntity* GetMaster(edict_t* pEntity)
	{
		dataStack<CMasterEntity*> tempStack;
		CMasterEntity* pMaster;

		tempStack = m_Masters;

		while (!tempStack.IsEmpty())
		{
			pMaster = tempStack.ChooseFromStack();

			if (pMaster->IsForEntity(pEntity))
			{
				tempStack.Init();

				return pMaster;
			}
		}

		return NULL;
	}

	dataStack<CMasterEntity*> m_Masters;
};

//////////////////////////
// NS STRUCTURE STUFF
//////////////////////////
//
// Use this for emualting the hivemind to check if alien
// structures are under attack.
//
class CStructure
{
public:

	CStructure()
	{
		m_pEntity = NULL;
		m_fEndAttack = 0;
		m_fPrevHealth = 0;
	}

	CStructure(edict_t* pEdict)
	{
		m_pEntity = pEdict;
		m_fEndAttack = 0;
		m_fPrevHealth = m_pEntity->v.health;
	}

	BOOL IsUnderAttack(void)
	{
		return (m_fEndAttack > gpGlobals->time);
	}

	void UnderAttack(void)
	{
		// set under attack for 1 second
		m_fEndAttack = gpGlobals->time + 1.0;
	}

	void Update(void)
	{
		if (m_pEntity && !FNullEnt(m_pEntity))
		{
			// see if its underattack and update previouse health
			// to see if the health has changed next time.
			if (m_fPrevHealth > m_pEntity->v.health)
				UnderAttack();
			m_fPrevHealth = m_pEntity->v.health;
		}
		else
			m_fPrevHealth = 0;
	}

	BOOL IsValid(void)
	{
		return (m_pEntity && !m_pEntity->free &&
			m_pEntity->v.health && (EntityIsAlienStruct(m_pEntity) || EntityIsMarineStruct(m_pEntity)));
	}

	int GetStructureType(void)
	{
		return m_pEntity->v.iuser3;
	}

	edict_t* GetEntity(void)
	{
		return m_pEntity;
	}

	BOOL operator == (CStructure pStruct)
	{
		return (m_pEntity == pStruct.GetEntity());
	}

private:

	float m_fEndAttack;
	float m_fPrevHealth;
	edict_t* m_pEntity;
};

//
// use a hash table of stacks
// to store hive mind entities
// for quick additions and member
// checks
//
#define ALIEN_STRUCT_HASH_MAX 10

class CStructures
{
public:

	void AddStructure(edict_t* pStructure, int iHashVal)
	{
		int iIndex = iHashVal % ALIEN_STRUCT_HASH_MAX;

		if (!m_Structures[iIndex].IsMember(pStructure))
			m_Structures[iIndex].Push(CStructure(pStructure));
	}

	void FreeLocalMemory(void)
	{
		int i;

		for (i = 0; i < ALIEN_STRUCT_HASH_MAX; i++)
			m_Structures[i].Destroy();
	}

	// keep checking hurt structures
	edict_t* Tick(int* iBestPriority)
	{
		dataStack<CStructure> tempStack;

		CStructure* pStructure;

		int iStructure = 0;
		int iPriority = 0;

		int i;

		edict_t* pUnderAttackStruct = NULL;

		for (i = 0; i < ALIEN_STRUCT_HASH_MAX; i++)
		{
			tempStack = m_Structures[i];

			while (!tempStack.IsEmpty())
			{
				pStructure = tempStack.ChoosePointerFromStack();

				if (pStructure->IsValid())
				{
					pStructure->Update();

					if (pStructure->IsUnderAttack())
					{
						iStructure = pStructure->GetStructureType();

						switch (iStructure)
						{
							// Different priorities for different structures under attack
							// this will determine how many aliens will rush to the structure
							// to defend it.
						case AVH_USER3_HIVE:
							iPriority = 6; // maximum for hive.
							break;
						case AVH_USER3_DEFENSE_CHAMBER:
							iPriority = 4;
							break;
						case AVH_USER3_MOVEMENT_CHAMBER:
							iPriority = 2;
							break;
						case AVH_USER3_OFFENSE_CHAMBER:
							iPriority = 3;
							break;
						case AVH_USER3_SENSORY_CHAMBER:
							iPriority = 1;
							break;
						case AVH_USER3_ALIENRESTOWER:
							iPriority = 5;
							break;
						case AVH_USER3_COMMANDER_STATION:
							iPriority = 10; // max for command console
							break;
						case AVH_USER3_TURRET_FACTORY:
						case AVH_USER3_ADVANCED_TURRET_FACTORY:
							iPriority = 9;
							break;
						case AVH_USER3_INFANTRYPORTAL:
							iPriority = 8;
							break;
						default:
							iPriority = 0;
							break;
						}

						if (iPriority > * iBestPriority)
						{
							pUnderAttackStruct = pStructure->GetEntity();
							*iBestPriority = iPriority;
						}
					}
				}
				else
				{
					// Remove this
					m_Structures[i].RemoveByPointer(pStructure);

					// structure changed, restart
					tempStack = m_Structures[i];
				}
			}

			if (!tempStack.IsEmpty())
			{
				tempStack.Init();
			}
		}

		return pUnderAttackStruct;
	}

private:
	dataStack<CStructure> m_Structures[ALIEN_STRUCT_HASH_MAX];
};

////////////////////////
// NS GORGE STUFF
////////////////////////
//
// stuff for the gorge
// stores what it needs to build and how many
// it can build etc.
//
class CStructToBuild
{
public:
	CStructToBuild()
	{
		m_iAmountOf = 0;
		m_iPriority = 0;
	}

	CStructToBuild(int iAmount, int iPriority)
	{
		m_iAmountOf = iAmount;
		m_iPriority = iPriority;
	}

	int m_iAmountOf;
	int m_iPriority;
};

class CThingToBuild
{
public:
	CThingToBuild()
	{
		m_iOffs = CStructToBuild();
		m_iDefs = CStructToBuild();
		m_iMovs = CStructToBuild();
		m_iSens = CStructToBuild();
	}

	CThingToBuild(int iOffs, int iDefs, int iMovs, int iSens)
	{
		m_iOffs.m_iAmountOf = iOffs;
		m_iDefs.m_iAmountOf = iDefs;
		m_iMovs.m_iAmountOf = iMovs;
		m_iSens.m_iAmountOf = iSens;
	}

	void GetThingsToBuild(int* iOffs, int* iDefs, int* iMovs, int* iSens)
	{
		*iOffs = m_iOffs.m_iAmountOf;
		*iDefs = m_iDefs.m_iAmountOf;
		*iMovs = m_iMovs.m_iAmountOf;
		*iSens = m_iSens.m_iAmountOf;
	}

	CStructToBuild m_iOffs;
	CStructToBuild m_iDefs;
	CStructToBuild m_iMovs;
	CStructToBuild m_iSens;
};

class CThingsToBuild
{
public:
	CThingsToBuild()
	{
		m_forHive = CThingToBuild(4, 2, 1, 1);
		m_forResTower = CThingToBuild(2, 2, 1, 1);
		m_forOffenseChamber = CThingToBuild(0, 1, 0, 0);
		m_forDefenseChamber = CThingToBuild(0, 0, 0, 0);
		m_forSensoryChamber = CThingToBuild(0, 0, 0, 0);
		m_forMovementChamber = CThingToBuild(0, 0, 0, 0);
	}

	void Clear(void)
	{
		memset(this, 0, sizeof(CThingsToBuild));
	}

	CThingToBuild m_forHive;
	CThingToBuild m_forResTower;
	CThingToBuild m_forOffenseChamber;
	CThingToBuild m_forDefenseChamber;
	CThingToBuild m_forSensoryChamber;
	CThingToBuild m_forMovementChamber;
};

typedef enum
{
	BOTCAM_NONE = 0,
	BOTCAM_BOT,
	BOTCAM_ENEMY,
	BOTCAM_WAYPOINT,
	BOTCAM_FP
}eCamLookState;

// one bot cam, other players can tune into it
class CBotCam
{
public:

	CBotCam();

	void Spawn();

	void Think();

	void Clear();

	BOOL TuneIn(edict_t* pPlayer);

	void TuneOff(edict_t* pPlayer);

	BOOL IsWorking();

	BOOL BotHasEnemy()
	{
		if (!m_pCurrentBot)
			return FALSE;

		return (m_pCurrentBot->m_pEnemy != NULL);
	}

	void SetCurrentBot(CBot* pBot)
	{
		m_pCurrentBot = pBot;
		m_fNextChangeBotTime = gpGlobals->time + RANDOM_FLOAT(5.0, 7.5);
		m_fNextChangeState = gpGlobals->time;
	}

private:
	CBot* m_pCurrentBot;
	eCamLookState m_iState;
	edict_t* m_pCameraEdict;
	float m_fNextChangeBotTime;
	float m_fNextChangeState;
	BOOL m_bTriedToSpawn;
	//float m_fThinkTime;
	TraceResult tr;
	int m_iPositionSet;
	Vector vBotOrigin;

	//HudText m_Hudtext;
	//BOOL m_TunedIn[MAX_PLAYERS];
};

class CTFCGoal
{
public:

	void init()
	{
		memset(this, 0, sizeof(CTFCGoal));
	}

	CTFCGoal()
	{
		init();
	}

	CTFCGoal(edict_t* pEntity)
	{
		init();
		m_pEntity = pEntity;
	}

	inline BOOL isEdict(edict_t* pEdict)
	{
		return (m_pEntity == pEdict);
	}

	inline void setTeam(int team)
	{
		m_iTeam = team;
	}

	inline BOOL isForTeam(int team)
	{
		return m_iTeam == team;
	}

	inline void setGoal(int goal)
	{
		m_iGoal = goal;
	}

	inline void setGroup(int group)
	{
		m_iGroup = group;
	}

	inline int getGroup()
	{
		return m_iGroup;
	}

	inline int getTeam()
	{
		return m_iTeam;
	}

	inline int getGoal()
	{
		return m_iGoal;
	}

	inline BOOL isForGroup(int iGroup)
	{
		return m_iGroup;
	}

	inline BOOL isForGoal(int goal)
	{
		return m_iGoal == goal;
	}

	inline edict_t* edict()
	{
		return m_pEntity;
	}

private:
	edict_t* m_pEntity;
	int m_iTeam;
	int m_iGoal;
	int m_iGroup;
};

/*class CTFCFlag : public CTFCGoal
{
public:
	void init ()
	{
		memset(this,0,sizeof(CTFCFlag));
	}

	CTFCFlag()
	{
		init();
	}

	CTFCFlag ( edict_t *pEntity )
	{
		init();
		m_pEntity = pEntity;
	}

	inline BOOL isEdict ( edict_t *pEdict )
	{
		return (m_pEntity == pEdict);
	}

	inline void setTeam ( int team )
	{
		m_iTeam = team;
	}

	inline BOOL isForTeam ( int team )
	{
		return m_iTeam == team;
	}

	inline void setGoal ( int goal )
	{
		m_iGoal = goal;
	}

	inline BOOL isGroup ( int group )
	{
		return m_iGroup == group;
	}

	inline void setGroup ( int group )
	{
		m_iGroup = group;
	}

	inline int getGroup()
	{
		return m_iGroup;
	}

	inline int getTeam()
	{
		return m_iTeam;
	}

	inline int getGoal()
	{
		return m_iGoal;
	}

private:
	edict_t *m_pEntity;
	int m_iTeam;
	int m_iGoal;
	int m_iGroup;
};*/

class CTFCFlags
{
public:

	void FreeMemory()
	{
		m_Flags.Destroy();
		m_Flags.Init();
	}

	CTFCGoal* addFlag(edict_t* pFlag)
	{
		m_Flags.Push(CTFCGoal(pFlag));

		return m_Flags.GetHeadInfoPointer();
	}

	void removePrevFlag()
	{
		m_Flags.Pop();
	}

	edict_t* getFlag(int team, int active_goal)
	{
		return NULL;
	}

	BOOL isFlag(edict_t* pFlag, int team, BOOL bEnemyFlag = FALSE)
	{
		dataStack<CTFCGoal> tempStack = m_Flags;
		CTFCGoal* pGotFlag;

		BOOL bValid;

		while (!tempStack.IsEmpty())
		{
			pGotFlag = tempStack.ChoosePointerFromStack();

			if (pGotFlag->isEdict(pFlag))
			{
				bValid = FALSE;

				if (bEnemyFlag) // for demoman, e.g., to pipe enemy flags
					bValid = (pGotFlag->getTeam() && !pGotFlag->isForTeam(team));
				else
					bValid = (pGotFlag->isForTeam(0) || pGotFlag->isForTeam(team));

				if (bValid)
				{
					tempStack.Init();

					return TRUE;
				}
			}
		}

		return FALSE;
	}

	BOOL getFlagInfo(edict_t* pFlag, int* group, int* goal, int* team)
	{
		dataStack<CTFCGoal> tempStack = m_Flags;
		CTFCGoal* pGotFlag;

		while (!tempStack.IsEmpty())
		{
			pGotFlag = tempStack.ChoosePointerFromStack();

			if (pGotFlag->isEdict(pFlag))
			{
				*group = pGotFlag->getGroup();
				*team = pGotFlag->getTeam();
				*goal = pGotFlag->getGoal();

				tempStack.Init();

				return TRUE;
			}
		}

		return FALSE;
	}

	edict_t* getRandomHeldFlagByTeam(int team)
	{
		dataStack<CTFCGoal> tempStack = m_Flags;
		CTFCGoal* pGotFlag;

		while (!tempStack.IsEmpty())
		{
			pGotFlag = tempStack.ChoosePointerFromStack();

			edict_t* pFlag = pGotFlag->edict();

			if (pFlag->v.owner && (UTIL_GetTeam(pFlag->v.owner) == team))
			{
				tempStack.Init();
				return pFlag;
			}
		}

		return NULL;
	}

	BOOL playerHasFlag(edict_t* pPlayer)
	{
		dataStack<CTFCGoal> tempStack = m_Flags;
		CTFCGoal* pGotFlag;

		while (!tempStack.IsEmpty())
		{
			pGotFlag = tempStack.ChoosePointerFromStack();

			edict_t* pFlag = pGotFlag->edict();

			if (pFlag->v.owner == pPlayer)
			{
				tempStack.Init();
				return TRUE;
			}
		}

		return FALSE;
	}

private:
	dataStack<CTFCGoal> m_Flags;
};

/*class CTFCCapturePoint : public CTFCGoal
{
public:

	void init ()
	{
		memset(this,0,sizeof(CTFCCapturePoint));
	}

	CTFCCapturePoint()
	{
		init();
	}

	CTFCCapturePoint(edict_t *pEntity)
	{
		init();
		m_pEntity = pEntity;
	}

	inline void setGoal ( int goal )
	{
		m_iGoal = goal;
	}

	inline int getGoal ()
	{
		return m_iGoal;
	}

	inline void setTeam ( int team )
	{
		m_iTeam = team;
	}

	inline BOOL isForTeam ( int team )
	{
		return (m_iTeam == team);
	}

	inline BOOL isEdict ( edict_t *pEdict )
	{
		return (m_pEntity == pEdict);
	}

	inline BOOL isForGroup ( int iGroup )
	{
		return m_iGroup;
	}

	inline void setGroup ( int group )
	{
		m_iGroup = group;
	}

	inline BOOL isForGoal ( int goal )
	{
		return m_iGoal == goal;
	}

	inline edict_t *edict ()
	{
		return m_pEntity;
	}

private:
	int m_iGoal;
	int m_iGroup;
	edict_t *m_pEntity;
	int m_iTeam;
};*/

class CTFCCapturePoints
{
public:

	void FreeMemory()
	{
		m_CapPoints.Destroy();
		m_CapPoints.Init();
	}

	CTFCGoal* addCapturePoint(edict_t* cap_point)
	{
		m_CapPoints.Push(CTFCGoal(cap_point));

		return m_CapPoints.GetHeadInfoPointer();
	}

	void removePrevCapPoint()
	{
		m_CapPoints.Pop();
	}

	edict_t* getCapturePoint(int group, int goal, int team, BOOL noInfo = FALSE);

private:
	dataStack<CTFCGoal> m_CapPoints;
};
/*
class CTFCSpawn
{
public:

private:
	int team;
	int active;
};

class CTFCSpawns
{
public:

private:
};*/

class CBotNSTech
{
public:

	CBotNSTech()
	{
		m_iCost = 0;
		m_iSlot = 0;
		m_iRad = 0;
		m_bAvailable = FALSE;
		m_iMessageId = (AvHMessageID)0;

		return;
	}

	CBotNSTech(AvHMessageID iId, short int iCost, short int bAvailable, short int iRadius, short int iSlot)
	{
		m_iCost = iCost;
		m_iSlot = iSlot;
		m_bAvailable = bAvailable;
		m_iMessageId = iId;
		m_iRad = iRadius;
	}

	inline BOOL operator == (CBotNSTech const other)
	{
		return other.getID() == getID();
	}

	inline AvHMessageID getID() const
	{
		return m_iMessageId;
	}

	BOOL getAvailable() const
	{
		return m_bAvailable;
	}

	int getCost() const
	{
		return (int)m_iCost;
	}

	void update(CBotNSTech const tech)
	{
		m_bAvailable = tech.getAvailable();
		m_iCost = tech.getCost();
	}

private:
	short int m_iCost;
	short int m_iSlot;
	short int m_iRad;
	BOOL m_bAvailable;
	AvHMessageID m_iMessageId;
};

class CBotNSTechs
{
public:
	void addTech(CBotNSTech tech)
	{
		if (!m_Techs.IsMember(tech))
			m_Techs.Add(tech);
		else
		{
			CBotNSTech* pExist = m_Techs.getExisting(tech);

			if (pExist)
			{
				pExist->update(tech);
			}
		}
	}

	void getTechs(int iMaxCost, dataUnconstArray<CBotNSTech*> pvTechs)
	{
		for (int i = 0; i < m_Techs.Size(); i++)
		{
			if (m_Techs[i].getCost() < iMaxCost)
				pvTechs.Add(&(m_Techs[i]));
		}
	}

	void freeMemory()
	{
		m_Techs.Clear();
	}

private:
	dataUnconstArray<CBotNSTech> m_Techs;
};

//
// GLOBALS
// store all bots etc in here
//
class CBotGlobals
{
public:

	CBot* otherBotAtWaypoint(CBot* pBot, int iWpt)
	{
		int i = 0;
		CBot* pOtherBot;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			pOtherBot = &m_Bots[i];

			if (pOtherBot == pBot)
				continue;

			if (pOtherBot->m_iCurrentWaypointIndex == iWpt)
				return pOtherBot;
		}

		return NULL;
	}

	void saveLearnedData();
	void loadLearnedData();

	void printBotBoredom(edict_t* pEdictTo)
	{
		int i;

		CBot* pBot;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			pBot = &m_Bots[i];

			if (pBot->IsUsed())
			{
				pBot->printBoredoms(pEdictTo);
			}
		}
	}
	void ReadBotFolder(void);

	inline TraceResult* Traceline(Vector& vecStart, Vector& vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t* pentIgnore)
	{
		UTIL_TraceLine(vecStart, vecEnd, igmon, ignoreGlass, pentIgnore, &m_tr);

		return &m_tr;
	}

	BOOL NetMessageStarted(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);

	void ReadThingsToBuild(void);

	BOOL IsConfigSettingOn(int iConfigSetting)
		// returns a True value if the config setting iConfigSetting
		// is On, False if Off..
	{
		return ((m_iConfigSettings & iConfigSetting) > 0);
	}

	void ChangeConfigSetting(int iConfigSetting, int State)
		// Put the config setting iConfigSetting on or off depending on iState
	{
		if (State == 0)
			m_iConfigSettings &= ~iConfigSetting;
		else
			m_iConfigSettings |= iConfigSetting;
	}

	void StartFrame(void);

	void ReadConfig(void);

	// Free stuff after game is finished / DLL is detached
	void FreeGlobalMemory(void);

	// Free stuff after map changes
	void FreeLocalMemory(void);

	// initializing on map change
	void MapInit(void);

	//quicker "is NS" test
	inline BOOL IsNS(void)
	{
		return m_bIsNS;
	}

	BOOL m_bIsNS;

	// get number of clients trying to join
	// can sometimes be buggy because of HL's clientDisconnect
	int GetNumJoiningClients(void)
	{
		int i;
		int iNum;

		iNum = 0;

		for (i = 0; i < MAX_PLAYERS; i++)
			iNum += m_iJoiningClients[i];

		return iNum;
	}

	inline edict_t* GetCommander(void)
	{
		return m_pCommander.Get();
	}

	inline void SetCommander(edict_t* pComm)
	{
		m_pCommander.Set(pComm);
	}

	BOOL IsCombatMap()
	{
		return m_bCombatMap;
	}

	void Init(void)
		// init, happens very first thing. nearly.
	{
		memset(&m_Squads, 0, sizeof(CBotSquads));
		memset(m_iJoiningClients, 0, sizeof(int) * MAX_PLAYERS);

		memset(m_iTeamScores, 0, sizeof(int) * MAX_TEAMS);
		//m_szWelcomeMessage = NULL;
		//m_bIsNs3final = TRUE;

		setDefaults();

		/////////////////////////////////////////////////
		// ADD AVAILABLE MODS HERE
		/////////////////////////////////////////////////
#ifndef __linux__
		// WIN32
		// Add mod folders and their .dll filenames.
		//
		m_Mods.AddMod("ios", "ios", MOD_IOS);
		m_Mods.AddMod("ns", "ns", MOD_NS);
		// steam ns 3 beta 1
		m_Mods.AddMod("nsp", "ns", MOD_NS);
		m_Mods.AddMod("TS", "mp", MOD_TS);
		m_Mods.AddMod("BumperCars", "hl", MOD_BUMPERCARS);
		m_Mods.AddMod("dmc", "dmc", MOD_DMC);
		m_Mods.AddMod("rc2", "rc2", MOD_RC2);
		m_Mods.AddMod("RockCrowbar", "rc", MOD_RC);
		m_Mods.AddMod("SvenCoop", "hl", MOD_SVENCOOP);
		m_Mods.AddMod("valve", "hl", MOD_HL_DM);
		m_Mods.AddMod("hlrally", "hlrally", MOD_HL_RALLY);
		m_Mods.AddMod("cstrike", "mp", MOD_COUNTERSTRIKE);
		m_Mods.AddMod("tfc", "tfc", MOD_TFC);
		m_Mods.AddMod("rspecies", "hl", MOD_RS); // rival species meh..
		//m_Mods.AddMod("gearbox","opfor",MOD_GEARBOX); // Support for OP4CTF [APG]RoboCop[CL]
#else
		// LINUX
		// Add mod folders and their name before the _i386 postfix of the so files.
		//
		m_Mods.AddMod("ios", "ios", MOD_IOS);
		// ns 2
		// .e.g in linux the ns dll is ns_i386.so and win32 it is ns.dll
		m_Mods.AddMod("ns", "ns", MOD_NS);
		// steam ns 3 beta 1
		m_Mods.AddMod("nsp", "ns", MOD_NS);
		m_Mods.AddMod("TS", "mp", MOD_TS);
		m_Mods.AddMod("BumperCars", "hl", MOD_BUMPERCARS);
		m_Mods.AddMod("dmc", "dmc", MOD_DMC);
		m_Mods.AddMod("rc2", "rc2", MOD_RC2);
		m_Mods.AddMod("RockCrowbar", "rc", MOD_RC);
		m_Mods.AddMod("SvenCoop", "hl", MOD_SVENCOOP);
		m_Mods.AddMod("valve", "hl", MOD_HL_DM);
		m_Mods.AddMod("hlrally", "hlr", MOD_HL_RALLY);
		m_Mods.AddMod("cstrike", "mp", MOD_COUNTERSTRIKE);
		m_Mods.AddMod("tfc", "tfc", MOD_TFC);
		m_Mods.AddMod("rspecies", "hl", MOD_RS);
		//m_Mods.AddMod("gearbox","opfor",MOD_GEARBOX); // Support for OP4CTF [APG]RoboCop[CL]

#endif
		// same for linux & windows
		m_Mods.AddMod("bg", "bg", MOD_BG);

		// game init stuff
		//GameInit();
	}

	void setDefaults()
	{
		//m_bGotTsObjectives = FALSE;
		m_pMessageEntity = NULL;
		m_pDebugMessage = NULL;
		m_fWallStickTol = BOT_WAYPOINT_TOUCH_DIST;

		m_iLanguage = BOT_LANG_ENGLISH;
		m_bIsNS = FALSE;

		//m_pTFCGroup = NULL;
		m_pTFCDetect = NULL;
		m_pTFCGoal = NULL;
		m_bCombatMap = FALSE;
		m_pMarineStart = NULL;
		//m_iJoiningClients = 0;
		m_fBotStuckSpeed = 7.0;
		m_iMaxVisUpdateRevs = 200;
		m_fReadConfigTime = 0;
		m_bAutoPathWaypoint = TRUE;
		m_bWaypointsHavePaths = FALSE;
		m_fUpdateVisTime = 0;
		m_iDebugLevels = 0;
		m_iBotMsgIndex = -1;
		m_iCurrentMessageState = 0;
		m_iCurrentMessageState2 = 0;

		m_bGameRules = FALSE;

		m_bIsFakeClientCommand = FALSE;
		m_fTurnSpeed = 15;

		m_CurrentMessage = NULL;

		m_fAutoBuildTime = 0;
		m_bAutoBuilt = FALSE;

		m_bNetMessageStarted = FALSE;

		m_CurrentHandledCvar = NULL;

		m_pListenServerEdict = NULL;

		m_iBotChatPercent = DEFAULT_BOT_CHAT_PERCENT;
		m_iBotChatReplyPercent = 33;

		// Set the bots array to 32 (max bots)
		//m_Bots.SetSize(32);

		//m_iNumClients = 0;

		m_fMapInitTime = 0;
		m_fBotRejoinTime = 0;

		m_fUpdateLadderTime = -1;

		m_szModFolder = NULL;

		m_iConfigSettings = BOT_CONFIG_TOOLTIPS | BOT_CONFIG_BLINKING;

		m_fGorgeAmount = 0.4;

		m_fAutoWaypointCheckTime = 0.5;

		m_uaBotModels.Init();

		m_sModelIndexFireball = 0;

		//		m_EntityMasters.Init();

		m_pCommander.Set(NULL);

		//	m_fBotAimUpdate = 1;
		//	m_fBotAimFactor = 1;

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

		m_iMinBots = -1;
		m_iMaxBots = -1;

		m_iMaxPathRevs = 200;

		m_bTeamPlay = FALSE;

		m_ThingsToBuild = NULL;

		m_currCapPoint = NULL;
		m_currFlag = NULL;

		prevBackPackInvalid = FALSE;
		prevCapturePointInvalid = FALSE;
		prevFlagInvalid = FALSE;

		m_fHiveImportance = 1.0f;
		m_fResTowerImportance = 0.7f;
		m_fHealingImportance = 0.75f;
		m_fStructureBuildingImportance = 0.4f;
		m_fRefillStructureImportance = 0.5f;
	}

	void LoadBotModels(void);

	CBotGlobals()
	{
		//Init();
	}

	inline void SayToolTip(edict_t* pEntity, eToolTip tooltip)
	{
		UTIL_BotToolTip(pEntity, m_iLanguage, tooltip);
	}

	inline BOOL IsMod(int iMod)
	{
		return m_iCurrentMod == iMod;
	}

	void SetupBotChat(void);

	const char* GetModInfo(void);

	inline BOOL IsDebugLevelOn(int iDebugLevel)
	{
		return ((m_iDebugLevels & iDebugLevel) > 0);
	}

	void KeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd);

	BOOL TFC_IsAvailableFlag(edict_t* pFlag, int team, BOOL bEnemyFlag = FALSE);

	inline BOOL TFC_teamsAreAllies(int team1, int team2)
	{
		return (team_allies[team1] & (1 << (team2 - 1)));
	}

	BOOL TFC_playerHasFlag(edict_t* pPlayer);
	BOOL TFC_getCaptureLocationForFlag(Vector* vec, edict_t* pFlag);

	int TFC_getTeamViaColorMap(edict_t* pEdict);
	int TFC_getBestClass(int prefclass, int iTeam);
	BOOL TFC_canGoClass(int iClass, int iTeam);
	int TFC_getBestTeam(int team);
	char* TFC_getClassName(int iClass);

	//short int m_iNumHivesAdded;

//	dataStack<entity_master_t> m_EntityMasters;
	//dataStack<entity_master_t> m_MultiManagers;

	CTypeVector<short int> TFCGoals[MAX_TEAMS];

	CBotSquads m_Squads;

	short int m_sModelIndexFireball;

	BOOL m_bWaypointsHavePaths;

	//float m_fBotAimUpdate;
	//float m_fBotAimFactor;

	CBotMenu m_Menus[BOT_MENU_MAX_ITEMS];

	int m_iConfigSettings;

	BOOL m_bAutoPathWaypoint;

	BOOL m_bTeamPlay;

	float m_fNextJoinTeam;

	float m_fAutoWaypointCheckTime;

	dataUnconstArray<char*> m_uaBotModels;

	float m_fAutoBuildTime;
	BOOL m_bAutoBuilt;

	CStructures m_HiveMind;
	// hack for marines
	CStructure m_CommConsole;

	BOOL m_bHasDefTech;
	BOOL m_bHasSensTech;
	BOOL m_bHasMovTech;

	float m_fWallStickTol;

	int m_iJoiningClients[MAX_PLAYERS];

	int m_iTeamScores[MAX_TEAMS];
	//int m_iJoiningClients;
	//int m_iNumClients;

	BOOL m_bNetMessageStarted;

	short int m_iMinBots;
	short int m_iMaxBots;

	eLanguage m_iLanguage;

	int m_iMaxVisUpdateRevs;

	float m_fHiveImportance;
	float m_fResTowerImportance;
	float m_fHealingImportance;
	float m_fStructureBuildingImportance;
	float m_fRefillStructureImportance;

	float m_fBotStuckSpeed;
	float m_fUpdateLadderTime;
	float m_fUpdateVisTime;

	CModInfos m_Mods;

	char* m_szModFolder;

	int m_iDebugLevels;
	//BOOL m_bGotTsObjectives;
	//BOOL m_bIsNs3final;

	hive_info_t m_Hives[BOT_MAX_HIVES];
	CBotNSTechs m_TeamTechs[MAX_TEAMS];
	edict_t* m_pMarineStart;

	float m_fReadConfigTime;

	short int m_iBotChatPercent;
	short int m_iBotChatReplyPercent;
	/////////////////////////////////
	// NET MESSAGES

	short int m_iBotMsgIndex;
	// State Machines
	short int m_iCurrentMessageState;
	// May require another, for some messages (inside loops)
	short int m_iCurrentMessageState2;

	int m_iMaxPathRevs;

	// LOCATIONS NOT REQUIRED IN NS 2.0
//	CLocations m_Locations; // locations available in map

	edict_t* m_pMessageEntity;
	CBotNetMessages m_NetEntityMessages;
	CBotNetMessages m_NetAllMessages;
	CAllowedPlayers m_BotUsers;

	// Natural selection, things to build .. (for aliens only so far)
	CThingsToBuild* m_ThingsToBuild;

	CBotNetMessage* m_CurrentMessage;
	CBotNetMessage* m_pDebugMessage;

	SBotChatMessages m_BotChat;

	CWeapons m_Weapons; // weapons available in game/mod
	CWeaponPresets m_WeaponPresets;

	////////////////////////////////
	// CVARS & COMMANDS

	CBotCvar* m_CurrentHandledCvar;
	CBotCvarList m_BotCvars;

	CMasterEntities m_Masters;

	BOOL m_bIsFakeClientCommand;
	short int m_iFakeArgCount;

	int m_iForceTeam;

	float m_fGorgeAmount;

	CBotCam m_BotCam;

	// Need to know who is running a listen server
	// so they have access to all commands
	edict_t* m_pListenServerEdict;

	////////////////////////////////////
	// STORED STRINGS

	// Stores a list of strings being used in a hash table so we can
	// point to them using pointers for, in the long run, less memory
	CStoredStrings m_Strings;

	///////////////////////////////////
	// USED TEMPLATES

	// Bots are global
	CBot m_Bots[MAX_PLAYERS];

	// Clients in the game (Real Players in metamod)
	//                     (Holds all players in non-metamod version)
	CClients m_Clients;

	int m_iWaypointTexture;

	BOOL m_bGameRules;

	//////////////////////////////////
	// OTHER

	float m_fClientUpdateTime;
	BOOL m_bBotCanRejoin;

	float m_fMapInitTime;
	float m_fBotRejoinTime;

	short int m_iNumBots;

	BOOL m_bCanUpgradeDef;
	BOOL m_bCanUpgradeSens;
	BOOL m_bCanUpgradeMov;

	TraceResult m_tr;

	short int m_iCurrentMod;

	CGA m_TSWeaponChoices;
	CGA m_pCombatGA[2];

	//CGA m_combatGAForTeam[MAX_TEAMS];
	CGA m_enemyCostGAsForTeam[MAX_TEAMS];
	//	CGA m_TFCspiesForTeam[MAX_TEAMS];

	edict_t* m_pTFCDetect;
	edict_t* m_pTFCGoal;

	void setMapType(eTFCMapType theMapType)
	{
		m_TFCMapType = theMapType;
	}

	int getMapType()
	{
		return m_TFCMapType;
	}

	BOOL isMapType(eTFCMapType theMapType)
	{
		return (theMapType == m_TFCMapType);
	}

	edict_t* findBackpack(Vector const location, int team, int min_health, int min_cells, int min_armor, int min_ammo)
	{
		return m_Backpacks.findBackpack(location, team, min_health, min_cells, min_armor, min_ammo);
	}

	void TFC_setOnlyGoal(int goal, int team)
	{
		m_iValidGoals[team - 1].Clear();
		m_iValidGoals[team - 1].Add(goal);
	}

	void TFC_addGoal(int goal, int team)
	{
		m_iValidGoals[team - 1].Add(goal);
	}

	BOOL TFC_isValidGoal(int goal, int team)
	{
		int i = 0;

		if (m_iValidGoals[team - 1].IsEmpty())
			return TRUE; // no goals stored as valid, just return TRUE (for normal maps e.g.)

		for (i = 0; i < m_iValidGoals[i].Size(); i++)
		{
			if (m_iValidGoals[team - 1].ReturnValueFromIndex(i) == goal)
				return TRUE;
		}

		return FALSE;
	}

	/*	void changeWelcomeMessage ( char *message )
		{
			if ( m_szWelcomeMessage )
				free(m_szWelcomeMessage);
			m_szWelcomeMessage = strdup(message);
		}*/

	void buildFileName(const char* in_filename, char* out_filename)
	{
		sprintf(out_filename, "%s%s", m_szBotFolder, in_filename);
	}

	const char* botFolder()
	{
		return m_szBotFolder;
	}

	edict_t* randomHeldFlagOnTeam(int team);
	float m_fTurnSpeed;
private:

	char m_szBotFolder[256];

	void GameInit(void);

	///////////
	// NS
	BOOL m_bCombatMap;
	MyEHandle m_pCommander;

	///////////
	// TFC
	eTFCMapType m_TFCMapType;

	int max_team_players[MAX_TEAMS];
	int team_class_limits[MAX_TEAMS];
	int team_allies[MAX_TEAMS];

	BOOL prevBackPackInvalid;
	BOOL prevCapturePointInvalid;
	BOOL prevFlagInvalid;

	CTFCBackpacks m_Backpacks;
	CTFCBackpack* m_currBackPack;

	CTFCCapturePoints m_CapPoints;
	CTFCGoal* m_currCapPoint;

	CTFCFlags m_Flags;
	CTFCGoal* m_currFlag;

	dataUnconstArray<int> m_iValidGoals[MAX_TEAMS];

	//char *m_szWelcomeMessage;
};

/*
#define EMPTY_STRING ""
#define LANG_STRING_ERROR "<lang-error>"

class CLanguage
{
	char *m_szName;
}

class CLanguages
{
public:
	AddMessage(
private:
	dataStack<char*> m_Messages[26];
}*/

const char* GetArg(const char* command, int arg_number);
void FakeClientCommand(edict_t* pFakeClient, const char* fmt, ...);

///////////////////////////////////////////////////////////////////
// POST BOT DEFINED FUNCTIONS
// (depend on structures defined above)

void BotFunc_InitProfile(bot_profile_t* bpBotProfile);
void BotFunc_ReadProfile(FILE* fp, bot_profile_t* bpBotProfile);
void BotFunc_WriteProfile(FILE* fp, bot_profile_t* bpBotProfile);
CBot* UTIL_GetBotPointer(const edict_t* pEdict);

BOOL BotFunc_EntityIsMoving(entvars_t* pev);
edict_t* BotFunc_FindNearestButton(Vector vOrigin, entvars_t* pDoor, Vector* vFoundOrigin = NULL);

////////////////////////////////////////////////////
// NAVIGATION

void BotTurnAtWall(CBot* pBot, TraceResult* tr);
BOOL BotCantMoveForward(CBot* pBot, TraceResult* tr);
BOOL BotCanJumpUp(CBot* pBot); // BotCanJumpUp : By : Botman
BOOL BotCanDuckUnder(CBot* pBot);
BOOL BotCheckWallOnLeft(CBot* pBot);
BOOL BotCheckWallOnRight(CBot* pBot);
const char* BotFunc_GetRandomPlayerName(CBot* pBot, int iState);

////////////////////////////////////////////////////
// ENGINE FUNCTIONS

void GameDLLInit(void);
int DispatchSpawn(edict_t* pent);
void DispatchThink(edict_t* pent);
void DispatchUse(edict_t* pentUsed, edict_t* pentOther);
void DispatchTouch(edict_t* pentTouched, edict_t* pentOther);
void DispatchBlocked(edict_t* pentBlocked, edict_t* pentOther);
void DispatchKeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd);
void DispatchSave(edict_t* pent, SAVERESTOREDATA* pSaveData);
int DispatchRestore(edict_t* pent, SAVERESTOREDATA* pSaveData, int globalEntity);
void DispatchObjectCollsionBox(edict_t* pent);
void SaveWriteFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount);
void SaveReadFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount);
void SaveGlobalState(SAVERESTOREDATA* pSaveData);
void RestoreGlobalState(SAVERESTOREDATA* pSaveData);
void ResetGlobalState(void);
BOOL ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
void ClientDisconnect(edict_t* pEntity);
void ClientKill(edict_t* pEntity);
void ClientPutInServer(edict_t* pEntity);
void ClientCommand(edict_t* pEntity);
void ClientUserInfoChanged(edict_t* pEntity, char* infobuffer);
void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
void ServerDeactivate(void);
void PlayerPreThink(edict_t* pEntity);
void PlayerPostThink(edict_t* pEntity);
void StartFrame(void);
void ParmsNewLevel(void);
void ParmsChangeLevel(void);
const char* GetGameDescription(void);
void PlayerCustomization(edict_t* pEntity, customization_t* pCust);
void SpectatorConnect(edict_t* pEntity);
void SpectatorDisconnect(edict_t* pEntity);
void SpectatorThink(edict_t* pEntity);
void Sys_Error(const char* error_string);
void PM_Move(struct playermove_s* ppmove, int server);
void PM_Init(struct playermove_s* ppmove);
char PM_FindTextureType(char* name);
void SetupVisibility(edict_t* pViewEntity, edict_t* pClient, unsigned char** pvs, unsigned char** pas);
void UpdateClientData(const struct edict_s* ent, int sendweapons, struct clientdata_s* cd);
int AddToFullPack(struct entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet);
void CreateBaseline(int player, int eindex, struct entity_state_s* baseline, struct edict_s* entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
void RegisterEncoders(void);
int GetWeaponData(struct edict_s* player, struct weapon_data_s* info);
void CmdStart(const edict_t* player, const struct usercmd_s* cmd, unsigned int random_seed);
void CmdEnd(const edict_t* player);
int ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
int GetHullBounds(int hullnumber, float* mins, float* maxs);
void CreateInstancedBaselines(void);
int InconsistentFile(const edict_t* player, const char* filename, char* disconnect_message);
int AllowLagCompensation(void);
void RCBot_ServerCommand(void);
int GetModId(void);
void GetGameDirectory(char* szDir);
float UTIL_GetAvoidAngle(edict_t* pEdict, Vector origin);
void ReadBotUsersConfig(void);
void BotFunc_MakeSquad(CClient* pClient);
void AssertMessage(BOOL bAssert, char* fmt, ...);
int UTIL_GetBuildWaypoint(Vector vSpawn, dataStack<int>* iFailedGoals = NULL);
int BotFunc_GetStructureForGorgeBuild(entvars_t* pGorge, entvars_t* pEntitypev);
void BotFunc_KickBotFromTeam(int iTeam);
BOOL BotFunc_BreakableIsEnemy(edict_t* pBreakable, edict_t* pEdict);
///////////////////////////////////////////////////////
// NAVIGATION
int BotNavigate_AStarAlgo(CBot* pBot, int iFrom, int iTo, BOOL bContinue);
int BotNavigate_FindNextWaypoint(CBot* pBot);
BOOL BotNavigate_UpdateWaypoint(CBot* pBot);
PATH* BotNavigate_FindPathFromTo(int iFrom, int iTo, int iTeam);

BOOL UTIL_makeTSweapon(edict_t* pOwner, eTSWeaponID weaponid);
#endif // BOT_H
