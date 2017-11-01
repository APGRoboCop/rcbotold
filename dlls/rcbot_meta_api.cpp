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
// rcbot_meta_api.cpp
//
//////////////////////////////////////////////////
//
// Required for metamod.
//

#ifdef RCBOT_META_BUILD

#include "extdll.h"
#include "h_export_meta.h"
//#include "api_info.h"
#include "meta_api.h"
#include "bot_const.h"
#include "bot.h"

#include "studio.h"

#include "engine.h"

meta_globals_t *gpMetaGlobals;      // metamod globals

// From SDK dlls/h_export.cpp:
	extern cvar_t bot_ver_cvar;
//! Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;

int debug_engine;
char *g_argv;

enginefuncs_t meta_engfuncs = 
{
	NULL,						// pfnPrecacheModel()
	NULL,						// pfnPrecacheSound()
	NULL,						// pfnSetModel()
	NULL,						// pfnModelIndex()
	NULL,						// pfnModelFrames()

	NULL,						// pfnSetSize()
	NULL,						// pfnChangeLevel()
	NULL,						// pfnGetSpawnParms()
	NULL,						// pfnSaveSpawnParms()

	NULL,						// pfnVecToYaw()
	NULL,						// pfnVecToAngles()
	NULL,						// pfnMoveToOrigin()
	NULL,						// pfnChangeYaw()
	NULL,						// pfnChangePitch()

	NULL,						// pfnFindEntityByString()
	NULL,						// pfnGetEntityIllum()
	NULL,						// pfnFindEntityInSphere()
	NULL,						// pfnFindClientInPVS()
	NULL,						// pfnEntitiesInPVS()

	NULL,						// pfnMakeVectors()
	NULL,						// pfnAngleVectors()

	NULL,						// pfnCreateEntity()
	NULL,						// pfnRemoveEntity()
	NULL,						// pfnCreateNamedEntity()

	NULL,						// pfnMakeStatic()
	NULL,						// pfnEntIsOnFloor()
	NULL,						// pfnDropToFloor()

	NULL,						// pfnWalkMove()
	NULL,						// pfnSetOrigin()

	NULL,						// pfnEmitSound()
	NULL,						// pfnEmitAmbientSound()

	NULL,						// pfnTraceLine()
	NULL,						// pfnTraceToss()
	NULL,						// pfnTraceMonsterHull()
	NULL,						// pfnTraceHull()
	NULL,						// pfnTraceModel()
	NULL,						// pfnTraceTexture()
	NULL,						// pfnTraceSphere()
	NULL,						// pfnGetAimVector()

	NULL,						// pfnServerCommand()
	NULL,						// pfnServerExecute()
	NULL,						// pfnClientCommand()

	NULL,						// pfnParticleEffect()
	NULL,						// pfnLightStyle()
	NULL,						// pfnDecalIndex()
	NULL,						// pfnPointContents()

	NULL,						// pfnMessageBegin()
	NULL,						// pfnMessageEnd()

	NULL,						// pfnWriteByte()
	NULL,						// pfnWriteChar()
	NULL,						// pfnWriteShort()
	NULL,						// pfnWriteLong()
	NULL,						// pfnWriteAngle()
	NULL,						// pfnWriteCoord()
	NULL,						// pfnWriteString()
	NULL,						// pfnWriteEntity()

	NULL,						// pfnCVarRegister()
	NULL,						// pfnCVarGetFloat()
	NULL,						// pfnCVarGetString()
	NULL,						// pfnCVarSetFloat()
	NULL,						// pfnCVarSetString()

	NULL,						// pfnAlertMessage()
	NULL,						// pfnEngineFprintf()

	NULL,						// pfnPvAllocEntPrivateData()
	NULL,						// pfnPvEntPrivateData()
	NULL,						// pfnFreeEntPrivateData()

	NULL,						// pfnSzFromIndex()
	NULL,						// pfnAllocString()

	NULL, 						// pfnGetVarsOfEnt()
	NULL,						// pfnPEntityOfEntOffset()
	NULL,						// pfnEntOffsetOfPEntity()
	NULL,						// pfnIndexOfEdict()
	NULL,						// pfnPEntityOfEntIndex()
	NULL,						// pfnFindEntityByVars()
	NULL,						// pfnGetModelPtr()

	NULL,						// pfnRegUserMsg()

	NULL,						// pfnAnimationAutomove()
	NULL,						// pfnGetBonePosition()

	NULL,						// pfnFunctionFromName()
	NULL,						// pfnNameForFunction()

	NULL,						// pfnClientPrintf()
	NULL,						// pfnServerPrint()

	NULL,						// pfnCmd_Args()
	NULL,						// pfnCmd_Argv()
	NULL,						// pfnCmd_Argc()

	NULL,						// pfnGetAttachment()

	NULL,						// pfnCRC32_Init()
	NULL,						// pfnCRC32_ProcessBuffer()
	NULL,						// pfnCRC32_ProcessByte()
	NULL,						// pfnCRC32_Final()

	NULL,						// pfnRandomLong()
	NULL,						// pfnRandomFloat()

	NULL,						// pfnSetView()
	NULL,						// pfnTime()
	NULL,						// pfnCrosshairAngle()

	NULL,						// pfnLoadFileForMe()
	NULL,						// pfnFreeFile()

	NULL,						// pfnEndSection()
	NULL,						// pfnCompareFileTime()
	NULL,						// pfnGetGameDir()
	NULL,						// pfnCvar_RegisterVariable()
	NULL,						// pfnFadeClientVolume()
	NULL,						// pfnSetClientMaxspeed()
	NULL,						// pfnCreateFakeClient()
	NULL,						// pfnRunPlayerMove()
	NULL,						// pfnNumberOfEntities()

	NULL,						// pfnGetInfoKeyBuffer()
	NULL,						// pfnInfoKeyValue()
	NULL,						// pfnSetKeyValue()
	NULL,						// pfnSetClientKeyValue()

	NULL,						// pfnIsMapValid()
	NULL,						// pfnStaticDecal()
	NULL,						// pfnPrecacheGeneric()
	NULL, 						// pfnGetPlayerUserId()
	NULL,						// pfnBuildSoundMsg()
	NULL,						// pfnIsDedicatedServer()
	NULL,						// pfnCVarGetPointer()
	NULL,						// pfnGetPlayerWONId()

	NULL,						// pfnInfo_RemoveKey()
	NULL,						// pfnGetPhysicsKeyValue()
	NULL,						// pfnSetPhysicsKeyValue()
	NULL,						// pfnGetPhysicsInfoString()
	NULL,						// pfnPrecacheEvent()
	NULL,						// pfnPlaybackEvent()

	NULL,						// pfnSetFatPVS()
	NULL,						// pfnSetFatPAS()

	NULL,						// pfnCheckVisibility()

	NULL,						// pfnDeltaSetField()
	NULL,						// pfnDeltaUnsetField()
	NULL,						// pfnDeltaAddEncoder()
	NULL,						// pfnGetCurrentPlayer()
	NULL,						// pfnCanSkipPlayer()
	NULL,						// pfnDeltaFindField()
	NULL,						// pfnDeltaSetFieldByIndex()
	NULL,						// pfnDeltaUnsetFieldByIndex()

	NULL,						// pfnSetGroupMask()

	NULL,						// pfnCreateInstancedBaseline()
	NULL,						// pfnCvar_DirectSet()

	NULL,						// pfnForceUnmodified()

	NULL,						// pfnGetPlayerStats()

	NULL,						// pfnAddServerCommand()

	// Added in SDK 2.2:
	NULL,						// pfnVoice_GetClientListening()
	NULL,						// pfnVoice_SetClientListening()

	// Added for HL 1109 (no SDK update):
	NULL,						// pfnGetPlayerAuthId()
};
// Global vars from metamod:

gamedll_funcs_t *gpGamedllFuncs;    // gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs;     // metamod utility functions

META_FUNCTIONS gMetaFunctionTable = 
{ 
        NULL, // pfnGetEntityAPI() 
		NULL, // pfnGetEntityAPI_Post() 
		GetEntityAPI2, // pfnGetEntityAPI2() 
		NULL, // pfnGetEntityAPI2_Post() 
		NULL, // pfnGetNewDLLFunctions() 
		NULL, // pfnGetNewDLLFunctions_Post() 
		GetEngineFunctions, // pfnGetEngineFunctions() 
		NULL // pfnGetEngineFunctions_Post() 
}; 

plugin_info_t Plugin_info = { 
    META_INTERFACE_VERSION, // interface version 
		BOT_NAME, // plugin name 
		BOT_VER, // plugin version 
		__DATE__, // date of creation 
		BOT_AUTHOR, // plugin author 
		BOT_WEBSITE, // plugin URL 
		BOT_DBG_MSG_TAG, // plugin logtag 
		PT_STARTUP, // when loadable 
		PT_NEVER, // when unloadable (used to be PT_ANYTIME)
}; 

extern CBotGlobals gBotGlobals;

#if defined(_WIN32) && !defined(__CYGWIN__)
// Required DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{   
   if (fdwReason == DLL_PROCESS_DETACH)
   {
	  gBotGlobals.FreeGlobalMemory();
   }

   return TRUE;
}
#else
void _fini (void)
{
	gBotGlobals.FreeGlobalMemory();
}
#endif

C_DLLEXPORT int Meta_Query(char *interfaceVersion, plugin_info_t **plinfo, mutil_funcs_t *pMetaUtilFuncs)
{ 
	// this function is the first function ever called by metamod in the plugin DLL. Its purpose 
	// is for metamod to retrieve basic information about the plugin, such as its meta-interface 
	// version, for ensuring compatibility with the current version of the running metamod. 
	
	// keep track of the pointers to metamod function tables metamod gives us 
	gpMetaUtilFuncs = pMetaUtilFuncs; 
	*plinfo = &Plugin_info; 
	
	// check for interface version compatibility 
	if (strcmp (interfaceVersion, Plugin_info.ifvers) != 0) 
	{ 
		int mmajor = 0, mminor = 0, pmajor = 0, pminor = 0; 
		
		LOG_CONSOLE (PLID, "%s: meta-interface version mismatch (metamod: %s, %s: %s)", Plugin_info.name, interfaceVersion, Plugin_info.name, Plugin_info.ifvers); 
		LOG_MESSAGE (PLID, "%s: meta-interface version mismatch (metamod: %s, %s: %s)", Plugin_info.name, interfaceVersion, Plugin_info.name, Plugin_info.ifvers); 
		
		// if plugin has later interface version, it's incompatible (update metamod) 
		sscanf (interfaceVersion, "%d:%d", &mmajor, &mminor); 
		sscanf (META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor); 
		
		if ((pmajor > mmajor) || ((pmajor == mmajor) && (pminor > mminor))) 
		{ 
			LOG_CONSOLE (PLID, "metamod version is too old for this plugin; update metamod"); 
			LOG_ERROR (PLID, "metamod version is too old for this plugin; update metamod"); 
			return (FALSE); 
		} 
		
		// if plugin has older major interface version, it's incompatible (update plugin) 
		else if (pmajor < mmajor) 
		{ 
			LOG_CONSOLE (PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin"); 
			LOG_ERROR (PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin"); 
			return (FALSE); 
		} 
	} 
	
	return (TRUE); // tell metamod this plugin looks safe 
} 


C_DLLEXPORT int Meta_Attach (PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) 
{ 
	// this function is called when metamod attempts to load the plugin. Since it's the place 
	// where we can tell if the plugin will be allowed to run or not, we wait until here to make 
	// our initialization stuff, like registering CVARs and dedicated server commands. 
	
	// are we allowed to load this plugin now ? 
	if (now > Plugin_info.loadable) 
	{ 
		LOG_CONSOLE (PLID, "%s: plugin NOT attaching (can't load plugin right now)", Plugin_info.name); 
		LOG_ERROR (PLID, "%s: plugin NOT attaching (can't load plugin right now)", Plugin_info.name); 
		return (FALSE); // returning FALSE prevents metamod from attaching this plugin 
	} 

	if ( pFunctionTable == NULL )
	{
		BotMessage(NULL,2,"Err: Received NULL pFunctionTable from Metamod... not my fault... check running plugins!!!");
	}
	
	// keep track of the pointers to engine function tables metamod gives us 
	gpMetaGlobals = pMGlobals; 
	memcpy (pFunctionTable, &gMetaFunctionTable, sizeof (META_FUNCTIONS)); 
	gpGamedllFuncs = pGamedllFuncs; 
	
	// print a message to notify about plugin attaching 
	LOG_CONSOLE (PLID, "%s: plugin attaching", Plugin_info.name); 
	LOG_MESSAGE (PLID, "%s: plugin attaching", Plugin_info.name); 
	
	CVAR_REGISTER(&bot_ver_cvar);
	// ask the engine to register the CVARs this plugin uses 
	/*CVAR_REGISTER (&cheesys_cvar_1); 
	CVAR_REGISTER (&cheesys_cvar_2); 
	CVAR_REGISTER (&and_so_on); 
	
	// ask the engine to register the server commands this plugin uses 
	REG_SVR_COMMAND ("cheesys_command_1", TheFunctionToHandleCheesysCommand1); 
	REG_SVR_COMMAND ("cheesys_command_2", TheFunctionToHandleCheesysCommand2); */

	REG_SVR_COMMAND (BOT_COMMAND_ACCESS, RCBot_ServerCommand);
	
	return (TRUE); // returning TRUE enables metamod to attach this plugin 
} 


C_DLLEXPORT int Meta_Detach (PLUG_LOADTIME now, PL_UNLOAD_REASON reason) 
{ 
	// this function is called when metamod unloads the plugin. A basic check is made in order 
	// to prevent unloading the plugin if its processing should not be interrupted. 
	
	// is metamod allowed to unload the plugin ? 
	if ((now > Plugin_info.unloadable) && (reason != PNL_CMD_FORCED)) 
	{ 
		LOG_CONSOLE (PLID, "%s: plugin NOT detaching (can't unload plugin right now)", Plugin_info.name); 
		LOG_ERROR (PLID, "%s: plugin NOT detaching (can't unload plugin right now)", Plugin_info.name); 
		return (FALSE); // returning FALSE prevents metamod from unloading this plugin 
	} 	

	gBotGlobals.FreeGlobalMemory();
	
	return (TRUE); // returning TRUE enables metamod to unload this plugin 
}

C_DLLEXPORT int GetEngineFunctions (enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion) 
{ 
	// this is one of the initialization functions hooked by metamod in the engine API 
	
	// keep track of pointers to the functions we use in the engine API 

	meta_engfuncs.pfnCmd_Args = pfnCmd_Args;
	meta_engfuncs.pfnCmd_Argv = pfnCmd_Argv;
	meta_engfuncs.pfnCmd_Argc = pfnCmd_Argc;   
	meta_engfuncs.pfnPrecacheModel = pfnPrecacheModel;
	meta_engfuncs.pfnPrecacheSound = pfnPrecacheSound;
	meta_engfuncs.pfnSetModel = pfnSetModel;
	meta_engfuncs.pfnModelIndex = pfnModelIndex;
	meta_engfuncs.pfnModelFrames = pfnModelFrames;
	meta_engfuncs.pfnSetSize = pfnSetSize;
	meta_engfuncs.pfnChangeLevel = pfnChangeLevel;
	meta_engfuncs.pfnGetSpawnParms = pfnGetSpawnParms;
	meta_engfuncs.pfnSaveSpawnParms = pfnSaveSpawnParms;
	meta_engfuncs.pfnVecToYaw = pfnVecToYaw;
	meta_engfuncs.pfnVecToAngles = pfnVecToAngles;
	meta_engfuncs.pfnMoveToOrigin = pfnMoveToOrigin;
	meta_engfuncs.pfnChangeYaw = pfnChangeYaw;
	meta_engfuncs.pfnChangePitch = pfnChangePitch;
	meta_engfuncs.pfnFindEntityByString = pfnFindEntityByString;
	meta_engfuncs.pfnGetEntityIllum = pfnGetEntityIllum;
	meta_engfuncs.pfnFindEntityInSphere = pfnFindEntityInSphere;
	meta_engfuncs.pfnFindClientInPVS = pfnFindClientInPVS;
	meta_engfuncs.pfnEntitiesInPVS = pfnEntitiesInPVS;
	meta_engfuncs.pfnMakeVectors = pfnMakeVectors;
	meta_engfuncs.pfnAngleVectors = pfnAngleVectors;
	meta_engfuncs.pfnCreateEntity = pfnCreateEntity;
	meta_engfuncs.pfnRemoveEntity = pfnRemoveEntity;
	meta_engfuncs.pfnCreateNamedEntity = pfnCreateNamedEntity;
	meta_engfuncs.pfnMakeStatic = pfnMakeStatic;
	meta_engfuncs.pfnEntIsOnFloor = pfnEntIsOnFloor;
	meta_engfuncs.pfnDropToFloor = pfnDropToFloor;
	meta_engfuncs.pfnWalkMove = pfnWalkMove;
	meta_engfuncs.pfnSetOrigin = pfnSetOrigin;
	meta_engfuncs.pfnEmitSound = pfnEmitSound;
	meta_engfuncs.pfnEmitAmbientSound = pfnEmitAmbientSound;
	meta_engfuncs.pfnTraceLine = pfnTraceLine;
	meta_engfuncs.pfnTraceToss = pfnTraceToss;
	meta_engfuncs.pfnTraceMonsterHull = pfnTraceMonsterHull;
	meta_engfuncs.pfnTraceHull = pfnTraceHull;
	meta_engfuncs.pfnTraceModel = pfnTraceModel;
	meta_engfuncs.pfnTraceTexture = pfnTraceTexture;
	meta_engfuncs.pfnTraceSphere = pfnTraceSphere;
	meta_engfuncs.pfnGetAimVector = pfnGetAimVector;
	meta_engfuncs.pfnServerCommand = pfnServerCommand;
	meta_engfuncs.pfnServerExecute = pfnServerExecute;
	
	meta_engfuncs.pfnClientCommand = pfnClientCommand;
	
	meta_engfuncs.pfnParticleEffect = pfnParticleEffect;
	meta_engfuncs.pfnLightStyle = pfnLightStyle;
	meta_engfuncs.pfnDecalIndex = pfnDecalIndex;
	meta_engfuncs.pfnPointContents = pfnPointContents;
	meta_engfuncs.pfnMessageBegin = pfnMessageBegin;
	meta_engfuncs.pfnMessageEnd = pfnMessageEnd;
	meta_engfuncs.pfnWriteByte = pfnWriteByte;
	meta_engfuncs.pfnWriteChar = pfnWriteChar;
	meta_engfuncs.pfnWriteShort = pfnWriteShort;
	meta_engfuncs.pfnWriteLong = pfnWriteLong;
	meta_engfuncs.pfnWriteAngle = pfnWriteAngle;
	meta_engfuncs.pfnWriteCoord = pfnWriteCoord;
	meta_engfuncs.pfnWriteString = pfnWriteString;
	meta_engfuncs.pfnWriteEntity = pfnWriteEntity;
	meta_engfuncs.pfnCVarRegister = pfnCVarRegister;
	meta_engfuncs.pfnCVarGetFloat = pfnCVarGetFloat;
	meta_engfuncs.pfnCVarGetString = pfnCVarGetString;
	meta_engfuncs.pfnCVarSetFloat = pfnCVarSetFloat;
	meta_engfuncs.pfnCVarSetString = pfnCVarSetString;
	meta_engfuncs.pfnPvAllocEntPrivateData = pfnPvAllocEntPrivateData;
	meta_engfuncs.pfnPvEntPrivateData = pfnPvEntPrivateData;
	meta_engfuncs.pfnFreeEntPrivateData = pfnFreeEntPrivateData;
	meta_engfuncs.pfnSzFromIndex = pfnSzFromIndex;
	meta_engfuncs.pfnAllocString = pfnAllocString;
	meta_engfuncs.pfnGetVarsOfEnt = pfnGetVarsOfEnt;
	meta_engfuncs.pfnPEntityOfEntOffset = pfnPEntityOfEntOffset;
	meta_engfuncs.pfnEntOffsetOfPEntity = pfnEntOffsetOfPEntity;
	meta_engfuncs.pfnIndexOfEdict = pfnIndexOfEdict;
	meta_engfuncs.pfnPEntityOfEntIndex = pfnPEntityOfEntIndex;
	meta_engfuncs.pfnFindEntityByVars = pfnFindEntityByVars;
	meta_engfuncs.pfnGetModelPtr = pfnGetModelPtr;
	meta_engfuncs.pfnRegUserMsg = pfnRegUserMsg;
	meta_engfuncs.pfnAnimationAutomove = pfnAnimationAutomove;
	meta_engfuncs.pfnGetBonePosition = pfnGetBonePosition;
	meta_engfuncs.pfnFunctionFromName = pfnFunctionFromName;
	meta_engfuncs.pfnNameForFunction = pfnNameForFunction;
	meta_engfuncs.pfnClientPrintf = pfnClientPrintf;
	meta_engfuncs.pfnServerPrint = pfnServerPrint;
	meta_engfuncs.pfnGetAttachment = pfnGetAttachment;
	meta_engfuncs.pfnCRC32_Init = pfnCRC32_Init;
	meta_engfuncs.pfnCRC32_ProcessBuffer = pfnCRC32_ProcessBuffer;
	meta_engfuncs.pfnCRC32_ProcessByte = pfnCRC32_ProcessByte;
	meta_engfuncs.pfnCRC32_Final = pfnCRC32_Final;
	meta_engfuncs.pfnRandomLong = pfnRandomLong;
	meta_engfuncs.pfnRandomFloat = pfnRandomFloat;
	meta_engfuncs.pfnSetView = pfnSetView;
	meta_engfuncs.pfnTime = pfnTime;
	meta_engfuncs.pfnCrosshairAngle = pfnCrosshairAngle;
	meta_engfuncs.pfnLoadFileForMe = pfnLoadFileForMe;
	meta_engfuncs.pfnFreeFile = pfnFreeFile;
	meta_engfuncs.pfnEndSection = pfnEndSection;
	meta_engfuncs.pfnCompareFileTime = pfnCompareFileTime;
	meta_engfuncs.pfnGetGameDir = pfnGetGameDir;
	meta_engfuncs.pfnCvar_RegisterVariable = pfnCvar_RegisterVariable;
	meta_engfuncs.pfnFadeClientVolume = pfnFadeClientVolume;
	meta_engfuncs.pfnSetClientMaxspeed = pfnSetClientMaxspeed;
	meta_engfuncs.pfnCreateFakeClient = pfnCreateFakeClient;
	meta_engfuncs.pfnRunPlayerMove = pfnRunPlayerMove;
	meta_engfuncs.pfnNumberOfEntities = pfnNumberOfEntities;
	meta_engfuncs.pfnGetInfoKeyBuffer = pfnGetInfoKeyBuffer;
	meta_engfuncs.pfnInfoKeyValue = pfnInfoKeyValue;
	meta_engfuncs.pfnSetKeyValue = pfnSetKeyValue;
	meta_engfuncs.pfnSetClientKeyValue = pfnSetClientKeyValue;
	meta_engfuncs.pfnIsMapValid = pfnIsMapValid;
	meta_engfuncs.pfnStaticDecal = pfnStaticDecal;
	meta_engfuncs.pfnPrecacheGeneric = pfnPrecacheGeneric;
	meta_engfuncs.pfnGetPlayerUserId = pfnGetPlayerUserId;
	meta_engfuncs.pfnBuildSoundMsg = pfnBuildSoundMsg;
	meta_engfuncs.pfnIsDedicatedServer = pfnIsDedicatedServer;
	meta_engfuncs.pfnCVarGetPointer = pfnCVarGetPointer;
	meta_engfuncs.pfnGetPlayerWONId = pfnGetPlayerWONId;
	
	// Do these at start
	gBotGlobals.Init();	
	gBotGlobals.GetModInfo();
	
	SetupMenus();

	//gBotGlobals.GameInit();

	// copy the whole table for metamod to know which functions we are using here 
	memcpy (pengfuncsFromEngine,&meta_engfuncs, sizeof (enginefuncs_t)); 	
	
	return (TRUE); // alright 
}

//#if defined(_WIN32) && !defined(__CYGWIN__)
void WINAPI GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
//#else
//extern "C" DLLEXPORT void GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
//#endif
{	
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

	g_argv = (char *) malloc (1024);
}


#endif