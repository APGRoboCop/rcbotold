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
 // bot_const.h
 //
 //////////////////////////////////////////////////
 //
 // Constants and macros
 //

#ifndef __BOT_CONST_H__
#define __BOT_CONST_H__

constexpr int EXPLODE_RADIUS = 300;
constexpr int MAX_WAYPOINTS = 1024;

enum : std::uint8_t
{
	FL_ASTAR_OPEN = 1 << 0,
	FL_ASTAR_CLOSED = 1 << 1,
	FL_ASTAR_PARENT = 1 << 2,
	FL_ASTAR_HEURISTIC = 1 << 3
};

typedef enum : std::uint8_t
{
	TS_State_Normal = 0,
	TS_State_JumpDuck,
	TS_State_Stunt
}eTS_State;

class AStarNode
{
public:
	AStarNode();
	bool heuristicSet() const;

	void setHeuristic(float botDist, float goalDist, bool bIsTeleport = false);

	bool hasParent() const;

	bool isOpen() const;

	bool isClosed() const;

	void unClose();

	void close();

	void unOpen();

	void open();
	float getHeuristic() const;

	short int getParent() const;
	void setParent(short iWpt);

	bool operator()(const AStarNode* a, const AStarNode* b) const;

	bool operator<(const AStarNode* b) const;

	bool precedes(const AStarNode* b) const;

	short int m_iWaypoint;

	float m_fCost;
private:
	float m_fHeuristic;
	short int m_iParent;
	short int flags;
};

typedef enum : std::uint8_t
{
	BOT_CLIMB_NONE = 0,
	BOT_CLIMB_NOT_TOUCHING, // current waypoint is a ladder waypoint but not touching it yet.
	BOT_CLIMB_CLIMBING,		// currently climbing something
	BOT_CLIMB_NOT_FLYING,	// current waypoint is a fly waypoint but i'm not flying
	BOT_CLIMB_FLYING,		// currently flying
	BOT_CLIMB_WALL_STICKING, // current wall-sticking
	BOT_CLIMB_TRYING_WALL_STICK // trying to wall-stick
}eClimbType;

typedef enum : std::uint8_t
{
	BOT_CAN_NONE,
	BOT_CAN_BUILD_HIVE,
	BOT_CAN_BUILD_RESOURCE,
	BOT_CAN_REFILL_STRUCT, // already built but needs health
	BOT_CAN_BUILD_STRUCT,
	BOT_CAN_HEAL,
	BOT_CAN_BUILD_SENTRY,
	BOT_CAN_UPGRADE_SENTRY,
	BOT_CAN_BUILD_TELE_ENTRANCE,
	BOT_CAN_BUILD_TELE_EXIT,
	BOT_CAN_BUILD_DISPENSER,
	BOT_CAN_GET_METAL,
	BOT_CAN_REPAIR_TELE_EXIT
}eCanDoStuff;

// macros

#ifdef __linux__
#define NS_PLAYER_EXPERIENCE_OFFSET 1554
#define NS_PLAYER_POINTS_OFFSET (NS_PLAYER_EXPERIENCE_OFFSET + 2)
#else
#define NS_PLAYER_EXPERIENCE_OFFSET 1579
#define NS_PLAYER_POINTS_OFFSET (NS_PLAYER_EXPERIENCE_OFFSET + 2)
#endif

//#define NS_GET_PLAYER_POINTS(player) (int)((char*)(player)+NS_PLAYER_POINTS_OFFSET)
#define NS_GET_PLAYER_POINTS(player) *(int*)(((int)(player)->pvPrivateData)+NS_PLAYER_POINTS_OFFSET)

/////////////
// SVENCOOP MONSTERS
enum : std::int8_t
{
	R_AL = -2,	// (ALLY) pals. Good alternative to R_NO when applicable.
	R_FR = -1,	// (FEAR)will run
	R_NO = 0,	// (NO RELATIONSHIP) disregard
	R_DL = 1,	// (DISLIKE) will attack
	R_HT = 2,	// (HATE)will attack this character instead of any visible DISLIKEd characters
	R_NM = 3	// (NEMESIS)  A monster Will ALWAYS attack its nemsis, no matter what
};

// these bits represent the monster's memory
enum : std::uint32_t
{
	MEMORY_CLEAR = 0,
	bits_MEMORY_PROVOKED = 1u << 0,    // right now only used for houndeyes.
	bits_MEMORY_INCOVER = 1u << 1,     // monster knows it is in a covered position.
	bits_MEMORY_SUSPICIOUS = 1u << 2,  // Ally is suspicious of the player, and will move to provoked more easily
	bits_MEMORY_PATH_FINISHED = 1u << 3,   // Finished monster path (just used by big momma for now)
	bits_MEMORY_ON_PATH = 1u << 4,     // Moving on a path
	bits_MEMORY_MOVE_FAILED = 1u << 5, // Movement has already failed
	bits_MEMORY_FLINCHED = 1u << 6,    // Has already flinched
	bits_MEMORY_KILLED = 1u << 7,      // HACKHACK -- remember that I've already called my Killed()
	bits_MEMORY_CUSTOM4 = 1u << 28,    // Monster-specific memory
	bits_MEMORY_CUSTOM3 = 1u << 29,    // Monster-specific memory
	bits_MEMORY_CUSTOM2 = 1u << 30,    // Monster-specific memory
	bits_MEMORY_CUSTOM1 = 1u << 31     // Monster-specific memory
};

constexpr int MAX_REMEMBER_POSITIONS = 8;

//////////////////
// CLIENTS STEAM IDS
constexpr int STEAM_ID_LEN = 32;

///////////////////////////////////////////////////////////////////////
// BOT CONTANTS
// (constants that bot structures may depend on below)

constexpr float MAX_JUMP_HEIGHT = 45.0f;

constexpr int MAX_TEAMS = 4;
constexpr int BOT_MIN_SOUND_DIST = 128;

constexpr int BOT_MIN_MSECVAL = 1;
constexpr int BOT_MAX_MSECVAL = 60;

constexpr float BOT_DEF_MSECDELTIME = 0.5f;

constexpr int BOT_MAX_PASSWORD_LEN = 16;

// start messages
// Science and Industry
enum : std::uint8_t
{
	MSG_SI_IDLE = 1,
	MSG_SI_TEAM_SELECT = 2,
	MSG_SI_MODEL_SELECT = 3
};

////////////////////////////////
// NATURAL SELECTION TEAMS
enum : std::uint8_t
{
	TEAM_NONE = 0,
	TEAM_MARINE = 1,
	TEAM_ALIEN = 2,
	TEAM_AUTOTEAM = 5
};

enum : std::uint8_t
{
	TYPE_UNKNOWN = 8
};

enum : std::uint8_t
{
	TYPE_SOLDIER = 0,
	TYPE_HEAVY = 1,
	TYPE_SKULK = 2,
	TYPE_GORGE = 3,
	TYPE_LERK = 4,
	TYPE_FADE = 5,
	TYPE_ONOS = 6,
	TYPE_EVOLVING = 7
};

#define BOT_USERS_FILE "bot_users.ini"
//#define BOT_PASSWORD_KEY "rcbot_pass"
// Bot Sounds
// different sound types the bot might hear
typedef enum : std::uint8_t
{
	SOUND_UNKNOWN = 0,
	SOUND_PLAYER,
	SOUND_DOOR,
	SOUND_WEAPON,
	SOUND_FOLLOW,
	SOUND_NEEDBACKUP,
	SOUND_NEEDHEALTH,
	SOUND_NEEDAMMO,
	SOUND_COVERING,
	SOUND_TAUNT,
	SOUND_INPOSITION,
	SOUND_INCOMING,
	SOUND_MOVEOUT,
	SOUND_ALLCLEAR,
	SOUND_HUD,
	SOUND_BUTTON,
	SOUND_TURRETS,
	SOUND_MISC,
	SOUND_ATTACK,
	SOUND_BUILDINGHERE,
	SOUND_TAKE_COVER,
	SOUND_PLAYER_PAIN
}eSoundType;
// Bot Hear Prototypes

///////////////////////////////////////////////////////////////////////
// NATURAL SELECTION
// (mod defined structures : Flayra@overmind.org)

typedef enum : std::uint8_t
{
	// Special or misc. actions
	MESSAGE_NULL = 0,

	// Use an item or ability (these are currently forced to be less than 10 because it's
	// used as an index into some weapons/equipment array <sigh>)
	WEAPON_NEXT = 1,
	WEAPON_RELOAD = 2,
	WEAPON_DROP = 3,

	// Admin commands
	ADMIN_READYROOM = 5,
	ADMIN_VOTEDOWNCOMMANDER = 6,

	// Talk to your teammates
	SAYING_1 = 7,
	SAYING_2 = 8,
	SAYING_3 = 9,
	SAYING_4 = 10,
	SAYING_5 = 11,
	SAYING_6 = 12,
	SAYING_7 = 13,
	SAYING_8 = 14,
	SAYING_9 = 15,

	// Chat
	COMM_CHAT_PUBLIC = 16,
	COMM_CHAT_TEAM = 17,
	COMM_CHAT_NEARBY = 18,

	// Research
	RESEARCH_ARMOR_ONE = 20,
	RESEARCH_ARMOR_TWO = 21,
	RESEARCH_ARMOR_THREE = 22,
	RESEARCH_WEAPONS_ONE = 23,
	RESEARCH_WEAPONS_TWO = 24,
	RESEARCH_WEAPONS_THREE = 25,
	TURRET_FACTORY_UPGRADE = 26,
	BUILD_CAT = 27,
	RESEARCH_JETPACKS = 28,
	RESEARCH_HEAVYARMOR = 29,
	RESEARCH_DISTRESSBEACON = 30,
	RESEARCH_RESUPPLY = 31, // NS Combat
	MESSAGE_CANCEL = 32,
	RESEARCH_MOTIONTRACK = 33,
	RESEARCH_PHASETECH = 34,
	RESOURCE_UPGRADE = 35,
	RESEARCH_ELECTRICAL = 36,
	RESEARCH_GRENADES = 37,

	// Buildings
	BUILD_HEAVY = 38,
	BUILD_JETPACK = 39,
	BUILD_INFANTRYPORTAL = 40,
	BUILD_RESOURCES = 41,
	BUILD_TURRET_FACTORY = 43,
	BUILD_ARMSLAB = 45,
	BUILD_PROTOTYPE_LAB = 46,
	RESEARCH_CATALYSTS = 47,
	BUILD_ARMORY = 48,
	ARMORY_UPGRADE = 49,
	BUILD_NUKE_PLANT = 50,
	BUILD_OBSERVATORY = 51,
	RESEARCH_HEALTH = 52,

	BUILD_SCAN = 53,
	BUILD_PHASEGATE = 55,
	BUILD_TURRET = 56,
	BUILD_SIEGE = 57,
	BUILD_COMMANDSTATION = 58,

	// Weapons and items
	BUILD_HEALTH = 59,
	BUILD_AMMO = 60,
	BUILD_MINES = 61,
	BUILD_WELDER = 62,
	BUILD_UNUSED = 63,
	BUILD_SHOTGUN = 64,
	BUILD_HMG = 65,
	BUILD_GRENADE_GUN = 66,
	BUILD_NUKE = 67,

	// Misc
	BUILD_RECYCLE = 69,

	GROUP_CREATE_1 = 70,
	GROUP_CREATE_2 = 71,
	GROUP_CREATE_3 = 72,
	GROUP_CREATE_4 = 73,
	GROUP_CREATE_5 = 74,

	GROUP_SELECT_1 = 75,
	GROUP_SELECT_2 = 76,
	GROUP_SELECT_3 = 77,
	GROUP_SELECT_4 = 78,
	GROUP_SELECT_5 = 79,

	// Orders
	ORDER_REQUEST = 80,
	ORDER_ACK = 81,

	// Commander mode
	COMMANDER_MOUSECOORD = 82,
	COMMANDER_MOVETO = 83,
	COMMANDER_SCROLL = 84,
	COMMANDER_DEFAULTORDER = 104,
	COMMANDER_SELECTALL = 105,
	COMMANDER_REMOVESELECTION = 96,

	// Sub-menus
	MENU_BUILD = 85,
	MENU_BUILD_ADVANCED = 86,
	MENU_ASSIST = 87,
	MENU_EQUIP = 88,
	MENU_SOLDIER_FACE = 89,

	// These messages aren't sent as impulses, but are used internally in the shared code.  They can be removed if necessary with some work.
	ALIEN_BUILD_RESOURCES = 90,
	ALIEN_BUILD_OFFENSE_CHAMBER = 91,
	ALIEN_BUILD_DEFENSE_CHAMBER = 92,
	ALIEN_BUILD_SENSORY_CHAMBER = 93,
	ALIEN_BUILD_MOVEMENT_CHAMBER = 94,
	ALIEN_BUILD_HIVE = 95,

	IMPULSE_FLASHLIGHT = 100,
	IMPULSE_SPRAYPAINT = 201,
	IMPULSE_DEMORECORD = 204,

	// Alien menu items (there are assumptions that these are contiguous)
	ALIEN_EVOLUTION_ONE = 101,		// Carapace
	ALIEN_EVOLUTION_TWO = 102,		// Regeneration
	ALIEN_EVOLUTION_THREE = 103,	// Redemption

	ALIEN_EVOLUTION_SEVEN = 107,	// Celerity
	ALIEN_EVOLUTION_EIGHT = 108,	// Adrenaline
	ALIEN_EVOLUTION_NINE = 109,		// Silence
	ALIEN_EVOLUTION_TEN = 110,		// Cloaking
	ALIEN_EVOLUTION_ELEVEN = 111,	// Pheromones
	ALIEN_EVOLUTION_TWELVE = 112,	// Scent of fear

	// Alien lifeforms
	ALIEN_LIFEFORM_ONE = 113,
	ALIEN_LIFEFORM_TWO = 114,
	ALIEN_LIFEFORM_THREE = 115,
	ALIEN_LIFEFORM_FOUR = 116,
	ALIEN_LIFEFORM_FIVE = 117,

	// Unlock alien abilities in Combat mode
	BUILD_RESUPPLY = 31,
	ALIEN_HIVE_TWO_UNLOCK = 118,
	ALIEN_HIVE_THREE_UNLOCK = 126,
	COMBAT_TIER2_UNLOCK = 31,
	COMBAT_TIER3_UNLOCK = 54,

	// Alien abilities
	ALIEN_ABILITY_LEAP = 119,
	ALIEN_ABILITY_BLINK = 120,
	ALIEN_ABILITY_CHARGE = 122,

	COMMANDER_NEXTIDLE = 123,
	COMMANDER_NEXTAMMO = 124,
	COMMANDER_NEXTHEALTH = 125,

	MESSAGE_LAST = 127

	// NOTE: If this gets larger then a byte, AvHTechNode will have to change it's networking, possibly other code too
	// NOTE: When changing any of these values, make sure to update titles.txt, skill.cfg and dlls\game.cpp, and tech*s.spr
} AvHMessageID;

//-----------------------------------------------------------------------------
// Possible Research Options - Science and Industry
//-----------------------------------------------------------------------------
enum ResearchGoal : std::uint8_t
{
	RESEARCH_NULL = 0,			// starting research option (sets up research), it's also the end-game research
	RESEARCH_ARMOR_25,			// level one armor upgrade
	RESEARCH_ARMOR_50,			// level two armor upgrade
	RESEARCH_A_REGEN,			// player armor regeneration
	RESEARCH_ARMOR_75,			// level three armor upgrade
	RESEARCH_A_REGEN2,			// advanced player armor regeneration
	RESEARCH_ARMOR_100,			// level four armor upgrade
	RESEARCH_H_REGEN,			// player health regeneration
	RESEARCH_H_REGEN2,			// advanced player health regeneration
	RESEARCH_LEGS_1,			// faster player movement
	RESEARCH_AMMO_REPLICATE,	// ammo replication
	RESEARCH_STRENGTH,			// bionic strength
	RESEARCH_SUPERJUMP,			// super jump module
	RESEARCH_OPTICS,			// optical implants
	RESEARCH_LEGS_2,			// long jump power
	RESEARCH_AMMO_REPLICATE2,	// advanced ammo replication
	RESEARCH_STRENGTH2,			// enhanced strength
	RESEARCH_SHOTGUN,			// weapon_shotgun
	RESEARCH_TOMMYGUN,			// weapon_tommygun
	RESEARCH_SNUZI,				// weapon_snUZI
	RESEARCH_GRENADE,			// weapon_grenade
	RESEARCH_FEV,				// flesh eating virus
	RESEARCH_CROSSBOW,			// weapon_crossbow
	RESEARCH_GAUSS,				// weapon_gauss
	RESEARCH_ROCKETPISTOL,		// weapon_rocketpistol
	RESEARCH_BRIEFCASE,			// weapon_briefcase
	RESEARCH_RADIO,				// weapon_transistor
	RESEARCH_CLOAKING,			// cloaking device
	RESEARCH_MINDRAY,			// weapon_mindray
	RESEARCH_EMPCANNON,			// weapon_empcannon
	RESEARCH_VOMIT,				// weapon_vomit
	RESEARCH_SATCHEL,			// weapon_satchel
	RESEARCH_TRIPMINE,			// weapon_tripmine
	RESEARCH_COFFEE,			// quicker wake up for scientists and higher max eff.
	RESEARCH_MANUFACTURING,		// cash reward, incentive for scis (cheaper and quicker replacemnts)
	//RESEARCH_PRODUCTION,		// shortend weapon/ammo spawn times
	RESEARCH_CLONING,			// cheaper/quicker player respawn
	RESEARCH_COLT,				// weapon_colt

	NUM_RESEARCH_OPTIONS		// leave this as the last item in the list
};

// -----------------------------------------------------------------------------
// notice_type
//
// Notice types are used with the "Notice" message. Send one of these types as a
// BYTE to the player
// -----------------------------------------------------------------------------
enum notice_type : std::uint8_t
{
	Notice_None = 0,

	Notice_Kill_Def,
	Notice_Kill_DefNoisy,
	Notice_Kill_DefRecruiter,
	Notice_Kill_Recruiter,
	Notice_Kill_RecruiterResource,
	Notice_Sci_Recruit,
	Notice_Sci_RecruitAssist,
	Notice_Sci_OtherRecruit,
	Notice_Resource_Capture,
	Notice_Resource_OtherCapture,
	Notice_Research_CanSalvage,
	Notice_Research_Salvage,
	Notice_Research_OtherSalvage,
	Notice_Sabotage,
	Notice_Kill_Death,
	Notice_Sci_NewHire,
	Notice_Sci_Kill,
	Notice_Kill_TeamKiller,
	Notice_Sabotage_Opponent,
	Notice_Sabotage_Replace,
	Notice_Award_LlamaOn,
	Notice_Award_LlamaOff,
	Notice_Award_EotMOn,
	Notice_Award_EotMOff,
	Notice_Research_Completed,
	Notice_Research_Start,
	Notice_Research_Future,
	Notice_Research_Espionage,
	Notice_Sci_Missing,
	Notice_Research_Vote,
	Notice_Research_VoteResults,
	Notice_Sci_Defend,
	Notice_Sci_Attack,
	Notice_Sci_AttackStill,
	Notice_Sci_Dead,
	Notice_NoTripmine,
	Notice_NoSatchel,
	Notice_TeamButton,
	Notice_ChangeModel,
	Notice_Research_Castvote,
	Notice_Research_End,
	Notice_Startmatch,

	NUM_NOTICETYPES		// leave this one last
};

typedef struct
{
	bool	researched;
	bool	canidate;
	bool	stolen;
	bool	disabled;
} bot_research_t;

////////////////
// NS Stuff
// 200 units away max from building (2d distance)
constexpr int MAX_BUILD_RANGE = 800;

constexpr int NS_HIVE_RESOURCES = 40;
constexpr int NS_ONOS_RESOURCES = 75;
constexpr int NS_FADE_RESOURCES = 50;
constexpr int NS_LERK_RESOURCES = 30;
constexpr int NS_GORGE_RESOURCES = 10;
constexpr int NS_SKULK_RESOURCES = 2;
constexpr int NS_RESOURCE_TOWER_RESOURCES = 15;
constexpr int NS_DEFENSE_CHAMBER_RESOURCES = 10;
constexpr int NS_OFFENSE_CHAMBER_RESOURCES = 10;
constexpr int NS_MOVEMENT_CHAMBER_RESOURCES = 10;
constexpr int NS_SENSORY_CHAMBER_RESOURCES = 10;

typedef enum : std::uint8_t
{
	PLAYERCLASS_NONE = 0,
	PLAYERCLASS_ALIVE_MARINE,
	PLAYERCLASS_ALIVE_JETPACK_MARINE,
	PLAYERCLASS_ALIVE_HEAVY_MARINE,
	PLAYERCLASS_ALIVE_LEVEL1,
	PLAYERCLASS_ALIVE_LEVEL2,
	PLAYERCLASS_ALIVE_LEVEL3,
	PLAYERCLASS_ALIVE_LEVEL4,
	PLAYERCLASS_ALIVE_LEVEL5,
	PLAYERCLASS_ALIVE_DIGESTING,
	PLAYERCLASS_ALIVE_GESTATING,
	PLAYERCLASS_DEAD_MARINE,
	PLAYERCLASS_DEAD_ALIEN,
	PLAYERCLASS_COMMANDER,
	PLAYERCLASS_REINFORCING,
	PLAYERCLASS_SPECTATOR,
	PLAYERCLASS_REINFORCINGCOMPLETE
} AvHPlayerClass;

// Only one of these allowed per entity, stored in pev->iuser3.
typedef enum : std::uint8_t
{
	AVH_USER3_NONE = 0,
	AVH_USER3_MARINE_PLAYER,
	AVH_USER3_COMMANDER_PLAYER,
	AVH_USER3_ALIEN_PLAYER1,
	AVH_USER3_ALIEN_PLAYER2,
	AVH_USER3_ALIEN_PLAYER3,
	AVH_USER3_ALIEN_PLAYER4,
	AVH_USER3_ALIEN_PLAYER5,
	AVH_USER3_ALIEN_EMBRYO,
	AVH_USER3_SPAWN_TEAMA,
	AVH_USER3_SPAWN_TEAMB,
	AVH_USER3_PARTICLE_ON,				// only valid for AvHParticleEntity: entindex as int in fuser1, template index stored in fuser2
	AVH_USER3_PARTICLE_OFF,				// only valid for AvHParticleEntity: particle system handle in fuser1
	AVH_USER3_WELD,						// float progress (0 - 100) stored in fuser1
	AVH_USER3_ALPHA,					// fuser1 indicates how much alpha this entity toggles to in commander mode, fuser2 for players
	AVH_USER3_MARINEITEM,				// Something a friendly marine can pick up
	AVH_USER3_WAYPOINT,
	AVH_USER3_HIVE,
	AVH_USER3_NOBUILD,
	AVH_USER3_USEABLE,
	AVH_USER3_AUDIO_ON,
	AVH_USER3_AUDIO_OFF,
	AVH_USER3_FUNC_RESOURCE,
	AVH_USER3_COMMANDER_STATION,
	AVH_USER3_TURRET_FACTORY,
	AVH_USER3_ARMORY,
	AVH_USER3_ADVANCED_ARMORY,
	AVH_USER3_ARMSLAB,
	AVH_USER3_PROTOTYPE_LAB,
	AVH_USER3_OBSERVATORY,
	AVH_USER3_CHEMLAB,
	AVH_USER3_MEDLAB,
	AVH_USER3_NUKEPLANT,
	AVH_USER3_TURRET,
	AVH_USER3_SIEGETURRET,
	AVH_USER3_RESTOWER,
	AVH_USER3_PLACEHOLDER,
	AVH_USER3_INFANTRYPORTAL,
	AVH_USER3_NUKE,
	AVH_USER3_BREAKABLE,
	AVH_USER3_UMBRA,
	AVH_USER3_PHASEGATE,
	AVH_USER3_DEFENSE_CHAMBER,
	AVH_USER3_MOVEMENT_CHAMBER,
	AVH_USER3_OFFENSE_CHAMBER,
	AVH_USER3_SENSORY_CHAMBER,
	AVH_USER3_ALIENRESTOWER,
	AVH_USER3_HEAVY,
	AVH_USER3_JETPACK,
	AVH_USER3_ADVANCED_TURRET_FACTORY,
	AVH_USER3_SPAWN_READYROOM,
	AVH_USER3_CLIENT_COMMAND,
	AVH_USER3_FUNC_ILLUSIONARY,
	AVH_USER3_MENU_BUILD,
	AVH_USER3_MENU_BUILD_ADVANCED,
	AVH_USER3_MENU_ASSIST,
	AVH_USER3_MENU_EQUIP,
	AVH_USER3_MINE,
	AVH_USER3_UNKNOWN,
	AVH_USER3_MAX
} AvHUser3;

typedef enum : std::uint8_t
{
	WEAPON_ON_TARGET = 0x01,
	WEAPON_IS_CURRENT = 0x02,
	WEAPON_IS_ENABLED = 0x04
} CurWeaponStateFlags;

typedef enum : std::uint8_t
{
	BALANCE_ACTION_INSERT_INT = 0,
	BALANCE_ACTION_INSERT_FLOAT = 1,
	BALANCE_ACTION_INSERT_STRING = 2,
	BALANCE_ACTION_REMOVE = 3,
	BALANCE_ACTION_CLEAR = 4,
	BALANCE_ACTION_NOTIFY_PENDING = 5,
	BALANCE_ACTION_NOTIFY_FINISHED = 6
} BalanceMessageAction;

// AvHSpecials, only one per entity, stored in pev->iuser4.
// Stored in iuser4.  Some entities don't use these values, but most do.  The ones that don't include:
// AVH_USER3_AUDIO_OFF
// AVH_USER3_AUDIO_ON
typedef enum
{
	MASK_NONE = 0x00000000,
	MASK_VIS_SIGHTED = 0x00000001,	// This means this is an entity that can be seen by at least one member of the opposing team.  Assumes commanders can never be seen.
	MASK_VIS_DETECTED = 0x00000002,	// This entity has been detected by the other team but isn't currently seen
	MASK_BUILDABLE = 0x00000004,	// This entity is buildable
	MASK_UPGRADE_1 = 0x00000008,	// Marine weapons 1, armor, marine basebuildable slot #0
	MASK_UPGRADE_2 = 0x00000010,	// Marine weapons 2, regen, marine basebuildable slot #1
	MASK_UPGRADE_3 = 0x00000020,	// Marine weapons 3, redemption, marine basebuildable slot #2
	MASK_UPGRADE_4 = 0x00000040,	// Marine armor 1, speed, marine basebuildable slot #3
	MASK_UPGRADE_5 = 0x00000080,	// Marine armor 2, adrenaline, marine basebuildable slot #4
	MASK_UPGRADE_6 = 0x00000100,	// Marine armor 3, silence, marine basebuildable slot #5
	MASK_UPGRADE_7 = 0x00000200,	// Marine jetpacks, Cloaking, marine basebuildable slot #6
	MASK_UPGRADE_8 = 0x00000400,	// Pheromone, motion-tracking, marine basebuildable slot #7
	MASK_UPGRADE_9 = 0x00000800,	// Scent of fear, exoskeleton
	MASK_UPGRADE_10 = 0x00001000,	// Defensive level 2, power armor
	MASK_UPGRADE_11 = 0x00002000,	// Defensive level 3, electrical defense
	MASK_UPGRADE_12 = 0x00004000,	// Movement level 2,
	MASK_UPGRADE_13 = 0x00008000,	// Movement level 3, marine heavy armor
	MASK_UPGRADE_14 = 0x00010000,	// Sensory level 2
	MASK_UPGRADE_15 = 0x00020000,	// Sensory level 3
	MASK_ALIEN_MOVEMENT = 0x00040000,	// Onos is charging
	MASK_WALLSTICKING = 0x00080000,	// Flag for wall-sticking
	MASK_BUFFED = 0x00100000,	// Alien is in range of active primal scream, or marine is under effects of catalyst
	MASK_UMBRA = 0x00200000,
	MASK_DIGESTING = 0x00400000,	// When set on a visible player, player is digesting.  When set on invisible player, player is being digested
	MASK_RECYCLING = 0x00800000,
	MASK_TOPDOWN = 0x01000000,
	MASK_PLAYER_STUNNED = 0x02000000,	// Player has been stunned by stomp
	MASK_ENSNARED = 0x04000000,
	MASK_ALIEN_EMBRYO = 0x08000000,
	MASK_SELECTABLE = 0x10000000,
	MASK_PARASITED = 0x20000000,
	MASK_SENSORY_NEARBY = 0x40000000
} AvHUpgradeMask;

typedef enum : std::uint8_t
{
	ALIEN_UPGRADE_CATEGORY_INVALID = 0,
	ALIEN_UPGRADE_CATEGORY_DEFENSE,
	ALIEN_UPGRADE_CATEGORY_OFFENSE,
	ALIEN_UPGRADE_CATEGORY_MOVEMENT,
	ALIEN_UPGRADE_CATEGORY_SENSORY,
	ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE,
} AvHAlienUpgradeCategory;

typedef enum : std::uint8_t
{
	ORDERTARGETTYPE_UNDEFINED = 0,
	ORDERTARGETTYPE_GLOBAL = 1,
	ORDERTARGETTYPE_LOCATION = 2,
	ORDERTARGETTYPE_TARGET = 3
}AvHOrderTargetType;

typedef enum : std::uint8_t
{
	ORDERTYPE_UNDEFINED = 0,
	ORDERTYPEL_DEFAULT,
	ORDERTYPEL_MOVE,

	ORDERTYPET_ATTACK,
	ORDERTYPET_BUILD,
	ORDERTYPET_GUARD,
	ORDERTYPET_WELD,
	ORDERTYPET_GET,

	ORDERTYPEG_HOLD_POSITION,
	ORDERTYPEG_CODE_DEPLOY_MINES,
	ORDERTYPEG_CODE_GREEN,
	ORDERTYPEG_CODE_YELLOW,
	ORDERTYPEG_CODE_RED,

	ORDERTYPE_MAX
}AvHOrderType;

typedef enum : std::uint8_t
{
	TEAMMATE_MARINE_ORDER_WELD = 0,
	TEAMMATE_MARINE_ORDER_FOLLOW = 1,
	TEAMMATE_MARINE_ORDER_COVER = 2,
	TEAMMATE_MARINE_ORDER_UNKNOWN = 3,
	TEAMMATE_ALIEN_ORDER_HEAL = 4,
	TEAMMATE_ALIEN_ORDER_FOLLOW = 5,
	TEAMMATE_ALIEN_ORDER_COVER = 6,
	TEAMMATE_MARINE_LEFT_ARROW = 16,
	TEAMMATE_MARINE_RIGHT_ARROW = 17,
	TEAMMATE_ALIEN_LEFT_ARROW = 18,
	TEAMMATE_ALIEN_RIGHT_ARROW = 19,
	TEAMMATE_ALIEN_ORDER_UNKNOWN = 20
} TeammateOrderEnum;

////////////////////////////////////////////////////////////////
// BOT AI COMPONENTS
// Ideas: [Lars Liden] : Valve

///////////////////
// DEFINITIONS

// Bot Conditions
// --------------
// All conditions here specify the index in an array
// Use bit mask, much quicker at adding/removing and checking. limited to 32 conditions.

enum
{
	BOT_CONDITION_SEE_ENEMY = 1 << 0,	// See an enemy
	BOT_CONDITION_HEAR_DANGER = 1 << 1,	// Hear danger
	BOT_CONDITION_SEE_NEXT_WAYPOINT = 1 << 2,	// bot see's it's waypoint, useful for checking when stuck
	BOT_CONDITION_TASK_EDICT_NA = 1 << 3,	// Task edict not avaialable anymore.
	BOT_CONDITION_ENEMY_OCCLUDED = 1 << 4,	// Enemy blocked by something??
	BOT_CONDITION_ENEMY_DEAD = 1 << 5,	// Enemy killed
	BOT_CONDITION_HAS_WEAPON = 1 << 6,	// Has weapons
	BOT_CONDITION_OUT_OF_AMMO = 1 << 7,	// Out of ammo on current weapon
	BOT_CONDITION_SQUAD_LEADER_DEAD = 1 << 8,	// Bots leader died
	BOT_CONDITION_SEE_SQUAD_LEADER = 1 << 9,	// See's squad leader
	BOT_CONDITION_SEE_TASK_EDICT = 1 << 10,	// see the current edict for current task
	BOT_CONDITION_SEE_GOAL_EDICT = 1 << 11,	// see's the entity at bots objective
	BOT_CONDITION_CANT_SHOOT = 1 << 12,	// can't shoot
	BOT_CONDITION_SEE_BUILDABLE = 1 << 13,
	BOT_CONDITION_SEE_ENEMY_HEAD = 1 << 14,
	BOT_CONDITION_SEE_ENEMY_BODY = 1 << 15,
	BOT_CONDITION_NEED_WEAPONS = 1 << 16,
	BOT_CONDITION_ENEMY_IS_OSPREY = 1 << 17,
	BOT_CONDITION_STOPPED_BUILDING = 1 << 18,
	BOT_CONDITION_TASKS_CORRUPTED = 1 << 19,	// bot tasks can change while in task code causing problems
	BOT_CONDITION_TASK_EDICT_PAIN = 1 << 20,	// bot heardtask edict
	BOT_CONDITION_WANT_TO_LEAVE_GAME = 1 << 21,	// will be set if the bot wants to leave the game
	BOT_CONDITION_SELECTED_GUN = 1 << 22,	// TS: make a gun to use
	BOT_CONDITION_DONT_CLEAR_OBJECTIVES = 1 << 23
};

constexpr float BOT_HEAR_DISTANCE = 650.0f;

constexpr int BOT_UPGRADE_DEF = 1;
constexpr int BOT_UPGRADE_SEN = 2;
constexpr int BOT_UPGRADE_MOV = 3;

constexpr int BOT_LADDER_UNKNOWN = 0;
constexpr int BOT_LADDER_UP = 1;
constexpr int BOT_LADDER_DOWN = 2;

constexpr int BOT_TASKSTATUS_RANDOM_WAYPOINT = -1;
constexpr int BOT_TASKSTATUS_JUMP_OUT = -2;
constexpr int BOT_TASKSTATUS_STILL_PENDING = -3;

//////////////////////
// ENUMERTOR TYPES

/*

  Tasks & Schedules

*/

typedef enum : std::uint8_t
// collection of tasks
// (Not really.. just told to be so when you make the tasks)
// see bot_navigate lift tasks.
{
	BOT_SCHED_NONE = 0,
	BOT_SCHED_USE_LIFT,
	BOT_SCHED_WELD,
	BOT_SCHED_BUILD,
	BOT_SCHED_NS_CHECK_STRUCTURE,
	BOT_SCHED_NS_CHECK_HIVE,
	BOT_SCHED_RUN_FOR_COVER,
	BOT_SCHED_LOOK_FOR_WEAPON,
	BOT_SCHED_USE_TANK,
	BOT_SCHED_FOLLOW_LEADER,
	BOT_SCHED_USING_SCIENTIST,
	BOT_SCHED_USING_BARNEY,
	BOT_SCHED_PICKUP_FLAG,
	BOT_SCHED_USE_TELEPORTER,
	BOT_SCHED_MAKE_NEW_TELE_EXIT
	// etc
}eScheduleDesc;

typedef enum : std::uint8_t
{
	/*								*
	 *  General + NS bot tasks		*
	 */
	BOT_TASK_NONE,
	BOT_TASK_USE_TANK,				// currently using a turret
	BOT_TASK_WAIT_FOR_LIFT,
	BOT_TASK_RELOAD,
	BOT_TASK_LISTEN_TO_SOUND,
	BOT_TASK_FIND_ENEMY_PATH,
	BOT_TASK_FOLLOW_ENEMY,
	BOT_TASK_FOLLOW_LEADER,
	BOT_TASK_FIND_WEAPON,
	BOT_TASK_FIND_PATH,
	BOT_TASK_RUN_PATH,
	BOT_TASK_PICKUP_ITEM,
	BOT_TASK_WAIT_FOR_RESOURCES,
	BOT_TASK_GOTO_FLANK_POSITION,
	BOT_TASK_RANGE_ATTACK,
	BOT_TASK_NORMAL_ATTACK,
	BOT_TASK_HIDE,
	BOT_TASK_ASSEMBLE_SQUAD,
	BOT_TASK_WAIT_FOR_ORDERS,
	BOT_TASK_SOLO_RUN,
	BOT_TASK_ATTACK_ENEMY,
	BOT_TASK_CHANGE_WEAPON,
	BOT_TASK_MOVE_TO_VECTOR,
	BOT_TASK_WELD_OBJECT,
	BOT_TASK_GOTO_OBJECT,
	BOT_TASK_HEAL_PLAYER,
	BOT_TASK_ALIEN_UPGRADE,
	BOT_TASK_BUILD_ALIEN_STRUCTURE,
	BOT_TASK_FACE_VECTOR,
	BOT_TASK_FACE_EDICT,
	BOT_TASK_WAIT_AND_FACE_VECTOR,
	BOT_TASK_AVOID_OBJECT,
	BOT_TASK_BUILD,				// Might be same as USE, but requires the bot to wait until it is built.
	BOT_TASK_USE,				// USE object
	BOT_TASK_DEFEND,			// DEFEND object
	BOT_TASK_WAIT,				// WAIT at object (for team mates for example)
	BOT_TASK_DEPLOY_MINES,
	BOT_TASK_USE_AMMO_DISP,
	BOT_TASK_WALK_PATH,
	BOT_TASK_TYPE_MESSAGE,
	BOT_TASK_WAIT_FOR_ENTITY,
	BOT_TASK_USE_DOOR_BUTTON,
	BOT_TASK_SEARCH_FOR_ENEMY,
	BOT_TASK_ALIEN_EVOLVE,
	BOT_TASK_USE_HEV_CHARGER,
	BOT_TASK_USE_HEALTH_CHARGER,
	BOT_TASK_HUMAN_TOWER,
	BOT_TASK_THROW_GRENADE,
	BOT_TASK_PUSH_PUSHABLE,
	BOT_TASK_SECONDARY_ATTACK,
	BOT_TASK_WAIT_FOR_FLAG,
	/*								*
	 *  NS Command tasks to be used	*
	 *  in comm chair				*
	 *								*
	 *  not used!					*
	 */
	BOT_COMMAND_TASK_MOVE_TO_VECTOR,
	BOT_COMMAND_TASK_SELECT_PLAYERS,
	BOT_COMMAND_TASK_BUILD_OBJECT,
	BOT_COMMAND_TASK_ISSUE_ORDER,
	BOT_COMMAND_TASK_SELECT_POINT,
	BOT_COMMAND_TASK_SELECT_AREA,
	BOT_COMMAND_TASK_ISSUE_COMMAND,
	/*								*
	 *  example TFC tasks			*
	 */
	 // engineer build tasks
	 // take integer
	 // 1 to build, 0 to destroy
	 /*BOT_TASK_TFC_BUILD_SENTRY,
	 // take edict of sentry
	 BOT_TASK_TFC_REPAIR_BUILDABLE,
	 BOT_TASK_TFC_BUILD_TELEPORT_ENTRANCE,
	 BOT_TASK_TFC_BUILD_TELEPORT_EXIT,
	 BOT_TASK_TFC_BUILD_DISPENSER,
	 BOT_TASK_TFC_DETONATE_DISPENSER,
	 // Feign, taskint = 0 for silent feign,
	 // 1 for non silent feign
	 BOT_TASK_TFC_FEIGN_DEATH,
	 BOT_TASK_TFC_SNIPE,
	 BOT_TASK_TFC_PLACE_DETPACK,
	 BOT_TASK_TFC_CONC_JUMP,
	 BOT_TASK_TFC_ROCKET_JUMP,
	 BOT_TASK_TFC_PLACE_PIPES,
	 BOT_TASK_TFC_DETONATE_PIPES,
	 BOT_TASK_TFC_DISGUISE,
	 BOT_TASK_TFC_DETONATE_ENTRY_TELEPORT,
	 BOT_TASK_TFC_DETONATE_EXIT_TELEPORT,
	 BOT_TASK_TFC_ROTATE_SENTRY,
	 BOT_TASK_TFC_DISCARD,*/
	 ///////////////////////////////////////
	 /* find cover pos, Take an integer which
	  * will represent memory position of
	  * vector to alter
	  */
	BOT_TASK_FIND_COVER_POS,
	BOT_TASK_USE_TELEPORT,
	BOT_TASK_COMBAT_UPGRADE,
	BOT_TASK_CROUCH,
	BOT_TASK_ACCEPT_HEALTH,
	BOT_TASK_WAIT_FOR_BOT_AT_WPT,
	BOT_TASK_BLINK,
	BOT_TASK_WEB,
	BOT_TASK_USE_TELEPORTER,
	BOT_TASK_WAIT_FOR_RESOURCE_TOWER_BUILD,
	BOT_TASK_IMPULSE,
	BOT_TASK_DROP_WEAPON,
	BOT_TASK_SENSE_ENEMY
}eBotTask;

// A Task the bot should only change its view angles for
// i.e. turning
typedef enum : std::uint8_t
{
	BOT_LOOK_TASK_NONE,
	BOT_LOOK_TASK_NEXT_WAYPOINT,
	BOT_LOOK_TASK_SEARCH_FOR_ENEMY,
	BOT_LOOK_TASK_FACE_ENEMY,
	BOT_LOOK_TASK_FACE_TEAMMATE,
	BOT_LOOK_TASK_FACE_GOAL_OBJECT,
	BOT_LOOK_TASK_FACE_ENTITY,
	BOT_LOOK_TASK_FACE_TASK_VECTOR,
	BOT_LOOK_TASK_FACE_TASK_EDICT,
	BOT_LOOK_TASK_SEARCH_FOR_LAST_ENEMY,
	BOT_LOOK_TASK_LOOK_AROUND,
	BOT_LOOK_TASK_FACE_GROUND,
	BOT_LOOK_TASK_FACE_NEAREST_REMEMBER_POS
}eBotLookTask;

// bot events.
// can be handled inside bot code whilst being called from outside the bot
typedef enum : std::uint8_t
{
	BOT_EVENT_DIED,
	BOT_EVENT_KILL,
	BOT_EVENT_KILL_SELF,
	BOT_EVENT_SEE_TEAMMATE_DIE,
	BOT_EVENT_SEE_ENEMY_DIE,
	BOT_EVENT_SEE_PLAYER_DIE,
	BOT_EVENT_SEE_TEAMMATE_KILL,
	BOT_EVENT_SEE_ENEMY_KILL,
	BOT_EVENT_HURT,
	BOT_EVENT_HEAR_TEAMMATE_DIE,
	BOT_EVENT_FAIL_TASK,
	BOT_EVENT_COMPLETE_TASK,
	BOT_EVENT_LEAVING
}eBotEvent;

enum eBotCvarState : std::uint8_t
{
	BOT_CVAR_NOTEXIST = 0,
	BOT_CVAR_ACCESSED,
	BOT_CVAR_NEEDACCESS,
	BOT_CVAR_ERROR,
	BOT_CVAR_CONTINUE
};

///////////////////////////////////////////
// MESSAGES

typedef enum : std::uint8_t
{
	BOT_LANG_ENGLISH = 0,
	BOT_LANG_MAX
}eLanguage;

// NO MORE THAN 31 !!!
typedef enum : std::uint8_t
{
	BOT_TOOL_TIP_SERVER_WELCOME = 0,
	BOT_TOOL_TIP_CLIENT_WELCOME,
	BOT_TOOL_TIP_SQUAD_HELP,
	BOT_TOOL_TIP_ADMIN_HELP,
	BOT_TOOL_TIP_CPU_HELP,
	BOT_TOOL_TIP_NO_WAYPOINTS,
	BOT_TOOL_TIP_CREATE_SQUAD,
	BOT_TOOL_TIP_COMMANDER_WELCOME,
	BOT_TOOL_TIP_COMMANDER_MARINE_ORDER,
	BOT_TOOL_TIP_COMMANDER_SQUAD,
	BOT_TOOL_TIP_COMMANDER_SQUAD2,
	BOT_TOOL_TIP_COMMANDER_HEALTH,
	BOT_TOOL_TIP_BOTCAM_HELP,
	BOT_TOOL_TIP_BOTCAM_ON,
	BOT_TOOL_TIP_MAX
}eToolTip;

///////////////////////////////////////////////////////////////////////////////////
// BOT DEFINITIONS
constexpr int LADDER_UP = 1;
constexpr int LADDER_UNKNOWN = 0;
constexpr int LADDER_DOWN = 2;

constexpr int BOT_PITCH_SPEED = 30;
constexpr int BOT_YAW_SPEED = 30;

enum : std::uint8_t
{
	RESPAWN_NONE = 0,
	RESPAWN_IDLE = 1,
	RESPAWN_NEED_TO_RESPAWN = 2,
	RESPAWN_IS_RESPAWNING = 3,
	RESPAWN_LEFT_GAME = 4,
	RESPAWN_NEED_TO_REJOIN = 5
};

constexpr float BOT_DEFAULT_REACTION_TIME = 0.3f;
constexpr float BOT_DEFAULT_THINK_TIME = 0.04f;

//#define BOT_WAYPOINT_TOUCH_DIST(pev) max36.0pev->absmin.z
constexpr float BOT_WAYPOINT_TOUCH_DIST = 32.0f;

#define BOT_DEFAULT_NAME "RCBot"
///////////////////////////////////////////////////////////////////////////////////
// MOD ID's
// add new mod Id's to the end!!!
// notice some aren't used
enum : std::uint8_t
{
	MOD_NOT_SUPPORTED = 0,
	MOD_ANY = 1,
	MOD_NS = 2,			// natural selection
	MOD_BUMPERCARS = 3,
	MOD_RC = 4,			// rocket crowbar
	MOD_RC2 = 5,		// rocket crowbar 2
	MOD_SI = 6,			// TODO: Science and Industry
	MOD_DMC = 7,		// deathmatch classic
	MOD_TS = 8,			// the specialists
	MOD_GEARBOX = 9,	// Opposing Force [APG]RoboCop[CL]
	MOD_IOS = 10,		// international online soccer (gave up :p)
	MOD_BG = 11,		// battlegrounds
	MOD_HL_RALLY = 12,  // half-life rally (game crashes when adding a bot!!)
	MOD_HL_DM = 13,		// half-life deathmatch
	MOD_RS = 14,		// rival species meh...
	MOD_WW = 15,		// TODO: Wizard Wars
	MOD_FLF = 16,		// TODO: Front Line Force
};

enum : std::uint8_t
{
	VGUI_MENU_NONE = 0,
	VGUI_MENU_IOS_MOTD = 1,
	VGUI_MENU_IOS_SELECT_TEAM = 2,
	VGUI_MENU_IOS_SELECT_POSITION = 3,
	VGUI_MENU_BG_SELECT_TEAM = 2,
	VGUI_MENU_BG_SELECT_BRITISH_CLASS = 3,
	VGUI_MENU_BG_SELECT_AMERICAN_CLASS = 4
};
constexpr bool TEAM_BLACK_MESA = false;
constexpr bool TEAM_OPPOSING_FORCE = true;

constexpr bool VGUI_MENU_WW_TEAM_SELECT = false;
constexpr bool VGUI_MENU_WW_CLASS_SELECT = true;

//#define VGUI_MENU_TFC_TEAM_SELECT 2
//#define VGUI_MENU_TFC_CLASS_SELECT 3

////////////////////////////
/*#define TFC_MAX_CLASSES 10

#define TFC_CLASS_CIVILIAN  0
#define TFC_CLASS_SCOUT		1
#define TFC_CLASS_SNIPER	2
#define TFC_CLASS_SOLDIER	3
#define TFC_CLASS_DEMOMAN	4
#define TFC_CLASS_MEDIC		5
#define TFC_CLASS_HWGUY		6
#define TFC_CLASS_PYRO		7
#define TFC_CLASS_SPY		8
#define TFC_CLASS_ENGINEER	9

/////////// macros
#define TFC_CLASS_CIVILIAN_LIMIT    (int)CVAR_GET_FLOAT("cr_civilian")
#define TFC_CLASS_SCOUT_LIMIT		(int)CVAR_GET_FLOAT("cr_scout")
#define TFC_CLASS_SNIPER_LIMIT		(int)CVAR_GET_FLOAT("cr_sniper")
#define TFC_CLASS_SOLDIER_LIMIT		(int)CVAR_GET_FLOAT("cr_soldier")
#define TFC_CLASS_DEMOMAN_LIMIT		(int)CVAR_GET_FLOAT("cr_demoman")
#define TFC_CLASS_MEDIC_LIMIT		(int)CVAR_GET_FLOAT("cr_medic")
#define TFC_CLASS_HWGUY_LIMIT		(int)CVAR_GET_FLOAT("cr_hwguy")
#define TFC_CLASS_PYRO_LIMIT		(int)CVAR_GET_FLOAT("cr_pyro")
#define TFC_CLASS_SPY_LIMIT			(int)CVAR_GET_FLOAT("cr_spy")
#define TFC_CLASS_ENGINEER_LIMIT	(int)CVAR_GET_FLOAT("cr_engineer")

#define TFC_TELEPORTER_ENTRANCE 1
#define TFC_TELEPORTER_EXIT		2
*/
typedef enum : std::uint8_t
{
	MAP_UNKNOWN = 0,		// unknown map type
	MAP_CTF = 1,			// normal capture the flag (flag in enemy base) e.g. 2fort
	MAP_CAPTURE = 2,		// capture without flag e.g. warpath
	MAP_ATTACK_DEFEND = 3,  // team attacks other defends e.g. dustbowl/avanti
	MAP_CTF_BASE = 4,		// capture the flag (flag in your base) e.g. epicenter
	MAP_FLAG_MULTIPLE = 5,  // take many flags capture all of them, e.g. flagrun
	MAP_CAPTURE_FLAG_MULTIPLE = 6,  // capture many points e.g. cz2
	MAP_VIP = 7, // hunted type map*
	NON_TS_TEAMPLAY = 12
}eMapType;

////////////////////////////////
// AUTHOR SHIP & PLUGIN INFO

#define BOT_NAME "RCBot"
#define BOT_WEBSITE "http://rcbot.bots-united.com"
#define BOT_DBG_MSG_TAG "[RCBOT] "
#define BOT_DEBUG_TAG "[DEBUG]"

///
// combat stuff in NS, what bots want to get
enum
{
	BOT_COMBAT_WANT_SHOTGUN = 1 << 0,
	BOT_COMBAT_WANT_HMG = 1 << 1,
	BOT_COMBAT_WANT_GRENADE_GUN = 1 << 2,
	BOT_COMBAT_WANT_JETPACK = 1 << 3,
	BOT_COMBAT_WANT_ARMOR = 1 << 4,
	BOT_COMBAT_WANT_WELDER = 1 << 5,
	BOT_COMBAT_WANT_RESUPPLY = 1 << 6,
	BOT_COMBAT_WANT_MINES = 1 << 7,
	BOT_COMBAT_WANT_CATALYST = 1 << 8,
	BOT_COMBAT_WANT_MOVE_DETECTION = 1 << 9,
	BOT_COMBAT_WANT_LERK = 1 << 10,
	BOT_COMBAT_WANT_FADE = 1 << 11,
	BOT_COMBAT_WANT_ONOS = 1 << 12,
	BOT_COMBAT_WANT_DEFUP1 = 1 << 13,
	BOT_COMBAT_WANT_DEFUP2 = 1 << 14,
	BOT_COMBAT_WANT_DEFUP3 = 1 << 15,
	BOT_COMBAT_WANT_MOVUP1 = 1 << 16,
	BOT_COMBAT_WANT_MOVUP2 = 1 << 17,
	BOT_COMBAT_WANT_MOVUP3 = 1 << 18,
	BOT_COMBAT_WANT_SENUP1 = 1 << 19,
	BOT_COMBAT_WANT_SENUP2 = 1 << 20,
	BOT_COMBAT_WANT_SENUP3 = 1 << 21
};

//////////////////////////////////////////////
// Bits in GA Individual for combat
//////////////////////////////////////////////
enum : std::uint8_t
{
	BOT_GA_COMBAT_WANT_SHOTGUN = 1,
	BOT_GA_COMBAT_WANT_HMG = 2,
	BOT_GA_COMBAT_WANT_GRENADE_GUN = 3,
	BOT_GA_COMBAT_WANT_JETPACK = 4,
	BOT_GA_COMBAT_WANT_ARMOR = 5,
	BOT_GA_COMBAT_WANT_WELDER = 6,
	BOT_GA_COMBAT_WANT_RESUPPLY = 7,
	BOT_GA_COMBAT_WANT_MINES = 8,
	BOT_GA_COMBAT_WANT_MOVEMENT_DETECT = 9,

	BOT_GA_COMBAT_WANT_LERK = 10,
	BOT_GA_COMBAT_WANT_FADE = 11,
	BOT_GA_COMBAT_WANT_ONOS = 12,

	BOT_GA_COMBAT_WANT_DEF_UPGRADE1 = 13,
	BOT_GA_COMBAT_WANT_DEF_UPGRADE2 = 14,
	BOT_GA_COMBAT_WANT_DEF_UPGRADE3 = 15,
	BOT_GA_COMBAT_WANT_MOV_UPGRADE1 = 16,
	BOT_GA_COMBAT_WANT_MOV_UPGRADE2 = 17,
	BOT_GA_COMBAT_WANT_MOV_UPGRADE3 = 18,
	BOT_GA_COMBAT_WANT_SEN_UPGRADE1 = 19,
	BOT_GA_COMBAT_WANT_SEN_UPGRADE2 = 20,
	BOT_GA_COMBAT_WANT_SEN_UPGRADE3 = 21
};

//////////////////////////////////////////////

#ifdef __linux__

///////////////////////
// linux names
///////////////////////
#ifdef RCBOT_META_BUILD

#ifdef _DEBUG
#define BOT_VER "1.51b11_mm_debug"
#else
#define BOT_VER "1.51b11_mm"
#endif

#else

#ifdef _DEBUG
#define BOT_VER "1.51b11_debug"
#else
#define BOT_VER "1.51b11"
#endif

#endif
//////////////////////

#else

///////////////////////
// windows names
///////////////////////
#ifdef RCBOT_META_BUILD

#ifdef _DEBUG
#define BOT_VER "1.51b11_mm_debug"
#else
#define BOT_VER "1.51b11_mm"
#endif

#else

#ifdef _DEBUG
#define BOT_VER "1.51b11_debug"
#else
#define BOT_VER "1.51b11"
#endif

#endif
//////////////////////

#endif
				// bot version
//#define BOT_DATE "10/03/2006"		// date of creation
#define BOT_AUTHOR "Cheeseh (cheeseh@bots-united.com)" // bot author
#define BOT_TAG "[RCBOT]"
#define BOT_VER_CVAR "rcbot_ver"
#define BOT_COMMAND_ACCESS "rcbot" // main bot command

constexpr int MAX_PLAYERS = 32;
constexpr int RCBOT_ACCESS_FORCE_GRIP = 9;

#define BOT_CRASHLOG_FILE "rcbot_crashlog.txt"
#define BOT_PROFILES_FILE "bot_profiles.ini"
#define BOT_WEAPON_PRESETS_FILE "bot_weapons.ini"
#define BOT_PLAYER_ID_FILE "bot_playerids.ini"

#define BOT_PLAYER_ID_FILE_HEADER "# Automatically generated, do not edit.\n# however this file is safe to delete\n# but will be re-generated.\n"

////////////////////////////////////////////
// BOT CONFIGURATION

constexpr int DEFAULT_BOT_CHAT_PERCENT = 15;
constexpr int BOT_CHAT_MESSAGE_LENGTH = 128;
constexpr int BOT_CHAT_TYPE_SPEED_SEC = 9;

enum
{
	BOT_CONFIG_WAIT_FOR_ORDERS = 1 << 0,
	BOT_CONFIG_DONT_SHOOT = 1 << 1,
	BOT_CONFIG_CHATTING = 1 << 2,
	BOT_CONFIG_REAL_MODE = 1 << 3,
	BOT_CONFIG_COMMANDING = 1 << 4,
	BOT_CONFIG_RESERVE_BOT_SLOTS = 1 << 5,
	BOT_CONFIG_CHAT_REPLY_TO_BOTS = 1 << 6,
	BOT_CONFIG_MARINE_AUTO_BUILD = 1 << 7,
	BOT_CONFIG_CHAT_DONT_LEARN = 1 << 8,
	BOT_CONFIG_BALANCE_TEAMS = 1 << 9,
	BOT_CONFIG_TOOLTIPS = 1 << 10,
	BOT_CONFIG_ENABLE_BOTCAM = 1 << 11,
	BOT_CONFIG_AUTOWAYPOINT_HUMANS = 1 << 12,
	BOT_CONFIG_UNSTICK = 1 << 13,
	BOT_CONFIG_ABNORMAL_GAME = 1 << 14,
	BOT_CONFIG_BOTS_LEAVE_AND_JOIN = 1 << 15,
	BOT_CONFIG_BOTS_WAIT_FOR_BOTS = 1 << 16,
	BOT_CONFIG_NOT_NS3_FINAL = 1 << 17,
	BOT_CONFIG_BLINKING = 1 << 18,
	BOT_CONFIG_WAIT_AT_RESOURCES = 1 << 19,
	// marine bots wait for commander to drop res tower
	BOT_CONFIG_DISABLE_WEAPON_LEARN = 1 << 20,
	BOT_CONFIG_TS_KUNGFU = 1 << 21,
	BOT_CONFIG_DISABLE_BOT_SQUADS = 1 << 22,
	BOT_CONFIG_TS_DONT_STEAL_WEAPONS = 1 << 23,
	BOT_CONFIG_TS_DONT_PICKUP_WEAPONS = 1 << 24
};

////////////////////////////////////////////
// BOT CHAT
// TODO: those need to be declared for bot_chat.ini to work [APG]RoboCop[CL]
#define BOT_CHAT_FILE "bot_chat.ini"

enum eBotChatType : std::int8_t
{
	BOT_CHAT_UNKNOWN = -1,
	BOT_CHAT_KILLS = 0,
	BOT_CHAT_KILLED = 1,
	BOT_CHAT_THANKS = 2,
	BOT_CHAT_HELP = 3,
	BOT_CHAT_IDLE = 4,
	BOT_CHAT_LAUGH = 5,
	BOT_CHAT_GREETINGS = 6,
	BOT_CHAT_LEAVEGAME = 7,
	BOT_CHAT_MAX = 8
};

enum eBotRepType : std::uint8_t
{
	BOT_REP_UNKNOWN = 0,
	BOT_REP_NEUTRAL,
	BOT_REP_FRIENDLY,
	BOT_REP_UNFRIENDLY
};

enum eBattleGroundsMessage : std::uint8_t
{
	BOT_BG_MSG_YES = 1,
	BOT_BG_MSG_NO,
	BOT_BG_MSG_FOLLOW,
	BOT_BG_MSG_LEAVE,
	BOT_BG_MSG_UNDERFIRE,
	BOT_BG_MSG_SPREADOUT,
	BOT_BG_MSG_BATTLECRY,
	BOT_BG_MSG_MEDIC,
	BOT_BG_MSG_ADVANCE,
	BOT_BG_MSG_RETREAT,
	BOT_BG_MSG_RALLY,
	BOT_BG_MSG_HALT,
	BOT_BG_MSG_LINEUP,
	BOT_BG_MSG_MAKEREADY,
	BOT_BG_MSG_PRESENT_ARMS,
	BOT_BG_MSG_FIRE,
	BOT_BG_MSG_CEASEFIRE
};

/*
#define BOT_CHAT_LAUGH_TAG "laugh"
#define BOT_CHAT_KILL_TAG "kills"
#define BOT_CHAT_KILLED_TAG "killed"
#define BOT_CHAT_THANKS_TAG "thanks"
#define BOT_CHAT_GREETINGS_TAG "greetings"
#define BOT_CHAT_IDLE_TAG "idle"
#define BOT_CHAT_HELP_TAG "help"*/

/////////////////////////
// DEBUG LEVELS							Debug Messages Will Be Shown When :
enum : std::uint16_t
{
	BOT_DEBUG_TOUCH_LEVEL = 1 << 0,	// Bot touched object
	BOT_DEBUG_THINK_LEVEL = 1 << 1,	// Bot thinks
	BOT_DEBUG_HEAR_LEVEL = 1 << 2,	// Bot hears a sound
	BOT_DEBUG_MESSAGE_LEVEL = 1 << 3,	// Bot recieves net message
	BOT_DEBUG_BLOCK_LEVEL = 1 << 4,	// Bot blocks object
	BOT_DEBUG_MOVE_LEVEL = 1 << 5,	// Bot moves somewhere
	BOT_DEBUG_AIM_LEVEL = 1 << 6,	// Bot aims at something
	BOT_DEBUG_NAV_LEVEL = 1 << 7,	// Bot touches/finds waypoints
	BOT_DEBUG_SEE_LEVEL = 1 << 8	// Bot sees something
};

/////////////////////////////////
// PROFILE

constexpr int BOT_PROFILE_READ_ERROR = 0;
constexpr int BOT_PROFILE_READ_BOT_INFO = 1;
constexpr int BOT_PROFILE_READ_REP_INFO = 2;

#define BOT_PROFILE_FAVMOD "favmod="
#define BOT_PROFILE_FAVTEAM "favteam="
#define BOT_PROFILE_FAVMAP "favmap="
#define BOT_PROFILE_SKILL "skill="
#define BOT_PROFILE_SPRAY "spray="
#define BOT_PROFILE_GORGE_PERCENT "gorge_percent="
#define BOT_PROFILE_LERK_PERCENT "lerk_percent="
#define BOT_PROFILE_FADE_PERCENT "fade_percent="
#define BOT_PROFILE_ONOS_PERCENT "onos_percent="
#define BOT_PROFILE_BOT_NAME "name="
#define BOT_PROFILE_NUMGAMES "numgames="

#define POINTER_TO_INT(x)		*(int*)x
#define POINTER_TO_FLOAT(x)		*(float*)x
#define POINTER_TO_STRING(x)	(char*)x

#define POINTER_INCREMENT_VALUE(x) (*(x) = *(x) + 1)
#define POINTER_VALUE(x) (*(x))
//////////////////////////////
// PATH INDEXES FOR A WAYPOINT
constexpr int MAX_PATH_INDEX = 4;

#define BEGIN_SEARCH_THROUGH_PLAYERS(Variable) \
        int i; \
        for ( i = 1; i <= gpGlobals->maxClients; i ++ ) \
{ \
			(Variable) = INDEXENT(i);\
			if ( (Variable) == NULL )\
				continue;\
			if ( (Variable)->free )\
				continue;\

#define END_SEARCH_THROUGH_PLAYERS }

/////////////////////
constexpr int MAX_BOT_ID = 1000;

//////////////////////////////////////////////////////////
// SKILLS

constexpr int MAX_BOT_SKILL = 100;
constexpr int MIN_BOT_SKILL = 1;
constexpr int DEF_BOT_SKILL = (MAX_BOT_SKILL + MIN_BOT_SKILL) / 2;

//////////////////////////////////////////////////////////
// REPUTATION DEFINITIONS

constexpr int BOT_MAX_REP = 10;
constexpr int BOT_MIN_REP = 0;
constexpr int BOT_MID_REP = 5;
constexpr int BOT_LOW_REP = 3;
constexpr int BOT_HIGH_REP = 6;

constexpr int BOT_VIEW_DISTANCE = 2048;
//#define BOT_DEFAULT_TURN_SPEED 10

#define BOT_PROFILES_FOLDER "botprofiles"

// sven co-op entity flags - w00tguy
/*#define SF_BREAK_INSTANT                256// instant break if hit with crowbar/electric crowbar/wrench (depends on weapon key)
#define SF_BREAK_EXPLOSIVES             512 // takes damage from explosives only
#define SF_BREAK_IMMUNE_TO_CLIENTS      64 // can't be damaged by players
#define SF_BREAK_REPAIRABLE             8 // can be repaired with the wrench
*/

#endif
