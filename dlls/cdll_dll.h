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
//  cdll_dll.h

// this file is included by both the game-dll and the client-dll,

#ifndef CDLL_DLL_H
#define CDLL_DLL_H

//#ifdef RCBOT_TS_BUILD
constexpr int MAX_WEAPONS = 37;		// ??? 37 for TS;
//#else
//	#define MAX_WEAPONS	32
//#endif

enum : std::uint8_t
{
	MAX_WEAPON_SLOTS = 5, // hud item selection slots
	MAX_ITEM_TYPES = 6	// hud item selection slots
};

enum : std::uint8_t
{
	MAX_ITEMS = 5	// hard coded item types
};

enum : std::uint8_t
{
	HIDEHUD_WEAPONS = ( 1<<0 ),
	HIDEHUD_FLASHLIGHT = (1<<1 ),
	HIDEHUD_ALL = ( 1<<2 ),
	HIDEHUD_HEALTH = ( 1<<3 )
};

enum : std::uint8_t
{
	MAX_AMMO_TYPES = 32, // ???
	MAX_AMMO_SLOTS = 32		// not really slots
};

enum : std::uint8_t
{
	HUD_PRINTNOTIFY = 1,
	HUD_PRINTCONSOLE = 2,
	HUD_PRINTTALK = 3,
	HUD_PRINTCENTER = 4
};

enum : std::uint8_t
{
	WEAPON_SUIT = 31
};

#endif