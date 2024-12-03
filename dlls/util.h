/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// Misc utility code
//
#ifndef __UTIL_H__
#define __UTIL_H__

#ifdef _WIN32
#include <windef.h>
#endif

#ifndef ACTIVITY_H
#include "activity.h"
#endif

#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif
inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float* pOrigin, entvars_t* ent);  // implementation later in this file

extern globalvars_t* gpGlobals;

// Use this instead of ALLOC_STRING on constant strings
#define STRING(offset)		(const char *)(gpGlobals->pStringBase + (int)(offset))
#define MAKE_STRING(str)	((int)(str) - (int)STRING(0))

inline edict_t* FIND_ENTITY_BY_CLASSNAME(edict_t* entStart, const char* pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "classname", pszName);
}

inline edict_t* FIND_ENTITY_BY_TARGETNAME(edict_t* entStart, const char* pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "targetname", pszName);
}

// for doing a reverse lookup. Say you have a door, and want to find its button.
inline edict_t* FIND_ENTITY_BY_TARGET(edict_t* entStart, const char* pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "target", pszName);
}

// Keeps clutter down a bit, when writing key-value pairs
#define WRITEKEY_INT(pf, szKeyName, iKeyValue)									\
		ENGINE_FPRINTF(pf, "\"%s\" \"%d\"\n", szKeyName, iKeyValue)
#define WRITEKEY_FLOAT(pf, szKeyName, flKeyValue)								\
		ENGINE_FPRINTF(pf, "\"%s\" \"%f\"\n", szKeyName, flKeyValue)
#define WRITEKEY_STRING(pf, szKeyName, szKeyValue)								\
		ENGINE_FPRINTF(pf, "\"%s\" \"%s\"\n", szKeyName, szKeyValue)
#define WRITEKEY_VECTOR(pf, szKeyName, flX, flY, flZ)							\
		ENGINE_FPRINTF(pf, "\"%s\" \"%f %f %f\"\n", szKeyName, flX, flY, flZ)

// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits)		((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)	((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)		((int)(flBitVector) & (bit))

// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL

// Until we figure out why "const" gives the compiler problems, we'll just have to use
// this bogus "empty" define to mark things as constant.
#define CONSTANT

// More explicit than "int"
typedef int EOFFSET;

// In case it's not alread defined
typedef int BOOL;

// In case this ever changes
#define M_PI			3.14159265358979323846

// Keeps clutter down a bit, when declaring external entity/global method prototypes
#define DECLARE_GLOBAL_METHOD(MethodName) \
		extern void DLLEXPORT MethodName( void )
#define GLOBAL_METHOD(funcname)					void DLLEXPORT funcname(void)

// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) \
	extern "C" EXPORT void mapClassName( entvars_t *pev ); \
	void mapClassName( entvars_t *pev ) { GetClassPtr( (DLLClassName *)pev ); }

//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef DEBUG
extern edict_t* DBG_EntOfVars(const entvars_t* pev);
inline edict_t* ENT(const entvars_t* pev) { return DBG_EntOfVars(pev); }
#else
inline edict_t* ENT(const entvars_t* pev) { return pev->pContainingEntity; }
#endif
inline edict_t* ENT(edict_t* pent) { return pent; }
inline edict_t* ENT(EOFFSET eoffset) { return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
inline EOFFSET OFFSET(EOFFSET eoffset) { return eoffset; }
inline EOFFSET OFFSET(const edict_t* pent)
{
#ifdef _DEBUG
	if (!pent)
		ALERT(at_error, "Bad ent in OFFSET()\n");
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent);
}
inline EOFFSET OFFSET(entvars_t* pev)
{
#ifdef _DEBUG
	if (!pev)
		ALERT(at_error, "Bad pev in OFFSET()\n");
#endif
	return OFFSET(ENT(pev));
}
inline entvars_t* VARS(entvars_t* pev) { return pev; }

inline entvars_t* VARS(edict_t* pent)
{
	if (!pent)
		return nullptr;

	return &pent->v;
}

inline entvars_t* VARS(EOFFSET eoffset) { return VARS(ENT(eoffset)); }
inline int	  ENTINDEX(edict_t* pEdict) { return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
inline edict_t* INDEXENT(int iEdictNum) { return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }
inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float* pOrigin, entvars_t* ent) {
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ENT(ent));
}

// Testing the three types of "entity" for nullity
enum : std::uint8_t
{
	eoNullEntity = 0
};

inline bool FNullEnt(EOFFSET eoffset) { return eoffset == 0; }
inline bool FNullEnt(const edict_t* pent) { return pent == nullptr || FNullEnt(OFFSET(pent)); }
inline bool FNullEnt(entvars_t* pev) { return pev == nullptr || FNullEnt(OFFSET(pev)); }

// Testing strings for nullity
enum : std::uint8_t
{
	iStringNull = 0
};

inline bool FStringNull(int iString) { return iString == iStringNull; }

enum : std::uint8_t
{
	cchMapNameMost = 32
};

// Dot products for view cone checking
#define VIEW_FIELD_FULL		(-1.0f) // +-180 degrees
#define	VIEW_FIELD_WIDE		(-0.7f) // +-135 degrees 0.1 // +-85 degrees, used for full FOV checks
#define	VIEW_FIELD_NARROW	0.7f // +-45 degrees, more narrow check used to set up ranged attacks
#define	VIEW_FIELD_ULTRA_NARROW	0.9f // +-25 degrees, more narrow check used to set up ranged attacks

// All monsters need this data
enum : std::int8_t
{
	DONT_BLEED = (-1)
};

#define		BLOOD_COLOR_RED		(BYTE)247
#define		BLOOD_COLOR_YELLOW	(BYTE)195
#define		BLOOD_COLOR_GREEN	BLOOD_COLOR_YELLOW

typedef enum : std::uint8_t
{
	MONSTERSTATE_NONE = 0,
	MONSTERSTATE_IDLE,
	MONSTERSTATE_COMBAT,
	MONSTERSTATE_ALERT,
	MONSTERSTATE_HUNT,
	MONSTERSTATE_PRONE,
	MONSTERSTATE_SCRIPT,
	MONSTERSTATE_PLAYDEAD,
	MONSTERSTATE_DEAD
} MONSTERSTATE;

// Things that toggle (buttons/triggers/doors) need this
typedef enum : std::uint8_t
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
} TOGGLE_STATE;

// Misc useful
inline bool FStrEq(const char* sz1, const char* sz2)
{
	return (std::strcmp(sz1, sz2) == 0);
}
inline bool FClassnameIs(edict_t* pent, const char* szClassname)
{
	return FStrEq(STRING(VARS(pent)->classname), szClassname);
}
inline bool FClassnameIs(entvars_t* pev, const char* szClassname)
{
	return FStrEq(STRING(pev->classname), szClassname);
}

class CBaseEntity;

// Misc. Prototypes
extern void			UTIL_SetSize(entvars_t* pev, const Vector& vecMin, const Vector& vecMax);
extern float		UTIL_VecToYaw(const Vector& vec);
extern Vector		UTIL_VecToAngles(const Vector& vec);
extern float		UTIL_AngleMod(float a);
extern float		UTIL_AngleDiff(float destAngle, float srcAngle);

extern CBaseEntity* UTIL_FindEntityInSphere(CBaseEntity* pStartEntity, const Vector& vecCenter, float flRadius);
extern CBaseEntity* UTIL_FindEntityByString(CBaseEntity* pStartEntity, const char* szKeyword, const char* szValue);
extern CBaseEntity* UTIL_FindEntityByClassname(CBaseEntity* pStartEntity, const char* szName);
extern CBaseEntity* UTIL_FindEntityByTargetname(CBaseEntity* pStartEntity, const char* szName);
extern CBaseEntity* UTIL_FindEntityGeneric(const char* szName, Vector& vecSrc, float flRadius);

// returns a CBaseEntity pointer to a player by index.  Only returns if the player is spawned and connected
// otherwise returns NULL
// Index is 1 based
extern CBaseEntity* UTIL_PlayerByIndex(int playerIndex);

#define UTIL_EntitiesInPVS(pent)			(*g_engfuncs.pfnEntitiesInPVS)(pent)
extern void			UTIL_MakeVectors(const Vector& vecAngles);

// Pass in an array of pointers and an array size, it fills the array and returns the number inserted
extern int			UTIL_MonstersInSphere(CBaseEntity** pList, int listMax, const Vector& center, float radius);
extern int			UTIL_EntitiesInBox(CBaseEntity** pList, int listMax, const Vector& mins, const Vector& maxs, int flagMask);

inline void UTIL_MakeVectorsPrivate(const Vector& vecAngles, float* p_vForward, float* p_vRight, float* p_vUp)
{
	g_engfuncs.pfnAngleVectors(vecAngles, p_vForward, p_vRight, p_vUp);
}

extern void			UTIL_MakeAimVectors(const Vector& vecAngles); // like MakeVectors, but assumes pitch isn't inverted
extern void			UTIL_MakeInvVectors(const Vector& vec, globalvars_t* pgv);

extern void			UTIL_SetOrigin(entvars_t* pev, const Vector& vecOrigin);
extern void			UTIL_EmitAmbientSound(edict_t* entity, const Vector& vecOrigin, const char* samp, float vol, float attenuation, int fFlags, int pitch);
extern void			UTIL_ParticleEffect(const Vector& vecOrigin, const Vector& vecDirection, ULONG ulColor, ULONG ulCount);
extern void			UTIL_ScreenShake(const Vector& center, float amplitude, float frequency, float duration, float radius);
extern void			UTIL_ScreenShakeAll(const Vector& center, float amplitude, float frequency, float duration);
extern void			UTIL_ShowMessage(const char* pString, CBaseEntity* pPlayer);
extern void			UTIL_ShowMessageAll(const char* pString);
extern void			UTIL_ScreenFadeAll(const Vector& color, float fadeTime, float holdTime, int alpha, int flags);
extern void			UTIL_ScreenFade(CBaseEntity* pEntity, const Vector& color, float fadeTime, float fadeHold, int alpha, int flags);

typedef enum : std::uint8_t { ignore_monsters = 1, dont_ignore_monsters = 0, missile = 2 } IGNORE_MONSTERS;
typedef enum : std::uint8_t { ignore_glass = 1, dont_ignore_glass = 0 } IGNORE_GLASS;
extern void			UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, edict_t* pentIgnore, TraceResult* ptr);
extern void			UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t* pentIgnore, TraceResult* ptr);
typedef enum : std::uint8_t { point_hull = 0, human_hull = 1, large_hull = 2, head_hull = 3 } hull_enum;
extern void			UTIL_TraceHull(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t* pentIgnore, TraceResult* ptr);
extern TraceResult	UTIL_GetGlobalTrace();
extern void			UTIL_TraceModel(const Vector& vecStart, const Vector& vecEnd, int hullNumber, edict_t* pentModel, TraceResult* ptr);
extern Vector		UTIL_GetAimVector(edict_t* pent, float flSpeed);
extern int			UTIL_PointContents(const Vector& vec);

extern int			UTIL_IsMasterTriggered(string_t sMaster, CBaseEntity* pActivator);
extern void			UTIL_BloodStream(const Vector& origin, const Vector& direction, int color, int amount);
extern void			UTIL_BloodDrips(const Vector& origin, const Vector& direction, int color, int amount);
extern Vector		UTIL_RandomBloodVector();
extern bool			UTIL_ShouldShowBlood(int bloodColor);
extern void			UTIL_BloodDecalTrace(TraceResult* pTrace, int bloodColor);
extern void			UTIL_DecalTrace(TraceResult* pTrace, int decalNumber);
extern void			UTIL_PlayerDecalTrace(TraceResult* pTrace, int playernum, int decalNumber, bool bIsCustom);
extern void			UTIL_GunshotDecalTrace(TraceResult* pTrace, int decalNumber);
extern void			UTIL_Sparks(const Vector& position);
extern void			UTIL_Ricochet(const Vector& position, float scale);
extern void			UTIL_StringToVector(float* pVector, const char* pString);
extern void			UTIL_StringToIntArray(int* pVector, int count, const char* pString);
extern Vector		UTIL_ClampVectorToBox(const Vector& input, const Vector& clampSize);
extern float		UTIL_Approach(float target, float value, float speed);
extern float		UTIL_ApproachAngle(float target, float value, float speed);
extern float		UTIL_AngleDistance(float next, float cur);

extern char* UTIL_VarArgs(char* format, ...);
extern void			UTIL_Remove(CBaseEntity* pEntity);
extern bool			UTIL_IsValidEntity(edict_t* pent);
extern bool			UTIL_TeamsMatch(const char* pTeamName1, const char* pTeamName2);

// Use for ease-in, ease-out style interpolation (accel/decel)
extern float		UTIL_SplineFraction(float value, float scale);

// Search for water transition along a vertical line
extern float		UTIL_WaterLevel(const Vector& position, float minz, float maxz);
extern void			UTIL_Bubbles(Vector mins, Vector maxs, int count);
extern void			UTIL_BubbleTrail(Vector from, Vector to, int count);

// allows precacheing of other entities
extern void			UTIL_PrecacheOther(const char* szClassname);

// prints a message to each client
extern void			UTIL_ClientPrintAll(int msg_dest, const char* msg_name, const char* param1 = nullptr, const char* param2 = nullptr, const char* param3 = nullptr, const char* param4 = nullptr);
inline void			UTIL_CenterPrintAll(const char* msg_name, const char* param1 = nullptr, const char* param2 = nullptr, const char* param3 = nullptr, const char* param4 = nullptr)
{
	UTIL_ClientPrintAll(HUD_PRINTCENTER, msg_name, param1, param2, param3, param4);
}

class CBasePlayerItem;
class CBasePlayer;
extern bool UTIL_GetNextBestWeapon(CBasePlayer* pPlayer, CBasePlayerItem* pCurrentWeapon);

// prints messages through the HUD
extern void ClientPrint(entvars_t* client, int msg_dest, const char* msg_name, const char* param1 = nullptr, const char* param2 = nullptr, const char* param3 = nullptr, const char* param4 = nullptr);

// prints a message to the HUD say (chat)
extern void			UTIL_SayText(const char* pText, CBaseEntity* pEntity);
extern void			UTIL_SayTextAll(const char* pText, CBaseEntity* pEntity);

typedef struct hudtextparms_s
{
	float		x;
	float		y;
	int			effect;
	byte		r1, g1, b1, a1;
	byte		r2, g2, b2, a2;
	float		fadeinTime;
	float		fadeoutTime;
	float		holdTime;
	float		fxTime;
	int			channel;
} hudtextparms_t;

// prints as transparent 'title' to the HUD
extern void			UTIL_HudMessageAll(const hudtextparms_t& textparms, const char* pMessage);
extern void			UTIL_HudMessage(CBaseEntity* pEntity, const hudtextparms_t& textparms, const char* pMessage);

// for handy use with ClientPrint params
extern char* UTIL_dtos1(int d);
extern char* UTIL_dtos2(int d);
extern char* UTIL_dtos3(int d);
extern char* UTIL_dtos4(int d);

// Writes message to console with timestamp and FragLog header.
extern void			UTIL_LogPrintf(const char* fmt, ...);

// Sorta like FInViewCone, but for nonmonsters.
extern float UTIL_DotPoints(const Vector& vecSrc, const Vector& vecCheck, const Vector& vecDir);

extern void UTIL_StripToken(const char* pKey, char* pDest);// for redundant keynames

// Misc functions
extern void SetMovedir(entvars_t* pev);
extern Vector VecBModelOrigin(entvars_t* pevBModel);
extern int BuildChangeList(LEVELLIST* pLevelList, int maxList);

//
// How did I ever live without ASSERT?
//
#ifdef	DEBUG
void DBG_AssertFunction(bool fExpr, const char* szExpr, const char* szFile, int szLine, const char* szMessage);
#define ASSERT(f)		DBG_AssertFunction(f, #f, __FILE__, __LINE__, NULL)
#define ASSERTSZ(f, sz)	DBG_AssertFunction(f, #f, __FILE__, __LINE__, sz)
#else	// !DEBUG
#define ASSERT(f)
#define ASSERTSZ(f, sz)
#endif	// !DEBUG

extern DLL_GLOBAL const Vector g_vecZero;

//
// Constants that were used only by QC (maybe not used at all now)
//
// Un-comment only as needed
//
enum : std::uint8_t
{
	LANGUAGE_ENGLISH = 0,
	LANGUAGE_GERMAN = 1,
	LANGUAGE_FRENCH = 2,
	LANGUAGE_BRITISH = 3
};

extern DLL_GLOBAL int			g_Language;

enum : std::uint8_t
{
	AMBIENT_SOUND_STATIC = 0, // medium radius attenuation
	AMBIENT_SOUND_EVERYWHERE = 1,
	AMBIENT_SOUND_SMALLRADIUS = 2,
	AMBIENT_SOUND_MEDIUMRADIUS = 4,
	AMBIENT_SOUND_LARGERADIUS = 8,
	AMBIENT_SOUND_START_SILENT = 16,
	AMBIENT_SOUND_NOT_LOOPING = 32
};

enum : std::uint8_t
{
	SPEAKER_START_SILENT = 1	// wait for trigger 'on' to start announcements
};

enum : std::uint16_t
{
	SND_SPAWNING = (1<<8), // duplicated in protocol.h we're spawing, used in some cases for ambients
	SND_STOP = (1<<5), // duplicated in protocol.h stop sound
	SND_CHANGE_VOL = (1<<6), // duplicated in protocol.h change sound vol
	SND_CHANGE_PITCH = (1<<7)		// duplicated in protocol.h change sound pitch
};

enum : std::uint8_t
{
	LFO_SQUARE = 1,
	LFO_TRIANGLE = 2,
	LFO_RANDOM = 3
};

// func_rotating
enum : std::uint8_t
{
	SF_BRUSH_ROTATE_Y_AXIS = 0,
	SF_BRUSH_ROTATE_INSTANT = 1,
	SF_BRUSH_ROTATE_BACKWARDS = 2,
	SF_BRUSH_ROTATE_Z_AXIS = 4,
	SF_BRUSH_ROTATE_X_AXIS = 8,
	SF_PENDULUM_AUTO_RETURN = 16,
	SF_PENDULUM_PASSABLE = 32
};

enum : std::uint16_t
{
	SF_BRUSH_ROTATE_SMALLRADIUS = 128,
	SF_BRUSH_ROTATE_MEDIUMRADIUS = 256,
	SF_BRUSH_ROTATE_LARGERADIUS = 512
};

enum : std::uint8_t
{
	PUSH_BLOCK_ONLY_X = 1,
	PUSH_BLOCK_ONLY_Y = 2
};

#define VEC_HULL_MIN		Vector(-16, -16, -36)
#define VEC_HULL_MAX		Vector( 16,  16,  36)
#define VEC_HUMAN_HULL_MIN	Vector( -16, -16, 0 )
#define VEC_HUMAN_HULL_MAX	Vector( 16, 16, 72 )
#define VEC_HUMAN_HULL_DUCK	Vector( 16, 16, 36 )

#define VEC_VIEW			Vector( 0, 0, 28 )

#define VEC_DUCK_HULL_MIN	Vector(-16, -16, -18 )
#define VEC_DUCK_HULL_MAX	Vector( 16,  16,  18)
#define VEC_DUCK_VIEW		Vector( 0, 0, 12 )

enum : std::uint8_t
{
	SVC_TEMPENTITY = 23,
	SVC_INTERMISSION = 30,
	SVC_CDTRACK = 32,
	SVC_WEAPONANIM = 35,
	SVC_ROOMTYPE = 37
};

// triggers
enum : std::uint8_t
{
	SF_TRIGGER_ALLOWMONSTERS = 1, // monsters allowed to fire this trigger
	SF_TRIGGER_NOCLIENTS = 2, // players not allowed to fire this trigger
	SF_TRIGGER_PUSHABLES = 4// only pushables can fire this trigger
};

// func breakable
enum : std::uint16_t
{
	SF_BREAK_TRIGGER_ONLY = 1, // may only be broken by trigger
	SF_BREAK_TOUCH = 2, // can be 'crashed through' by running player (plate glass)
	SF_BREAK_PRESSURE = 4, // can be broken by a player standing on it
	SF_BREAK_CROWBAR = 256// instant break if hit with crowbar
};

// func_pushable (it's also func_breakable, so don't collide with those flags)
enum : std::uint8_t
{
	SF_PUSH_BREAKABLE = 128
};

enum : std::uint8_t
{
	SF_LIGHT_START_OFF = 1
};

enum : std::uint8_t
{
	SPAWNFLAG_NOMESSAGE = 1,
	SPAWNFLAG_NOTOUCH = 1,
	SPAWNFLAG_DROIDONLY = 4
};

enum : std::uint8_t
{
	SPAWNFLAG_USEONLY = 1 // can't be touched, must be used (buttons)
};

enum : std::uint8_t
{
	TELE_PLAYER_ONLY = 1,
	TELE_SILENT = 2
};

enum : std::uint8_t
{
	SF_TRIG_PUSH_ONCE = 1
};

// Sound Utilities

// sentence groups
enum : std::uint16_t
{
	CBSENTENCENAME_MAX = 16,
	CVOXFILESENTENCEMAX = 1536		// max number of sentences in game. NOTE: this must match
};

// CVOXFILESENTENCEMAX in engine\sound.h!!!

extern char gszallsentencenames[CVOXFILESENTENCEMAX][CBSENTENCENAME_MAX];
extern int gcallsentences;

int USENTENCEG_Pick(int isentenceg, char* szfound);
int USENTENCEG_PickSequential(int isentenceg, char* szfound, int ipick, int freset);
void USENTENCEG_InitLRU(unsigned char* plru, int count);

void SENTENCEG_Init();
void SENTENCEG_Stop(edict_t* entity, int isentenceg, int ipick);
int SENTENCEG_PlayRndI(edict_t* entity, int isentenceg, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlayRndSz(edict_t* entity, const char* szrootname, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlaySequentialSz(edict_t* entity, const char* szrootname, float volume, float attenuation, int flags, int pitch, int ipick, int freset);
int SENTENCEG_GetIndex(const char* szrootname);
int SENTENCEG_Lookup(const char* sample, char* sentencenum);

void TEXTURETYPE_Init();
char TEXTURETYPE_Find(char* name);
float TEXTURETYPE_PlaySound(TraceResult* ptr, Vector vecSrc, Vector vecEnd, int iBulletType);

enum : std::uint8_t
{
	CBTEXTURENAMEMAX = 13			// only load first n chars of name
};

enum : std::uint8_t
{
	CHAR_TEX_CONCRETE = 'C', // texture types
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_FLESH = 'F'
};

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch.   150 to 70 is the realistic range.
// EMIT_SOUND_DYN with pitch != 100 should be used sparingly, as it's not quite as
// fast as EMIT_SOUND (the pitchshift mixer is not native coded).

void EMIT_SOUND_DYN(edict_t* entity, int channel, const char* sample, float volume, float attenuation,
	int flags, int pitch);

inline void EMIT_SOUND(edict_t* entity, int channel, const char* sample, float volume, float attenuation)
{
	EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t* entity, int channel, const char* sample)
{
	EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

void EMIT_SOUND_SUIT(edict_t* entity, const char* sample);
void EMIT_GROUPID_SUIT(edict_t* entity, int isentenceg);
void EMIT_GROUPNAME_SUIT(edict_t* entity, const char* groupname);

#define PRECACHE_SOUND_ARRAY( a ) \
	{ for (int i = 0; i < ARRAYSIZE( a ); i++ ) PRECACHE_SOUND((char *) (a) [i]); }

#define EMIT_SOUND_ARRAY_DYN( chan, array ) \
	EMIT_SOUND_DYN ( ENT(pev), chan , (array) [ RANDOM_LONG(0,ARRAYSIZE( array )-1) ], 1.0, ATTN_NORM, 0, RANDOM_LONG(95,105) );

#define RANDOM_SOUND_ARRAY( array ) (array) [ RANDOM_LONG(0,ARRAYSIZE( (array) )-1) ]

#define PLAYBACK_EVENT( flags, who, index ) PLAYBACK_EVENT_FULL( flags, who, index, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#define PLAYBACK_EVENT_DELAY( flags, who, index, delay ) PLAYBACK_EVENT_FULL( flags, who, index, delay, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );

enum : std::uint8_t
{
	GROUP_OP_AND = 0,
	GROUP_OP_NAND = 1
};

extern int g_groupmask;
extern int g_groupop;

class UTIL_GroupTrace
{
public:
	UTIL_GroupTrace(int groupmask, int op);
	~UTIL_GroupTrace();

private:
	int m_oldgroupmask, m_oldgroupop;
};

void UTIL_SetGroupTrace(int groupmask, int op);
void UTIL_UnsetGroupTrace();

int UTIL_SharedRandomLong(unsigned int seed, int low, int high);
float UTIL_SharedRandomFloat(unsigned int seed, float low, float high);

float UTIL_WeaponTimeBase();
#endif // __UTIL_H__
