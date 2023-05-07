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


	void KeyValue( KeyValueData *pkvd ) override;

	void MakeIdealYaw( Vector vecTarget );
	virtual float ChangeYaw ( int speed );
	virtual BOOL HasHumanGibs();
	virtual BOOL HasAlienGibs();
	virtual void FadeMonster();	// Called instead of GibMonster() when gibs are disabled
	virtual void GibMonster();
	virtual Activity GetDeathActivity ();
	Activity GetSmallFlinchActivity();
	virtual void BecomeDead();
	BOOL		 ShouldGibMonster( int iGib );
	void		 CallGibMonster();
	virtual BOOL	ShouldFadeOnDeath();
	BOOL FCheckAITrigger();// checks and, if necessary, fires the monster's trigger target. 
	virtual int IRelationship ( CBaseEntity *pTarget );
	int TakeHealth( float flHealth, int bitsDamageType ) override;
	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	int			DeadTakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	float DamageForce( float damage );
	void Killed( entvars_t *pevAttacker, int iGib ) override;
	virtual void PainSound () { return; }

	void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );
	void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType );

	void	SetConditions( int iConditions ) { m_afConditions |= iConditions; }
	void	ClearConditions( int iConditions ) { m_afConditions &= ~iConditions; }
	BOOL HasConditions( int iConditions ) { if ( m_afConditions & iConditions ) return true; return false; }
	BOOL HasAllConditions( int iConditions ) { if ( (m_afConditions & iConditions) == iConditions ) return true; return false; }

	void	Remember( int iMemory ) { m_afMemory |= iMemory; }
	void	Forget( int iMemory ) { m_afMemory &= ~iMemory; }
	BOOL HasMemory( int iMemory ) { if ( m_afMemory & iMemory ) return true; return false; }
	BOOL HasAllMemories( int iMemory ) { if ( (m_afMemory & iMemory) == iMemory ) return true; return false; }

	// This will stop animation until you call ResetSequenceInfo() at some point in the future
	void StopAnimation() { pev->framerate = 0; }

	virtual void ReportAIState();
	virtual void MonsterInitDead();	// Call after animation/pose is set up
	void EXPORT CorpseFallThink();

	virtual void Look ( int iDistance );// basic sight function for monsters
	virtual CBaseEntity* BestVisibleEnemy ();// finds best visible enemy for attack
	CBaseEntity *CheckTraceHullAttack( float flDist, int iDamage, int iDmgType );
	virtual BOOL FInViewCone ( CBaseEntity *pEntity );// see if pEntity is in monster's view cone
	virtual BOOL FInViewCone ( Vector *pOrigin );// see if given location is in monster's view cone
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override;
	void MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir );
	BOOL	IsAlive() override { return (pev->deadflag != DEAD_DEAD); }

};


#endif
