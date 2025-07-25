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
/*

Class Hierachy

CBaseEntity
	CBaseDelay
		CBaseToggle
			CBaseItem
			CBaseMonster
				CBaseCycler
				CBasePlayer
				CBaseGroup
*/
#ifndef CBASE_H
#define CBASE_H

constexpr int MAX_PATH_SIZE = 10; // max number of nodes available for a path.

// These are caps bits to indicate what an object's capabilities (currently used for save/restore and level transitions)
enum : uint32_t {
	FCAP_CUSTOMSAVE = 0x00000001,
	FCAP_ACROSS_TRANSITION = 0x00000002, // should transfer between transitions
	FCAP_MUST_SPAWN = 0x00000004,		// Spawn after restore
	FCAP_DONT_SAVE = 0x80000000,		// Don't save this
	FCAP_IMPULSE_USE = 0x00000008,		// can be used by the player
	FCAP_CONTINUOUS_USE = 0x00000010,	// can be used by the player
	FCAP_ONOFF_USE = 0x00000020,		// can be used by the player
	FCAP_DIRECTIONAL_USE = 0x00000040,	// Player sends +/- 1 when using (currently only tracktrains)
	FCAP_MASTER = 0x00000080,			// Can be used to "master" other entities (like multisource)
	FCAP_FORCE_TRANSITION = 0x00000080  // ALWAYS goes across transitions
};

#include "saverestore.h"
#include "schedule.h"

#ifndef MONSTEREVENT_H
#include "monsterevent.h"
#endif

// C functions for external declarations that call the appropriate C++ methods

#ifndef __linux__
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define EXPORT	_declspec( dllexport )
#else
#define EXPORT	__declspec( dllexport )
#endif
#else
#define EXPORT
#endif

#ifndef RCBOT_META_BUILD

extern "C" EXPORT int GetEntityAPI(DLL_FUNCTIONS* pFunctionTable, int interfaceVersion);
extern "C" EXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion);
extern "C" EXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion);

#endif

extern "C" EXPORT int DispatchSpawn(edict_t* pent);
extern "C" EXPORT void DispatchKeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd);
extern "C" EXPORT void DispatchTouch(edict_t* pentTouched, edict_t* pentOther);
extern "C" EXPORT void DispatchUse(edict_t* pentUsed, edict_t* pentOther);
extern "C" EXPORT void DispatchThink(edict_t* pent);
extern "C" EXPORT void DispatchBlocked(edict_t* pentBlocked, edict_t* pentOther);
extern "C" EXPORT void DispatchSave(edict_t* pent, SAVERESTOREDATA* pSaveData);
extern "C" EXPORT int  DispatchRestore(edict_t* pent, SAVERESTOREDATA* pSaveData, int globalEntity);
extern "C" EXPORT void	DispatchObjectCollsionBox(edict_t* pent);
extern "C" EXPORT void SaveWriteFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount);
extern "C" EXPORT void SaveReadFields(SAVERESTOREDATA* pSaveData, const char* pname, void* pBaseData, TYPEDESCRIPTION* pFields, int fieldCount);
extern "C" EXPORT void SaveGlobalState(SAVERESTOREDATA* pSaveData);
extern "C" EXPORT void RestoreGlobalState(SAVERESTOREDATA* pSaveData);
extern "C" EXPORT void ResetGlobalState();

typedef enum : std::uint8_t { USE_OFF = 0, USE_ON = 1, USE_SET = 2, USE_TOGGLE = 3 } USE_TYPE;

extern "C" EXPORT void FireTargets(const char* targetName, CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

typedef void (CBaseEntity::* BASEPTR)();
typedef void (CBaseEntity::* ENTITYFUNCPTR)(CBaseEntity* pOther);
typedef void (CBaseEntity::* USEPTR)(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

// For CLASSIFY
enum : std::uint8_t
{
	CLASS_NONE = 0,
	CLASS_MACHINE = 1,
	CLASS_PLAYER = 2,
	CLASS_HUMAN_PASSIVE = 3,
	CLASS_HUMAN_MILITARY = 4,
	CLASS_ALIEN_MILITARY = 5,
	CLASS_ALIEN_PASSIVE = 6,
	CLASS_ALIEN_MONSTER = 7,
	CLASS_ALIEN_PREY = 8,
	CLASS_ALIEN_PREDATOR = 9,
	CLASS_INSECT = 10,
	CLASS_PLAYER_ALLY = 11,
	CLASS_PLAYER_BIOWEAPON = 12, // hornets and snarks.launched by players
	CLASS_ALIEN_BIOWEAPON = 13, // hornets and snarks.launched by the alien menace
	CLASS_BARNACLE = 99 // special because no one pays attention to it, and it eats a wide cross-section of creatures.
};

class CBaseEntity;
class CBaseMonster;
class CBasePlayerItem;
class CSquadMonster;

constexpr int SF_NORESPAWN = (1 << 30);// !!!set this bit on guns and stuff that should never respawn.; 

//
// EHANDLE. Safe way to point to CBaseEntities who may die between frames
//
class EHANDLE
{
private:
	edict_t* m_pent = nullptr;
	int		m_serialnumber = 0;
public:
	edict_t* Get();
	edict_t* Set(edict_t* pent);

	operator int();

	operator CBaseEntity* ();

	CBaseEntity* operator = (CBaseEntity* pEntity);
	CBaseEntity* operator ->();
};

//
// Base Entity.  All entity types derive from this
//
class CBaseEntity
{
public:
	virtual ~CBaseEntity() = default;
	// Constructor.  Set engine to use C/C++ callback functions
	// pointers to engine data
	entvars_t* pev;		// Don't need to save/restore this pointer, the engine resets it

	// path corners
	CBaseEntity* m_pGoalEnt;// path corner we are heading towards
	CBaseEntity* m_pLink;// used for temporary link-list operations.

	// initialization functions
	virtual void	Spawn() {}
	virtual void	Precache() {}
	virtual void	KeyValue(KeyValueData* pkvd) { pkvd->fHandled = false; }
	virtual int		Save(CSave& save);
	virtual int		Restore(CRestore& restore);
	virtual int		ObjectCaps() { return FCAP_ACROSS_TRANSITION; }
	virtual void	Activate() {}

	// Setup the object->object collision box (pev->mins / pev->maxs is the object->world collision box)
	virtual void	SetObjectCollisionBox();

	// Classify - returns the type of group (i.e, "houndeye", or "human military" so that monsters with different classnames
	// still realize that they are teammates. (overridden for monsters that form groups)
	virtual int Classify() { return CLASS_NONE; }
	virtual void DeathNotice(entvars_t* pevChild) {}// monster maker children use this to tell the monster maker that they have died.

	static	TYPEDESCRIPTION m_SaveData[];

	virtual void	TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	virtual int		TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual int		TakeHealth(float flHealth, int bitsDamageType);
	virtual void	Killed(entvars_t* pevAttacker, int iGib);
	virtual int		BloodColor() { return DONT_BLEED; }
	virtual void	TraceBleed(float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	virtual bool    IsTriggered(CBaseEntity* pActivator) { return true; }
	virtual CBaseMonster* MyMonsterPointer() { return nullptr; }
	virtual CSquadMonster* MySquadMonsterPointer() { return nullptr; }
	virtual	int		GetToggleState() { return TS_AT_TOP; }
	virtual void	AddPoints(int score, bool bAllowNegativeScore) {}
	virtual void	AddPointsToTeam(int score, bool bAllowNegativeScore) {}
	virtual bool	AddPlayerItem(CBasePlayerItem* pItem) { return false; }
	virtual bool	RemovePlayerItem(CBasePlayerItem* pItem) { return false; }
	virtual int 	GiveAmmo(int iAmount, char* szName, int iMax) { return -1; }
	virtual float	GetDelay() { return 0; }
	virtual int		IsMoving() { return pev->velocity != g_vecZero; }
	virtual void	OverrideReset() {}
	virtual int		DamageDecal(int bitsDamageType);
	// This is ONLY used by the node graph to test movement through a door
	virtual void	SetToggleState(int state) {}
	virtual void    StartSneaking() {}
	virtual void    StopSneaking() {}
	virtual bool	OnControls(entvars_t* pev) { return false; }
	virtual bool    IsSneaking() { return false; }
	virtual bool	IsAlive() { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	virtual bool	IsBSPModel() { return pev->solid == SOLID_BSP || pev->movetype == MOVETYPE_PUSHSTEP; }
	virtual bool	ReflectGauss() { return (IsBSPModel() && !pev->takedamage); }
	virtual bool	HasTarget(string_t targetname) { return FStrEq(STRING(targetname), STRING(pev->targetname)); }
	virtual bool    IsInWorld();
	virtual	bool	IsPlayer() { return false; }
	virtual bool	IsNetClient() { return false; }
	virtual const char* TeamID() { return ""; }

	//	virtual void	SetActivator( CBaseEntity *pActivator ) {}
	virtual CBaseEntity* GetNextTarget();

	// fundamental callbacks
	void (CBaseEntity ::* m_pfnThink)();
	void (CBaseEntity ::* m_pfnTouch)(CBaseEntity* pOther);
	void (CBaseEntity ::* m_pfnUse)(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void (CBaseEntity ::* m_pfnBlocked)(CBaseEntity* pOther);

	virtual void Think() { if (m_pfnThink) (this->*m_pfnThink)(); }
	virtual void Touch(CBaseEntity* pOther) { if (m_pfnTouch) (this->*m_pfnTouch)(pOther); }

	virtual void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, const float value)
	{
		if (m_pfnUse)
			(this->*m_pfnUse)(pActivator, pCaller, useType, value);
	}
	virtual void Blocked(CBaseEntity* pOther) { if (m_pfnBlocked) (this->*m_pfnBlocked)(pOther); }

	// allow engine to allocate instance data
	void* operator new(size_t stAllocateBlock, entvars_t* pev)
	{
		return (void*)ALLOC_PRIVATE(ENT(pev), stAllocateBlock);
	}

	// don't use this.
#if _MSC_VER >= 1200 // only build this code if MSVC++ 6.0 or higher
	void operator delete(void* pMem, entvars_t* pev)
	{
		pev->flags |= FL_KILLME;
	}
#endif

	void UpdateOnRemove();

	// common member functions
	void EXPORT SUB_Remove();
	void EXPORT SUB_DoNothing();
	void EXPORT SUB_StartFadeOut();
	void EXPORT SUB_FadeOut();
	void EXPORT SUB_CallUseToggle() { this->Use(this, this, USE_TOGGLE, 0); }
	int			ShouldToggle(USE_TYPE useType, bool currentState);
	void		FireBullets(ULONG	cShots, Vector  vecSrc, Vector	vecDirShooting, Vector	vecSpread, float flDistance, int iBulletType, int iTracerFreq = 4, int iDamage = 0, entvars_t* pevAttacker = nullptr);

	virtual CBaseEntity* Respawn() { return nullptr; }

	virtual void SUB_UseTargets(CBaseEntity* pActivator, USE_TYPE useType, float value);
	// Do the bounding boxes of these two intersect?
	int		Intersects(CBaseEntity* pOther);
	void	MakeDormant();
	int		IsDormant();
	static bool    IsLockedByMaster() { return false; }

#ifdef _DEBUG
	static CBaseEntity* Instance(edict_t* pent)
	{
		if (!pent)
			pent = ENT(0);
		CBaseEntity* pEnt = static_cast<CBaseEntity*>(GET_PRIVATE(pent));
		ASSERT(pEnt != NULL);
		return pEnt;
	}
#else
	static CBaseEntity* Instance(edict_t* pent)
	{
		if (!pent)
			pent = ENT(0);
		CBaseEntity* pEnt = static_cast<CBaseEntity*>(GET_PRIVATE(pent));
		return pEnt;
	}
#endif

	static CBaseEntity* Instance(entvars_t* pev) { return Instance(ENT(pev)); }
	static CBaseEntity* Instance(const int eoffset) { return Instance(ENT(eoffset)); }

	static CBaseMonster* GetMonsterPointer(entvars_t* pevMonster)
	{
		if (CBaseEntity* pEntity = Instance(pevMonster))
			return pEntity->MyMonsterPointer();
		return nullptr;
	}

	static CBaseMonster* GetMonsterPointer(edict_t* pentMonster)
	{
		if (CBaseEntity* pEntity = Instance(pentMonster))
			return pEntity->MyMonsterPointer();
		return nullptr;
	}

	// Ugly code to lookup all functions to make sure they are exported when set.
#ifdef _DEBUG
	void FunctionCheck(void* pFunction, char* name)
	{
#ifndef __linux__
		if (pFunction && !NAME_FOR_FUNCTION((unsigned long)(pFunction)))
			ALERT(at_error, "No EXPORT: %s:%s (%08lx)\n", STRING(pev->classname), name, (unsigned long)pFunction);
#endif // __linux__
	}

	BASEPTR	ThinkSet(BASEPTR func, char* name)
	{
		m_pfnThink = func;
		FunctionCheck((void*)*((int*)((char*)this + (offsetof(CBaseEntity, m_pfnThink)))), name);
		return func;
	}
	ENTITYFUNCPTR TouchSet(ENTITYFUNCPTR func, char* name)
	{
		m_pfnTouch = func;
		FunctionCheck((void*)*((int*)((char*)this + (offsetof(CBaseEntity, m_pfnTouch)))), name);
		return func;
	}
	USEPTR	UseSet(USEPTR func, char* name)
	{
		m_pfnUse = func;
		FunctionCheck((void*)*((int*)((char*)this + (offsetof(CBaseEntity, m_pfnUse)))), name);
		return func;
	}
	ENTITYFUNCPTR	BlockedSet(ENTITYFUNCPTR func, char* name)
	{
		m_pfnBlocked = func;
		FunctionCheck((void*)*((int*)((char*)this + (offsetof(CBaseEntity, m_pfnBlocked)))), name);
		return func;
	}

#endif

	// virtual functions used by a few classes

	// used by monsters that are created by the MonsterMaker
	virtual	void UpdateOwner() {}

	//
	static CBaseEntity* Create(char* szName, const Vector& vecOrigin, const Vector& vecAngles, edict_t* pentOwner = nullptr);

	virtual bool FBecomeProne() { return false; }
	edict_t* edict() const { return ENT(pev); }
	EOFFSET eoffset() const { return OFFSET(pev); }
	int	  entindex() const { return ENTINDEX(edict()); }

	virtual Vector Center() { return (pev->absmax + pev->absmin) * 0.5; } // center point of entity
	virtual Vector EyePosition() { return pev->origin + pev->view_ofs; } // position of eyes
	virtual Vector EarPosition() { return pev->origin + pev->view_ofs; } // position of ears
	virtual Vector BodyTarget(const Vector& posSrc) { return Center(); } // position to shoot at

	virtual int Illumination() { return GETENTITYILLUM(ENT(pev)); }

	virtual	bool FVisible(CBaseEntity* pEntity);
	virtual	bool FVisible(const Vector& vecOrigin);

	//We use this variables to store each ammo count.
	int ammo_9mm;
	int ammo_357;
	int ammo_bolts;
	int ammo_buckshot;
	int ammo_rockets;
	int ammo_uranium;
	int ammo_hornets;
	int ammo_argrens;
	//Special stuff for grenades and satchels.
	float m_flStartThrow;
	float m_flReleaseThrow;
	int m_chargeReady;
	int m_fInAttack;

	enum EGON_FIRESTATE : std::uint8_t { FIRE_OFF, FIRE_CHARGE };
	int m_fireState;
};

// Ugly technique to override base member functions
// Normally it's illegal to cast a pointer to a member function of a derived class to a pointer to a
// member function of a base class.  static_cast is a sleezy way around that problem.

#ifdef _DEBUG

#define SetThink( a ) ThinkSet( static_cast <void (CBaseEntity::*)(void)> (a), #a )
#define SetTouch( a ) TouchSet( static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a), #a )
#define SetUse( a ) UseSet( static_cast <void (CBaseEntity::*)(	CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )> (a), #a )
#define SetBlocked( a ) BlockedSet( static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a), #a )

#else

#define SetThink( a ) m_pfnThink = static_cast <void (CBaseEntity::*)(void)> (a)
#define SetTouch( a ) m_pfnTouch = static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a)
#define SetUse( a ) m_pfnUse = static_cast <void (CBaseEntity::*)( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )> (a)
#define SetBlocked( a ) m_pfnBlocked = static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a)

#endif

class CPointEntity : public CBaseEntity
{
public:
	void	Spawn() override;
	int	ObjectCaps() override { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	//private:
};

typedef struct locksounds			// sounds that doors and buttons make when locked/unlocked
{
	string_t	sLockedSound;		// sound a door makes when it's locked
	string_t	sLockedSentence;	// sentence group played when door is locked
	string_t	sUnlockedSound;		// sound a door makes when it's unlocked
	string_t	sUnlockedSentence;	// sentence group played when door is unlocked

	int		iLockedSentence;		// which sentence in sentence group to play next
	int		iUnlockedSentence;		// which sentence in sentence group to play next

	float	flwaitSound;			// time delay between playing consecutive 'locked/unlocked' sounds
	float	flwaitSentence;			// time delay between playing consecutive sentences
	BYTE	bEOFLocked;				// true if hit end of list of locked sentences
	BYTE	bEOFUnlocked;			// true if hit end of list of unlocked sentences
} locksound_t;

void PlayLockSounds(entvars_t* pev, locksound_t* pls, int flocked, int fbutton);

//
// MultiSouce
//

constexpr int MAX_MULTI_TARGETS = 16; // maximum number of targets a single multi_manager entity may be assigned.;
constexpr int MS_MAX_TARGETS = 32;

class CMultiSource : public CPointEntity
{
public:
	void Spawn() override;
	void KeyValue(KeyValueData* pkvd) override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	int	ObjectCaps() override { return (CPointEntity::ObjectCaps() | FCAP_MASTER); }
	bool IsTriggered(CBaseEntity* pActivator) override;
	void EXPORT Register();
	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];

	EHANDLE		m_rgEntities[MS_MAX_TARGETS];
	int			m_rgTriggered[MS_MAX_TARGETS];

	int			m_iTotal;
	string_t	m_globalstate;
};

//
// generic Delay entity.
//
class CBaseDelay : public CBaseEntity
{
public:
	float		m_flDelay;
	int			m_iszKillTarget;

	void	KeyValue(KeyValueData* pkvd) override;
	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];
	// common member functions
	void SUB_UseTargets(CBaseEntity* pActivator, USE_TYPE useType, float value) override;
	void EXPORT DelayThink();
};

class CBaseAnimating : public CBaseDelay
{
public:
	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];

	// Basic Monster Animation functions
	float StudioFrameAdvance(float flInterval = 0.0f); // accumulate animation frame time from last time called until now
	int	 GetSequenceFlags();
	int  LookupActivity(int activity);
	int  LookupActivityHeaviest(int activity);
	int  LookupSequence(const char* label);
	void ResetSequenceInfo();
	void DispatchAnimEvents(float flFutureInterval = 0.1f); // Handle events that have happend since last time called up until X seconds into the future
	virtual void HandleAnimEvent(MonsterEvent_t* pEvent) {}
	float SetBoneController(int iController, float flValue);
	void InitBoneControllers();
	float SetBlending(int iBlender, float flValue);
	void GetBonePosition(int iBone, Vector& origin, Vector& angles);
	void GetAutomovement(Vector& origin, Vector& angles, float flInterval = 0.1f);
	int  FindTransition(int iEndingSequence, int iGoalSequence, int* piDir);
	void GetAttachment(int iAttachment, Vector& origin, Vector& angles);
	void SetBodygroup(int iGroup, int iValue);
	int GetBodygroup(int iGroup);
	int ExtractBbox(int sequence, float* mins, float* maxs);
	void SetSequenceBox();

	// animation needs
	float				m_flFrameRate;		// computed FPS for current sequence
	float				m_flGroundSpeed;	// computed linear movement rate for current sequence
	float				m_flLastEventCheck;	// last time the event list was checked
	bool				m_fSequenceFinished;// flag set when StudioAdvanceFrame moves across a frame boundry
	bool				m_fSequenceLoops;	// true if the sequence loops
};

//
// generic Toggle entity.
//
constexpr int SF_ITEM_USE_ONLY = 256;//  ITEM_USE_ONLY = BUTTON_USE_ONLY = DOOR_USE_ONLY!!!; 

class CBaseToggle : public CBaseAnimating
{
public:
	void				KeyValue(KeyValueData* pkvd) override;

	TOGGLE_STATE		m_toggle_state;
	float				m_flActivateFinished;//like attack_finished, but for doors
	float				m_flMoveDistance;// how far a door should slide or rotate
	float				m_flWait;
	float				m_flLip;
	float				m_flTWidth;// for plats
	float				m_flTLength;// for plats

	Vector				m_vecPosition1;
	Vector				m_vecPosition2;
	Vector				m_vecAngle1;
	Vector				m_vecAngle2;

	int					m_cTriggersLeft;		// trigger_counter only, # of activations remaining
	float				m_flHeight;
	EHANDLE				m_hActivator;
	void (CBaseToggle::* m_pfnCallWhenMoveDone)();
	Vector				m_vecFinalDest;
	Vector				m_vecFinalAngle;

	int					m_bitsDamageInflict;	// DMG_ damage type that the door or tigger does

	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	static	TYPEDESCRIPTION m_SaveData[];

	int		GetToggleState() override { return m_toggle_state; }
	float	GetDelay() override { return m_flWait; }

	// common member functions
	void LinearMove(Vector	vecDest, float flSpeed);
	void EXPORT LinearMoveDone();
	void AngularMove(Vector vecDestAngle, float flSpeed);
	void EXPORT AngularMoveDone();
	bool IsLockedByMaster();

	static float		AxisValue(int flags, const Vector& angles);
	static void			AxisDir(entvars_t* pev);
	static float		AxisDelta(int flags, const Vector& angle1, const Vector& angle2);

	string_t m_sMaster;		// If this button has a master switch, this is the targetname.
	// A master switch must be of the multisource type. If all
	// of the switches in the multisource have been triggered, then
	// the button will be allowed to operate. Otherwise, it will be
	// deactivated.
};
#define SetMoveDone( a ) m_pfnCallWhenMoveDone = static_cast <void (CBaseToggle::*)(void)> (a)

// people gib if their health is <= this at the time of death
enum : std::int8_t
{
	GIB_HEALTH_VALUE = (-30)
};

enum : std::uint8_t
{
	ROUTE_SIZE = 8, // how many waypoints a monster can store at one time
	MAX_OLD_ENEMIES = 4 // how many old enemies to remember
};

enum : std::uint16_t
{
	bits_CAP_DUCK = ( 1 << 0 ), // crouch
	bits_CAP_JUMP = ( 1 << 1 ), // jump/leap
	bits_CAP_STRAFE = ( 1 << 2 ), // strafe ( walk/run sideways)
	bits_CAP_SQUAD = ( 1 << 3 ), // can form squads
	bits_CAP_SWIM = ( 1 << 4 ), // proficiently navigate in water
	bits_CAP_CLIMB = ( 1 << 5 ), // climb ladders/ropes
	bits_CAP_USE = ( 1 << 6 ), // open doors/push buttons/pull levers
	bits_CAP_HEAR = ( 1 << 7 ), // can hear forced sounds
	bits_CAP_AUTO_DOORS = ( 1 << 8 ), // can trigger auto doors
	bits_CAP_OPEN_DOORS = ( 1 << 9 ), // can open manual doors
	bits_CAP_TURN_HEAD = ( 1 << 10)// can turn head, always bone controller 0
};

enum : std::uint16_t
{
	bits_CAP_RANGE_ATTACK1 = (1 << 11), // can do a range attack 1
	bits_CAP_RANGE_ATTACK2 = (1 << 12), // can do a range attack 2
	bits_CAP_MELEE_ATTACK1 = (1 << 13), // can do a melee attack 1
	bits_CAP_MELEE_ATTACK2 = (1 << 14)// can do a melee attack 2
};

enum : std::uint16_t
{
	bits_CAP_FLY = (1 << 15)// can fly, move all around
};

#define bits_CAP_DOORS_GROUP    (bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)

// used by suit voice to indicate damage sustained and repaired type to player

// instant damage

enum : std::uint16_t
{
	DMG_GENERIC = 0, // generic damage was done
	DMG_CRUSH = (1 << 0), // crushed by falling or moving object
	DMG_BULLET = (1 << 1), // shot
	DMG_SLASH = (1 << 2), // cut, clawed, stabbed
	DMG_BURN = (1 << 3), // heat burned
	DMG_FREEZE = (1 << 4), // frozen
	DMG_FALL = (1 << 5), // fell too far
	DMG_BLAST = (1 << 6), // explosive blast damage
	DMG_CLUB = (1 << 7), // crowbar, punch, headbutt
	DMG_SHOCK = (1 << 8), // electric shock
	DMG_SONIC = (1 << 9), // sound pulse shockwave
	DMG_ENERGYBEAM = (1 << 10), // laser or other high energy beam
	DMG_NEVERGIB = (1 << 12), // with this bit OR'd in, no damage type will be able to gib victims upon death
	DMG_ALWAYSGIB = (1 << 13), // with this bit OR'd in, any damage type can be made to gib victims upon death.
	DMG_DROWN = (1 << 14) // Drowning
};
// time-based damage
enum : std::int16_t
{
	DMG_TIMEBASED = (~(0x3fff))	// mask for time-based damage
};

enum
{
	DMG_PARALYZE = (1 << 15), // slows affected creature down
	DMG_NERVEGAS = (1 << 16), // nerve toxins, very bad
	DMG_POISON = (1 << 17), // blood poisioning
	DMG_RADIATION = (1 << 18), // radiation exposure
	DMG_DROWNRECOVER = (1 << 19), // drowning recovery
	DMG_ACID = (1 << 20), // toxic chemicals or acid burns
	DMG_SLOWBURN = (1 << 21), // in an oven
	DMG_SLOWFREEZE = (1 << 22), // in a subzero freezer
	DMG_MORTAR = (1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)
};

// these are the damage types that are allowed to gib corpses
#define DMG_GIB_CORPSE		( DMG_CRUSH | DMG_FALL | DMG_BLAST | DMG_SONIC | DMG_CLUB )

// these are the damage types that have client hud art
#define DMG_SHOWNHUD		(DMG_POISON | DMG_ACID | DMG_FREEZE | DMG_SLOWFREEZE | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK)

// NOTE: tweak these values based on gameplay feedback:

constexpr unsigned PARALYZE_DURATION = 2;       // number of 2 second intervals to take damage
constexpr float PARALYZE_DAMAGE = 1.0f;    // damage to take each 2 second interval

constexpr unsigned NERVEGAS_DURATION = 2;
constexpr float NERVEGAS_DAMAGE = 5.0f;

constexpr unsigned POISON_DURATION = 5;
constexpr float POISON_DAMAGE = 2.0f;

constexpr unsigned RADIATION_DURATION = 2;
constexpr float RADIATION_DAMAGE = 1.0f;

constexpr unsigned ACID_DURATION = 2;
constexpr float ACID_DAMAGE = 5.0f;

constexpr unsigned SLOWBURN_DURATION = 2;
constexpr float SLOWBURN_DAMAGE = 1.0f;

constexpr unsigned SLOWFREEZE_DURATION = 2;
constexpr float SLOWFREEZE_DAMAGE = 1.0f;

enum : std::uint8_t
{
	itbd_Paralyze = 0,
	itbd_NerveGas = 1,
	itbd_Poison = 2,
	itbd_Radiation = 3,
	itbd_DrownRecover = 4,
	itbd_Acid = 5,
	itbd_SlowBurn = 6,
	itbd_SlowFreeze = 7,
	CDMG_TIMEBASED = 8
};

// when calling KILLED(), a value that governs gib behavior is expected to be
// one of these three values
enum : std::uint8_t
{
	GIB_NORMAL = 0,	// gib if entity was overkilled
	GIB_NEVER = 1,	// never gib, no matter how much death damage is done ( freezing, etc )
	GIB_ALWAYS = 2	// always gib ( Houndeye Shock, Barnacle Bite )
};

//class CBaseMonster;
class CCineMonster;
class CSound;

#include "basemonster.h"

char* ButtonSound(int sound);				// get string of button sound number

//
// Generic Button
//
class CBaseButton : public CBaseToggle
{
public:
	void Spawn() override;
	void Precache() override;
	void RotSpawn();
	void KeyValue(KeyValueData* pkvd) override;

	void ButtonActivate();
	void SparkSoundCache();

	void EXPORT ButtonShot();
	void EXPORT ButtonTouch(CBaseEntity* pOther);
	void EXPORT ButtonSpark();
	void EXPORT TriggerAndWait();
	void EXPORT ButtonReturn();
	void EXPORT ButtonBackHome();
	void EXPORT ButtonUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	int		TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	int		Save(CSave& save) override;
	int		Restore(CRestore& restore) override;

	enum BUTTON_CODE : std::uint8_t { BUTTON_NOTHING, BUTTON_ACTIVATE, BUTTON_RETURN };
	BUTTON_CODE	ButtonResponseToTouch();

	static	TYPEDESCRIPTION m_SaveData[];
	// Buttons that don't take damage can be IMPULSE used
	int	ObjectCaps() override { return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | (pev->takedamage ? 0 : FCAP_IMPULSE_USE); }

	bool	m_fStayPushed;	// button stays pushed in until touched again?
	bool	m_fRotating;		// a rotating button?  default is a sliding button.

	string_t m_strChangeTarget;	// if this field is not null, this is an index into the engine string array.
	// when this button is touched, it's target entity's TARGET field will be set
	// to the button's ChangeTarget. This allows you to make a func_train switch paths, etc.

	locksound_t m_ls;			// door lock sounds

	BYTE	m_bLockedSound;		// ordinals from entity selection
	BYTE	m_bLockedSentence;
	BYTE	m_bUnlockedSound;
	BYTE	m_bUnlockedSentence;
	int		m_sounds;
};

//
// Weapons
//

constexpr int BAD_WEAPON = 0x00007FFF;

//
// Converts a entvars_t * to a class pointer
// It will allocate the class and entity if necessary
//
template <class T> T* GetClassPtr(T* a)
{
	entvars_t* pev = (entvars_t*)a;

	// allocate entity if necessary
	if (pev == nullptr)
		pev = VARS(CREATE_ENTITY());

	// get the private data
	a = (T*)GET_PRIVATE(ENT(pev));

	if (a == nullptr)
	{
		// allocate private data
		a = new(pev) T;
		a->pev = pev;
	}
	return a;
}

constexpr int TRACER_FREQ = 4;// Tracers fire every 4 bullets; 

typedef struct _SelAmmo
{
	BYTE	Ammo1Type;
	BYTE	Ammo1;
	BYTE	Ammo2Type;
	BYTE	Ammo2;
} SelAmmo;

// this moved here from world.cpp, to allow classes to be derived from it
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================
class CWorld : public CBaseEntity
{
public:
	void Spawn() override;
	void Precache() override;
	void KeyValue(KeyValueData* pkvd) override;
};

//
// tank stuff
//

enum : std::uint16_t
{
	SF_TANK_ACTIVE = 0x0001,
	SF_TANK_PLAYER = 0x0002,
	SF_TANK_HUMANS = 0x0004,
	SF_TANK_ALIENS = 0x0008,
	SF_TANK_LINEOFSIGHT = 0x0010,
	SF_TANK_CANCONTROL = 0x0020,
	SF_TANK_SOUNDON = 0x8000
};

enum TANKBULLET : std::uint8_t
{
	TANK_BULLET_NONE = 0,
	TANK_BULLET_9MM = 1,
	TANK_BULLET_MP5 = 2,
	TANK_BULLET_12MM = 3,
};

//			Custom damage
//			env_laser (duration is 0.5 rate of fire)
//			rockets
//			explosion?

class CFuncTank : public CBaseEntity
{
public:
	void	Spawn() override;
	void	Precache() override;
	void	KeyValue(KeyValueData* pkvd) override;
	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	void	Think() override;
	void	TrackTarget();

	virtual void Fire(const Vector& barrelEnd, const Vector& forward, entvars_t* pevAttacker);
	virtual Vector UpdateTargetPosition(CBaseEntity* pTarget)
	{
		return pTarget->BodyTarget(pev->origin);
	}

	void	StartRotSound();
	void	StopRotSound();

	// Bmodels don't go across transitions
	int	ObjectCaps() override { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	bool IsActive() const { return (pev->spawnflags & SF_TANK_ACTIVE) ? true : false; }
	void TankActivate() { pev->spawnflags |= SF_TANK_ACTIVE; pev->nextthink = pev->ltime + 0.1f; m_fireLast = 0; }
	void TankDeactivate() { pev->spawnflags &= ~SF_TANK_ACTIVE; m_fireLast = 0; StopRotSound(); }
	bool CanFire() const { return (gpGlobals->time - m_lastSightTime) < m_persist; }
	bool		InRange(float range);

	// Acquire a target.  pPlayer is a player in the PVS
	edict_t* FindTarget(edict_t* pPlayer);

	void		TankTrace(const Vector& vecStart, const Vector& vecForward, const Vector& vecSpread, TraceResult& tr);

	Vector BarrelPosition() const
	{
		Vector forward(0.0f, 0.0f, 0.0f);
		Vector right(0.0f, 0.0f, 0.0f);
		Vector up(0.0f, 0.0f, 0.0f);
		UTIL_MakeVectorsPrivate(pev->angles, forward, right, up);
		return pev->origin + (forward * m_barrelPos.x) + (right * m_barrelPos.y) + (up * m_barrelPos.z);
	}

	void		AdjustAnglesForBarrel(Vector& angles, float distance);

	int	Save(CSave& save) override;
	int	Restore(CRestore& restore) override;
	static	TYPEDESCRIPTION m_SaveData[];

	bool OnControls(entvars_t* pevTest) override;
	bool StartControl(CBasePlayer* pController);
	void StopControl();
	void ControllerPostFrame();

protected:
	CBasePlayer* m_pController;
	float		m_flNextAttack;
	Vector		m_vecControllerUsePos;

	float		m_yawCenter;	// "Center" yaw
	float		m_yawRate;		// Max turn rate to track targets
	float		m_yawRange;		// Range of turning motion (one-sided: 30 is +/- 30 degress from center)
	// Zero is full rotation
	float		m_yawTolerance;	// Tolerance angle

	float		m_pitchCenter;	// "Center" pitch
	float		m_pitchRate;	// Max turn rate on pitch
	float		m_pitchRange;	// Range of pitch motion as above
	float		m_pitchTolerance;	// Tolerance angle

	float		m_fireLast;		// Last time I fired
	float		m_fireRate;		// How many rounds/second
	float		m_lastSightTime;// Last time I saw target
	float		m_persist;		// Persistence of firing (how long do I shoot when I can't see)
	float		m_minRange;		// Minimum range to aim/track
	float		m_maxRange;		// Max range to aim/track

	Vector		m_barrelPos;	// Length of the freakin barrel
	float		m_spriteScale;	// Scale of any sprites we shoot
	int			m_iszSpriteSmoke;
	int			m_iszSpriteFlash;
	TANKBULLET	m_bulletType;	// Bullet type
	int			m_iBulletDamage; // 0 means use Bullet type's default damage

	Vector		m_sightOrigin;	// Last sight of target
	int			m_spread;		// firing spread
	int			m_iszMaster;	// Master entity (game_team_master or multisource)
};
#endif // CBASE_H