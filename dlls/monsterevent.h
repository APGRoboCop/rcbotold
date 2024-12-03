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
#ifndef MONSTEREVENT_H
#define MONSTEREVENT_H

typedef struct
{
	int			event;
	char		*options;
} MonsterEvent_t;

enum : std::uint16_t
{
	EVENT_SPECIFIC = 0,
	EVENT_SCRIPTED = 1000,
	EVENT_SHARED = 2000,
	EVENT_CLIENT = 5000
};

enum : std::uint16_t
{
	MONSTER_EVENT_BODYDROP_LIGHT = 2001,
	MONSTER_EVENT_BODYDROP_HEAVY = 2002
};

enum : std::uint16_t
{
	MONSTER_EVENT_SWISHSOUND = 2010
};

#endif		// MONSTEREVENT_H
