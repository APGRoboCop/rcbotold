// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// h_export.h - prototypes for h_export.cpp

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of RCBot and Metamod.
 *
 *    RCBot and Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
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

#ifndef H_EXPORT_H
#define H_EXPORT_H

#include "osdep.h"			// DLLEXPORT, WINAPI, etc

 // Our GiveFnptrsToDll, called by engine.

 //#if defined(_WIN32) && !defined(__CYGWIN__)
typedef void (WINAPI* GIVE_ENGINE_FUNCTIONS_FN) (enginefuncs_t* pengfuncsFromEngine, globalvars_t*);
C_DLLEXPORT void WINAPI GiveFnptrsToDll(const enginefuncs_t* pengfuncsFromEngine, globalvars_t* pGlobals);
//#else
//	extern "C" DLLEXPORT void GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals );
//#endif
//#ifdef __linux__
//extern "C" DLLEXPORT void GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
//#else
//C_DLLEXPORT void WINAPI GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals );
//#endif

#endif /* H_EXPORT_H */