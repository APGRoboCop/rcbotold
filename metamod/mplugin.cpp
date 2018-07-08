// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// mplugin.cpp - functions for individual plugin (class MPlugin)

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
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

#include <errno.h>				// errno, etc
#include <malloc.h>				// malloc, etc
#include <sys/types.h>			// stat
#include <sys/stat.h>			// stat

#include <extdll.h>				// always

#include "mplugin.h"			// me
#include "metamod.h"			// GameDLL, etc
#include "mreg.h"				// MRegCmdList::show(int), etc
#include "mhook.h"				// class MHook
#include "h_export.h"			// GIVE_ENGINE_FUNCTIONS_FN, etc
#include "dllapi.h"				// FN_GAMEINIT, etc
#include "support_meta.h"		// full_gamedir_path,
#include "types_meta.h"			// mBOOL
#include "log_meta.h"			// logging functions, etc
#include "osdep.h"				// win32 snprintf, is_absolute_path,


// Parse a line from plugins.ini into a plugin.
// meta_errno values:
//  - ME_COMMENT	ignored commented line
//  - ME_FORMAT		invalid line format
//  - ME_OSNOTSUP	plugin is not for this OS
mBOOL MPlugin::ini_parseline(char *line) {
	char *token;
	char *ptr_token;
	char *cp;

	if(line[0]=='#' || line[0]==';' || strstr(line, "//")==line) {
		META_DEBUG(7, ("ini: Ignoring commented line: %s", line));
		RETURN_ERRNO(mFALSE, ME_COMMENT);
	}

	// grab platform ("win32" or "linux")
	token=strtok_r(line, " \t", &ptr_token);
	if(!token)
		RETURN_ERRNO(mFALSE, ME_FORMAT);
	if(strcasecmp(token, PLATFORM)) {
		// plugin is not for this OS
		META_DEBUG(7, ("ini: Ignoring entry for %s", token));
		RETURN_ERRNO(mFALSE, ME_OSNOTSUP);
	}

	// grab filename
	token=strtok_r(NULL, " \t\r\n", &ptr_token);
	if(!token)
		RETURN_ERRNO(mFALSE, ME_FORMAT);
	STRNCPY(filename, token, sizeof(filename));
	normalize_pathname(filename);
	// Store name of just the actual _file_, without dir components.
	cp=strrchr(filename, '/');
	if(cp)
		file=cp+1;
	else
		file=filename;

	// Grab description.
	// Just get the the rest of the line, minus line-termination.
	token=strtok_r(NULL, "\n\r", &ptr_token);
	if(token) {
		token=token+strspn(token, " \t");			// skip whitespace
		STRNCPY(desc, token, sizeof(desc));
	}
	else {
		// If no description is specified, temporarily use plugin file,
		// until plugin can be queried, and desc replaced with info->name.
		snprintf(desc, sizeof(desc), "<%s>", file);
	}

	// Make full pathname (from gamedir if relative, remove "..",
	// backslashes, etc).
	full_gamedir_path(filename, pathname);

	source=PS_INI;
	status=PL_VALID;
	return(mTRUE);
}

// Parse a line from console "load" command into a plugin.
// meta_errno values:
//  - ME_FORMAT		invalid line format
mBOOL MPlugin::cmd_parseline(const char *line) {
	char buf[NAME_MAX + PATH_MAX + MAX_DESC_LEN];
	char *token;
	char *ptr_token;
	char *cp;

	STRNCPY(buf, line, sizeof(buf));

	// remove "load"
	token=strtok_r(buf, " \t", &ptr_token);
	if(!token)
		RETURN_ERRNO(mFALSE, ME_FORMAT);

	// grab filename
	token=strtok_r(NULL, " \t", &ptr_token);
	if(!token)
		RETURN_ERRNO(mFALSE, ME_FORMAT);
	STRNCPY(filename, token, sizeof(filename));
	normalize_pathname(filename);
	// store name of just the actual _file_, without dir components
	cp=strrchr(filename, '/');
	if(cp)
		file=cp+1;
	else
		file=filename;

	// Grab description.
	// Specify no delimiter chars, as we just want the rest of the line.
	token=strtok_r(NULL, "", &ptr_token);
	if(token) {
		token=token+strspn(token, " \t");			// skip whitespace
		STRNCPY(desc, token, sizeof(desc));
	}
	else {
		// if no description is specified, temporarily use plugin file,
		// until plugin can be queried, and desc replaced with info->name.
		snprintf(desc, sizeof(desc), "<%s>", file);
	}

	// Make full pathname (from gamedir if relative, remove "..",
	// backslashes, etc).
	full_gamedir_path(filename, pathname);

	source=PS_CMD;
	status=PL_VALID;
	return(mTRUE);
}

// Make sure this plugin has the necessary minimal information.
// meta_errno values:
//  - ME_ARGUMENT	missing necessary fields in plugin
mBOOL MPlugin::check_input(void) {
	// doublecheck our input/state
	if(status < PL_VALID) {
		META_ERROR("dll: Tried to operate on plugin[%d] with a non-valid status (%d)", index, str_status());
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(!file || !file[0]) {
		META_ERROR("dll: Tried to operate on plugin[%d] with an empty file", 
				index);
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(!filename[0]) {
		META_ERROR("dll: Tried to operate on plugin[%d] with an empty filename",
				index);
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(!pathname[0]) {
		META_ERROR("dll: Tried to operate on plugin[%d] with an empty pathname",
				index);
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(!desc[0]) {
		// if no description is specified, temporarily use plugin file,
		// until plugin can be queried, and desc replaced with info->name.
		snprintf(desc, sizeof(desc), "<%s>", file);
	}
	return(mTRUE);
}

// Try to resolve a plugin's filename as a (possibly partial) path to an
// actual filename on disk, to facilitate easier console load-command
// arguments.  Uses resolve_dirs, resolve_prefix, and resolve_suffix below.  
// Example paths that it tries:
//    filename
//    Gamedir/filename.dll, Gamedir/filename.so
//    Gamedir/filename_i386.so
//    Gamedir/dlls/mm_filename_i386.so
//    Gamedir/dlls/filename_mm_i386.so
//    Gamedir/dlls/filename_MM_i386.so
// meta_errno values:
//  - ME_NOTFOUND	couldn't find a matching file for the partial name
//  - errno's from check_input()
mBOOL MPlugin::resolve(void) {
	char *found;
	char *cp;
	int len;
	if(!check_input()) {
		// details logged, meta_errno set in check_input()
		return(mFALSE);
	}
	if(is_absolute_path(filename))
		found=resolve_prefix(filename);
	else
		found=resolve_dirs(filename);

	if(!found) {
		META_DEBUG(2, ("Couldn't resolve '%s' to file", filename));
		RETURN_ERRNO(mFALSE, ME_NOTFOUND);
	}
	META_DEBUG(2, ("Resolved '%s' to file '%s'", filename, found));
	// store pathname: the resolved path (should be absolute)
	STRNCPY(pathname, found, sizeof(pathname));
	// store file: the name of the file (without dir)
	cp=strrchr(pathname, '/');
	if(cp)
		file=cp+1;
	else
		file=pathname;
	// store pathname: the gamedir relative path, or an absolute path
	len=strlen(GameDLL.gamedir);
	if(strncasecmp(pathname, GameDLL.gamedir, len) == 0)
		STRNCPY(filename, pathname+len+1, sizeof(filename));
	else
		STRNCPY(filename, pathname, sizeof(filename));

	return(mTRUE);
}

// For the given path, tries to find file in several possible 
// directories.
// Try:
//     GAMEDIR/filename
//     GAMEDIR/dlls/filename
// meta_errno values:
//  - none
char *MPlugin::resolve_dirs(char *path) {
	struct stat st;
	static char buf[PATH_MAX];
	char *found;

	snprintf(buf, sizeof(buf), "%s/%s", GameDLL.gamedir, path);
	// try this path
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	// try other file prefixes in this path
	if((found=resolve_prefix(buf)))
		return(found);

	snprintf(buf, sizeof(buf), "%s/dlls/%s", GameDLL.gamedir, path);
	// try this path
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	// try other file prefixes for this path
	if((found=resolve_prefix(buf)))
		return(found);

	return(NULL);
}

// For the given path, tries several possible filename prefixes.
// Try:
//     dir/mm_file
//     dir/file
// meta_errno values:
//  - none
char *MPlugin::resolve_prefix(char *path) {
	struct stat st;
	char *cp, *fname;
	char dname[PATH_MAX];
	static char buf[PATH_MAX];
	char *found;

	// try "mm_" prefix FIRST.
	// split into dirname and filename
	STRNCPY(dname, path, sizeof(dname));
	cp=strrchr(dname, '/');
	if(cp) {
		*cp='\0';
		fname=cp+1;
		snprintf(buf, sizeof(buf), "%s/mm_%s", dname, fname);
	}
	else {
		// no directory in given path
		snprintf(buf, sizeof(buf), "mm_%s", path);
	}
	// try this path
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	// try other suffixes for this path
	if((found=resolve_suffix(buf)))
		return(found);

	// try other suffixes for the original path
	if((found=resolve_suffix(path)))
		return(found);

	return(NULL);
}

// For the given path, tries several different filename suffixes.
// Try:
//     path
//     path_mm
//     path_MM
//     path.so (linux), path.dll (win32)
//     path_i386.so, path_i486.so, etc (if linux)
// meta_errno values:
//  - none
char *MPlugin::resolve_suffix(char *path) {
	struct stat st;
	static char buf[PATH_MAX];
	char *found;

	// Hmm, recursion.
	if(!strstr(path, "_mm")) {
		char *tmpbuf;
		snprintf(buf, sizeof(buf), "%s_mm", path);
		tmpbuf=strdup(buf);
		found=resolve_suffix(tmpbuf);
		free(tmpbuf);
		if(found) return(found);
	}
	if(!strstr(path, "_MM")) {
		char *tmpbuf;
		snprintf(buf, sizeof(buf), "%s_MM", path);
		tmpbuf=strdup(buf);
		found=resolve_suffix(tmpbuf);
		free(tmpbuf);
		if(found) return(found);
	}

#ifdef _WIN32
	snprintf(buf, sizeof(buf), "%s.dll", path);
#elif defined(linux)
	snprintf(buf, sizeof(buf), "%s.so", path);
#else
#error "OS unrecognized"
#endif /* _WIN32 */
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);

#ifdef linux
	snprintf(buf, sizeof(buf), "%s_i386.so", path);
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	snprintf(buf, sizeof(buf), "%s_i486.so", path);
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	snprintf(buf, sizeof(buf), "%s_i586.so", path);
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
	snprintf(buf, sizeof(buf), "%s_i686.so", path);
	if(stat(buf, &st) == 0 && S_ISREG(st.st_mode))
		return(buf);
#endif /* linux */

	return(NULL);
}

// Load a plugin; query, check allowed time, attach.
// meta_errno values:
//  - ME_ARGUMENT	missing necessary fields in plugin
//  - ME_ALREADY	this plugin already loaded
//  - ME_BADREQ		plugin not marked for load
//  - ME_DELAYED	load request is delayed (till changelevel?)
//  - ME_NOTALLOWED	plugin not loadable after startup
//  - errno's from query()
//  - errno's from attach()
//  - errno's from check_input()
mBOOL MPlugin::load(PLUG_LOADTIME now) {
	if(!check_input()) {
		// details logged, meta_errno set in check_input()
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(status >= PL_RUNNING) {
		META_ERROR("dll: Not loading plugin '%s'; already loaded (status=%s)", 
				desc, str_status());
		RETURN_ERRNO(mFALSE, ME_ALREADY);
	}
	if(action != PA_LOAD && action != PA_ATTACH) {
		META_ERROR("dll: Not loading plugin '%s'; not marked for load (action=%s)", desc, str_action());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}

	if(status != PL_OPENED) {
		// query plugin; open file and get info about it
		if(!query()) {
			META_ERROR("dll: Skipping plugin '%s'; couldn't query", desc);
			if(meta_errno != ME_DLOPEN) {
				if(DLCLOSE(handle) != 0) {
					META_ERROR("dll: Couldn't close plugin file '%s': %s", 
							file, DLERROR());
				}
				else
					handle=NULL;
			}
			status=PL_BADFILE;
			// meta_errno should be already set in query()
			return(mFALSE);
		}
		status=PL_OPENED;
	}

	// are we allowed to attach this plugin at this time?
	if(info->loadable < now) {
		if(info->loadable > PT_STARTUP) {
			// will try to attach again at next opportunity
			META_DEBUG(2, ("dll: Delaying load plugin '%s'; can't attach now: allowed=%s; now=%s", 
						desc, str_loadable(), str_loadtime(now, SL_SIMPLE)));
			RETURN_ERRNO(mFALSE, ME_DELAYED);
		}
		else {
			META_DEBUG(2, ("dll: Failed load plugin '%s'; can't attach now: allowed=%s; now=%s", 
						desc, str_loadable(), str_loadtime(now, SL_SIMPLE)));
			// don't try to attach again later
			action=PA_NONE;
			RETURN_ERRNO(mFALSE, ME_NOTALLOWED);
		}
	}

	// attach plugin; get function tables
	if(attach(now) != mTRUE) {
		META_ERROR("dll: Failed to attach plugin '%s'", desc);
		// Note we don't dlclose() here, since we're returning PL_FAILED,
		// which implies that it's been dlopened and queried successfully.
		// Doing so causes crashes, because things like "meta list" try to
		// look at *info, which is in the DLL memory space and unaccessible
		// (segfault) after dlclosed.
		status=PL_FAILED;
		// meta_errno should be already set in attach()
		return(mFALSE);
	}
	status=PL_RUNNING;
	action=PA_NONE;

	// If not loading at server startup, then need to call plugin's
	// GameInit, since we've passed that.
	if(now != PT_STARTUP) {
		FN_GAMEINIT pfn_gameinit=NULL;
		if(dllapi_table && (pfn_gameinit=dllapi_table->pfnGameInit))
			pfn_gameinit();
	}
	// If loading during map, then I'd like to call plugin's
	// ServerActivate, since we've passed that.  However, I'm not sure what
	// arguments to give it...  So, we'll just have to say for the
	// moment that plugins that are loadable during a map can't need to
	// hook ServerActivate.

	META_LOG("dll: Loaded plugin '%s': %s v%s %s, %s", desc, info->name, 
			info->version, info->date, info->author);
	return(mTRUE);
}

// Query a plugin:
//  - dlopen() the file, store the handle
//  - dlsym() and call:
//	    Meta_Init (if present) - tell dll it'll be used as a metamod plugin
//	    GiveFnptrsToDll - give engine function ptrs 
//	    Meta_Query - say "hi" and get info about plugin
// meta_errno values:
//  - ME_DLOPEN		dlopen/loadlibrary failed; see dlerror() for details
//  - ME_DLMISSING	couldn't find a query() or giveFuncs() in plugin
//  - ME_DLERROR	plugin query() returned error
//  - ME_NULLDATA	info struct from query() was null
mBOOL MPlugin::query(void) {
	META_INIT_FN pfn_init;
	GIVE_ENGINE_FUNCTIONS_FN pfn_give_engfuncs;
	META_QUERY_FN pfn_query;

	// open the plugin DLL
	if(!(handle=DLOPEN(pathname))) {
		META_ERROR("dll: Failed query plugin '%s'; Couldn't open file '%s': %s",
				desc, pathname, DLERROR());
		RETURN_ERRNO(mFALSE, ME_DLOPEN);
	}

	// First, we check to see if they have a Meta_Query.  We would normally
	// dlsym this just prior to calling it, after having called
	// GiveFnptrsToDll, but things like standalone-DLL bots do all their
	// startup in GiveFnptrsToDll, and trying to load them as metamod
	// plugins creates all sorts of crashes.  So, we do a trivial check
	// first to see if the DLL looks like a metamod plugin before
	// proceeding with the normal order.  Note that we still have to call 
	// GiveFnptrsToDll before Meta_Query, because the latter typically uses 
	// engine functions like AlertMessage, which have to be passed along via 
	// GiveFnptrsToDll.
	pfn_query = (META_QUERY_FN) DLSYM(handle, "Meta_Query");
	if(!pfn_query) {
		META_ERROR("dll: Failed query plugin '%s'; Couldn't find Meta_Query(): %s", desc, DLERROR());
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	}

	// Call Meta_Init, if present.  This is an optional plugin routine to
	// allow plugin to do any special initializing or other processing
	// prior to the standard query/attach procedure.  
	//
	// In particular, this should allow for DLL's that can operate as both
	// a standalone dll AND a metamod plugin.  This routine has to be
	// called _before_ GiveFnptrsToDll, since the dll will usually do all
	// its startup processing in GiveFn, and has to know at that point
	// whether it needs to do just mm plugin startup, or all the extra
	// startup needed for a standalone DLL.
	//
	// This passes nothing and returns nothing, and the routine in the
	// plugin can NOT use any Engine functions, as they haven't been
	// provided yet (done next, in GiveFnptrsToDll).
	pfn_init = (META_INIT_FN) DLSYM(handle, "Meta_Init");
	if(pfn_init) {
		pfn_init();
		META_DEBUG(6, ("dll: Plugin '%s': Called Meta_Init()", desc));
	}
	else {
		META_DEBUG(5, ("dll: no Meta_Init present in plugin '%s'", desc));
		// don't return; not an error
	}
	
	// pass on engine function table and globals to plugin
	if(!(pfn_give_engfuncs = (GIVE_ENGINE_FUNCTIONS_FN) DLSYM(handle, "GiveFnptrsToDll"))) {
		// META_ERROR("dll: Couldn't find GiveFnptrsToDll() in plugin '%s': %s", desc, DLERROR());
		META_ERROR("dll: Failed query plugin '%s'; Couldn't find GiveFnptrsToDll(): %s", desc, DLERROR());
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	}
	pfn_give_engfuncs(Engine.pl_funcs, Engine.globals);
	META_DEBUG(6, ("dll: Plugin '%s': Called GiveFnptrsToDll()", desc));

	// Call plugin's Meta_Query(), to pass our meta interface version, and get
	// plugin's info structure.
	meta_errno=ME_NOERROR;
	info=NULL;
	// Make a copy of the meta_util function table for each plugin, for the
	// same reason.
	memcpy(&mutil_funcs, &MetaUtilFunctions, sizeof(mutil_funcs));

	if(pfn_query(META_INTERFACE_VERSION, &info, &mutil_funcs) != TRUE) {
		META_ERROR("dll: Failed query plugin '%s'; Meta_Query returned error", 
				desc);
		meta_errno=ME_DLERROR;
	}
	else {
		META_DEBUG(6, ("dll: Plugin '%s': Called Meta_Query() successfully", 
					desc));
	}

	// Check for interface differences...  Generally, a difference in major
	// version will be incompatible, and a plugin that expects a later
	// minor version will be incompatible (it's expecting things that this
	// Metamod won't be supplying).  Plugins that use an older minor
	// version will still work, as backward-compability within major
	// version is expected (forward-compatibility is not).
	//
	// Note, this check is done regardless of whether meta_query returns an
	// error.
	if(info && !FStrEq(info->ifvers, META_INTERFACE_VERSION)) {
		int mmajor=0, mminor=0, pmajor=0, pminor=0;
		META_DEBUG(3, ("dll: Note: Plugin '%s' interface version didn't match; expected %s, found %s", desc, META_INTERFACE_VERSION, info->ifvers));
		sscanf(META_INTERFACE_VERSION, "%d:%d", &mmajor, &mminor);
		sscanf(info->ifvers, "%d:%d", &pmajor, &pminor);
		// If plugin has later interface version, it's incompatible 
		// (update metamod).
		if(pmajor > mmajor || (pmajor==mmajor && pminor > mminor)) {
			META_ERROR("dll: Plugin '%s' requires a newer version of Metamod (Metamod needs at least interface %s not the current %s)", desc, info->ifvers, META_INTERFACE_VERSION);
			meta_errno=ME_IFVERSION;
		}
		// If plugin has older major interface version, it's incompatible
		// (update plugin).
		else if(pmajor < mmajor) {
			META_ERROR("dll: Plugin '%s' is out of date and incompatible with this version of Metamod; please find a newer version of the plugin (plugin needs at least interface %s not the current %s)", desc, META_INTERFACE_VERSION, info->ifvers);
			meta_errno=ME_IFVERSION;
		}
		// Plugin has identical major, with older minor.  This is supposed to be
		// backwards compatible, so we warn, but we still accept it.
		else if(pmajor==mmajor && pminor < mminor)
			META_LOG("dll: Note: plugin '%s' is using an older interface version (%s), not the latest interface version (%s); there might be an updated version of the plugin", desc, info->ifvers, META_INTERFACE_VERSION);
		else
			META_LOG("dll: Plugin '%s': unexpected version comparision; metavers=%s, mmajor=%d, mminor=%d; plugvers=%s, pmajor=%d, pminor=%d", 
					desc, META_INTERFACE_VERSION, mmajor, mminor, info->ifvers, pmajor, pminor);
	}

	if(meta_errno == ME_IFVERSION) {
		META_ERROR("dll: Rejected plugin '%s' due to interface version incompatibility (mm=%s, pl=%s)", desc, META_INTERFACE_VERSION, info->ifvers);
		// meta_errno is set already above
		// caller will dlclose()
		return(mFALSE);
	}
	else if(meta_errno != ME_NOERROR)
		// some other error, already logged
		return(mFALSE);

	if(!info) {
		META_ERROR("dll: Failed query plugin '%s'; Empty info structure", desc);
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_NULLRESULT);
	}
	// Replace temporary desc with plugin's internal name.
	if(desc[0] == '<')
		STRNCPY(desc, info->name, sizeof(desc));
	META_DEBUG(6, ("dll: Plugin '%s': Query successful", desc));
	return(mTRUE);
}

// Attach a plugin:
//	- dlsym() and call:
//	    Meta_Attach - get table of api tables, give meta_globals
//  - if provided by plugin, call various "Get" function pointers, 
//    and store resulting function tables:
//		GetEntityAPI			(std)
//		GetEntityAPI2			(std sdk2)
//		GetNewDLLFunctions		(std sdk2)
//
//		GetEntityAPI_Post		(meta)
//		GetEntityAPI2_Post		(meta)
//		GetNewDLLFunctions_Post	(meta)
//
//		GetEngineFunctions		(meta)
//		GetEngineFunctions_Post	(meta)
// meta_errno values:
//  - ME_DLMISSING	couldn't find meta_attach() in plugin
//  - ME_DLERROR	plugin attach() returned error
//  - ME_NOMEM		failed malloc
mBOOL MPlugin::attach(PLUG_LOADTIME now) {
	int ret;
	int iface_vers;

	META_ATTACH_FN pfn_attach;
	META_FUNCTIONS meta_table;

	// Make copy of gameDLL's function tables for each plugin, so we don't
	// risk the plugins screwing with the tables everyone uses.
	if(GameDLL.funcs.dllapi_table && !gamedll_funcs.dllapi_table) {
		gamedll_funcs.dllapi_table = (DLL_FUNCTIONS *) malloc(sizeof(DLL_FUNCTIONS));
		if(!gamedll_funcs.dllapi_table) {
			META_ERROR("dll: Failed attach plugin '%s': Failed malloc() for dllapi_table");
			RETURN_ERRNO(mFALSE, ME_NOMEM);
		}
		memcpy(gamedll_funcs.dllapi_table, GameDLL.funcs.dllapi_table, 
				sizeof(DLL_FUNCTIONS));
	}
	if(GameDLL.funcs.newapi_table && !gamedll_funcs.newapi_table) {
		gamedll_funcs.newapi_table = (NEW_DLL_FUNCTIONS *) malloc(sizeof(NEW_DLL_FUNCTIONS));
		if(!gamedll_funcs.newapi_table) {
			META_ERROR("dll: Failed attach plugin '%s': Failed malloc() for newapi_table");
			RETURN_ERRNO(mFALSE, ME_NOMEM);
		}
		memcpy(gamedll_funcs.newapi_table, GameDLL.funcs.newapi_table, 
				sizeof(NEW_DLL_FUNCTIONS));
	}
	if(!(pfn_attach = (META_ATTACH_FN) DLSYM(handle, "Meta_Attach"))) {
		META_ERROR("dll: Failed attach plugin '%s': Couldn't find Meta_Attach(): %s", desc, DLERROR());
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	}

	memset(&meta_table, 0, sizeof(meta_table));
	// get table of function tables,
	// give public meta globals
	ret=pfn_attach(now, &meta_table, &PublicMetaGlobals, &gamedll_funcs);
	if(ret != TRUE) {
		META_ERROR("dll: Failed attach plugin '%s': Error from Meta_Attach(): %d", desc, ret);
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_DLERROR);
	}
	META_DEBUG(6, ("dll: Plugin '%s': Called Meta_Attach() successfully", desc));

	// Rather than duplicate code, we use another ugly macro.  Again,
	// a function isn't an option since we have varying types.
#define GET_FUNC_TABLE_FROM_PLUGIN(pfnGetFuncs, STR_GetFuncs, struct_field, API_TYPE, TABLE_TYPE, vers_pass, vers_int, vers_want) \
	if(meta_table.pfnGetFuncs) { \
		struct_field = (TABLE_TYPE*) malloc(sizeof(TABLE_TYPE)); \
		if(meta_table.pfnGetFuncs(struct_field, vers_pass)) { \
			META_DEBUG(3, ("dll: Plugin '%s': Found %s", desc, STR_GetFuncs)); \
		} \
		else { \
			META_ERROR("dll: Failure calling %s in plugin '%s'", STR_GetFuncs, desc); \
			if(vers_int != vers_want) \
				META_ERROR("dll: Interface version didn't match; expected %d, found %d", vers_want, vers_int); \
		} \
	} \
	else { \
		META_DEBUG(5, ("dll: Plugin '%s': No %s", desc, STR_GetFuncs)); \
		struct_field=NULL; \
	}

	// Look for API-NEW interface in plugin.  We do this before API2/API, because
	// that's what the engine appears to do..
	// But we only do this if the gamedll provides these, so that we don't
	// give a plugin the idea that we'll call them, when in fact we won't
	// (since we don't export them to the engine when the gamedll doesn't
	// provide them).
	if(GameDLL.funcs.newapi_table) {
		iface_vers=NEW_DLL_FUNCTIONS_VERSION;
		GET_FUNC_TABLE_FROM_PLUGIN(pfnGetNewDLLFunctions, 
				"GetNewDLLFunctions", newapi_table, 
				NEW_DLL_FUNCTIONS_FN, NEW_DLL_FUNCTIONS,
				&iface_vers, iface_vers, NEW_DLL_FUNCTIONS_VERSION);
		iface_vers=NEW_DLL_FUNCTIONS_VERSION;
		GET_FUNC_TABLE_FROM_PLUGIN(pfnGetNewDLLFunctions_Post, 
				"GetNewDLLFunctions_Post", newapi_post_table, 
				NEW_DLL_FUNCTIONS_FN, NEW_DLL_FUNCTIONS,
				&iface_vers, iface_vers, NEW_DLL_FUNCTIONS_VERSION);
	}

	// Look for API2 interface in plugin; preferred over API-1.
	iface_vers=INTERFACE_VERSION;
	GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEntityAPI2, 
			"GetEntityAPI2", dllapi_table, 
			APIFUNCTION2, DLL_FUNCTIONS,
			&iface_vers, iface_vers, INTERFACE_VERSION);
	iface_vers=INTERFACE_VERSION;
	GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEntityAPI2_Post, 
			"GetEntityAPI2_Post", dllapi_post_table, 
			APIFUNCTION2, DLL_FUNCTIONS,
			&iface_vers, iface_vers, INTERFACE_VERSION);

	// Look for old-style API in plugin, if API2 interface wasn't found.
	if(!dllapi_table && !dllapi_post_table) {
		GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEntityAPI, 
				"GetEntityAPI", dllapi_table, 
				APIFUNCTION, DLL_FUNCTIONS,
				INTERFACE_VERSION, INTERFACE_VERSION, INTERFACE_VERSION);
		GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEntityAPI_Post, 
				"GetEntityAPI_Post", dllapi_post_table, 
				APIFUNCTION, DLL_FUNCTIONS,
				INTERFACE_VERSION, INTERFACE_VERSION, INTERFACE_VERSION);
	}

	// Look for Engine interface.
	iface_vers=ENGINE_INTERFACE_VERSION;
	GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEngineFunctions, 
			"GetEngineFunctions", engine_table, 
			GET_ENGINE_FUNCTIONS_FN, enginefuncs_t,
			&iface_vers, iface_vers, ENGINE_INTERFACE_VERSION);
	iface_vers=ENGINE_INTERFACE_VERSION;
	GET_FUNC_TABLE_FROM_PLUGIN(pfnGetEngineFunctions_Post, 
			"GetEngineFunctions_Post", engine_post_table, 
			GET_ENGINE_FUNCTIONS_FN, enginefuncs_t,
			&iface_vers, iface_vers, ENGINE_INTERFACE_VERSION);

	if(!dllapi_table && !dllapi_post_table 
		&& !newapi_table && !newapi_post_table
		&& !engine_table && !engine_post_table)
	{
		META_LOG("dll: Plugin '%s' isn't catching _any_ functions ??", desc);
	}

	time_loaded=time(NULL);
	return(mTRUE);
}

// Unload a plugin.  Check time, detach.
// meta_errno values:
//  - ME_ARGUMENT	missing necessary fields in plugin
//  - ME_ALREADY	this plugin already unloaded
//  - ME_BADREQ		plugin not marked for unload
//  - ME_DELAYED	unload request is delayed (till changelevel?)
//  - ME_NOTALLOWED	plugin not unloadable after startup
//  - errno's from check_input()
mBOOL MPlugin::unload(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	if(!check_input()) {
		// details logged, meta_errno set in check_input()
		RETURN_ERRNO(mFALSE, ME_ARGUMENT);
	}
	if(status < PL_RUNNING) {
		META_ERROR("dll: Not unloading plugin '%s'; already unloaded (status=%s)", desc, str_status());
		RETURN_ERRNO(mFALSE, ME_ALREADY);
	}
	if(action != PA_UNLOAD && action != PA_RELOAD) {
		META_ERROR("dll: Not unloading plugin '%s'; not marked for unload (action=%s)", desc, str_action());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}

	// Are we allowed to detach this plugin at this time?
	// If forcing unload, we disregard when plugin wants to be unloaded.
	if(info->unloadable < now) {
		if(reason == PNL_CMD_FORCED) {
			META_DEBUG(2, ("dll: Forced unload plugin '%s' overriding allowed times: allowed=%s; now=%s", desc, str_unloadable(), str_loadtime(now, SL_SIMPLE)));
		}
		else {
			if(info->unloadable > PT_STARTUP) {
				META_DEBUG(2, ("dll: Delaying unload plugin '%s'; can't detach now: allowed=%s; now=%s", desc, str_unloadable(), str_loadtime(now, SL_SIMPLE)));
				// caller should give message to user
				// try to unload again at next opportunity
				RETURN_ERRNO(mFALSE, ME_DELAYED);
			}
			else {
				META_DEBUG(2, ("dll: Failed unload plugin '%s'; can't detach now: allowed=%s; now=%s", desc, str_unloadable(), str_loadtime(now, SL_SIMPLE)));
				// don't try to unload again later
				action=PA_NONE;
				RETURN_ERRNO(mFALSE, ME_NOTALLOWED);
			}
		}
	}

	// If unloading during map, then I'd like to call plugin's
	// ServerDeactivate.  However, I don't want to do this until I start
	// calling ServerActivate when loading during map, since the SDK
	// indicates these two routines should match call for call.

	// detach plugin
	if(!detach(now, reason)) {
		if(reason == PNL_CMD_FORCED) {
			META_DEBUG(2, ("dll: Forced unload plugin '%s' overriding failed detach"));
		}
		else {
			META_ERROR("dll: Failed to detach plugin '%s'; ", desc);
			// meta_errno should be already set in detach()
			return(mFALSE);
		}
	}

	// successful detach, or forced unload

	// Unmark registered commands for this plugin (by index number).
	RegCmds->disable(index);
	// Unmark registered cvars for this plugin (by index number).
	RegCvars->disable(index);

#ifdef UNFINISHED
	// Remove all requested hooks from this plugin (by index number).
	Hooks->remove_all(info);
#endif /* UNFINISHED */

	// Close the file.  Note: after this, attempts to reference any memory
	// locations in the file will produce a segfault.
	if(DLCLOSE(handle) != 0) {
		META_ERROR("dll: Couldn't close plugin file '%s': %s", file, DLERROR());
		status=PL_FAILED;
		RETURN_ERRNO(mFALSE, ME_DLERROR);
	}
	handle=NULL;

	if(action==PA_UNLOAD) {
		status=PL_EMPTY;
		clear();
	}
	else if(action==PA_RELOAD) {
		status=PL_VALID;
		action=PA_LOAD;
	}
	META_LOG("dll: Unloaded plugin '%s' for reason '%s'", desc, str_reason(reason));
	return(mTRUE);
}

// Inform plugin we're going to unload it.
// meta_errno values:
//  - ME_DLMISSING	couldn't find meta_detach() in plugin
//  - ME_DLERROR	plugin detach() returned error
mBOOL MPlugin::detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	int ret;
	META_DETACH_FN pfn_detach;

	if(!(pfn_detach = (META_DETACH_FN) DLSYM(handle, "Meta_Detach"))) {
		META_ERROR("dll: Error detach plugin '%s': Couldn't find Meta_detach(): %s", desc, DLERROR());
		// caller will dlclose()
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	}

	ret=pfn_detach(now, reason);
	if(ret != TRUE) {
		META_ERROR("dll: Failed detach plugin '%s': Error from Meta_Detach(): %d", desc, ret);
		RETURN_ERRNO(mFALSE, ME_DLERROR);
	}
	META_DEBUG(6, ("dll: Plugin '%s': Called Meta_Detach() successfully", desc));
	return(mTRUE);
}

// Reload a plugin; unload and load again.
// meta_errno values:
//  - errno's from unload()
//  - errno's from load()
mBOOL MPlugin::reload(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	if(!unload(now, reason)) {
		META_ERROR("dll: Failed to unload plugin '%s' for reloading", desc);
		// meta_errno should be set already in unload()
		return(mFALSE);
	}
	if(!load(now)) {
		META_ERROR("dll: Failed to reload plugin '%s' after unloading", desc);
		// meta_errno should be set already in load()
		return(mFALSE);
	}
	return(mTRUE);
}

// Pause a plugin; temporarily disabled for API routines.
// meta_errno values:
//  - ME_ALREADY	this plugin already paused
//  - ME_BADREQ		can't pause; not running
//  - ME_NOTALLOWED	plugin doesn't want to be paused
mBOOL MPlugin::pause(void) {
	if(status == PL_PAUSED) {
		META_ERROR("Not pausing plugin '%s'; already paused", desc);
		RETURN_ERRNO(mFALSE, ME_ALREADY);
	}
	if(status != PL_RUNNING) {
		META_ERROR("Cannot pause plugin '%s'; not currently running (status=%s)", desc, str_status());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}

	// are we allowed to pause this plugin?
	if(info->unloadable < PT_ANYPAUSE) {
		META_ERROR("Cannot pause plugin '%s'; not allowed by plugin (allowed=%s)", desc, str_unloadable());
		action=PA_NONE;
		RETURN_ERRNO(mFALSE, ME_NOTALLOWED);
	}

	status=PL_PAUSED;
	META_LOG("Paused plugin '%s'", desc);
	return(mTRUE);
}

// Unpause a plugin.
// meta_errno values:
//  - ME_BADREQ		can't unpause; not paused
mBOOL MPlugin::unpause(void) {
	if(status != PL_PAUSED) {
		META_ERROR("Cannot unpause plugin '%s'; not currently paused (status=%s)", desc, str_status());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}
	status=PL_RUNNING;
	META_LOG("Unpaused plugin '%s'", desc);
	return(mTRUE);
}

// Retry pending action, presumably from a previous failure.
// meta_errno values:
//  - ME_BADREQ		no pending action
//  - errno's from load()
//  - errno's from unload()
//  - errno's from reload()
mBOOL MPlugin::retry(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
	if(action==PA_LOAD)
		return(load(now));
	else if(action==PA_ATTACH)
		return(load(now));
	else if(action==PA_UNLOAD)
		return(unload(now, reason));
	else if(action==PA_RELOAD)
		return(reload(now, reason));
	else {
		META_ERROR("No pending action to retry for plugin '%s'; (status=%s, action=%s)", desc, str_status(), str_action());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}
}

// Clear a plugin (it failed a previous action and should be
// removed from the list, or it's being unloaded).
// meta_errno values:
//  - ME_BADREQ		not marked for clearing
//  - ME_DLERROR	failed to dlclose
mBOOL MPlugin::clear(void) {
	if(status != PL_FAILED && status != PL_BADFILE
			&& status != PL_EMPTY && status != PL_OPENED) {
		META_ERROR("Cannot clear plugin '%s'; not marked as failed, empty, or open (status=%s)", desc, str_status());
		RETURN_ERRNO(mFALSE, ME_BADREQ);
	}
	// If file is open, close the file.  Note: after this, attempts to
	// reference any memory locations in the file will produce a segfault.
	if(handle && DLCLOSE(handle) != 0) {
		META_ERROR("dll: Couldn't close plugin file '%s': %s", file, DLERROR());
		status=PL_FAILED;
		RETURN_ERRNO(mFALSE, ME_DLERROR);
	}
	handle=NULL;

	if(gamedll_funcs.dllapi_table) free(gamedll_funcs.dllapi_table);
	if(gamedll_funcs.newapi_table) free(gamedll_funcs.newapi_table);

	status=PL_EMPTY;
	action=PA_NULL;
	handle=NULL;
	info=NULL;
	time_loaded=0;
	dllapi_table=NULL;
	dllapi_post_table=NULL;
	newapi_table=NULL;
	newapi_post_table=NULL;
	engine_table=NULL;
	engine_post_table=NULL;
	return(mTRUE);
}

// List information about plugin to console.
void MPlugin::show(void) {
	char *cp, *tstr;
	int n, width;
	width=13;
	META_CONS("%*s: %s", width, "name", info ? info->name : "(nil)");
	META_CONS("%*s: %s", width, "desc", desc);
	META_CONS("%*s: %s", width, "status", str_status());
	META_CONS("%*s: %s", width, "action", str_action());
	META_CONS("%*s: %s", width, "filename", filename);
	META_CONS("%*s: %s", width, "file", file);
	META_CONS("%*s: %s", width, "pathname", pathname);
	META_CONS("%*s: %d", width, "index", index);
	META_CONS("%*s: %s", width, "source", str_source());
	META_CONS("%*s: %s", width, "loadable", str_loadable(SL_ALLOWED));
	META_CONS("%*s: %s", width, "unloadable", str_unloadable(SL_ALLOWED));
	META_CONS("%*s: %s", width, "version", info ? info->version : "(nil)");
	META_CONS("%*s: %s", width, "date", info ? info->date : "(nil)");
	META_CONS("%*s: %s", width, "author", info ? info->author : "(nil)");
	META_CONS("%*s: %s", width, "url", info ? info->url : "(nil)");
	META_CONS("%*s: %s", width, "logtag", info ? info->logtag : "(nil)");
	// ctime() includes newline at EOL
	tstr=ctime(&time_loaded);
	if((cp=strchr(tstr, '\n')))
		*cp='\0';
	META_CONS("%*s: %s", width, "last loaded", tstr);
	// XXX show file time ?

	if(dllapi_table) {
		META_CONS("DLLAPI functions:");
		SHOW_DEF_DLLAPI(dllapi_table, "   ", "");
		META_CONS("%d functions (dllapi)", n);
	}
	else
		META_CONS("No DLLAPI functions.");
	if(dllapi_post_table) {
		META_CONS("DLLAPI-Post functions:");
		SHOW_DEF_DLLAPI(dllapi_post_table, "   ", "_Post");
		META_CONS("%d functions (dllapi post)", n);
	}
	else
		META_CONS("No DLLAPI-Post functions.");

	if(newapi_table) {
		META_CONS("NEWAPI functions:");
		SHOW_DEF_NEWAPI(newapi_table, "   ", "");
		META_CONS("%d functions (newapi)", n);
	}
	else
		META_CONS("No NEWAPI functions.");
	if(newapi_post_table) {
		META_CONS("NEWAPI-Post functions:");
		SHOW_DEF_NEWAPI(newapi_post_table, "   ", "_Post");
		META_CONS("%d functions (newapi post)", n);
	}
	else
		META_CONS("No NEWAPI-Post functions.");

	if(engine_table) {
		META_CONS("Engine functions:");
		SHOW_DEF_ENGINE(engine_table, "   ", "");
		META_CONS("%d functions (engine)", n);
	}
	else
		META_CONS("No Engine functions.");
	if(engine_post_table) {
		META_CONS("Engine-Post functions:");
		SHOW_DEF_ENGINE(engine_post_table, "   ", "_Post");
		META_CONS("%d functions (engine post)", n);
	}
	else
		META_CONS("No Engine-Post functions.");
	RegCmds->show(index);
	RegCvars->show(index);
}

// Check whether the file on disk looks like it's been updated since we
// last loaded the plugin.
// meta_errno values:
//  - ME_NOFILE		couldn't find file
//  - ME_NOERROR	no error; false indicates file not newer
mBOOL MPlugin::newer_file(void) {
	struct stat st;
	time_t file_time;

	if(stat(pathname, &st) != 0)
		RETURN_ERRNO(mFALSE, ME_NOFILE);
	file_time=max(st.st_ctime, st.st_mtime);
	META_DEBUG(5, ("newer_file? file=%s; load=%d, file=%d; ctime=%d, mtime=%d",
				file, time_loaded, file_time, st.st_ctime, st.st_mtime));
	if(file_time > time_loaded)
		return(mTRUE);
	else
		RETURN_ERRNO(mFALSE, ME_NOERROR);
}

// Return a string describing status of plugin.
// SIMPLE is the default.
// SHOW is max 4 chars, for "show" output.
// meta_errno values:
//  - none
char *MPlugin::str_status(STR_STATUS fmt) {
	switch(status) {
		case PL_EMPTY:
			if(fmt==ST_SHOW) return("empt");
			else return("empty");
		case PL_VALID:
			if(fmt==ST_SHOW) return("info");
			else return("valid");
		case PL_BADFILE:
			if(fmt==ST_SHOW) return("badf");
			else return("badfile");
		case PL_OPENED:
			if(fmt==ST_SHOW) return("open");
			else return("opened");
		case PL_FAILED:
			if(fmt==ST_SHOW) return("fail");
			else return("failed");
		case PL_RUNNING:
			if(fmt==ST_SHOW) return("RUN");
			else return("running");
		case PL_PAUSED:
			if(fmt==ST_SHOW) return("PAUS");
			else return("paused");
		default:
			if(fmt==ST_SHOW) return(UTIL_VarArgs("UNK%d", status));
			return(UTIL_VarArgs("unknown (%d)", status));
	}
}

// Return a string (one word) describing requested action for plugin.
// SIMPLE is the default.
// SHOW is max 4 chars, for "show" output.
// meta_errno values:
//  - none
char *MPlugin::str_action(STR_ACTION fmt) {
	switch(action) {
		case PA_NULL:
			if(fmt==SA_SHOW) return("NULL");
			else return("null");
		case PA_NONE:
			if(fmt==SA_SHOW) return(" -  ");
			else return("none");
		case PA_KEEP:
			if(fmt==SA_SHOW) return("keep");
			else return("keep");
		case PA_LOAD:
			if(fmt==SA_SHOW) return("load");
			else return("load");
		case PA_ATTACH:
			if(fmt==SA_SHOW) return("atch");
			else return("attach");
		case PA_UNLOAD:
			if(fmt==SA_SHOW) return("unld");
			else return("unload");
		case PA_RELOAD:
			if(fmt==SA_SHOW) return("relo");
			else return("reload");
		default:
			if(fmt==SA_SHOW) return(UTIL_VarArgs("UNK%d", action));
			else return(UTIL_VarArgs("unknown (%d)", action));
	}
}

// Return a string describing given plugin loadtime.
// SIMPLE is the default.
// SHOW is max 3 chars, for "show" output.
// ALLOWED is in terms of when it's allowed to load/unload.
// NOW is to describe current situation of load/unload attempt.
// meta_errno values:
//  - none
char *MPlugin::str_loadtime(PLUG_LOADTIME ptime, STR_LOADTIME fmt) {
	switch(ptime) {
		case PT_NEVER:
			if(fmt==SL_SHOW) return("Never");
			else return("never");
		case PT_STARTUP:
			if(fmt==SL_SHOW) return("Start");
			else if(fmt==SL_ALLOWED) return("at server startup");
			else if(fmt==SL_NOW) return("during server startup");
			else return("startup");
		case PT_CHANGELEVEL:
			if(fmt==SL_SHOW) return("Chlvl");
			else if(fmt==SL_ALLOWED) return("at changelevel");
			else if(fmt==SL_NOW) return("during changelevel");
			else return("changelevel");
		case PT_ANYTIME:
			if(fmt==SL_SHOW) return("ANY");
			else if(fmt==SL_ALLOWED) return("at any time");
			else if(fmt==SL_NOW) return("during map");
			else return("anytime");
		case PT_ANYPAUSE:
			if(fmt==SL_SHOW) return("Pause");
			else if(fmt==SL_ALLOWED) return("at any time, and pausable");
			else if(fmt==SL_NOW) return("for requested pause");
			else return("pausable");
		default:
			if(fmt==SL_SHOW) return(UTIL_VarArgs("UNK-%d", ptime));
			else return(UTIL_VarArgs("unknown (%d)", ptime));
	}
}

// Return a string describing why a plugin is to be unloaded.
// meta_errno values:
//  - none
char *MPlugin::str_reason(PL_UNLOAD_REASON preason) {
	switch(preason) {
		case PNL_NULL:
			return("null");
		case PNL_INI_DELETED:
			return("deleted from ini file");
		case PNL_FILE_NEWER:
			return("file on disk is newer");
		case PNL_COMMAND:
			return("server command");
		case PNL_CMD_FORCED:
			return("forced by server command");
		default:
			return(UTIL_VarArgs("unknown (%d)", preason));
	}
}

// Return a string describing how the plugin was loaded.
// meta_errno values:
//  - none
char *MPlugin::str_source(STR_SOURCE fmt) {
	switch(source) {
		case PS_INI:
			if(fmt==SO_SHOW) return("ini");
			else return("ini file");
		case PS_CMD:
			if(fmt==SO_SHOW) return("cmd");
			else return("console command");
		default:
			if(fmt==SO_SHOW) return(UTIL_VarArgs("UNK%d", source));
			else return(UTIL_VarArgs("unknown (%d)", source));
	}
}
