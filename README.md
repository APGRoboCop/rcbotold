# rcbotold
RCbot created by Cheeseh

--------------
Supported Mods
--------------

- Bumpercars
- Battlegrounds
- Deathmatch Classic
- Opposing Force (CTF)
- Natural Selection
- Rocket Crowbar
- Science and Industry
- The Specialists
- Wizard Wars

RCBot versions
--------------

V 1.51-beta3
---------

* added: Enhanced performance
* added: AMBuild compiling support
* added: Opposing Force CTF, S&I and WizWars Support
* added: C++17 Standard library support 
* added: Strafing combat to evade from being easy target bots
* changed: Increased the bot's flexability
* changed: Reduced redundant coding
* changed: Reverted basemonster.h that was modified for Sven Coop
* changed: The bot add or spawn delay for 10-12 seconds to prevent crashes
* changed: Increasing bot's viewing angle to detect enemies better
* changed: Reduced further Sven Co-op and TFC support
* changed  Welcome Message modernised and is now displaying properly
* changed: Converted some doubles into floats
* changed: Added returns for NS CAlienAction 
* changed: NS Squad Menu repaired and should hopefully work
* TODO: To allow Skulk Aliens to bite and attack properly
* TODO: To allow bots to detect enemies and incoming damages properly
* TODO: To prevent bots in TS v3.0 on firing empty rounds and lying proning on floors

V 1.5-final
---------

* added: Few waypoints for The Specialists v3.0 and Natural Selection v3.x
* added: The Specialists v3.0 support, big thanks to Cheeseh and some admins from tsmod.net
* added: Performance optimisations
* changed: Did a Code Cleanup to allow the builds to be more streamlined
* changed: Replaced most of the C style casts with C++ casts
* changed: Sven Co-op v5 or later aren't supported anymore use RCBot AngelScript instead
* changed: dropped support for TFC in favour for FoxBot
* fixed: Some potential errors and bugs have been wiped
* TODO: To prevent bots in TS v3.0 on firing empty rounds and lying proning on floors
* TODO: Enhance their combat tactics by having better enemy detection
* TODO: To learn properly on avoiding taking too much falling damage and die in lava pits - despite using proper waypoints
* TODO: To fix that NS Squad Joining Bug or enhance it

V 1.443
---------

* added: linux build optimised for floating-point arithmetic and tuned for newer arch
* added: waypoint console info for adding and delete a single waypoint
* added: support for Metamod 1.19 or higher
* added: support for new Sven Co-op v5 weapons like the M16, Deagle, SporeGun and ShockRoach (not crucial)
* changed: bot_navigate.cpp line 471
* changed: RCBot compiled with HLSDK 2.3-p4 and Metamod SDK v1.21
* changed: all builds optimised for Pentium 4 SSE2 or higher processors
* fixed: extdll.h that appears to fail to compile on GCC 6+ for #ifndef min and max
* note: bot_ga.cpp appears to be unstable with BotGAValues::get(int)()

V 1.3
---------

* added: support for Battlegrounds mod.
* added: support for half-life deathmatch.
* added: megaHAL bot chatting [thanks pierre at racc bot (racc.bots-united.com)].
* added: new command "rcbot config chat_percent" and "rcbot config chat_reply_percent" (See commands text file)
* added: new command "rcbot config marine_auto_build". If set to 1, an infantry portal
       will automatically be built nearby the command console when a bot joins the server.
* added: program does not allow clients to join if min bots has not been reached (if bots still need to join).
* added: new config command reserve_bot_slots (to configure above addition).
* added: ability to kick bots from certain team in natural selection in bot menu.
* added: ability to make a squad through squad menu.
* added: ability to return bots in squad back to formation in squad menu.
* added: more lift/elevator functionality.
* added: bots in svencoop search for weapons they need if they dont have weapon to kill gargantuas etc.
* added: improved Aliens in NS (they check out buildings under attack etc)
* added: new commands "config balance_teams" "config
* fixed: fixed NS Gorges not building much.
* fixed: bots using all weapons in DMC/all known bugs in DMC fixed.
* fixed: crash bug in natural selection attack code.
* fixed: bots now treat "func_train" entities like doors/lifts.
* fixed: bots killing themselves when waiting for commander orders in NS.
* fixed: bots join team in NS one at a time incase of crashing.
* fixed: wait_for_orders command not working in NS.

V 1.2
---------

All Mods
--------
* fixed: bots going out of water better.
* fixed: bots 'hearing' dead players (spectators)
* fixed: bots not moving sometimes when they cant find an objective waypoint.
* fixed: bot menus interfering with other metamod plugin menus
* fixed: bots know what buttons to press (hopefully) for doors that use 'masters'
* fixed: bots know if doors can be opened if there is an opens later waypoint through it.
* fixed: bots know buttons that can open doors that use masters.
* fixed: bug when bots re-joining server after map change if kicked.
* fixed: bug when bots not returning to server with max bots set after clients leave game.
* fixed: Waypoint visibility files reduced in size

* changed: some ladder stuff

* added: more performance commands (max_update_vision_revs, update_vision_time) see commands text file
* added: some stuck in water code
* added: bots kill self if they can't move for a while (can configure with 'bot_stuck_speed' command)
* added: update_ladder_time command for configuring bots ladder behaviour
* added: bots look for/attack enemies that killed team mates that they see die.
* added: Squad code, player can make bots join their squad, bots can create their own squads, ability to alter formation, leave squad (as leader), remove squad, remove all squads (in case of problems)
* added: classic +force_grip command from old rcbot

Sven Coop
---------
* fixed: bots not using teleports very often

Natural Selection
-----------------
* added: aliens won't melee attack electrified structures
* added: marines don't waste ammo when attacking defense chambers and resource towers (use knife or welder)

Bumper Cars
-----------
* added: bots using bombs in bumpercars
* added: bots choosing random model in bumpercars
* added: some better movement in bumpercars
* added: bots attack grunts in hunt the grunt maps

V 1.1
---------
* fixed: killbot command not working in metamod
* fixed: bot menu kick bot option not working
* fixed: crash bugs in task code
* fixed: weapon selection problems (when having to reload)
* fixed: problems when marines are out of ammo in NS (didnt attempt to change weapon?).
* fixed: some DMC weapon problems.
* fixed: aliens getting stuck trying to evolve in places where they cant.
* fixed: problem with some config files not loading in linux
* fixed: bots trying to use buttons too high up.
* fixed: bots shooting all enemies in svencoop and not shooting all allies.
* fixed: crashes in metamod when using other plugins. ("no meta return in args()" bug)
* fixed: crash bugs with bot chat & bot events
* fixed: bots go closer to hev/health chargers, they sometimes got stuck.
* fixed: when bots visit all enemy positions they will continue to objective.
* fixed: bots will only try to use armor chargers/pick up armor when they have less than 50 armor.
* fixed: couldn't delete waypoints that wern't visible (e.g. through walls)
* fixed: bots listening to sounds better.
* fixed: gorge behaviour, should build hives & resource towers more often
* fixed: bots staying in team their were added to after map change
* fixed: marines properly avoiding infantry portals in NS to avoid telefrag kills.
* fixed: bots shoot 'explosive only' breakables / gargantuas with RPGs or grenades, 
        and dont shoot them if they dont have any explosive weapons (in Svencoop)
* fixed: bots should be able to shoot barnacles in svencoop
* added: alien gorges & marines build objects as they see them.
* added: bots take cover when reloading (in svencoop)
* added: map specific configs
* added: 'unreachable' waypoint types (helps bots find cover areas)

V 1.01
--------

* fixed: Altered weapons config (you can also edit it) to make bots not to use
       the sniper rifle too close to the enemy in SvenCoop beta 3.0.

* fixed: Small bug with bot menu (couldn't exit menu)

* fixed: Small bug where NS alien bots might try to drop their leap 
       weapon before attacking (which they cant do) causing them 
       not to shoot.

* fixed: Bug with alien bots not when they dont have any long range weapons

* fixed: fixed bot profile layout (removed [bot] from each one, not needed in this version)
       and fixed repetitions of names in some profiles.

* fixed: Bug in pathfinding algorithm, causing long loops.

* fixed: error in linux_install text file.

* added: Significant reduce in CPU Usage

* added: New command "rcbot config max_path_revs" the lower the value, better CPU
       performance, but takes bots slightly longer in time to find a path.

* added: Human tower waypoint, bots will crouch for a player to jump on them or jump
       on a player thats already crouching nearby it. (I have added this waypoint into
       the map "extension" to test it)

* added: ability to add bots to certain teams in Natural-selection using the bot menu.

V. 1.0
--------
General:

-: Supports Mods: NS 2.0+, SvenCoop 2.1/3.0, DMC, HL Bumpercars 1.3.
-: Seperate profile for each bot, means each bot is unique
-: New task system.
-: New bot chat system.
-: New movement code, bots can target onto enemies while moving along their path.
-: Bot reputation system allows bots to like/dislike players.
-: Hopefully a reduce in CPU usage.
-: Metamod version and normal version for windows available.
-: Seperate weapon configuration file can be edited by the user.

AI:

-: Bots remember positions of enemies and return to them if they die.
-: Bots now always jump & crouch to jump higher.
-: Bots can use lifts/doors better
-: Bots can try to run for cover if they get damaged.

Svencoop:

They CAN:

-: Do long jumps (using crouch-jump waypoint)
-: Shoot monsters
-: Use chaingun's and other new weapons in Svencoop 3
-: Don't shoot scientists/barneys
-: Use approriate door buttons
-: Use "use only" doors
-: Heal players
-: Attempts to heal players that shout "medic!" (If the bot sees the player)
-: Take cover when they get damaged
-: Take cover when they see a grenade
-: Take cover when someone yells "take cover!" (If the bot sees the player)
-: Use health / armor rechargers
-: Swim like champions :p have a bit of trouble getting out of water though :(

They CAN'T

-: Do human towers.
-: Carry out complicated objectives.

NS:

-: Gorges should be improved.
-: Gorges ask for help if under attack

New waypoint types:

-: Fly waypoint (For NS at the moment for Lerk, Jetpack etc).
-: End Level/Objective waypoint.
-: Crouch-Jump waypoint.
-: Lift button waypoint.
-: Wait for lift waypoint works now.
-: Teleport waypoint (Similar to teleport waypoint in RCBot 1.5 for RC)
-: Weapon,ammo,health,armor waypoints.
-: Stay close to waypoint (Bot slows down and stays close to the waypoint)
-: Opens Later Waypoint (Useful for svencoop, denotes the waypoint is in an
-- area that opens up later, example: a teleport).

PROBLEMS Still occuring in this version:

-: Ladder climbing isn't very good :(
-: Skulks can't wall-stick very well :(

LIMITATIONS of the version of the bot

-: Bots depend on more waypoints in more areas
-- i.e. if they can't find a waypoint nearby they won't move
-- and waypoints in more areas give bots better cover places

COMMANDS

See rcbot_commands.txt for details

http://rcbot.bots-united.com/

1. Install & Supported Mods
2. Playing
3. Commands
4. Waypoints
5. Admin Use
6. Faq
7. Reporting bugs
8. Credits

--------
1. Install
--------

First of all EXTRACT all files into the Half-Life Folder, it should create a
folder called rcbot inside HL folder with all the files in it.

REMEMBER : Extract it to Half-Life folder ONLY, NOT the MOD folder i.e.
bot files must be in Half-Life/rcbot

For LINUX see the linux_install.txt file for details.

---------
Windows : 
---------
WITHOUT METAMOD:

Make sure you have downloaded the NON-metamod version for WINDOWS.

Extract the zip into the Half-life folder, make sure all files are
in the Half-Life folder/rcbot directory.

Open the liblist.gam file (in the MOD folder that you want to play)...
with a suitable text editing program and then EDIT the line (do not 
make a copy of the line or else the bot won't work!):

gamedll "<MOD_DLL>"

to 

gamedll "..\rcbot\dlls\rcbot.dll"

gamedll_linux "../rcbot/dlls/rcbot.so"

--------------------------------------------------------------
WITH METAMOD:

Make sure you have downloaded the METAMOD version of the bot for WINDOWS.

Extract the zip into the Half-life folder, make sure all files are
in the Half-Life folder/rcbot directory.

Go into addons/metamod directory and EDIT the plugins.ini file

Add the line:

win32 ..\rcbot\dlls\rcbot_mm.dll

linux ../rcbot/dlls/rcbot_mm.so

to the file.

--------
2. Playing
--------

Once you've created  a game, and you want to add bots, use the  console command
"rcbot addbot"

-OR- bind a key to "rcbot bot_menu"

and Select "Add Random Bot" To add a bot.

If playing NS you can use the bot menu to tell which team you want the new bot
to join. Select the "Add bot to team" menu item and select the team you want.

You can also kick bots from the game by using the bot menu (bot_menu command).

You can set up min_bots and max_bots setting in the config file.
Read the rcbot_commands.txt about these commands.

--------
3. Commands
--------

The commands are slightly different from last versions but most commands are the same
but to access them you must type "rcbot" before each, incase other bots are running
and have the same commands.

Read the rcbot_commands.txt file about the commands.

--------
4. Waypoints
--------

The waypoints are slightly different from last versions but most commands are the same
but to access them you must type "rcbot" before each, incase other bots are running
and have the same commands.

Read the rcbot_commands.txt file about the commands or rcbot_waypoint_readme.txt

--------
5. Admin Use
--------

If you want to have access to bot commands on a dedicated server or another server
you must set up your name and password on the bot_users.ini file on the server.

Open and read the bot_users.ini file for details on how to make a user.

Then whilst in the game your name must be that same name as written in the config file,
and type the command

rcbot set_pass <your password in bot_users.ini file>

this will authenticate you and if successful you should have access to the bot commands
with the access level written in the bot_users.ini next to your name & password.

--------
6. Faq
--------

SEE faq.htm that comes with the bot! It will be in the rcbot folder.

!!!Also, check out the FAQ page on the rcbot site for the latest FAQ page!!!

-------------------
7. Reporting Bugs
-------------------

Please follow these steps.

i.   Make sure the bug hasn't been mentioned as a known problem
ii.  Check the rcbot forum (http://rcbot.bots-united.com/forums) for current bug reports made
iii. Post in forum or e-mail me with :

     What map you were playing,
     What you think the bot(s) are trying to do,
     Info in your config file.

--------------
8. Credits
--------------

- Cheeseh : RCBot founder
- RoboCop : For compiling Linux builds and adding waypoints
- Anonymous Player : For adding AMBuild support
- Botman : For the main HPB_Bot template
- Flayra : For NS and code needed to get a bot to work with NS
- Sven Viking & Sniper: For SvenCoop and allowing me access to Beta versions to allow bot support  
- PM (racc.bots-united.com) : For good FakeClientCommand code.
- Jason Hutchens : For MegaHAL used for bot personality
- SandMan : For waypointing NS maps
- RedFox : For the BotCam code from FoXBot
- Tsmod.net : For allowing TS v3.0 support
- Tschumann : For the code needed for Op4CTF