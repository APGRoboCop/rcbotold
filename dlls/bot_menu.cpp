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

#include "bot_commands.h"

#include "bot_menu.h"

extern CBotGlobals gBotGlobals;
extern WAYPOINTS waypoints;

/*
  Setup Menus


  Makes the menus

  If you want to make a new menu, make a new menu type in the bot_menu.h file


*/
// this should be a CBotGlobals method 8)
void SetupMenus ( void )
{
	// menu type = BOT_MENU_WAYPOINT_MAIN
	// give this menu the title "waypoint menu"
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN] = CBotMenu("Waypoint Menu");

	// Add a menu item to ANOTHER menu so that it opens a new menu
	// noticed the ampersand '&' and another menu giving the address of the menu
	// that it wants to spawn when activated
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].AddMenuItem(1,"Team Specific Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].AddMenuItem(2,"Give Other Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].AddMenuItem(3,"Edit Paths",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].AddMenuItem(4,"Clear Waypoints",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_CONFIRM_DELETE]);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_CONFIRM_DELETE] = CBotMenu("Clear All Waypoints?");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_CONFIRM_DELETE].AddMenuItem(1,"Yes",BotMenu_Func_DeleteWaypoints);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_CONFIRM_DELETE].AddMenuItem(2,"No",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_MAIN]);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_CONFIRM_DELETE].AddExitMenuItem(5);
	
	if ( gBotGlobals.IsNS() )
	{
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC] = CBotMenu("Team Specific");
		// Add a menu item to which calls a FUNCTION so that it calls the function when activated
		// notice the name of the function you want to call is written here
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(1,"Marines",BotMenu_Func_Team_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(2,"Aliens",BotMenu_Func_Team_Waypoint);
	}
	else
	{
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC] = CBotMenu("Team Specific");
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(1,"Team 1",BotMenu_Func_Team_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(2,"Team 2",BotMenu_Func_Team_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(3,"Team 3",BotMenu_Func_Team_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddMenuItem(4,"Team 4",BotMenu_Func_Team_Waypoint);
	}

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_TEAM_SPEC].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS] = CBotMenu("Give Flags");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS].AddMenuItem(1,"Jump",BotMenu_Func_Jump_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS].AddMenuItem(2,"Crouch-Jump",BotMenu_Func_CrouchJump_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS].AddMenuItem(3,"Crouch",BotMenu_Func_Crouch_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2] = CBotMenu("Give More Flags...");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(1,"Lift Button",BotMenu_Func_Lift_Waypoint);

	if ( gBotGlobals.IsMod(MOD_TS) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(2,"Stunt",BotMenu_Func_Wall_Stick_Waypoint);
	else if ( gBotGlobals.IsMod(MOD_SVENCOOP) || gBotGlobals.IsMod(MOD_TFC) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(2,"Pain/Death",BotMenu_Func_Wall_Stick_Waypoint);
	else
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(2,"Wall-Stick",BotMenu_Func_Wall_Stick_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(3,"Fly",BotMenu_Func_Fly_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS2].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3] = CBotMenu("Give More Flags...");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3].AddMenuItem(1,"Teleport",BotMenu_Func_Teleport_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3].AddMenuItem(2,"Tank",BotMenu_Func_Tank_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3].AddMenuItem(3,"Wait For Lift",BotMenu_Func_WaitLift_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4]);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS3].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4] = CBotMenu("Give More Flags...");

	if ( gBotGlobals.IsMod(MOD_BG) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(1,"Capture Point",BotMenu_Func_EndLevel_Waypoint);
	else if ( gBotGlobals.IsMod(MOD_HL_RALLY) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(1,"End of Race/New Lap",BotMenu_Func_EndLevel_Waypoint);
	else if ( gBotGlobals.IsMod(MOD_TFC) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(1,"Detpack (Also add \"Opens Later\" point at other side)",BotMenu_Func_EndLevel_Waypoint);
	else
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(1,"End Of Level/Objective",BotMenu_Func_EndLevel_Waypoint);
	

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(2,"Stay Close To",BotMenu_Func_StayClose_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(3,"Opens Later",BotMenu_Func_OpensLater_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS4].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5] = CBotMenu("Give More Flags...");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5].AddMenuItem(1,"Human Tower",BotMenu_Func_HumanTower_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5].AddMenuItem(2,"Un-Reachable",BotMenu_Func_Unreachable_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5].AddMenuItem(3,"Ladder",BotMenu_Func_Ladder_Waypoint);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS5].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6] = CBotMenu("More Waypoint Flags...");


	if ( gBotGlobals.IsMod(MOD_TFC) )
	{
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(1,"Sniper Point",Bot_Menu_GrenThrow_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(2,"Sentry Point",BotMenu_Func_Pushable_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(3,"Flag Point",BotMenu_Func_Sci_Waypoint);
	}
	else
	{
		if ( gBotGlobals.IsMod(MOD_HL_RALLY) )
			gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(1,"Start of Race",Bot_Menu_Important_Waypoint);
		else if ( gBotGlobals.IsMod(MOD_SVENCOOP) )
			gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(1,"Throw Grenade",Bot_Menu_GrenThrow_Waypoint);
		else
			gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(1,"Important Waypoint",Bot_Menu_Important_Waypoint);	
		
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(2,"Pushable Point",BotMenu_Func_Pushable_Waypoint);
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(3,"Scientist Point",BotMenu_Func_Sci_Waypoint);
	}

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddMenuItem(4,"More Flags",&gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7]);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS6].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7] = CBotMenu("More Waypoint Flags...");

	if ( gBotGlobals.IsMod(MOD_TFC) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(1,"Capture Point",BotMenu_Func_Barney_Waypoint);
	else
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(1,"Barney Point",BotMenu_Func_Barney_Waypoint);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(2,"Check for lift",BotMenu_CheckForLift_Waypoint);

	//if ( gBotGlobals.IsMod(MOD_TFC) )
	//	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(3,"Defend Zone",BotMenu_Func_Defend_Waypoint);
	/*else*/
	if ( gBotGlobals.IsMod(MOD_COUNTERSTRIKE) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(3,"VIP Rescue Zone",BotMenu_Func_Defend_Waypoint);
	else if ( gBotGlobals.IsMod(MOD_NS) )
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(3,"Marine Building",BotMenu_Func_Defend_Waypoint);
	else
		gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddMenuItem(3,"Defend Zone",BotMenu_Func_Defend_Waypoint);

	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_GIVE_FLAGS7].AddExitMenuItem(5);



	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS] = CBotMenu("Edit Paths");
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS].AddMenuItem(1,"Remove All Paths",BotMenu_Func_Remove_All_Paths);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS].AddMenuItem(2,"Remove Paths TO",BotMenu_Func_Remove_Paths_To);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS].AddMenuItem(3,"Remove Paths FROM",BotMenu_Func_Remove_Paths_From);
	gBotGlobals.m_Menus[BOT_MENU_WAYPOINT_EDIT_PATHS].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN] = CBotMenu("Bot Menu");
	gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].AddMenuItem(1,"Add random bot",BotMenu_Func_AddBot);
	gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].AddMenuItem(2,"Kick random bot",BotMenu_Func_KickBot);
	gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].AddExitMenuItem(5);	

	if ( gBotGlobals.IsNS() )
	{		
		gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].AddMenuItem(3,"Add bot to team",&gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM]);
		gBotGlobals.m_Menus[BOT_MENU_BOT_MAIN].AddMenuItem(4,"Kick bot from team",&gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM]);

		gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM] = CBotMenu ("Kick bot from team");
		gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM].AddMenuItem(1,"Marines",BotMenu_Func_KickBotFromTeam);
		gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM].AddMenuItem(2,"Aliens",BotMenu_Func_KickBotFromTeam);
		gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM].AddMenuItem(3,"Any Team",BotMenu_Func_KickBotFromTeam);
		gBotGlobals.m_Menus[BOT_MENU_KICKBOT_TEAM].AddExitMenuItem(5);


		gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM] = CBotMenu("Add Bot to Team");
		gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM].AddMenuItem(1,"Marines",BotMenu_Func_AddBotToTeam);
		gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM].AddMenuItem(2,"Aliens",BotMenu_Func_AddBotToTeam);
		gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM].AddMenuItem(3,"Auto Team",BotMenu_Func_AddBotToTeam);
		gBotGlobals.m_Menus[BOT_MENU_BOT_ADDBOT_TEAM].AddExitMenuItem(5);
	}

	gBotGlobals.m_Menus[BOT_MENU_SQUAD] = CBotMenu ( "Edit Squad" );
	gBotGlobals.m_Menus[BOT_MENU_SQUAD].AddMenuItem(1,"Edit Formation",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD].AddMenuItem(2,"Edit Member Spread",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_SPREAD]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD].AddMenuItem(3,"Edit Modes",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD].AddMenuItem(4,"More Options",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1] = CBotMenu ( "More Squad Options" );	
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1].AddMenuItem(1,"Make Squad",BotFunc_MakeSquad);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1].AddMenuItem(2,"Leave Squad",BotMenu_Func_Squad_Leave);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1].AddMenuItem(3,"Remove Squad",BotMenu_Func_Squad_Remove);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1].AddMenuItem(4,"Remove All Bot Squads",BotMenu_Func_Squad_RemoveAllBotSquads);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_OPTIONS1].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1] = CBotMenu ( "Squad Modes" );
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1].AddMenuItem(1,"Open/Hold Fire",BotMenu_Func_SquadMode1);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1].AddMenuItem(2,"Keep Low/Stand up",BotMenu_Func_SquadMode1);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1].AddMenuItem(3,"Stealth/Normal",BotMenu_Func_SquadMode1);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1].AddMenuItem(4,"Return to formation",BotMenu_Func_SquadMode1);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_MODES1].AddExitMenuItem(5);
	

	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION] = CBotMenu("Squad Formation");
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION].AddMenuItem(1,"Wedge",BotMenu_Func_Squad_Form);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION].AddMenuItem(2,"Line",BotMenu_Func_Squad_Form);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION].AddMenuItem(3,"Column",BotMenu_Func_Squad_Form);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION].AddMenuItem(4,"More",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2] = CBotMenu("More Squad Formations");
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2].AddMenuItem(1,"Echelon Left",BotMenu_Func_Squad_Form2);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2].AddMenuItem(2,"Echelon Right",BotMenu_Func_Squad_Form2);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2].AddMenuItem(3,"Vee",BotMenu_Func_Squad_Form2);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2].AddMenuItem(4,"Back",&gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION]);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_FORMATION2].AddExitMenuItem(5);

	gBotGlobals.m_Menus[BOT_MENU_SQUAD_SPREAD] = CBotMenu("Squad Formation");
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_SPREAD].AddMenuItem(1,"Wider",BotMenu_Func_Squad_Spread);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_SPREAD].AddMenuItem(2,"Narrower",BotMenu_Func_Squad_Spread);
	gBotGlobals.m_Menus[BOT_MENU_SQUAD_SPREAD].AddExitMenuItem(5);
}

/*


  MENU CODE

*/

CBotMenu :: ~CBotMenu ()
{
	//DestroyMenu ();
}

CBotMenu :: CBotMenu( const char *szCaption )
{
	this->InitMenu();

	m_szCaption = gBotGlobals.m_Strings.GetString(szCaption);
}

void CBotMenu :: DestroyMenu(void)
{
	int i;

	for ( i = 0; i < 10; i ++ )
	{
		if ( m_Menus[i] != NULL )
			m_Menus[i]->Init();
	}
}

void CBotMenu :: InitMenu (void)
{
	memset(m_Menus,0,sizeof(CBotMenuItem*)*10);
	//m_iExitItem = -1;
}

CBotMenuItem :: ~CBotMenuItem ()
{
	//if ( m_pNextMenu )
	//	delete m_pNextMenu;

	Init();
}

CBotMenuItem :: CBotMenuItem ( const char *szMenuCaption, CBotMenu *pNextMenu )
{
	m_szMenuCaption = NULL;
	m_pNextMenu = NULL;

	if ( !szMenuCaption )
		return;

	if ( !*szMenuCaption )
		return;

	if ( !pNextMenu )
		return;

	m_szMenuCaption = gBotGlobals.m_Strings.GetString(szMenuCaption);
	m_pNextMenu = pNextMenu;
}

CBotMenuItem :: CBotMenuItem(const char *szMenuCaption)
{
	this->Init();

	m_szMenuCaption = gBotGlobals.m_Strings.GetString(szMenuCaption);
}

CBotMenuItem :: CBotMenuItem( const char *szMenuCaption, void (*pMenuFunction)(CClient*) )
{
	m_szMenuCaption = NULL;
	m_pNextMenu = NULL;

	if ( !szMenuCaption )
		return;

	if ( !*szMenuCaption )
		return;

	if ( !pMenuFunction )
		return;

	m_szMenuCaption = gBotGlobals.m_Strings.GetString(szMenuCaption);
	m_pMenuFunction = pMenuFunction;
}

void CBotMenuItem :: Activate(CClient *pClient )
{
	if ( m_pNextMenu != NULL )
	{
		pClient->m_pMenu = m_pNextMenu;
		m_pNextMenu->Render(pClient);
	}
	else if ( m_pMenuFunction )
	{
		// bug fix : Nullify current menu
		pClient->m_pMenu = NULL;
		(*m_pMenuFunction)(pClient);
	}
	else
	{
		pClient->m_pMenu = NULL;
	}
}

void CBotMenu :: Render ( CClient *pClient )
{
	int iSlots;
	int i;
	char szMenuText[512];
	char szMenuItemText[64];
	
	if ( pClient == NULL )
		return;
	
	szMenuText[0] = 0;
	szMenuItemText[0] = 0;
	
	iSlots = 0;
	
	pClient->m_pMenu = this;
	
	sprintf(szMenuText,"%s\n-----\nOptions:\n",m_szCaption);	

	for ( i = 0; i < 10; i ++ )
	{
		if ( m_Menus[i] != NULL )
		{			
			if ( i == 0 )
				iSlots |= (1<<9);
			else
				iSlots |= (1<<(i-1));

			if ( m_Menus[i]->HasNextMenu() )
				sprintf(szMenuItemText,"%d. %s...\n",i,m_Menus[i]->GetCaption());
			else
				sprintf(szMenuItemText,"%d. %s\n",i,m_Menus[i]->GetCaption());

			strcat(szMenuText,szMenuItemText);
		}
	}
	
	UTIL_ShowMenu(pClient->GetPlayer(),iSlots,-1,FALSE,szMenuText);
}

/*

  MENU FUNCTIONS

  functions that are mainly called through menus

*/

void BotMenu_Func_Jump_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( WaypointFlags(iWpt) & W_FL_JUMP )
		waypoints[iWpt].flags &= ~W_FL_JUMP;
	else
		waypoints[iWpt].flags |= W_FL_JUMP;
}

void BotMenu_Func_Defend_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( WaypointFlags(iWpt) & W_FL_DEFEND_ZONE )
		waypoints[iWpt].flags &= ~W_FL_DEFEND_ZONE;
	else
		waypoints[iWpt].flags |= W_FL_DEFEND_ZONE;
}


void BotMenu_Func_CrouchJump_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( WaypointFlags(iWpt) & W_FL_CROUCHJUMP )
		waypoints[iWpt].flags &= ~W_FL_CROUCHJUMP;
	else
		waypoints[iWpt].flags |= W_FL_CROUCHJUMP;
}

void BotMenu_Func_Crouch_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( WaypointFlags(iWpt) & W_FL_CROUCH )
		waypoints[iWpt].flags &= ~W_FL_CROUCH;
	else
		waypoints[iWpt].flags |= W_FL_CROUCH;
}

void BotMenu_Func_Wall_Stick_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( WaypointFlags(iWpt) & W_FL_WALL_STICK )
		waypoints[iWpt].flags &= ~W_FL_WALL_STICK;
	else
		waypoints[iWpt].flags |= W_FL_WALL_STICK;
}

void BotMenu_Func_Remove_All_Paths ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	int i;

	for ( i = 0; i < MAX_WAYPOINTS; i ++ )
		WaypointDeletePath(iWpt, i);

	WaypointDeletePath(iWpt);
}

void BotMenu_Func_Remove_Paths_To ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	WaypointDeletePath(iWpt);
}

void BotMenu_Func_Remove_Paths_From ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	int i;

	for ( i = 0; i < MAX_WAYPOINTS; i ++ )
		WaypointDeletePath(iWpt, i);
}

void BotMenu_Func_Lift_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_LIFT)
	{
		waypoints[iWpt].flags &= ~W_FL_LIFT;
	}
	else
		waypoints[iWpt].flags |= W_FL_LIFT;
}

void BotMenu_Func_Squad_Spread ( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);
	
	if ( pSquad == NULL )
	{
		ClientPrint(pEntity,HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}
	
	switch ( pClient->m_iLastMenuItemSelected )
	{
	case 1:
		pSquad->ChangeSpread(pSquad->GetSpread()+24);
		break;
	case 2:
		pSquad->ChangeSpread(pSquad->GetSpread()-24);
		break;
	}
}

void BotMenu_Func_Squad_Form ( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if ( pSquad == NULL )
	{
		ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}

	switch ( pClient->m_iLastMenuItemSelected )
	{
	case 1:
		// wedge
		pSquad->ChangeFormation(SQUAD_FORM_WEDGE);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now WEDGE\n");
		break;
	case 2:
		//line
		pSquad->ChangeFormation(SQUAD_FORM_LINE);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now LINE\n");
		break;
	case 3:
		//column
		pSquad->ChangeFormation(SQUAD_FORM_COLUMN);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now COLUMN\n");
		break;
	}

}

void BotMenu_Func_Squad_Form2 ( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if ( pSquad == NULL )
	{
		ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}

	switch ( pClient->m_iLastMenuItemSelected )
	{
	case 1:
		//echelon
		pSquad->ChangeFormation(SQUAD_FORM_ECH_LEFT);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now ECHELON LEFT\n");
		break;
	case 2:
		//line
		pSquad->ChangeFormation(SQUAD_FORM_ECH_RIGHT);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now ECHELON RIGHT\n");
		break;
	case 3:
		//column
		pSquad->ChangeFormation(SQUAD_FORM_VEE);
		ClientPrint(pEntity,HUD_PRINTTALK,"Squad formation is now VEE\n");
		break;
	}

}


void BotMenu_Func_Team_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;
	if ( pClient->m_iLastMenuItemSelected == -1 )
        return;
    if ( pClient->m_iLastMenuItemSelected > 4 )
        return;
    
	if (waypoints[iWpt].flags & W_FL_TEAM_SPECIFIC)
	{
		waypoints[iWpt].flags &= ~W_FL_TEAM;
		waypoints[iWpt].flags &= ~W_FL_TEAM_SPECIFIC; // off
	}
	else
	{
		int team = pClient->m_iLastMenuItemSelected;
		
		team--;  // make 0 to 3
		
		// this is kind of a kludge (team bits MUST be LSB!!!)
		waypoints[iWpt].flags |= team;
		waypoints[iWpt].flags |= W_FL_TEAM_SPECIFIC; // on
	}
}

void BotMenu_Func_Fly_Waypoint			( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_FLY)
	{
		waypoints[iWpt].flags &= ~W_FL_FLY;
	}
	else
		waypoints[iWpt].flags |= W_FL_FLY;
}

void BotMenu_Func_Teleport_Waypoint		( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_TELEPORT)
	{
		waypoints[iWpt].flags &= ~W_FL_TELEPORT;
	}
	else
		waypoints[iWpt].flags |= W_FL_TELEPORT;
}

void BotMenu_Func_Tank_Waypoint			( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_TANK)
	{
		waypoints[iWpt].flags &= ~W_FL_TANK;
	}
	else
		waypoints[iWpt].flags |= W_FL_TANK;
}

void BotMenu_Func_WaitLift_Waypoint		( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_WAIT_FOR_LIFT)
	{
		waypoints[iWpt].flags &= ~W_FL_WAIT_FOR_LIFT;
	}
	else
		waypoints[iWpt].flags |= W_FL_WAIT_FOR_LIFT;
}

void BotMenu_Func_EndLevel_Waypoint		( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_ENDLEVEL)
	{
		waypoints[iWpt].flags &= ~W_FL_ENDLEVEL;
	}
	else
		waypoints[iWpt].flags |= W_FL_ENDLEVEL;
}

void BotMenu_Func_KickBot               ( CClient *pClient )
{
	int i;

	CBot *pBot;

	dataUnconstArray<CBot*> m_Bots;

	m_Bots.Init();

	for ( i = 0; i < MAX_PLAYERS; i ++ )
	{		
		pBot = &gBotGlobals.m_Bots[i];

		if (!pBot->m_bIsUsed || !pBot->m_pEdict || (pBot->m_iRespawnState != RESPAWN_IDLE))
			continue;

		m_Bots.Add(pBot);		
	}

	if ( m_Bots.IsEmpty() )
		return;

	assert(m_Bots.Size() > 0);

	pBot = m_Bots.Random();

	if ( pBot )
		pBot->m_bKick = TRUE;

	m_Bots.Clear();
}

void BotMenu_Func_AddBot ( CClient *pClient )
{
	BotFunc_AddBot(pClient,NULL,NULL,NULL,NULL);
}

void BotMenu_Func_AddBotToTeam ( CClient *pClient )
{
	switch ( pClient->m_iLastMenuItemSelected )
	{
	case 1:
		BotFunc_AddBot(pClient,"1",NULL,NULL,NULL);
		break;
	case 2:
		BotFunc_AddBot(pClient,"2",NULL,NULL,NULL);
		break;
	case 3:
		BotFunc_AddBot(pClient,"5",NULL,NULL,NULL);
		break;
	}
}

void BotMenu_Func_StayClose_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_STAY_NEAR)
	{
		waypoints[iWpt].flags &= ~W_FL_STAY_NEAR;
	}
	else
		waypoints[iWpt].flags |= W_FL_STAY_NEAR;
}

void BotMenu_Func_OpensLater_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_OPENS_LATER)
	{
		waypoints[iWpt].flags &= ~W_FL_OPENS_LATER;
	}
	else
		waypoints[iWpt].flags |= W_FL_OPENS_LATER;
}

void BotMenu_Func_Unreachable_Waypoint  ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_UNREACHABLE)
	{
		waypoints[iWpt].flags &= ~W_FL_UNREACHABLE;
	}
	else
		waypoints[iWpt].flags |= W_FL_UNREACHABLE;

	BotMenu_Func_Remove_All_Paths(pClient);
}

void BotMenu_Func_Ladder_Waypoint	  ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_LADDER)
	{
		waypoints[iWpt].flags &= ~W_FL_LADDER;
	}
	else
		waypoints[iWpt].flags |= W_FL_LADDER;
}

void BotMenu_Func_HumanTower_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_HUMAN_TOWER)
	{
		waypoints[iWpt].flags &= ~W_FL_HUMAN_TOWER;
	}
	else
		waypoints[iWpt].flags |= W_FL_HUMAN_TOWER;
}

void Bot_Menu_Important_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_IMPORTANT)
	{
		waypoints[iWpt].flags &= ~W_FL_IMPORTANT;
	}
	else
		waypoints[iWpt].flags |= W_FL_IMPORTANT;
}

void Bot_Menu_GrenThrow_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_GREN_THROW)
	{
		waypoints[iWpt].flags &= ~W_FL_GREN_THROW;
	}
	else
		waypoints[iWpt].flags |= W_FL_GREN_THROW;
}

void BotMenu_Func_SquadMode1 ( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if ( pSquad == NULL )
	{
		ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}

	switch ( pClient->m_iLastMenuItemSelected )
	{
	case 1:// open / hold fire
		pSquad->ToggleFireMode();
		break;
	case 2: // keep low / stand up
		if ( pSquad->IsCrouchMode() )
			pSquad->SetCombatType(COMBAT_COMBAT);
		else
			pSquad->SetCombatType(COMBAT_CROUCH);
		break;
	case 3: // stealth / normal
		if ( pSquad->IsStealthMode() )
			pSquad->SetCombatType(COMBAT_COMBAT);
		else
			pSquad->SetCombatType(COMBAT_STEALTH);

		break;
	case 4:
		{
			pSquad->ReturnAllToFormation();
		}
	}
}

void BotMenu_Func_Squad_Leave           ( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if ( pSquad == NULL )
	{
		ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}

	gBotGlobals.m_Squads.ChangeLeader(pSquad);

	ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You have left your squad\n");
}

void BotMenu_Func_Squad_Remove			( CClient *pClient )
{
	edict_t *pEntity;
	CBotSquad *pSquad;

	pEntity = pClient->GetPlayer();

	pSquad = gBotGlobals.m_Squads.FindSquadByLeader(pEntity);

	if ( pSquad == NULL )
	{
		ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"You are not leader of a squad\n");
		return;
	}

	gBotGlobals.m_Squads.RemoveSquad(pSquad);

	ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,"Squad removed\n");
}

void BotMenu_Func_Squad_RemoveAllBotSquads			( CClient *pClient )
{
	int i;
	CBot *pBot;

	char msg[64];
	int iCount = 0;

	if ( pClient == NULL )
		return; // cant get player

	edict_t *pEntity = pClient->GetPlayer();

	if ( !pEntity )
		return; // cant get player
	
	for ( i = 0; i < MAX_PLAYERS; i ++ )
	{
		pBot = &gBotGlobals.m_Bots[i];
		
		if ( !pBot || !pBot->IsUsed() )
			continue;
		// only remove bot squads on same team to avoid
		// players removing squads from other teams
		if ( UTIL_GetTeam(pEntity) == pBot->GetTeam() )
		{
			if ( pBot->RemoveMySquad() )
				iCount++;		
		}
	}

	sprintf(msg,"%s %d bot squads removed\n",BOT_DBG_MSG_TAG,iCount);
	ClientPrint(pClient->GetPlayer(),HUD_PRINTTALK,msg);
}

void BotMenu_Func_DeleteWaypoints ( CClient *pClient )
{
	WaypointInit();
}

void BotMenu_Func_KickBotFromTeam			( CClient *pClient )
{
	int i;

	// list of possible bots to kick
	dataUnconstArray<CBot *> theBots;
	CBot *pBot;

	int iBotTeam;
	
	for ( i = 0; i < MAX_PLAYERS; i ++ )
	{		
		pBot = &gBotGlobals.m_Bots[i];
		
		if ( pBot && pBot->IsUsed() )
		{
			iBotTeam = UTIL_GetTeam(pBot->m_pEdict);
			
			switch ( gBotGlobals.m_iCurrentMod )
			{
			case MOD_NS:
				{
					switch ( pClient->m_iLastMenuItemSelected )
					{
					case 1:				
						// marines
						if ( iBotTeam == TEAM_MARINE )
							theBots.Add(pBot);
						break;
					case 2:
						if ( iBotTeam == TEAM_ALIEN )
							theBots.Add(pBot);
						// aliens
						break;
					case 3:
						theBots.Add(pBot);
						// any team
						break;
					}
				}
				break;
			default:
				break;
			}
			
		}
	}

	if ( !theBots.IsEmpty() )
	{
		pBot = theBots.Random();

		if ( pBot )
			pBot->m_bKick = TRUE;
			
		theBots.Clear();
	}

	return;
}

void BotMenu_Func_Pushable_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_PUSHABLE)
	{
		waypoints[iWpt].flags &= ~W_FL_PUSHABLE;
	}
	else
		waypoints[iWpt].flags |= W_FL_PUSHABLE;
}

void BotMenu_Func_Sci_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_SCIENTIST_POINT)
	{
		waypoints[iWpt].flags &= ~W_FL_SCIENTIST_POINT;
	}
	else
		waypoints[iWpt].flags |= W_FL_SCIENTIST_POINT;
}

void BotMenu_CheckForLift_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( waypoints[iWpt].flags & W_FL_CHECK_LIFT )
		waypoints[iWpt].flags &= ~W_FL_CHECK_LIFT;
	else
		waypoints[iWpt].flags |= W_FL_CHECK_LIFT;

}
/*
void BotMenu_CheckForLift_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if ( waypoints[iWpt].flags & iFlags )
		waypoints[iWpt].flags &= ~iFlags;
	else
		waypoints[iWpt].flags |= iFlags;

}
*/
void BotMenu_Func_Barney_Waypoint ( CClient *pClient )
{
	int iWpt = pClient->m_iCurrentWaypoint;

	if ( iWpt == -1 )
		return;

	if (waypoints[iWpt].flags & W_FL_BARNEY_POINT)
	{
		waypoints[iWpt].flags &= ~W_FL_BARNEY_POINT;
	}
	else
		waypoints[iWpt].flags |= W_FL_BARNEY_POINT;
}

/*
  TODO, make OO'd

  to make OO'd, something like...


class CMenuItem
{
	virtual void Action ( CClient *pClient );
};

class CRemoveSquad : public CMenuItem
{
	void Action ( CClient *pClient );
	{
		// remove squad
	}
};

class CMenu : public CMenuItem
{
	void Action ( CClient *pClient )
	{
		pClient->m_pMenu = this;
		Render(pClient);
	}

    void Render ( CClient *pClient )
	{
		...
	}
};
*/