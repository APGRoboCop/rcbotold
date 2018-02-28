/*

  SVENCOOP MONSTERS.H
  
	-Sniper
*/
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
/***
*
* Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
* 
* This product contains software technology licensed from Id
* Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
* All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/

#ifndef BASEMONSTER_H
#define BASEMONSTER_H

#define  bits_COND_CLIENT_PLAYERPUSH  ( bits_COND_SPECIAL1 )


struct WayPoint_t
{
	Vector	vecLocation;
	int		iType;
};

// Fix for compiling Win32 on VS 2013+ [APG]RoboCop[CL]
	typedef void Schedule_t;
	typedef void Task_t;
	typedef void CPathMonsterGoal;

#define MOVEGOAL_NONE				(0)
#define MOVEGOAL_TARGETENT			(bits_MF_TO_TARGETENT)
#define MOVEGOAL_ENEMY				(bits_MF_TO_ENEMY)
#define MOVEGOAL_PATHCORNER			(bits_MF_TO_PATHCORNER)
#define MOVEGOAL_LOCATION			(bits_MF_TO_LOCATION)
#define MOVEGOAL_NODE				(bits_MF_TO_NODE)

#define	TASKSTATUS_NEW				0			// Just started
#define TASKSTATUS_RUNNING			1			// Running task & movement
#define TASKSTATUS_RUNNING_MOVEMENT	2			// Just running movement
#define TASKSTATUS_RUNNING_TASK		3			// Just running task
#define TASKSTATUS_COMPLETE			4			// Completed, get next task

// these bits represent conditions that may befall the monster, of which some are allowed 
// to interrupt certain schedules. 
#define bits_COND_NO_AMMO_LOADED		( 1 << 0 ) // weapon needs to be reloaded!
#define	bits_COND_SEE_HATE				( 1 << 1 ) // see something that you hate
#define bits_COND_SEE_FEAR				( 1 << 2 ) // see something that you are afraid of
#define bits_COND_SEE_DISLIKE			( 1 << 3 ) // see something that you dislike
#define bits_COND_SEE_ENEMY				( 1 << 4 ) // target entity is in full view.
#define bits_COND_ENEMY_OCCLUDED		( 1 << 5 ) // target entity occluded by the world
#define bits_COND_SMELL_FOOD			( 1 << 6 )
#define bits_COND_ENEMY_TOOFAR			( 1 << 7 )
#define bits_COND_LIGHT_DAMAGE			( 1 << 8 ) // hurt a little 
#define bits_COND_HEAVY_DAMAGE			( 1 << 9 ) // hurt a lot
#define bits_COND_CAN_RANGE_ATTACK1		( 1 << 10)
#define bits_COND_CAN_MELEE_ATTACK1		( 1 << 11)
#define bits_COND_CAN_RANGE_ATTACK2		( 1 << 12)
#define bits_COND_CAN_MELEE_ATTACK2		( 1 << 13)
// #define bits_COND_CAN_RANGE_ATTACK3		( 1 << 14)
#define bits_COND_PROVOKED				( 1 << 15)
#define bits_COND_NEW_ENEMY				( 1 << 16)
#define bits_COND_HEAR_SOUND			( 1 << 17) // there is an interesting sound
#define bits_COND_SMELL					( 1 << 18) // there is an interesting scent
#define bits_COND_ENEMY_FACING_ME		( 1 << 19) // enemy is facing me
#define bits_COND_ENEMY_DEAD			( 1 << 20) // enemy was killed. If you get this in combat, try to find another enemy. If you get it in alert, victory dance.
#define bits_COND_SEE_CLIENT			( 1 << 21) // see a client
#define bits_COND_SEE_NEMESIS			( 1 << 22) // see my nemesis

#define bits_COND_SPECIAL1				( 1 << 28) // Defined by individual monster
#define bits_COND_SPECIAL2				( 1 << 29) // Defined by individual monster

#define bits_COND_TASK_FAILED			( 1 << 30)
#define bits_COND_SCHEDULE_DONE			( 1 << 31)

//
// generic Monster
//
class CBaseMonster : public CBaseToggle
{
private:
	int  m_afConditions;
	
public:
	
	typedef enum
	{
		SCRIPT_PLAYING = 0,  // Playing the sequence
			SCRIPT_WAIT,   // Waiting on everyone in the script to be ready
			SCRIPT_CLEANUP,   // Cancelling the script / cleaning up
			SCRIPT_WALK_TO_MARK,
			SCRIPT_RUN_TO_MARK,
	} SCRIPTSTATE;
	
	// these fields have been added in the process of reworking the state machine. (sjb)
	EHANDLE   m_hEnemy;   // the entity that the monster is fighting.
	EHANDLE   m_hTargetEnt;  // the entity that the monster is trying to reach
	EHANDLE   m_hTargetTank;
	
	EHANDLE   m_hOldEnemy[ MAX_OLD_ENEMIES ];
	Vector   m_vecOldEnemy[ MAX_OLD_ENEMIES ];
	
	float   m_flFieldOfView; // width of monster's field of view ( dot product )
	float   m_flWaitFinished; // if we're told to wait, this is the time that the wait will be over.
	float   m_flMoveWaitFinished;
	
	Activity  m_Activity;   // what the monster is doing (animation)
	Activity  m_IdealActivity; // monster should switch to this activity
	Activity  m_GaitActivity;  // gaitsequence -Sniper
	
	int    m_LastHitGroup;  // the last body region that took damage
	
	MONSTERSTATE m_MonsterState;  // monster's current state
	MONSTERSTATE m_IdealMonsterState;// monster should change to this state
	
	int    m_iTaskStatus;
	Schedule_t  *m_pSchedule;
	Schedule_t  *m_pScheduleSaved; //For land_on_ground schedules (remember last schedule and continue) -Sniper
	int    m_iScheduleIndex;
	
	WayPoint_t  m_Route[ ROUTE_SIZE ]; // Positions of movement
	int    m_movementGoal;   // Goal that defines route
	int    m_iRouteIndex;  // index into m_Route[]
	float   m_moveWaitTime;  // How long I should wait for something to move
	float   m_moveradius;  //Minimum radius. -Sniper
	Vector   m_vecMoveGoal;  // kept around for node graph moves, so we know our ultimate goal
	Activity  m_movementActivity; // When moving, set this activity
	
	int    m_iAudibleList;  // first index of a linked list of sounds that the monster can hear.
	int    m_afSoundTypes;
	
	Vector   m_vecLastPosition; // monster sometimes wants to return to where it started after an operation.
	int    m_iHintNode;  // this is the hint node that the monster is moving towards or performing active idle on.
	int    m_afMemory;
	int    m_bloodColor;  // color of blood particless
	int    m_iMaxHealth;  // keeps track of monster's maximum health value (for re-healing, etc)
	Vector   m_vecEnemyLKP;  // last known position of enemy. (enemy's origin)
	int    m_cAmmoLoaded;  // how much ammo is in the weapon (used to trigger reload anim sequences)
	int    m_afCapability;  // tells us what a monster can/can't do.
	float   m_flNextAttack;  // cannot attack again until this time
	
	int    m_bitsDamageType; // what types of damage has monster (player) taken
	BYTE   m_rgbTimeBasedDamage[CDMG_TIMEBASED]; // time based damage counters, decr. 1 per 2 seconds
	int    m_lastDamageAmount;// how much damage did monster (player) last take
	
	int    m_failSchedule;  // Schedule type to choose if current schedule fails
	
	float   m_flHungryTime;  // set this is a future time to stop the monster from eating for a while.
	
	float   m_flDistTooFar;  // if enemy farther away than this, bits_COND_ENEMY_TOOFAR set in CheckEnemy
	float   m_flDistLook;  // distance monster sees (Default 2048)
	
	int    m_iTriggerCondition;// for scripted AI, this is the condition that will cause the activation of the monster's TriggerTarget
	string_t  m_iszTriggerTarget; // name of target that should be fired.
	
	Vector   m_HackedGunPos;  // HACK until we can query end of gun
	
	// Scripted sequence Info
	SCRIPTSTATE  m_scriptState;  // internal cinematic state
	CCineMonster *m_pCine;
	CBaseEntity  *m_pCineBlocker; //Blah -Sniper
	
	virtual int  Save( CSave &save );
	virtual int  Restore( CRestore &restore );
	static   TYPEDESCRIPTION m_SaveData[];
	
	void KeyValue( KeyValueData *pkvd );
	
	// monster use function
	void EXPORT   MonsterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT   CorpseUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	
	// overrideable Monster member functions
	virtual int  BloodColor( void ) { return m_bloodColor; }
	virtual CBaseMonster *MyMonsterPointer( void ) { return this; }
	virtual void Look ( int iDistance );// basic sight function for monsters
	virtual void RunAI ( void );// core ai function! 
	void Listen ( void );
	
	//virtual BOOL IsAlive( void ) { return (pev->deadflag != DEAD_DEAD); }
	virtual BOOL IsAlive( void )
	{
		if( m_MonsterState != MONSTERSTATE_SCRIPT )
			return (pev->deadflag == DEAD_NO) && pev->health > 0;
		else
			return (pev->deadflag != DEAD_DEAD);
	}
	
	virtual BOOL ShouldFadeOnDeath( void );
	
	// Basic Monster AI functions
	virtual float ChangeYaw ( int speed );
	float VecToYaw( Vector vecDir );
	float FlYawDiff ( void );
	float DamageForce( float damage );
	
	// aone
	virtual void SetupFriendly(void) {}; // use this overridden to make some other friendly monsters
	// end aone
	
	// stuff written for new state machine
	virtual void MonsterThink( void );
	void EXPORT CallMonsterThink( void ) { this->MonsterThink(); }
	virtual int IRelationship ( CBaseEntity *pTarget );
	virtual void MonsterInit ( void );
	virtual void MonsterInitDead( void ); // Call after animation/pose is set up
	virtual void BecomeDead( int savedhealth );
	void EXPORT CorpseFallThink( void );
	
	void EXPORT MonsterInitThink ( void );
	virtual void StartMonster ( void );
	virtual CBaseEntity* BestVisibleEnemy ( void );// finds best visible enemy for attack
	virtual BOOL FInViewCone ( CBaseEntity *pEntity );// see if pEntity is in monster's view cone
	virtual BOOL FInViewCone ( Vector *pOrigin );// see if given location is in monster's view cone
	virtual void HandleAnimEvent( MonsterEvent_t *pEvent );
	
	virtual int CheckLocalMove ( const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist, Vector *pvecEndPosition );// check validity of a straight move through space
	
	virtual void Move( float flInterval = 0.1 );
	virtual void MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval );
	
	virtual BOOL ShouldAdvanceRoute( float flWaypointDist );
	
	virtual Activity GetStoppedActivity( void ) { return ACT_IDLE; }
	virtual void Stop( void ) { pev->gaitsequence = 0; m_IdealActivity = GetStoppedActivity(); } //-Sniper
	
	// This will stop animation until you call ResetSequenceInfo() at some point in the future
	inline void StopAnimation( void ) { pev->framerate = 0; }
	
	// these functions will survey conditions and set appropriate conditions bits for attack types.
	virtual BOOL CheckRangeAttack1( float flDot, float flDist );
	virtual BOOL CheckRangeAttack2( float flDot, float flDist );
	virtual BOOL CheckMeleeAttack1( float flDot, float flDist );
	virtual BOOL CheckMeleeAttack2( float flDot, float flDist );
	virtual BOOL CheckTankUsage( void );
	
	BOOL FHaveSchedule( void );
	BOOL FScheduleValid ( void );
	void ClearSchedule( void );
	BOOL FScheduleDone ( void );
	void ChangeSchedule ( Schedule_t *pNewSchedule );
	void NextScheduledTask ( void );
	Schedule_t *ScheduleInList( const char *pName, Schedule_t **pList, int listCount );
	
	virtual Schedule_t *ScheduleFromName( const char *pName );
	static Schedule_t *m_scheduleList[];
	
	void MaintainSchedule ( void );
	virtual void StartTask ( Task_t *pTask );
	virtual void RunTask ( Task_t *pTask );
	virtual Schedule_t *GetScheduleOfType( int Type );
	virtual Schedule_t *GetSchedule( void );
	virtual void ScheduleChange( void ) {}
	
	virtual int CanPlaySequence( BOOL fDisregardState, int interruptLevel );
	virtual int CanPlaySentence( BOOL fDisregardState ) { return IsAlive(); }
	virtual void PlaySentence( const char *pszSentence, float duration, float volume, float attenuation );
	virtual void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener );
	
	virtual void SentenceStop( void );
	
	Task_t *GetTask ( void );
	virtual MONSTERSTATE GetIdealState ( void );
	virtual void SetActivity ( Activity NewActivity );
	void SetSequenceByName ( char *szSequence );
	void SetState ( MONSTERSTATE State );
	virtual void ReportAIState( void );
	
	void CheckAttacks ( CBaseEntity *pTarget, float flDist );
	bool CheckAttacksFromPosition ( CBaseEntity *pTarget, float flDist, Vector vecPos );
	
	virtual int CheckEnemy ( CBaseEntity *pEnemy );
	void PushEnemy( CBaseEntity *pEnemy, Vector &vecLastKnownPos );
	BOOL PopEnemy( void );
	
	BOOL FGetNodeRoute ( Vector vecDest );
	BOOL TestFGetNodeRoute ( Vector vecDest ); //Movement testing stuff -Sniper
	
	inline void TaskComplete( void ) { if ( !HasConditions(bits_COND_TASK_FAILED) ) m_iTaskStatus = TASKSTATUS_COMPLETE; }
	void MovementComplete( void );
	inline void TaskFail( void ) { SetConditions(bits_COND_TASK_FAILED); }
	inline void TaskBegin( void ) { m_iTaskStatus = TASKSTATUS_RUNNING; }
	int TaskIsRunning( void );
	inline int TaskIsComplete( void ) { return (m_iTaskStatus == TASKSTATUS_COMPLETE); }
	inline int MovementIsComplete( void ) { return (m_movementGoal == MOVEGOAL_NONE); }
	
	int IScheduleFlags ( void );
	BOOL FRefreshRoute( float minradius = 0 );
	BOOL FRouteClear ( void );
	void RouteSimplify( CBaseEntity *pTargetEnt );
	void AdvanceRoute ( float distance );
	virtual BOOL FTriangulate ( const Vector &vecStart , const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex, Vector *pApexTwo, bool CoverPath );
	virtual BOOL FTriangulateExtension ( const Vector &vecStart , const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex );
	
	void MakeIdealYaw( Vector vecTarget );
	virtual void SetYawSpeed ( void ) { return; };// allows different yaw_speeds for each activity
	
	BOOL BuildRoute ( const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget, bool CoverPath );
	BOOL TestBuildRoute ( const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget ); //Movement testing stuff -Sniper
	
	virtual BOOL BuildNearestRoute ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
	int RouteClassify( int iMoveFlag );
	void InsertWaypoint ( Vector vecLocation, int afMoveFlags );
	
	BOOL FindLateralCover ( const Vector &vecThreat, const Vector &vecViewOffset );
	
	virtual BOOL FindCover ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
	virtual BOOL FindCoverGrenade ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
	virtual BOOL FindCoverDistance ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
	
	//Sniper
	virtual BOOL FindAttackPoint ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist );
	BOOL FindLateralAttackPoint ( const Vector &vecThreat, const Vector &vecViewOffset );
	BOOL FindLateralSafety ( const Vector &vecThreat, const Vector &vecViewOffset, float flMinDist, float flMaxDist );
	BOOL FindLateralSafetyGrenade ( const Vector &vecThreat, const Vector &vecViewOffset );
	//End Sniper
	
	virtual BOOL FValidateCover ( const Vector &vecCoverLocation ){return TRUE;};
	
	virtual float CoverRadius( void ) { return 784; } // Default cover radius
	
	virtual BOOL FCanCheckAttacks ( void );
	BOOL CheckAttacker( CBaseEntity *pAttacker );
	virtual void CheckAmmo( void ) { return; };
	virtual int IgnoreConditions ( void );
	
	inline void SetConditions( int iConditions ) { m_afConditions |= iConditions; }
	inline void ClearConditions( int iConditions ) { m_afConditions &= ~iConditions; }
	inline BOOL HasConditions( int iConditions ) { if ( m_afConditions & iConditions ) return TRUE; return FALSE; }
	inline BOOL HasAllConditions( int iConditions ) { if ( (m_afConditions & iConditions) == iConditions ) return TRUE; return FALSE; }
	
	virtual BOOL FValidateHintType( short sHint );
	int FindHintNode ( void );
	
	virtual BOOL FCanActiveIdle ( void );
	int FindRoamNode ( void ); // Monster Roaming! -Sniper
	
	CBaseEntity *FindTank ( void );
	
	void SetTurnActivity ( void );
	float FLSoundVolume ( CSound *pSound );
	
	BOOL MoveToNode( Activity movementAct, float waitTime, const Vector &goal, float minradius = 0);
	BOOL MoveToTarget( Activity movementAct, float waitTime, float minradius = 0);
	BOOL MoveToLocation( Activity movementAct, float waitTime, const Vector &goal, float minradius = 0);
	BOOL MoveToEnemy( Activity movementAct, float waitTime, float minradius = 0);
	BOOL MoveToCover( Activity movementAct, float waitTime, const Vector &goal, float minradius = 0); //-Sniper
	
	// Returns the time when the door will be open
	float OpenDoorAndWait( entvars_t *pevDoor );
	
	virtual int ISoundMask( void );
	virtual CSound* PBestSound ( void );
	virtual CSound* PBestScent ( void );
	virtual float HearingSensitivity( void ) { return 1.0; };
	
	BOOL FBecomeProne ( void );
	virtual void BarnacleVictimBitten( entvars_t *pevBarnacle );
	virtual void BarnacleVictimReleased( void );
	
	void SetEyePosition ( void );
	
	BOOL FShouldEat( void );// see if a monster is 'hungry'
	void Eat ( float flFullDuration );// make the monster 'full' for a while.
	
	CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
	BOOL FacingIdeal( void );
	
	BOOL FCheckAITrigger( void );// checks and, if necessary, fires the monster's trigger target.
	virtual BOOL NoFriendlyFire( void ) { return CBaseMonster::NoFriendlyFire(pev->origin); };
	virtual BOOL NoFriendlyFire( Vector vecPos ); //-Sniper
	virtual BOOL NoFriendlyFire( Vector vecPos, CBaseEntity *pTarget ); //-Sniper
	
	virtual BOOL FVisibleGunPos( CBaseEntity *pEntity, const Vector &vecPos ); //-Sniper
	virtual BOOL FInBulletCone ( CBaseEntity *pEntity, const Vector &vecPos ); //-Sniper
	
	BOOL BBoxFlat( void );
	
	// PrescheduleThink
	virtual void PrescheduleThink( void ) { return; };
	
	BOOL GetEnemy ( void );
	void MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	
	// combat functions
	float UpdateTarget ( entvars_t *pevTarget );
	virtual Activity GetDeathActivity ( void );
	Activity GetSmallFlinchActivity( void );
	virtual void Killed( entvars_t *pevAttacker, int iGib );
	virtual void GibMonster( void );
	BOOL   ShouldGibMonster( int iGib );
	virtual void   CallGibMonster( void );
	virtual BOOL HasHumanGibs( void );
	virtual BOOL HasAlienGibs( void );
	virtual void FadeMonster( void ); // Called instead of GibMonster() when gibs are disabled
	
	Vector ShootAtEnemy( const Vector &shootOrigin );
	virtual Vector BodyTarget( const Vector &posSrc )
	{
		//Shoot the Osprey at the engines, return closest -Sniper
		if( FClassnameIs(pev,"monster_osprey") || FClassnameIs(pev,"monster_blkop_osprey"))
		{
			Vector m_vecPosition1, m_vecPosition2;
			Vector new_angle;
			
			GetBonePosition( 4, m_vecPosition1, new_angle );
			GetBonePosition( 8, m_vecPosition2, new_angle );
			
			if( (m_vecPosition1 - posSrc).Length() > (m_vecPosition2 - posSrc).Length() )
				return m_vecPosition2;
			
			return m_vecPosition1;
		}
		
		return Center( ) * 0.75 + EyePosition() * 0.25;
	};  // position to shoot at
	
	virtual Vector  GetGunPosition( void );
	
	virtual int TakeHealth( float flHealth, int bitsDamageType, int health_cap = 0 );
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	int   DeadTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	
	void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	virtual int  IsMoving( void ) { return m_movementGoal != MOVEGOAL_NONE; }
	
	void RouteClear( void );
	void RouteNew( void );
	
	virtual void DeathSound ( void ) { return; };
	virtual void AlertSound ( void ) { return; };
	virtual void IdleSound ( void ) { return; };
	virtual void PainSound ( void ) { return; };
	
	virtual void StopFollowing( BOOL clearSchedule ) {}
	
	inline void Remember( int iMemory ) { m_afMemory |= iMemory; }
	inline void Forget( int iMemory ) { m_afMemory &= ~iMemory; }
	inline BOOL HasMemory( int iMemory ) { if ( m_afMemory & iMemory ) return TRUE; return FALSE; }
	inline BOOL HasAllMemories( int iMemory ) { if ( (m_afMemory & iMemory) == iMemory ) return TRUE; return FALSE; }
	
	BOOL ExitScriptedSequence( );
	BOOL CineCleanup( );
	
	CBaseEntity* DropItem ( char *pszItemName, const Vector &vecPos, const Vector &vecAng );// drop an item.
	
	void Precache(void){ CBaseEntity::Precache();};
	
	int m_fInitPlayerAlly;
	
	inline void UpdateShockGlow( void )
	{
		if ( !m_fGlowShellShock || pev->rendermode == kRenderNormal)
		{
			ShockGlowEffect( 1 ); //Turn glow on
			//0.65 seconds before stopping effect
			m_iShockTurnOffWait = gpGlobals->time + 0.65;
		}
	};
	
	inline void ShockGlowEffect( int m_iShockMode )
	{
		if( m_iShockMode == 1 ) //Turn on
		{ 
			pev->rendermode = kRenderNormal;
			pev->renderfx = kRenderFxGlowShell;
			pev->renderamt = 4;
			pev->rendercolor = Vector(85, 200, 255);
			m_fGlowShellShock = true;
		}
		else if ( m_iShockMode == 0 ) //Turn off
		{
			pev->rendermode = kRenderNormal;
			pev->renderfx = kRenderFxNone;
			pev->renderamt = 255;
			pev->rendercolor = Vector(0,0,0);
			m_fGlowShellShock = false; 
		}
	};
	
	BOOL m_fGlowShellShock;
	float m_iShockTurnOffWait;
	
	//Start Player Following -Sniper
	void EXPORT  FollowerPlayerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual BOOL IsPlayerFollowing( void );
	BOOL CanPlayerFollow( void );
	void LimitPlayerFollowers( CBaseEntity *pPlayer, int maxFollowers );
	void StartPlayerFollowing( CBaseEntity *pLeader );
	void StopPlayerFollowing( BOOL clearSchedule );
	int  m_fCanPlayerFollow;
	
	//Sound/Sentence played when monster starts following -Sniper
	virtual void UseSound ( void ) { return; };
	
	//Sound/Sentence played when monster stops following -Sniper
	virtual void UnUseSound ( void ) { return; };
	
	virtual void RideMonster( CBaseEntity *pOther );
	virtual void Touch( CBaseEntity *pOther );
	
	//End Player Following -Sniper
	bool m_fShouldLaunch; //Launch into the air! -Sniper
	
	Vector m_vecLastAlignmentOrigin;
	Vector m_vecLastAlignmentAngles;
	Vector m_vecLastSurfaceNormal;
	
	//These two functions aren't ever going to be monster-specific. -Sniper
	void AlignToGround( void ); // Ground alignment -Sniper
	void FloatInWater( void ); // Duh -Sniper
	
	int m_FormattedName; //For better name outputs. E.g. "Alien Slave" rather than "alien_slave" -Sniper
	
	virtual bool CheckScared( void );
	virtual void CheckCreatureDanger ( void );
	virtual void CheckFallDamage ( void );
	virtual void CheckRevival( void );
	void CheckSelectedTank ( void );
	void FindBestGoalEnt( void );
	
	void LadderMove( Vector vecDest, float speed );
	void LadderMoveDone( void );
	
	
	bool m_fCanFearCreatures;
	bool m_fFailSwitch;
	
	CSound *m_pGrenade;
	float m_flGrenadeRunStart;
	
	float m_flNextTankCheck;
	
	int  m_bloodchange;
	
	bool m_fShouldAlwaysActiveIdle;
	bool m_fShouldNeverActiveIdle;
	float m_flThinkDelay;
	
	float m_flFallVelocity; //Monsters now have fall damage. -Sniper
	float   m_flFallCheck;
	bool m_fCanCheckFallDamage;
	
	bool m_fNoInterruptions; // Blah
	float m_flMoveToRadius; //When in radius, script is done.
	
	float m_flNextMedicCall;
	virtual void MedicCallSound ( void ){ PainSound(); };
	
	CBaseEntity   m_pGuardEnt;    //Monster will guard this entity and turn down follow requests.
	string_t   m_iszGuardEntName;
	
	//CPathMonster  m_pLastPathMonster;  //Last Path_monster entity known
	//string_t   m_iszPathMonsterName;
	
	CPathMonsterGoal *m_pLastPathMonsterGoal; //Current info_monster_goal ent.
	string_t   m_iszPathMonsterGoalName;
	Vector m_vecFinalLadderDest;
};



#endif // BASEMONSTER_H
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
/*
#ifndef BASEMONSTER_H
#define BASEMONSTER_H

class CBaseMonster : public CBaseToggle
{
public:
	Activity			m_Activity;// what the monster is doing (animation)
	Activity			m_IdealActivity;// monster should switch to this activity
	int					m_LastHitGroup; // the last body region that took damage
	int					m_bitsDamageType;	// what types of damage has monster (player) taken
	BYTE				m_rgbTimeBasedDamage[CDMG_TIMEBASED];
	MONSTERSTATE		m_MonsterState;// monster's current state
	MONSTERSTATE		m_IdealMonsterState;// monster should change to this state
	int					m_afConditions;
	int					m_afMemory;
	float				m_flNextAttack;		// cannot attack again until this time
	EHANDLE				m_hEnemy;		 // the entity that the monster is fighting.
	EHANDLE				m_hTargetEnt;	 // the entity that the monster is trying to reach
	float				m_flFieldOfView;// width of monster's field of view ( dot product )
	int					m_bloodColor;		// color of blood particless
	Vector				m_HackedGunPos;	// HACK until we can query end of gun
	Vector				m_vecEnemyLKP;// last known position of enemy. (enemy's origin)


	void KeyValue( KeyValueData *pkvd );

	void MakeIdealYaw( Vector vecTarget );
	virtual float ChangeYaw ( int speed );
	virtual BOOL HasHumanGibs( void );
	virtual BOOL HasAlienGibs( void );
	virtual void FadeMonster( void );	// Called instead of GibMonster() when gibs are disabled
	virtual void GibMonster( void );
	virtual Activity GetDeathActivity ( void );
	Activity GetSmallFlinchActivity( void );
	virtual void BecomeDead( void );
	BOOL		 ShouldGibMonster( int iGib );
	void		 CallGibMonster( void );
	virtual BOOL	ShouldFadeOnDeath( void );
	BOOL FCheckAITrigger( void );// checks and, if necessary, fires the monster's trigger target. 
	virtual int IRelationship ( CBaseEntity *pTarget );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	int			DeadTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	float DamageForce( float damage );
	virtual void Killed( entvars_t *pevAttacker, int iGib );
	virtual void PainSound ( void ) { return; };

	void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );

	inline void	SetConditions( int iConditions ) { m_afConditions |= iConditions; }
	inline void	ClearConditions( int iConditions ) { m_afConditions &= ~iConditions; }
	inline BOOL HasConditions( int iConditions ) { if ( m_afConditions & iConditions ) return TRUE; return FALSE; }
	inline BOOL HasAllConditions( int iConditions ) { if ( (m_afConditions & iConditions) == iConditions ) return TRUE; return FALSE; }

	inline void	Remember( int iMemory ) { m_afMemory |= iMemory; }
	inline void	Forget( int iMemory ) { m_afMemory &= ~iMemory; }
	inline BOOL HasMemory( int iMemory ) { if ( m_afMemory & iMemory ) return TRUE; return FALSE; }
	inline BOOL HasAllMemories( int iMemory ) { if ( (m_afMemory & iMemory) == iMemory ) return TRUE; return FALSE; }

	// This will stop animation until you call ResetSequenceInfo() at some point in the future
	inline void StopAnimation( void ) { pev->framerate = 0; }

	virtual void ReportAIState( void );
	virtual void MonsterInitDead( void );	// Call after animation/pose is set up
	void EXPORT CorpseFallThink( void );

	virtual void Look ( int iDistance );// basic sight function for monsters
	virtual CBaseEntity* BestVisibleEnemy ( void );// finds best visible enemy for attack
	CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
	virtual BOOL FInViewCone ( CBaseEntity *pEntity );// see if pEntity is in monster's view cone
	virtual BOOL FInViewCone ( Vector *pOrigin );// see if given location is in monster's view cone
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir );
	virtual BOOL	IsAlive( void ) { return (pev->deadflag != DEAD_DEAD); }

};


#endif

*/
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
/*
#ifndef BASEMONSTER_H
#define BASEMONSTER_H

class CBaseMonster : public CBaseToggle
{
public:
	Activity			m_Activity;// what the monster is doing (animation)
	Activity			m_IdealActivity;// monster should switch to this activity
	int					m_LastHitGroup; // the last body region that took damage
	int					m_bitsDamageType;	// what types of damage has monster (player) taken
	BYTE				m_rgbTimeBasedDamage[CDMG_TIMEBASED];
	MONSTERSTATE		m_MonsterState;// monster's current state
	MONSTERSTATE		m_IdealMonsterState;// monster should change to this state
	int					m_afConditions;
	int					m_afMemory;
	float				m_flNextAttack;		// cannot attack again until this time
	EHANDLE				m_hEnemy;		 // the entity that the monster is fighting.
	EHANDLE				m_hTargetEnt;	 // the entity that the monster is trying to reach
	float				m_flFieldOfView;// width of monster's field of view ( dot product )
	int					m_bloodColor;		// color of blood particless
	Vector				m_HackedGunPos;	// HACK until we can query end of gun
	Vector				m_vecEnemyLKP;// last known position of enemy. (enemy's origin)


	void KeyValue( KeyValueData *pkvd );

	void MakeIdealYaw( Vector vecTarget );
	virtual float ChangeYaw ( int speed );
	virtual BOOL HasHumanGibs( void );
	virtual BOOL HasAlienGibs( void );
	virtual void FadeMonster( void );	// Called instead of GibMonster() when gibs are disabled
	virtual void GibMonster( void );
	virtual Activity GetDeathActivity ( void );
	Activity GetSmallFlinchActivity( void );
	virtual void BecomeDead( void );
	BOOL		 ShouldGibMonster( int iGib );
	void		 CallGibMonster( void );
	virtual BOOL	ShouldFadeOnDeath( void );
	BOOL FCheckAITrigger( void );// checks and, if necessary, fires the monster's trigger target. 
	virtual int IRelationship ( CBaseEntity *pTarget );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	int			DeadTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	float DamageForce( float damage );
	virtual void Killed( entvars_t *pevAttacker, int iGib );
	virtual void PainSound ( void ) { return; };

	void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );

	inline void	SetConditions( int iConditions ) { m_afConditions |= iConditions; }
	inline void	ClearConditions( int iConditions ) { m_afConditions &= ~iConditions; }
	inline BOOL HasConditions( int iConditions ) { if ( m_afConditions & iConditions ) return TRUE; return FALSE; }
	inline BOOL HasAllConditions( int iConditions ) { if ( (m_afConditions & iConditions) == iConditions ) return TRUE; return FALSE; }

	inline void	Remember( int iMemory ) { m_afMemory |= iMemory; }
	inline void	Forget( int iMemory ) { m_afMemory &= ~iMemory; }
	inline BOOL HasMemory( int iMemory ) { if ( m_afMemory & iMemory ) return TRUE; return FALSE; }
	inline BOOL HasAllMemories( int iMemory ) { if ( (m_afMemory & iMemory) == iMemory ) return TRUE; return FALSE; }

	// This will stop animation until you call ResetSequenceInfo() at some point in the future
	inline void StopAnimation( void ) { pev->framerate = 0; }

	virtual void ReportAIState( void );
	virtual void MonsterInitDead( void );	// Call after animation/pose is set up
	void EXPORT CorpseFallThink( void );

	virtual void Look ( int iDistance );// basic sight function for monsters
	virtual CBaseEntity* BestVisibleEnemy ( void );// finds best visible enemy for attack
	CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
	virtual BOOL FInViewCone ( CBaseEntity *pEntity );// see if pEntity is in monster's view cone
	virtual BOOL FInViewCone ( Vector *pOrigin );// see if given location is in monster's view cone
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir );
	virtual BOOL	IsAlive( void ) { return (pev->deadflag != DEAD_DEAD); }

};


#endif
*/