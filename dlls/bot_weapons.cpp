// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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

#include <functional>
#include <unordered_map>

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

#include <string_view>
#include <array>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// begin -- TS metamod weapon restriction plugin

constexpr std::array<std::string_view, 36> pszTSWeaponModels = { "0","models/w_glock18.mdl","2 No gun","models/w_uzi.mdl"
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

void CWeapon::SetWeapon(const int iId, const char* szClassname, const int iPrimAmmoMax, const int iSecAmmoMax,
	const int iHudSlot, const int iHudPosition, const int iFlags, const int iAmmoIndex1, const int iAmmoIndex2)
{
	if (m_szClassname == nullptr)
		m_szClassname = gBotGlobals.m_Strings.GetString(szClassname);

	m_iId = iId;
	m_iPrimAmmoMax = iPrimAmmoMax;
	m_iSecAmmoMax = iSecAmmoMax;
	m_iHudSlot = iHudSlot;
	m_iHudPosition = iHudPosition;
	m_iFlags = iFlags;
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
	m_iClip = 0;
	m_iReserve = 0;
	m_iMaxClip = 0;
}

bool CBotWeapon::CanReload() const
{
	if (IsMelee())
		return false;

	if (m_iAmmo1)
	{
		return *m_iAmmo1 > 0;
	}

	return false;
}

bool CWeapon::IsPrimary() const
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		return HudSlot() == 0;
	default:
		return false;
	}
}

bool CWeapon::IsSecondary() const
{
	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		return HudSlot() == 1;
	default:
		return false;
	}
}

void CBotWeapon::SetWeapon(const int iId, int* iAmmoList)
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

void CBotWeapon::setHasWeapon(const bool bVal)
{
	m_bHasWeapon = bVal;
}

void CBotWeapons::AddWeapon(const int iId)
{
	m_Weapons[iId].SetWeapon(iId, m_iAmmo.data());
}

void CWeapons::AddWeapon(const int iId, const char* szClassname, const int iPrimAmmoMax, const int iSecAmmoMax,
	const int iHudSlot, const int iHudPosition, const int iFlags, const int iAmmoIndex1, const int iAmmoIndex2)
{
	if (iId >= 0 && iId < MAX_WEAPONS)
	{
		if (m_Weapons[iId] == nullptr)
		{
			if (weapon_preset_t* pPreset = gBotGlobals.m_WeaponPresets.GetPreset(gBotGlobals.m_iCurrentMod,
				iId); pPreset == nullptr)
			{
				m_Weapons[iId] = std::make_unique<CWeapon>();
			}
			else
			{
				m_Weapons[iId] = std::make_unique<CWeaponPreset>(pPreset);
			}

			m_Weapons[iId]->SetWeapon(iId, szClassname, iPrimAmmoMax, iSecAmmoMax, iHudSlot, iHudPosition, iFlags,
				iAmmoIndex1, iAmmoIndex2);
		}
	}
}

void CWeaponPresets::ReadPresets()
{
	char filename[512];
	UTIL_BuildFileName(filename, BOT_WEAPON_PRESETS_FILE);

	std::ifstream file(filename);
	if (!file.is_open()) {
		return;
	}

	std::string line;
	weapon_preset_t sWeaponPreset{};
	int iModId = 0;
	bool bSkipMod = false;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') {
			continue;
		}

		// Trim whitespace and carriage returns
		line.erase(line.find_last_not_of(" \n\r\t") + 1);

		if (line.empty()) {
			continue;
		}

		try
		{
			if (line.rfind("[mod_id=", 0) == 0) {
				iModId = std::stoi(line.substr(8, line.length() - 9));
				bSkipMod = (iModId != gBotGlobals.m_iCurrentMod);
			}
			else if (bSkipMod) {
				continue;
			}
			else if (line.rfind("[weapon_id=", 0) == 0) {
				sWeaponPreset = {}; // Reset for new weapon
				sWeaponPreset.m_iModId = iModId;
				sWeaponPreset.m_iId = std::stoi(line.substr(11, line.length() - 12));
			}
			else if (line == "[/weapon]") {
				m_Presets.Push(sWeaponPreset);
			}
			else {
				const size_t equals_pos = line.find('=');
				if (equals_pos != std::string::npos) {
					const std::string_view key = std::string_view(line).substr(0, equals_pos);
					const std::string_view valueStr = std::string_view(line).substr(equals_pos + 1);
					const int value = std::stoi(std::string(valueStr));

					static const std::unordered_map<std::string_view, std::function<void(weapon_preset_t&, int)>> keyHandlers = {
						{"underwater", [](weapon_preset_t& p, int v) { p.m_bCanFireUnderWater = v; }},
						{"primaryfire", [](weapon_preset_t& p, int v) { p.m_bHasPrimaryFire = v; }},
						{"secondaryfire", [](weapon_preset_t& p, int v) { p.m_bHasSecondaryFire = v; }},
						{"primary_min_range", [](weapon_preset_t& p, int v) { p.m_fPrimMinRange = static_cast<float>(v); }},
						{"primary_max_range", [](weapon_preset_t& p, int v) { p.m_fPrimMaxRange = static_cast<float>(v); }},
						{"secondary_min_range", [](weapon_preset_t& p, int v) { p.m_fSecMinRange = static_cast<float>(v); }},
						{"secondary_max_range", [](weapon_preset_t& p, int v) { p.m_fSecMaxRange = static_cast<float>(v); }},
						{"is_melee", [](weapon_preset_t& p, int v) { p.m_bIsMelee = v; }},
						{"priority", [](weapon_preset_t& p, int v) { p.m_iPriority = v; }},
						{"maxclip", [](weapon_preset_t& p, int v) { p.m_iMaxClip = v; }}
					};

					if (auto it = keyHandlers.find(key); it != keyHandlers.end()) {
						it->second(sWeaponPreset, value);
					}
				}
			}
		}

		catch (const std::invalid_argument& ia)
		{
			// Log the error for invalid number format - [APG]RoboCop[CL]
			BotMessage(nullptr, 0, "Invalid argument in weapon preset file. Line: '%s' (%s)", line.c_str(), ia.what());
		}
		catch (const std::out_of_range& oor)
		{
			// Log the error for out-of-range number - [APG]RoboCop[CL]
			BotMessage(nullptr, 0, "Out of range value in weapon preset file. Line: '%s' (%s)", line.c_str(), oor.what());
		}
	}
}

bool CBotWeapon::HasWeapon(const edict_t* pEdict) const
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
void GetNoWeaponArray(short* Array)
{
	std::fill_n(Array, MAX_WEAPONS, 0);
}

void GetArrayOfExplosives(short* Array)
{
	std::fill_n(Array, MAX_WEAPONS, 0);
	Array[VALVE_WEAPON_HANDGRENADE] = 1;
	Array[VALVE_WEAPON_RPG] = 1;
	Array[VALVE_WEAPON_TRIPMINE] = 1;
	Array[VALVE_WEAPON_SATCHEL] = 1;
}

void GetAllowedWeapons(CBot* pBot, edict_t* pEnemy, const float fEnemyDist, short* iAllowedWeapons)
{
	if (!pEnemy)
		return;

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_NS:
		if (pBot->IsAlien())
		{
			if (pEnemy->v.iuser3 == AVH_USER3_BREAKABLE || EntityIsMarineStruct(pEnemy))
			{
				iAllowedWeapons[static_cast<int>(NSWeapon::UMBRA)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::SPORES)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::WEBSPINNER)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::METABOLIZE)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::PARASITE)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::BLINK)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::LEAP)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::CHARGE)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::PRIMALSCREAM)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::HEALINGSPRAY)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::STOMP)] = 0;
				iAllowedWeapons[static_cast<int>(NSWeapon::DEVOUR)] = 0;
			}
			else if (pEnemy->v.iuser3 == AVH_USER3_MARINE_PLAYER)
			{
				iAllowedWeapons[static_cast<int>(NSWeapon::BILEBOMB)] = 0;
				if (pEnemy->v.iuser4 & MASK_UPGRADE_13)
					iAllowedWeapons[static_cast<int>(NSWeapon::SPORES)] = 0;
				if (pEnemy->v.iuser4 & MASK_PLAYER_STUNNED)
					iAllowedWeapons[static_cast<int>(NSWeapon::STOMP)] = 0;
			}
		}
		else // Marine
		{
			iAllowedWeapons[static_cast<int>(NSWeapon::MINE)] = 0;
		}
		break;

	case MOD_GEARBOX:
		if (FStrEq("func_breakable", STRING(pEnemy->v.classname)))
		{
			iAllowedWeapons[static_cast<int>(GearboxWeapon::GRAPPLE)] = 0;

			if (pEnemy->v.spawnflags & 512) // Explosive breakable
			{
				GetNoWeaponArray(iAllowedWeapons);
				GetArrayOfExplosives(iAllowedWeapons);
				if (pBot->HasWeapon(static_cast<int>(GearboxWeapon::MP5)))
				{
					const CBotWeapon* weapon = pBot->m_Weapons.GetWeapon(static_cast<int>(GearboxWeapon::MP5));
					if (weapon->SecondaryAmmo() > 0 && weapon->SecondaryInRange(fEnemyDist))
						iAllowedWeapons[static_cast<int>(GearboxWeapon::MP5)] = 1;
				}
			}
			else if (pEnemy->v.spawnflags & 256) // Melee breakable
			{
				const bool hasInstantBreakWep = pBot->HasWeapon(static_cast<int>(GearboxWeapon::PIPEWRENCH));
				if (hasInstantBreakWep || pEnemy->v.health > 1000) {
					GetNoWeaponArray(iAllowedWeapons);
					iAllowedWeapons[static_cast<int>(GearboxWeapon::PIPEWRENCH)] = 1;
				}
			}
		}
		else if (FStrEq("ammo_spore", STRING(pEnemy->v.classname)))
		{
			iAllowedWeapons[static_cast<int>(GearboxWeapon::GRAPPLE)] = 1;
		}
		break;
	}
}

//
// Get the best weapon Id number for a bot facing an enemy pEnemy.
//
int CBotWeapons::GetBestWeaponId(CBot* pBot, edict_t* pEnemy)
{
	std::vector<CBotWeapon*> usableWeapons;
	std::vector<CBotWeapon*> otherWeapons;

	usableWeapons.reserve(MAX_WEAPONS);
	otherWeapons.reserve(MAX_WEAPONS);

	const edict_t* pEdict = pBot->m_pEdict;

	const Vector vEnemyOrigin = pEnemy ? EntityOrigin(pEnemy) : pBot->pev->origin;
	const float fEnemyDist = pEnemy ? pBot->DistanceFrom(vEnemyOrigin) : 0.0f;
	const bool bEnemyTooHigh = pEnemy ? (vEnemyOrigin.z > pBot->pev->origin.z + MAX_JUMP_HEIGHT) : false;

	const bool bUnderwater = pEdict->v.waterlevel == 3;
	const bool bIsDMC = gBotGlobals.m_iCurrentMod == MOD_DMC;
	const bool bIsBattleGrounds = gBotGlobals.m_iCurrentMod == MOD_BG;
	bool bWantToMelee = false;

	short iAllowedWeapons[MAX_WEAPONS];
	std::fill_n(iAllowedWeapons, MAX_WEAPONS, 1);

	GetAllowedWeapons(pBot, pEnemy, fEnemyDist, iAllowedWeapons);

	if (bIsBattleGrounds)
	{
		if (pBot->m_pCurrentWeapon != nullptr)
		{
			const bool bMeleeRangeCheck = pEnemy && fEnemyDist < 80.0f;
			const bool bMaxRangeCheck = pEnemy && fEnemyDist < 512.0f;

			bWantToMelee = (pBot->m_pCurrentWeapon->NeedToReload() && RANDOM_LONG(MIN_BOT_SKILL, MAX_BOT_SKILL) < pBot->m_Profile.m_iSkill || bMeleeRangeCheck) && bMaxRangeCheck;
		}

		if (pEnemy != nullptr && FStrEq(STRING(pEnemy->v.classname), "func_breakable"))
		{
			bWantToMelee = true;
		}
	}

	bool bEnemyIsElectrified = false;
	if (pEnemy && gBotGlobals.m_iCurrentMod == MOD_NS)
	{
		if (pBot->IsMarine())
		{
			switch (pEnemy->v.iuser3)
			{
			case AVH_USER3_DEFENSE_CHAMBER:
			case AVH_USER3_ALIENRESTOWER:
			case AVH_USER3_MOVEMENT_CHAMBER:
				if (fEnemyDist < REACHABLE_RANGE)
				{
					return static_cast<int>(NSWeapon::KNIFE);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			bEnemyIsElectrified = pEnemy->v.iuser4 & MASK_UPGRADE_11;
		}
	}

	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		CBotWeapon* pWeapon = &m_Weapons[i];

		if (iAllowedWeapons[i] == 0)
			continue;

		if ((!bIsDMC && !pBot->HasWeapon(i)) || !pWeapon->HasWeapon(pBot->m_pEdict))
			continue;

		if (gBotGlobals.IsNS())
		{
			if (pWeapon->GetID() == static_cast<int>(NSWeapon::STOMP) && pEnemy && pEnemy->v.velocity.Length() < 1)
			{
				continue;
			}
			if (pWeapon->GetID() == static_cast<int>(NSWeapon::PARASITE) && pEnemy && (pEnemy->v.iuser4 & MASK_PARASITED))
			{
				continue;
			}
		}

		if (bUnderwater && !pWeapon->CanBeUsedUnderWater())
		{
			continue;
		}

		if (pWeapon->IsMelee())
		{
			if (bEnemyTooHigh) continue;

			if (pEnemy && bEnemyIsElectrified)
			{
				std::vector<ga_value> inputs;
				inputs.emplace_back(pBot->pev->health / pBot->pev->max_health);
				inputs.emplace_back(pEnemy->v.health / pEnemy->v.max_health);
				inputs.emplace_back(pBot->pev->size.Length() / pEnemy->v.size.Length());
				inputs.emplace_back(pBot->NS_AmountOfEnergy() / 100);
				inputs.emplace_back((pBot->m_pCurrentWeapon ? pBot->m_pCurrentWeapon->GetID() : 0) / static_cast<float>(MAX_WEAPONS));
				inputs.emplace_back(pBot->onGround());
				inputs.emplace_back(pBot->pev->velocity.Length() / pBot->pev->maxspeed);

				pBot->dec_attackElectrified->input(inputs);
				pBot->dec_attackElectrified->execute();
				pBot->m_pElectricEnemy = pEnemy;

				if (pBot->dec_attackElectrified->trained() && !pBot->dec_attackElectrified->fired())
				{
					continue;
				}
			}

			otherWeapons.push_back(pWeapon);

			// Only consider melee as a primary choice at close range for TS [APG]RoboCop[CL]
			if (gBotGlobals.IsMod(MOD_TS) && pEnemy && fEnemyDist > 40.0f)
				continue;

			usableWeapons.push_back(pWeapon);
			continue;
		}

		if (!pWeapon->CanShootPrimary(pEdict, fEnemyDist, pBot->m_fDistanceFromWall))
		{
			// Gun can't fire right now but may just need a reload [APG]RoboCop[CL]
			if (pWeapon->NeedToReload())
			{
				otherWeapons.push_back(pWeapon);
				usableWeapons.emplace_back(pWeapon);
			}
			continue;
		}

		otherWeapons.push_back(pWeapon);

		if (bIsBattleGrounds && bWantToMelee)
		{
			continue;
		}

		if (pWeapon->OutOfAmmo())
		{
			continue;
		}

		if (pWeapon->NeedToReload())
		{
			usableWeapons.emplace_back(pWeapon); // Add to a list of weapons that could be reloaded
			continue; // But don't select it for immediate firing
		}

		usableWeapons.emplace_back(pWeapon); // Add usable weapon to the main selection list
	}

	if (!usableWeapons.empty())
	{
		const std::vector<CBotWeapon*>::iterator bestWeapon = std::max_element(
			usableWeapons.begin(), usableWeapons.end(), CompareBotWeapon());
		return (*bestWeapon)->GetID();
	}

	if (bIsBattleGrounds && bWantToMelee)
	{
		if (!otherWeapons.empty())
		{
			const std::vector<CBotWeapon*>::iterator bestWeapon = std::max_element(
				otherWeapons.begin(), otherWeapons.end(), CompareBotWeapon());
			return (*bestWeapon)->GetID();
		}
	}
	else
	{
		pBot->SetWeaponsNeeded(iAllowedWeapons);
		pBot->UpdateCondition(BOT_CONDITION_NEED_WEAPONS);

		if (!otherWeapons.empty())
		{
			const std::vector<CBotWeapon*>::iterator bestWeapon = std::max_element(
				otherWeapons.begin(), otherWeapons.end(), CompareBotWeapon());
			return (*bestWeapon)->GetID();
		}
	}

	return gBotGlobals.IsMod(MOD_TS) ? 36 : 0;
}

bool CBotWeapon::NeedToReload() const
{
	if (IsMelee() || m_iMaxClip == 0) // Melee weapons or weapons without a clip don't need to reload
		return false;

	switch (gBotGlobals.m_iCurrentMod)
	{
	case MOD_BUMPERCARS:
	case MOD_DMC:
		return false;
	case MOD_TS:
		// TS tracks reserve ammo per-weapon in m_iReserve [APG]RoboCop[CL]
		return m_iClip <= 0 && m_iReserve > 0;
	case MOD_HL_DM:
	case MOD_GEARBOX:
	default:
		if (m_iAmmo1)
		{
			return !m_iClip && *m_iAmmo1 > 0;
		}
		return false;
	}
}

bool CBotWeapon::CanShootPrimary(const edict_t* pEdict, const float flFireDist, const float flWallDist) const
{
	if (m_pWeaponInfo == nullptr)
		return true;

	if (gBotGlobals.m_iCurrentMod == MOD_DMC)
		return this->PrimaryAmmo() > 0;

	// Check if clip is empty or completely out of ammo - [APG]RoboCop[CL]
	if (this->OutOfAmmo() || this->ClipIsEmpty() ||
		(pEdict->v.waterlevel == 3 && !CanBeUsedUnderWater()) ||
		!m_pWeaponInfo->CanUsePrimary() ||
		!m_pWeaponInfo->PrimaryInRange(flFireDist))
	{
		return false;
	}

	if (gBotGlobals.IsMod(MOD_GEARBOX) && GetID() == VALVE_WEAPON_RPG)
	{
		if (!m_pWeaponInfo->PrimaryInRange(flWallDist))
			return false;
	}

	return true;
}

bool CBotWeapons::HasWeapon(edict_t* pEdict, const char* szClassname) const
{
	if (!szClassname) {
		return false;
	}
	// Use auto to deduce the iterator type - [APG]RoboCop[CL]
	const auto it = std::find_if(
		m_Weapons.begin() + 1, m_Weapons.end(),
		[&](const CBotWeapon& weapon)
		{
			if (!weapon.HasWeapon(pEdict))
			{
				return false;
			}

			const char* pClassname = weapon.GetClassname();
			return pClassname && std::string_view(pClassname) == szClassname;
		});

	return it != m_Weapons.end();
}