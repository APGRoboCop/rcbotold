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
 // waypoint.h
 //
 //////////////////////////////////////////////////
 //
 // Waypoint definitions
 //

#ifndef WAYPOINT_H
#define WAYPOINT_H

#ifndef __linux__
#define strdup _strdup
#endif

#include <climits>

#include "generic_class.h"

constexpr int MAX_WPT_LOCATIONS = 32;
constexpr int MAX_WPT_LOC = 31;

class CWaypointLocations
	// Hash table of waypoint indexes accross certian
	// buckets in the map on X/Y Co-ords for quicker
	// nearest waypoint finding and waypoint displaying.
{
public:

	CWaypointLocations()
	{
		Init();
	}

	void Init()
	{
		for (dataStack<int>(&m_iLocation)[32][32] : m_iLocations)
		{
			for (dataStack<int>(&j)[32] : m_iLocation)
			{
				for (dataStack<int>& k : j)
					k.Init();
			}
		}
	}

	void Clear()
	{
		for (dataStack<int> (&m_iLocation)[32][32] : m_iLocations)
		{
			for (dataStack<int>(&j)[32] : m_iLocation)
			{
				for (dataStack<int>& k : j)
					k.Destroy();
			}
		}
	}

	static void getMaxMins(const Vector& vOrigin, int& mini, int& minj, int& mink, int& maxi, int& maxj, int& maxk);

	int GetCoverWaypoint(const Vector& vPlayerOrigin, const Vector& vCoverFrom, dataStack<int>* iIgnoreWpts) const;

	void FindNearestCoverWaypointInBucket(int i, int j, int k, const Vector& vOrigin, float* pfMinDist, int* piIndex, const dataStack<int>* iIgnoreWpts, int iCoverFromWpt) const;

	void AddWptLocation(int iIndex, const float* fOrigin);

	void FindNearestInBucket(int i, int j, int k, const Vector& vOrigin, float* pfMinDist, int* piIndex, int iIgnoreWpt, bool bGetVisible = true, bool bGetUnReachable = false, bool bIsBot = false, const dataStack<int>* iFailedWpts = nullptr, bool bNearestAimingOnly = false) const;

	void DrawWaypoints(edict_t* pEntity, const Vector& vOrigin, float fDist) const;

	void DeleteWptLocation(int iIndex, const float* fOrigin);

	int NearestWaypoint(const Vector& vOrigin, float fNearestDist, int iIgnoreWpt, bool bGetVisible = true, bool bGetUnreachable = false, bool bIsBot = false, const dataStack<int>* iFailedWpts = nullptr, bool bNearestAimingOnly = false) const;

	void FillWaypointsInBucket(int i, int j, int k, const Vector& vOrigin, dataStack<int>* iWaypoints, dataStack<int>* iFailedWpts = nullptr) const;

	void FillNearbyWaypoints(const Vector& vOrigin, dataStack<int>* iWaypoints, dataStack<int>* iFailedWpts = nullptr) const;

private:

	dataStack<int> m_iLocations[MAX_WPT_LOCATIONS][MAX_WPT_LOCATIONS][MAX_WPT_LOCATIONS];
};

constexpr float REACHABLE_RANGE = 400.0f;

// defines for waypoint flags field (32 bits are available)
#define W_FL_TEAM			((1<<0) + (1<<1))  /* allow for 4 teams (0-3) */
#define W_FL_TEAM_SPECIFIC	(1<<2)  /* waypoint only for specified team */
#define W_FL_CROUCH			(1<<3)  /* must crouch to reach this waypoint */
#define W_FL_LADDER			(1<<4)  /* waypoint on a ladder */
#define W_FL_LIFT			(1<<5)  // lift button
#define W_FL_DOOR			(1<<6)  /* wait for door to open */
#define W_FL_HEALTH			(1<<7)  /* health kit (or wall mounted) location */
#define W_FL_ARMOR			(1<<8)  /* armor (or HEV) location */
#define W_FL_AMMO			(1<<9)  /* ammo location */
#define W_FL_CHECK_LIFT		(1<<10) /* checks for lift at this point */
#define W_FL_IMPORTANT		(1<<11) /* flag position (or hostage or president) */
#define W_FL_DYNAMIC_TELEPORTER ((1<<22)&(1<<21)) /* created automatically */
#define W_FL_RESCUE			(1<<12) /* flag return position (or rescue zone) */
//#define W_FL_TFC_CAPTURE_POINT (1<<12)
#define W_FL_MARINE_BUILDING (1<<12) // Ns: marine building put here
#define W_FL_SCIENTIST_POINT (1<<11)
//#define W_FL_TFC_FLAG_POINT (1<<11)
#define W_FL_BARNEY_POINT   (1<<12)
#define W_FL_DEFEND_ZONE    (1<<13)
//#define W_FL_CROUCHJUMP		(1<<13) /* weapon(s) here */
#define W_FL_AIMING			(1<<14) /* aiming waypoint */
//#define W_FL_JUMP			(1<<15) // { Be compatible with old rcbot 1.5
#define W_FL_CROUCHJUMP		(1<<16) // }
#define W_FL_WAIT_FOR_LIFT	(1<<17)/* wait for lift to be down before approaching this waypoint */
#define W_FL_WALL_STICK		(1<<18)
#define W_FL_STUNT          (1<<18)
#define W_FL_PAIN			(1<<18)
#define W_FL_JUMP           (1<<19)
#define W_FL_WEAPON			(1<<20) // Crouch and jump
#define W_FL_TELEPORT       (1<<21)
#define W_FL_TANK			(1<<22) // func_tank near waypoint
#define W_FL_FLY			(1<<23) // grapple waypoint for sven
#define W_FL_STAY_NEAR		(1<<24)
#define W_FL_ENDLEVEL       (1<<25) // end of level, in svencoop etc
//#define W_FL_TFC_DETPACK    (1<<25)
#define W_FL_OPENS_LATER    (1<<26)
#define W_FL_HUMAN_TOWER    (1<<27) // bot will crouch & wait for a player to jump on them
#define W_FL_UNREACHABLE    (1<<28) // not reachable by bot, used as a reference point for visibility only
#define W_FL_PUSHABLE       (1<<29)
//#define W_FL_TFC_SENTRY     (1<<29)
#define W_FL_GREN_THROW     (1<<30)
//#define W_FL_TFC_SNIPER     (1<<30)
#define W_FL_DELETED		(1<<31) /* used by waypoint allocation code */

enum : std::uint8_t
{
	WAYPOINT_VERSION = 4
};

//#define WAYPOINT_VERSION 5

enum : std::uint8_t
{
	W_FILE_FL_READ_VISIBILITY = (1<<0) // require to read visibility file
};

// define the waypoint file header structure...
typedef struct
{
	char filetype[8];  // should be "RC_bot\0"
	int  waypoint_file_version;
	int  waypoint_file_flags;  // used for visualisation
	int  number_of_waypoints;
	char mapname[32];  // name of map for these waypoints
} WAYPOINT_HDR;

// define the structure for waypoints...
/*typedef struct {
   int    flags;    // button, lift, flag, health, ammo, etc.
   Vector origin;   // location
} WAYPOINT;
*/
/*
class oldWAYPOINT
{
public:
   int  flags;		// button, lift, flag, health, ammo, etc.
   Vector origin;   // location

   WAYPOINT (int f, Vector vec)	{ flags = f; origin = vec;	};
   WAYPOINT ()					{ flags = 0; Vector(0,0,0);  };
};

class WAYPOINT
{
public:
   int  flags;		// button, lift, flag, health, ammo, etc.
   Vector origin;   // location
   int aiming;
   edict_t *connect;

   WAYPOINT (int f, Vector vec)	{ flags = f; origin = vec;	};
   WAYPOINT ()					{ flags = 0; Vector(0,0,0);  };
};*/

class WAYPOINT
{
public:
	int  flags;		// button, lift, flag, health, ammo, etc.
	Vector origin;   // location

	//virtual Vector getOrigin () { return origin; }

	WAYPOINT(const int f, const Vector& vec) : flags(f), origin(vec) {}
	WAYPOINT() : flags(0), origin(0, 0, 0) {}
};

class WAYPOINT2 : public WAYPOINT
{
public:
	short iAttachedIndex;

	//Vector getOrigin () { return pAttached->v.origin + origin; }
};

class WAYPOINTS
{
public:
	WAYPOINT& operator [] (int index);
private:
	WAYPOINT m_Waypoints[MAX_WAYPOINTS];
};
//////////////////////

constexpr int MAX_BITS = 32;

class CWaypointConversion
{
private:
	int m_iConvertTo[MAX_BITS] = {}; // e.g. jump, crouch etc
	char* m_szName = nullptr;
	char* m_szFolder = nullptr;
	char* m_szExtension = nullptr;
	char* m_szHeader = nullptr;
	int m_iVersion = 0;
public:

	void FreeMemory()
	{
		//if (getName())
			std::free(getName());
		//if (getFolder())
			std::free(getFolder());
		//if (getExtension())
			std::free(getExtension());
		//if (getHeader())
			std::free(getHeader());

		init();
	}

	void setExtension(const char* szExt)
	{
		m_szExtension = strdup(szExt);
	}

	void setFolder(const char* szFolder)
	{
		m_szFolder = strdup(szFolder);
	}

	void setName(const char* szName)
	{
		m_szName = strdup(szName); // duplicate the string
	}

	void setHeader(const char* szHeader)
	{
		m_szHeader = strdup(szHeader);
	}

	void setVersion(const int iVer)
	{
		m_iVersion = iVer;
	}

	std::FILE* openWaypoint() const;

	void setConvertBit(const int iBit, const int iFlag)
	{
		m_iConvertTo[iBit] = iFlag;
	}

	void init()
	{
		m_szName = nullptr;
		m_szFolder = nullptr;
		m_szExtension = nullptr;
		m_szHeader = nullptr;
		m_iVersion = 0;

		std::memset(m_iConvertTo, 0, sizeof(int) * MAX_BITS);
	}

	char* getName() const
	{
		return m_szName;
	}

	char* getFolder() const
	{
		return m_szFolder;
	}

	char* getExtension() const
	{
		return m_szExtension;
	}

	char* getHeader() const
	{
		return m_szHeader;
	}

	WAYPOINT convert(WAYPOINT* thisWaypoint) const
	{
		WAYPOINT convertedWpt;

		std::memset(&convertedWpt, 0, sizeof(WAYPOINT));

		for (int i = 0; i < MAX_BITS; i++)
		{
			if (thisWaypoint->flags & 1 << i)
			{
				convertedWpt.flags |= 1 << m_iConvertTo[i];
			}
		}

		convertedWpt.origin = thisWaypoint->origin;

		return convertedWpt;
	}

	int getVersion() const
	{
		return m_iVersion;
	}
};
/*
class CWaypointConversions // list of
{
private:
	dataUnconstArray<CWaypointConversion*> m_Convertions;
public:
	void init ()
	{
		m_Convertions.Clear();
	}

	CWaypointConversions()
	{
		init();
	}

	void FreeMemory()
	{
		int i;

		int size = m_Convertions.Size();

		for ( i = 0; i < size; i ++ )
		{
			CWaypointConversion *c = m_Convertions.ReturnValueFromIndex(i);
			c->FreeMemory();
		}

		m_Convertions.Clear();
	}
};
*/
class CWhichbotConvert : public CWaypointConversion
{
public:
	CWhichbotConvert()
	{
		constexpr char str1[] = "Whichbot";
		setName(str1);
		constexpr char str2[] = "wpt";
		setExtension(str2);
		constexpr char str3[] = "HPB_bot";
		setHeader(str3);

		setVersion(4);

#ifndef __linux__
		char str4[] = "addons\\whichbot\\data";
		setFolder(str4);
#else
		char str4[] = "addons/whichbot/data";
		setFolder(str4);
#endif

		setConvertBit(0, 1 << 0);
		setConvertBit(1, 1 << 1);
		setConvertBit(2, 1 << 2);
		setConvertBit(3, W_FL_CROUCH);
		setConvertBit(4, W_FL_LADDER);
		setConvertBit(5, W_FL_WAIT_FOR_LIFT);
		setConvertBit(6, W_FL_DOOR);
		setConvertBit(7, W_FL_HEALTH);
		setConvertBit(8, W_FL_ARMOR);
		setConvertBit(9, W_FL_AMMO);
		setConvertBit(18, W_FL_JUMP);
		setConvertBit(9, W_FL_LIFT);
		setConvertBit(18, W_FL_JUMP);
		setConvertBit(31, W_FL_DELETED);
		/*
		#define W_FL_TEAM        ((1<<0) + (1<<1))  // allow for 4 teams (0-3)
		#define W_FL_TEAM_SPECIFIC (1<<2)  // waypoint only for specified team
		#define W_FL_CROUCH      (1<<3)  // must crouch to reach this waypoint
		#define W_FL_LADDER      (1<<4)  // waypoint on a ladder
		W_FL_WAIT_FOR_LIFT #define W_FL_LIFT        (1<<5)  // wait for lift to be down before approaching this waypoint
		#define W_FL_DOOR        (1<<6)  // wait for door to open
		#define W_FL_HEALTH      (1<<7)  // health kit (or wall mounted) location
		#define W_FL_ARMOR       (1<<8)  // armor (or HEV) location
		#define W_FL_AMMO        (1<<9)  // ammo location
		#define W_FL_JUMP        (1<<18) // must jump to reach this waypoint
		W_FL_LIFT #define W_FL_LIFT_SWITCH (1<<21) // press this switch to activate the nearby lift
		W_FL_CHECK_FOR_LIFT #define W_FL_LIFT_WAIT (1 << 22) // wait at this waypoint for the lift to move
		#define W_FL_DELETED     (1<<31) // used by waypoint allocation code*/
	}
};
/*//TODO: Allow multiple conversion for various bots [APG]RoboCop[CL]
class CWhichbotConvert : public CWaypointConversion
{
public:
  CWhichbotConvert()
  {
	  setName("Whichbot");
	  setExtension("wpt");
	  setHeader("HPB_bot");
	  setVersion(4);

#ifndef __linux__
	  setFolder("addons\\whichbot\\data");
#else
	  setFolder("addons/whichbot/data");
#endif

	  setConvertBit(10,(1<<30));

	  setConvertBit(1,(1<<1));
	  setConvertBit(2,(1<<2));
	  setConvertBit(3,W_FL_CROUCH);
	  setConvertBit(4,W_FL_LADDER);
	  setConvertBit(5,W_FL_WAIT_FOR_LIFT);
	  setConvertBit(6,W_FL_DOOR);
	  setConvertBit(7,W_FL_HEALTH);
	  setConvertBit(8,W_FL_ARMOR);
	  setConvertBit(9,W_FL_AMMO);
	  setConvertBit(18,W_FL_JUMP);
	  setConvertBit(9,W_FL_LIFT);
	  setConvertBit(18,W_FL_CHECK_LIFT);
	  setConvertBit(31,W_FL_DELETED);

#define W_FL_TEAM        ((1<<0) + (1<<1))  // allow for 4 teams (0-3) //
#define W_FL_TEAM_SPECIFIC (1<<2)  // waypoint only for specified team //
#define W_FL_CROUCH      (1<<3)  // must crouch to reach this waypoint //
#define W_FL_LADDER      (1<<4)  // waypoint on a ladder //
#define W_FL_LIFT        (1<<5)  // wait for lift to be down before approaching this waypoint //
#define W_FL_DOOR        (1<<6)  // wait for door to open //
#define W_FL_HEALTH      (1<<7)  // health kit (or wall mounted) location //
#define W_FL_ARMOR       (1<<8)  // armor (or HEV) location //
#define W_FL_AMMO        (1<<9)  //ammo location //
#define W_FL_SNIPER      (1<<10) // sniper waypoint (a good sniper spot) //

#define W_FL_FLAG        (1<<11) /// flag position (or hostage or president) //
#define W_FL_FLF_CAP     (1<<11) // Front Line Force capture point //

#define W_FL_FLAG_GOAL   (1<<12) // flag return position (or rescue zone) //
#define W_FL_FLF_DEFEND  (1<<12) // Front Line Force defend point //

#define W_FL_PRONE       (1<<13) // go prone (laying down) //
#define W_FL_AIMING      (1<<14) // aiming waypoint //

#define W_FL_SENTRYGUN   (1<<15) // sentry gun waypoint for TFC //
#define W_FL_DISPENSER   (1<<16) // dispenser waypoint for TFC //

#define W_FL_WEAPON      (1<<17) // weapon_ entity location //
#define W_FL_JUMP        (1<<18) // jump waypoint //

#define W_FL_DELETED     (1<<31) // used by waypoint allocation code //
  }
};*/
///////////////////////

constexpr int g_iMaxVisibilityByte = MAX_WAYPOINTS * MAX_WAYPOINTS / 8;

class CWaypointVisibilityTable
{
public:
	CWaypointVisibilityTable()
	{
		constexpr int iSize = g_iMaxVisibilityByte;
		//create a heap...
		m_VisTable = static_cast<unsigned char*>(std::malloc(iSize));
		if (m_VisTable != nullptr) {
			std::memset(m_VisTable, 0, iSize);
		}
		else {
			// Handle memory allocation failure [APG]RoboCop[CL]
			throw std::bad_alloc();
		}
	}

	bool SaveToFile() const;

	bool ReadFromFile() const;

	int GetVisibilityFromTo(const int iFrom, const int iTo) const
	{
		// work out the position
		const int iPosition = iFrom * MAX_WAYPOINTS + iTo;

		const int iByte = iPosition / 8;
		const int iBit = iPosition % 8;

		if (iByte < g_iMaxVisibilityByte)
		{
			const unsigned char* ToReturn = m_VisTable + iByte;

			return (*ToReturn & 1 << iBit) > 0;
		}

		return false;
	}

	void ClearVisibilityTable() const
	{
		if (m_VisTable)
			std::memset(m_VisTable, 0, g_iMaxVisibilityByte);
	}

	void FreeVisibilityTable()
	{
		if (m_VisTable != nullptr)
		{
			std::free(m_VisTable);
			m_VisTable = nullptr;
		}
	}

	void SetVisibilityFromTo(const int iFrom, const int iTo, const bool bVisible) const
	{
		const int iPosition = iFrom * MAX_WAYPOINTS + iTo;

		const int iByte = iPosition / 8;
		const int iBit = iPosition % 8;

		if (iByte < g_iMaxVisibilityByte)
		{
			unsigned char* ToChange = m_VisTable + iByte;

			if (bVisible)
				*ToChange |= 1 << iBit;
			else
				*ToChange &= ~(1 << iBit);
		}
	}

	void WorkOutVisibilityTable(int iNumWaypoints) const;

private:

	unsigned char* m_VisTable;
	// use a heap of 1 byte * size to keep things simple.
};

#define WAYPOINT_UNREACHABLE   USHRT_MAX
#define WAYPOINT_MAX_DISTANCE (USHRT_MAX-1)

//#define MAX_PATH_INDEX 4

// waypoint function prototypes...

bool WaypointFlagsOnLadderOrFly(int iWaypointFlags);
void WaypointDrawBeam(edict_t* pEntity, const Vector& start, const Vector& end, int width,
	int noise, int red, int green, int blue, int brightness, int speed);
void WaypointInit();
int  WaypointFindPath(PATH** pPath, int* path_index, int waypoint_index, int team);
int  WaypointFindNearest(const edict_t* pEntity, float range, int team);
int  WaypointFindNearest(const Vector& v_src, const edict_t* pEntity, float range, int team);
int  WaypointFindNearestGoal(const Vector& v_src, edict_t* pEntity, float range, int team, int flags, const dataStack<int>* iIgnoreWpts);
int  WaypointFindRandomGoal(edict_t* pEntity, int team, const dataStack<int>* iIgnoreWpts);
int  WaypointFindRandomGoal(edict_t* pEntity, int team, int flags, const dataStack<int>* iIgnoreWpts);
int  WaypointFindRandomGoal(const Vector& v_src, edict_t* pEntity, float range, int team, int flags, const dataStack<int>* iIgnoreWpts);
int  WaypointFindNearestAiming(const Vector& v_origin);
void WaypointAdd(const CClient* pClient);
void WaypointAddPath(short add_index, short path_index);
void WaypointAddAiming(edict_t* pEntity);
void WaypointDelete(CClient* pClient);
void WaypointDeletePath(short del_index);
void WaypointDeletePath(short path_index, short del_index);//TODO: Duplicate? [APG]RoboCopCL]
void WaypointCreatePath(CClient* pClient, int cmd);
void WaypointRemovePath(CClient* pClient, int cmd);
bool WaypointLoad(edict_t* pEntity);
bool WaypointSave(bool bVisibilityMade, const CWaypointConversion* theConverter = nullptr);
bool WaypointReachable(Vector v_src, Vector v_dest, bool bDistCheck = true);
int  WaypointFindReachable(const edict_t* pEntity, float range, int team);
void WaypointPrintInfo(edict_t* pEntity);
//void WaypointThink();
void WaypointDrawIndex(edict_t* pEntity, int index);
float WaypointDistance(const Vector& vOrigin, int iWaypointIndex);
Vector WaypointOrigin(int iWaypointIndex);
int NearestWaypointToEdict(const edict_t* pEdict, int iIgnoreWpt, const dataStack<int>* iFailedWpts);
int NearestWaypointToOrigin(const Vector& vOrigin, int iIgnoreWpt, dataStack<int>* iFailedWpts = nullptr);
int WaypointFlags(int iWaypointIndex);
int WaypointAddOrigin(const Vector& vOrigin, int iFlags, edict_t* pEntity, bool bDraw = true, bool bSound = true, bool bAutoSetFlagsForPlayer = true);
void AutoWaypoint(); //TODO: Not implemented yet [APG]RoboCop[CL]
Vector BotNavigate_ScanFOV(CBot* pBot);
edict_t* PlayerNearVector(const Vector& vOrigin, float fRange);
bool CheckLift(CBot* pBot, Vector vCheckOrigin, const Vector& vCheckToOrigin);
#endif // WAYPOINT_H
