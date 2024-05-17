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
 // Weapons available in game and weapons that bots have
 // including ammo info etc.
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot_const.h"
#include "bot.h"
#include "waypoint.h"
#include "weaponinfo.h"
#include "bot_weapons.h"
#include "perceptron.h"

#include <queue>

// begin -- TS metamod weapon restriction plugin

const char* pszTSWeaponModels[] = { "0","models/w_glock18.mdl","2 No gun","models/w_uzi.mdl"
,"models/w_m3.mdl","models/w_m4.mdl","models/w_mp5sd.mdl","models/w_mp5k.mdl"
,"models/w_berettas.mdl","models/w_mk23.mdl","models/w_mk23_akimbo.mdl","models/w_usas.mdl"
,"models/w_desert.mdl","models/w_ak47.mdl","models/w_fnh.mdl","models/w_aug.mdl"
,"models/w_uzi.mdl","models/w_tmp.mdl","models/w_m82.mdl","models/w_pdw.mdl"
,"models/w_spas12.mdl","models/w_gold.mdl","models/w_glock22.mdl","models/w_ump.mdl"
,"models/w_m61.mdl","models/w_knife.mdl","models/w_mossberg.mdl","models/w_m16.mdl"
,"models/w_ruger.mdl","29 no gun here","30 no gun here","models/w_bull.mdl"
,"models/w_m60.mdl","models/w_sawedoff.mdl","models/w_katana.mdl","models/w_sealknife.mdl" };

// end

extern CBotGlobals gBotGlobals;

void CWeapon::SetWeapon(int iId, const char* szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2)
{
	if (m_szClassname == nullptr)
		m_szClassname = gBotGlobals.m_Strings.GetString(szClassname);

	m_iId = iId;
	m_iPrimAmmoMax = iPrimAmmoMax;
	m_iSecAmmoMax = iSecAmmoMax;
	m_iHudSlot = iHudSlot;
	m_iHudPosition = iHudPosition;

	m_iAmmoIndex1 = iAmmoIndex1;
	m_iAmmoIndex2 = iAmmoIndex2;

	m_bRegistered = true;
}

void CBotWeapon::Reset() //TODO: Experimental [APG]RoboCop[CL]
{
	// Reset fields to their initial state
	m_iId = 0;
	m_pWeaponInfo = nullptr;
	m_bHasWeapon = false;
	m_iAmmo1 = nullptr;
	m_iAmmo2 = nullptr;
}

BOOL CBotWeapon::CanReload() const
{
	if (IsMelee())
		return false;

	if (m_iAmmo1)
	{
		return *m_iAmmo1 > 0;
	}

	return false;
}

BOOL CWeapon::IsPrimary() const
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		return HudSlot() == 0;
	default:
		return false;
	}
}

BOOL CWeapon::IsSecondary() const
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		return HudSlot() == 1;
	default:
		return false;
	}
}

void CBotWeapon::SetWeapon(int iId, int* iAmmoList)
{
	this->Reset();

	m_iId = iId;
	m_pWeaponInfo = gBotGlobals.m_Weapons.GetWeapon(iId);

	if (m_pWeaponInfo != nullptr)
	{
		m_bHasWeapon = true;

		const int iAmmoIndex1 = m_pWeaponInfo->m_iAmmoIndex1;
		const int iAmmoIndex2 = m_pWeaponInfo->m_iAmmoIndex2;

		if (iAmmoList && iAmmoIndex1 != -1)
			m_iAmmo1 = &iAmmoList[iAmmoIndex1];

		if (iAmmoList && iAmmoIndex2 != -1)
			m_iAmmo2 = &iAmmoList[iAmmoIndex2];
	}
}

void CBotWeapon::setHasWeapon(BOOL bVal)
{
	m_bHasWeapon = bVal;
}

void CBotWeapons::AddWeapon(int iId)
{
	m_Weapons[iId].SetWeapon(iId, m_iAmmo);
}

void CWeapons::AddWeapon(int iId, const char* szClassname, int iPrimAmmoMax, int iSecAmmoMax, int iHudSlot, int iHudPosition, int iFlags, int iAmmoIndex1, int iAmmoIndex2)
{
	if (iId >= 0 && iId < MAX_WEAPONS)
	{
		if (m_Weapons[iId] == nullptr)
		{
			weapon_preset_t* pPreset = gBotGlobals.m_WeaponPresets.GetPreset(gBotGlobals.m_iCurrentMod, iId);

			if (pPreset == nullptr)
			{
				m_Weapons[iId] = new CWeapon();
			}
			else
			{
				m_Weapons[iId] = new CWeaponPreset(pPreset);
			}

			m_Weapons[iId]->SetWeapon(iId, szClassname, iPrimAmmoMax, iSecAmmoMax, iHudSlot, iHudPosition, iFlags, iAmmoIndex1, iAmmoIndex2);
		}
	}
}

void CWeaponPresets::ReadPresets()
{
	weapon_preset_t sWeaponPreset;

	char filename[512];

	UTIL_BuildFileName(filename, BOT_WEAPON_PRESETS_FILE);

	std::FILE* fp = std::fopen(filename, "r");

	if (fp == nullptr)
		return;

	char buffer[256];

	int iModId = 0;
	int iWeaponId = 0;
	int iValue;

	// bSkipMod will be true when the weapons are not for the current mod
	// and do not need to be loaded
	BOOL bSkipMod = false;

	std::memset(&sWeaponPreset, 0, sizeof(weapon_preset_t));

	while (std::fgets(buffer, 255, fp) != nullptr)
	{
		if (buffer[0] == '#')
			continue;

		size_t iLength = std::strlen(buffer);

		if (iLength <= 0) // blank line...
			continue;

		if (buffer[iLength - 1] == '\n')
		{
			buffer[--iLength] = 0;
		}

		if (iLength == 0) // blank line...
			continue;

#ifdef __linux__
		if (buffer[iLength - 1] == '\r')
		{
			buffer[--iLength] = 0;
		}

		if (iLength == 0) // blank line...
			continue;
#endif

		if (std::sscanf(buffer, "[mod_id=%d]", &iValue) == 1)
		{
			bSkipMod = iValue != gBotGlobals.m_iCurrentMod;

			if (!bSkipMod)
				iModId = iValue;

			continue;
		}

		if (bSkipMod)
			continue;

		if (std::sscanf(buffer, "[weapon_id=%d]", &iValue) == 1)
		{
			iWeaponId = iValue;

			std::memset(&sWeaponPreset, 0, sizeof(weapon_preset_t));

			sWeaponPreset.m_iId = iWeaponId;
			sWeaponPreset.m_iModId = static_cast<short int>(iModId);
			continue;
		}

		if (std::sscanf(buffer, "underwater=%d", &iValue) == 1)
		{
			sWeaponPreset.m_bCanFireUnderWater = iValue;

			continue;
		}

		if (std::sscanf(buffer, "primaryfire=%d", &iValue) == 1)
		{
			sWeaponPreset.m_bHasPrimaryFire = iValue;

			continue;
		}

		if (std::sscanf(buffer, "secondaryfire=%d", &iValue) == 1)
		{
			sWeaponPreset.m_bHasSecondaryFire = iValue;
			continue;
		}

		if (std::sscanf(buffer, "primary_min_range=%d", &iValue) == 1)
		{
			sWeaponPreset.m_fPrimMaxRange = static_cast<float>(iValue);
			continue;
		}

		if (std::sscanf(buffer, "primary_max_range=%d", &iValue) == 1)
		{
			sWeaponPreset.m_fPrimMaxRange = static_cast<float>(iValue);
			continue;
		}

		if (std::sscanf(buffer, "secondary_min_range=%d", &iValue) == 1)
		{
			sWeaponPreset.m_fSecMinRange = static_cast<float>(iValue);
			continue;
		}

		if (std::sscanf(buffer, "secondary_max_range=%d", &iValue) == 1)
		{
			sWeaponPreset.m_fSecMaxRange = static_cast<float>(iValue);
			continue;
		}

		if (std::sscanf(buffer, "is_melee=%d", &iValue) == 1)
		{
			sWeaponPreset.m_bIsMelee = iValue;
			continue;
		}

		if (std::sscanf(buffer, "priority=%d", &iValue) == 1)
		{
			sWeaponPreset.m_iPriority = iValue;
			continue;
		}

		if (std::strcmp(buffer, "[/weapon]") == 0)
			m_Presets.Push(sWeaponPreset);
	}

	std::fclose(fp);
}

BOOL CBotWeapon::HasWeapon(edict_t* pEdict) const
{
	if (pEdict)
	{
		switch (gBotGlobals.m_iCurrentMod)
		{
		case MOD_NS:

			if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL))
				return m_bHasWeapon;

			if (pEdict->v.team == TEAM_ALIEN)
			{
				if (m_bHasWeapon)
				{
					edict_t* pWeapon = nullptr;

					const char* szClassname = this->GetClassname();

					if (szClassname == nullptr)
						return false; // error

					while ((pWeapon = UTIL_FindEntityByClassname(pWeapon, szClassname)) != nullptr)
					{
						if (!(pWeapon->v.effects & EF_NODRAW))
							continue;

						if (pWeapon->v.origin == pEdict->v.origin)
							break;
					}

					if (pWeapon)
					{
						/*if ( !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_NOT_NS3_FINAL) )
						{
							if ( m_pWeaponInfo )
								return (m_pWeaponInfo->HudSlot())<=(UTIL_GetNumHives()+1);

							return false;
						}
						else*/ if (pWeapon->v.iuser3 != 1)
						{
							return false;
						}
					}

					return true;
				}

				return false;
			}
			break;
		default:
			break;
		}
	}

	return m_bHasWeapon;
}

// Fill arrays, each element represents a weapon id
// 1 indicates available, 0 indicates that it is not
// available...
void GetNoWeaponArray(short int* Array)
{
	std::memset(Array, 0, sizeof(short int) * MAX_WEAPONS);
}

void GetArrayOfExplosives(short int* Array)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		switch (i)
		{
		case VALVE_WEAPON_HANDGRENADE:
		case VALVE_WEAPON_RPG:
		case VALVE_WEAPON_TRIPMINE:
		case VALVE_WEAPON_SATCHEL:
			Array[i] = 1;
		}
	}
}

//
// Get the best weapon Id number for a bot facing an enemy pEnemy.
//
int CBotWeapons::GetBestWeaponId(CBot* pBot, edict_t* pEnemy)
{
	int i;

	std::priority_queue<CBotWeapon*, std::vector<CBotWeapon*>, CompareBotWeapon> Weapons;
	std::priority_queue<CBotWeapon*, std::vector<CBotWeapon*>, CompareBotWeapon> otherWeapons;
	//	dataQueue<int> Weapons;
	//	dataQueue<int> otherWeapons;

	CBotWeapon* pWeapon;
	//CWeapon *pWeaponInfo;

	edict_t* pEdict = pBot->m_pEdict;

	Vector vEnemyOrigin;
	float fEnemyDist;

	//TODO: This prevents bots from zapping themselves underwater with Lightning Gun in DMC? [APG]RoboCop[CL]
	BOOL bEnemyIsElectrified = false;
	BOOL bEnemyTooHigh = false;
	const BOOL bUnderwater = pEdict->v.waterlevel == 3;
	const BOOL bIsDMC = gBotGlobals.m_iCurrentMod == MOD_DMC;

	const BOOL bIsBattleGrounds = gBotGlobals.m_iCurrentMod == MOD_BG;
	BOOL bWantToMelee = false;

	short int iAllowedWeapons[MAX_WEAPONS];

	for (i = 0; i < MAX_WEAPONS; i++)
		iAllowedWeapons[i] = 1;

	if (pEnemy)
	{
		vEnemyOrigin = EntityOrigin(pEnemy);
		fEnemyDist = pBot->DistanceFrom(vEnemyOrigin);
		bEnemyTooHigh = vEnemyOrigin.z > pBot->pev->origin.z + MAX_JUMP_HEIGHT;
	}
	else
	{
		vEnemyOrigin = pBot->pev->origin;
		fEnemyDist = 0;
	}

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		if (pBot->IsAlien())
		{
			if (pEnemy)
			{
				if (pEnemy->v.iuser3 == AVH_USER3_BREAKABLE || EntityIsMarineStruct(pEnemy))
				{
					// cant leap structures etc...
					iAllowedWeapons[NS_WEAPON_UMBRA] = 0;
					iAllowedWeapons[NS_WEAPON_SPORES] = 0;
					iAllowedWeapons[NS_WEAPON_WEBSPINNER] = 0;
					iAllowedWeapons[NS_WEAPON_METABOLIZE] = 0;
					iAllowedWeapons[NS_WEAPON_PARASITE] = 0;
					iAllowedWeapons[NS_WEAPON_BLINK] = 0;
					// Abilities
					iAllowedWeapons[NS_ABILITY_LEAP] = 0;
					iAllowedWeapons[NS_ABILITY_CHARGE] = 0;
					iAllowedWeapons[NS_WEAPON_PRIMALSCREAM] = 0;
					iAllowedWeapons[NS_WEAPON_HEALINGSPRAY] = 0;
					iAllowedWeapons[NS_WEAPON_STOMP] = 0;
					iAllowedWeapons[NS_WEAPON_DEVOUR] = 0;
				}
				else if (pEnemy->v.iuser3 == AVH_USER3_MARINE_PLAYER)
				{
					iAllowedWeapons[NS_WEAPON_BILEBOMB] = 0;

					// heavy armour
					if (pEnemy->v.iuser4 & MASK_UPGRADE_13)
					{
						iAllowedWeapons[NS_WEAPON_SPORES] = 0;
					}

					if (pEnemy->v.iuser4 & MASK_PLAYER_STUNNED)
						iAllowedWeapons[NS_WEAPON_STOMP] = 0;
				}
			}
		}
		else
			iAllowedWeapons[NS_WEAPON_MINE] = 0;

		break;
		/*case MOD_TFC:
			if (pEnemy && pEnemy->v.flags & FL_MONSTER)
			{
				iAllowedWeapons[TF_WEAPON_MEDIKIT] = 0;
			}
			if (pBot->hasFlag())
			{
				iAllowedWeapons[TF_WEAPON_KNIFE] = 0;
				iAllowedWeapons[TF_WEAPON_AXE] = 0;
				iAllowedWeapons[TF_WEAPON_SPANNER] = 0;
			}
			break;*/
	case MOD_BG:
		if (pBot->m_pCurrentWeapon != nullptr)
		{
			// want to melee true if needing to reload OR enemy within melee range
			// AND random factor due to skill
			const BOOL bMeleeRangeCheck = pEnemy && fEnemyDist < 80.0f;
			const BOOL bMaxRangeCheck = pEnemy && fEnemyDist < 512.0f;

			bWantToMelee = (pBot->m_pCurrentWeapon->NeedToReload() && RANDOM_LONG(MIN_BOT_SKILL, MAX_BOT_SKILL) < pBot->
				m_Profile.m_iSkill || bMeleeRangeCheck) && bMaxRangeCheck;
		}

		if (pEnemy != nullptr)
		{
			// melee breakables.
			if (FStrEq(STRING(pEnemy->v.classname), "func_breakable"))
				bWantToMelee = true;
		}
		break;
	}

	if (pEnemy)
	{
		switch (gBotGlobals.m_iCurrentMod)
		{
			/*case MOD_SVENCOOP:
				if (FStrEq("func_breakable", STRING(pEnemy->v.classname)))
				{
					if (pEnemy->v.spawnflags & 512)
					{
						GetNoWeaponArray(iAllowedWeapons);
						GetArrayOfExplosives(iAllowedWeapons);//bExplosives = pEnemy->v.spawnflags & 512;

						if (pBot->HasWeapon(VALVE_WEAPON_MP5))
						{
							CBotWeapon* pWeapon = pBot->m_Weapons.GetWeapon(VALVE_WEAPON_MP5);

							if (pWeapon->SecondaryAmmo() > 0)
								iAllowedWeapons[VALVE_WEAPON_MP5] = 1;
						}
					}
				}
				else if (FStrEq("monster_gargantua", STRING(pEnemy->v.classname)))
				{
					// only use explosives & egon on garg
					GetNoWeaponArray(iAllowedWeapons);
					GetArrayOfExplosives(iAllowedWeapons);
					iAllowedWeapons[VALVE_WEAPON_EGON] = 1;

					if (pBot->HasWeapon(VALVE_WEAPON_MP5))
					{
						CBotWeapon* pWeapon = pBot->m_Weapons.GetWeapon(VALVE_WEAPON_MP5);

						if (pWeapon->SecondaryAmmo() > 0)
							iAllowedWeapons[VALVE_WEAPON_MP5] = 1;
					}
				}
				break;*/
		case MOD_GEARBOX: //w00tguy
			if (FStrEq("func_breakable", STRING(pEnemy->v.classname)))
			{
				iAllowedWeapons[GEARBOX_WEAPON_GRAPPLE] = 0; // grapple can't damage breakables

				if (pEnemy->v.spawnflags & 512)
				{
					GetNoWeaponArray(iAllowedWeapons);
					GetArrayOfExplosives(iAllowedWeapons);//bExplosives = pEnemy->v.spawnflags & 512;

					if (pBot->HasWeapon(GEARBOX_WEAPON_MP5))
					{
						const CBotWeapon* weapon = pBot->m_Weapons.GetWeapon(GEARBOX_WEAPON_MP5);

						if (weapon->SecondaryAmmo() > 0 && weapon->SecondaryInRange(fEnemyDist))
							iAllowedWeapons[GEARBOX_WEAPON_MP5] = 1;
					}
				}
				if (pEnemy->v.spawnflags & 256)
				{
					const bool hasInstantBreakWep = pBot->HasWeapon(GEARBOX_WEAPON_PIPEWRENCH);

					if (hasInstantBreakWep || pEnemy->v.health > 1000) {
						GetNoWeaponArray(iAllowedWeapons);
						iAllowedWeapons[GEARBOX_WEAPON_PIPEWRENCH] = 1;
					}
				}
			}
			break;
		case MOD_NS:
			if (pBot->IsMarine())
			{
				switch (pEnemy->v.iuser3)
				{
					// Use knife or welder(better) for things you dont want to waste ammo on it
				case AVH_USER3_DEFENSE_CHAMBER:
				case AVH_USER3_ALIENRESTOWER:
				case AVH_USER3_MOVEMENT_CHAMBER:
					if (fEnemyDist < REACHABLE_RANGE)
					{
						if (pBot->HasWeapon(NS_WEAPON_WELDER))
							return NS_WEAPON_WELDER;

						return NS_WEAPON_KNIFE;
					}
					break;
				default:
					break;
				}
			}
			else
			{
				// probably electrified
				bEnemyIsElectrified = pEnemy->v.iuser4 & MASK_UPGRADE_11;
			}
			break;
		default:
			break;
		}
	}

	for (i = 1; i < 30; i++)
	{
		pWeapon = &m_Weapons[i];

		if (iAllowedWeapons[i] == 0)
			continue;
		/*
		if ( bExplosives )
		{
			switch ( gBotGlobals.m_iCurrentMod )
				// ignore weapons that dont do explosive damage to enemies
			{
			case MOD_SVENCOOP:
				if ( ( i != VALVE_WEAPON_HANDGRENADE ) && (i != VALVE_WEAPON_RPG) )
					continue;
			default:
				break;
			}
		}*/

		// use the weapon code to see if we have the weapon in DMC
		if (!bIsDMC && !pBot->HasWeapon(i) || !pWeapon->HasWeapon(pBot->m_pEdict))
			continue;

		if (gBotGlobals.IsNS())
		{
			if (pWeapon->GetID() == NS_WEAPON_STOMP)
			{
				if (pEnemy)
				{
					if (pEnemy->v.velocity.Length() < 1)
						continue;
				}
			}
			else if (pWeapon->GetID() == NS_WEAPON_PARASITE)
			{
				if (pEnemy)
				{
					// already parasited
					if (pEnemy->v.iuser4 & MASK_PARASITED)
						continue;
				}
			}
		}

		if (bUnderwater)
		{
			if (!pWeapon->CanBeUsedUnderWater())
				continue;
		}

		if (pWeapon->IsMelee())
		{
			if (bEnemyTooHigh)
			{
				// too high to hit...
				continue;
			}
			else if (pEnemy && bEnemyIsElectrified)
			{
				std::vector<ga_value> inputs;

				int iweap = 0;

				if (pBot->m_pCurrentWeapon)
					iweap = pBot->m_pCurrentWeapon->GetID();

				inputs.emplace_back(pBot->pev->health / pBot->pev->max_health);
				inputs.emplace_back(pEnemy->v.health / pEnemy->v.max_health);
				inputs.emplace_back(pBot->pev->size.Length() / pEnemy->v.size.Length());
				inputs.emplace_back(pBot->NS_AmountOfEnergy() / 100);
				inputs.emplace_back(iweap / MAX_WEAPONS);
				inputs.emplace_back(pBot->onGround());
				inputs.emplace_back(pBot->pev->velocity.Length() / pBot->pev->maxspeed);

				pBot->dec_attackElectrified->input(&inputs);
				pBot->dec_attackElectrified->execute();

				pBot->m_pElectricEnemy = pEnemy;

				inputs.clear();

				if (pBot->dec_attackElectrified->trained() && !pBot->dec_attackElectrified->fired())//pBot->IsSkulk() )
				{
					// will be electrified if I try to attack...
					continue;
				}
			}

			otherWeapons.push(pWeapon);
			Weapons.push(pWeapon);//.Add(i);
			continue;
		}

		if (!pWeapon->CanShootPrimary(pEdict, fEnemyDist, pBot->m_fDistanceFromWall))
		{
			//if ( !pWeapon->CanShootSecondary() )
			continue;
		}

		otherWeapons.push(pWeapon);//.Add(i);

		if (bIsBattleGrounds)
		{
			// dont want to do other stuff except melee
			if (bWantToMelee)
				continue;
		}

		if (pWeapon->NeedToReload())//TODO: to allow bots to reload in TS [APG]RoboCop[CL]
		{
			Weapons.emplace(pWeapon);//.Add(i);
			continue;
		}

		if (pWeapon->OutOfAmmo())//TODO: to allow bots to reload in TS [APG]RoboCop[CL]
			continue;

		Weapons.emplace(pWeapon);//.Add(i);
	}

	if (Weapons.empty())//.IsEmpty() )
	{
		if (bIsBattleGrounds && bWantToMelee)
		{
			//Weapons._delete();
			//Weapons = otherWeapons;

			if (otherWeapons.empty())//.IsEmpty() )
			{
				if (gBotGlobals.IsMod(MOD_TS))
					return 36;
				return 0;
			}
			else
			{
				pWeapon = otherWeapons.top();

				return pWeapon->GetID();
			}
		}
		else
		{
			pBot->SetWeaponsNeeded(iAllowedWeapons);
			pBot->UpdateCondition(BOT_CONDITION_NEED_WEAPONS);
			//otherWeapons._delete();

			if (!otherWeapons.empty())
			{
				pWeapon = otherWeapons.top();

				return pWeapon->GetID();
			}

			if (gBotGlobals.IsMod(MOD_TS))
				return 36;
			return 0;
		}
	}

	pWeapon = Weapons.top();

	return pWeapon->GetID();

	/*

	//dataStack<int> WeaponChoice;
	dataQueue<int> tempQueue = Weapons;

	int iHighestMeleePriority = -1;
	int iHighestNonMeleePriority = -1;

	int iPriority;

	int iBestMeleeWeaponId = 0;
	int iBestNonMeleeWeaponId = 0;

	while ( !tempQueue.IsEmpty() )
	{
		i = tempQueue.ChooseFrom();

		pWeaponInfo = gBotGlobals.m_Weapons.GetWeapon(i);

		if ( pWeaponInfo )
		{
			iPriority = pWeaponInfo->GetPriority();

			if ( pWeaponInfo->IsMelee() )
			{
				if ( iPriority > iHighestMeleePriority )
				{
					iHighestMeleePriority = iPriority;
					iBestMeleeWeaponId = i;
				}
			}
			else
			{
				if ( iPriority > iHighestNonMeleePriority )
				{
					iHighestNonMeleePriority = iPriority;
					iBestNonMeleeWeaponId = i;
				}
			}
		}
	}

	Weapons._delete();

//	if ( !otherWeapons.IsEmpty() )
//		otherWeapons._delete();

	if ( iBestNonMeleeWeaponId != 0 )
	{
		// see if melee has higher priority than this
		if ( (!gBotGlobals.IsMod(MOD_TFC)||(pBot->pev->playerclass==TFC_CLASS_SPY)) && iBestMeleeWeaponId != 0 )
		{
			CWeapon *pMeleeWeapon = gBotGlobals.m_Weapons.GetWeapon(iBestMeleeWeaponId);
			CWeapon *pNonMeleeWeapon = gBotGlobals.m_Weapons.GetWeapon(iBestNonMeleeWeaponId);

			if ( pMeleeWeapon && pNonMeleeWeapon )
			{
				if ( pMeleeWeapon->PrimaryInRange(fEnemyDist) && (pMeleeWeapon->GetPriority() > pNonMeleeWeapon->GetPriority()) )
					return iBestMeleeWeaponId;
			}
		}

		return iBestNonMeleeWeaponId;
	}

	if ( gBotGlobals.IsMod(MOD_TS) )
		return 36;

	return iBestMeleeWeaponId;*/
}

BOOL CBotWeapon::NeedToReload() const
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_TS: //TODO: Add BOT_TASK_RELOAD to allow bots to reload sooner when attacking and running on a empty clip
		return !m_iClip && m_iReserve > 0;
	case MOD_BUMPERCARS:
	case MOD_DMC:
		return false;
	case MOD_GEARBOX:
		return !m_iClip && m_iReserve > 0;
	default:
		break;
	}

	if (m_iAmmo1)
	{
		return !m_iClip && *m_iAmmo1 > 0;
	}

	return false;
}

BOOL CBotWeapon::CanShootPrimary(edict_t* pEdict, float flFireDist, float flWallDist) const
{
	if (m_pWeaponInfo == nullptr)
		return true;

	if (gBotGlobals.m_iCurrentMod == MOD_DMC)
		return this->PrimaryAmmo() > 0;

	if (this->OutOfAmmo())//TODO: to allow bots to reload in TS [APG]RoboCop[CL]
		return false;
	if (pEdict->v.waterlevel == 3 && CanBeUsedUnderWater() == false)
		return false;

	if (!m_pWeaponInfo->CanUsePrimary())
		return false;

	if (gBotGlobals.IsMod(MOD_GEARBOX))
	{
		if (GetID() == VALVE_WEAPON_RPG)
		{
			if (!m_pWeaponInfo->PrimaryInRange(flWallDist))
				return false;
		}
	}

	if (!m_pWeaponInfo->PrimaryInRange(flFireDist))
		return false;

	return true;
}

BOOL CBotWeapons::HasWeapon(edict_t* pEdict, char* szClassname) const
{
	const char* pClassname;

	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (HasWeapon(pEdict, i))
		{
			if ((pClassname = m_Weapons[i].GetClassname()) != nullptr)
			{
				if (FStrEq(pClassname, szClassname))
					return true;
			}
		}
	}

	return false;
}
