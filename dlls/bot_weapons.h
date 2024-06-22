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
 // bot_weapons.h
 //
 //////////////////////////////////////////////////
 //
 //
 //
 /****************************************************
  * RCBOT Weapon Classes                             *
  *                                                  *
  * Code by : Paul Murphy {paul.murphy@ntlworld.com} *
  *                       {cheeseh@rcbot.net}        *
  *                                                  *
  ****************************************************
  *
  * Paul M'(cheeseh@rcbot.net/paul.murphy@ntlworld.com)
  * If using any of the class components please keep
  * this tag.. Please :p
  *
  ****************************************************
 */

#ifndef BOT_WEAPONS_H
#define BOT_WEAPONS_H

#include "generic_class.h"
#include "weaponinfo.h"

 ///////////////////////////////////////
 // WEAPON ID's BELOW...

 ////////////////////////////////////////
 // NATURAL SELECTION
 // FLYRA (Charlie Cleveland : flayra@overmind.org)
enum class NSWeapon
{
	NONE = 0,

	// Alien weapons
	CLAWS,
	SPIT,
	SPORES,
	SPIKE,
	BITE,		// Level 1 bite
	BITE2,		// Level 3 bite
	SWIPE,
	WEBSPINNER,
	METABOLIZE,
	PARASITE,
	BLINK,
	DIVINEWIND,

	// Adding a new weapon?  Don't forget to add it's weight in AvHGamerules::GetWeightForItemAndAmmo(AvHWeaponID inWeapon, int inNumRounds)
	KNIFE,
	PISTOL,
	MG,
	SONIC,
	HMG,
	WELDER,
	MINE,
	GRENADE_GUN,

	// Abilities
	LEAP,
	CHARGE,
	UMBRA,
	PRIMALSCREAM,
	BILEBOMB,
	ACIDROCKET,
	HEALINGSPRAY,
	GRENADE,
	STOMP,
	DEVOUR,

	// Can't go over 32 (client.cpp, GetWeaponData())

	MAX
};
//AvHWeaponID;
////////////////////
// HL DEATHMATCH

enum
{
	// weapon ID values for Valve's Half-Life Deathmatch
	VALVE_WEAPON_CROWBAR = 1,
	VALVE_WEAPON_GLOCK,
	VALVE_WEAPON_PYTHON,
	VALVE_WEAPON_MP5,
	VALVE_WEAPON_CHAINGUN,
	VALVE_WEAPON_CROSSBOW,
	VALVE_WEAPON_SHOTGUN,
	VALVE_WEAPON_RPG,
	VALVE_WEAPON_GAUSS,
	VALVE_WEAPON_EGON,
	VALVE_WEAPON_HORNETGUN,
	VALVE_WEAPON_HANDGRENADE,
	VALVE_WEAPON_TRIPMINE,
	VALVE_WEAPON_SATCHEL,
	VALVE_WEAPON_SNARK
};

//Sven Co-op 5.xx no longer supports Metamod [APG]RoboCop[CL]
/*#define SVEN_WEAPON_UZIAKIMBO	  16
#define SVEN_WEAPON_UZI	          17
#define SVEN_WEAPON_MEDKIT        18
#define SVEN_WEAPON_PIPEWRENCH    19
#define SVEN_WEAPON_MINIGUN       21
#define SVEN_WEAPON_GRAPPLE       22
#define SVEN_WEAPON_SNIPERRIFLE   23*/

constexpr int VALVE_MAX_NORMAL_BATTERY = 100;
constexpr int VALVE_HORNET_MAX_CARRY = 8;

// weapon ID values for Valve's Team Fortress Classic & 1.5
/*#define TF_WEAPON_UNKNOWN1       1
#define TF_WEAPON_UNKNOWN2       2
#define TF_WEAPON_MEDIKIT        3
#define TF_WEAPON_SPANNER        4
#define TF_WEAPON_AXE            5
#define TF_WEAPON_SNIPERRIFLE    6
#define TF_WEAPON_AUTORIFLE      7
#define TF_WEAPON_SHOTGUN        8
#define TF_WEAPON_SUPERSHOTGUN   9
#define TF_WEAPON_NAILGUN       10
#define TF_WEAPON_SUPERNAILGUN  11
#define TF_WEAPON_GL            12
#define TF_WEAPON_FLAMETHROWER  13
#define TF_WEAPON_RPG           14
#define TF_WEAPON_IC            15
#define TF_WEAPON_UNKNOWN16     16
#define TF_WEAPON_AC            17
#define TF_WEAPON_UNKNOWN18     18
#define TF_WEAPON_UNKNOWN19     19
#define TF_WEAPON_TRANQ         20
#define TF_WEAPON_RAILGUN       21
#define TF_WEAPON_PL            22
#define TF_WEAPON_KNIFE         23

// weapon ID values for Counter-Strike
#define CS_WEAPON_P228           1
#define CS_WEAPON_UNKNOWN2       2
#define CS_WEAPON_SCOUT          3
#define CS_WEAPON_HEGRENADE      4
#define CS_WEAPON_XM1014         5
#define CS_WEAPON_C4             6
#define CS_WEAPON_MAC10          7
#define CS_WEAPON_AUG            8
#define CS_WEAPON_SMOKEGRENADE   9
#define CS_WEAPON_ELITE         10
#define CS_WEAPON_FIVESEVEN     11
#define CS_WEAPON_UMP45         12
#define CS_WEAPON_SG550         13
#define CS_WEAPON_UNKNOWN14     14
#define CS_WEAPON_UNKNOWN15     15
#define CS_WEAPON_USP           16
#define CS_WEAPON_GLOCK18       17
#define CS_WEAPON_AWP           18
#define CS_WEAPON_MP5NAVY       19
#define CS_WEAPON_M249          20
#define CS_WEAPON_M3            21
#define CS_WEAPON_M4A1          22
#define CS_WEAPON_TMP           23
#define CS_WEAPON_G3SG1         24
#define CS_WEAPON_FLASHBANG     25
#define CS_WEAPON_DEAGLE        26
#define CS_WEAPON_SG552         27
#define CS_WEAPON_AK47          28
#define CS_WEAPON_KNIFE         29
#define CS_WEAPON_P90           30
*/
// weapon ID values for Gearbox's OpFor Deathmatch
enum class GearboxWeapon
{
	CROWBAR = 1,
	GLOCK = 2,
	PYTHON = 3,
	MP5 = 4,
	CHAINGUN = 5,
	CROSSBOW = 6,
	SHOTGUN = 7,
	RPG = 8,
	GAUSS = 9,
	EGON = 10,
	HORNETGUN = 11,
	HANDGRENADE = 12,
	TRIPMINE = 13,
	SATCHEL = 14,
	SNARK = 15,
	GRAPPLE = 16,
	EAGLE = 17,
	PIPEWRENCH = 18,
	M249 = 19,
	DISPLACER = 20,
	UNKNOWN21 = 21,
	SHOCKRIFLE = 22,
	SPORELAUNCHER = 23,
	SNIPERRIFLE = 24,
	KNIFE = 25
};

// weapon ID values for FrontLineForce
enum
{
	FLF_WEAPON_AK5 = 10,
	FLF_WEAPON_UNKNOWN11 = 11,
	FLF_WEAPON_UNKNOWN12 = 12,
	FLF_WEAPON_UNKNOWN13 = 13,
	FLF_WEAPON_UNKNOWN14 = 14,
	FLF_WEAPON_UNKNOWN15 = 15,
	FLF_WEAPON_MP5SD = 16,
	FLF_WEAPON_M4 = 17,
	FLF_WEAPON_FLASHBANG = 18,
	FLF_WEAPON_HEGRENADE = 19,
	FLF_WEAPON_MP5A2 = 20,
	FLF_WEAPON_UMP45 = 21,
	FLF_WEAPON_SPAS12 = 22,
	FLF_WEAPON_BERETTA = 23,
	FLF_WEAPON_KNIFE = 24,
	FLF_WEAPON_MAC10 = 25,
	FLF_WEAPON_MK23 = 26,
	FLF_WEAPON_MSG90 = 27,
	FLF_WEAPON_FAMAS = 28,
	FLF_WEAPON_HK21 = 29
};

enum class DMCWeapon
{
	AXE = 1,
	SHOTGUN = 2,
	DOUBLESHOTGUN = 3,
	NAILGUN = 4,
	SUPERNAIL = 5,
	GRENADE1 = 6,
	ROCKET1 = 7,
	LIGHTNING = 8,

	EXTRA_WEAPON = 8,

	SHELLS = 9,
	NAILS = 10,
	ROCKETS = 11,
	CELLS = 12,

	ARMOR1 = 13,
	ARMOR2 = 14,
	ARMOR3 = 15,

	SUPERHEALTH = 16,

	KEY1 = 17,
	KEY2 = 18,

	INVISIBILITY = 19,
	INVULNERABILITY = 20,
	SUIT = 21,
	QUAD = 22
};

//#define TS_WEAPON_TSGUN			1

enum class TSWeapon
{
	Glock18 = 1,
	MiniUzi = 3,
	BenelliM3 = 4,
	M4A1 = 5,
	MP5SD = 6,
	MP5K = 7,
	AkimboBerettas = 8,
	SocomMk23 = 9,
	Usas12 = 11,
	DesertEagle = 12,
	Ak47 = 13,
	FiveSeven = 14,
	SteyrAug = 15,
	SteyrTmp = 17,
	BarrettM82 = 18,
	HKPdw = 19,
	Spas12 = 20,
	Akimbocolts = 21,
	Glock20 = 22,
	Mac10 = 23,
	CombatKnife = 25,
	Mossberg500 = 26,
	M16A4 = 27,
	RugerMk1 = 28,
	M61Grenade = 24,
	C4 = 29,
	RagingBull = 31,
	M60 = 32,
	Sawedoff = 33,
	Katana = 34,
	SealKnife = 35,
	KungFu = 36
};

// weapon ID values for Science and Industry
enum class SIWeapon
{
	BRIEFCASE = 1,
	COLT = 2,
	VOMIT = 3,
	ROCKETPISTOL = 4,
	SHOTGUN = 5,
	TOMMYGUN = 6,
	SNUZI = 7,
	CROSSBOW = 8,
	GAUSS = 9,
	// WHERE IS WEAPON 10?
	MINDRAY = 11,
	HANDGRENADE = 12,
	TRIPMINE = 13,
	SATCHEL = 14,
	TRANSISTOR = 15,
	FLESHGRENADE = 16,
	RESOURCE = 17,
	CLOAK = 18,
	EMPCANNON = 19
};

// WIZARD WARS //TODO
enum class WizWeapon
{
	STAFF = 1,
	SHIELD,
	SEAL,
	MISSILE,
	DOUBLEMISSILE,
	UPDRAFT,
	LEVITATE,
	LIGHTNINGBOLT,
	SPOTBOLT,
	FLAMELICK,
	FIREBALL,
	DEATHRAY,
	SKULL,
	WHITERAY,
	FORCEBLAST,
	PEBBLEBLAST,
	ROLLINGSTONE,
	BIRD,
	BEARBITE,
	DRAGONBREATH,
	WYVERN,
	ICEPOKE,
	FREEZERAY,
	THORNBLAST,
	BEANSTALK,
	COMBOSPELL,
};

void GetNoWeaponArray(int* Array);
void GetArrayOfExplosives(int* Array);

class CBot;
/////////////////////////////////
// WEAPON CLASSES

// weapon classes below can hold information
// such as ammo left, the weapon's classname
// for selecting that weapon, etc.

// Weapon preset holds info for the bots
// e.g. if they can be shot in water
// fired by primary/secondary and their ranges.
typedef struct
{
	int m_iId;
	BOOL m_bCanFireUnderWater;

	BOOL m_bHasPrimaryFire;
	BOOL m_bHasSecondaryFire;

	float m_fPrimMinRange;
	float m_fPrimMaxRange;

	float m_fSecMinRange;
	float m_fSecMaxRange;

	//TODO: Maybe add ammo count and to hold fire button? [APG]RoboCop[CL]
	BOOL m_bIsMelee;

	short int m_iModId;
	int m_iPriority;
}weapon_preset_t;

class CWeaponPresets
{
public:
	void ReadPresets();

	weapon_preset_t* GetPreset(short int iModId, int iWeaponId) const
	{
		dataStack<weapon_preset_t> tempStack = m_Presets;

		while (!tempStack.IsEmpty())
		{
			weapon_preset_t* pPreset = tempStack.ChoosePointerFromStack();

			if (pPreset->m_iId == iWeaponId &&
				pPreset->m_iModId == iModId)
			{
				tempStack.Init();

				return pPreset;
			}
		}

		return nullptr;
	}

	void Destroy()
	{
		m_Presets.Destroy();
	}

private:
	dataStack<weapon_preset_t> m_Presets;
};

class CWeapon
{
public:
	virtual ~CWeapon() = default;

	CWeapon()
	{
		this->CWeapon::Init();
	}

	virtual void Init()
	{
		m_iAmmoIndex1 = -1;
		m_iAmmoIndex2 = -1;

		m_szClassname = nullptr;

		m_bRegistered = false; // Registered by the mod.

		m_iPrimAmmoMax = -1;
		m_iSecAmmoMax = -1;

		m_iHudSlot = -1;
		m_iHudPosition = -1;

		m_iFlags = 0;

		m_iId = 0;

		//std::memset(this,0,sizeof(CWeapon));
	}

	void SetWeapon(int iId, const char* szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2);

	virtual BOOL CanBeUsedUnderWater()
	{
		// no preset
		return true;
	}

	int MaxPrimaryAmmo() const
	{
		return m_iPrimAmmoMax;
	}

	virtual BOOL CanUsePrimary()
	{
		return true;
	}

	virtual BOOL CanUseSecondary()
	{
		// no preset
		return true;
	}

	BOOL IsRegistered() const
	{
		return m_bRegistered;
	}

	virtual int GetPriority()
	{
		return 0;
	}

	char* GetClassname() const
	{
		return m_szClassname;
	}

	virtual BOOL PrimaryInRange(float fRange)
	{
		return true;
	}

	virtual float PrimMaxRange()
	{
		return BOT_VIEW_DISTANCE;
	}

	virtual float PrimMinRange()
	{
		return BOT_VIEW_DISTANCE;
	}

	virtual BOOL SecondaryInRange(float fRange)
	{
		return true;
	}

	int HudSlot() const
	{
		return m_iHudSlot;
	}

	int HudPosition() const
	{
		return m_iHudPosition;
	}

	virtual BOOL IsMelee()
	{
		return m_iAmmoIndex1 == -1;
	}

	BOOL IsPrimary() const;

	BOOL IsSecondary() const;

	int m_iAmmoIndex1;
	int m_iAmmoIndex2;

private:

	//weapon_preset_t *m_pWeaponPreset;

	char* m_szClassname;

	BOOL m_bRegistered; // Registered by the mod.

	int m_iPrimAmmoMax;
	int m_iSecAmmoMax;

	int m_iHudSlot;
	int m_iHudPosition;

	int m_iFlags;

	int m_iId;
};

class CWeaponPreset : public CWeapon
{
public:
	//int m_iId;
	BOOL m_bCanFireUnderWater;

	BOOL m_bHasPrimaryFire;
	BOOL m_bHasSecondaryFire;

	float m_fPrimMinRange;
	float m_fPrimMaxRange;

	float m_fSecMinRange;
	float m_fSecMaxRange;

	BOOL m_bIsMelee;

	//short int m_iModId;
	int m_iPriority;

	CWeaponPreset()
	{
		this->CWeaponPreset::Init();
	}

	CWeaponPreset(weapon_preset_t* pPreset)
	{
		m_bCanFireUnderWater = pPreset->m_bCanFireUnderWater;

		m_bHasPrimaryFire = pPreset->m_bHasPrimaryFire;
		m_bHasSecondaryFire = pPreset->m_bHasSecondaryFire;

		m_fPrimMinRange = pPreset->m_fPrimMinRange;
		m_fPrimMaxRange = pPreset->m_fPrimMaxRange;

		m_fSecMinRange = pPreset->m_fSecMinRange;
		m_fSecMaxRange = pPreset->m_fSecMaxRange;

		m_bIsMelee = pPreset->m_bIsMelee;

		//m_iModId;
		m_iPriority = pPreset->m_iPriority;
	}

	void Init() override//replaced memset with std::fill_n [APG]RoboCop[CL]
	{
		std::fill_n(reinterpret_cast<char*>(this), sizeof(CWeaponPreset), 0);
	}

	BOOL CanBeUsedUnderWater() override
	{
		return m_bCanFireUnderWater;
	}

	BOOL CanUsePrimary() override
	{
		return m_bHasPrimaryFire;
	}

	BOOL CanUseSecondary() override
	{
		return m_bHasSecondaryFire;
	}

	int GetPriority() override
	{
		return m_iPriority;
	}

	BOOL PrimaryInRange(float fRange) override
	{
		return fRange >= m_fPrimMinRange &&
			fRange <= m_fPrimMaxRange;
	}

	float PrimMaxRange() override
	{
		return m_fPrimMinRange;
	}

	float PrimMinRange() override
	{
		return m_fPrimMaxRange;
	}

	BOOL SecondaryInRange(float fRange) override
	{
		return fRange >= m_fSecMinRange &&
			fRange <= m_fSecMaxRange;
	}

	BOOL IsMelee() override
	{
		return m_bIsMelee != false;
	}
};

class CWeapons
{
public:

	CWeapons()
	{
		this->Init();
	}

	~CWeapons()
	{
		for (CWeapon*& m_Weapon : m_Weapons)
		{
			if (m_Weapon != nullptr)
			{
				delete m_Weapon;
				m_Weapon = nullptr;
			}
		}
	}

	void AddWeapon(int iId, const char* szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2);

	void Init()
	{
		for (CWeapon*& m_Weapon : m_Weapons)
		{
			m_Weapon = nullptr;
		}
		//std::memset(m_Weapons,0,sizeof(CWeapon)*MAX_WEAPONS);
	}

	CWeapon* GetWeapon(int iId) const
	{
		if (iId >= 0 && iId < MAX_WEAPONS)
		{
			//if ( m_Weapons[iId].IsRegistered() )
			return m_Weapons[iId];
		}
		return nullptr;
	}

private:
	CWeapon* m_Weapons[MAX_WEAPONS];
};

class CBotWeapon
{
public:

	int GetID() const
	{
		return m_iId;
	}

	int HudSlot() const
	{
		if (m_pWeaponInfo)
			return m_pWeaponInfo->HudSlot();

		return -1;
	}

	CBotWeapon()
	{
		std::memset(this, 0, sizeof(CBotWeapon));
	}

	void SetWeapon(int iId, int* iAmmoList);

	void Reset();
	
	void setHasWeapon(BOOL bVal);

	BOOL LowOnAmmo() const
	{
		// less in reserve than current clip
		return !IsMelee() && m_iAmmo1 && *m_iAmmo1 < m_iClip;
	}

	BOOL OutOfAmmo() const
	{
		if (m_iAmmo1)
		{
			return m_iClip <= 0 && !*m_iAmmo1;
		}

		return false;
	}

	int PrimaryInRange(float fRange) const
	{
		if (m_pWeaponInfo->PrimaryInRange(fRange))
			return 0;

		if (m_pWeaponInfo->PrimMinRange() > fRange)
			return -1;

		if (m_pWeaponInfo->PrimMaxRange() < fRange)
			return 1;

		return 0;
	}

	float PrimMinRange() const
	{
		if (m_pWeaponInfo)
			return m_pWeaponInfo->PrimMinRange();
		return 0;
	}

	BOOL SecondaryInRange(float fRange) const
	{
		if (m_pWeaponInfo)
			return m_pWeaponInfo->SecondaryInRange(fRange);
		return 1;
	}

	BOOL CanGetMorePrimaryAmmo() const
	{
		switch (static_cast<int>(m_iId))
		{
		case static_cast<int>(NSWeapon::MINE):
		case static_cast<int>(NSWeapon::KNIFE):
		case static_cast<int>(NSWeapon::WELDER):
			return false;
			//break;
		default:
			break;
		}

		return !IsMelee() && PrimaryAmmo() < m_pWeaponInfo->MaxPrimaryAmmo();
	}

	BOOL IsPrimary() const
	{
		assert(m_pWeaponInfo != NULL);

		return m_pWeaponInfo->IsPrimary();
	}

	BOOL IsSecondary() const
	{
		assert(m_pWeaponInfo != NULL);

		return m_pWeaponInfo->IsSecondary();
	}

	BOOL NeedToReload() const;

	BOOL CanReload() const;

	BOOL CanShootPrimary(edict_t* pEdict, float flFireDist, float flWallDist) const;

	BOOL CanShootSecondary() const
	{
		const int iSecAmmo = SecondaryAmmo();

		return iSecAmmo == -1 || iSecAmmo > 0;
	}

	int PrimaryAmmo() const
	{
		if (m_iAmmo1)
			return *m_iAmmo1;

		return -1;
	}

	int SecondaryAmmo() const
	{
		if (m_iAmmo2)
			return *m_iAmmo2;

		return -1;
	}

	BOOL HasWeapon(edict_t* pEdict) const;

	void RemoveWeapon()
	{
		m_bHasWeapon = false;
	}

	void UpdateWeapon(int iClip)
	{
		m_iClip = iClip;
	}

	char* GetClassname() const
	{
		if (m_pWeaponInfo)
			return m_pWeaponInfo->GetClassname();
		return nullptr;
	}

	BOOL IsMelee() const
	{
		if (m_pWeaponInfo == nullptr)
			return true;
		return m_pWeaponInfo->IsMelee();
	}

	BOOL CanBeUsedUnderWater() const
	{
		if (m_pWeaponInfo == nullptr)
			return true;

		return m_pWeaponInfo->CanBeUsedUnderWater();
	}

	int GetPriority() const
	{
		if (m_pWeaponInfo == nullptr)
		{
			return 0;
		}

		return m_pWeaponInfo->GetPriority();
	}

	void setAmmoArray(int* pAmmo1, int* pAmmo2 = nullptr)
	{
		if (pAmmo1)
		{
			m_iAmmo1 = pAmmo1;
			m_iAmmo2 = pAmmo1;
		}
		if (pAmmo2)
			m_iAmmo2 = pAmmo2;
	}

	void setReserve(int iRes)
	{
		m_iReserve = iRes;
	}

	int getReserve() const
	{
		return m_iReserve;
	}

	int getMaxClip() const
	{
		return m_iMaxClip;
	}

	void checkMaxClip(int clip)
	{
		if (clip > m_iMaxClip)
			m_iMaxClip = clip;
	}

private:
	int m_iId;

	int m_iClip;

	// Extra ammo for this weapon in (used for TS only right now)
	int m_iReserve;

	// store pointers to ammo stored
	int* m_iAmmo1;
	int* m_iAmmo2;

	int m_iMaxClip;

	CWeapon* m_pWeaponInfo;

	BOOL m_bHasWeapon;
};

class CBotWeapons
{
public:

	int GetBestWeaponId(CBot* pBot, edict_t* pEnemy);

	void RemoveWeapon(int iId)
	{
		m_Weapons[iId].RemoveWeapon();
	}

	void RemoveWeapons()
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
			this->RemoveWeapon(i);
	}

	BOOL HasWeapon(edict_t* pEdict, const char* szClassname) const;

	BOOL HasWeapon(edict_t* pEdict, int iId) const
	{
		if (iId > 0 && iId < MAX_WEAPONS)
			return m_Weapons[iId].HasWeapon(pEdict);
		return false;
	}

	void setHasWeapon(int id, BOOL bVal)
	{
		for (CBotWeapon& m_Weapon : m_Weapons)
		{
			if (m_Weapon.GetID() == id)
			{
				m_Weapon.setHasWeapon(bVal);
				return;
			}
		}
	}

	void AddWeapon(int iId);

	void UpdateWeapon(int iId, int iClip)
	{
		m_Weapons[iId].UpdateWeapon(iClip);
	}

	CBotWeapon* GetWeapon(int iId)
	{
		if (iId >= 0 && iId < MAX_WEAPONS)
			return &m_Weapons[iId];

		return nullptr;
	}

	int GetPrimaryWeaponId() const
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (!m_Weapons[i].HasWeapon(nullptr))
				continue;
			if (m_Weapons[i].IsPrimary())
				return i;
		}

		return 0;
	}

	int GetSecondaryWeaponId() const
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (!m_Weapons[i].HasWeapon(nullptr))
				continue;
			if (m_Weapons[i].IsSecondary())
				return i;
		}

		return 0;
	}

	void UpdateAmmo(int iIndex, int iAmount)
	{
		if (iIndex >= 0 && iIndex < MAX_AMMO_SLOTS)
			m_iAmmo[iIndex] = iAmount;
	}

private:
	CBotWeapon m_Weapons[MAX_WEAPONS];

	int m_iAmmo[MAX_AMMO_SLOTS] = {};
};

class CompareBotWeapon
{
public:
	bool operator()(CBotWeapon* a, CBotWeapon* b) const
	{
		return a->GetPriority() < b->GetPriority();
	}
};

#endif // BOT_WEAPONS_H
