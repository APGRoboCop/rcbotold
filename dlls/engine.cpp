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
// engine.cpp
//
//////////////////////////////////////////////////
//
// engine functions from half-life
//

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "dllapi.h"
#include "meta_api.h"
#endif

#include "bot.h"
#include "bot_client.h"
#include "engine.h"

#include "waypoint.h"

#ifdef RCBOT_META_BUILD
extern globalvars_t  *gpGlobals;
#endif

extern enginefuncs_t g_engfuncs;
extern CBotGlobals gBotGlobals;
extern int debug_engine;
extern CWaypointLocations WaypointLocations;

static FILE *fp;
/*
void pfnAlertMessage( ALERT_TYPE atype, char *szFmt, ... )
{
	assert ( atype != at_error );

#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnAlertMessage)(atype,szFmt,...);
#endif
}
*/
int pfnPrecacheModel(char* s)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheModel: %s\n",s); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnPrecacheModel)(s);
#endif
}
int pfnPrecacheSound(char* s)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheSound: %s\n",s); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnPrecacheSound)(s);
#endif
}
void pfnSetModel(edict_t *e, const char *m)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetModel: edict=%x %s\n",e,m); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetModel)(e, m);
#endif
}
int pfnModelIndex(const char *m)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnModelIndex: %s\n",m); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnModelIndex)(m);
#endif
}
int pfnModelFrames(int modelIndex)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnModelFrames:\n"); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnModelFrames)(modelIndex);
#endif
}
void pfnSetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetSize: %x\n",e); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetSize)(e, rgflMin, rgflMax);
#endif
}
void pfnChangeLevel(char* s1, char* s2)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangeLevel:\n"); fclose(fp); }
    
    CBot *pBot;
    
    // kick any bot off of the server after time/frag limit...
    for (int index = 0; index < MAX_PLAYERS; index++)
    {
        pBot = &gBotGlobals.m_Bots[index];
        
        if (pBot->m_bIsUsed)  // is this slot used?
        {
            char cmd[40];
            
            sprintf(cmd, "kick \"%s\"\n", pBot->m_szBotName);
            
            pBot->m_iRespawnState = RESPAWN_NEED_TO_RESPAWN;
            
            SERVER_COMMAND(cmd);  // kick the bot using (kick "name")
        }
    }

#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnChangeLevel)(s1, s2);
#endif
}
void pfnGetSpawnParms(edict_t *ent)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetSpawnParms:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnGetSpawnParms)(ent);
#endif
}
void pfnSaveSpawnParms(edict_t *ent)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSaveSpawnParms:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnSaveSpawnParms)(ent);
#endif
}
float pfnVecToYaw(const float *rgflVector)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVecToYaw:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else   
    return (*g_engfuncs.pfnVecToYaw)(rgflVector);
#endif
}
void pfnVecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVecToAngles:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnVecToAngles)(rgflVectorIn, rgflVectorOut);
#endif
}
void pfnMoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMoveToOrigin:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnMoveToOrigin)(ent, pflGoal, dist, iMoveType);
#endif
}
void pfnChangeYaw(edict_t* ent)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangeYaw:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnChangeYaw)(ent);
#endif
}
void pfnChangePitch(edict_t* ent)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangePitch:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnChangePitch)(ent);
#endif
}
edict_t* pfnFindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityByString: %s\n",pszValue); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnFindEntityByString)(pEdictStartSearchAfter, pszField, pszValue);
#endif
}
int pfnGetEntityIllum(edict_t* pEnt)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetEntityIllum:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnGetEntityIllum)(pEnt);
#endif
}
edict_t* pfnFindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityInSphere:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnFindEntityInSphere)(pEdictStartSearchAfter, org, rad);
#endif
}
edict_t* pfnFindClientInPVS(edict_t *pEdict)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindClientInPVS:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnFindClientInPVS)(pEdict);
#endif
}
edict_t* pfnEntitiesInPVS(edict_t *pplayer)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntitiesInPVS:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnEntitiesInPVS)(pplayer);
#endif
}
void pfnMakeVectors(const float *rgflVector)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMakeVectors:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnMakeVectors)(rgflVector);
#endif
}
void pfnAngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAngleVectors:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnAngleVectors)(rgflVector, forward, right, up);
#endif
}
edict_t* pfnCreateEntity(void)
{
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
	edict_t *pent = (*g_engfuncs.pfnCreateEntity)();
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateEntity: %x\n",pent); fclose(fp); }
    return pent;
#endif
	
}
void pfnRemoveEntity(edict_t* e)
{


    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRemoveEntity: %x\n",e); fclose(fp); }
    if (debug_engine)
    {
        fp=fopen("bot.txt","a");
        fprintf(fp,"pfnRemoveEntity: %x\n",e);
        if (e->v.model != 0)
            fprintf(fp," model=%s\n", STRING(e->v.model));
        fclose(fp);
    }
    
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnRemoveEntity)(e);
#endif
}
edict_t* pfnCreateNamedEntity(int className)
{
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
	    
    edict_t *pent = (*g_engfuncs.pfnCreateNamedEntity)(className);

	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateNamedEntity: edict=%x name=%s\n",pent,STRING(className)); fclose(fp); }

	

    return pent;
#endif
}
void pfnMakeStatic(edict_t *ent)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMakeStatic:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnMakeStatic)(ent);
#endif
}
int pfnEntIsOnFloor(edict_t *e)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntIsOnFloor:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnEntIsOnFloor)(e);
#endif
}
int pfnDropToFloor(edict_t* e)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDropToFloor:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnDropToFloor)(e);
#endif
}
int pfnWalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWalkMove:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnWalkMove)(ent, yaw, dist, iMode);
#endif
}
void pfnSetOrigin(edict_t *e, const float *rgflOrigin)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetOrigin:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnSetOrigin)(e, rgflOrigin);
#endif
}
void pfnEmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
	if ( entity != NULL )
	{
		int i;
		CBot *pBot;

		Vector vOrigin;

		vOrigin = EntityOrigin(entity);
		
		eSoundType iSound = SOUND_UNKNOWN;
		
		if ( gBotGlobals.IsMod(MOD_SVENCOOP) )
		{
			if ( entity->v.flags & FL_CLIENT )
			{
				if ( (strncmp(sample,"speech/saveme",13) == 0) )
					iSound = SOUND_NEEDHEALTH;
				else if ( (strncmp(sample,"speech/grenade",14) == 0) )
					iSound = SOUND_TAKE_COVER;
			}
		}
		else if ( gBotGlobals.IsMod(MOD_TFC) )
		{
			if ( strncmp(sample,"speech/saveme",13) == 0)
				iSound = SOUND_NEEDHEALTH;
		}
		
		if ( iSound == SOUND_UNKNOWN )
		{
			// common sounds, like doors etc.
			if ( (sample[0] == 'd') && !strncmp(sample,"doors/",6) )
				iSound = SOUND_DOOR;
			else if ( (sample[0] == 'p') && !strncmp(sample,"plats/",6) )
				iSound = SOUND_DOOR;
			else if ( (sample[0] == 'w') && !strncmp(sample,"weapons/",8) )
				iSound = SOUND_WEAPON;
			else if ( (sample[0] == 'p') && !strncmp(sample,"player/",7) )
			{
				if ( strncmp(&sample[7],"pain",4) == 0 )
					iSound = SOUND_PLAYER_PAIN;
				else
					iSound = SOUND_PLAYER;
			}
			else if ( (sample[0] == 'b') && !strncmp(sample,"buttons/",8) )
				iSound = SOUND_BUTTON;
		}
		
		if ( iSound == SOUND_UNKNOWN )
		{
			switch ( gBotGlobals.m_iCurrentMod )
			{
			case MOD_NS:
				{
					//NS sounds like gorges / taunts/ radio etc
					int sample_num = 0;
					
					// Starts with "vox/" ?? (Do it manually...)
					if ( (sample[0] == 'v') && (sample[1] == 'o') &&
						(sample[2] == 'x') && (sample[3] == '/') )
					{
						if ( !strncmp(&sample[4],"ssay",4) ) // Marine Said Something
						{
							sample_num = atoi(&sample[8]);
							
							if ( (sample_num > 10) && (sample_num < 20) )			
								iSound = SOUND_FOLLOW;			
							else if ( sample_num < 30 )
								iSound = SOUND_COVERING;
							else if ( sample_num < 40 )
								iSound = SOUND_TAUNT;
							else if ( sample_num < 50 )							
								iSound = SOUND_NEEDHEALTH;							
							else if ( sample_num < 60 )
								iSound = SOUND_NEEDAMMO;
							else if ( sample_num < 70 )
								iSound = SOUND_INPOSITION;
							else if ( sample_num < 80 )
								iSound = SOUND_INCOMING;
							else if ( sample_num < 90 )
								iSound = SOUND_MOVEOUT;
							else if ( sample_num < 100 )
								iSound = SOUND_ALLCLEAR;			
						}
						else if ( !strncmp(&sample[4],"asay",4) ) // Alien Sound?
						{
							sample_num = atoi(&sample[8]);
							
							if ( sample_num < 20 )
								iSound = SOUND_UNKNOWN;
							
							if ( sample_num < 30 ) // healing sounds end in ..21, ..22.wav etc			
								iSound = SOUND_NEEDHEALTH;			
							else if ( sample_num < 40 )
								iSound = SOUND_NEEDBACKUP;
							else if ( sample_num < 50 )
								iSound = SOUND_INCOMING;
							else if ( sample_num < 60 )
								iSound = SOUND_ATTACK;
							else if ( sample_num < 70 )
								iSound = SOUND_BUILDINGHERE;
						}
					}
				}
				break;
			case MOD_BUMPERCARS:
				// bumpercars sounds, like horns, taunts etc
				break;
			default:
				break;
			}
		}		
		
		edict_t *pEntityOwner = entity->v.owner;

		for ( i = 0; i < 32; i ++ )
		{
			pBot = &gBotGlobals.m_Bots[i];
			
			if ( pBot == NULL )
				continue;
			if ( pBot->m_pEdict == NULL )
				continue;
			if ( pBot->m_pEdict == pEntityOwner )
				continue;
			if ( !pBot->m_bIsUsed )
				continue;
			if ( pBot->m_pEdict == entity )
				continue;
			if ( pBot->m_pEnemy != NULL )
				continue;

			if ( pBot->DistanceFrom(vOrigin) < BOT_HEAR_DISTANCE )
				pBot->HearSound(iSound,vOrigin,entity);
		}
		
	}

    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEmitSound:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);
#endif
}
void pfnEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEmitAmbientSound:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnEmitAmbientSound)(entity, pos, samp, vol, attenuation, fFlags, pitch);
#endif
}
void pfnTraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceLine:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnTraceLine)(v1, v2, fNoMonsters, pentToSkip, ptr);
#endif
}
void pfnTraceToss(edict_t* pent, edict_t* pentToIgnore, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceToss:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnTraceToss)(pent, pentToIgnore, ptr);
#endif
}
int pfnTraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceMonsterHull:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnTraceMonsterHull)(pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
#endif
}
void pfnTraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceHull:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnTraceHull)(v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
#endif
}
void pfnTraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceModel:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnTraceModel)(v1, v2, hullNumber, pent, ptr);
#endif
}
const char *pfnTraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 )
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceTexture:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnTraceTexture)(pTextureEntity, v1, v2);
#endif
}
void pfnTraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceSphere:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnTraceSphere)(v1, v2, fNoMonsters, radius, pentToSkip, ptr);
#endif
}
void pfnGetAimVector(edict_t* ent, float speed, float *rgflReturn)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetAimVector:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnGetAimVector)(ent, speed, rgflReturn);
#endif
}
void pfnServerCommand(char* str)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerCommand: %s\n",str); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnServerCommand)(str);
#endif
}
void pfnServerExecute(void)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerExecute:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnServerExecute)();
#endif
}
void pfnClientCommand(edict_t* pEdict, char* szFmt, ...)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnClientCommand=%s\n",szFmt); fclose(fp); }
#ifdef RCBOT_META_BUILD
   if (pEdict->v.flags & FL_FAKECLIENT)
      RETURN_META (MRES_SUPERCEDE);
   RETURN_META (MRES_IGNORED);
#else       
    return;
#endif
}
void pfnParticleEffect(const float *org, const float *dir, float color, float count)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnParticleEffect:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnParticleEffect)(org, dir, color, count);
#endif
}
void pfnLightStyle(int style, char* val)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnLightStyle:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnLightStyle)(style, val);
#endif
}
int pfnDecalIndex(const char *name)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDecalIndex:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnDecalIndex)(name);
#endif
}
int pfnPointContents(const float *rgflVector)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPointContents:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnPointContents)(rgflVector);
#endif
}

void pfnMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	BOOL no_error = gBotGlobals.NetMessageStarted(msg_dest,msg_type,pOrigin,ed);
	
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
	//if ( no_error )
		(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
#endif
}

void pfnMessageEnd(void)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMessageEnd:\n"); fclose(fp); }
        
        if ( gBotGlobals.m_CurrentMessage )
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"---------MESSAGE_END(\"%s\")-------\n",gBotGlobals.m_CurrentMessage->getMessageName());

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->messageEnd();
			else
				gBotGlobals.m_CurrentMessage->execute(NULL, gBotGlobals.m_iBotMsgIndex);  // NULL indicated msg end
		}
    }
    
    // clear out the bot message function pointers...

    gBotGlobals.m_CurrentMessage = NULL;
    gBotGlobals.m_iCurrentMessageState = 0;
    gBotGlobals.m_iCurrentMessageState2 = 0;

	gBotGlobals.m_bNetMessageStarted = FALSE;
    
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnMessageEnd)();
#endif
}
void pfnWriteByte(int iValue)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteByte: %d\n",iValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_BYTE(%d)\n",iValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeByte(iValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&iValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteByte)(iValue);
#endif
}
void pfnWriteChar(int iValue)
{
    if (gpGlobals->deathmatch)
    {
        //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteChar: %d\n",iValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{            
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_CHAR(%c)\n",(char)iValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeChar((char)iValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&iValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteChar)(iValue);
#endif
}
void pfnWriteShort(int iValue)
{
    if (gpGlobals->deathmatch)
    {

        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteShort: %d\n",iValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{            
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_SHORT(%d)\n",iValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeShort(iValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&iValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteShort)(iValue);
#endif
}
void pfnWriteLong(int iValue)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteLong: %d\n",iValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_LONG(%d)\n",iValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeLong(iValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&iValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteLong)(iValue);
#endif
}
void pfnWriteAngle(float flValue)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteAngle: %f\n",flValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_ANGLE(%0.3f)\n",flValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeAngle(flValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&flValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteAngle)(flValue);
#endif
}
void pfnWriteCoord(float flValue)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteCoord: %f\n",flValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_COORD(%0.3f)\n",flValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeCoord(flValue);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&flValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else   
    (*g_engfuncs.pfnWriteCoord)(flValue);
#endif
}
void pfnWriteString(const char *sz)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteString: %s\n",sz); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_STRING(%s)\n",sz);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeString(sz);
			else
				gBotGlobals.m_CurrentMessage->execute((void *)sz, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteString)(sz);
#endif
}
void pfnWriteEntity(int iValue)
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteEntity: %d\n",iValue); fclose(fp); }
        
        // if this message is for a bot, call the client message function...
        if (gBotGlobals.m_CurrentMessage)
		{
			if ( gBotGlobals.m_pDebugMessage == gBotGlobals.m_CurrentMessage )//gBotGlobals.IsDebugLevelOn(BOT_DEBUG_MESSAGE_LEVEL) )
				ALERT(at_console,"WRITE_ENTITY(%d)\n",iValue);

			if ( gBotGlobals.m_CurrentMessage->isStateMsg() )
				((CBotStatedNetMessage*)gBotGlobals.m_CurrentMessage)->writeEntity(INDEXENT(iValue));
			else
				gBotGlobals.m_CurrentMessage->execute((void *)&iValue, gBotGlobals.m_iBotMsgIndex);
		}
    }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnWriteEntity)(iValue);
#endif
}
void pfnCVarRegister(cvar_t *pCvar)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarRegister:\n"); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCVarRegister)(pCvar);
#endif
}
float pfnCVarGetFloat(const char *szVarName)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetFloat: %s\n",szVarName); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnCVarGetFloat)(szVarName);
#endif
}
const char* pfnCVarGetString(const char *szVarName)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetString:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnCVarGetString)(szVarName);
#endif
}
void pfnCVarSetFloat(const char *szVarName, float flValue)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarSetFloat:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCVarSetFloat)(szVarName, flValue);
#endif
}
void pfnCVarSetString(const char *szVarName, const char *szValue)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarSetString:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCVarSetString)(szVarName, szValue);
#endif
}
void* pfnPvAllocEntPrivateData(edict_t *pEdict, long cb)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPvAllocEntPrivateData:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnPvAllocEntPrivateData)(pEdict, cb);
#endif
}
void* pfnPvEntPrivateData(edict_t *pEdict)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPvEntPrivateData:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnPvEntPrivateData)(pEdict);
#endif
}
void pfnFreeEntPrivateData(edict_t *pEdict)
{
	BotMessage(NULL,0,"Free ent provate data:");

    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFreeEntPrivateData:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnFreeEntPrivateData)(pEdict);
#endif
}
const char* pfnSzFromIndex(int iString)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSzFromIndex:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnSzFromIndex)(iString);
#endif
}
int pfnAllocString(const char *szValue)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAllocString:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnAllocString)(szValue);
#endif
}
entvars_t* pfnGetVarsOfEnt(edict_t *pEdict)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetVarsOfEnt:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnGetVarsOfEnt)(pEdict);
#endif
}
edict_t* pfnPEntityOfEntOffset(int iEntOffset)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPEntityOfEntOffset:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnPEntityOfEntOffset)(iEntOffset);
#endif
}
int pfnEntOffsetOfPEntity(const edict_t *pEdict)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntOffsetOfPEntity: %x\n",pEdict); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnEntOffsetOfPEntity)(pEdict);
#endif
}
int pfnIndexOfEdict(const edict_t *pEdict)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIndexOfEdict: %x\n",pEdict); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
#endif
}
edict_t* pfnPEntityOfEntIndex(int iEntIndex)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPEntityOfEntIndex:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnPEntityOfEntIndex)(iEntIndex);
#endif
}
edict_t* pfnFindEntityByVars(entvars_t* pvars)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityByVars:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnFindEntityByVars)(pvars);
#endif
}
void* pfnGetModelPtr(edict_t* pEdict)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetModelPtr: %x\n",pEdict); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnGetModelPtr)(pEdict);
#endif
}

int pfnRegUserMsg(const char *pszName, int iSize)
{
	int msg = 0;

#ifdef RCBOT_META_BUILD

	extern plugin_info_t Plugin_info;

	//msg = GET_USER_MSG_ID(&Plugin_info, pszName, &iSize);
#else
    
    msg = (*g_engfuncs.pfnRegUserMsg)(pszName, iSize);
    
    if (gpGlobals->deathmatch)
    {
#ifdef _DEBUG
        fp=fopen("bot.txt","a"); fprintf(fp,"pfnRegUserMsg: pszName=%s msg=%d\n",pszName,msg); fclose(fp);
#endif
                
    }
#endif

	gBotGlobals.m_NetEntityMessages.UpdateMessage(pszName,msg,iSize);
	gBotGlobals.m_NetAllMessages.UpdateMessage(pszName,msg,iSize);

#ifdef RCBOT_META_BUILD
		RETURN_META_VALUE(MRES_IGNORED,0);
#else
		return msg;
#endif

}

void pfnAnimationAutomove(const edict_t* pEdict, float flTime)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAnimationAutomove:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnAnimationAutomove)(pEdict, flTime);
#endif
}

void pfnGetBonePosition(const edict_t* pEdict, int iBone, float *rgflOrigin, float *rgflAngles )
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetBonePosition:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnGetBonePosition)(pEdict, iBone, rgflOrigin, rgflAngles);
#endif
}

unsigned long pfnFunctionFromName( const char *pName )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFunctionFromName:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnFunctionFromName)(pName);
#endif
}

const char *pfnNameForFunction( unsigned long function )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnNameForFunction:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnNameForFunction)(function);
#endif
}

void pfnClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnClientPrintf:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnClientPrintf)(pEdict, ptype, szMsg);
#endif
}

void pfnServerPrint( const char *szMsg )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerPrint: %s\n",szMsg); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnServerPrint)(szMsg);
#endif
}
void pfnGetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles )
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetAttachment:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnGetAttachment)(pEdict, iAttachment, rgflOrigin, rgflAngles);
#endif
}
void pfnCRC32_Init(CRC32_t *pulCRC)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_Init:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCRC32_Init)(pulCRC);
#endif
}
void pfnCRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_ProcessBuffer:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCRC32_ProcessBuffer)(pulCRC, p, len);
#endif
}
void pfnCRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_ProcessByte:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCRC32_ProcessByte)(pulCRC, ch);
#endif
}
CRC32_t pfnCRC32_Final(CRC32_t pulCRC)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_Final:\n"); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,pulCRC);
#else
    return (*g_engfuncs.pfnCRC32_Final)(pulCRC);
#endif
}
long pfnRandomLong(long lLow, long lHigh)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRandomLong: lLow=%d lHigh=%d\n",lLow,lHigh); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnRandomLong)(lLow, lHigh);
#endif
}
float pfnRandomFloat(float flLow, float flHigh)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRandomFloat:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnRandomFloat)(flLow, flHigh);
#endif
}
void pfnSetView(const edict_t *pClient, const edict_t *pViewent )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetView:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetView)(pClient, pViewent);
#endif
}
float pfnTime( void )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTime:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnTime)();
#endif
}
void pfnCrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCrosshairAngle:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCrosshairAngle)(pClient, pitch, yaw);
#endif
}
byte *pfnLoadFileForMe(char *filename, int *pLength)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnLoadFileForMe: filename=%s\n",filename); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnLoadFileForMe)(filename, pLength);
#endif
}
void pfnFreeFile(void *buffer)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFreeFile:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnFreeFile)(buffer);
#endif
}
void pfnEndSection(const char *pszSectionName)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEndSection:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnEndSection)(pszSectionName);
#endif
}
int pfnCompareFileTime(char *filename1, char *filename2, int *iCompare)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCompareFileTime:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnCompareFileTime)(filename1, filename2, iCompare);
#endif
}
void pfnGetGameDir(char *szGetGameDir)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetGameDir:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnGetGameDir)(szGetGameDir);
#endif
}
void pfnCvar_RegisterVariable(cvar_t *variable)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCvar_RegisterVariable:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCvar_RegisterVariable)(variable);
#endif
}
void pfnFadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFadeClientVolume:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnFadeClientVolume)(pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
#endif
}
void pfnSetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
	// Is this player a bot?
	CBot *pBot = UTIL_GetBotPointer(pEdict);

	if ( pBot )
	{
		pBot->m_fMaxSpeed = fNewMaxspeed;
	}

    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetClientMaxspeed: edict=%x %f\n",pEdict,fNewMaxspeed); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetClientMaxspeed)(pEdict, fNewMaxspeed);
#endif
}
edict_t * pfnCreateFakeClient(const char *netname)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateFakeClient:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnCreateFakeClient)(netname);
#endif
}
void pfnRunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRunPlayerMove:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnRunPlayerMove)(fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
#endif
}
int pfnNumberOfEntities(void)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnNumberOfEntities:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnNumberOfEntities)();
#endif
}
char* pfnGetInfoKeyBuffer(edict_t *e)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetInfoKeyBuffer:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
#endif
}
char* pfnInfoKeyValue(char *infobuffer, char *key)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnInfoKeyValue: %s %s\n",infobuffer,key); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnInfoKeyValue)(infobuffer, key);
#endif
}
void pfnSetKeyValue(char *infobuffer, char *key, char *value)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetKeyValue: %s %s\n",key,value); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetKeyValue)(infobuffer, key, value);
#endif
}
void pfnSetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
	edict_t *pEdict = INDEXENT(clientIndex);

	// Copy a players reputation info to a new name
	if ( strcmp(key,"name") == 0 )
	{
		if ( pEdict )
		{
			CBot *pBot;
			CBotReputation *pRep;			
			CBotReputations *pRepList;
			int i;
			
			int iOldPlayerRepId = GetPlayerEdictRepId(pEdict);

			if ( iOldPlayerRepId != -1 ) // otherwise : error...
			{		
				for ( i = 0; i < MAX_PLAYERS; i ++ )
				{
					pBot = &gBotGlobals.m_Bots[i];
					
					if ( pBot && (pBot->m_iRespawnState == RESPAWN_IDLE) )
					{
						if ( pBot->m_pEdict && pBot->m_bIsUsed )
						{
							if ( pBot->m_pEdict == pEdict )
								continue;
							
							pRepList = &pBot->m_Profile.m_Rep;
							
							if ( (pRep = pBot->m_Profile.m_Rep.GetRep(iOldPlayerRepId)) != NULL )
							{
								// New name = value

								int iNewPlayerRepId = GetPlayerRepId(value);

								if ( pBot->m_Profile.m_Rep.GetRep(iNewPlayerRepId) == NULL )
									pRepList->AddRep(iNewPlayerRepId,pRep->CurrentRep());
							}
							else
							{
							//	ERROR!

							}
						}
					}
				}
			}
			
			int iFlags = pEdict->v.flags;
			
			if ( (iFlags & FL_CLIENT) && !(iFlags & FL_FAKECLIENT) )
			{
				CClient *pClient = gBotGlobals.m_Clients.GetClientByEdict(pEdict);
				
				if ( pClient )
				{
					pClient->m_bRecheckAuth	= TRUE;
				}
			}
			
		}

	}
	
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetClientKeyValue: %s %s\n",key,value); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
#endif
}
int pfnIsMapValid(char *filename)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsMapValid:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnIsMapValid)(filename);
#endif
}
void pfnStaticDecal( const float *origin, int decalIndex, int entityIndex, int modelIndex )
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnStaticDecal:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnStaticDecal)(origin, decalIndex, entityIndex, modelIndex);
#endif
}
int pfnPrecacheGeneric(char* s)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheGeneric: %s\n",s); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnPrecacheGeneric)(s);
#endif
}
int pfnGetPlayerUserId(edict_t *e )
{
    if (gpGlobals->deathmatch)
    {
        if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerUserId: %x\n",e); fclose(fp); }       
    }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnGetPlayerUserId)(e);
#endif
}

const char *pfnGetPlayerAuthId (edict_t *e)
{
	static const char *BOT_STEAM_ID = "BOT";
	BOOL bIsBot = ( UTIL_GetBotPointer(e) != NULL );
#ifdef RCBOT_META_BUILD
   
   if ( bIsBot )
      RETURN_META_VALUE (MRES_SUPERCEDE, BOT_STEAM_ID);

   RETURN_META_VALUE (MRES_IGNORED, NULL);
#else

   if ( bIsBot )
	   return BOT_STEAM_ID;

   return (*g_engfuncs.pfnGetPlayerAuthId)(e);
#endif
}

void pfnBuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnBuildSoundMsg:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
#endif
}
int pfnIsDedicatedServer(void)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsDedicatedServer:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnIsDedicatedServer)();
#endif
}
cvar_t* pfnCVarGetPointer(const char *szVarName)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetPointer: %s\n",szVarName); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnCVarGetPointer)(szVarName);
#endif
}
unsigned int pfnGetPlayerWONId(edict_t *e)
{
	if ( gBotGlobals.IsMod(MOD_RC2) )
	{
		if ( e->v.flags & FL_FAKECLIENT )
		{
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_SUPERCEDE,ENTINDEX(e));
#else
    return ENTINDEX(e);
#endif
		}
	}

    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerWONId: %x\n",e); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnGetPlayerWONId)(e);
#endif
}


// new stuff for SDK 2.0

void pfnInfo_RemoveKey(char *s, const char *key)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnInfo_RemoveKey:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnInfo_RemoveKey)(s, key);
#endif
}
const char *pfnGetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPhysicsKeyValue:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnGetPhysicsKeyValue)(pClient, key);
#endif
}
void pfnSetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetPhysicsKeyValue:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetPhysicsKeyValue)(pClient, key, value);
#endif
}
const char *pfnGetPhysicsInfoString(const edict_t *pClient)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPhysicsInfoString:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnGetPhysicsInfoString)(pClient);
#endif
}
unsigned short pfnPrecacheEvent(int type, const char *psz)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheEvent:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnPrecacheEvent)(type, psz);
#endif
}
void pfnPlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
                      float *origin, float *angles, float fparam1,float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPlaybackEvent:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnPlaybackEvent)(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
#endif
}
unsigned char *pfnSetFatPVS(float *org)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetFatPVS:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnSetFatPVS)(org);
#endif
}
unsigned char *pfnSetFatPAS(float *org)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetFatPAS:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,NULL);
#else
    return (*g_engfuncs.pfnSetFatPAS)(org);
#endif
}
int pfnCheckVisibility(const edict_t *entity, unsigned char *pset)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCheckVisibility:\n"); fclose(fp); }
    
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnCheckVisibility)(entity, pset);
#endif
}
void pfnDeltaSetField(struct delta_s *pFields, const char *fieldname)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaSetField:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnDeltaSetField)(pFields, fieldname);
#endif
}
void pfnDeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaUnsetField:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnDeltaUnsetField)(pFields, fieldname);
#endif
}
void pfnDeltaAddEncoder(char *name, void (*conditionalencode)( struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaAddEncoder:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnDeltaAddEncoder)(name, conditionalencode);
#endif
}
int pfnGetCurrentPlayer(void)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetCurrentPlayer:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnGetCurrentPlayer)();
#endif
}
int pfnCanSkipPlayer(const edict_t *player)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCanSkipPlayer:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnCanSkipPlayer)(player);
#endif
}
int pfnDeltaFindField(struct delta_s *pFields, const char *fieldname)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaFindField:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnDeltaFindField)(pFields, fieldname);
#endif
}
void pfnDeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaSetFieldByIndex:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnDeltaSetFieldByIndex)(pFields, fieldNumber);
#endif
}
void pfnDeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
    //   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaUnsetFieldByIndex:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnDeltaUnsetFieldByIndex)(pFields, fieldNumber);
#endif
}
void pfnSetGroupMask(int mask, int op)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetGroupMask:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnSetGroupMask)(mask, op);
#endif
}
int pfnCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateInstancedBaseline:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META_VALUE(MRES_IGNORED,0);
#else
    return (*g_engfuncs.pfnCreateInstancedBaseline)(classname, baseline);
#endif
}
void pfnCvar_DirectSet(struct cvar_s *var, char *value)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCvar_DirectSet:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnCvar_DirectSet)(var, value);
#endif
}
void pfnForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnForceUnmodified:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnForceUnmodified)(type, mins, maxs, filename);
#endif
}
void pfnGetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
    if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerStats:\n"); fclose(fp); }
#ifdef RCBOT_META_BUILD
    RETURN_META(MRES_IGNORED);
#else
    (*g_engfuncs.pfnGetPlayerStats)(pClient, ping, packet_loss);
#endif
}

int pfnCmd_Argc (void) 
{ 
    // this function returns the number of arguments the current client command string has. Since 
    // bots have no client DLL and we may want a bot to execute a client command, we had to 
    // implement a g_argv string in the bot DLL for holding the bots' commands, and also keep 
    // track of the argument count. Hence this hook not to let the engine ask an unexistent client 
    // DLL for a command we are holding here. Of course, real clients commands are still retrieved 
    // the normal way, by asking the engine. 
#ifdef RCBOT_META_BUILD    
   if (gBotGlobals.m_bIsFakeClientCommand)
      RETURN_META_VALUE (MRES_SUPERCEDE, gBotGlobals.m_iFakeArgCount);

   RETURN_META_VALUE (MRES_IGNORED, 0);
#else
    // is this a bot issuing that client command ? 
    if (gBotGlobals.m_bIsFakeClientCommand) 
        return (gBotGlobals.m_iFakeArgCount); // if so, then return the argument count we know 

    return ((*g_engfuncs.pfnCmd_Argc) ()); // ask the engine how many arguments there are 
#endif

} 


const char *pfnCmd_Args (void) 
{ 
    // this function returns a pointer to the whole current client command string. Since bots 
    // have no client DLL and we may want a bot to execute a client command, we had to implement 
    // a g_argv string in the bot DLL for holding the bots' commands, and also keep track of the 
    // argument count. Hence this hook not to let the engine ask an unexistent client DLL for a 
    // command we are holding here. Of course, real clients commands are still retrieved the 
    // normal way, by asking the engine. 
    
    extern char *g_argv;
    
    // is this a bot issuing that client command ? 
    if (gBotGlobals.m_bIsFakeClientCommand) 
    { 
#ifdef RCBOT_META_BUILD
        // is it a "say" or "say_team" client command ? 
        if (strncmp ("say ", g_argv, 4) == 0) 
            RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[0] + 4); // skip the "say" bot client command (bug in HL engine) 
        else if (strncmp ("say_team ", g_argv, 9) == 0) 
            RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[0] + 9); // skip the "say_team" bot client command (bug in HL engine) 
        
        RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[0]); // else return the whole bot client command string we know 
#else
        // is it a "say" or "say_team" client command ? 
        if (strncmp ("say ", g_argv, 4) == 0) 
            return (&g_argv[0] + 4); // skip the "say" bot client command (bug in HL engine) 
        else if (strncmp ("say_team ", g_argv, 9) == 0) 
            return (&g_argv[0] + 9); // skip the "say_team" bot client command (bug in HL engine) 
        
        return (&g_argv[0]); // else return the whole bot client command string we know 
#endif
    } 

#ifdef RCBOT_META_BUILD
	RETURN_META_VALUE (MRES_IGNORED, NULL);
#else
    return ((*g_engfuncs.pfnCmd_Args) ()); // ask the client command string to the engine 
#endif

} 


const char *pfnCmd_Argv (int argc) 
{ 
    // this function returns a pointer to a certain argument of the current client command. Since 
    // bots have no client DLL and we may want a bot to execute a client command, we had to 
    // implement a g_argv string in the bot DLL for holding the bots' commands, and also keep 
    // track of the argument count. Hence this hook not to let the engine ask an unexistent client 
    // DLL for a command we are holding here. Of course, real clients commands are still retrieved 
    // the normal way, by asking the engine.
    
    extern char *g_argv;


#ifdef RCBOT_META_BUILD
   if (gBotGlobals.m_bIsFakeClientCommand)
   {
		RETURN_META_VALUE (MRES_SUPERCEDE,GetArg (g_argv, argc));
   }
   else
   {
	   RETURN_META_VALUE (MRES_IGNORED,NULL);
	   //return ((*g_engfuncs.pfnCmd_Argv) (argc));    
   }
#else
    // is this a bot issuing that client command ? 
    if (gBotGlobals.m_bIsFakeClientCommand) 	
        return (GetArg (g_argv, argc)); // if so, then return the wanted argument we know 
    
    return ((*g_engfuncs.pfnCmd_Argv) (argc)); // ask the argument number "argc" to the engine 
#endif
}

const char *GetArg (const char *command, int arg_number) 
{ 
    // the purpose of this function is to provide fakeclients (bots) with the same Cmd_Argv 
    // convenience the engine provides to real clients. This way the handling of real client 
    // commands and bot client commands is exactly the same, just have a look in engine.cpp 
    // for the hooking of pfnCmd_Argc, pfnCmd_Args and pfnCmd_Argv, which redirects the call 
    // either to the actual engine functions (when the caller is a real client), either on 
    // our function here, which does the same thing, when the caller is a bot. 
    
    int length, i, index = 0, arg_count = 0, fieldstart, fieldstop; 
    
    static char arg[1024];
    
//	if ( arg_number == 0 )
		arg[0] = 0; // reset arg 

	if ( !command || !*command )
		return NULL;

    length = strlen (command); // get length of command 
    
    // while we have not reached end of line 
    while ((index < length) && (arg_count <= arg_number)) 
    { 
        while ((index < length) && (command[index] == ' ')) 
            index++; // ignore spaces 
        
        // is this field multi-word between quotes or single word ? 
        if (command[index] == '"') 
        { 
            index++; // move one step further to bypass the quote 
            fieldstart = index; // save field start position 
            while ((index < length) && (command[index] != '"')) 
                index++; // reach end of field 
            fieldstop = index - 1; // save field stop position 
            index++; // move one step further to bypass the quote 
        } 
        else 
        { 
            fieldstart = index; // save field start position 
            while ((index < length) && (command[index] != ' ')) 
                index++; // reach end of field 
            fieldstop = index - 1; // save field stop position 
        } 
        
        // is this argument we just processed the wanted one ? 
        if (arg_count == arg_number) 
        { 
            for (i = fieldstart; i <= fieldstop; i++) 
                arg[i - fieldstart] = command[i]; // store the field value in a string 
            arg[i - fieldstart] = 0; // terminate the string 
        } 
        
        arg_count++; // we have processed one argument more 
    } 
    
    return (&arg[0]); // returns the wanted argument 
} 


