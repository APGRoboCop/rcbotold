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
//=========================================================
// Scheduling 
//=========================================================
#ifndef	SCHEDULE_H
#define	SCHEDULE_H

enum
{
	bits_COND_SEE_HATE = ( 1 << 1 ), // see something that you hate
	bits_COND_SEE_FEAR = ( 1 << 2 ), // see something that you are afraid of
	bits_COND_SEE_DISLIKE = ( 1 << 3 ), // see something that you dislike
	bits_COND_SEE_ENEMY = ( 1 << 4 ), // target entity is in full view.
	bits_COND_LIGHT_DAMAGE = ( 1 << 8 ), // hurt a little 
	bits_COND_HEAVY_DAMAGE = ( 1 << 9 ), // hurt a lot
	bits_COND_SEE_CLIENT = ( 1 << 21 ), // see a client
	bits_COND_SEE_NEMESIS = ( 1 << 22 ) // see my nemesis
};


#endif	// SCHEDULE_H
