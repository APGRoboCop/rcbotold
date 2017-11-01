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

#ifndef __BOT_WEAPONS_H__
#define __BOT_WEAPONS_H__

#include "generic_class.h"
#include "weaponinfo.h"

///////////////////////////////////////
// WEAPON ID's BELOW...

////////////////////////////////////////
// NATURAL SELECTION
// FLYRA (Charlie Cleveland : flayra@overmind.org)
enum
{
	NS_WEAPON_NONE = 0,

	// Alien weapons
	NS_WEAPON_CLAWS,
	NS_WEAPON_SPIT,
	NS_WEAPON_SPORES,
	NS_WEAPON_SPIKE,
	NS_WEAPON_BITE,		// Level 1 bite
	NS_WEAPON_BITE2,	// Level 3 bite
	NS_WEAPON_SWIPE,
	NS_WEAPON_WEBSPINNER,
	NS_WEAPON_METABOLIZE,
	NS_WEAPON_PARASITE,
	NS_WEAPON_BLINK,
	NS_WEAPON_DIVINEWIND,

	// Adding a new weapon?  Don't forget to add it's weight in AvHGamerules::GetWeightForItemAndAmmo(AvHWeaponID inWeapon, int inNumRounds)
	NS_WEAPON_KNIFE,
	NS_WEAPON_PISTOL,
	NS_WEAPON_MG,
	NS_WEAPON_SONIC,
	NS_WEAPON_HMG,
	NS_WEAPON_WELDER,
	NS_WEAPON_MINE,
	NS_WEAPON_GRENADE_GUN,

	// Abilities
	NS_ABILITY_LEAP,
	NS_ABILITY_CHARGE,

	NS_WEAPON_UMBRA,
	NS_WEAPON_PRIMALSCREAM,
	NS_WEAPON_BILEBOMB,
	NS_WEAPON_ACIDROCKET,
	NS_WEAPON_HEALINGSPRAY,
	NS_WEAPON_BABBLER,
	NS_WEAPON_STOMP,
	NS_WEAPON_DEVOUR

	// Can't go over 32 (client.cpp, GetWeaponData())

};

////////////////////
// HL DEATHMATCH

enum
{
// weapon ID values for Valve's Half-Life Deathmatch
 VALVE_WEAPON_CROWBAR=1,
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

#define SVEN_WEAPON_UZIAKIMBO	  16
#define SVEN_WEAPON_UZI	          17
#define SVEN_WEAPON_MEDKIT        18
#define SVEN_WEAPON_PIPEWRENCH    19
#define SVEN_WEAPON_MINIGUN       21
#define SVEN_WEAPON_GRAPPLE       22
#define SVEN_WEAPON_SNIPERRIFLE   23

#define VALVE_MAX_NORMAL_BATTERY   100
#define VALVE_HORNET_MAX_CARRY      8


// weapon ID values for Valve's Team Fortress Classic & 1.5
#define TF_WEAPON_UNKNOWN1       1
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


// weapon ID values for Gearbox's OpFor Deathmatch
#define GEARBOX_WEAPON_CROWBAR       1
#define GEARBOX_WEAPON_GLOCK         2
#define GEARBOX_WEAPON_PYTHON        3
#define GEARBOX_WEAPON_MP5           4
#define GEARBOX_WEAPON_CHAINGUN      5
#define GEARBOX_WEAPON_CROSSBOW      6
#define GEARBOX_WEAPON_SHOTGUN       7
#define GEARBOX_WEAPON_RPG           8
#define GEARBOX_WEAPON_GAUSS         9
#define GEARBOX_WEAPON_EGON         10
#define GEARBOX_WEAPON_HORNETGUN    11
#define GEARBOX_WEAPON_HANDGRENADE  12
#define GEARBOX_WEAPON_TRIPMINE     13
#define GEARBOX_WEAPON_SATCHEL      14
#define GEARBOX_WEAPON_SNARK        15
#define GEARBOX_WEAPON_GRAPPLE      16
#define GEARBOX_WEAPON_EAGLE        17
#define GEARBOX_WEAPON_PIPEWRENCH   18
#define GEARBOX_WEAPON_M249         19
#define GEARBOX_WEAPON_DISPLACER    20
#define GEARBOX_WEAPON_UNKNOWN21    21
#define GEARBOX_WEAPON_SHOCKRIFLE   22
#define GEARBOX_WEAPON_SPORELAUNCHER 23
#define GEARBOX_WEAPON_SNIPERRIFLE  24
#define GEARBOX_WEAPON_KNIFE        25


// weapon ID values for FrontLineForce
#define FLF_WEAPON_AK5           10
#define FLF_WEAPON_UNKNOWN11     11
#define FLF_WEAPON_UNKNOWN12     12
#define FLF_WEAPON_UNKNOWN13     13
#define FLF_WEAPON_UNKNOWN14     14
#define FLF_WEAPON_UNKNOWN15     15
#define FLF_WEAPON_MP5SD         16
#define FLF_WEAPON_M4            17
#define FLF_WEAPON_FLASHBANG     18
#define FLF_WEAPON_HEGRENADE     19
#define FLF_WEAPON_MP5A2         20
#define FLF_WEAPON_UMP45         21
#define FLF_WEAPON_SPAS12        22
#define FLF_WEAPON_BERETTA       23
#define FLF_WEAPON_KNIFE         24
#define FLF_WEAPON_MAC10         25
#define FLF_WEAPON_MK23          26
#define FLF_WEAPON_MSG90         27
#define FLF_WEAPON_FAMAS         28
#define FLF_WEAPON_HK21          29

#define DMC_WEAPON_AXE            1
#define DMC_WEAPON_SHOTGUN        2
#define DMC_WEAPON_DOUBLESHOTGUN  3
#define DMC_WEAPON_NAILGUN        4
#define DMC_WEAPON_SUPERNAIL      5
#define DMC_WEAPON_GRENADE1       6
#define DMC_WEAPON_ROCKET1        7
#define DMC_WEAPON_LIGHTNING      8

#define DMC_EXTRA_WEAPON                 8

#define DMC_SHELLS                       9
#define DMC_NAILS                        10
#define DMC_ROCKETS                      11
#define DMC_CELLS                        12

#define DMC_ARMOR1                       13
#define DMC_ARMOR2                       14
#define DMC_ARMOR3                       15
#define DMC_SUPERHEALTH                  16

#define DMC_KEY1                         17
#define DMC_KEY2                         18

#define DMC_INVISIBILITY                 19
#define DMC_INVULNERABILITY              20
#define DMC_SUIT                         21
#define DMC_QUAD                         22

typedef enum 
{	
	TS_Glock18= 1,
	TS_MiniUzi= 3,
	TS_BenelliM3 =4,
	TS_M4A1= 5,
	TS_MP5SD= 6,
	TS_MP5K=7,
	TS_AkimboBerettas =8,
	TS_SocomMk23 = 9,
	TS_Usas12= 11,
	TS_DesertEagle = 12,
	TS_Ak47= 13,
	TS_FiveSeven = 14,
	TS_SteyrAug = 15,
	TS_SteyrTmp = 17,
	TS_BarrettM82 = 18,
	TS_HKPdw = 19,
	TS_Spas12= 20,
	TS_Akimbocolts = 21,
	TS_Glock20 = 22,
	TS_Mac10= 23,
	TS_CombatKnife = 25,
	TS_Mossberg500 = 26,
	TS_M16A4= 27,
	TS_RugerMk1 = 28,
	TS_M61Grenade = 24,
	TS_C4= 29,
	TS_RagingBull = 31,
	TS_M60= 32,
	TS_Sawedoff = 33,
	TS_Katana= 34,
	TS_SealKnife = 35,
	TS_KungFu = 36
}eTSWeaponID;


void GetNoWeaponArray ( int *Array );
void GetArrayOfExplosives ( int *Array );


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

	int m_fPrimMinRange;
	int m_fPrimMaxRange;

    int m_fSecMinRange;
	int m_fSecMaxRange;

	BOOL m_bIsMelee;

	short int m_iModId;
	int m_iPriority;

}weapon_preset_t;



class CWeaponPresets
{
public:
	void ReadPresets ( void );
	
	weapon_preset_t *GetPreset(short int iModId, int iWeaponId)
	{
		dataStack<weapon_preset_t> tempStack = m_Presets;
		weapon_preset_t *pPreset;

		while ( !tempStack.IsEmpty() )
		{
			pPreset = tempStack.ChoosePointerFromStack();

			if (( pPreset->m_iId == iWeaponId ) &&
			    ( pPreset->m_iModId == iModId ))
			{
				tempStack.Init();

				return pPreset;
			}
		}

		return NULL;
	}

	void Destroy ( void )
	{
		m_Presets.Destroy();
	}

private:
	dataStack<weapon_preset_t> m_Presets;
};

class CWeapon
{
public:
	CWeapon ( )
	{
		this->Init();
	}

	void Init ( void )
	{
		m_iAmmoIndex1 = -1;
		m_iAmmoIndex2 = -1;
		
		m_szClassname = NULL;
		
		m_bRegistered = FALSE; // Registered by the mod.
		
		m_iPrimAmmoMax = -1;
		m_iSecAmmoMax = -1;
		
		m_iHudSlot = -1;
		m_iHudPosition = -1;
		
		m_iFlags = 0;
		
		m_iId = 0;
		
		//memset(this,0,sizeof(CWeapon));
	}

	void SetWeapon ( int iId, const char *szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2 );

	virtual BOOL CanBeUsedUnderWater ( void )
	{
		// no preset
			return TRUE;
	}

	inline int MaxPrimaryAmmo ( void )
	{
		return m_iPrimAmmoMax;
	}

	virtual BOOL CanUsePrimary ( void )
	{
		return TRUE;
	}

	virtual BOOL CanUseSecondary ( void )
	{
		// no preset
		return TRUE;
	}

	BOOL IsRegistered ( void )
	{
		return m_bRegistered;
	}

	virtual int GetPriority ( void )
	{
		return 0;
	}

	inline char *GetClassname ( void )
	{
		return m_szClassname;
	}

	virtual BOOL PrimaryInRange ( float fRange )
	{
		return TRUE;
	}

	virtual float PrimMaxRange ( void )
	{
		return BOT_VIEW_DISTANCE;
	}

	virtual float PrimMinRange ( void )
	{
		return BOT_VIEW_DISTANCE;
	}

	virtual BOOL SecondaryInRange ( float fRange )
	{
		return TRUE;
	}

	int HudSlot ( void )
	{
		return m_iHudSlot;
	}	

	int HudPosition ( void )
	{
		return m_iHudPosition;
	}

	virtual BOOL IsMelee ( void )
	{
		return (m_iAmmoIndex1 == -1);
	}

	BOOL IsPrimary ( void );

	BOOL IsSecondary ( void );

	int m_iAmmoIndex1;
	int m_iAmmoIndex2;

private:

	//weapon_preset_t *m_pWeaponPreset;

	char *m_szClassname;

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

	int m_fPrimMinRange;
	int m_fPrimMaxRange;

    int m_fSecMinRange;
	int m_fSecMaxRange;

	BOOL m_bIsMelee;

	//short int m_iModId;
	int m_iPriority;
	
	CWeaponPreset ( )
	{
		this->Init();
	}
	
	CWeaponPreset ( weapon_preset_t *pPreset )
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
	
	void Init ( void )
	{
		memset(this,0,sizeof(CWeaponPreset));
	}	

	BOOL CanBeUsedUnderWater ( void )
	{
		return m_bCanFireUnderWater;
	}

	BOOL CanUsePrimary ( void )
	{
		return (m_bHasPrimaryFire);
	}

	BOOL CanUseSecondary ( void )
	{
		return (m_bHasSecondaryFire);
	}

	int GetPriority ( void )
	{
		return ( m_iPriority );
	}

	BOOL PrimaryInRange ( float fRange )
	{
		return (( fRange >= m_fPrimMinRange )&&
			    ( fRange <= m_fPrimMaxRange ));
	}

	float PrimMaxRange ( void )
	{
		return m_fPrimMinRange;
	}

	float PrimMinRange ( void )
	{
		return m_fPrimMaxRange;
	}

	BOOL SecondaryInRange ( float fRange )
	{
		return (( fRange >= m_fSecMinRange )&&
			    ( fRange <= m_fSecMaxRange ));
	}

	BOOL IsMelee ( void )
	{
		return ( m_bIsMelee == 1 );
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
		int i;

		for ( i = 0; i < MAX_WEAPONS; i ++ )
		{
			if ( m_Weapons[i] != NULL )
			{
				delete m_Weapons[i];
				m_Weapons[i] = NULL;
			}
		}
	}

	void AddWeapon ( int iId, const char *szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2 );

	void Init ( void )
	{
		int i;

		for ( i = 0; i < MAX_WEAPONS; i ++ )
		{
			m_Weapons[i] = NULL;
		}
		//memset(m_Weapons,0,sizeof(CWeapon)*MAX_WEAPONS);
	}

	CWeapon *GetWeapon ( int iId )
	{
		if (( iId >= 0 ) && ( iId < MAX_WEAPONS ))
		{
			//if ( m_Weapons[iId].IsRegistered() )
			return m_Weapons[iId];
		}
		return NULL;
	}	

private:
	CWeapon *m_Weapons[MAX_WEAPONS];

};

class CBotWeapon
{
public:

	inline int GetID ( void ) const
	{
		return m_iId;
	}

	int HudSlot ( void )
	{
		if ( m_pWeaponInfo )
			return m_pWeaponInfo->HudSlot();
		
		return -1;
	}

	CBotWeapon ()
	{
		memset(this,0,sizeof(CBotWeapon));
	}

	void SetWeapon ( int iId, int *iAmmoList );

	void setHasWeapon ( BOOL bVal );

	BOOL LowOnAmmo ( void )
	{
		// less in reserve than current clip
		return (!IsMelee() && m_iAmmo1 && (*m_iAmmo1 < m_iClip) );
	}


	BOOL OutOfAmmo ( void )
	{
		if ( m_iAmmo1 )
		{
			return ( (m_iClip <= 0) && !*m_iAmmo1 );
		}

		return FALSE;		
	}

	int PrimaryInRange ( float fRange )
	{
		if ( m_pWeaponInfo->PrimaryInRange(fRange) )
			return 0;

		if ( m_pWeaponInfo->PrimMinRange() > fRange )
			return -1;

		if ( m_pWeaponInfo->PrimMaxRange() < fRange )
			return 1;
		
		return 0;
	}

	float PrimMinRange ( void )
	{
		if ( m_pWeaponInfo )
			return m_pWeaponInfo->PrimMinRange();
		return 0;
	}

	BOOL SecondaryInRange ( float fRange )
	{
		if ( m_pWeaponInfo )
			return m_pWeaponInfo->SecondaryInRange(fRange);
		return 1;
	}

	BOOL CanGetMorePrimaryAmmo ( void )
	{
		switch ( m_iId )
		{
		case NS_WEAPON_MINE:
		case NS_WEAPON_KNIFE:
		case NS_WEAPON_WELDER:
			return FALSE;
			break;
		default:
			break;
		}

		return ( !IsMelee() && ( PrimaryAmmo() < m_pWeaponInfo->MaxPrimaryAmmo() ) );
	}

	BOOL IsPrimary ( void )
	{
		assert(m_pWeaponInfo != NULL);

		return m_pWeaponInfo->IsPrimary();
	}

	BOOL IsSecondary ( void )
	{
		assert(m_pWeaponInfo != NULL);

		return m_pWeaponInfo->IsSecondary();
	}

	BOOL NeedToReload ( void );

	BOOL CanReload ( void );

	BOOL CanShootPrimary ( edict_t *pEdict, float flFireDist, float flWallDist );

	BOOL CanShootSecondary ( void )
	{
		int iSecAmmo = SecondaryAmmo();

		return ((iSecAmmo == -1) || (iSecAmmo > 0));
	}

	inline int PrimaryAmmo ( void )
	{
		if ( m_iAmmo1 )
			return *m_iAmmo1;

		return -1;
	}

	inline int SecondaryAmmo ( void )
	{
		if ( m_iAmmo2 )
			return *m_iAmmo2;

		return -1;
	}

	BOOL HasWeapon ( edict_t *pEdict );

	void RemoveWeapon ( void )
	{
		m_bHasWeapon = FALSE;
	}

	void UpdateWeapon ( int iClip )
	{
		m_iClip = iClip;
	}

    char *GetClassname ( void )
	{
		if ( m_pWeaponInfo )
			return m_pWeaponInfo->GetClassname();
		else
			return NULL;
	}

	BOOL IsMelee ( void )
	{
		if ( m_pWeaponInfo == NULL )
			return TRUE;
		return m_pWeaponInfo->IsMelee();
	}

	BOOL CanBeUsedUnderWater ( void )
	{		
		if ( m_pWeaponInfo == NULL )
			return TRUE;

		return m_pWeaponInfo->CanBeUsedUnderWater();
	}

	int GetPriority ( void )
	{
		if ( m_pWeaponInfo == NULL )
		{
			return 0;
		}

		return m_pWeaponInfo->GetPriority();
	}

	inline void setAmmoArray ( int *pAmmo1, int *pAmmo2 = NULL )
	{
		if ( pAmmo1 )
		{
			m_iAmmo1 = pAmmo1;
			m_iAmmo2 = pAmmo1;
		}
		if ( pAmmo2 )
			m_iAmmo2 = pAmmo2;

	}

	inline void setReserve ( int iRes )
	{
		m_iReserve = iRes;
	}

	int getReserve ()
	{
		return m_iReserve;
	}

	int getMaxClip ()
	{
		return m_iMaxClip;
	}

	void checkMaxClip (int clip)
	{
		if ( clip > m_iMaxClip )
			m_iMaxClip = clip;
	}

private:
	int m_iId;

	int m_iClip;

	// Extra ammo for this weapon in (used for TS only right now)
	int m_iReserve;

	// store pointers to ammo stored
	int *m_iAmmo1;
	int *m_iAmmo2;

	int m_iMaxClip;
	
	CWeapon *m_pWeaponInfo;

	BOOL m_bHasWeapon;
};

class CBotWeapons
{
public:

	int GetBestWeaponId( CBot *pBot, edict_t *pEnemy );

	void RemoveWeapon ( int iId )
	{
		m_Weapons[iId].RemoveWeapon();
	}

	void RemoveWeapons ( void )
	{
		int i;

		for ( i = 0; i < MAX_WEAPONS; i ++ )
			this->RemoveWeapon(i);
	}

	BOOL HasWeapon ( edict_t *pEdict, char *szClassname );

	BOOL HasWeapon ( edict_t *pEdict, int iId )
	{
		if ((iId > 0 ) && (iId < MAX_WEAPONS))
			return m_Weapons[iId].HasWeapon(pEdict);
		else
			return FALSE;
	}

	void setHasWeapon ( int id, BOOL bVal )
	{
		for ( int i = 0; i < MAX_WEAPONS; i ++ )
		{
			if ( m_Weapons[i].GetID() == id )
			{
				m_Weapons[i].setHasWeapon(bVal);
				return;
			}
		}
	}

	void AddWeapon ( int iId );

	void UpdateWeapon ( int iId, int iClip )
	{
		m_Weapons[iId].UpdateWeapon(iClip);
	}

	CBotWeapon *GetWeapon ( int iId )
	{
		if (( iId >= 0 ) && ( iId < MAX_WEAPONS ))
			return &m_Weapons[iId];

		return NULL;
	}

	int GetPrimaryWeaponId ( void )
	{
		int i;

		for ( i = 0; i < MAX_WEAPONS; i ++ )
		{
			if ( !m_Weapons[i].HasWeapon(NULL) )
				continue;
			if ( m_Weapons[i].IsPrimary() )
				return i;
		}

		return 0;
	}

	int GetSecondaryWeaponId ( void )
	{
		int i;

		for ( i = 0; i < MAX_WEAPONS; i ++ )
		{
			if ( !m_Weapons[i].HasWeapon(NULL) )
				continue;
			if ( m_Weapons[i].IsSecondary() )
				return i;
		}

		return 0;
	}

	void UpdateAmmo ( int iIndex, int iAmount )
	{
		if (( iIndex >= 0 ) && ( iIndex < MAX_AMMO_SLOTS ))
			m_iAmmo[iIndex] = iAmount;
	}

private:
	CBotWeapon m_Weapons[MAX_WEAPONS];

	int m_iAmmo[MAX_AMMO_SLOTS];
};

class CompareBotWeapon
{
public:
	bool operator()(CBotWeapon *a, CBotWeapon *b)
	{
		return a->GetPriority()<b->GetPriority();
	}
};


#endif // BOT_WEAPONS_H

