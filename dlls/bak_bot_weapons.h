//////////////////////////////////////////////////
// RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
//
// (http://www.rcbot.net)
//
// Based on botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_client.h
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
 * Copyright (c) 2003 Paul M'(cheeseh@rcbot.net/paul.murphy@ntlworld.com)
 * If using any of these components please keep this tag
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
		memset(this,0,sizeof(CWeapon));
	}

	void SetWeapon ( int iId, const char *szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2 );

	BOOL CanBeUsedUnderWater ( void )
	{
		if ( m_pWeaponPreset == NULL ) // no preset
			return TRUE;
		
		return m_pWeaponPreset->m_bCanFireUnderWater;
	}

	inline int MaxPrimaryAmmo ( void )
	{
		return m_iPrimAmmoMax;
	}

	BOOL CanUsePrimary ( void )
	{
		if ( m_pWeaponPreset == NULL ) // no preset
			return TRUE;

		return (m_pWeaponPreset->m_bHasPrimaryFire);
	}

	BOOL CanUseSecondary ( void )
	{
		if ( m_pWeaponPreset == NULL ) // no preset
			return TRUE;

		return (m_pWeaponPreset->m_bHasSecondaryFire);
	}

	BOOL IsRegistered ( void )
	{
		return m_bRegistered;
	}

	int GetPriority ( void )
	{
		if ( m_pWeaponPreset == NULL )
			return 0;

		return ( m_pWeaponPreset->m_iPriority );
	}

	inline char *GetClassname ( void )
	{
		return m_szClassname;
	}

	BOOL PrimaryInRange ( float fRange )
	{
		if ( m_pWeaponPreset == NULL )
			return TRUE;

		return (( fRange >= m_pWeaponPreset->m_fPrimMinRange )&&
			    ( fRange <= m_pWeaponPreset->m_fPrimMaxRange ));
	}

	float PrimMaxRange ( void )
	{
		return m_pWeaponPreset->m_fPrimMinRange;
	}

	float PrimMinRange ( void )
	{
		return m_pWeaponPreset->m_fPrimMaxRange;
	}

	BOOL SecondaryInRange ( float fRange )
	{
		if ( m_pWeaponPreset == NULL )
			return TRUE;

		return (( fRange >= m_pWeaponPreset->m_fSecMinRange )&&
			    ( fRange <= m_pWeaponPreset->m_fSecMaxRange ));
	}

	int HudSlot ( void )
	{
		return m_iHudSlot;
	}

	BOOL IsMelee ( void )
	{
		if ( m_pWeaponPreset )
		{
			return ( m_pWeaponPreset->m_bIsMelee == 1 );
		}

		return (m_iAmmoIndex1 == -1);
	}

	BOOL IsPrimary ( void );

	BOOL IsSecondary ( void );

	int m_iAmmoIndex1;
	int m_iAmmoIndex2;

private:

	weapon_preset_t *m_pWeaponPreset;

	char *m_szClassname;

	BOOL m_bRegistered; // Registered by the mod.

	int m_iPrimAmmoMax;
	int m_iSecAmmoMax;

	int m_iHudSlot;
	int m_iHudPosition;

	int m_iFlags;

	int m_iId;
};

class CWeapons
{
public:

	CWeapons()
	{
		this->Init();
	}

	void AddWeapon ( int iId, const char *szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2 )
	{
		if (( iId >= 0 ) && ( iId < MAX_WEAPONS ))
		{
			m_Weapons[iId].SetWeapon(iId,szClassname,iPrimAmmoMax,iSecAmmoMax,iHudSlot,iHudPosition,iFlags,iAmmoIndex1,iAmmoIndex2);
		}
	}

	void Init ( void )
	{
		memset(m_Weapons,0,sizeof(CWeapon)*MAX_WEAPONS);
	}

	CWeapon *GetWeapon ( int iId )
	{
		if (( iId >= 0 ) && ( iId < MAX_WEAPONS ))
		{
			//if ( m_Weapons[iId].IsRegistered() )
				return &m_Weapons[iId];
		}
		return NULL;
	}	

private:
	CWeapon m_Weapons[MAX_WEAPONS];

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
		return m_pWeaponInfo->SecondaryInRange(fRange);
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

	BOOL CanShootPrimary ( edict_t *pEdict, float flFireDist );

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

private:
	int m_iId;

	int m_iClip;

	int *m_iAmmo1;
	int *m_iAmmo2;
	
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
/*
typedef struct
{
   char szClassname[64];
   int  iAmmo1;     // ammo index for primary ammo
   int  iAmmo1Max;  // max primary ammo
   int  iAmmo2;     // ammo index for secondary ammo
   int  iAmmo2Max;  // max secondary ammo
   int  iSlot;      // HUD slot (0 based)
   int  iPosition;  // slot position
   int  iId;        // weapon ID
   int  iFlags;     // flags???
} bot_weapon_t;*/
/*
class CWeaponPreset
{

}

class CWeaponPresets
{

}*/

/*
class CWeapon
{
public:
	CWeapon()
	{
		m_szClassname = NULL;
		m_iId = 0;
	}

	CWeapon(char *szClassname, int iId, int iAmmo1Max, int iAmmo2Max, int iSlot, int iPosition, int iFlags );
	
	int GetWeaponId ( void ) const
	{
		return m_iId;
	}

	BOOL CanFireUnderWater ( void );

private:

	char *m_szClassname;
	
	int m_iId;
	
	int  m_iAmmo1Max;  // max primary ammo
	int  m_iAmmo2Max;  // max secondary ammo

	int  m_iSlot;      // HUD slot (0 based)
	int  m_iPosition;  // slot position

	int  m_iFlags;     // flags???

	weapon_preset_t *m_pWeaponInfo;
};
*/

#endif // BOT_WEAPONS_H

