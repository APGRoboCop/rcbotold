//////////////////////////////////////////////////
// RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
//
// (http://www.rcbot.net)
//
// Based on botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_combat.cpp
//

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot.h"
#include "bot_weapons.h"

//extern bot_weapon_t weapon_defs[MAX_WEAPONS];



/*
   float f_distance;

   edict_t *pEdict = pBot->pEdict;

   // aim for the head and/or body
   Vector v_enemy = BotBodyTarget( pBot->pBotEnemy, pBot ) - GetGunPosition(pEdict);

   pEdict->v.v_angle = UTIL_VecToAngles( v_enemy );

   if (pEdict->v.v_angle.y > 180)
      pEdict->v.v_angle.y -=360;

   // Paulo-La-Frite - START bot aiming bug fix
   if (pEdict->v.v_angle.x > 180)
      pEdict->v.v_angle.x -=360;

   // set the body angles to point the gun correctly
   pEdict->v.angles.x = pEdict->v.v_angle.x / 3;
   pEdict->v.angles.y = pEdict->v.v_angle.y;
   pEdict->v.angles.z = 0;

   // adjust the view angle pitch to aim correctly (MUST be after body v.angles stuff)
   pEdict->v.v_angle.x = -pEdict->v.v_angle.x;
   // Paulo-La-Frite - END

   float x = pEdict->v.v_angle.y;
   if (x > 180) x -= 360;
   if (fabs(pEdict->v.ideal_yaw - x) > 2.0)
      fp = NULL;

   pEdict->v.ideal_yaw = UTIL_FixFloatAngle(pEdict->v.v_angle.y);

   
*/