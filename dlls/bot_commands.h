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
// bot_commands.h
//
//////////////////////////////////////////////////
//
// Prototypes for bot commands & cvar functions
//

#ifndef __BOT_COMMANDS_H__
#define __BOT_COMMANDS_H__

#define BOT_COMMAND_ACCESS_ALLOWED				0
#define BOT_COMMAND_ACCESS_WAYPOINTING		(1<<0)
#define BOT_COMMAND_ACCESS_ADD_REMOVE_BOTS	(1<<1)
#define BOT_COMMAND_ACCESS_UTIL				(1<<2)
#define BOT_COMMAND_ACCESS_CONFIG			(1<<3)
#define BOT_COMMAND_ACCESS_DEBUG			(1<<4)
#define BOT_COMMAND_ACCESS_USERS            (1<<5)

// test function
void GiveTSWeapon ( edict_t *pEdict, int id );

void SetupCommands ( void );

eBotCvarState BotFunc_AddBot ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );

class CRemoveBotCommand : public CBotCvar
{
public:
	CRemoveBotCommand ()
	{
		setupCommand("removebot",BOT_COMMAND_ACCESS_ADD_REMOVE_BOTS);
	}

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"removebot command");
		
	}
	
	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CDebugBotCommand : public CBotCvar
{
public:
	CDebugBotCommand ()
	{
		setupCommand("debug_bot",BOT_COMMAND_ACCESS_DEBUG,FALSE);
	}

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"debug_bot command");
		BotMessage(pEntity,0,"=================");
		BotMessage(pEntity,0,"commands");
		BotMessage(pEntity,0,"--------");
		BotMessage(pEntity,0,"\"debug_bot bot <bot name>\" : shows bot task & wpt info on screen");
		BotMessage(pEntity,0,"\"debug_bot off\" stops showing bot info on screen");
	}

	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CDebugEntCommand : public CBotCvar
{
public:
	CDebugEntCommand ()
	{
		setupCommand("debug_ent",BOT_COMMAND_ACCESS_DEBUG,FALSE);
	}

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"debug_ent command");
		BotMessage(pEntity,0,"=================");
		BotMessage(pEntity,0,"commands");
		BotMessage(pEntity,0,"--------");
		BotMessage(pEntity,0,"\"debug_ent entity\" : shows facing entity info on screen");
		BotMessage(pEntity,0,"\"debug_ent off\" stops showing info on screen");
	}

	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};		

class CUsersCommand : public CBotCvar
{
public:
	CUsersCommand ()
	{
		setupCommand("users",BOT_COMMAND_ACCESS_USERS,TRUE);
	}

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"users command");
		BotMessage(pEntity,0,"=============");
		BotMessage(pEntity,0,"commands: ");
		BotMessage(pEntity,0,"---------");
		BotMessage(pEntity,0,"readusers (Reads bot_users.ini file again)");
		BotMessage(pEntity,0,"adduser -- adds a user\nUsage: <player name> <access level>");
		BotMessage(pEntity,0,"removeuser -- removes a user\nUsage: <(player name) / #(user id)>");
		BotMessage(pEntity,0,"showusers -- shows all allowed bot users");
	}

	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CHelpCommand : public CBotCvar
{
public:
    CHelpCommand ()
    {
        setupCommand("help",BOT_COMMAND_ACCESS_ALLOWED,TRUE);
    }

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"help command");
		BotMessage(pEntity,0,"------------");
		BotMessage(pEntity,0,"Usage : rcbot help [command name]");
		BotMessage(pEntity,0,"You can view useful tips on commands");
		BotMessage(pEntity,0,"and how to use them");
		BotMessage(pEntity,0,"-- general --");
		BotMessage(pEntity,0,"commands may have parameters");
		BotMessage(pEntity,0,"- if a parameter is needed that parameter is enclosed in '<' and '>'");
		BotMessage(pEntity,0,"  (example: rcbot util teleport [player name])");
		BotMessage(pEntity,0,"- if a parameter is optional that parameter is enclosed in '[' and ']'");
		BotMessage(pEntity,0,"  (example: rcbot util killbot <bot name>)");
	}
	
	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CAutoWaypointCommand : public CBotCvar
{
public:
    CAutoWaypointCommand ()
    {
        setupCommand("autowaypoint",BOT_COMMAND_ACCESS_WAYPOINTING,TRUE);
    }

	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"auto_waypoint command");
		BotMessage(pEntity,0,"---------------------");
		BotMessage(pEntity,0,"Usage : rcbot autowaypoint [player name] [on (1) | off (0)]");
		BotMessage(pEntity,0,"Automatically places waypoints on player");
		BotMessage(pEntity,0,"If no player name is issued, it sets autowaypoint ON/OFF for you");
		BotMessage(pEntity,0,"If no on/off value is issued, it toggles the autowaypoint state");
		BotMessage(pEntity,0,"You must issue a player name if using a dedicated server");
	}
	
	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CMenuSelectCommand : public CBotCvar
{
public:
    CMenuSelectCommand ()
    {
        setupCommand("menuselect",BOT_COMMAND_ACCESS_ALLOWED,FALSE);
    }
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CSetPassCommand : public CBotCvar
{
public:
    CSetPassCommand ()
    {
        setupCommand("set_pass",BOT_COMMAND_ACCESS_ALLOWED,FALSE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"set_pass command help");
		BotMessage(pEntity,0,"---------------------");
		BotMessage(pEntity,0,"Usage : rcbot set_pass <password>");
		BotMessage(pEntity,0,"Allows the bot program to check the entered password");
		BotMessage(pEntity,0,"with your name to gain access to more rcbot commands");		
		BotMessage(pEntity,0,"Your password is case sensitive and your game name is");
		BotMessage(pEntity,0,"also case sensitive, they must match on list of users");
	}
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CWaypointMenuCommand : public CBotCvar
{
public:
    CWaypointMenuCommand ()
    {
        setupCommand("waypoint_menu",BOT_COMMAND_ACCESS_WAYPOINTING,FALSE);
    }
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CBotMenuCommand : public CBotCvar
{
public:
    CBotMenuCommand ()
    {
        setupCommand("bot_menu",BOT_COMMAND_ACCESS_ADD_REMOVE_BOTS,FALSE);
    }

	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CUtilCommand : public CBotCvar
{
public:
    CUtilCommand ()
    {
        setupCommand("util",BOT_COMMAND_ACCESS_UTIL,TRUE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"util command help");
		BotMessage(pEntity,0,"-----------------");
		BotMessage(pEntity,0,"killbot <bot name>");
		BotMessage(pEntity,0,"kills the bot with the name of <bot name>\n");
		BotMessage(pEntity,0,"set_teleport");
		BotMessage(pEntity,0,"sets your current teleport possition, anyone you teleport will teleport where you are\n");
		BotMessage(pEntity,0,"teleport [player name]");
		BotMessage(pEntity,0,"will teleport the player with [player name] or yourself if it is not issued\n");
		BotMessage(pEntity,0,"notouch [player name]");
		BotMessage(pEntity,0,"will stop touch calls reaching the player with [player name] or yourself if it is not issued.");
		BotMessage(pEntity,0,"no touch means you won't pick up objects and wont go through teleports (needed for waypointing teleports)\n");
		BotMessage(pEntity,0,"notarget_mode [player name]");
		BotMessage(pEntity,0,"will stop monsters shooting the player with [player name] or yourself if it is not issued.\n");
		BotMessage(pEntity,0,"send_sound <sound path>[.wav]");
		BotMessage(pEntity,0,"makes the sound in the <sound path> play on everyones client provided they have the sound\n");
		BotMessage(pEntity,0,"noclip_mode [player name]");
		BotMessage(pEntity,0,"will make the player fly through walls with [player name] or yourself if it is not issued\n");
		BotMessage(pEntity,0,"godmode [player name]");
		BotMessage(pEntity,0,"will put god mode on the player with [player name] or yourself if it is not issued\n");
		BotMessage(pEntity,0,"showboredom : will print bot's boredom (CAREFUL!! May overflow your client [bug?])\n");
		BotMessage(pEntity,0,"showreps : will print bots reputations (CAREFUL!! May overflow your client [bug?])\n");
		
	}
	
	eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CAddBotCommand : public CBotCvar
{
public:
    CAddBotCommand ()
    {
        setupCommand("addbot",BOT_COMMAND_ACCESS_ADD_REMOVE_BOTS,TRUE);
    }

	void showHelp ( edict_t *pEntity );

     eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );

};

class CWaypointCommand : public CBotCvar
{
public:
    CWaypointCommand ()
    {
        setupCommand("waypoint",BOT_COMMAND_ACCESS_WAYPOINTING,FALSE);
    }
	
	void showHelp ( edict_t *pEntity );
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
	
};

class CBotCamCommand : public CBotCvar
{
public:
    CBotCamCommand ()
    {
        setupCommand("botcam",BOT_COMMAND_ACCESS_ALLOWED,FALSE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"bot cam command help");
		BotMessage(pEntity,0,"--------------------");
		BotMessage(pEntity,0,"Provided that \"rcbot config allow_botcam\" is 1");
		BotMessage(pEntity,0,"you can view the bot cam, simply type...");
		BotMessage(pEntity,0,"\"rcbot botcam\" to view the bot cam, or");
		BotMessage(pEntity,0,"\"rcbot botcam off\" to stop viewing the bot cam");
	}
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CBotSquadCommand : public CBotCvar
{
public:
    CBotSquadCommand ()
    {
        setupCommand("squad",BOT_COMMAND_ACCESS_ALLOWED,FALSE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"squad command help");
		BotMessage(pEntity,0,"------------------");
		BotMessage(pEntity,0,"Allows you control squad as a leader");
		BotMessage(pEntity,0,"Available sub commands are:");
		BotMessage(pEntity,0,"\"form <formation name>\" (change formation)");
		BotMessage(pEntity,0,"\"spread <spread size>\" (change space between squad members)");
		BotMessage(pEntity,0,"NOTE: I recommend using the \"rcbot squad_menu\" command instead");
	}
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CDebugCommand : public CBotCvar
{
public:
    CDebugCommand ()
    {
        setupCommand("debug",BOT_COMMAND_ACCESS_DEBUG,TRUE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"debug command help");
		BotMessage(pEntity,0,"------------------");
		BotMessage(pEntity,0,"Usage: rcbot debug <debug level> <on/off>");
		BotMessage(pEntity,0,"on = 1, off = 0");
		BotMessage(pEntity,0,"Available debug levels are:");
		BotMessage(pEntity,0,"\"touch\"");
		BotMessage(pEntity,0,"\"think\"");
		BotMessage(pEntity,0,"\"hear\"");
		BotMessage(pEntity,0,"\"message\"");
		BotMessage(pEntity,0,"\"block\"");
		BotMessage(pEntity,0,"\"move\"");
		BotMessage(pEntity,0,"\"aim\"");
		BotMessage(pEntity,0,"\"nav\"");
		BotMessage(pEntity,0,"\"see\"");		
	}
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CSquadMenuCommand : public CBotCvar
{
public:
    CSquadMenuCommand ()
    {
        setupCommand("squad_menu",BOT_COMMAND_ACCESS_ALLOWED,FALSE);
    }

	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
	
};

class CConfigCommand : public CBotCvar
{
public:
    CConfigCommand ()
    {
        setupCommand("config",BOT_COMMAND_ACCESS_CONFIG,TRUE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"config command help");
		BotMessage(pEntity,0,"-------------------");
		BotMessage(pEntity,0,"available config commands are...");
		BotMessage(pEntity,0,"-----------------------");
		BotMessage(pEntity,0,"Boolean commands");
		BotMessage(pEntity,0,"these commands take booleans as parameters only");
		BotMessage(pEntity,0,"booleans are 0 or 1");
		BotMessage(pEntity,0,".......................");
		BotMessage(pEntity,0,"bots_leave_and_join (incomplete)");
		BotMessage(pEntity,0,"tooltips");
		BotMessage(pEntity,0,"autowaypoint_humans");
		BotMessage(pEntity,0,"allow_botcam");
		BotMessage(pEntity,0,"balance_teams");
		BotMessage(pEntity,0,"chat_dont_learn");
		BotMessage(pEntity,0,"marine_auto_build");
		BotMessage(pEntity,0,"chat_reply_to_bots");
		BotMessage(pEntity,0,"wait_for_orders");
		BotMessage(pEntity,0,"reserve_bot_slots");
		BotMessage(pEntity,0,"dont_shoot");
		BotMessage(pEntity,0,"chatting");
		BotMessage(pEntity,0,"real_mode");
		BotMessage(pEntity,0,"commanding");
		BotMessage(pEntity,0,"abnormal_game (alien v alien eg.)");
		BotMessage(pEntity,0,"bots_wait_for_bots (bots wait at wpts)");
		BotMessage(pEntity,0,"not_ns3final");
		BotMessage(pEntity,0,"blinking (fade blinking on/off)");
		BotMessage(pEntity,0,"wait_at_res (bots wait at res fountains in NS)");
		BotMessage(pEntity,0,"ts_kungfu (bots use kung fu only in TS");
		BotMessage(pEntity,0,"disable_weapon_learning (bots choose random weapons)");
		BotMessage(pEntity,0,"disable_bot_squads (bots dont make squads)");
		BotMessage(pEntity,0,"-----------------------");
		BotMessage(pEntity,0,"Integer commands");
		BotMessage(pEntity,0,"these commands take integers as parameters only");
		BotMessage(pEntity,0,"integers are -1, 0, 1, 2, 3, 4... etc.");
		BotMessage(pEntity,0,".......................");
		BotMessage(pEntity,0,"min_bots");
		BotMessage(pEntity,0,"max_bots");
		BotMessage(pEntity,0,"chat_percent");
		BotMessage(pEntity,0,"chat_reply_percent");
		BotMessage(pEntity,0,"max_path_revs");
		BotMessage(pEntity,0,"max_update_vision_revs");
		BotMessage(pEntity,0,"-----------------------");
		BotMessage(pEntity,0,"Float commands");
		BotMessage(pEntity,0,"these commands take real numbers as parameters only");
		BotMessage(pEntity,0,"float numbers are 0.0, 0.5, 0.6, 1.0, 2.1... etc.");
		BotMessage(pEntity,0,".......................");
		BotMessage(pEntity,0,"bot_stuck_speed");
		BotMessage(pEntity,0,"update_ladder_time");
		BotMessage(pEntity,0,"gorge_amount");
		BotMessage(pEntity,0,"wall_stick_tolerance");
		BotMessage(pEntity,0,"lerk_hold_time");
		BotMessage(pEntity,0,"lerk_flap_time");
		BotMessage(pEntity,0,"update_vision_time");	
		BotMessage(pEntity,0,"u_hive");	
		BotMessage(pEntity,0,"u_restower");	
		BotMessage(pEntity,0,"u_healing");	
		BotMessage(pEntity,0,"u_structure");	
		BotMessage(pEntity,0,"u_refill");	
	}
	/*
		"rcbot config u_hive"	
		"rcbot config u_restower"	
		"rcbot config u_healing"	
		"rcbot config u_structure"	
	    "rcbot config u_refill"
		*/
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

class CPathWaypointCommand : public CBotCvar
{
public:
    CPathWaypointCommand ()
    {
        setupCommand("pathwaypoint",BOT_COMMAND_ACCESS_WAYPOINTING,FALSE);
    }
	
	void showHelp ( edict_t *pEntity )
	{
		BotMessage(pEntity,0,"pathwaypoint command help");
		BotMessage(pEntity,0,"-------------------------");
		BotMessage(pEntity,0,"\"pathwaypoint on\" shows paths between waypoints");
		BotMessage(pEntity,0,"\"pathwaypoint off\" stops showing paths");
		BotMessage(pEntity,0,"\"pathwaypoint create1\" marks the waypoint you are standing at to create a path from");
		BotMessage(pEntity,0,"\"pathwaypoint create2\" creates a path from the marked waypoint from, to this waypoint");
		BotMessage(pEntity,0,"\"pathwaypoint remove1\" marks the waypoint you are standing at to remove a path from");
		BotMessage(pEntity,0,"\"pathwaypoint remove2\" removes a path from the marked waypoint from, to this waypoint");
		BotMessage(pEntity,0,"\"pathwaypoint enable\" allows automatic path creation (default = enabled)");
		BotMessage(pEntity,0,"\"pathwaypoint disable\" disallows automatic path creation");
	}
	
	 eBotCvarState action ( CClient *pClient, const char *arg1, const char *arg2, const char *arg3, const char *arg4 );
};

#endif