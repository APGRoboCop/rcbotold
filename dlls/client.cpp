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
// client.cpp
//
//////////////////////////////////////////////////
//
// Client functions, all players on the server are clients
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

extern CBotGlobals gBotGlobals;

extern CWaypointLocations WaypointLocations;
extern CWaypointVisibilityTable WaypointVisibility;

// waypoints with information bits (flags)
extern WAYPOINTS waypoints;//[MAX_WAYPOINTS];

// number of waypoints currently in use
extern int num_waypoints;

// declare the array of head pointers to the path structures...
extern PATH *paths[MAX_WAYPOINTS];

//
// Set up player
void CClient :: SetEdict ( edict_t *pPlayer )
{
	// set edict of player
	m_pPlayer = pPlayer;

	// set won id [if available]
	m_iWonId = (*g_engfuncs.pfnGetPlayerWONId)(pPlayer);
}

void CClient :: Think ( void )
{
	// debugging a bot
	if ( m_iDebugBot != -1 )
	{
		if ( (m_iDebugBot >= MAX_PLAYERS) || (m_iDebugBot < 0) )
			m_iDebugBot = -1;
		else if ( m_fDebugBotTime < gpGlobals->time )
		{
			CBot *pBot = &gBotGlobals.m_Bots[m_iDebugBot];

			if ( !pBot->IsUsed() )
				m_iDebugBot = -1;
			else
			{
				HudText hudmessage;

				/*
				void SetPosition ( float x, float y )
void SetEffect ( int effect )
void SetFadeTime ( float fadein, float fadeout )
void SetHoldTime ( float holdtime )
void SetChannel ( int channel )
void SetEffectTime ( float time )
void SayMessage  ( const char *message, edict_t *pPlayer );
void SayMessage  ( const char *message, Vector colour1, Vector colour2, edict_t *pPlayer );
void Initialise  ( void );
void SetColour1  ( Vector colours, int alpha )
void SetColour2  ( Vector colours, int alpha )
void InitMessage ( const char *message );
*/
				char message[512];
				CBotTask *pTask;

				if ( (pTask=pBot->currentTask()) == NULL )
					sprintf(message,"Debugging Bot: \"%s\"\nNo Tasks\nWaypoint: %d, Goal: %d",pBot->m_szBotName,pBot->m_iCurrentWaypointIndex,pBot->m_iWaypointGoalIndex);
				else
				{
					sprintf(message,"Debugging Bot: \"%s\"\nTask: %s\n int: %d\n float: %0.2f\nSchedule: %s\nPrev Task:%s\nPrev Failed:%s\nWaypoint: %d, Goal: %d\nLook task: %s",
						pBot->m_szBotName,pTask->getTaskDescription(),pTask->TaskInt(),pTask->TaskFloat(),
						pTask->getScheduleDescription(),
						CBotTask::_getTaskDescription(pBot->m_iLastBotTask),
						CBotTask::_getTaskDescription(pBot->m_iLastFailedTask),pBot->m_iCurrentWaypointIndex,pBot->m_iWaypointGoalIndex,pBot->getLookTaskDescription());
				}

				hudmessage.SetChannel(3);
				hudmessage.SetEffect(HUD_EFFECT_CREDITS);
				hudmessage.SetFadeTime(0,0);
				hudmessage.SetEffectTime(1);
				hudmessage.SetPosition(0.6,0.1);
				hudmessage.SetColour1(Vector(200,200,200),200);
				hudmessage.SetColour2(Vector(255,255,255),200);
				hudmessage.SayMessage(message,m_pPlayer);
			}
			
			m_fDebugBotTime = gpGlobals->time + 1.0;
		}
	}

	if ( m_pDebugEnt && (m_fDebugEntTime < gpGlobals->time) )
	{
		HudText hudmessage;
		
		/*
		void SetPosition ( float x, float y )
		void SetEffect ( int effect )
		void SetFadeTime ( float fadein, float fadeout )
		void SetHoldTime ( float holdtime )
		void SetChannel ( int channel )
		void SetEffectTime ( float time )
		void SayMessage  ( const char *message, edict_t *pPlayer );
		void SayMessage  ( const char *message, Vector colour1, Vector colour2, edict_t *pPlayer );
		void Initialise  ( void );
		void SetColour1  ( Vector colours, int alpha )
		void SetColour2  ( Vector colours, int alpha )
		void InitMessage ( const char *message );
		*/
		char message[1024];
		
		sprintf(message,"name %s, flags %d\nhealth %0.1f, max_health %0.2f, armorvalue %0.2f\niuser1 %d, iuser2 %d\niuser3 %d, iuser4 %d\nfuser1 %0.2f, fuser2 %0.2f\nfuser3 %0.2f, fuser4 %0.2f\nvuser1 (%0.2f,%0.2f,%0.2f), vuser2 (%0.2f,%0.2f,%0.2f)\nvuser3 (%0.2f,%0.2f,%0.2f), vuser4 (%0.2f,%0.2f,%0.2f)",
			STRING(m_pDebugEnt->v.classname),m_pDebugEnt->v.flags,
			m_pDebugEnt->v.health,m_pDebugEnt->v.max_health,m_pDebugEnt->v.armorvalue,
			m_pDebugEnt->v.iuser1,m_pDebugEnt->v.iuser2,
			m_pDebugEnt->v.iuser3,m_pDebugEnt->v.iuser4,
			m_pDebugEnt->v.fuser1,m_pDebugEnt->v.fuser2,
			m_pDebugEnt->v.fuser3,m_pDebugEnt->v.fuser4,
			m_pDebugEnt->v.vuser1.x,m_pDebugEnt->v.vuser1.y,m_pDebugEnt->v.vuser1.z,
			m_pDebugEnt->v.vuser2.x,m_pDebugEnt->v.vuser2.y,m_pDebugEnt->v.vuser2.z,
			m_pDebugEnt->v.vuser3.x,m_pDebugEnt->v.vuser3.y,m_pDebugEnt->v.vuser3.z,
			m_pDebugEnt->v.vuser4.x,m_pDebugEnt->v.vuser4.y,m_pDebugEnt->v.vuser4.z);
		
		hudmessage.SetChannel(5);
		hudmessage.SetEffect(HUD_EFFECT_CREDITS);
		hudmessage.SetFadeTime(0,0);
		hudmessage.SetEffectTime(1);
		hudmessage.SetPosition(0.0,0.1);
		hudmessage.SetColour1(Vector(200,200,200),200);
		hudmessage.SetColour2(Vector(255,255,255),200);
		hudmessage.SayMessage(message,m_pPlayer);
		
		m_fDebugEntTime = gpGlobals->time + 1.0f;
	}

	if ( m_bRecheckAuth )
	{                   
		CAllowedPlayer *pUser;

		pUser = gBotGlobals.m_BotUsers.GetPlayer(this);
		
		if ( pUser )
		{
			pUser->GiveClientAccess(this);
			
			BotMessage(m_pPlayer,0,"You have been authenticated");
		}
		
		m_bRecheckAuth = FALSE;
	}

	if ( m_fCheckSentryTime && (m_fCheckSentryTime < gpGlobals->time) )
	{
		edict_t *pent = NULL;

		m_fCheckSentryTime = 0;

		while ( (pent = UTIL_FindEntityByClassname(pent,"building_sentrygun")) != NULL )
		{
			if ( !ignoreSentries.IsMember(pent) && (gBotGlobals.TFC_getTeamViaColorMap(pent) == m_pPlayer->v.team))
			{
				if ( BotFunc_DistanceBetweenEdicts(pent,m_pPlayer) < 128 )
				{
					setTFCSentry(pent);
					pent->v.iuser1 = 1;
					pent->v.euser1 = m_pPlayer;
					break;
				}
			}
		}
	}

	if ( m_pForceGripEntity )
	{
		entvars_t *pev = &m_pPlayer->v;
		TraceResult tr;

		Vector vSrc = pev->origin + pev->view_ofs;
		Vector vEnd = vSrc + gpGlobals->v_forward * 210.0;		
		Vector vComp;
		Vector vAngles;

		float fDist;

		UTIL_MakeVectors(pev->v_angle);

		UTIL_TraceLine(vSrc,vEnd,ignore_monsters,dont_ignore_glass,m_pPlayer,&tr);

		if ( tr.flFraction < 1.0 )
		{
			vComp = tr.vecEndPos - vSrc;
			fDist = vComp.Length();
			vComp = vComp / fDist;
			
			vEnd = vSrc + (vComp * (fDist-m_pForceGripEntity->v.size.Length2D()));
		}

		vComp = vSrc - vEnd;

		fDist = vComp.Length();

		UTIL_SetOrigin(&m_pForceGripEntity->v,vEnd);

		vAngles = UTIL_VecToAngles(vComp);
		UTIL_FixAngles(&vAngles);

		vAngles.z = 0;
		vAngles.x = m_pForceGripEntity->v.v_angle.x;

		m_pForceGripEntity->v.v_angle = vAngles;
		//m_pForceGripEntity->v.angles.x = -m_pForceGripEntity->v.angles.x;
		m_pForceGripEntity->v.angles = m_pForceGripEntity->v.v_angle;
		m_pForceGripEntity->v.angles.x = m_pForceGripEntity->v.v_angle.x/3;
		m_pForceGripEntity->v.flags |= FL_FROZEN;
	}
	
	// not a bot
	// and has not recieved a welcome?
	if ( !(m_pPlayer->v.flags & FL_FAKECLIENT) )
	{			
			// and has not recieved a welcome?
			if ( gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TOOLTIPS) && !HasToolTipSent(BOT_TOOL_TIP_SERVER_WELCOME) && !HasToolTipSent(BOT_TOOL_TIP_CLIENT_WELCOME) )
			{
				// joined 5 seconds ago
				if ( (m_fJoinServerTime + 5.0) < gpGlobals->time )
				{
					// send message
					
					// send special message to server player
					if ( m_pPlayer == gBotGlobals.m_pListenServerEdict )
					{
						AddNewToolTip(BOT_TOOL_TIP_SERVER_WELCOME);	
					}
					else
					{
						// send other message to client
						// (where to get the bot e.g.!)
						AddNewToolTip(BOT_TOOL_TIP_CLIENT_WELCOME);
					}
					
					switch ( RANDOM_LONG(0,4) )
					{
					case 0:
						UTIL_PlaySound(m_pPlayer,"scientist/hello");
						break;
					case 1:
						UTIL_PlaySound(m_pPlayer,"scientist/hello2");
						break;
					case 2:
						UTIL_PlaySound(m_pPlayer,"scientist/hellofreeman");
						break;
					case 3:
						UTIL_PlaySound(m_pPlayer,"scientist/hellofromlab");
						break;
					case 4:
						UTIL_PlaySound(m_pPlayer,"scientist/hellothere");
						break;
					}
				}
			}
			// given a welcome
			else if ( !HasToolTipSent(BOT_TOOL_TIP_CREATE_SQUAD) && (m_pPlayer->v.flags & FL_ONGROUND) )
			{
				if ( gBotGlobals.m_iNumBots > 1 )
				{
					// joined at least 30 seconds ago
					if ( (m_fJoinServerTime + 30.0) < gpGlobals->time )
					{
						AddNewToolTip(BOT_TOOL_TIP_CREATE_SQUAD);

						if ( gBotGlobals.IsConfigSettingOn(BOT_CONFIG_ENABLE_BOTCAM) )
							AddNewToolTip(BOT_TOOL_TIP_BOTCAM_HELP);
					}
				}
			}
			
			if ( gBotGlobals.IsNS() )
			{
				if ( EntityIsCommander(m_pPlayer) )
				{
					AddNewToolTip(BOT_TOOL_TIP_COMMANDER_WELCOME);
					AddNewToolTip(BOT_TOOL_TIP_COMMANDER_SQUAD);
					AddNewToolTip(BOT_TOOL_TIP_COMMANDER_SQUAD2);
				}
			}		
	}

	if ( hasCmd(CLIENT_CMD_PROJ_REPEL) )
	{
		edict_t *pEntity = NULL;

		Vector v_other;
		Vector v_comp;
		Vector v_origin = m_pPlayer->v.origin-Vector(0,0,m_pPlayer->v.size.z*0.5);
		Vector v_original_vel;

		while ( (pEntity = UTIL_FindEntityInSphere(pEntity,m_pPlayer->v.origin,m_pPlayer->v.size.Length()*1.1)) != NULL )
		{
			if ( pEntity == m_pPlayer )
				continue;
			if ( pEntity->v.owner == m_pPlayer )
				continue;
			if ( !ENTINDEX(pEntity) )
				continue;
			if ( pEntity->v.flags & FL_WORLDBRUSH )
				continue;

			if ( BotFunc_EntityIsMoving(&pEntity->v) )
			{
				v_original_vel = pEntity->v.velocity;
				v_comp = EntityOrigin(pEntity)-v_origin;
				pEntity->v.velocity = v_comp.Normalize()*v_original_vel.Length();
				
				//v_other = CrossProduct(v_comp.Normalize(),Vector(0,0,1));

				//pEntity->v.velocity = CrossProduct(v_comp,v_other);
			}
		}
	}

	// Waypoints ON???
	if ( m_bWaypointOn && (m_fWaypointDisplayTime < gpGlobals->time) )
	{
		int n;
		
		int iCurrentWaypoint;
		
		edict_t *pPlayer;
		
		Vector vOrigin;

		/////////////////////////////////////////////////////////////////
		// GET ALL INFO NEEDED...
		m_fWaypointDisplayTime = gpGlobals->time + 1.0;
		
		pPlayer = GetPlayer();

		vOrigin = pPlayer->v.origin;
		
		iCurrentWaypoint = m_iCurrentWaypoint;
		/////////////////////////////////////////////////////////////////
		// DRAW WAYPOINTS
		WaypointLocations.DrawWaypoints(pPlayer,vOrigin,600);
		/////////////////////////////////////////////////////////////////
		
		if ( m_bWaypointPathsOn )
		{
			iCurrentWaypoint = WaypointLocations.NearestWaypoint(vOrigin,BOT_WAYPOINT_TOUCH_DIST,-1);
			
			if ( iCurrentWaypoint != -1 )
				m_iCurrentWaypoint = iCurrentWaypoint;
			
			if ( iCurrentWaypoint != -1 )
			{		
				// check if path waypointing is on...
				
				PATH *p;
				
				p = paths[iCurrentWaypoint];					
				
				while (p != NULL)
				{
					n = 0;
					
					while (n < MAX_PATH_INDEX)
					{
						if (p->index[n] != -1)
						{
							// draw a white line to this index's waypoint
							WaypointDrawBeam(pPlayer, waypoints[iCurrentWaypoint].origin, waypoints[p->index[n]].origin, 10, 2, 250, 250, 250, 200, 10);
						}
						
						n++;
					}
					
					p = p->next;  // go to next node in linked list
				}
				
			}				
		}
	}

	if ( IsAutoWaypointOn() )
	{
		if ( !m_bSetUpAutoWaypoint || !EntityIsAlive(m_pPlayer) )
		{
			int i;
			int start = 0;

			if ( EntityIsAlive(m_pPlayer) )
				start = 1; // grab one location


			m_fLastAutoWaypointCheckTime = gpGlobals->time + gBotGlobals.m_fAutoWaypointCheckTime;

			if ( EntityIsAlive(m_pPlayer) )
				m_vLastAutoWaypointCheckPos[0].SetVector(m_pPlayer->v.origin);

			for ( i = start; i < MAX_STORED_AUTOWAYPOINT; i++ )
			{
				m_vLastAutoWaypointCheckPos[i].UnSetPoint();
			}

			m_vLastAutoWaypointPlacePos = m_pPlayer->v.origin;
			m_bSetUpAutoWaypoint = TRUE;
			m_fCanPlaceJump = 0;
			m_iLastButtons = 0;

			m_iLastJumpWaypointIndex = -1;
			m_iLastLadderWaypointIndex = -1;
			m_iLastMoveType = 0;
			m_fCanPlaceLadder = 0;
			m_iJoinLadderWaypointIndex = -1;
		}
		else
		{
			entvars_t *pev = &m_pPlayer->v;
			Vector vPlayerOrigin = pev->origin;

			// ****************************************************
			// Jump waypoint
			//
			// wait until checkJump is not -1 (meaning that bot is in the air)
			// set checkJump to time+0.5 after landing 
			// 
			// ****************************************************

			if ( (pev->waterlevel < 3) && (m_fCanPlaceJump < gpGlobals->time) )
			{	
				BOOL bStunt = gBotGlobals.IsMod(MOD_TS)&&(m_iLastButtons & IN_ALT1);
				Vector v_floor;

				if ( (m_fCanPlaceJump != -1) && (bStunt||(m_iLastButtons & IN_JUMP)) && !(pev->flags & FL_ONGROUND) )
				{
					int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 80.0, -1, TRUE, FALSE, FALSE, NULL);

					m_iLastJumpWaypointIndex = -1;
					
					if ( iNearestWpt == -1 )
					{
						if ( bStunt )
						{
							m_iLastJumpWaypointIndex = WaypointAddOrigin(vPlayerOrigin,W_FL_STUNT,m_pPlayer,m_bWaypointOn,m_bWaypointOn);

							if ( (m_iLastJumpWaypointIndex != -1) && UTIL_CanStand(waypoints[m_iLastJumpWaypointIndex].origin,&v_floor) )
							{
								waypoints[m_iLastJumpWaypointIndex].flags &= ~W_FL_CROUCH;
								waypoints[m_iLastJumpWaypointIndex].origin = v_floor+Vector(0,0,36);
							}
						}
						else
							m_iLastJumpWaypointIndex = WaypointAddOrigin(vPlayerOrigin,W_FL_JUMP,m_pPlayer,m_bWaypointOn,m_bWaypointOn);
					}
					else
						m_iLastJumpWaypointIndex = iNearestWpt; // can still update a current waypoint for land position
					
					m_vLastAutoWaypointPlacePos = vPlayerOrigin;

					m_fCanPlaceJump = -1;

					if ( bStunt )
						m_iJumpType = JUMP_TYPE_STUNT;
					else
						m_iJumpType = JUMP_TYPE_JUMP;
				}
				// ****************************************************
				// Join jump waypoint to the landed waypoint
				// ****************************************************
				else if ( (m_fCanPlaceJump == -1) && (pev->flags & FL_ONGROUND) )
				{
					if ( m_iLastJumpWaypointIndex != -1 )
					{
						int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 80.0, -1, TRUE, FALSE, FALSE, NULL);
						
						if ( iNearestWpt == -1 )
						{
							int iNewWpt = WaypointAddOrigin(vPlayerOrigin,0,m_pPlayer,m_bWaypointOn,m_bWaypointOn);

							if ( iNewWpt != -1 )
							{
								if ( BotNavigate_FindPathFromTo(m_iLastJumpWaypointIndex,iNewWpt,-1) == NULL )
								{
									WaypointAddPath(m_iLastJumpWaypointIndex,iNewWpt);																	
								}

								if ( m_iJumpType == JUMP_TYPE_STUNT )							
									waypoints[m_iLastJumpWaypointIndex].flags |= W_FL_STUNT;																
								else
									waypoints[m_iLastJumpWaypointIndex].flags |= W_FL_JUMP;
								
								if ( UTIL_CanStand(waypoints[iNewWpt].origin,&v_floor) )
								{
									waypoints[iNewWpt].flags &= ~W_FL_CROUCH;
									waypoints[iNewWpt].origin = v_floor+Vector(0,0,36);
								}
							}
						}
						else if ( iNearestWpt != m_iLastJumpWaypointIndex )
						{
							if ( BotNavigate_FindPathFromTo(m_iLastJumpWaypointIndex,iNearestWpt,-1) == NULL )
							{
								WaypointAddPath(m_iLastJumpWaypointIndex,iNearestWpt);
								
								if ( m_iJumpType == JUMP_TYPE_STUNT )
									waypoints[m_iLastJumpWaypointIndex].flags |= W_FL_STUNT;								
								else
									waypoints[m_iLastJumpWaypointIndex].flags |= W_FL_JUMP;								
							}
						}
					}

					m_iLastJumpWaypointIndex = -1;

					// wait a sec after player lands before checking jump again
					m_fCanPlaceJump = gpGlobals->time + 0.5;
				}				
			}

			BOOL bCheckDistance = (pev->movetype != MOVETYPE_FLY) && (m_fCanPlaceLadder == 0); // always check distance unless ladder climbing

			// ****************************************************
			// Ladder waypoint
			// make the frist waypoint (e.g. bottom waypoint)
			// ****************************************************
			if ( (pev->movetype == MOVETYPE_FLY) && !(m_iLastMoveType == MOVETYPE_FLY) )
			{
				// went ON to a ladder

				int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 80.0, -1, TRUE, FALSE, FALSE, NULL);

				m_iLastLadderWaypointIndex = -1;
					
				if ( iNearestWpt == -1 )
					m_iLastLadderWaypointIndex = WaypointAddOrigin(vPlayerOrigin,W_FL_LADDER,m_pPlayer,m_bWaypointOn,m_bWaypointOn);
				else
				{
					m_iLastLadderWaypointIndex = iNearestWpt; // can still update a current waypoint for land position

					waypoints[m_iLastLadderWaypointIndex].flags |= W_FL_LADDER; // update flags
				}
					
				m_vLastAutoWaypointPlacePos = vPlayerOrigin;

				bCheckDistance = FALSE;

				m_fCanPlaceLadder = 0;

				// need to unset every check point when going on ladder first time
				for ( int i = 0; i < MAX_STORED_AUTOWAYPOINT; i ++ )
				{
						m_vLastAutoWaypointCheckPos[i].UnSetPoint();					
				}
			}
			else if ( !(pev->movetype == MOVETYPE_FLY) && (m_iLastMoveType == MOVETYPE_FLY) )
			{
				// went OFF a ladder
				m_fCanPlaceLadder = gpGlobals->time + 0.2;
			}
			
			// ****************************************************
			// If we have walked off a ladder for a small amount of time
			// Make the top/bottom ladder waypoint
			// ****************************************************
			if ( m_fCanPlaceLadder && (m_fCanPlaceLadder < gpGlobals->time ) )
			{
				if ( m_iLastLadderWaypointIndex != -1 )
					// place a ladder waypoint before jumping off
				{
					int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 80.0, -1, TRUE, FALSE, FALSE, NULL);
					
					if ( iNearestWpt == -1 )
					{
						int iNewWpt = WaypointAddOrigin(vPlayerOrigin,W_FL_LADDER,m_pPlayer,m_bWaypointOn,m_bWaypointOn);
						
						if ( iNewWpt != -1 )
						{
							m_iJoinLadderWaypointIndex = iNewWpt;

							if ( BotNavigate_FindPathFromTo(m_iLastLadderWaypointIndex,iNewWpt,-1) == NULL )
								WaypointAddPath(m_iLastLadderWaypointIndex,iNewWpt);
						}
					}
					else if ( iNearestWpt != m_iLastLadderWaypointIndex )
					{
						m_iJoinLadderWaypointIndex = iNearestWpt;

						if ( BotNavigate_FindPathFromTo(m_iLastLadderWaypointIndex,iNearestWpt,-1) == NULL )
							WaypointAddPath(m_iLastLadderWaypointIndex,iNearestWpt);
					}				
				}
				
				m_iLastLadderWaypointIndex = -1;
				
				bCheckDistance = FALSE;

				m_fCanPlaceLadder = 0;
			}
			
			// ****************************************************
			// Join top ladder waypoint to a ground waypoint
			// ****************************************************
			if ( (m_iJoinLadderWaypointIndex != -1) && (pev->flags & FL_ONGROUND) && (pev->movetype == MOVETYPE_WALK) )
			{
				int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 40.0, m_iJoinLadderWaypointIndex, TRUE, FALSE, FALSE, NULL);
				
				if ( iNearestWpt == -1 )
				{
					int iNewWpt = WaypointAddOrigin(vPlayerOrigin,0,m_pPlayer,m_bWaypointOn,m_bWaypointOn);
					
					if ( iNewWpt != -1 )
					{
						if ( BotNavigate_FindPathFromTo(m_iJoinLadderWaypointIndex,iNewWpt,-1) == NULL )
							WaypointAddPath(m_iJoinLadderWaypointIndex,iNewWpt);
					}
				}
				else if ( iNearestWpt != m_iJoinLadderWaypointIndex )
				{
					if ( BotNavigate_FindPathFromTo(m_iJoinLadderWaypointIndex,iNearestWpt,-1) == NULL )
						WaypointAddPath(m_iJoinLadderWaypointIndex,iNearestWpt);
				}

				m_iJoinLadderWaypointIndex = -1;
			}

			m_iLastButtons = pev->button;
			m_iLastMoveType = pev->movetype;

			if ( m_fLastAutoWaypointCheckTime < gpGlobals->time )
			{
				// ****************************************
				// Corner - Check
				// ****************************************
				//
				// place a "Check - point" at player origin
				//

				CAutoWaypointCheck *vCurVector;				
				Vector vCheckOrigin;

				Vector vPlacePosition;
				int iFlags = 0;
				BOOL bPlace = FALSE;
				
				int i;
				int n;

				TraceResult tr;

				int numset = 0;
				int last = 0;

				for ( n = 0; n < MAX_STORED_AUTOWAYPOINT; n ++ )
				{
					if ( m_vLastAutoWaypointCheckPos[n].IsVectorSet() )
					{
						numset++;
					}
				}

				if ( numset == MAX_STORED_AUTOWAYPOINT )
				{
					// move check points down
					for ( n = 0; n < (MAX_STORED_AUTOWAYPOINT-1); n ++ )
					{
						m_vLastAutoWaypointCheckPos[n] = m_vLastAutoWaypointCheckPos[n+1];						
					}
					
					last = MAX_STORED_AUTOWAYPOINT-1;
				}
				else
				{					
					last = numset;
				}

				iFlags = 0;

				// sort out flags for this waypoint depending on player
				if ((pev->flags & FL_DUCKING) == FL_DUCKING)
				{
					iFlags |= W_FL_CROUCH;  // crouching waypoint
				}
				
				if (pev->movetype == MOVETYPE_FLY)
					iFlags |= W_FL_LADDER;  // waypoint on a ladder

				m_vLastAutoWaypointCheckPos[last].SetPoint(vPlayerOrigin,iFlags);
				
				if ( (m_iLastJumpWaypointIndex==-1) && bCheckDistance && ((vPlayerOrigin - m_vLastAutoWaypointPlacePos).Length() > 200) )
				{
					int iNearestWpt = WaypointLocations.NearestWaypoint(vPlayerOrigin, 150.0, -1, TRUE, FALSE, FALSE, NULL);
					
					if ( iNearestWpt == -1 )
						WaypointAddOrigin(vPlayerOrigin,0,m_pPlayer,m_bWaypointOn,m_bWaypointOn);
					
					// set regardless
					m_vLastAutoWaypointPlacePos = vPlayerOrigin;
				}

				// search for occluded check points from player
				for ( i = 0; i < MAX_STORED_AUTOWAYPOINT; i++ )
				{
					vCurVector = &m_vLastAutoWaypointCheckPos[i];

					if ( !vCurVector->IsVectorSet() )
						continue;

					vCheckOrigin = vCurVector->GetVector();

					UTIL_TraceLine(vPlayerOrigin,vCheckOrigin,ignore_monsters,ignore_glass,m_pPlayer,&tr);

					if ( m_bDebugAutoWaypoint )
					{
						WaypointDrawBeam(m_pPlayer,vCheckOrigin+Vector(0,0,16),vCheckOrigin-Vector(0,0,16),20,1,255,1,1,150,10);
						WaypointDrawBeam(m_pPlayer,vPlayerOrigin,vCheckOrigin,20,1,1,255,1,150,10);
					}
					
					if ( tr.flFraction < 1.0 )
					{
						if ( tr.pHit )
						{
							// on a lift/train moving "fast"
							if ( tr.pHit->v.velocity.Length() > 20.0 )
								continue;
						}
						// find next which is visible
						for ( n = i+1; n < MAX_STORED_AUTOWAYPOINT; n++ )
						{
							vCurVector = &m_vLastAutoWaypointCheckPos[n];
							
							if ( !vCurVector->IsVectorSet() )
								continue;
							
							vCheckOrigin = vCurVector->GetVector();
							
							UTIL_TraceLine(vPlayerOrigin,vCheckOrigin,ignore_monsters,ignore_glass,m_pPlayer,&tr);

							if ( m_bDebugAutoWaypoint )
								WaypointDrawBeam(m_pPlayer,vPlayerOrigin,vCheckOrigin,20,1,1,1,255,150,10);
							
							if ( tr.flFraction >= 1.0 )
							{
								int iNearestWpt = WaypointLocations.NearestWaypoint(vCheckOrigin, 100.0, -1, TRUE, FALSE, FALSE, NULL);
								
								if ( iNearestWpt == -1 )
								{
									bPlace = TRUE;		
									vPlacePosition = vCheckOrigin;
									iFlags = vCurVector->getFlags();
									
									break;
								}
								else
									continue;
							}
						}
						
					}
				}

				if ( bPlace )
				{
					int inewwpt = WaypointAddOrigin(vPlacePosition,iFlags,m_pPlayer,m_bWaypointOn,m_bWaypointOn,FALSE);
					Vector v_floor;

					m_vLastAutoWaypointPlacePos = vPlacePosition;

					if ( (m_iLastJumpWaypointIndex != -1) && UTIL_CanStand(waypoints[inewwpt].origin,&v_floor) )
					{
						waypoints[inewwpt].flags &= ~W_FL_CROUCH;
						//waypoints[inewwpt].origin = v_floor+Vector(0,0,36);
					}
					//clear from i

					int pos = n;
					int n = 0;

					for ( n = 0; pos < MAX_STORED_AUTOWAYPOINT; n ++ )
					{
						m_vLastAutoWaypointCheckPos[n] = m_vLastAutoWaypointCheckPos[pos];

						pos++;
					}

					for ( n = n; n < MAX_STORED_AUTOWAYPOINT; n ++ )
					{
						m_vLastAutoWaypointCheckPos[n].UnSetPoint();					
					}
				}

				m_fLastAutoWaypointCheckTime = gpGlobals->time + gBotGlobals.m_fAutoWaypointCheckTime;
			}
		}
	}

	m_Tooltips.Think(m_pPlayer);
}

void CClient :: FreeGlobalMemory ( void )
{
	if ( m_vTeleportVector )
		free(m_vTeleportVector);
	
	m_vTeleportVector = NULL;
	m_Tooltips.Clear();
	
	memset(this,0,sizeof(CClient));
}	

void CClient :: AddNewToolTip ( eToolTip iToolTip )
{
	if ( !gBotGlobals.IsConfigSettingOn(BOT_CONFIG_TOOLTIPS) )
		return; // tooltips switched off
	
	// only send if haven't sent it before
	if ( !HasToolTipSent(iToolTip) )
	{
		// update send state
		ToolTipSent(iToolTip);

		// don't send to bots
		if ( m_pPlayer->v.flags & FL_FAKECLIENT )
			return;

		// add tool tip to queue
		m_Tooltips.Add(iToolTip);
		
	}
}


void CPendingToolTips :: Think (edict_t *pPlayer)
{
	if ( !m_Tooltips.IsEmpty() )
	{
		if ( m_fNextTime < gpGlobals->time )
		{				
			eToolTip theTip = m_Tooltips.GetFrontInfo();
						
			gBotGlobals.SayToolTip(pPlayer,theTip);

			m_Tooltips.RemoveFront();
			
			m_fNextTime = gpGlobals->time + 20.0;
		}
	}
}

void CClient :: setSteamID ()
{
	if ( m_pPlayer )
	{
		const char *steamid = (*g_engfuncs.pfnGetPlayerAuthId)(m_pPlayer);

		if ( steamid && *steamid )
		{
			strncpy(m_szSteamId,steamid,STEAM_ID_LEN-1);
			m_szSteamId[STEAM_ID_LEN-1] = 0;
		}
	}
}

void CClient :: checkBuiltSentry ()
{
	m_fCheckSentryTime = gpGlobals->time + 0.1;
}

void CClient :: clearIgnoreSentries ()
{
	ignoreSentries.Clear();
}

void CClient :: addIgnoreSentry ( edict_t *pEdict )
{
	ignoreSentries.Add(pEdict);
}

CClient *CClients :: ClientConnected ( edict_t *pPlayer )
{
	// Find a free client spot
	
	/*int i = 0;
	
	while ( (i < MAX_PLAYERS) && m_Clients[i].IsUsed() )
		i++;*/
	int i = ENTINDEX(pPlayer)-1;

//	gBotGlobals.m_iNumClients ++;
	
	if ( i < MAX_PLAYERS )
	{		
		CClient *pClient = GetClientByIndex(i);

		gBotGlobals.m_iJoiningClients[i] = 0;

		gBotGlobals.m_BotCam.TuneOff(pPlayer);
		
		pClient->SetEdict(pPlayer);
		pClient->setSteamID();

		pClient->m_fJoinServerTime = gpGlobals->time;

		int iPlayerRepId = GetPlayerRepId(STRING(pPlayer->v.netname));
		int i;
		CBot *pBot;
		
		pClient->UpdatePlayerRepId ( iPlayerRepId );

		if ( gBotGlobals.IsConfigSettingOn(BOT_CONFIG_AUTOWAYPOINT_HUMANS) )
		{
			if ( !(pPlayer->v.flags & FL_FAKECLIENT) )
				pClient->AutoWaypoint(1);
		}
		
		for ( i = 0; i < MAX_PLAYERS; i ++ )
		{
			pBot = &gBotGlobals.m_Bots[i];
			
			if ( !pBot )
				continue;
			if ( !pBot->m_bIsUsed || !pBot->m_pEdict )
				continue;
			if ( pBot->m_pEdict == pPlayer )
				continue;
			if ( pBot->m_iRespawnState != RESPAWN_IDLE )
				continue;
			
			pBot->m_Profile.m_Rep.AddLoadRep(pBot->m_Profile.m_iProfileId,iPlayerRepId);
		}
		
		CAllowedPlayer *pUser;
		
		pUser = gBotGlobals.m_BotUsers.GetPlayer(pClient);
		
		if ( pUser )
		{
			pUser->GiveClientAccess(pClient);
			BotMessage(pPlayer,0,"You have been authenticated");
		}
		else
		{
			if ( !IS_DEDICATED_SERVER() )
			{
				if ( gBotGlobals.m_pListenServerEdict == NULL )
				{
					gBotGlobals.m_pListenServerEdict = pPlayer;
					pClient->SetAccessLevel(10);
				}
			}
		}
		
		return pClient;
	}
	else
		BugMessage(pPlayer,"Could not allocate client space for %s",STRING(pPlayer->v.netname));		
	
	return NULL;
}

void CClients :: ClientDisconnected ( edict_t *pPlayer )
{
	CClient *pClient = GetClientByEdict(pPlayer);

//	gBotGlobals.m_iNumClients --;
//	gBotGlobals.m_iJoiningClients --;

	//BotMessage(NULL,0,"Client left, number of clients = %d",UTIL_GetNumClients());

	if ( pClient )
	{
		ClientDisconnected(pClient);
	}
	else
	{
		BugMessage(pPlayer,"Could not find %s in list of clients, may be caused by a player that hasn't connected properly",STRING(pPlayer->v.netname));
	}
}

CClient *CClients :: GetClientByRepId(int iRepId)
{
	int i;
	CClient *pClient = NULL;

	for ( i = 0; i < MAX_PLAYERS; i ++ )
	{
		pClient = &m_Clients[i];

		if ( pClient->IsUsed() )
		{
			if ( pClient->GetPlayerRepId() == iRepId )
				return pClient;
		}
	}

	return NULL;
}

void CClients :: ClientDisconnected(CClient *pClient)
{
	int iPlayerRepId = pClient->GetPlayerRepId();
	int i;
	CBot *pBot;

	edict_t *pPlayer = pClient->GetPlayer();	

	int iPlayerIndex = -1;

	////////////////////
	// squad leave fix
	CBotSquad *pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pPlayer);

	// if leader of a squad, change the leader
	if ( pSquad )
	{
		gBotGlobals.m_Squads.ChangeLeader(pSquad);
	}
	////////////////////

	if ( pPlayer != NULL )
		iPlayerIndex = ENTINDEX(pPlayer) - 1;

	if ( pPlayer == gBotGlobals.m_pListenServerEdict )
		gBotGlobals.m_pListenServerEdict = NULL;

	// give a few seconds before adding more bots.
	gBotGlobals.m_fBotRejoinTime = gpGlobals->time + 2.0;

	BOOL RemoveGreeting = ( iPlayerIndex != -1 );
	
	if ( iPlayerRepId >= 0 )
	{
		for ( i = 0; i < MAX_PLAYERS; i ++ )
		{
			pBot = &gBotGlobals.m_Bots[i];
			
			if ( !pBot )
				continue;
			if ( !pBot->m_bIsUsed || !pBot->m_pEdict )
				continue;
			
			pBot->m_Profile.m_Rep.RemoveSaveRep(pBot->m_Profile.m_iProfileId,iPlayerRepId);
			
			if ( RemoveGreeting )
				pBot->m_iSaidGreetingsTo &= ~(1<<iPlayerIndex);
		}
	}

	pClient->RemoveTeleportVector();
	
	pClient->Init();
}
