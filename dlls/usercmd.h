/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
#ifndef USERCMD_H
#define USERCMD_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

typedef struct usercmd_s
{
	// 4-byte types
	int		impact_index;
	vec3_t	viewangles;     // Command view angles.
	vec3_t	impact_position;
	float	forwardmove;    // Forward velocity.
	float	sidemove;       // Sideways velocity.
	float	upmove;         // Upward velocity.
	
	// 2-byte types
	short	lerp_msec;      // Interpolation time on client
	unsigned short  buttons;  // Attack buttons
	
	// 1-byte types
	byte	msec;           // Duration in ms of command
	byte	lightlevel;     // Light level at spot where we are standing.
	byte    impulse;          // Impulse command issued.
	byte	weaponselect;	// Current weapon id
} usercmd_t;

#endif // USERCMD_H