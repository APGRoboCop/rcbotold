// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// h_export.cpp - main exported DLL functionality

// From SDK dlls/h_export.cpp:

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

===== h_export.cpp ========================================================

  Entity classes exported by Halflife.

*/


#include <extdll.h>			// always

#include "h_export.h"		// me
#include "metamod.h"		// engine_t, etc
#include "log_meta.h"		// META_DEV, etc

// From SDK dlls/h_export.cpp:

#ifdef _WIN32
//! Required DLL entry point
// The above SDK comment indicates this routine is required, but the MSDN
// documentation indicates it's actually optional.  We keep it, though, for
// completeness.
BOOL WINAPI DllMain(HINSTANCE /* hinstDLL */, DWORD fdwReason, 
		LPVOID /* lpvReserved */)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		/* nothing */
    }
	else if (fdwReason == DLL_PROCESS_DETACH) {
		/* nothing */
    }
	return(TRUE);
}
#elif defined(linux)
// Linux routines to correspond to ATTACH and DETACH cases above.  These
// aren't required by linux, but are included here for completeness, and
// just in case we come across a need to do something at dll load or
// unload.
void _init(void) {
	// called before dlopen() returns
}
void _fini(void) {
	// called before dlclose() returns
}
#endif

//! Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
engine_t Engine;

// Receive engine function table from engine.
//
// This appears to be the _first_ DLL routine called by the engine, so this
// is where we hook to load all the other DLLs (game, plugins, etc), which
// is actually all done in meta_startup().
void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, 
		globalvars_t *pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(g_engfuncs));
	gpGlobals = pGlobals;
	Engine.funcs = &g_engfuncs;
	Engine.globals = pGlobals;
	// NOTE!  Have to call logging function _after_ copying into g_engfuncs, so
	// that g_engfuncs.pfnAlertMessage() can be resolved properly, heh. :)
	META_DEV("called: GiveFnptrsToDll");

	// Load plugins, load game dll.
	metamod_startup();

	return;
}
