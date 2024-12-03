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

#ifndef	ACTIVITY_H
#define	ACTIVITY_H


typedef enum : std::uint8_t
{
	ACT_RESET = 0,		// Set m_Activity to this invalid value to force a reset to m_IdealActivity
	ACT_IDLE = 1,
	ACT_GUARD = 2,
	ACT_WALK = 3,
	ACT_RUN = 4,
	ACT_FLY = 5, // Fly (and flap if appropriate)
	ACT_SWIM = 6,
	ACT_HOP = 7, // vertical jump
	ACT_LEAP = 8, // long forward jump
	ACT_FALL = 9,
	ACT_LAND = 10,
	ACT_STRAFE_LEFT = 11,
	ACT_STRAFE_RIGHT = 12,
	ACT_ROLL_LEFT = 13, // tuck and roll, left
	ACT_ROLL_RIGHT = 14, // tuck and roll, right
	ACT_TURN_LEFT = 15, // turn quickly left (stationary)
	ACT_TURN_RIGHT = 16, // turn quickly right (stationary)
	ACT_CROUCH = 17, // the act of crouching down from a standing position
	ACT_CROUCHIDLE = 18, // holding body in crouched position (loops)
	ACT_STAND = 19, // the act of standing from a crouched position
	ACT_USE = 20,
	ACT_SIGNAL1 = 21,
	ACT_SIGNAL2 = 22,
	ACT_SIGNAL3 = 23,
	ACT_TWITCH = 24,
	ACT_COWER = 25,
	ACT_SMALL_FLINCH = 26,
	ACT_BIG_FLINCH = 27,
	ACT_RANGE_ATTACK1 = 28,
	ACT_RANGE_ATTACK2 = 29,
	ACT_MELEE_ATTACK1 = 30,
	ACT_MELEE_ATTACK2 = 31,
	ACT_RELOAD = 32,
	ACT_ARM = 33, // pull out gun, for instance
	ACT_DISARM = 34, // reholster gun
	ACT_EAT = 35, // monster chowing on a large food item (loop)
	ACT_DIESIMPLE = 36,
	ACT_DIEBACKWARD = 37,
	ACT_DIEFORWARD = 38,
	ACT_DIEVIOLENT = 39,
	ACT_BARNACLE_HIT = 40, // barnacle tongue hits a monster
	ACT_BARNACLE_PULL = 41, // barnacle is lifting the monster ( loop )
	ACT_BARNACLE_CHOMP = 42, // barnacle latches on to the monster
	ACT_BARNACLE_CHEW = 43, // barnacle is holding the monster in its mouth ( loop )
	ACT_SLEEP = 44,
	ACT_INSPECT_FLOOR = 45, // for active idles, look at something on or near the floor
	ACT_INSPECT_WALL = 46,
	// for active idles, look at something directly ahead of you ( doesn't HAVE to be a wall or on a wall )
	ACT_IDLE_ANGRY = 47, // alternate idle animation in which the monster is clearly agitated. (loop)
	ACT_WALK_HURT = 48, // limp  (loop)
	ACT_RUN_HURT = 49, // limp  (loop)
	ACT_HOVER = 50, // Idle while in flight
	ACT_GLIDE = 51, // Fly (don't flap)
	ACT_FLY_LEFT = 52, // Turn left in flight
	ACT_FLY_RIGHT = 53, // Turn right in flight
	ACT_DETECT_SCENT = 54, // this means the monster smells a scent carried by the air
	ACT_SNIFF = 55, // this is the act of actually sniffing an item in front of the monster
	ACT_BITE = 56, // some large monsters can eat small things in one bite. This plays one time, EAT loops.
	ACT_THREAT_DISPLAY = 57, // without attacking, monster demonstrates that it is angry. (Yell, stick out chest, etc )
	ACT_FEAR_DISPLAY = 58, // monster just saw something that it is afraid of
	ACT_EXCITED = 59, // for some reason, monster is excited. Sees something he really likes to eat, or whatever.
	ACT_SPECIAL_ATTACK1 = 60, // very monster specific special attacks.
	ACT_SPECIAL_ATTACK2 = 61,
	ACT_COMBAT_IDLE = 62, // agitated idle.
	ACT_WALK_SCARED = 63,
	ACT_RUN_SCARED = 64,
	ACT_VICTORY_DANCE = 65, // killed a player, do a victory dance.
	ACT_DIE_HEADSHOT = 66, // die, hit in head. 
	ACT_DIE_CHESTSHOT = 67, // die, hit in chest
	ACT_DIE_GUTSHOT = 68, // die, hit in gut
	ACT_DIE_BACKSHOT = 69, // die, hit in back
	ACT_FLINCH_HEAD = 70,
	ACT_FLINCH_CHEST = 71,
	ACT_FLINCH_STOMACH = 72,
	ACT_FLINCH_LEFTARM = 73,
	ACT_FLINCH_RIGHTARM = 74,
	ACT_FLINCH_LEFTLEG = 75,
	ACT_FLINCH_RIGHTLEG = 76,
} Activity;


typedef struct {
	int	type;
	char *name;
} activity_map_t;

extern activity_map_t activity_map[];


#endif	//ACTIVITY_H
