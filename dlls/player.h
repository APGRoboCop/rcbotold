#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <cstdint>

#include "basemonster.h"
#include "bot.h"
#include "cbase.h"
#include "cdll_dll.h"
#include "const.h"
#include "eiface.h"
#include "progdefs.h"
#include "util.h"
#include "vector.h"

constexpr int PLAYER_FATAL_FALL_SPEED = 1024; // approx 60 feet
constexpr int PLAYER_MAX_SAFE_FALL_SPEED = 580; // approx 20 feet
constexpr float DAMAGE_FOR_FALL_SPEED = 100.0f / (PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED); // damage per unit per second.
constexpr int PLAYER_MIN_BOUNCE_SPEED = 200;
constexpr float PLAYER_FALL_PUNCH_THRESHHOLD = 350.0f; // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
enum PlayerPhysicsFlags : std::uint8_t
{
	PFLAG_ONLADDER = (1 << 0),
	PFLAG_ONSWING = (1 << 0),
	PFLAG_ONTRAIN = (1 << 1),
	PFLAG_ONBARNACLE = (1 << 2),
	PFLAG_DUCKING = (1 << 3),		// In the process of ducking, but totally squatted yet
	PFLAG_USING = (1 << 4),		// Using a continuous entity
	PFLAG_OBSERVER = (1 << 5),		// player is locked in stationary cam mode. Spectators can move, observers can't.
};

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
constexpr int CSUITPLAYLIST = 4;		// max of 4 suit sentences queued up at any time

constexpr bool SUIT_GROUP = true;
constexpr bool SUIT_SENTENCE = false;

constexpr int SUIT_REPEAT_OK = 0;
constexpr int SUIT_NEXT_IN_30SEC = 30;
constexpr int SUIT_NEXT_IN_1MIN = 60;
constexpr int SUIT_NEXT_IN_5MIN = 300;
constexpr int SUIT_NEXT_IN_10MIN = 600;
constexpr int SUIT_NEXT_IN_30MIN = 1800;
constexpr int SUIT_NEXT_IN_1HOUR = 3600;

constexpr int CSUITNOREPEAT = 32;

constexpr const char* SOUND_FLASHLIGHT_ON = "items/flashlight1.wav";
constexpr const char* SOUND_FLASHLIGHT_OFF = "items/flashlight1.wav";

constexpr int TEAM_NAME_LENGTH = 16;

enum class PlayerAnim : std::uint8_t
{
	Idle,
	Walk,
	Jump,
	SuperJump,
	Die,
	Attack1,
};

constexpr int MAX_ID_RANGE = 2048;
constexpr int SBAR_STRING_SIZE = 128;

enum class SbarData : std::uint8_t
{
	TargetName = 1,
	TargetHealth,
	TargetArmor,
	End,
};

constexpr float CHAT_INTERVAL = 1.0f;

class CBasePlayer : public CBaseMonster
{
public:
	int					random_seed;    // See that is shared between client & server for shared weapons code

	int					m_iPlayerSound;// the index of the sound list slot reserved for this player
	int					m_iTargetVolume;// ideal sound volume.
	int					m_iWeaponVolume;// how loud the player's weapon is right now.
	int					m_iExtraSoundTypes;// additional classification for this weapon's sound
	int					m_iWeaponFlash;// brightness of the weapon flash
	float				m_flStopExtraSoundTime;

	float				m_flFlashLightTime;	// Time until next battery draw/Recharge
	int					m_iFlashBattery;		// Flashlight Battery Draw

	int					m_afButtonLast;
	int					m_afButtonPressed;
	int					m_afButtonReleased;

	edict_t* m_pentSndLast;			// last sound entity to modify player room type
	float				m_flSndRoomtype;		// last roomtype set by sound entity
	float				m_flSndRange;			// dist from player to sound entity

	float				m_flFallVelocity;

	int					m_rgItems[MAX_ITEMS];
	int					m_fKnownItem;		// True when a new item needs to be added
	int					m_fNewAmmo;			// True when a new item has been added

	unsigned			m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	float				m_fNextSuicideTime; // the time after which the player can next use the suicide command

	// these are time-sensitive things that we keep track of
	float				m_flTimeStepSound;	// when the last stepping sound was made
	float				m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float				m_flSwimTime;		// how long player has been underwater
	float				m_flDuckTime;		// how long we've been ducking
	float				m_flWallJumpTime;	// how long until next walljump

	float				m_flSuitUpdate;					// when to play next suit update
	int					m_rgSuitPlayList[CSUITPLAYLIST];// next sentencenum to play for suit update
	int					m_iSuitPlayNext;				// next sentence slot for queue storage;
	int					m_rgiSuitNoRepeat[CSUITNOREPEAT];		// suit sentence no repeat list
	float				m_rgflSuitNoRepeatTime[CSUITNOREPEAT];	// how long to wait before allowing repeat
	int					m_lastDamageAmount;		// Last damage taken
	float				m_tbdPrev;				// Time-based damage timer

	float				m_flgeigerRange;		// range to nearest radiation source
	float				m_flgeigerDelay;		// delay per update of range msg to client
	int					m_igeigerRangePrev;
	int					m_iStepLeft;			// alternate left/right foot stepping sound
	char				m_szTextureName[CBTEXTURENAMEMAX];	// current texture name we're standing on
	char				m_chTextureType;		// current texture type

	int					m_idrowndmg;			// track drowning damage taken
	int					m_idrownrestored;		// track drowning damage restored

	int					m_bitsHUDDamage;		// Damage bits for the current fame. These get sent to
	// the hude via the DAMAGE message
	bool				m_fInitHUD;				// True when deferred HUD restart msg needs to be sent
	bool				m_fGameHUDInitialized;
	int					m_iTrain;				// Train control position
	bool				m_fWeapon;				// Set this to false to force a reset of the current weapon HUD info

	EHANDLE				m_pTank;				// the tank which the player is currently controlling,  NULL if no tank
	float				m_fDeadTime;			// the time at which the player died  (used in PlayerDeathThink())

	bool			m_fNoPlayerSound;	// a debugging feature. Player makes no sound if this is true.
	bool			m_fLongJump; // does this player have the longjump module?

	float       m_tSneaking;
	int			m_iUpdateTime;		// stores the number of frame ticks before sending HUD update messages
	int			m_iClientHealth;	// the health currently known by the client.  If this changes, send a new
	int			m_iClientBattery;	// the Battery currently known by the client.  If this changes, send a new
	int			m_iHideHUD;		// the players hud weapon info is to be hidden
	int			m_iClientHideHUD;
	int			m_iFOV;			// field of view
	int			m_iClientFOV;	// client's known FOV
	// usable player items
	CBasePlayerItem* m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem* m_pActiveItem;
	CBasePlayerItem* m_pClientActiveItem;  // client version of the active item
	CBasePlayerItem* m_pLastItem;
	// shared ammo slots
	int	m_rgAmmo[MAX_AMMO_SLOTS];
	int	m_rgAmmoLast[MAX_AMMO_SLOTS];

	Vector				m_vecAutoAim;
	bool				m_fOnTarget;
	int					m_iDeaths;
	float				m_iRespawnFrames;	// used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty;  // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames;// Custom clan logo frames for this player
	float	m_flNextDecalTime;// next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];

	void Spawn();
	void Pain();

	//	virtual void Think( void );
	void Jump();
	void Duck();
	void PreThink();
	void PostThink();
	Vector GetGunPosition();
	int TakeHealth(float flHealth, int bitsDamageType);
	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void Killed(entvars_t* pevAttacker, int iGib);

	Vector BodyTarget(const Vector& posSrc)
	{
		//int pev = 0;
		return Center() + pev->view_ofs * RANDOM_FLOAT(0.5f, 1.1f);
	} // position to shoot at

	void StartSneaking() { m_tSneaking = gpGlobals->time - 1; }
	void StopSneaking() { m_tSneaking = gpGlobals->time + 30; }
	bool IsSneaking() { return m_tSneaking <= gpGlobals->time; }
	bool IsAlive()
	{
		//bool pev = false;
		return pev->deadflag == DEAD_NO && pev->health > 0;
	}

	bool ShouldFadeOnDeath() override { return false; }
	bool IsPlayer() { return true; }			// Spectators should return false for this, they aren't "players" as far as game logic is concerned

	virtual bool IsNetClient() { return true; }		// Bots should return false for this, they can't receive NET messages
	// Spectators should return true for this
	const char* TeamID();

	int		Save(CSave& save);
	int		Restore(CRestore& restore);
	void RenewItems();
	void PackDeadPlayerItems();
	void RemoveAllItems(bool removeSuit);
	bool SwitchWeapon(CBasePlayerItem* pWeapon);

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	void UpdateClientData();

	static	TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	int				ObjectCaps() { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void			Precache();
	bool			IsOnLadder();
	bool			FlashlightIsOn();
	void			FlashlightTurnOn();
	void			FlashlightTurnOff();

	void UpdatePlayerSound();
	void DeathSound();

	int Classify();
	void SetAnimation(PlayerAnim playerAnim);
	void SetWeaponAnimType(const char* szExtention);
	char m_szAnimExtention[32];

	// custom player functions
	void ImpulseCommands();
	void CheatImpulseCommands(int iImpulse);

	void StartDeathCam();
	void StartObserver(Vector vecPosition, Vector vecViewAngle);

	void AddPoints(int score, bool bAllowNegativeScore);
	void AddPointsToTeam(int score, bool bAllowNegativeScore);
	bool AddPlayerItem(CBasePlayerItem* pItem);
	bool RemovePlayerItem(CBasePlayerItem* pItem);
	void DropPlayerItem(char* pszItemName);
	bool HasPlayerItem(CBasePlayerItem* pCheckItem);
	bool HasNamedPlayerItem(const char* pszItemName);
	bool HasWeapons();// do I have ANY weapons?
	void SelectPrevItem(int iItem);
	void SelectNextItem(int iItem);
	void SelectLastItem();
	void SelectItem(const char* pstr);
	void ItemPreFrame();
	void ItemPostFrame();
	void GiveNamedItem(const char* szName);
	void EnableControl(bool fControl);

	int  GiveAmmo(int iAmount, char* szName, int iMax);
	void SendAmmoUpdate();

	void WaterMove();
	void EXPORT PlayerDeathThink();
	void PlayerUse();

	void CheckSuitUpdate();
	void SetSuitUpdate(char* name, int fgroup, int iNoRepeat);
	void UpdateGeigerCounter();
	void CheckTimeBasedDamage();

	bool FBecomeProne();
	void BarnacleVictimBitten(entvars_t* pevBarnacle);
	void BarnacleVictimReleased();
	static int GetAmmoIndex(const char* psz);
	int AmmoInventory(int iAmmoIndex);
	int Illumination();

	void ResetAutoaim();
	Vector GetAutoaimVector(float flDelta);
	Vector AutoaimDeflection(Vector& vecSrc, float flDist, float flDelta);

	void ForceClientDllUpdate();  // Forces all client .dll specific data to be resent to client.

	void DeathMessage(entvars_t* pevKiller);

	void SetCustomDecalFrames(int nFrames);
	int GetCustomDecalFrames();

	void TabulateAmmo();
	//void CBasePlayer::TabulateAmmo(); // Bugged for older Metamod Addons? [APG]RoboCop[CL]

	float m_flStartCharge;
	float m_flAmmoStartCharge;
	float m_flPlayAftershock;
	float m_flNextAmmoBurn;// while charging, when to absorb another unit of player's ammo?

	//Player ID
	void InitStatusBar();
	void UpdateStatusBar();
	int m_izSBarState[static_cast<int>(SbarData::End)];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[SBAR_STRING_SIZE];
	char m_SbarString1[SBAR_STRING_SIZE];

	float m_flNextChatTime;
};

constexpr float AUTOAIM_2DEGREES = 0.0348994967025f;
constexpr float AUTOAIM_5DEGREES = 0.08715574274766f;
constexpr float AUTOAIM_8DEGREES = 0.1391731009601f;
constexpr float AUTOAIM_10DEGREES = 0.1736481776669f;

extern int	gmsgHudText;
extern bool gInitHUD;

#endif