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

#ifdef _WIN32
#define strcmpi _strcmpi
#endif

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

typedef int bool;

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

//void DebugMessage(int iDebugLevel, edict_t* pEntity, int errorlevel, const char* fmt, ...);
//void BotMessage(edict_t* pEntity, int errorlevel, const char* fmt, ...);
//void BugMessage(edict_t* pEntity, const char* fmt, ...);

int GetPlayerRepId(const char* szPlayerName);
int GetPlayerEdictRepId(edict_t* pEdict);

//////////////////////////////
// BOT_H DEFINED FUNCTIONS
void BotFunc_ChangeAngles(float* fSpeed, const float* fIdeal, float* fCurrent, float* fUpdate);
bool UTIL_FriendlyHatesPlayer(edict_t* pEntity, edict_t* pPlayer);
edict_t* UTIL_getEntityInFront(edict_t* pEntity);
void ExplosionCreate(const Vector& center, const Vector& angles, edict_t* pOwner, float magnitude, bool doDamage);
Vector UTIL_GetGroundVector(edict_t* pEdict);
bool UTIL_EntityIsHive(const edict_t* pEdict);
int UTIL_CountEntitiesInRange(const char* classname, const Vector& vOrigin, float fRange);
int UTIL_SentryLevel(const edict_t* pEntity);
void RadiusDamage(Vector vecSrc, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
void UTIL_BotScreenShake(const Vector& center, float amplitude, float frequency, float duration, float radius);
edict_t* UTIL_GetPlayerByPlayerId(int id);
Vector UTIL_FurthestVectorAroundYaw(CBot* pBot);
int UTIL_ClassOnTeam(int iClass, int iTeam);
bool UTIL_EntityHasClassname(const edict_t* pEntity, const char* classname);
float UTIL_GetPlayerEnergy(const entvars_t* pev);
int UTIL_NS_GetMaxArmour(const edict_t* pEntity);
edict_t* UTIL_GetCommander();
int UTIL_CountEntities(const char* classname);
edict_t* BotFunc_FindRandomEntity(const char* szClassname);
bool UTIL_IsGrenadeRocket(const edict_t* pEntity);
//void BotFunc_StringCopy(char *, const char *);
bool BotFunc_IsLongRangeWeapon(int iId);
bool BotFunc_IncreaseRep(int iRep, float fInfo = 0.0f, float fSkill = 0.0f);
bool BotFunc_DecreaseRep(int iRep, float fInfo = 0.0f, float fSkill = 0.0f);
int BotFunc_GetRepArrayNum(int iRep);
void BotFunc_TraceToss(edict_t* ent, edict_t* ignore, TraceResult* tr);

CBaseEntity* CreateEnt(char* szName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner);

bool UTIL_PlayerStandingOnEntity(edict_t* pEntity, int team, edict_t* pIgnore = nullptr);

bool BotFunc_FillString(char* string, const char* fill_point, const char* fill_with, size_t max_len);

bool	EntityIsBuildable(const edict_t* pEdict);
bool    EntityIsWeldable(edict_t* pEntity);
bool	EntityIsAlive(const edict_t* pEdict);
bool	EntityIsMarine(edict_t* pEdict);
bool	EntityIsAlien(edict_t* pEdict);
bool    EntityIsCommander(const edict_t* pEdict);
bool    EntityIsSelectable(const edict_t* pEdict);
bool	EntityIsMarineStruct(const edict_t* pEdict);
bool	EntityIsAlienStruct(const edict_t* pEdict);

edict_t* BotFunc_NS_MarineBuild(int iUser3, const char* szClassname, Vector vOrigin, edict_t* pEntityUser = nullptr, bool bBuilt = false);
edict_t* BotFunc_NS_CommanderBuild(int iUser3, const char* szClassname, const Vector& vOrigin);
bool UTIL_IsResourceFountainUsed(const edict_t* pFountain);
//////////////////////////////
// UTIL FUNCTIONS

#ifndef RCBOT_META_BUILD
unsigned short FixedUnsigned16(float value, float scale);
short FixedSigned16(float value, float scale);
#endif
//int LookupActivity( void *pmodel, entvars_t *pev, int activity );
Vector AbsOrigin(const edict_t* pEdict);
bool UTIL_TankUsed(const edict_t* pTank);
void UTIL_BotHudMessageAll(const hudtextparms_t& textparms, const char* pMessage);
void UTIL_BotHudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage);
//unsigned short FixedUnsigned16( float value, float scale );
//short FixedSigned16( float value, float scale );
void UTIL_BotToolTip(edict_t* pEntity, eLanguage iLang, eToolTip iTooltip);

int strpos(char* pos, char* start);
void ReadMapConfig();
int RoundToNearestInteger(float fVal);
int Ceiling(float fVal);
bool UTIL_IsButton(const edict_t* pButton);
bool UTIL_CanStand(const Vector& origin, Vector* v_floor);

void UTIL_GetArgFromString(char** szString, char* szArg);

void UTIL_GetArgsFromString(char* szString, char* szArg1, char* szArg2, char* szArg3, char* szArg4);

edict_t* UTIL_FindEntityInSphere(edict_t* pentStart, const Vector& vecCenter,
	float flRadius);

edict_t* UTIL_FindEntityByString(edict_t* pentStart, const char* szKeyword,
	const char* szValue);

bool UTIL_AcceptablePushableVector(const edict_t* pPushable, const Vector& vOrigin);
float UTIL_EntityDistance(const entvars_t* pev, const Vector& vOrigin);
float UTIL_EntityDistance2D(const entvars_t* pev, const Vector& vOrigin);
float UTIL_GetBestPushableDistance(const edict_t* pPushable);
Vector UTIL_GetDesiredPushableVector(const Vector& vOrigin, const edict_t* pPushable);

//edict_t* UTIL_TFC_PlayerHasFlag(edict_t* pPlayer);

void UTIL_PlaySoundToAll(const char* szSound);
void UTIL_PlaySound(edict_t* pPlayer, const char* szSound);

void BotPrintTalkMessageOne(edict_t* pClient, const char* fmt, ...);

bool UTIL_FuncResourceIsOccupied(const edict_t* pFuncResource);
int NS_GetPlayerLevel(int exp);
int UTIL_GetNumClients(bool bReport = false);
edict_t* UTIL_FindEntityByTarget(edict_t* pentStart, const char* szName);
edict_t* UTIL_FindEntityByClassname(edict_t* pentStart, const char* szName);
edict_t* UTIL_FindEntityByTargetname(edict_t* pentStart, const char* szName);
void		UTIL_SayText(const char* pText, edict_t* pEdict);
void		UTIL_HostSay(edict_t* pEntity, int teamonly, char* message);
int		UTIL_GetTeam(edict_t* pEntity);
int	    UTIL_GetClass(edict_t* pEntity);
int		UTIL_GetBotIndex(const edict_t* pEdict);
int		UTIL_MasterTriggered(string_t sMaster, const CBaseEntity* pActivator);
void		UTIL_ShowMenu(edict_t* pEdict, int slots, int displaytime, bool needmore, const char* pText);
void		UTIL_BuildFileName(char* filename, const char* arg1, const char* arg2 = nullptr);

edict_t* UTIL_FacingEnt(edict_t* pPlayer, bool any = false);

void     UTIL_FixAngles(Vector* vAngles);
void     UTIL_FixFloatAngle(float* fAngle);
bool		UTIL_CanBuildHive(const entvars_t* pev);

float UTIL_EntityAnglesToVector2D(const entvars_t* pev, const Vector* pOrigin); // For 2d Movement
float UTIL_EntityAnglesToVector3D(const entvars_t* pev, const Vector* pOrigin);
edict_t* UTIL_FindNearestEntity(char** szClassnames, int iNames, const Vector& vOrigin, float fRange, bool bVisible, edict_t* pIgnore = nullptr);

Vector UTIL_LengthFromVector(const Vector& relation, float length);

bool	BotFunc_FInViewCone(const Vector* pOrigin, const edict_t* pEdict);
bool	BotFunc_FVisible(const Vector& vecOrigin, edict_t* pEdict);
Vector	Center(edict_t* pEdict);
Vector	GetGunPosition(const edict_t* pEdict);
Vector	EntityOrigin(const edict_t* pEdict);

edict_t* UTIL_CheckTeleEntrance(const Vector& vOrigin, edict_t* pExit, edict_t* pOwner);
edict_t* UTIL_CheckTeleExit(const Vector& vOrigin, edict_t* pOwner, edict_t* pEntrance);

Vector UTIL_AngleBetweenOrigin(const entvars_t* pev, const Vector& vOrigin);
bool UTIL_OnGround(const entvars_t* pev);
int     UTIL_TFC_getMaxArmor(edict_t* pEdict);
void	ClientPrint(edict_t* pEdict, int msg_dest, const char* msg_name);
float   UTIL_YawAngleBetweenOrigin(const entvars_t* pev, const Vector& vOrigin);
bool	UTIL_IsFacingEntity(const entvars_t* pev, const entvars_t* pevEntity);
float   UTIL_AngleBetweenVectors(const Vector& vec1, const Vector& vec2);
float	UTIL_AnglesBetweenEdictOrigin(const edict_t* pEdict, const Vector& origin);

int		UTIL_PlayersOnTeam(int iTeam);
int		UTIL_SpeciesOnTeam(int iSpecies, bool bIgnoreEmbryos = false);
int		UTIL_GetNumHives();
int		UTIL_CountBuiltEntities(const char* classname);
edict_t* UTIL_GetRandomUnbuiltHive();
edict_t* UTIL_FindRandomUnusedFuncResource(const CBot* pBot);
edict_t* UTIL_UpdateSounds(const entvars_t* pev);
edict_t* UTIL_FindPlayerByTruncName(const char* name);
void strlow(char* str);
void strhigh(char* str);
void UTIL_CountBuildingsInRange(const Vector& vOrigin, float fRange, int* iDefs, int* iOffs, int* iSens, int* iMovs);
float BotFunc_DistanceBetweenEdicts(const edict_t* pEdict1, const edict_t* pEdict2);

//float UTIL_AngleDiff(float destAngle, float srcAngle);

void BotPrintTalkMessage(const char* fmt, ...);

int BotFunc_GetBitSetOf(int iId);
void BotFile_Write(const char* string);

//////////////////////////////////////////////////////////////////////////////////
// END : FUNCTION PROTOTYPES

//TODO: To use `typedef struct sel_ammo` to reload properly? [APG]RoboCop[CL]
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

	bool operator == (CBotReputation Rep) const
	{
		return Rep.IsForPlayer(m_iPlayerRepId);
	}

	void UpdateRep(int iRep)
	{
		m_iRep = iRep;
	}

	int CurrentRep() const
	{
		return m_iRep;
	}

	int GetRepId() const
	{
		return m_iPlayerRepId;
	}

	bool IsForPlayer(int iPlayerRepId) const
	{
		return m_iPlayerRepId == iPlayerRepId;
	}

	void printRep(const CBot* forBot, edict_t* pPrintTo) const;

private:
	int m_iPlayerRepId;
	int m_iRep;
};

class CBotReputations
{
public:

	void printReps(CBot* forBot, edict_t* pPrintTo) const
	{
		dataStack<CBotReputation> tempStack = m_RepList;

		while (!tempStack.IsEmpty())
		{
			const CBotReputation* pRep = tempStack.ChoosePointerFromStack();

			pRep->printRep(forBot, pPrintTo);
		}
	}

	void Destroy()
	{
		m_RepList.Destroy();
	}

	CClient* GetRandomClient(int iRep) const;

	static void WriteToFile(int iBotProfile, CBotReputation* pRep);

	void RemoveSaveRep(int iBotProfile, int iPlayerRepId);

	void SaveAllRep(int iBotProfile) const;

	void AddLoadRep(int iBotProfile, int iPlayerRepId);

	void NewRep(int iPlayerRepId)
		// Create a NEW Reputation value for player name szPlayerName
		// With a default Rep of a Mid way rep.
	{
		AddRep(iPlayerRepId, BOT_MID_REP);
	}

	int AverageRepOnServer()
	{
		int iTotal = 0;
		int iNum = 0;

		if (m_RepList.IsEmpty())
			return 0;

		dataStack<CBotReputation> tempStack = m_RepList;

		while (!tempStack.IsEmpty())
		{
			const CBotReputation* pRep = tempStack.ChoosePointerFromStack();

			// bug fixed... didn't add :-P
			iTotal += pRep->CurrentRep();
			iNum++;
		}

		return iTotal / iNum;
	}

	void AddRep(int iPlayerRepId, int iRep)
		// Add a NEW Reputation WITH a reputation value
	{
		const CBotReputation* l_Rep = GetRep(iPlayerRepId);

		if (l_Rep == nullptr) // Don't want to add a duplicate
			m_RepList.Push(CBotReputation(iPlayerRepId, iRep));
	}

	CBotReputation* GetCreateRep(int iPlayerRepId)
	{
		dataStack<CBotReputation> tempStack = m_RepList;

		while (!tempStack.IsEmpty())
		{
			CBotReputation* l_Rep = tempStack.ChoosePointerFromStack();

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

	CBotReputation* GetRep(int iPlayerRepId) const
	{
		dataStack<CBotReputation> tempStack = m_RepList;

		while (!tempStack.IsEmpty())
		{
			CBotReputation* l_Rep = tempStack.ChoosePointerFromStack();

			if (l_Rep->IsForPlayer(iPlayerRepId))
			{
				// Fool pointer, dont free memory
				tempStack.Init();

				return l_Rep;
			}
		}

		return nullptr;
	}

	int GetClientRep(const CClient* pClient) const;

	void IncreaseRep(int iPlayerRepId) const
	{
		if (iPlayerRepId == -1)
			return;

		if (CBotReputation* l_Rep = GetRep(iPlayerRepId))
		{
			const int iNewRep = l_Rep->CurrentRep() + 1;

			if (iNewRep > BOT_MAX_REP)
				return;

			l_Rep->UpdateRep(iNewRep);
		}
		else
		{
			// oh crap wth?

			BugMessage(nullptr, "bad rep data... (try deleting all botprofiles/*.rcr files :-( )");
		}
	}

	void DecreaseRep(int iPlayerRepId) const
	{
		if (iPlayerRepId == -1)
			return;

		if (CBotReputation* l_Rep = GetRep(iPlayerRepId))
		{
			const int iNewRep = l_Rep->CurrentRep() - 1;

			if (iNewRep < BOT_MIN_REP)
				return;

			l_Rep->UpdateRep(iNewRep);
		}
		else
		{
			// oh crap wth?

			BugMessage(nullptr, "bad rep data... (try deleting all botprofiles/*.rcr files :-( )");
		}
	}

	void Init()
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
			std::free(m_szLocationName);
		}

		this->Init();
	}

	edict_t *GetEntity ( void )
	{
		return m_pEntity;
	}

	bool HasEntity ( edict_t *pEntity )
	{
		return m_pEntity == pEntity;
	}

	const char *GetLocationName()
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

	void Destroy ()
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
	int m_iSerialNumber = 0;
	edict_t* m_pEdict = nullptr;
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
			return nullptr;
		}

		return nullptr;
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
			m_pEdict = nullptr;
			m_iSerialNumber = 0;
		}
	}
};

class CBotTask
{
public:

	CBotTask()
	{
		std::memset(this, 0, sizeof(CBotTask));
	}

	CBotTask(const eBotTask iTask, const int iScheduleId = 0, edict_t* pInfo = nullptr, const int iInfo = 0, const float fInfo = 0.0f,
		const Vector& vInfo = Vector(0, 0, 0), const float fTimeToComplete = -1.0f) : m_vInfo(vInfo)/*, CBotTask *GoalTask = NULL */
	{
		// cheap way of adding schedules.. ;)
		// means if this task fails we can fail every other task with the same
		// schedule id
		m_iScheduleId = iScheduleId;

		m_Task = iTask;
		m_iInfo = iInfo;
		m_pInfo.Set(pInfo);
		m_fInfo = fInfo;
		m_bFoundPath = false;

		m_iScheduleDescription = BOT_SCHED_NONE;

		if (fTimeToComplete == -1.0f)
			m_fTimeToComplete = -1.0f;
		else
			m_fTimeToComplete = gpGlobals->time + fTimeToComplete;
		//m_pGoalTask = GoalTask;
	}

	bool TimedOut() const
	{
		if (m_fTimeToComplete != -1.0f)
		{
			return m_fTimeToComplete < gpGlobals->time;
		}

		return false;
	}

	float TimeToComplete() const
	{
		return m_fTimeToComplete - gpGlobals->time;
	}

	void SetVector(Vector const& vNewVector)
	{
		m_vInfo = vNewVector;
	}

	void SetFloat(float fNewFloat)
	{
		m_fInfo = fNewFloat;
	}

	void ChangeScheduleDesc(eScheduleDesc iSchedDesc)
	{
		m_iScheduleDescription = iSchedDesc;
	}

	bool IsOfSchedule(eScheduleDesc iSchedDesc) const
	{
		return m_iScheduleDescription == iSchedDesc;
	}

	void SetInt(int iNewInt)
	{
		m_iInfo = iNewInt;
	}

	eBotTask Task() const
	{
		return m_Task;
	}

	edict_t* TaskEdict() const
	{
		return m_pInfo.Get();
	}

	void SetEdict(edict_t* pEdict)
	{
		m_pInfo.Set(pEdict);
	}

	Vector TaskVector() const
	{
		return m_vInfo;
	}

	int TaskInt() const
	{
		return m_iInfo;
	}

	float TaskFloat() const
	{
		return m_fInfo;
	}

	bool HasPath() const
	{
		return m_bFoundPath;
	}

	void SetPathInfo(bool bPathFound)
	{
		m_bFoundPath = bPathFound;
	}

	void SetScheduleId(int iScheduleId)
	{
		m_iScheduleId = iScheduleId;
	}

	int GetScheduleId() const
	{
		return m_iScheduleId;
	}

	void SetTimeToComplete(float fTime)
	{
		//////// BIGGG Problem fixed (must add gpGlobals time, duuhhhh!!)
		m_fTimeToComplete = gpGlobals->time + fTime;
	}

	bool operator == (const CBotTask& Task) const;

	const char* getTaskDescription() const
	{
		return _getTaskDescription(m_Task);
	}

	static const char* _getTaskDescription(eBotTask itask)
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
			/*/case BOT_TASK_TFC_BUILD_SENTRY:
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
				return "BOT_TASK_TFC_ROTATE_SENTRY";*/
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
		case BOT_TASK_WAIT_FOR_BOT_AT_WPT:
			return "BOT_TASK_WAIT_FOR_BOT_AT_WPT";
		case BOT_TASK_WAIT_FOR_RESOURCE_TOWER_BUILD:
			return "BOT_TASK_WAIT_FOR_RESOURCE_TOWER_BUILD";
		case BOT_TASK_IMPULSE:
			return "BOT_TASK_IMPULSE";
		case BOT_TASK_DROP_WEAPON:
			return "BOT_TASK_DROP_WEAPON";
		case BOT_TASK_SENSE_ENEMY:
			return "BOT_TASK_SENSE_ENEMY";
		}

		return "Unknown";
	}

	eScheduleDesc GetScheduleDesc() const
	{
		return m_iScheduleDescription;
	}

	const char* getScheduleDescription() const
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
		case BOT_SCHED_USE_TELEPORTER:
			return "BOT_SCHED_USE_TELEPORTER";
		case BOT_SCHED_MAKE_NEW_TELE_EXIT:
			return "BOT_SCHED_MAKE_NEW_TELE_EXIT";
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
	bool        m_bFoundPath;
	//
	/////////////////////////////////////

	//////////////////////////////
	// Incase task takes too long
	float       m_fTimeToComplete;
};

class CBotTasks
{
public:

	void SetTimeToCompleteSchedule(int iScheduleId, float fTime) const
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		fTime += gpGlobals->time;

		while (!tempStack.IsEmpty())
		{
			CBotTask* tempTask = tempStack.ChoosePointerFrom();

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
		const int iNewScheduleId = GetNewScheduleId();

		for (int i = iNumTasks - 1; i >= 0; i--)
		{
			pTasks[i].SetScheduleId(iNewScheduleId);
			pTasks[i].ChangeScheduleDesc(iScheduleDescription);

			AddTaskToFront(pTasks[i]);
		}
	}

	void RemoveTimedOutSchedules()
	{
		const CBotTask* pCurrentTask = this->CurrentTask();

		if (pCurrentTask == nullptr)
			return;

		const int iSchedIgnore = pCurrentTask->GetScheduleId();

		dataQueue<CBotTask> tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* tempTask = tempStack.ChoosePointerFrom();

			const int iFailSchedule = tempTask->GetScheduleId();

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

	void GiveSchedIdDescription(int iSchedId, eScheduleDesc iSchedDesc) const
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			CBotTask* tempTask = tempStack.ChoosePointerFrom();

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
	bool HasSchedule(eScheduleDesc iSchedDesc) const
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->IsOfSchedule(iSchedDesc))
			{
				tempStack.Init();
				return true;
			}
		}

		return false;
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

		while (!tempStack.IsEmpty())
		{
			try
			{
				const CBotTask* pTask = tempStack.ChoosePointerFrom();

				if (*pTask == Task)
				{
					tempStack.Init();

					const int iScheduleId = pTask->GetScheduleId();

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

		while (!tempStack.IsEmpty())
		{
			const CBotTask* pTask = tempStack.ChoosePointerFrom();

			if (pTask->Task() == iTask)
			{
				tempStack.Init();

				const int iScheduleId = pTask->GetScheduleId();

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

	void FlushTasks()
	{
		m_Tasks.Destroy();
	}

	bool HasTask(eBotTask iTask)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* tempTask = tempStack.ChoosePointerFrom();

			if (tempTask->Task() == iTask)
			{
				tempStack.Init();

				return true;
			}
		}

		return false;
	}

	// get a actual task  from a task type
	void GetTask(eBotTask iTask, CBotTask* GotTask) const
	{
		dataQueue<CBotTask> tempStack = m_Tasks;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* pTask = tempStack.ChoosePointerFrom();

			if (pTask->Task() == iTask)
			{
				*GotTask = *pTask;
				tempStack.Init();
				return;
			}
		}

		*GotTask = CBotTask(BOT_TASK_NONE);
	}

	bool HasTask(CBotTask const& Task)
	{
		return m_Tasks.IsMember(Task);//changed
	}

	CBotTask* CurrentTask()
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

	int GetNewScheduleId() const
		// Return an unused Schedule Id...
	{
		int iScheduleId = 1;

		dataQueue<CBotTask> tempStack = m_Tasks;

		// search through list to find a unique Id.
		while (!tempStack.IsEmpty())
		{
			const CBotTask* pTask = tempStack.ChoosePointerFrom();

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
	bool FinishSchedule(int iScheduleId)
		// Finish all tasks with ScheduleId as its schedule
	{
		dataQueue<CBotTask> tempStack = m_Tasks;
		bool bRemoved = false;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* pTask = tempStack.ChoosePointerFrom();

			if (pTask->GetScheduleId() == iScheduleId)
			{
				m_Tasks.RemoveByPointer(pTask);
				bRemoved = true;
				tempStack = m_Tasks;
			}
		}

		return bRemoved;
	}

	//
	// Removes all tasks from a schedule description
	bool FinishSchedule(eScheduleDesc iSchedDesc)
	{
		dataQueue<CBotTask> tempStack = m_Tasks;
		bool bRemoved = false;

		while (!tempStack.IsEmpty())
		{
			const CBotTask* pTask = tempStack.ChoosePointerFrom();

			if (pTask->IsOfSchedule(iSchedDesc))
			{
				m_Tasks.RemoveByPointer(pTask);
				bRemoved = true;
				tempStack = m_Tasks;
			}
		}

		return bRemoved;
	}

	void FinishedCurrentTask(bool bFailed = false)
	{
		CBotTask* pTask = CurrentTask();

		const eBotTask iTask = pTask->Task();
		const bool bPathInfo = pTask->HasPath();
		const int iScheduleId = pTask->GetScheduleId();

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
				pTask->SetPathInfo(bPathInfo);
			}
		}
	}

	bool NoTasksLeft()
	{
		return m_Tasks.IsEmpty();
	}

	void RemovePaths() const
	{
		// Altered bot's path info

		dataQueue<CBotTask> tempStack = m_Tasks;

		// will increment head, we've already set that to bPathFound
		if (tempStack.IsEmpty())
			return;

		if (tempStack.ChoosePointerFrom())
		{
			while (!tempStack.IsEmpty())
			{
				if (CBotTask* pPrevTask = tempStack.ChoosePointerFrom())
				{
					// Changed path info to false for these tasks
					pPrevTask->SetPathInfo(false);
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
	path* next;   // link to next structure
} PATH;

////////////////////////////
// Bot Profile Info
// TODO: those need to be initialised for Bot Profile to work? [APG]RoboCop[CL]
typedef struct bot_profile_s
{
	int m_iProfileId;

	int m_iFavMod;
	int m_iFavTeam;
	int m_iSkill;

	const char* m_szFavMap;
	const char* m_szSpray;
	const char* m_szBotName;

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
		m_pPath = nullptr;
		// in case this is added remove it later
		// by showing that its violated
		m_bViolated = true;
	}

	CBotFailedPath(PATH* pPath)
	{
		m_pPath = pPath;
		m_bViolated = false;
	}

	bool IsForPath(PATH* pPath) const
	{
		return m_pPath == pPath;
	}

	void Violate()
	{
		m_bViolated = true;
	}

	bool IsViolated() const
	{
		return m_bViolated;
	}

private:
	PATH* m_pPath; // the actual path
	bool m_bViolated; // the path is going to be removed if true
};

class CBotFailedPaths
{
public:

	bool Violate(PATH* pPath) const
		// violate path, make it unavailable.
	{
		dataStack<CBotFailedPath> tempStack = m_FailedPaths;

		while (!tempStack.IsEmpty())
		{
			CBotFailedPath* pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsForPath(pPath))
			{
				pFailedPath->Violate();

				// MUST reset temporary stack to a NULL Head!!!
				tempStack.Init();

				return true;
			}
		}

		return false;
	}

	bool ClearViolatedPaths()
		// Clear paths that have been checked and dealt with in path finding
	{
		dataStack<CBotFailedPath> tempStack = m_FailedPaths;

		bool bRemoved = false;

		while (!tempStack.IsEmpty())
		{
			const CBotFailedPath* pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsViolated())
			{
				m_FailedPaths.RemoveByPointer(pFailedPath);

				bRemoved = true;
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

		while (!tempStack.IsEmpty())
		{
			const CBotFailedPath* pFailedPath = tempStack.ChoosePointerFromStack();

			if (pFailedPath->IsForPath(pPath))
			{
				tempStack.Init();
				// can't add , already in
				return;
			}
		}

		m_FailedPaths.Push(pPath);
	}

	void ClearFailedPaths()
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
		std::memset(this, 0, sizeof(CTypeVector<T>));
	}

	void SetVector(const Vector& vVec)
	{
		m_x = static_cast<T>(vVec.x);
		m_y = static_cast<T>(vVec.y);
		m_z = static_cast<T>(vVec.z);

		m_bVectorSet = true;
	}

	Vector GetVector() const
	{
		return { static_cast<float>(m_x), static_cast<float>(m_y), static_cast<float>(m_z) };
	}

	bool IsVectorSet() const
	{
		return m_bVectorSet;
	}

	void UnSet()
	{
		m_bVectorSet = false;
	}
protected:
	T m_x, m_y, m_z;

	bool m_bVectorSet;
};

class CAutoWaypointCheck : public CTypeVector<float>
{
public:
	void SetPoint(const Vector& vec, int iFlags)
	{
		m_iFlags = iFlags;

		SetVector(vec);
	}

	int getFlags() const
	{
		return m_iFlags;
	}

	void UnSetPoint()
	{
		m_bVectorSet = false;
		m_iFlags = 0;
	}
private:
	int m_iFlags = 0;
};

enum : std::uint8_t
{
	BOT_REMEMBER_POSITION = 0,
	BOT_REMEMBER_SENTRY = (1<<0),
	BOT_REMEMBER_LOST_ENEMY = (1<<1),
	BOT_REMEMBER_SOUND = (1<<2),
	BOT_REMEMBER_1ST_SEE_ENEMY = (1<<3)
};

class CRememberPosition
{
public:

	CRememberPosition()
	{
		std::memset(this, 0, sizeof(CRememberPosition));
	}

	CRememberPosition(const Vector& vOrigin, edict_t* pEntity)
		: m_vOrigin(vOrigin), m_iFlags(0) // initialize m_iFlags to 0
	{
		setEntity(pEntity);
	}

	void setFlags(int flags)
	{
		m_iFlags = flags;
	}

	bool hasFlags(int flags) const
	{
		return (m_iFlags & flags) > 0;
	}

	void setEntity(edict_t* pEntity)
	{
		m_pEntity.Set(pEntity);
	}

	edict_t* getEntity() const
	{
		return m_pEntity.Get();
	}

	bool isEntity(edict_t* pEntity) const
	{
		if (pEntity == nullptr)
			return false;

		return pEntity == getEntity();
	}

	Vector getVector()
	{
		return m_vOrigin;
	}

	void setVisibleOrigin(Vector const& vVis)
	{
		m_vVisibleOrigin = vVis;
	}

	Vector getVisibleOrigin()
	{
		return m_vVisibleOrigin;
	}

	bool operator == (const CRememberPosition& other) const
	{
		return other.getEntity() == getEntity();
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

	void Init()
	{
		std::memset(this, 0, sizeof(CRememberedPositions));

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

		bool gotPosition ()
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

	void addPosition(Vector const& vOrigin, edict_t* pEntity, int flags, const Vector& vVisibleOrigin)
	{
		CRememberPosition newPosition = CRememberPosition(vOrigin, pEntity);
		newPosition.setFlags(flags);
		newPosition.setVisibleOrigin(vVisibleOrigin);

		m_Positions.getExisting(newPosition);

		if (const int index = m_Positions.getExistingIndex(newPosition); index != -1)
			m_Positions.RemoveByIndex(index);
		else if (m_Positions.Size() > MAX_REMEMBER_POSITIONS)
			m_Positions.RemoveByIndex(0);

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
				return &m_Positions[i];
		}

		return nullptr;
	}

	void removePosition(edict_t* pEntity)
	{
		//		int j;

		for (int i = 0; i < m_Positions.Size(); i++)
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
				return &m_Positions[i];
		}

		return nullptr;
	}

	bool gotPosition() const
	{
		return !m_Positions.IsEmpty();
	}

	CRememberPosition* getNewest()
	{
		return &m_Positions[m_Positions.Size() - 1];//[m_iNewest]);//.getVector();
	}

	CRememberPosition* positionNearest(const Vector& vOrigin, const Vector& vFrom)
	{
		CRememberPosition* nearest = nullptr;
		float fNearest = 0;

		for (int i = 0; i < m_Positions.Size(); i++)
		{
			const float fDistance = (m_Positions[i].getVector() - vOrigin).Length() + (m_Positions[i].getVector() - vFrom).Length();

			if (!nearest || fDistance < fNearest)
			{
				nearest = &m_Positions[i];
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

	bool HasVisited ( void )
	{
		return ((m_iFlags & BOT_RES_FLAG_VISITED)==BOT_RES_FLAG_VISITED);
	}

	bool IsHostile ( void )
	{
		return ((m_iFlags & BOT_RES_FLAG_HOSTILE)==BOT_RES_FLAG_HOSTILE);
	}

	bool HasFailedGoal ( void )
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
// TODO: need to be initialised for Bot Class to work? [APG]RoboCop[CL]
#define SQUAD_DEFAULT_SPREAD 50.0f// say 50 units between each member...?

enum eSquadForm : std::uint8_t
{
	SQUAD_FORM_NONE = 0,
	SQUAD_FORM_WEDGE,
	SQUAD_FORM_LINE,
	SQUAD_FORM_ECH_LEFT,
	SQUAD_FORM_ECH_RIGHT,
	SQUAD_FORM_COLUMN,
	SQUAD_FORM_VEE
};

enum eCombatType : std::uint8_t
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

		bCanFire = true;

		if ((pLeader = GetLeader()) != nullptr)
			m_vLeaderAngle = pLeader->v.angles;
	}

	void ReturnAllToFormation();

	void SetLeader(edict_t* pLeader)
	{
		m_pLeader.Set(pLeader);
	}

	void ToggleFireMode()
	{
		bCanFire = !bCanFire;

		if (bCanFire)
			BotPrintTalkMessageOne(m_pLeader.Get(), "Squad can now OPEN FIRE");
		else
			BotPrintTalkMessageOne(m_pLeader.Get(), "Squad is now HOLDING FIRE");
	}

	bool SquadCanShoot() const
	{
		return bCanFire;
	}

	bool IsStealthMode() const
	{
		return m_CombatType == COMBAT_STEALTH;
	}

	bool IsProneMode() const
	{
		return m_CombatType == COMBAT_PRONE;
	}

	bool IsCrouchMode() const
	{
		return m_CombatType == COMBAT_CROUCH;
	}

	edict_t* GetLeader() const
	{
		return m_pLeader.Get();
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
			std::strcpy(szCombatType, "STEALTH");
			break;
		case COMBAT_PRONE:
			std::strcpy(szCombatType, "PRONE");
			break;
		case COMBAT_CROUCH:
			std::strcpy(szCombatType, "CROUCH");
			break;
		case COMBAT_NONE:
		case COMBAT_COMBAT:
			std::strcpy(szCombatType, "NORMAL");
			break;
		}

		BotPrintTalkMessageOne(pLeader, "Combat mode is now %s\n", szCombatType);
	}

	void ChangeLeader()
	{
		if (m_theSquad.IsEmpty())
		{
			SetLeader(nullptr);
		}
		else
		{
			m_pLeader = m_theSquad.Pop();

			if (m_theSquad.IsEmpty())
				SetLeader(nullptr);
			else
			{
				Init(); // new squad init
			}
		}
	}

	void removeMember(edict_t* pMember)
	{
		dataStack<MyEHandle> tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			const MyEHandle* temp = tempStack.ChoosePointerFromStack();

			if (temp->Get() == pMember)
			{
				m_theSquad.RemoveByPointer(temp);
				tempStack.Init();
				return;
			}
		}
	}

	bool IsLeader(edict_t* pLeader) const
	{
		return GetLeader() == pLeader;
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

	int numMembers() const
	{
		int num = 0;

		dataStack<MyEHandle> tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			tempStack.ChooseFromStack();

			num++;
		}

		return num;
	}

	bool IsMember(edict_t* pEdict) const
	{
		dataStack<MyEHandle> tempStack = m_theSquad;

		while (!tempStack.IsEmpty())
		{
			MyEHandle temp = tempStack.ChooseFromStack();

			if (temp.Get() == pEdict)
			{
				tempStack.Init();
				return true;
			}
		}

		return false;
	}

	bool isFormation(eSquadForm theFormation) const
	{
		return m_theDesiredFormation == theFormation;
	}

	void ChangeFormation(eSquadForm theNewFormation)
	{
		m_theDesiredFormation = theNewFormation;
	}

	float GetSpread() const
	{
		return m_fDesiredSpread;
	}

	void ChangeSpread(float fNewSpread)
	{
		m_fDesiredSpread = fNewSpread;
	}

	int GetFormationPosition(edict_t* pEdict) const
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

	Vector GetFormationVector(edict_t* pEdict) const;

	void UpdateAngles()
	{
		const edict_t* pLeader = GetLeader();

		if (pLeader->v.velocity.Length2D() != 0.0f)
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
	bool bCanFire;

	eCombatType m_CombatType;

	Vector m_vLeaderAngle;
};

/*class CTFCBackpack
{
public:
	CTFCBackpack()
	{
		std::memset(this, 0, sizeof(CTFCBackpack));
	}

	CTFCBackpack(edict_t* pEdict)
	{
		std::memset(this, 0, sizeof(CTFCBackpack));

		m_pEdict = pEdict;
	}

	bool givesHealth(int val)
	{
		return m_iHealth >= val;
	}

	bool givesAmmo(int val)
	{
		return m_iAmmo >= val;
	}

	bool givesArmor(int val)
	{
		return m_iArmor >= val;
	}

	bool givesCells(int val)
	{
		return m_iCells >= val;
	}

	edict_t* getEdict()
	{
		return m_pEdict;
	}

	bool isEdict(edict_t* pEdict)
	{
		return m_pEdict == pEdict;
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

	bool isForTeam(int team)
	{
		if (m_iTeam == 0)
			return true; // any team

		return m_iTeam == team;
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

		pNearest = nullptr;
		fNearestDist = 4096.0f;

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
};*/

constexpr int LEARNED_FILE_VER = 1;

class CLearnedHeader
{
public:
	CLearnedHeader(int iId);

	bool operator == (const CLearnedHeader& other) const;
	bool operator != (const CLearnedHeader& other) const
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

	void FreeMemory()
	{
		dataStack<CBotSquad*> tempStack = m_theSquads;

		while (!tempStack.IsEmpty())
		{
			const CBotSquad* pSquad = tempStack.ChooseFromStack();

			//			if (pSquad)
			delete pSquad;
			pSquad = nullptr;
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

	CBotSquad* FindSquadByLeader(edict_t* pLeader) const
	{
		dataStack<CBotSquad*> tempStack = m_theSquads;

		while (!tempStack.IsEmpty())
		{
			CBotSquad* pSquad = tempStack.ChooseFromStack();

			if (pSquad->IsLeader(pLeader))
			{
				tempStack.Init();
				return pSquad;
			}
		}

		return nullptr;
	}

	void RemoveSquad(CBotSquad* pSquad);

	void UpdateAngles()
	{
		dataStack<CBotSquad*> tempStack = m_theSquads;

		CBotSquad* pSquad = nullptr;

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

constexpr int BOT_COST_RANGE = 64;
constexpr int BOT_COST_BUCKETS = 7;

class TSObjective
{
public:
	TSObjective()
	{
		m_iId = 0;
		m_vOrigin = Vector(0, 0, 0);
		m_szName = nullptr;
	}
	TSObjective(int id, const Vector& origin, char* name)
		: m_iId(id), m_vOrigin(origin), m_szName(name)
	{}
	/*guurk*/
	int operator ==(TSObjective& comp) const
	{
		return m_iId;
	}
	/*/guurk*/

	int getID() const { return m_iId; }

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
	bool m_bNearestRememberPointVisible;
	bool m_bLookingForEnemy;
	CPerceptron* dec_runForCover;
	CPerceptron* dec_faceHurtOrigin;
	CPerceptron* dec_followEnemy;
	CPerceptron* dec_flapWings;

	float m_fRespawnTime = 0.0f;

	//CPerceptron *dec_useBlink;
	//CPerceptron *dec_pressBlinkButton;
	bool m_bFlappedWings;
	bool m_bZoom;
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
	bool bSetPrevAimOrigin;
	bool bAimStopMoving;
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

	bool m_bUsedMelee;
	CBotWeapon m_weapon; // The bot's weapon

	unsigned char m_iBoredom; // negative if really bored

	CTypeVector<short int> m_seenFlagPos;

	void decideJumpDuckStrafe(float fEnemyDist, const Vector& vEnemyOrigin);

	//float utility ( CBotTask eTask );

	void SawSpy(edict_t* pPlayer)
	{
		m_pSpySpotted = pPlayer;
		m_fSeeSpyTime = gpGlobals->time + RANDOM_FLOAT(4.0f, 8.0f);
	}

	CBotGAValues* m_GASurvival;
	//	CBotGAValues *m_GATFCSpy;
	float m_fLowestEnemyCost;
	Vector m_vLowestEnemyCostVec;

	float fRangeCosts[BOT_COST_BUCKETS][BOT_COST_BUCKETS];

	void clearEnemyCosts();

	void workEnemyCosts(edict_t* pEntity, const Vector& vOrigin, float fDistance);

	float m_fNextWorkRangeCosts;

	bool m_bPlacedPipes;
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
	bool m_bHasFlag; // has a flag or the only flag?
	edict_t* m_pFlag; // holding this flag

	bool m_bCommInit;
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
	bool m_bSelectedCar;

	//bool m_bSelectedGun; // TS

	// current gear
	int m_iGear;

	/////////////////////////////////
	// evloutionary fitness
	float m_fSurvivalTime;
	float m_fSpawnTime;
	int m_iNumFragsSinceDeath;

	// --------------------------------

	void loadLearnedData() const;

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

	bool m_bStartedGame;
	bool m_bChoseClass;

	//int m_iSentryLevel;

	bool m_bHasLongJump;

	int m_iLastButtons;

	bool m_bSaidInPosition;
	bool m_bSaidMoveOut;
	bool m_bSaidAllClear;

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

	bool m_bLadderAnglesSet;
	Vector m_vLadderAngles;

	float m_fLastNoMove;
	/*
		/////////////////////////
		// TFC private stuff

		CTypeVector<short int> m_fFeignPosition;
	*/
	float m_fHoldAttackTime;

	void FlapWings();

	bool m_bBuiltDispenser;
	bool m_bIsDisguised;

	// teleport/phase gate
	float m_fUseTeleporterTime;

	//bool m_bBuiltTeleportEntrance;
	//bool m_bBuiltTeleportExit;

	bool SentryNeedsRepaired() const;

	//void RepairSentry(int iNewScheduleId);

	bool m_bLookForNewTasks;

	void DoTasks();

	float m_f3dVelocity;
	float m_f2dVelocity;

	CTypeVector<short int> m_vSpawnPosition;

	void CheckStuck();

	//void workButtons ();

	dataUnconstArray<edict_t*> m_pTeleportersUsed;

	bool hasUsedTeleporter(edict_t* pTeleport)
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

	void saveLearnedData() const;
	void touchedWpt();
	void gotStuck();

	bool hasFlag() const
	{
		return m_bHasFlag && m_pFlag;
	}

	edict_t* getSentry() const;

	static bool CanBuild(const edict_t* pEdict, int* metal = nullptr);

	const char* getLookTaskDescription() const
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
		case BOT_LOOK_TASK_FACE_NEAREST_REMEMBER_POS:
			return "BOT_LOOK_TASK_FACE_NEAREST_REMEMBER_POS";
		}

		return "Unknown";
	}

	void setupDataStructures();
	//void freeDataStructures ();

	CPerceptron* dec_attackElectrified;
	CPerceptron* dec_stunt;
	edict_t* m_pElectricEnemy;

	static bool isInAnimate(const edict_t* pEntity);

	bool isFriendly(const edict_t* pEntity) const;

	CBotSquad* getSquad() const
	{
		return m_stSquad;
	}

	bool onGround() const
	{
		return UTIL_OnGround(pev);
	}

	void printBoredoms(edict_t* pEdictTo) const
	{
		BotMessage(pEdictTo, 0, "%s's boredom = %d (%0.1f percent)", m_szBotName, m_iBoredom, static_cast<float>(m_iBoredom) / 255);
	}

	void havingFun()
	{
		// keep boredom within 0 and 255.
		if (m_iBoredom < 255)
			m_iBoredom++;
	}

	void gettingBored()
	{
		if (m_iBoredom > 0)
			m_iBoredom--;
	}

	bool WantToLeaveGame() const;

	bool WantToFindEnemy() const;

	void PlayerDead(edict_t* pPlayer)
	{
		if (pPlayer == m_pSpySpotted)
			m_pSpySpotted = nullptr;
	}

	void LookForNewTasks();
	/*
		bool CanHealPlayer ( edict_t *pPlayer )
		{
			bool bCanHeal = false;

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
					bCanHeal = true;
				else if ( ( pev->playerclass == TFC_CLASS_ENGINEER ) && HasWeapon(TF_WEAPON_SPANNER) )
					bCanHeal = true;
			}
		}*/

		//bool ThinkSpyOnTeam(edict_t* pSpy);

	void ThrowGrenade(const edict_t* pEnemy, int preference = 0, bool bDontPrime = false);

	bool ThrowingGrenade() const;

	// No longer required for TFC [APG]RoboCop[CL]
	/*void LostDisguise()
	{
		m_bIsDisguised = false;
	}

	void Disguised()
	{
		m_bIsDisguised = true;
	}

	void DispenserBuilt()
	{
		m_bBuiltDispenser = true;
	}

	void DispenserDestroyed()
	{
		m_bBuiltDispenser = false;
	}*/

	//void NeedMetal(bool flush = true, bool priority = false, int iSched = 0);
	//void FindBackPack(int health = 0, int cells = 0, int armour = 0, int ammo = 0, bool flush = true, bool priority = false, int iSched = 0);

	void CantBuildHere()
	{
		m_fNextBuildTime = gpGlobals->time + RANDOM_FLOAT(1.0f, 2.0f);
	}

	void StoppedBuilding()
	{
		UpdateCondition(BOT_CONDITION_STOPPED_BUILDING);
	}

	/*bool HasBuiltSentry();

	void setSentry ( edict_t *pEdict )
	{
		m_pSentry.Set(pEdict);
	}*/

	bool builtTeleporterEntrance() const;

	bool builtTeleporterExit() const;

	edict_t* getTeleporterExit() const;

	/*void TeleportEntranceDestroyed ()
	{
		m_bBuiltTeleportEntrance = false;
	}

	void TeleportExitDestroyed ()
	{
		m_bBuiltTeleportExit = false;
		//m_pTeleExit = NULL;
	}

	void SentryDestroyed ()
	{
		m_iSentryLevel = 0;
		m_pSentry.Set(NULL);
	}*/

	//void UpgradedSentry ()
	//{
	//	m_iSentryLevel ++;
	//}

	edict_t* m_pSentryToKill;

	//void TFC_UpdateFlagInfo();

	bool m_bAcceptHealth; // if true bot will wait for health when he gets it.

	Vector m_vAvoidMoveTo;
	float m_fAvoidTime;

	edict_t* m_MegaHALTalkEdict;

	void SayInPosition();

	bool IsUsingTank() const;

	edict_t* GetUsingTank() const
	{
		if (m_pTank)
			return m_pTank;

		return nullptr;
	}

	bool CanUseTank(edict_t* pTank) const
	{
		if (m_fNextUseTank > gpGlobals->time)
			return false;
		// already using one
		if (IsUsingTank())
			return false;
		if (UTIL_TankUsed(pTank))
			return false;
		return true;
	}

	// Use secondary attack for one time only..
	// not a worky??!
	void UseRPGLaser()
	{
		AddPriorityTask(CBotTask(BOT_TASK_SECONDARY_ATTACK, 0));
	}

	bool IsSquadLeader() const
	{
		return !m_pSquadLeader && m_stSquad;
	}

	//bool IsHoldingMiniGun();

	// temporary storage to later pick up weapons needed
	void SetWeaponsNeeded(short int* iArray)
	{
		std::memcpy(m_iWeaponsNeeded, iArray, sizeof(short int) * MAX_WEAPONS);
	}

	// returns player standing on the bot
	edict_t* PlayerStandingOnMe() const;

	// returns player bot is standing on
	edict_t* StandingOnPlayer() const;

	// store buttons while on ladder in case of think time clearing buttons
	void SetLadderMovement(int iButtons)
	{
		m_iLadderButtons = iButtons;
	}

	// leave squad/clear it completely
	bool RemoveMySquad();

	// add another player to bots squad
	void AddToSquad(edict_t* pLeader);

	// available to add to a squad?
	bool CanAddToSquad(edict_t* pLeader);

	// is used in the game?
	bool IsUsed() const
	{
		return m_bIsUsed && m_pEdict;
	}

	// is in the squad "theSquad"?
	bool InSquad(CBotSquad* theSquad) const
	{
		return theSquad == m_stSquad;
	}

	// remove squad from bot
	void ClearSquad()
	{
		m_stSquad = nullptr;
		m_pSquadLeader = nullptr;
	}

	void SetSquad(CBotSquad* theSquad)
	{
		ClearSquad();
		m_stSquad = theSquad;
	}

	void SetVGUIState(int iVguiMenu)
	{
		m_iVguiMenu = iVguiMenu;
	}

	bool BotWantsCombatItem(int iCombatItem) const
	{
		return (m_iCombatInfo & iCombatItem) > 0;
	}

	void SetWantCombatItem(int iCombatItem, bool bSet)
	{
		if (bSet)
			m_iCombatInfo |= iCombatItem;
		else
			m_iCombatInfo &= ~iCombatItem;
	}

	// shortcut pointer to bots pEdict pointer, set up on bot create
	// nullified on initialise, bot functions assume this is not NULL.
	entvars_s* pev;

	// Returns true when already visited resource tower
	// (already added to list of those visited)
	bool HasVisitedResourceTower(const edict_t* pEdict) const;

	// add resource tower to a list of those visited so
	// gorge doesn't go back to ones we've been to already
	void AddVisitedResourceTower(edict_t* pEdict);

	// Make bot remember this vector position, bot will return to it at some point
	void RememberPosition(const Vector& vOrigin, edict_t* pEntity, int flags = 0);

	// Add a task to the end of the task stack
	void AddTask(const CBotTask& Task);

	// Add a task to the fron of the task stack
	void AddPriorityTask(const CBotTask& Task);

	// stop bot from moving
	void StopMoving();

	// change vector bot wants to move to
	void SetMoveVector(const Vector& vOrigin);

	// The bot's task it needs to carry out when reaching its goal
	// in NS when given an order by the commander
	CBotTask m_OrderTask;

	// Goal origin, a somewhat nearby vector to
	// where the bot wants to go
	CTypeVector<short int> m_vGoalOrigin;

	// using crappy non-waypointed navigation?
	bool m_bNotFollowingWaypoint;

	// Bots weapons
	CBotWeapons m_Weapons;
	// the pointer to the current drawn weapon
	CBotWeapon* m_pCurrentWeapon;

	/////////////////////////////////////////
	// Navigation

	// open list of waypoints for A*
	dataUnconstArray<AStarNode*> sOpenList;//dataStack<int>
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
	bool m_bIsUsed;

	// the bots actaul name in the game
	char* m_szBotName;

	// the bots "bot profile" info
	bot_profile_t m_Profile;

	// bots current team
	int m_iTeam;

	// If kill is true, bot will kill itself
	bool m_bKill;
	// if kick is true, bot will be kicked from the game
	bool m_bKick;

	// If last attack failed
	//bool m_bShotFailed;

	// does bot want to use an ammo dispenser
	// to get more ammo?
	bool m_bCanUseAmmoDispenser;

	// has bot has already gestated in NS aliens?
	bool m_bHasGestated;

	//////////////////////////
	// for LOOK_TASKS

	Vector m_vCurrentLookDir;
	float m_fLastLookTime;

	////////////////////////////

	////////////////////////////
	// for Moving

	Vector m_vMoveToVector;		// Is the vector that the bot wants to move to.
	bool m_bMoveToIsValid;		// Is false When bot has no waypoint.

	/////////////////////////////
	//

	int m_iRespawnState;		// Is set to RESPAWN_NEED_TO_RESPAWN upon level
	// change so we know to rejoin the bot.
	//float m_fNextCheckWptVis;
	//bool m_bWaypointVis;

	/////////////////////////////
	// NAVIGATION RELATED

	int GetNextWaypoint() const
		// get waypoint AFTER current waypoint,
			// return current if no next one.
	{
		dataStack<int> tempStack = m_stBotPaths;
		int iNextWaypoint = m_iCurrentWaypointIndex;

		while (!tempStack.IsEmpty() && iNextWaypoint == m_iCurrentWaypointIndex)
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
	void SetLadderAngles(Vector const& vAngles)
	{
		m_vLadderAngles = vAngles;
		m_bLadderAnglesSet = true;
	}

	// clear the angles
	void UnSetLadderAngleAndMovement()
	{
		m_bLadderAnglesSet = false;
		m_iLadderButtons = 0;
	}

	// get the angles....
	Vector GetLadderAngles()
	{
		return m_vLadderAngles;
	}

	// if we have set ladder angles?
	bool LadderAnglesSet() const
	{
		return m_bLadderAnglesSet;
	}

	// >?
	bool LadderMovementSet() const
	{
		return m_iLadderButtons != 0;
	}

	//////////////////////////
	// DMC functions

	// Useful so bot doesn't run away and always follows enemies etc.
	int m_iQuakeItems;

	// returns true if bot picked up invulnerabilty
	bool DMC_HasInvulnerability() const
	{
		return (m_iQuakeItems & 1 << static_cast<int>(DMCWeapon::INVULNERABILITY)) > 0;
	}
	bool DMC_HasInvisibility() const
	{
		return (m_iQuakeItems & 1 << static_cast<int>(DMCWeapon::INVISIBILITY)) > 0;
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

	bool m_bSaidGreetings;

	///////////////////////////
	// TASK RELATED

	//bool m_bGotEnemyPath;

	eBotTask m_iLastBotTask;
	eBotTask m_iLastFailedTask; // make sure we dont try that last failed task for a while
	eScheduleDesc m_iLastFailedSchedule;

	////////////////////////////
	// What to do when a bot completes an order in NS
	bool m_bHasOrder;
	// e.g. Wait, assemble squad, find a new task etc...
	//CBotTask m_OrderCompleteTask;

	AvHOrderType m_iOrderType;

	bool m_bHasAskedForOrder;

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

	int m_iCombatInfo;
	//short int m_iCombatUpgrades;

	edict_t* m_pSquadLeader;
	float    m_fFollowSquadTime;
	Vector m_vTeleporter;
	float m_fNextCheckTeleporterExitTime;

	bool     m_bNeedToInit;

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

	void RunPlayerMove();

	void Init();

	/////////////////////////
	// VIEWING ANGLES AND MOVEMENT

	void WorkViewAngles();
	void WorkMoveDirection();
	void SetViewAngles(const Vector& pOrigin);

	////////////////////////

	void EnemyDown(edict_t* pEnemy);
	bool IsEnemy(edict_t* pEntity);

	// FREE LOCAL MEMORY
	// free memory of structures inside the bot
	// do NOT call within bot functions
	void FreeLocalMemory();

	int GetLadderDir(bool bCheckWaypoint = false) const;

	bool NotStartedGame();

	void StartGame();

	void ReplyToMessage(char* szMessage, edict_t* pSender, int iTeamOnly);

	void FreeGlobalMemory()
	{
		this->FreeLocalMemory();

		Init();
	}

	void DuckAndJump();
	void JumpAndDuck();

	CBotTask* currentTask() const
	{
		return m_CurrentTask;
	}

	void ClearTasks()
	{
		//m_WptObjectiveTask = CBotTask(BOT_TASK_NONE);

		m_OrderTask = CBotTask(BOT_TASK_NONE);

		m_CurrentTask = nullptr;

		m_Tasks.FlushTasks();

		//m_GoalTasks.Destroy();
	}

	void BotCommand();

	void UpdateMsec();

	//void MapInit(); //Duplicate? [APG]RoboCop[CL]

	//void BotOnLadder(); //TODO: Needs revised [APG]RoboCop[CL]

	void UseTank(edict_t* pTank);

	/////////////////////////////////////
	// VISIBLES
	bool IsInVisibleList(edict_t* pEntity) const;

	bool HasUpgraded(int iUpgrade) const;

	bool FVisible(const edict_t* pEntity);

	bool FVisible(const Vector& vecOrigin) const;

	bool FInViewCone(Vector* pOrigin) const;

	float DotProductFromOrigin(const Vector* pOrigin) const;

	Vector GetGunPosition() const
	{
		return pev->origin + pev->view_ofs;
	}

	bool     IsAlive() const
	{
		return pev->deadflag == DEAD_NO && pev->health > 0;
	}

	bool     SwitchWeapon(byte iId);

	bool     IsCurrentWeapon(int iId) const
	{
		if (m_pCurrentWeapon == nullptr)
			return false;
		return m_pCurrentWeapon->GetID() == iId;
	}
	// ----------------------------------------------------------
	// Flashlight functions for putting a flashlight on or off

	bool     FlashlightIsOn() const
	{
		return (pev->effects & EF_DIMLIGHT) == EF_DIMLIGHT;
	}

	void     FlashlightTurnOn() const
	{
		if (!FlashlightIsOn())  // If flashlight is already on, we dont want to turn it off!
		{
			pev->impulse = 100;
		}
	}

	void     FlashlightTurnOff() const
	{
		if (FlashlightIsOn())
		{
			pev->impulse = 100;
		}
	}
	// ----------------------------------------------------------
	//

	// Returns true if pEntity isn't an entity
	// that the bot doesn't want to avoid.
	bool CanAvoid(edict_t* pEntity, float fDistanceToEntity, float fAvoidDistance) const;

	static void ChangeAngles(float* fSpeed, const float* fIdeal, float* fCurrent, float* fUpdate);

	bool IsOnLadder() const
	{
		return pev->movetype == MOVETYPE_FLY;
	}

	void     Jump() const
	{
		pev->button |= IN_JUMP;
	}

	// if bot can fly, this will return true
	bool CanFly() const;
	/*{
		return IsMarine() && HasJetPack() || IsLerk();
	}*/

	// set crouch button
	void     Duck() const
	{
		pev->button |= IN_DUCK;
	}

	// returns bots ideal climb situation (e.g. flying, climbing, wall-sticking)
	eClimbType GetClimbType() const;

	bool     PrimaryAttack() const;

	bool     SecondaryAttack() const
	{
		pev->button |= IN_ATTACK2;

		// Can't shoot while reloading
		if (pev->button & IN_RELOAD)
			return false;

		return true;
	}

	void Use() const
	{
		pev->button |= IN_USE;
	}

	void AltButton() const
	{
		pev->button |= IN_ALT1;
	}

	void Reload() const
	{
		pev->button |= IN_RELOAD;
	}

	// -------------------------------------------------------
	// Bot Conditions
	// Functions for setting, checking and removing conditions

	void     UpdateCondition(int iCondition)
	{
		m_ibBotConditions |= iCondition;
	}

	bool     HasCondition(int iCondition) const
	{
		return (m_ibBotConditions & iCondition) == iCondition;
	}

	void     RemoveCondition(int iCondition)
	{
		m_ibBotConditions &= ~iCondition;
	}

	// updates some conditions
	void     UpdateConditions();

	// -------------------------------------------------------

	//int		BotDanger				( void );

	// does bot want to pursue the enemy or stop and do his own thing?
	bool     WantToFollowEnemy(const edict_t* pEnemy) const;

	// returns true when bot is facing the ideal aim position
	bool	 FacingIdeal() const;

	// returns true if bot can pickup the entity pPickup
	bool     CanPickup(const edict_t* pPickup) const;

	// bot hears a sound type from a specific origin vector
	void     HearSound(eSoundType iSound, const Vector& vOrigin, edict_t* pEdict);

	// when the bot wants to think this function is called
	void     Think();

	// when an entity touches the bot this is called
	bool     Touch(edict_t* pentTouched);

	// when an entity blocks the bot, this is called
	void     Blocked(const edict_t* pentBlocked);

	// returns bot team
	int      GetTeam() const;

	// a special event
	void     BotEvent(eBotEvent iEvent, edict_t* pInfo = nullptr, edict_t* pExtInfo = nullptr, float* pFloatInfo = nullptr);

	// initialize after dying
	void     SpawnInit(bool bInit);

	// look for a new enemy
	edict_t* FindEnemy();

	//void	 EnemyLost				 ( edict_t *pEnemy );

	// does stuff when the bot finds an enemy
	void     EnemyFound(edict_t* pEnemy);

	// returns true if bot knows where the enemy went
	bool     HasSeenEnemy(const edict_t* pEnemy) const;

	// get bots vector to aim at for shooting the enemy
	Vector   GetAimVector(const edict_t* pBotEnemy);

	// get the distance from vector vec
	float    DistanceFrom(const Vector& vOrigin, bool twoD = false) const;

	// set up tasks so bot runs from the origin
	void	 RunForCover(const Vector& vOrigin, bool bDoItNow = false, int iScheduleId = 0);

	/*bool     IsReloading ()
	{
		void *pmodel = GET_MODEL_PTR( m_pEdict );
		int iReloadAnim = LookupActivity(pmodel,pev,ACT_RELOAD);

		return ( pev->sequence == iReloadAnim );
	}*/

	void     Impulse(int impulse) const { pev->impulse = impulse; }
	// BUTTONS - END

	bool     EvolveInto(int species);
	short int SpeciesOnTeam(int species) const;
	short int EvolvedSpeciesOnTeam(int species);

	void	 BotChat(eBotChatType iChatType, edict_t* pChatEdict = nullptr, bool bSayNow = false);

	bool     BotCanUseBuiltStructure(const edict_t* structure) const;
	edict_t* BotCheckForWeldables();
	float    DistanceFromEdict(const edict_t* pEntity) const;

	//edict_t *NearestStructure		( int structure_type );

	//////////////////
	// NS Stuff

	bool     IsInReadyRoom() const;

	float NS_AmountOfEnergy() const
	{
		return pev->fuser3 / 10.0f;
	}

	bool     IsCommander() const;
	bool     IsMarine() const;

	bool     IsGestating() const;
	bool     IsAlien() const;
	bool     IsSkulk() const;
	bool     IsGorge() const;
	bool     IsLerk() const;
	bool     IsFade() const;
	bool     IsOnos() const;
	/////////////////
	// location stuff
	//void     GetLocation             ( void );
	//bool     GotLocation             ( void );

	//////////////////////////////////////////////////

	//edict_t* NearestUsableDoor         ( void );
	//edict_t* NearestVisibleEdict       ( const char *classname ); // returns the nearest edict with classname which is visible to the bot.
	//edict_t* NearestVisibleEdict       ( void ); // returns any nearest edict that is visible

	bool     UpdateVisibles();

	bool     BotCanTalk();	//TODO: Not implemented yet [APG]RoboCop[CL]
	//void     BotSayBuild               ( const char *msg );
	void     BotSay(const char* msg);//, const char *arg1, int team_only );

	// returns true when a bot is skulk and is sticking to a wall in NS mod.
	bool     IsWallSticking() const { return (pev->iuser4 & MASK_WALLSTICKING) == MASK_WALLSTICKING && (pev->flags & FL_ONGROUND) == 0 && pev->waterlevel < 2; }

	bool hasWeb() const;
	bool hasBlink() const;

	// returns true when bot thinks it is low on ammunition on its current weapon.
	bool LowOnAmmo() const //TODO: Required for TS and Op4CTF? [APG]RoboCop[CL]
	{
		return m_weapon.LowOnAmmo();
	} 

	bool     HasJetPack() const { return IsMarine() && (pev->iuser4 & MASK_UPGRADE_7) == MASK_UPGRADE_7; }
	bool     HasUser4Mask(int iBits) const { return (pev->iuser4 & iBits) == iBits; }

	/* Commander stuff */

	//void     BotCommander              ( void );
	//void     BotCommanderLogOut        ( void );
	//void     BotCommanderLogIn         ( void );
	//int      BotGetCommand             ( void );
	//bool     BotGiveCommand            ( void );
	//bool     BotBuild                  ( void );
	//bool     SelectPlayersForOrder     ( edict_t *pent );

	// returns true when bot can follow the player pEdict
	/*/bool CanFollow(const edict_t* pEdict) //TODO: Experimental [APG]RoboCop[CL]
	{
		//if ( pEdict == m_pEdict )
		//	return false;

		if (pEdict->v.flags & FL_FAKECLIENT)
			return false;

		if (pEdict->v.team != GetTeam())
			return false;

		return true;
	}*/

	// Checks bots weapon bitmask
	bool     HasWeapon(int iWeapon) const;

	// Does bot have ANY weapons?
	bool     HasWeapons() const
	{
		return m_iBotWeapons == 0;
	}

	//edict_t *m_pTeleExit;
};

constexpr int HUD_TEXT_LENGTH = 256;

//	0 : "Fade In/Out"
//	1 : "Credits"
//	2 : "Scan Out"
enum : std::uint8_t
{
	HUD_EFFECT_FADE_IN_OUT = 0,
	HUD_EFFECT_CREDITS = 1,
	HUD_EFFECT_FADE_SCAN_OUT = 2
};

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
	HudText(bool tooltip) : m_textParms() // Use default constructor of m_textParms
	{
		Initialise();

		if (tooltip)
		{
			SetColour1(Vector(255, 255, 0), 75);
			SetColour2(Vector(127, 127, 0), 0);

			SetPosition(0.1f, 0.6f);
			SetEffect(HUD_EFFECT_FADE_SCAN_OUT);
			SetEffectTime(0.1f);
			SetHoldTime(3.0f);
			SetFadeTime(0.1f, 1.0f);
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
	void SayMessage(const char* message, const Vector& colour1, const Vector& colour2, edict_t* pPlayer);
	void Initialise();
	void SetColour1(Vector const& colours, int alpha)
	{
		m_textParms.r1 = static_cast<byte>(colours.x);
		m_textParms.g1 = static_cast<byte>(colours.y);
		m_textParms.b1 = static_cast<byte>(colours.z);
		m_textParms.a1 = static_cast<byte>(alpha);
	}
	void SetColour2(Vector const& colours, int alpha)
	{
		m_textParms.r2 = static_cast<byte>(colours.x);
		m_textParms.g2 = static_cast<byte>(colours.y);
		m_textParms.b2 = static_cast<byte>(colours.z);
		m_textParms.a2 = static_cast<byte>(alpha);
	}
	void InitMessage(const char* message);

private:
	hudtextparms_t m_textParms;
	char m_sMessage[HUD_TEXT_LENGTH];
};

///////////////////////////////////////////////////////////
// STRING STORAGE

constexpr int STRING_HASHES = 26;

// class of strings
class CStoredStrings
{
public:

	CStoredStrings()
	{
		for (dataStack<char*>& i : szStringsHead)
			i.Init();
	}

	void FreeStrings()
	{
		for (dataStack<char*>& i : szStringsHead)
		{
			dataStack<char*> s_tempStack = i;

			while (!s_tempStack.IsEmpty())
			{
				if (void* pFree = s_tempStack.ChooseFromStack())
				{
					std::free(pFree);
					pFree = nullptr;
				}
			}

			i.Destroy();
		}
	}

	char* GetString(const char* szString)
	{
		char iHashNum = szString[0];

		if (iHashNum < 0)
			iHashNum = -iHashNum;

		const int iHashValue = iHashNum % STRING_HASHES;

		assert(iHashValue >= 0);
		assert(iHashValue < STRING_HASHES);

		if (iHashValue < 0 || iHashValue >= STRING_HASHES)
			return nullptr; // problem...

		dataStack<char*> s_tempStack = szStringsHead[iHashValue];

		while (!s_tempStack.IsEmpty())
		{
			char* l_RetString = s_tempStack.ChooseFromStack();

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

		char* szNewString = static_cast<char*>(std::malloc(sizeof(char) * std::strlen(szString) + 1));
		if (szNewString == nullptr) {
			// handle error, for example throw an exception or return [APG]RoboCop[CL]
			throw std::bad_alloc();
		}
		std::strcpy(szNewString, szString);

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

constexpr int MAX_STORED_AUTOWAYPOINT = 5;

constexpr int CLIENT_CMD_PROJ_REPEL = 1;
constexpr int CLIENT_CMD_GREN_DEACT = 2;

constexpr int JUMP_TYPE_JUMP = 0;
constexpr int JUMP_TYPE_STUNT = 1;

class CClient
{
public:

	CClient()
	{
		Init();
	}

	void Think();

	void Init()
	{
		std::memset(this, 0, sizeof(CClient));

		m_iCurrentWaypoint = -1;
		m_iPlayerId = -1;
		m_iPathWaypointCreateIndex = -1;
		m_iPathWaypointRemoveIndex = -1;
		m_iLastMenuItemSelected = -1;
		m_Tooltips.Clear();

		m_iDebugBot = -1;
	}

	char* GetPass()
	{
		return m_szPass;
	}

	~CClient()
	{
		if (m_vTeleportVector)
		{
			delete m_vTeleportVector;
			m_vTeleportVector = nullptr;
		}
		m_iWonId = 0;
	}

	char* steamID()
	{
		return m_szSteamId;
	}

	bool IsUsed() const
	{
		return m_pPlayer != nullptr;
	}

	void SetAccessLevel(int iAccessLevel)
	{
		m_iAccessLevel = iAccessLevel;
	}

	int GetAccessLevel() const
	{
		return m_iAccessLevel;
	}

	bool IsPlayer(edict_t* pPlayer) const
	{
		return m_pPlayer == pPlayer;
	}

	edict_t* GetPlayer() const
	{
		return m_pPlayer;
	}

	bool HasPlayerName(const char* szPlayerName) const
	{
		// Pointers the same somehow?? Well thats the same :)
		if (szPlayerName == const_cast<char*>(STRING(m_pPlayer->v.netname)))
			return true;

		return std::strcmp(szPlayerName, STRING(m_pPlayer->v.netname)) == 0;
	}

	unsigned int GetWonId() const
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

	int GetPlayerRepId() const
	{
		return m_iPlayerId;
	}

	void RemoveTeleportVector()
	{
		if (m_vTeleportVector)
		{
			delete m_vTeleportVector;
			m_vTeleportVector = nullptr;
		}
	}

	void setSteamID();

	void SetTeleportVector(Vector const& vOrigin)
	{
		RemoveTeleportVector();
		m_vTeleportVector = new Vector(vOrigin);
	}

	const Vector* GetTeleportVector() const
	{
		return m_vTeleportVector;
	}

	void addCmd(int cmd) { m_iCmd |= cmd; }
	void remCmd(int cmd) { m_iCmd &= ~cmd; }
	bool hasCmd(int cmd) const { return (m_iCmd & cmd) == cmd; }
	void toggleCmd(int cmd) { if (hasCmd(cmd)) remCmd(cmd); else addCmd(cmd); }

	void FreeGlobalMemory();

	edict_t* m_pForceGripEntity;

	bool m_bNoTouch;

	bool m_bWaypointOn;
	bool m_bWaypointPathsOn;

	float m_fWaypointDisplayTime;
	int m_iCurrentWaypoint;

	CPendingToolTips m_Tooltips;

	CBotMenu* m_pMenu;

	int m_iPathWaypointCreateIndex;
	int m_iPathWaypointRemoveIndex;

	int m_iLastMenuItemSelected;

	float m_fJoinServerTime;

	bool m_bRecheckAuth;

	// the bot the player is debugging
	int m_iDebugBot;
	edict_t* m_pDebugEnt;
	float m_fDebugEntTime;

	// player sets password for access
	void SetPass(const char* szPass)
	{
		std::strncpy(m_szPass, szPass, BOT_MAX_PASSWORD_LEN - 1);
		m_szPass[BOT_MAX_PASSWORD_LEN - 1] = 0;

		m_bRecheckAuth = true;
	}

	// check if player has already had this tool-tip sent to them
	bool HasToolTipSent(eToolTip iToolTip) const
	{
		return (m_iToolTipsSent & 1 << static_cast<int>(iToolTip)) != 0;
	}

	// sets that the tool tip has been sent
	void ToolTipSent(eToolTip iToolTip)
	{
		m_iToolTipsSent |= 1 << static_cast<int>(iToolTip);
	}

	// sets auto waypoint on/off or toggle
	void AutoWaypoint(int state = -1)
	{
		m_bDebugAutoWaypoint = false;
		m_bSetUpAutoWaypoint = false;

		if (state == 2)
			m_bDebugAutoWaypoint = true;

		if (state == -1)
			m_bAutoWaypoint = !m_bAutoWaypoint;
		else
		{
			m_bAutoWaypoint = (state > 0);
		}
	}

	bool IsAutoWaypointOn() const
	{
		return m_bAutoWaypoint;
	}

	void TFCTeleExitDestroyed()
	{
		if (m_pTFCTeleEntrance.Get())
			m_pTFCTeleEntrance.Get()->v.euser1 = nullptr;

		m_pTFCTeleExit.Set(nullptr);
	}

	void setTFCTeleExit(edict_t* pEntity)
	{
		m_pTFCTeleExit.Set(pEntity);
	}

	edict_t* getTFCTeleExit() const
	{
		return m_pTFCTeleExit.Get();
	}

	void TFCTeleEntranceDestroyed()
	{
		m_pTFCTeleEntrance.Set(nullptr);
	}

	void setTFCTeleEntrance(edict_t* pEntity)
	{
		m_pTFCTeleEntrance.Set(pEntity);
	}

	edict_t* getTFCTeleEntrance() const
	{
		return m_pTFCTeleEntrance.Get();
	}

	void TFCSentryDestroyed()
	{
		m_pSentry.Set(nullptr);
	}

	void setTFCSentry(edict_t* pEntity)
	{
		m_pSentry.Set(pEntity);
	}

	edict_t* getTFCSentry() const
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
	bool m_bAutoWaypoint;

	CAutoWaypointCheck m_vLastAutoWaypointCheckPos[MAX_STORED_AUTOWAYPOINT];
	Vector m_vLastAutoWaypointPlacePos;
	float m_fLastAutoWaypointCheckTime;
	bool m_bSetUpAutoWaypoint;
	bool m_bDebugAutoWaypoint;

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
		std::memset(m_Clients, 0, sizeof(CClient) * 32);
		//m_Clients.SetSize(32);
	}

	edict_t* FindClient(const char* szPlayerName) const
	{
		for (const CClient& m_Client : m_Clients)
		{
			if (m_Client.IsUsed())
			{
				if (m_Client.HasPlayerName(szPlayerName))
					return m_Client.GetPlayer();
			}
		}

		return nullptr;
	}

	CClient* GetClientByRepId(int iRepId);

	CClient* ClientConnected(edict_t* pPlayer);

	void ClientDisconnected(edict_t* pPlayer);

	static void ClientDisconnected(CClient* pClient);

	CClient* GetClientByIndex(int iIndex)
	{
		if (iIndex >= 0 && iIndex < MAX_PLAYERS)
			return &m_Clients[iIndex];

		return nullptr;
	}

	CClient* GetClientByEdict(edict_t* pPlayer)
	{
		if (pPlayer == nullptr)
			return nullptr;

		int iIndex = ENTINDEX(pPlayer);

		if (iIndex <= gpGlobals->maxClients)
		{
			iIndex--;

			if (iIndex >= 0)
			{
				if (m_Clients[iIndex].GetPlayer() == nullptr)
					return nullptr;

				return &m_Clients[iIndex];
			}
		}

		return nullptr;

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

	void FreeGlobalMemory()
	{
		for (CClient& m_Client : this->m_Clients)
		{
			m_Client.FreeGlobalMemory();
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
		std::memset(this, 0, sizeof(CAllowedPlayer));
	}

	CAllowedPlayer(const char* szName, const char* szPass, int iAccessLevel, const char* szSteamId)
	{
		std::strcpy(m_szName, szName);
		std::strncpy(m_szPass, szPass, BOT_MAX_PASSWORD_LEN - 1);
		m_szPass[BOT_MAX_PASSWORD_LEN - 1] = 0;
		std::strncpy(m_szSteamId, szSteamId, STEAM_ID_LEN - 1);
		m_szSteamId[STEAM_ID_LEN - 1] = 0;
		m_iAccessLevel = iAccessLevel;
	}

	bool steamID_defined() const
	{
		return m_szSteamId[0] != 0;
	}

	bool IsForSteamID(const char* steamID) const
	{
		// gotta take care if no steam id is mentioned, could be WON we are playing
		if (steamID && *steamID && *m_szSteamId) //m_szSteamId always true [APG]RoboCop[CL]
			return std::strcmp(m_szSteamId, steamID) == 0;

		return false;
	}

	bool operator == (const CAllowedPlayer& player) const
	{
		// = Steam ID , or = name + password
		return (player.IsForName(m_szName) && player.IsForPass(m_szPass)) || player.IsForSteamID(m_szSteamId);
	}

	bool IsForName(const char* szName) const
	{
		return std::strcmp(szName, m_szName) == 0;
	}

	bool IsForPass(const char* szPass /*const unsigned int iWonId*/) const
	{
		return FStrEq(m_szPass, szPass);//((unsigned int)m_iWonid == (unsigned int)iWonId);
	}

	bool IsForClient(CClient* pClient) const;

	void GiveClientAccess(CClient* pClient) const
	{
		pClient->SetAccessLevel(m_iAccessLevel);
	}

	void ShowUser(edict_t* pEntity)
	{
		BotMessage(pEntity, 0, R"(name="%s", pass="%s", accesslevel=%d, steamID="%s")", m_szName, m_szPass, m_iAccessLevel, m_szSteamId);
	}

	//	unsigned int GetWonId ( void )
	//	{
	//		return m_iWonid;
	//	}

	const char* GetName() const
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
		std::memset(this, 0, sizeof(CAllowedPlayers));
	}

	void ShowUsers(edict_t* pEntity) const
	{
		dataStack<CAllowedPlayer> tempStack = m_AllowedPlayers;

		while (!tempStack.IsEmpty())
		{
			CAllowedPlayer* pPlayer = tempStack.ChoosePointerFromStack();

			pPlayer->ShowUser(pEntity);
		}
	}

	void AddPlayer(const char* szName, const char* szPass, int iAccessLevel, const char* szSteamId)
	{
		const CAllowedPlayer PlayerToAdd = CAllowedPlayer(szName, szPass, iAccessLevel, szSteamId);

		// remove old one if it exists
		m_AllowedPlayers.Remove(PlayerToAdd);

		// add new
		m_AllowedPlayers.Push(PlayerToAdd);
	}

	void RemovePlayer(const char* szName, const char* szPass, const char* szSteamId)
	{
		m_AllowedPlayers.Remove(CAllowedPlayer(szName, szPass, 0, szSteamId));
	}

	CAllowedPlayer* GetPlayer(CClient* pClient) const
	{
		dataStack<CAllowedPlayer> tempStack = m_AllowedPlayers;

		while (!tempStack.IsEmpty())
		{
			CAllowedPlayer* Player = tempStack.ChoosePointerFromStack();

			if (Player->IsForClient(pClient))
			{
				tempStack.Init();

				return Player;
			}
		}

		return nullptr;
	}

	void Destroy()
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
	virtual ~CBotCvar() = default;

	CBotCvar()
	{
		m_iAccessLevel = 0;
		m_bCanUseOnDedicatedServer = false;
		m_szCvarName = nullptr;
	}

	const char* GetCommandName() const
	{
		return m_szCvarName;
	}

	virtual eBotCvarState action(CClient* pClient, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
	{
		return BOT_CVAR_ERROR;
	}

	bool error() const
	{
		return !m_bCanUseOnDedicatedServer && IS_DEDICATED_SERVER();
	}

	void setupCommand(const char* szCvarName, int iAccessLevel, bool bCanUseOnDedicatedServer = true);

	bool needAccess(int iAccessLevel) const
	{
		if (m_iAccessLevel > 0)
			return (iAccessLevel & m_iAccessLevel) == 0;

		return false;
	}

	bool IsCvar(const char* szCvarName) const
	{
		if (szCvarName == m_szCvarName)
			return true; // Same pointers, same string

		// other wise do a string compare
		return std::strcmp(szCvarName, m_szCvarName) == 0;
	}

	virtual void showHelp(edict_t* pEntity)
	{
		BotMessage(pEntity, 0, "There is no help for this command");
		//return;
	}

private:

	int m_iAccessLevel;
	char* m_szCvarName;
	bool m_bCanUseOnDedicatedServer;
};

class CBotCvarList
{
public:

	void Destroy()
	{
		dataStack<CBotCvar*> l_tempCvars = m_Cvars;

		while (!l_tempCvars.IsEmpty())
		{
			const CBotCvar* l_Cvar = l_tempCvars.ChooseFromStack();

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

	CBotCvar* GetCvar(const char* szCvarName) const
	{
		if (szCvarName == nullptr)
			return nullptr;

		dataStack<CBotCvar*> l_tempCvars = m_Cvars;

		while (!l_tempCvars.IsEmpty())
		{
			CBotCvar* l_Cvar = l_tempCvars.ChooseFromStack();

			if (l_Cvar->IsCvar(szCvarName))
			{
				l_tempCvars.Init();

				return l_Cvar;
			}
		}

		return nullptr;
	}

	void PrintCommands(edict_t* pEdict) const
	{
		dataStack<CBotCvar*> tempStack = m_Cvars;

		while (!tempStack.IsEmpty())
		{
			const CBotCvar* pCommand = tempStack.ChooseFromStack();

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
		std::strcpy(m_szModFolder, szModFolder);
		std::strcpy(m_szDllFile, szDllFile);
		m_iModId = iModId;
	}

	bool IsForMod(const char* szFolder) const
	{
		return strcmpi(szFolder, m_szModFolder) == 0;
	}

	short GetModId() const
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
	CModInfo* GetModInfo(const char* szModFolder) const
	{
		dataStack<CModInfo> tempStack = m_Mods;

		while (!tempStack.IsEmpty())
		{
			CModInfo* pInfo = tempStack.ChoosePointerFromStack();

			if (pInfo->IsForMod(szModFolder))
			{
				tempStack.Init();

				return pInfo;
			}
		}

		return nullptr;
	}

	// at the end, free memory
	void DestroyInfo()
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

constexpr int BOT_MAX_HIVES = 3;

///////////////////////////
// MASTER STUFF
//
// made this myself to get the state of doors that use masters
//
typedef enum : std::uint8_t { MASTER_NONE, MASTER_NOT_TRIGGERED, MASTER_TRIGGERED, MASTER_FAULT } eMasterType;

class CMasterEntity
{
public:
	CMasterEntity(edict_t* pEntity, const char* szMasterName)
	{
		m_pEntity = pEntity;
		m_szMasterName = new char[std::strlen(szMasterName) + 1];
		std::strcpy(m_szMasterName, szMasterName);
	}

	~CMasterEntity()
	{
		delete m_szMasterName;
		m_szMasterName = nullptr;
		m_pEntity = nullptr;
	}

	bool IsForEntity(edict_t* pEntity) const
	{
		return m_pEntity == pEntity;
	}

	eMasterType CanFire(edict_t* pActivator) const;

	edict_t* FindButton(const Vector& vOrigin) const;

private:
	char* m_szMasterName;
	edict_t* m_pEntity;
};

// list of all entities that use masters
class CMasterEntities
{
public:
	void FreeLocalMemory()
	{
		dataStack<CMasterEntity*> temp = m_Masters;

		while (!temp.IsEmpty())
		{
			const CMasterEntity* m = temp.ChooseFromStack();

			delete m;
			m = nullptr;
		}

		m_Masters.Destroy();
	}

	void AddMaster(edict_t* pEntity, const char* szMasterName)
	{
		m_Masters.Push(new CMasterEntity(pEntity, szMasterName));
	}

	eMasterType EntityCanFire(edict_t* pEntity, edict_t* pActivator) const
	{
		if (const CMasterEntity* pMaster = GetMaster(pEntity))
			return pMaster->CanFire(pActivator);

		return MASTER_NONE;
	}

	edict_t* GetButtonForEntity(edict_t* pEntity, Vector const& vOrigin) const
	{
		if (const CMasterEntity* pMaster = GetMaster(pEntity))
			return pMaster->FindButton(vOrigin);

		return nullptr;
	}

private:

	CMasterEntity* GetMaster(edict_t* pEntity) const
	{
		dataStack<CMasterEntity*> tempStack = m_Masters;

		while (!tempStack.IsEmpty())
		{
			CMasterEntity* pMaster = tempStack.ChooseFromStack();

			if (pMaster->IsForEntity(pEntity))
			{
				tempStack.Init();

				return pMaster;
			}
		}

		return nullptr;
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
		m_pEntity = nullptr;
		m_fEndAttack = 0.0f;
		m_fPrevHealth = 0.0f;
	}

	CStructure(edict_t* pEdict)
	{
		m_pEntity = pEdict;
		m_fEndAttack = 0.0f;
		m_fPrevHealth = m_pEntity->v.health;
	}

	bool IsUnderAttack() const
	{
		return m_fEndAttack > gpGlobals->time;
	}

	void UnderAttack()
	{
		// set under attack for 1 second
		m_fEndAttack = gpGlobals->time + 1.0f;
	}

	void Update()
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
			m_fPrevHealth = 0.0f;
	}

	bool IsValid() const
	{
		return m_pEntity && !m_pEntity->free &&
			m_pEntity->v.health && (EntityIsAlienStruct(m_pEntity) || EntityIsMarineStruct(m_pEntity));
	}

	int GetStructureType() const
	{
		return m_pEntity->v.iuser3;
	}

	edict_t* GetEntity() const
	{
		return m_pEntity;
	}

	bool operator == (CStructure pStruct) const
	{
		return m_pEntity == pStruct.GetEntity();
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
constexpr int ALIEN_STRUCT_HASH_MAX = 10;

class CStructures
{
public:

	void AddStructure(edict_t* pStructure, int iHashVal)
	{
		const int iIndex = iHashVal % ALIEN_STRUCT_HASH_MAX;

		if (!m_Structures[iIndex].IsMember(pStructure))
			m_Structures[iIndex].Push(CStructure(pStructure));
	}

	void FreeLocalMemory()
	{
		for (dataStack<CStructure>& m_Structure : m_Structures)
			m_Structure.Destroy();
	}

	// keep checking hurt structures
	edict_t* Tick(int* iBestPriority)
	{
		int iPriority;

		edict_t* pUnderAttackStruct = nullptr;

		for (dataStack<CStructure>& m_Structure : m_Structures)
		{
			dataStack<CStructure> tempStack = m_Structure;

			while (!tempStack.IsEmpty())
			{
				CStructure* pStructure = tempStack.ChoosePointerFromStack();

				if (pStructure->IsValid())
				{
					pStructure->Update();

					if (pStructure->IsUnderAttack())
					{
						switch (pStructure->GetStructureType())
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
							iPriority = 7;
							break;
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

						if (iPriority > *iBestPriority)
						{
							pUnderAttackStruct = pStructure->GetEntity();
							*iBestPriority = iPriority;
						}
					}
				}
				else
				{
					// Remove this
					m_Structure.RemoveByPointer(pStructure);

					// structure changed, restart
					tempStack = m_Structure;
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

	void GetThingsToBuild(int* iOffs, int* iDefs, int* iMovs, int* iSens) const
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

	void Clear()
	{
		std::memset(this, 0, sizeof(CThingsToBuild));
	}

	CThingToBuild m_forHive;
	CThingToBuild m_forResTower;
	CThingToBuild m_forOffenseChamber;
	CThingToBuild m_forDefenseChamber;
	CThingToBuild m_forSensoryChamber;
	CThingToBuild m_forMovementChamber;
};

typedef enum : std::uint8_t
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

	bool TuneIn(edict_t* pPlayer) const;

	static void TuneOff(const edict_t* pPlayer);

	bool IsWorking() const;

	bool BotHasEnemy() const
	{
		if (!m_pCurrentBot)
			return false;

		return m_pCurrentBot->m_pEnemy != nullptr;
	}

	void SetCurrentBot(CBot* pBot)
	{
		m_pCurrentBot = pBot;
		m_fNextChangeBotTime = gpGlobals->time + RANDOM_FLOAT(5.0f, 7.5f);
		m_fNextChangeState = gpGlobals->time;
	}

private:
	CBot* m_pCurrentBot;
	eCamLookState m_iState;
	edict_t* m_pCameraEdict;
	float m_fNextChangeBotTime;
	float m_fNextChangeState;
	bool m_bTriedToSpawn;
	//float m_fThinkTime;
	TraceResult tr;
	int m_iPositionSet;
	Vector vBotOrigin;

	//HudText m_Hudtext;
	//bool m_TunedIn[MAX_PLAYERS];
};

class CTFCGoal
{
public:

	void init()
	{
		std::memset(this, 0, sizeof(CTFCGoal));
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

	bool isEdict(edict_t* pEdict) const
	{
		return m_pEntity == pEdict;
	}

	void setTeam(int team)
	{
		m_iTeam = team;
	}

	bool isForTeam(int team) const
	{
		return m_iTeam == team;
	}

	void setGoal(int goal)
	{
		m_iGoal = goal;
	}

	void setGroup(int group)
	{
		m_iGroup = group;
	}

	int getGroup() const
	{
		return m_iGroup;
	}

	int getTeam() const
	{
		return m_iTeam;
	}

	int getGoal() const
	{
		return m_iGoal;
	}

	bool isForGroup(int iGroup) const
	{
		return m_iGroup;
	}

	bool isForGoal(int goal) const
	{
		return m_iGoal == goal;
	}

	edict_t* edict() const
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
		std::memset(this,0,sizeof(CTFCFlag));
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

	inline bool isEdict ( edict_t *pEdict )
	{
		return (m_pEntity == pEdict);
	}

	inline void setTeam ( int team )
	{
		m_iTeam = team;
	}

	inline bool isForTeam ( int team )
	{
		return m_iTeam == team;
	}

	inline void setGoal ( int goal )
	{
		m_iGoal = goal;
	}

	inline bool isGroup ( int group )
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

	static edict_t* getFlag(int team, int active_goal)
	{
		return nullptr;
	}

	bool isFlag(edict_t* pFlag, int team, bool bEnemyFlag = false) const
	{
		dataStack<CTFCGoal> tempStack = m_Flags;

		while (!tempStack.IsEmpty())
		{
			const CTFCGoal* pGotFlag = tempStack.ChoosePointerFromStack();

			if (pGotFlag->isEdict(pFlag))
			{
				bool bValid;

				if (bEnemyFlag) // for demoman, e.g., to pipe enemy flags
					bValid = pGotFlag->getTeam() && !pGotFlag->isForTeam(team);
				else
					bValid = pGotFlag->isForTeam(0) || pGotFlag->isForTeam(team);

				if (bValid)
				{
					tempStack.Init();

					return true;
				}
			}
		}

		return false;
	}

	bool getFlagInfo(edict_t* pFlag, int* group, int* goal, int* team) const
	{
		dataStack<CTFCGoal> tempStack = m_Flags;

		while (!tempStack.IsEmpty())
		{
			const CTFCGoal* pGotFlag = tempStack.ChoosePointerFromStack();

			if (pGotFlag->isEdict(pFlag))
			{
				*group = pGotFlag->getGroup();
				*team = pGotFlag->getTeam();
				*goal = pGotFlag->getGoal();

				tempStack.Init();

				return true;
			}
		}

		return false;
	}

	edict_t* getRandomHeldFlagByTeam(int team) const
	{
		dataStack<CTFCGoal> tempStack = m_Flags;

		while (!tempStack.IsEmpty())
		{
			const CTFCGoal* pGotFlag = tempStack.ChoosePointerFromStack();

			edict_t* pFlag = pGotFlag->edict();

			if (pFlag->v.owner && UTIL_GetTeam(pFlag->v.owner) == team)
			{
				tempStack.Init();
				return pFlag;
			}
		}

		return nullptr;
	}

	bool playerHasFlag(edict_t* pPlayer) const
	{
		dataStack<CTFCGoal> tempStack = m_Flags;

		while (!tempStack.IsEmpty())
		{
			const CTFCGoal* pGotFlag = tempStack.ChoosePointerFromStack();

			const edict_t* pFlag = pGotFlag->edict();

			if (pFlag->v.owner == pPlayer)
			{
				tempStack.Init();
				return true;
			}
		}

		return false;
	}

private:
	dataStack<CTFCGoal> m_Flags;
};

/*class CTFCCapturePoint : public CTFCGoal
{
public:

	void init ()
	{
		std::memset(this,0,sizeof(CTFCCapturePoint));
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

	inline bool isForTeam ( int team )
	{
		return (m_iTeam == team);
	}

	inline bool isEdict ( edict_t *pEdict )
	{
		return (m_pEntity == pEdict);
	}

	inline bool isForGroup ( int iGroup )
	{
		return m_iGroup;
	}

	inline void setGroup ( int group )
	{
		m_iGroup = group;
	}

	inline bool isForGoal ( int goal )
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

	edict_t* getCapturePoint(int group, int goal, int team, bool noInfo = false);

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
		m_bAvailable = false;
		m_iMessageId = static_cast<AvHMessageID>(0);

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

	bool operator == (const CBotNSTech& other) const
	{
		return other.getID() == getID();
	}

	AvHMessageID getID() const
	{
		return m_iMessageId;
	}

	bool getAvailable() const
	{
		return m_bAvailable;
	}

	int getCost() const
	{
		return m_iCost;
	}

	void update(const CBotNSTech& tech)
	{
		m_bAvailable = tech.getAvailable();
		m_iCost = tech.getCost();
	}

private:
	int m_iCost;
	short int m_iSlot;
	short int m_iRad;
	bool m_bAvailable;
	AvHMessageID m_iMessageId;
};

class CBotNSTechs
{
public:
	void addTech(const CBotNSTech& tech)
	{
		if (!m_Techs.IsMember(tech))
			m_Techs.Add(tech);
		else
		{
			if (CBotNSTech* pExist = m_Techs.getExisting(tech))
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
				pvTechs.Add(&m_Techs[i]);
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
		for (CBot& m_Bot : m_Bots)
		{
			CBot* pOtherBot = &m_Bot;

			if (pOtherBot == pBot)
				continue;

			if (pOtherBot->m_iCurrentWaypointIndex == iWpt)
				return pOtherBot;
		}

		return nullptr;
	}

	void saveLearnedData() const;
	void loadLearnedData();

	void printBotBoredom(edict_t* pEdictTo)
	{
		for (CBot& m_Bot : m_Bots)
		{
			const CBot* pBot = &m_Bot;

			if (pBot->IsUsed())
			{
				pBot->printBoredoms(pEdictTo);
			}
		}
	}
	void ReadBotFolder();

	TraceResult* Traceline(Vector& vecStart, Vector& vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t* pentIgnore)
	{
		UTIL_TraceLine(vecStart, vecEnd, igmon, ignoreGlass, pentIgnore, &m_tr);

		return &m_tr;
	}

	bool NetMessageStarted(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);

	void ReadThingsToBuild() const;

	bool IsConfigSettingOn(int iConfigSetting) const
		// returns a True value if the config setting iConfigSetting
		// is On, False if Off..
	{
		return (m_iConfigSettings & iConfigSetting) > 0;
	}

	void ChangeConfigSetting(int iConfigSetting, int State)
		// Put the config setting iConfigSetting on or off depending on iState
	{
		if (State == 0)
			m_iConfigSettings &= ~iConfigSetting;
		else
			m_iConfigSettings |= iConfigSetting;
	}

	void StartFrame();

	void ReadConfig();

	// Free stuff after game is finished / DLL is detached
	void FreeGlobalMemory();

	// Free stuff after map changes
	void FreeLocalMemory();

	// initializing on map change
	void MapInit();

	//quicker "is NS" test
	bool IsNS() const
	{
		return m_bIsNS;
	}

	bool m_bIsNS;

	//Science and Industry
	int m_iTeamCash;
	int m_iSciCount;

	int m_iCarry;
	int m_iCarryTeam;
	int m_iCarryClass;

	int m_iGoal;
	int m_iGoalTeam;
	int m_iGoalClass;

	int m_iNotice;
	int m_iNoticeTeam;
	int m_iNoticeClass;

	int m_iVote;
	int m_iVoteTeam;
	int m_iVoteClass;

	// get number of clients trying to join
	// can sometimes be buggy because of HL's clientDisconnect
	int GetNumJoiningClients() const
	{
		int iNum = 0;

		for (const int m_iJoiningClient : m_iJoiningClients)
			iNum += m_iJoiningClient;

		return iNum;
	}

	edict_t* GetCommander() const
	{
		return m_pCommander.Get();
	}

	void SetCommander(edict_t* pComm)
	{
		m_pCommander.Set(pComm);
	}

	bool IsCombatMap() const
	{
		return m_bCombatMap;
	}

	void Init()
		// init, happens very first thing. nearly.
	{
		std::memset(&m_Squads, 0, sizeof(CBotSquads));
		std::memset(m_iJoiningClients, 0, sizeof(int) * MAX_PLAYERS);

		std::memset(m_iTeamScores, 0, sizeof(int) * MAX_TEAMS);
		//m_szWelcomeMessage = nullptr;
		//m_bIsNs3final = true;

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
		m_Mods.AddMod("ts", "mp", MOD_TS);
		m_Mods.AddMod("BumperCars", "hl", MOD_BUMPERCARS);
		m_Mods.AddMod("dmc", "dmc", MOD_DMC);
		m_Mods.AddMod("rc2", "rc2", MOD_RC2);
		m_Mods.AddMod("RockCrowbar", "rc", MOD_RC);
		//m_Mods.AddMod("SvenCoop", "hl", MOD_SVENCOOP);
		m_Mods.AddMod("valve", "hl", MOD_HL_DM);
		m_Mods.AddMod("hlrally", "hlrally", MOD_HL_RALLY);
		//m_Mods.AddMod("cstrike", "mp", MOD_COUNTERSTRIKE);
		//m_Mods.AddMod("tfc", "tfc", MOD_TFC);
		m_Mods.AddMod("rspecies", "hl", MOD_RS); // rival species meh..
		m_Mods.AddMod("gearbox", "opfor", MOD_GEARBOX); // Support for OP4CTF [APG]RoboCop[CL]
		m_Mods.AddMod("wizardwars_beta", "wizardwars", MOD_WW); // Support for Wiz Wars v1.3.0 [APG]RoboCop[CL]
		m_Mods.AddMod("wizawars", "hl", MOD_WW); // Support for Wiz Wars v1.2.9
		m_Mods.AddMod("wizardwars", "wizardwars", MOD_WW); // Support for Wiz Wars v1.2.7 [APG]RoboCop[CL]
		m_Mods.AddMod("si", "si", MOD_SI); // Support for Science and Industry [APG]RoboCop[CL]
		m_Mods.AddMod("frontline", "frontline", MOD_FLF); // Support for Front Line Force [APG]RoboCop[CL]

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
		m_Mods.AddMod("ts", "ts", MOD_TS);
		m_Mods.AddMod("BumperCars", "hl", MOD_BUMPERCARS);
		m_Mods.AddMod("dmc", "dmc", MOD_DMC);
		m_Mods.AddMod("rc2", "rc2", MOD_RC2);
		m_Mods.AddMod("RockCrowbar", "rc", MOD_RC);
		//m_Mods.AddMod("SvenCoop", "hl", MOD_SVENCOOP);
		m_Mods.AddMod("valve", "hl", MOD_HL_DM);
		m_Mods.AddMod("hlrally", "hlr", MOD_HL_RALLY);
		//m_Mods.AddMod("cstrike", "mp", MOD_COUNTERSTRIKE);
		//m_Mods.AddMod("tfc", "tfc", MOD_TFC);
		m_Mods.AddMod("rspecies", "hl", MOD_RS);
		m_Mods.AddMod("gearbox", "opfor", MOD_GEARBOX); // Support for OP4CTF [APG]RoboCop[CL]
		m_Mods.AddMod("wizardwars_beta", "wizardwars", MOD_WW); // Support for Wiz Wars v1.3.0 [APG]RoboCop[CL]
		m_Mods.AddMod("wizawars", "mp", MOD_WW); // Support for Wiz Wars v1.2.9
		m_Mods.AddMod("wizardwars", "wizardwars", MOD_WW); // Support for Wiz Wars v1.2.7 [APG]RoboCop[CL]
		m_Mods.AddMod("si", "si", MOD_SI); // Support for Science and Industry [APG]RoboCop[CL]
		m_Mods.AddMod("frontline", "front", MOD_FLF); // Support for Front Line Force [APG]RoboCop[CL]

#endif
		// same for linux & windows
		m_Mods.AddMod("bg", "bg", MOD_BG);

		// game init stuff
		//GameInit();
	}

	void setDefaults()
	{
		m_bGotTsObjectives = false;
		m_pMessageEntity = nullptr;
		m_pDebugMessage = nullptr;
		m_fWallStickTol = BOT_WAYPOINT_TOUCH_DIST;

		m_iLanguage = BOT_LANG_ENGLISH;
		m_bIsNS = false;

		//m_pTFCGroup = NULL;
		//m_pTFCDetect = NULL;
		//m_pTFCGoal = NULL;
		m_bCombatMap = false;
		m_pMarineStart = nullptr;
		//m_iJoiningClients = 0;
		m_fBotStuckSpeed = 7.0f;
		m_iMaxVisUpdateRevs = 200;
		m_fReadConfigTime = 0.0f;
		m_bAutoPathWaypoint = true;
		m_bWaypointsHavePaths = false;
		m_fUpdateVisTime = 0.0f;
		m_iDebugLevels = 0;
		m_iBotMsgIndex = -1;
		m_iCurrentMessageState = 0;
		m_iCurrentMessageState2 = 0;

		m_bGameRules = false;

		m_bIsFakeClientCommand = false;
		m_fTurnSpeed = 15.0f;

		m_CurrentMessage = nullptr;

		m_fAutoBuildTime = 0.0f;
		m_bAutoBuilt = false;

		m_bNetMessageStarted = false;

		m_CurrentHandledCvar = nullptr;

		m_pListenServerEdict = nullptr;

		m_iBotChatPercent = DEFAULT_BOT_CHAT_PERCENT;
		m_iBotChatReplyPercent = 33;

		// Set the bots array to 32 (max bots)
		//m_Bots.SetSize(32);

		//m_iNumClients = 0;

		m_fMapInitTime = 0.0f;
		m_fBotRejoinTime = 5.0f;

		m_fUpdateLadderTime = -1.0f;

		m_szModFolder = nullptr;

		m_iConfigSettings = BOT_CONFIG_TOOLTIPS | BOT_CONFIG_BLINKING;

		m_fGorgeAmount = 0.4f;

		m_fAutoWaypointCheckTime = 0.5f;

		m_uaBotModels.Init();

		m_sModelIndexFireball = 0;

		//		m_EntityMasters.Init();

		m_pCommander.Set(nullptr);

		m_fBotAimUpdate = 1.0f;
		m_fBotAimFactor = 1.0f;

		m_bHasDefTech = false;
		m_bHasSensTech = false;
		m_bHasMovTech = false;

		m_bCanUpgradeDef = false;
		m_bCanUpgradeSens = false;
		m_bCanUpgradeMov = false;

		std::memset(m_Hives, 0, sizeof(hive_info_t) * BOT_MAX_HIVES);
		//m_iNumHivesAdded = 0;

		m_fClientUpdateTime = 0.0f;
		m_bBotCanRejoin = false;

		m_fNextJoinTeam = 0.0f;
		m_iNumBots = 0;

		m_iMinBots = -1;
		m_iMaxBots = -1;

		m_iMaxPathRevs = 200;

		m_bTeamPlay = false;

		m_ThingsToBuild = nullptr;

		m_currCapPoint = nullptr;
		m_currFlag = nullptr;

		prevBackPackInvalid = false;
		prevCapturePointInvalid = false;
		prevFlagInvalid = false;

		m_fHiveImportance = 1.0f;
		m_fResTowerImportance = 0.7f;
		m_fHealingImportance = 0.75f;
		m_fStructureBuildingImportance = 0.4f;
		m_fRefillStructureImportance = 0.5f;
	}

	void LoadBotModels();

	CBotGlobals()
	{
		Init();
		m_bIsNS = false;
		m_sModelIndexFireball = 0;
		m_bWaypointsHavePaths = false;
		m_iConfigSettings = 0;
		m_bAutoPathWaypoint = false;

		m_bTeamPlay = false;
		m_fNextJoinTeam = 0.0f;
		m_fAutoWaypointCheckTime = 0.5f;
		m_fAutoBuildTime = 0.0f;
		m_bAutoBuilt = false;
		m_bHasDefTech = false;
		m_bHasSensTech = false;
		m_bHasMovTech = false;
		m_fWallStickTol = 0.0f;

		m_bNetMessageStarted = false;
		m_iMinBots = -1;
		m_iMaxBots = -1;
		m_bWelcomeMsg = true;

		m_iMaxVisUpdateRevs = 200;
		m_fHiveImportance = 1.0f;
		m_fResTowerImportance = 0.7f;
		m_fHealingImportance = 0.75f;
		m_fStructureBuildingImportance = 0.4f;
		m_fRefillStructureImportance = 0.5f;
		m_fBotStuckSpeed = 7.0f;
		m_fUpdateLadderTime = -1.0f;
		m_fUpdateVisTime = 0.0f;
		m_szModFolder = nullptr;
		m_iDebugLevels = 0;

		m_pMarineStart = nullptr;
		m_fReadConfigTime = 0.0f;
		m_iBotChatPercent = 0;
		m_iBotChatReplyPercent = 33;
		m_iBotMsgIndex = -1;
		m_iCurrentMessageState = 0;
		m_iCurrentMessageState2 = 0;
		m_iMaxPathRevs = 200;

		m_pMessageEntity = nullptr;
		m_ThingsToBuild = nullptr;
		m_CurrentMessage = nullptr;
		m_pDebugMessage = nullptr;
		m_CurrentHandledCvar = nullptr;

		m_bIsFakeClientCommand = false;
		m_iFakeArgCount = 0;
		m_iForceTeam = 0;
		m_fGorgeAmount = 0.4f;
		m_pListenServerEdict = nullptr;
		m_iWaypointTexture = 0;

		m_bGameRules = false;
		m_fClientUpdateTime = 0.0f;
		m_bBotCanRejoin = false;
		m_fMapInitTime = 0.0f;
		m_fBotRejoinTime = 5.0f;
		m_iNumBots = 0;

		m_bCanUpgradeDef = false;
		m_bCanUpgradeSens = false;
		m_bCanUpgradeMov = false;

		m_iCurrentMod = 0;
		m_fTurnSpeed = 15.0f;
		m_bCombatMap = false;

		prevBackPackInvalid = false;
		prevCapturePointInvalid = false;
		prevFlagInvalid = false;
		//m_currBackPack = nullptr;
		m_currCapPoint = nullptr;
		m_currFlag = nullptr;
	}

	void SayToolTip(edict_t* pEntity, eToolTip tooltip) const
	{
		UTIL_BotToolTip(pEntity, m_iLanguage, tooltip);
	}

	bool IsMod(int iMod) const
	{
		return m_iCurrentMod == iMod;
	}

	void SetupBotChat();

	const char* GetModInfo();

	bool IsDebugLevelOn(int iDebugLevel) const
	{
		return (m_iDebugLevels & iDebugLevel) > 0;
	}

	void KeyValue(edict_t* pentKeyvalue, const KeyValueData* pkvd);

	/*bool TFC_IsAvailableFlag(edict_t* pFlag, int team, bool bEnemyFlag = false);

	bool TFC_teamsAreAllies(int team1, int team2)
	{
		return team_allies[team1] & 1 << team2 - 1;
	}

	bool TFC_playerHasFlag(edict_t* pPlayer);
	bool TFC_getCaptureLocationForFlag(Vector* vec, edict_t* pFlag);

	int TFC_getTeamViaColorMap(edict_t* pEdict);
	int TFC_getBestClass(int prefclass, int iTeam);
	bool TFC_canGoClass(int iClass, int iTeam);
	int TFC_getBestTeam(int team);
	char* TFC_getClassName(int iClass);

	//short int m_iNumHivesAdded;

	//dataStack<entity_master_t> m_EntityMasters;
	//dataStack<entity_master_t> m_MultiManagers;

	CTypeVector<short int> TFCGoals[MAX_TEAMS];
	*/
	CBotSquads m_Squads;

	short int m_sModelIndexFireball;

	bool m_bWaypointsHavePaths;

	float m_fBotAimUpdate;
	float m_fBotAimFactor;

	CBotMenu m_Menus[BOT_MENU_MAX_ITEMS];

	int m_iConfigSettings;

	bool m_bAutoPathWaypoint;

	bool m_bTeamPlay;

	float m_fNextJoinTeam;

	float m_fAutoWaypointCheckTime;

	dataUnconstArray<char*> m_uaBotModels;

	float m_fAutoBuildTime;
	bool m_bAutoBuilt;

	CStructures m_HiveMind;
	// hack for marines
	CStructure m_CommConsole;

	bool m_bHasDefTech;
	bool m_bHasSensTech;
	bool m_bHasMovTech;

	float m_fWallStickTol;

	int m_iJoiningClients[MAX_PLAYERS];

	int m_iTeamScores[MAX_TEAMS];
	//int m_iJoiningClients;
	//int m_iNumClients;

	bool m_bNetMessageStarted;

	short int m_iMinBots;
	short int m_iMaxBots;

	bool m_bWelcomeMsg;

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
	bool m_bGotTsObjectives;
	bool m_bIsNs3final;

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

	bool m_bIsFakeClientCommand;
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

	bool m_bGameRules;

	//////////////////////////////////
	// OTHER

	float m_fClientUpdateTime;
	bool m_bBotCanRejoin;

	float m_fMapInitTime;
	float m_fBotRejoinTime;

	short int m_iNumBots;

	bool m_bCanUpgradeDef;
	bool m_bCanUpgradeSens;
	bool m_bCanUpgradeMov;

	TraceResult m_tr;

	short m_iCurrentMod;

	CGA m_TSWeaponChoices;
	CGA m_pCombatGA[2];

	//CGA m_combatGAForTeam[MAX_TEAMS];
	CGA m_enemyCostGAsForTeam[MAX_TEAMS];
	/*	CGA m_TFCspiesForTeam[MAX_TEAMS];

	edict_t* m_pTFCDetect;
	edict_t* m_pTFCGoal;*/

	void setMapType(eMapType theMapType)
	{
		m_MapType = theMapType;
	}

	int getMapType() const
	{
		return m_MapType;
	}

	bool isMapType(eMapType theMapType) const
	{
		return theMapType == m_MapType;
	}

	//TODO: Wizard Wars may require this code [APG]RoboCop[CL]
	/*edict_t* findBackpack(Vector const location, int team, int min_health, int min_cells, int min_armor, int min_ammo)
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

	bool TFC_isValidGoal(int goal, int team)
	{
		int i = 0;

		if (m_iValidGoals[team - 1].IsEmpty())
			return true; // no goals stored as valid, just return true (for normal maps e.g.)

		for (i = 0; i < m_iValidGoals[i].Size(); i++)
		{
			if (m_iValidGoals[team - 1].ReturnValueFromIndex(i) == goal)
				return true;
		}

		return false;
	}*/

	/*void changeWelcomeMessage ( char *message )
	{
			if ( m_szWelcomeMessage )
			std::free(m_szWelcomeMessage);
		m_szWelcomeMessage = strdup(message);
		}*/

	void buildFileName(const char* in_filename, char* out_filename, const size_t out_filename_size)
	{
		snprintf(out_filename, out_filename_size, "%s%s", m_szBotFolder, in_filename);
	}

	const char* botFolder() const
	{
		return m_szBotFolder;
	}

	//edict_t* randomHeldFlagOnTeam(int team);
	float m_fTurnSpeed;
private:

	char m_szBotFolder[256];

	void GameInit();

	///////////
	// NS
	bool m_bCombatMap;
	MyEHandle m_pCommander;

	///////////
	// TFC
	eMapType m_MapType;

	//TODO: To be used for WizWars
	int max_team_players[MAX_TEAMS];
	int team_class_limits[MAX_TEAMS];
	int team_allies[MAX_TEAMS];

	bool prevBackPackInvalid;
	bool prevCapturePointInvalid;
	bool prevFlagInvalid;

	//CTFCBackpacks m_Backpacks;
	//CTFCBackpack* m_currBackPack;

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

const char* GetArg(const char* command, unsigned int arg_number);
void FakeClientCommand(edict_t* pFakeClient, const char* fmt, ...);

///////////////////////////////////////////////////////////////////
// POST BOT DEFINED FUNCTIONS
// (depend on structures defined above)

void BotFunc_InitProfile(bot_profile_t* bpBotProfile);
void BotFunc_ReadProfile(std::FILE* fp, bot_profile_t* bpBotProfile);
void BotFunc_WriteProfile(std::FILE* fp, const bot_profile_t* bpBotProfile);
CBot* UTIL_GetBotPointer(const edict_t* pEdict);

bool BotFunc_EntityIsMoving(const entvars_t* pev);
edict_t* BotFunc_FindNearestButton(const Vector& vOrigin, const entvars_t* pDoor, Vector* vFoundOrigin = nullptr);

////////////////////////////////////////////////////
// NAVIGATION

void BotTurnAtWall(const CBot* pBot, const TraceResult* tr);
bool BotCantMoveForward(const CBot* pBot, TraceResult* tr);
bool BotCanJumpUp(const CBot* pBot); // BotCanJumpUp : By : Botman
bool BotCanDuckUnder(const CBot* pBot);
bool BotCheckWallOnLeft(CBot* pBot);
bool BotCheckWallOnRight(CBot* pBot);
const char* BotFunc_GetRandomPlayerName(CBot* pBot, int iState);

////////////////////////////////////////////////////
// ENGINE FUNCTIONS

void GameDLLInit();
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
void ResetGlobalState();
int ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
void ClientDisconnect(edict_t* pEntity);
void ClientKill(edict_t* pEntity);
void ClientPutInServer(edict_t* pEntity);
void ClientCommand(edict_t* pEntity);
void ClientUserInfoChanged(edict_t* pEntity, char* infobuffer);
void ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
void ServerDeactivate();
void PlayerPreThink(edict_t* pEntity);
void PlayerPostThink(edict_t* pEntity);
void StartFrame();
void ParmsNewLevel();
void ParmsChangeLevel();
const char* GetGameDescription();
void PlayerCustomization(edict_t* pEntity, customization_t* pCust);
void SpectatorConnect(edict_t* pEntity);
void SpectatorDisconnect(edict_t* pEntity);
void SpectatorThink(edict_t* pEntity);
void Sys_Error(const char* error_string);
void PM_Move(playermove_s* ppmove, int server);
void PM_Init(playermove_s* ppmove);
char PM_FindTextureType(char* name);
void SetupVisibility(edict_t* pViewEntity, edict_t* pClient, unsigned char** pvs, unsigned char** pas);
void UpdateClientData(const edict_s* ent, int sendweapons, clientdata_s* cd);
int AddToFullPack(entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet);
void CreateBaseline(int player, int eindex, entity_state_s* baseline, edict_s* entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
void RegisterEncoders();
int GetWeaponData(edict_s* player, weapon_data_s* info);
void CmdStart(const edict_t* player, const usercmd_s* cmd, unsigned int random_seed);
void CmdEnd(const edict_t* player);
int ConnectionlessPacket(const netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
int GetHullBounds(int hullnumber, float* mins, float* maxs);
void CreateInstancedBaselines();
int InconsistentFile(const edict_t* player, const char* filename, char* disconnect_message);
int AllowLagCompensation();
void RCBot_ServerCommand();
int GetModId();
float UTIL_GetAvoidAngle(const edict_t* pEdict, const Vector& origin);
void ReadBotUsersConfig();
void BotFunc_MakeSquad(CClient* pClient);
void AssertMessage(bool bAssert, char* fmt, ...);
int UTIL_GetBuildWaypoint(const Vector& vSpawn, dataStack<int>* iFailedGoals = nullptr);
int BotFunc_GetStructureForGorgeBuild(const entvars_t* pGorge, entvars_t* pEntitypev);
void BotFunc_KickBotFromTeam(int iTeam);
bool BotFunc_BreakableIsEnemy(const edict_t* pBreakable, const edict_t* pEdict);
///////////////////////////////////////////////////////
// NAVIGATION
int BotNavigate_AStarAlgo(CBot* pBot, int iFrom, int iTo, bool bContinue);
int BotNavigate_FindNextWaypoint(CBot* pBot);
bool BotNavigate_UpdateWaypoint(CBot* pBot);
PATH* BotNavigate_FindPathFromTo(int iFrom, int iTo, int iTeam);

bool UTIL_makeTSweapon(edict_t* pOwner, TSWeapon weaponid);
#endif // BOT_H
