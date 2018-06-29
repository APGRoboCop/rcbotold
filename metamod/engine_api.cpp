// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// engine_api.cpp - implementation of Half-Life engine functions

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

#include <stdio.h>			// vsnprintf, etc
#include <stdarg.h>			// va_start, etc

#include <extdll.h>			// always

#include "engine_api.h"		// me
#include "metamod.h"		// SETUP_API_CALLS, etc
#include "thread_logparse.h"	// LogQueue, etc
#include "api_info.h"		// dllapi_info, etc
#include "log_meta.h"		// META_ERROR, etc
#include "osdep.h"			// win32 vsnprintf, etc


// Engine routines, functions returning "void".
#define META_ENGINE_HANDLE_void(FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS_void(FN_TYPE, pfnName, engine_info); \
	CALL_PLUGIN_API_void(P_PRE, pfnName, pfn_args, engine_table); \
	CALL_ENGINE_API_void(pfnName, pfn_args); \
	CALL_PLUGIN_API_void(P_POST, pfnName, pfn_args, engine_post_table);

// Engine routines, functions returning an actual value.
#define META_ENGINE_HANDLE(ret_t, ret_init, FN_TYPE, pfnName, pfn_args) \
	SETUP_API_CALLS(ret_t, ret_init, FN_TYPE, pfnName, engine_info); \
	CALL_PLUGIN_API(P_PRE, ret_init, pfnName, pfn_args, MRES_SUPERCEDE, engine_table); \
	CALL_ENGINE_API(pfnName, pfn_args); \
	CALL_PLUGIN_API(P_POST, ret_init, pfnName, pfn_args, MRES_OVERRIDE, engine_post_table);


// Engine routines, printf-style functions returning "void".
#define META_ENGINE_HANDLE_void_varargs(FN_TYPE, pfnName, pfn_arg, fmt_arg) \
	SETUP_API_CALLS_void(FN_TYPE, pfnName, engine_info); \
	char buf[MAX_STRBUF_LEN]; \
	va_list ap; \
	META_DEBUG(loglevel, ("In %s: fmt=%s", pfn_string, fmt_arg)); \
	va_start(ap, fmt_arg); \
	vsnprintf(buf, sizeof(buf), fmt_arg, ap); \
	va_end(ap); \
	CALL_PLUGIN_API_void(P_PRE, pfnName, (pfn_arg, "%s", buf), engine_table); \
	CALL_ENGINE_API_void(pfnName, (pfn_arg, "%s", buf)); \
	CALL_PLUGIN_API_void(P_POST, pfnName, (pfn_arg, "%s", buf), engine_post_table);

// Engine routines, printf-style functions returning an actual value.
#define META_ENGINE_HANDLE_varargs(ret_t, ret_init, FN_TYPE, pfnName, pfn_arg, fmt_arg) \
	SETUP_API_CALLS(ret_t, ret_init, FN_TYPE, pfnName, engine_info); \
	char buf[MAX_STRBUF_LEN]; \
	va_list ap; \
	META_DEBUG(loglevel, ("In %s: fmt=%s", pfn_string, fmt_arg)); \
	va_start(ap, fmt_arg); \
	vsnprintf(buf, sizeof(buf), fmt_arg, ap); \
	va_end(ap); \
	CALL_PLUGIN_API(P_PRE, ret_init, pfnName, (pfn_arg, "%s", buf), MRES_SUPERCEDE, engine_table); \
	CALL_ENGINE_API(pfnName, (pfn_arg, "%s", buf)); \
	CALL_PLUGIN_API(P_POST, ret_init, pfnName, (pfn_arg, "%s", buf), MRES_OVERRIDE, engine_post_table);


int mm_PrecacheModel(char *s) {
	META_ENGINE_HANDLE(int, 0, FN_PRECACHEMODEL, pfnPrecacheModel, (s));
	RETURN_API()
}
int mm_PrecacheSound(char *s) {
	META_ENGINE_HANDLE(int, 0, FN_PRECACHESOUND, pfnPrecacheSound, (s));
	RETURN_API()
}
void mm_SetModel(edict_t *e, const char *m) {
	META_ENGINE_HANDLE_void(FN_SETMODEL, pfnSetModel, (e, m));
	RETURN_API_void()
}
int mm_ModelIndex(const char *m) {
	META_ENGINE_HANDLE(int, 0, FN_MODELINDEX, pfnModelIndex, (m));
	RETURN_API()
}
int mm_ModelFrames(int modelIndex) {
	META_ENGINE_HANDLE(int, 0, FN_MODELFRAMES, pfnModelFrames, (modelIndex));
	RETURN_API()
}

void mm_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax) {
	META_ENGINE_HANDLE_void(FN_SETSIZE, pfnSetSize, (e, rgflMin, rgflMax));
	RETURN_API_void()
}
void mm_ChangeLevel(char *s1, char *s2) {
	META_ENGINE_HANDLE_void(FN_CHANGELEVEL, pfnChangeLevel, (s1, s2));
	RETURN_API_void()
}
void mm_GetSpawnParms(edict_t *ent) {
	META_ENGINE_HANDLE_void(FN_GETSPAWNPARMS, pfnGetSpawnParms, (ent));
	RETURN_API_void()
}
void mm_SaveSpawnParms(edict_t *ent) {
	META_ENGINE_HANDLE_void(FN_SAVESPAWNPARMS, pfnSaveSpawnParms, (ent));
	RETURN_API_void()
}

float mm_VecToYaw(const float *rgflVector) {
	META_ENGINE_HANDLE(float, 0.0, FN_VECTOYAW, pfnVecToYaw, (rgflVector));
	RETURN_API()
}
void mm_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut) {
	META_ENGINE_HANDLE_void(FN_VECTOANGLES, pfnVecToAngles, (rgflVectorIn, rgflVectorOut));
	RETURN_API_void()
}
void mm_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType) {
	META_ENGINE_HANDLE_void(FN_MOVETOORIGIN, pfnMoveToOrigin, (ent, pflGoal, dist, iMoveType));
	RETURN_API_void()
}
void mm_ChangeYaw(edict_t *ent) {
	META_ENGINE_HANDLE_void(FN_CHANGEYAW, pfnChangeYaw, (ent));
	RETURN_API_void()
}
void mm_ChangePitch(edict_t *ent) {
	META_ENGINE_HANDLE_void(FN_CHANGEPITCH, pfnChangePitch, (ent));
	RETURN_API_void()
}

edict_t *mm_FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_FINDENTITYBYSTRING, pfnFindEntityByString, (pEdictStartSearchAfter, pszField, pszValue));
	RETURN_API()
}
int mm_GetEntityIllum(edict_t *pEnt) {
	META_ENGINE_HANDLE(int, 0, FN_GETENTITYILLUM, pfnGetEntityIllum, (pEnt));
	RETURN_API()
}
edict_t *mm_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_FINDENTITYINSPHERE, pfnFindEntityInSphere, (pEdictStartSearchAfter, org, rad));
	RETURN_API()
}
edict_t *mm_FindClientInPVS(edict_t *pEdict) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_FINDCLIENTINPVS, pfnFindClientInPVS, (pEdict));
	RETURN_API()
}
edict_t *mm_EntitiesInPVS(edict_t *pplayer) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_ENTITIESINPVS, pfnEntitiesInPVS, (pplayer));
	RETURN_API()
}

void mm_MakeVectors(const float *rgflVector) {
	META_ENGINE_HANDLE_void(FN_MAKEVECTORS, pfnMakeVectors, (rgflVector));
	RETURN_API_void()
}
void mm_AngleVectors(const float *rgflVector, float *forward, float *right, float *up) {
	META_ENGINE_HANDLE_void(FN_ANGLEVECTORS, pfnAngleVectors, (rgflVector, forward, right, up));
	RETURN_API_void()
}

edict_t *mm_CreateEntity(void) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_CREATEENTITY, pfnCreateEntity, ());
	RETURN_API()
}
void mm_RemoveEntity(edict_t *e) {
	META_ENGINE_HANDLE_void(FN_REMOVEENTITY, pfnRemoveEntity, (e));
	RETURN_API_void()
}
edict_t *mm_CreateNamedEntity(int className) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_CREATENAMEDENTITY, pfnCreateNamedEntity, (className));
	RETURN_API()
}

void mm_MakeStatic(edict_t *ent) {
	META_ENGINE_HANDLE_void(FN_MAKESTATIC, pfnMakeStatic, (ent));
	RETURN_API_void()
}
int mm_EntIsOnFloor(edict_t *e) {
	META_ENGINE_HANDLE(int, 0, FN_ENTISONFLOOR, pfnEntIsOnFloor, (e));
	RETURN_API()
}
int mm_DropToFloor(edict_t *e) {
	META_ENGINE_HANDLE(int, 0, FN_DROPTOFLOOR, pfnDropToFloor, (e));
	RETURN_API()
}

int mm_WalkMove(edict_t *ent, float yaw, float dist, int iMode) {
	META_ENGINE_HANDLE(int, 0, FN_WALKMOVE, pfnWalkMove, (ent, yaw, dist, iMode));
	RETURN_API()
}
void mm_SetOrigin(edict_t *e, const float *rgflOrigin) {
	META_ENGINE_HANDLE_void(FN_SETORIGIN, pfnSetOrigin, (e, rgflOrigin));
	RETURN_API_void()
}

void mm_EmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch) {
	META_ENGINE_HANDLE_void(FN_EMITSOUND, pfnEmitSound, (entity, channel, sample, volume, attenuation, fFlags, pitch));
	RETURN_API_void()
}
void mm_EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch) {
	META_ENGINE_HANDLE_void(FN_EMITAMBIENTSOUND, pfnEmitAmbientSound, (entity, pos, samp, vol, attenuation, fFlags, pitch));
	RETURN_API_void()
}

void mm_TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr) {
	META_ENGINE_HANDLE_void(FN_TRACELINE, pfnTraceLine, (v1, v2, fNoMonsters, pentToSkip, ptr));
	RETURN_API_void()
}
void mm_TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr) {
	META_ENGINE_HANDLE_void(FN_TRACETOSS, pfnTraceToss, (pent, pentToIgnore, ptr));
	RETURN_API_void()
}
int mm_TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr) {
	META_ENGINE_HANDLE(int, 0, FN_TRACEMONSTERHULL, pfnTraceMonsterHull, (pEdict, v1, v2, fNoMonsters, pentToSkip, ptr));
	RETURN_API()
}
void mm_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr) {
	META_ENGINE_HANDLE_void(FN_TRACEHULL, pfnTraceHull, (v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr));
	RETURN_API_void()
}
void mm_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr) {
	META_ENGINE_HANDLE_void(FN_TRACEMODEL, pfnTraceModel, (v1, v2, hullNumber, pent, ptr));
	RETURN_API_void()
}
const char *mm_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 ) {
	META_ENGINE_HANDLE(const char *, NULL, FN_TRACETEXTURE, pfnTraceTexture, (pTextureEntity, v1, v2));
	RETURN_API()
}
void mm_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr) {
	META_ENGINE_HANDLE_void(FN_TRACESPHERE, pfnTraceSphere, (v1, v2, fNoMonsters, radius, pentToSkip, ptr));
	RETURN_API_void()
}
void mm_GetAimVector(edict_t *ent, float speed, float *rgflReturn) {
	META_ENGINE_HANDLE_void(FN_GETAIMVECTOR, pfnGetAimVector, (ent, speed, rgflReturn));
	RETURN_API_void()
}

void mm_ServerCommand(char *str) {
	META_ENGINE_HANDLE_void(FN_SERVERCOMMAND, pfnServerCommand, (str));
	RETURN_API_void()
}
void mm_ServerExecute(void) {
	META_ENGINE_HANDLE_void(FN_SERVEREXECUTE, pfnServerExecute, ());
	RETURN_API_void()
}
void mm_engClientCommand(edict_t *pEdict, char *szFmt, ...) {
	META_ENGINE_HANDLE_void_varargs(FN_CLIENTCOMMAND_ENG, pfnClientCommand, pEdict, szFmt);
	RETURN_API_void()
}

void mm_ParticleEffect(const float *org, const float *dir, float color, float count) {
	META_ENGINE_HANDLE_void(FN_PARTICLEEFFECT, pfnParticleEffect, (org, dir, color, count));
	RETURN_API_void()
}
void mm_LightStyle(int style, char *val) {
	META_ENGINE_HANDLE_void(FN_LIGHTSTYLE, pfnLightStyle, (style, val));
	RETURN_API_void()
}
int mm_DecalIndex(const char *name) {
	META_ENGINE_HANDLE(int, 0, FN_DECALINDEX, pfnDecalIndex, (name));
	RETURN_API()
}
int mm_PointContents(const float *rgflVector) {
	META_ENGINE_HANDLE(int, 0, FN_POINTCONTENTS, pfnPointContents, (rgflVector));
	RETURN_API()
}

void mm_MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) {
	META_ENGINE_HANDLE_void(FN_MESSAGEBEGIN, pfnMessageBegin, (msg_dest, msg_type, pOrigin, ed));
	RETURN_API_void()
}
void mm_MessageEnd(void) {
	META_ENGINE_HANDLE_void(FN_MESSAGEEND, pfnMessageEnd, ());
	RETURN_API_void()
}

void mm_WriteByte(int iValue) {
	META_ENGINE_HANDLE_void(FN_WRITEBYTE, pfnWriteByte, (iValue));
	RETURN_API_void()
}
void mm_WriteChar(int iValue) {
	META_ENGINE_HANDLE_void(FN_WRITECHAR, pfnWriteChar, (iValue));
	RETURN_API_void()
}
void mm_WriteShort(int iValue) {
	META_ENGINE_HANDLE_void(FN_WRITESHORT, pfnWriteShort, (iValue));
	RETURN_API_void()
}
void mm_WriteLong(int iValue) {
	META_ENGINE_HANDLE_void(FN_WRITELONG, pfnWriteLong, (iValue));
	RETURN_API_void()
}
void mm_WriteAngle(float flValue) {
	META_ENGINE_HANDLE_void(FN_WRITEANGLE, pfnWriteAngle, (flValue));
	RETURN_API_void()
}
void mm_WriteCoord(float flValue) {
	META_ENGINE_HANDLE_void(FN_WRITECOORD, pfnWriteCoord, (flValue));
	RETURN_API_void()
}
void mm_WriteString(const char *sz) {
	META_ENGINE_HANDLE_void(FN_WRITESTRING, pfnWriteString, (sz));
	RETURN_API_void()
}
void mm_WriteEntity(int iValue) {
	META_ENGINE_HANDLE_void(FN_WRITEENTITY, pfnWriteEntity, (iValue));
	RETURN_API_void()
}

void mm_CVarRegister(cvar_t *pCvar) {
	META_ENGINE_HANDLE_void(FN_CVARREGISTER, pfnCVarRegister, (pCvar));
	RETURN_API_void()
}
float mm_CVarGetFloat(const char *szVarName) {
	META_ENGINE_HANDLE(float, 0.0, FN_CVARGETFLOAT, pfnCVarGetFloat, (szVarName));
	RETURN_API()
}
const char *mm_CVarGetString(const char *szVarName) {
	META_ENGINE_HANDLE(const char *, NULL, FN_CVARGETSTRING, pfnCVarGetString, (szVarName));
	RETURN_API()
}
void mm_CVarSetFloat(const char *szVarName, float flValue) {
	META_ENGINE_HANDLE_void(FN_CVARSETFLOAT, pfnCVarSetFloat, (szVarName, flValue));
	RETURN_API_void()
}
void mm_CVarSetString(const char *szVarName, const char *szValue) {
	META_ENGINE_HANDLE_void(FN_CVARSETSTRING, pfnCVarSetString, (szVarName, szValue));
	RETURN_API_void()
}

void mm_AlertMessage(ALERT_TYPE atype, char *szFmt, ...) {
#ifndef UNFINISHED
	META_ENGINE_HANDLE_void_varargs(FN_ALERTMESSAGE, pfnAlertMessage, atype, szFmt);
#else /* UNFINISHED */
	// Expand macro, since we need to do extra work here.

	// usual setup
	SETUP_API_CALLS_void(FN_ALERTMESSAGE, pfnAlertMessage, engine_info);
	char buf[MAX_STRBUF_LEN];
	va_list ap;
	int len;
	char *qmsg;
	META_DEBUG(loglevel, ("In %s: fmt=%s", pfn_string, szFmt));
	va_start(ap, szFmt);
	len=vsnprintf(buf, sizeof(buf), szFmt, ap) + 1;
	va_end(ap);
	// pass logmsg string to log parsing thread
	/// qmsg=strdup(buf);
	qmsg=(char *) malloc(len * sizeof(char));
	if(!qmsg)
		META_ERROR("malloc failed for logmsg to thread queue");
	else {
		STRNCPY(qmsg, buf, len);
		LogQueue->push(qmsg);
	}
	// usual passing to plugins/engine
	CALL_PLUGIN_API_void(P_PRE, pfnAlertMessage, (atype, "%s", buf), engine_table);
	CALL_ENGINE_API_void(pfnAlertMessage, (atype, "%s", buf));
	CALL_PLUGIN_API_void(P_POST, pfnAlertMessage, (atype, "%s", buf), engine_post_table);
#endif /* UNFINISHED */

	// usual return.
	RETURN_API_void()
}
#ifdef HLSDK_3_2_OLD_EIFACE
void mm_EngineFprintf(FILE *pfile, char *szFmt, ...) {
#else
void mm_EngineFprintf(void *pfile, char *szFmt, ...) {
#endif
	META_ENGINE_HANDLE_void_varargs(FN_ENGINEFPRINTF, pfnEngineFprintf, pfile, szFmt);
	RETURN_API_void()
}

#ifdef HLSDK_3_2_OLD_EIFACE
void *mm_PvAllocEntPrivateData(edict_t *pEdict, long cb) {
#else
void *mm_PvAllocEntPrivateData(edict_t *pEdict, int32 cb) {
#endif
	META_ENGINE_HANDLE(void *, NULL, FN_PVALLOCENTPRIVATEDATA, pfnPvAllocEntPrivateData, (pEdict, cb));
	RETURN_API()
}
void *mm_PvEntPrivateData(edict_t *pEdict) {
	META_ENGINE_HANDLE(void *, NULL, FN_PVENTPRIVATEDATA, pfnPvEntPrivateData, (pEdict));
	RETURN_API()
}
void mm_FreeEntPrivateData(edict_t *pEdict) {
	META_ENGINE_HANDLE_void(FN_FREEENTPRIVATEDATA, pfnFreeEntPrivateData, (pEdict));
	RETURN_API_void()
}

const char *mm_SzFromIndex(int iString) {
	META_ENGINE_HANDLE(const char *, NULL, FN_SZFROMINDEX, pfnSzFromIndex, (iString));
	RETURN_API()
}
int mm_AllocString(const char *szValue) {
	META_ENGINE_HANDLE(int, 0, FN_ALLOCSTRING, pfnAllocString, (szValue));
	RETURN_API()
}

struct entvars_s *mm_GetVarsOfEnt(edict_t *pEdict) {
	META_ENGINE_HANDLE(struct entvars_s *, NULL, FN_GETVARSOFENT, pfnGetVarsOfEnt, (pEdict));
	RETURN_API()
}
edict_t *mm_PEntityOfEntOffset(int iEntOffset) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_PENTITYOFENTOFFSET, pfnPEntityOfEntOffset, (iEntOffset));
	RETURN_API()
}
int mm_EntOffsetOfPEntity(const edict_t *pEdict) {
	META_ENGINE_HANDLE(int, 0, FN_ENTOFFSETOFPENTITY, pfnEntOffsetOfPEntity, (pEdict));
	RETURN_API()
}
int mm_IndexOfEdict(const edict_t *pEdict) {
	META_ENGINE_HANDLE(int, 0, FN_INDEXOFEDICT, pfnIndexOfEdict, (pEdict));
	RETURN_API()
}
edict_t *mm_PEntityOfEntIndex(int iEntIndex) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_PENTITYOFENTINDEX, pfnPEntityOfEntIndex, (iEntIndex));
	RETURN_API()
}
edict_t *mm_FindEntityByVars(struct entvars_s *pvars) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_FINDENTITYBYVARS, pfnFindEntityByVars, (pvars));
	RETURN_API()
}
void *mm_GetModelPtr(edict_t *pEdict) {
	META_ENGINE_HANDLE(void *, NULL, FN_GETMODELPTR, pfnGetModelPtr, (pEdict));
	RETURN_API()
}

int mm_RegUserMsg(const char *pszName, int iSize) {
	int imsgid;
	MRegMsg *nmsg=NULL;
	META_ENGINE_HANDLE(int, 0, FN_REGUSERMSG, pfnRegUserMsg, (pszName, iSize));
	// Expand the macro, since we need to do extra work.
	/// RETURN_API()
	if (--CALL_API_count>0)
		/*Restore backup*/
		PublicMetaGlobals = backup_meta_globals;
	if(status==MRES_OVERRIDE) {
		META_DEBUG(loglevel, ("Returning (override) %s()", pfn_string));
		imsgid=override_ret;
	}
	else
		imsgid=orig_ret;
	// Add the msgid, name, and size to our saved list, if we haven't
	// already.
	nmsg=RegMsgs->find(imsgid);
	if(nmsg) {
		if(FStrEq(pszName, nmsg->name))
			// This name/msgid pair was already registered.
			META_DEBUG(3, ("user message registered again: name=%s, msgid=%d", pszName, imsgid));
		else
			// This msgid was previously used by a different message name.
			META_ERROR("user message id reused: msgid=%d, oldname=%s, newname=%s", imsgid, nmsg->name, pszName);
	}
	else
		RegMsgs->add(pszName, imsgid, iSize);
	return(imsgid);
}

void mm_AnimationAutomove(const edict_t *pEdict, float flTime) {
	META_ENGINE_HANDLE_void(FN_ANIMATIONAUTOMOVE, pfnAnimationAutomove, (pEdict, flTime));
	RETURN_API_void()
}
void mm_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles ) {
	META_ENGINE_HANDLE_void(FN_GETBONEPOSITION, pfnGetBonePosition, (pEdict, iBone, rgflOrigin, rgflAngles));
	RETURN_API_void()
}

#ifdef HLSDK_3_2_OLD_EIFACE
unsigned long mm_FunctionFromName( const char *pName ) {
	META_ENGINE_HANDLE(unsigned long, 0, FN_FUNCTIONFROMNAME, pfnFunctionFromName, (pName));
#else
uint32 mm_FunctionFromName( const char *pName ) {
	META_ENGINE_HANDLE(uint32, 0, FN_FUNCTIONFROMNAME, pfnFunctionFromName, (pName));
#endif
	RETURN_API()
}
#ifdef HLSDK_3_2_OLD_EIFACE
const char *mm_NameForFunction( unsigned long function ) {
#else
const char *mm_NameForFunction( uint32 function ) {
#endif
	META_ENGINE_HANDLE(const char *, NULL, FN_NAMEFORFUNCTION, pfnNameForFunction, (function));
	RETURN_API()
}

//! JOHN: engine callbacks so game DLL can print messages to individual clients
void mm_ClientPrintf( edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg ) {
	META_ENGINE_HANDLE_void(FN_CLIENTPRINTF, pfnClientPrintf, (pEdict, ptype, szMsg));
	RETURN_API_void()
}
void mm_ServerPrint( const char *szMsg ) {
	META_ENGINE_HANDLE_void(FN_SERVERPRINT, pfnServerPrint, (szMsg));
	RETURN_API_void()
}

//! these 3 added so game DLL can easily access client 'cmd' strings
const char *mm_Cmd_Args( void ) {
	META_ENGINE_HANDLE(const char *, NULL, FN_CMD_ARGS, pfnCmd_Args, ());
	RETURN_API()
}
const char *mm_Cmd_Argv( int argc ) {
	META_ENGINE_HANDLE(const char *, NULL, FN_CMD_ARGV, pfnCmd_Argv, (argc));
	RETURN_API()
}
int mm_Cmd_Argc( void ) {
	META_ENGINE_HANDLE(int, 0, FN_CMD_ARGC, pfnCmd_Argc, ());
	RETURN_API()
}

void mm_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles ) {
	META_ENGINE_HANDLE_void(FN_GETATTACHMENT, pfnGetAttachment, (pEdict, iAttachment, rgflOrigin, rgflAngles));
	RETURN_API_void()
}

void mm_CRC32_Init(CRC32_t *pulCRC) {
	META_ENGINE_HANDLE_void(FN_CRC32_INIT, pfnCRC32_Init, (pulCRC));
	RETURN_API_void()
}
void mm_CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len) {
	META_ENGINE_HANDLE_void(FN_CRC32_PROCESSBUFFER, pfnCRC32_ProcessBuffer, (pulCRC, p, len));
	RETURN_API_void()
}
void mm_CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch) {
	META_ENGINE_HANDLE_void(FN_CRC32_PROCESSBYTE, pfnCRC32_ProcessByte, (pulCRC, ch));
	RETURN_API_void()
}
CRC32_t mm_CRC32_Final(CRC32_t pulCRC) {
	META_ENGINE_HANDLE(CRC32_t, 0, FN_CRC32_FINAL, pfnCRC32_Final, (pulCRC));
	RETURN_API()
}

#ifdef HLSDK_3_2_OLD_EIFACE
long mm_RandomLong(long lLow, long lHigh) {
	META_ENGINE_HANDLE(long, 0, FN_RANDOMLONG, pfnRandomLong, (lLow, lHigh));
#else
int32 mm_RandomLong(int32 lLow, int32 lHigh) {
	META_ENGINE_HANDLE(int32, 0, FN_RANDOMLONG, pfnRandomLong, (lLow, lHigh));
#endif
	RETURN_API()
}
float mm_RandomFloat(float flLow, float flHigh) {
	META_ENGINE_HANDLE(float, 0.0, FN_RANDOMFLOAT, pfnRandomFloat, (flLow, flHigh));
	RETURN_API()
}

void mm_SetView(const edict_t *pClient, const edict_t *pViewent ) {
	META_ENGINE_HANDLE_void(FN_SETVIEW, pfnSetView, (pClient, pViewent));
	RETURN_API_void()
}
float mm_Time( void ) {
	META_ENGINE_HANDLE(float, 0.0, FN_TIME, pfnTime, ());
	RETURN_API()
}
void mm_CrosshairAngle(const edict_t *pClient, float pitch, float yaw) {
	META_ENGINE_HANDLE_void(FN_CROSSHAIRANGLE, pfnCrosshairAngle, (pClient, pitch, yaw));
	RETURN_API_void()
}

byte * mm_LoadFileForMe(char *filename, int *pLength) {
	META_ENGINE_HANDLE(byte *, NULL, FN_LOADFILEFORME, pfnLoadFileForMe, (filename, pLength));
	RETURN_API()
}
void mm_FreeFile(void *buffer) {
	META_ENGINE_HANDLE_void(FN_FREEFILE, pfnFreeFile, (buffer));
	RETURN_API_void()
}

//! trigger_endsection
void mm_EndSection(const char *pszSectionName) {
	META_ENGINE_HANDLE_void(FN_ENDSECTION, pfnEndSection, (pszSectionName));
	RETURN_API_void()
}
int mm_CompareFileTime(char *filename1, char *filename2, int *iCompare) {
	META_ENGINE_HANDLE(int, 0, FN_COMPAREFILETIME, pfnCompareFileTime, (filename1, filename2, iCompare));
	RETURN_API()
}
void mm_GetGameDir(char *szGetGameDir) {
	META_ENGINE_HANDLE_void(FN_GETGAMEDIR, pfnGetGameDir, (szGetGameDir));
	RETURN_API_void()
}
void mm_Cvar_RegisterVariable(cvar_t *variable) {
	META_ENGINE_HANDLE_void(FN_CVAR_REGISTERVARIABLE, pfnCvar_RegisterVariable, (variable));
	RETURN_API_void()
}
void mm_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds) {
	META_ENGINE_HANDLE_void(FN_FADECLIENTVOLUME, pfnFadeClientVolume, (pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds));
	RETURN_API_void()
}
void mm_SetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed) {
	META_ENGINE_HANDLE_void(FN_SETCLIENTMAXSPEED, pfnSetClientMaxspeed, (pEdict, fNewMaxspeed));
	RETURN_API_void()
}
//! returns NULL if fake client can't be created
edict_t * mm_CreateFakeClient(const char *netname) {
	META_ENGINE_HANDLE(edict_t *, NULL, FN_CREATEFAKECLIENT, pfnCreateFakeClient, (netname));
	RETURN_API()
}
void mm_RunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec ) {
	META_ENGINE_HANDLE_void(FN_RUNPLAYERMOVE, pfnRunPlayerMove, (fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec));
	RETURN_API_void()
}
int mm_NumberOfEntities(void) {
	META_ENGINE_HANDLE(int, 0, FN_NUMBEROFENTITIES, pfnNumberOfEntities, ());
	RETURN_API()
}

//! passing in NULL gets the serverinfo
char *mm_GetInfoKeyBuffer(edict_t *e) {
	META_ENGINE_HANDLE(char *, NULL, FN_GETINFOKEYBUFFER, pfnGetInfoKeyBuffer, (e));
	RETURN_API()
}
char *mm_InfoKeyValue(char *infobuffer, char *key) {
	META_ENGINE_HANDLE(char *, NULL, FN_INFOKEYVALUE, pfnInfoKeyValue, (infobuffer, key));
	RETURN_API()
}
void mm_SetKeyValue(char *infobuffer, char *key, char *value) {
	META_ENGINE_HANDLE_void(FN_SETKEYVALUE, pfnSetKeyValue, (infobuffer, key, value));
	RETURN_API_void()
}
void mm_SetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value) {
	META_ENGINE_HANDLE_void(FN_SETCLIENTKEYVALUE, pfnSetClientKeyValue, (clientIndex, infobuffer, key, value));
	RETURN_API_void()
}

int mm_IsMapValid(char *filename) {
	META_ENGINE_HANDLE(int, 0, FN_ISMAPVALID, pfnIsMapValid, (filename));
	RETURN_API()
}
void mm_StaticDecal( const float *origin, int decalIndex, int entityIndex, int modelIndex ) {
	META_ENGINE_HANDLE_void(FN_STATICDECAL, pfnStaticDecal, (origin, decalIndex, entityIndex, modelIndex));
	RETURN_API_void()
}
int mm_PrecacheGeneric(char *s) {
	META_ENGINE_HANDLE(int, 0, FN_PRECACHEGENERIC, pfnPrecacheGeneric, (s));
	RETURN_API()
}
//! returns the server assigned userid for this player. useful for logging frags, etc. returns -1 if the edict couldn't be found in the list of clients
int mm_GetPlayerUserId(edict_t *e ) {
	META_ENGINE_HANDLE(int, 0, FN_GETPLAYERUSERID, pfnGetPlayerUserId, (e));
	RETURN_API()
}
void mm_BuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) 
{
	META_ENGINE_HANDLE_void(FN_BUILDSOUNDMSG, pfnBuildSoundMsg, (entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed));
	RETURN_API_void()
}
//! is this a dedicated server?
int mm_IsDedicatedServer(void) {
	META_ENGINE_HANDLE(int, 0, FN_ISDEDICATEDSERVER, pfnIsDedicatedServer, ());
	RETURN_API()
}
cvar_t *mm_CVarGetPointer(const char *szVarName) {
	META_ENGINE_HANDLE(cvar_t *, NULL, FN_CVARGETPOINTER, pfnCVarGetPointer, (szVarName));
	RETURN_API()
}
//! returns the server assigned WONid for this player. useful for logging frags, etc. returns -1 if the edict couldn't be found in the list of clients
unsigned int mm_GetPlayerWONId(edict_t *e) {
	META_ENGINE_HANDLE(unsigned int, 0, FN_GETPLAYERWONID, pfnGetPlayerWONId, (e));
	RETURN_API()
}

//! YWB 8/1/99 TFF Physics additions
void mm_Info_RemoveKey( char *s, const char *key ) {
	META_ENGINE_HANDLE_void(FN_INFO_REMOVEKEY, pfnInfo_RemoveKey, (s, key));
	RETURN_API_void()
}
const char *mm_GetPhysicsKeyValue( const edict_t *pClient, const char *key ) {
	META_ENGINE_HANDLE(const char *, NULL, FN_GETPHYSICSKEYVALUE, pfnGetPhysicsKeyValue, (pClient, key));
	RETURN_API()
}
void mm_SetPhysicsKeyValue( const edict_t *pClient, const char *key, const char *value ) {
	META_ENGINE_HANDLE_void(FN_SETPHYSICSKEYVALUE, pfnSetPhysicsKeyValue, (pClient, key, value));
	RETURN_API_void()
}
const char *mm_GetPhysicsInfoString( const edict_t *pClient ) {
	META_ENGINE_HANDLE(const char *, NULL, FN_GETPHYSICSINFOSTRING, pfnGetPhysicsInfoString, (pClient));
	RETURN_API()
}
unsigned short mm_PrecacheEvent( int type, const char *psz ) {
	META_ENGINE_HANDLE(unsigned short, 0, FN_PRECACHEEVENT, pfnPrecacheEvent, (type, psz));
	RETURN_API()
}
void mm_PlaybackEvent( int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 ) 
{
	META_ENGINE_HANDLE_void(FN_PLAYBACKEVENT, pfnPlaybackEvent, (flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2));
	RETURN_API_void()
}

unsigned char *mm_SetFatPVS( float *org ) {
	META_ENGINE_HANDLE(unsigned char *, 0, FN_SETFATPVS, pfnSetFatPVS, (org));
	RETURN_API()
}
unsigned char *mm_SetFatPAS( float *org ) {
	META_ENGINE_HANDLE(unsigned char *, 0, FN_SETFATPAS, pfnSetFatPAS, (org));
	RETURN_API()
}

int mm_CheckVisibility( const edict_t *entity, unsigned char *pset ) {
	META_ENGINE_HANDLE(int, 0, FN_CHECKVISIBILITY, pfnCheckVisibility, (entity, pset));
	RETURN_API()
}

void mm_DeltaSetField( struct delta_s *pFields, const char *fieldname ) {
	META_ENGINE_HANDLE_void(FN_DELTASETFIELD, pfnDeltaSetField, (pFields, fieldname));
	RETURN_API_void()
}
void mm_DeltaUnsetField( struct delta_s *pFields, const char *fieldname ) {
	META_ENGINE_HANDLE_void(FN_DELTAUNSETFIELD, pfnDeltaUnsetField, (pFields, fieldname));
	RETURN_API_void()
}
void mm_DeltaAddEncoder( char *name, void (*conditionalencode)( struct delta_s *pFields, const unsigned char *from, const unsigned char *to ) ) {
	META_ENGINE_HANDLE_void(FN_DELTAADDENCODER, pfnDeltaAddEncoder, (name, conditionalencode));
	RETURN_API_void()
}
int mm_GetCurrentPlayer( void ) {
	META_ENGINE_HANDLE(int, 0, FN_GETCURRENTPLAYER, pfnGetCurrentPlayer, ());
	RETURN_API()
}
int mm_CanSkipPlayer( const edict_t *player ) {
	META_ENGINE_HANDLE(int, 0, FN_CANSKIPPLAYER, pfnCanSkipPlayer, (player));
	RETURN_API()
}
int mm_DeltaFindField( struct delta_s *pFields, const char *fieldname ) {
	META_ENGINE_HANDLE(int, 0, FN_DELTAFINDFIELD, pfnDeltaFindField, (pFields, fieldname));
	RETURN_API()
}
void mm_DeltaSetFieldByIndex( struct delta_s *pFields, int fieldNumber ) {
	META_ENGINE_HANDLE_void(FN_DELTASETFIELDBYINDEX, pfnDeltaSetFieldByIndex, (pFields, fieldNumber));
	RETURN_API_void()
}
void mm_DeltaUnsetFieldByIndex( struct delta_s *pFields, int fieldNumber ) {
	META_ENGINE_HANDLE_void(FN_DELTAUNSETFIELDBYINDEX, pfnDeltaUnsetFieldByIndex, (pFields, fieldNumber));
	RETURN_API_void()
}

void mm_SetGroupMask( int mask, int op ) {
	META_ENGINE_HANDLE_void(FN_SETGROUPMASK, pfnSetGroupMask, (mask, op));
	RETURN_API_void()
}

int mm_engCreateInstancedBaseline( int classname, struct entity_state_s *baseline ) {
	META_ENGINE_HANDLE(int, 0, FN_CREATEINSTANCEDBASELINE, pfnCreateInstancedBaseline, (classname, baseline));
	RETURN_API()
}
void mm_Cvar_DirectSet( struct cvar_s *var, char *value ) {
	META_ENGINE_HANDLE_void(FN_CVAR_DIRECTSET, pfnCvar_DirectSet, (var, value));
	RETURN_API_void()
}

//! Forces the client and server to be running with the same version of the specified file
//!( e.g., a player model ).
//! Calling this has no effect in single player
void mm_ForceUnmodified( FORCE_TYPE type, float *mins, float *maxs, const char *filename ) {
	META_ENGINE_HANDLE_void(FN_FORCEUNMODIFIED, pfnForceUnmodified, (type, mins, maxs, filename));
	RETURN_API_void()
}

void mm_GetPlayerStats( const edict_t *pClient, int *ping, int *packet_loss ) {
	META_ENGINE_HANDLE_void(FN_GETPLAYERSTATS, pfnGetPlayerStats, (pClient, ping, packet_loss));
	RETURN_API_void()
}

void mm_AddServerCommand( char *cmd_name, void (*function) (void) ) {
	META_ENGINE_HANDLE_void(FN_ADDSERVERCOMMAND, pfnAddServerCommand, (cmd_name, function));
	RETURN_API_void()
}

// Added in SDK 2.2:

//! For voice communications, set which clients hear eachother.
//! NOTE: these functions take player entity indices (starting at 1).
qboolean mm_Voice_GetClientListening(int iReceiver, int iSender) {
	META_ENGINE_HANDLE(qboolean, false, FN_VOICE_GETCLIENTLISTENING, pfnVoice_GetClientListening, (iReceiver, iSender));
	RETURN_API()
}
qboolean mm_Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen) {
	META_ENGINE_HANDLE(qboolean, false, FN_VOICE_SETCLIENTLISTENING, pfnVoice_SetClientListening, (iReceiver, iSender, bListen));
	RETURN_API()
}

// Added for HL 1109 (no SDK update):

const char *mm_GetPlayerAuthId(edict_t *e) {
	META_ENGINE_HANDLE(const char *, NULL, FN_GETPLAYERAUTHID, pfnGetPlayerAuthId, (e));
	RETURN_API()
}

// Added 2003-11-10 (no SDK update):

sequenceEntry_s *mm_SequenceGet(const char *fileName, const char *entryName) {
	META_ENGINE_HANDLE(sequenceEntry_s *, NULL, FN_SEQUENCEGET, pfnSequenceGet, (fileName, entryName));
	RETURN_API()
}

sentenceEntry_s *mm_SequencePickSentence(const char *groupName, int pickMethod, int *picked) {
	META_ENGINE_HANDLE(sentenceEntry_s *, NULL, FN_SEQUENCEPICKSENTENCE, pfnSequencePickSentence, (groupName, pickMethod, picked));
	RETURN_API()
}

int mm_GetFileSize(char *filename) {
	META_ENGINE_HANDLE(int, 0, FN_GETFILESIZE, pfnGetFileSize, (filename));
	RETURN_API()
}

unsigned int mm_GetApproxWavePlayLen(const char *filepath) {
	META_ENGINE_HANDLE(unsigned int, 0, FN_GETAPPROXWAVEPLAYLEN, pfnGetApproxWavePlayLen, (filepath));
	RETURN_API()
}

int mm_IsCareerMatch(void) {
	META_ENGINE_HANDLE(int, 0, FN_ISCAREERMATCH, pfnIsCareerMatch, ());
	RETURN_API()
}

int mm_GetLocalizedStringLength(const char *label) {
	META_ENGINE_HANDLE(int, 0, FN_GETLOCALIZEDSTRINGLENGTH, pfnGetLocalizedStringLength, (label));
	RETURN_API()
}

void mm_RegisterTutorMessageShown(int mid) {
	META_ENGINE_HANDLE_void(FN_REGISTERTUTORMESSAGESHOWN, pfnRegisterTutorMessageShown, (mid));
	RETURN_API_void()
}

int mm_GetTimesTutorMessageShown(int mid) {
	META_ENGINE_HANDLE(int, 0, FN_GETTIMESTUTORMESSAGESHOWN, pfnGetTimesTutorMessageShown, (mid));
	RETURN_API()
}

void mm_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength) {
	META_ENGINE_HANDLE_void(FN_PROCESSTUTORMESSAGEDECAYBUFFER, pfnProcessTutorMessageDecayBuffer, (buffer, bufferLength));
	RETURN_API_void()
}

void mm_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength) {
	META_ENGINE_HANDLE_void(FN_CONSTRUCTTUTORMESSAGEDECAYBUFFER, pfnConstructTutorMessageDecayBuffer, (buffer, bufferLength));
	RETURN_API_void()
}

void mm_ResetTutorMessageDecayData(void) {
	META_ENGINE_HANDLE_void(FN_RESETTUTORMESSAGEDECAYDATA, pfnResetTutorMessageDecayData, ());
	RETURN_API_void()
}

//Added 2005-08-11 (no SDK update)
void mm_QueryClientCvarValue(const edict_t *pEdict, const char *cvarName)
{
	g_Players.set_player_cvar_query(pEdict, cvarName);

	META_ENGINE_HANDLE_void(FN_QUERYCLIENTCVARVALUE, pfnQueryClientCvarValue, (pEdict, cvarName));
	RETURN_API_void();
}

//Added 2005-11-22 (no SDK update)
void mm_QueryClientCvarValue2(const edict_t *pEdict, const char *cvarName, int requestId)
{
	META_ENGINE_HANDLE_void(FN_QUERYCLIENTCVARVALUE2, pfnQueryClientCvarValue2, (pEdict, cvarName, requestId));
	RETURN_API_void();
}

meta_enginefuncs_t meta_engfuncs(
	&mm_PrecacheModel,			// pfnPrecacheModel()
	&mm_PrecacheSound,			// pfnPrecacheSound()
	&mm_SetModel,				// pfnSetModel()
	&mm_ModelIndex,				// pfnModelIndex()
	&mm_ModelFrames,			// pfnModelFrames()

	&mm_SetSize,				// pfnSetSize()
	&mm_ChangeLevel,			// pfnChangeLevel()
	&mm_GetSpawnParms,			// pfnGetSpawnParms()
	&mm_SaveSpawnParms,			// pfnSaveSpawnParms()

	&mm_VecToYaw,				// pfnVecToYaw()
	&mm_VecToAngles,			// pfnVecToAngles()
	&mm_MoveToOrigin,			// pfnMoveToOrigin()
	&mm_ChangeYaw,				// pfnChangeYaw()
	&mm_ChangePitch,			// pfnChangePitch()

	&mm_FindEntityByString,		// pfnFindEntityByString()
	&mm_GetEntityIllum,			// pfnGetEntityIllum()
	&mm_FindEntityInSphere,		// pfnFindEntityInSphere()
	&mm_FindClientInPVS,		// pfnFindClientInPVS()
	&mm_EntitiesInPVS,			// pfnEntitiesInPVS()

	&mm_MakeVectors,			// pfnMakeVectors()
	&mm_AngleVectors,			// pfnAngleVectors()

	&mm_CreateEntity,			// pfnCreateEntity()
	&mm_RemoveEntity,			// pfnRemoveEntity()
	&mm_CreateNamedEntity,		// pfnCreateNamedEntity()

	&mm_MakeStatic,				// pfnMakeStatic()
	&mm_EntIsOnFloor,			// pfnEntIsOnFloor()
	&mm_DropToFloor,			// pfnDropToFloor()

	&mm_WalkMove,				// pfnWalkMove()
	&mm_SetOrigin,				// pfnSetOrigin()

	&mm_EmitSound,				// pfnEmitSound()
	&mm_EmitAmbientSound,		// pfnEmitAmbientSound()

	&mm_TraceLine,				// pfnTraceLine()
	&mm_TraceToss,				// pfnTraceToss()
	&mm_TraceMonsterHull,		// pfnTraceMonsterHull()
	&mm_TraceHull,				// pfnTraceHull()
	&mm_TraceModel,				// pfnTraceModel()
	&mm_TraceTexture,			// pfnTraceTexture()
	&mm_TraceSphere,			// pfnTraceSphere()
	&mm_GetAimVector,			// pfnGetAimVector()

	&mm_ServerCommand,			// pfnServerCommand()
	&mm_ServerExecute,			// pfnServerExecute()
	&mm_engClientCommand,		// pfnClientCommand()	// D'oh, ClientCommand in dllapi too.

	&mm_ParticleEffect,			// pfnParticleEffect()
	&mm_LightStyle,				// pfnLightStyle()
	&mm_DecalIndex,				// pfnDecalIndex()
	&mm_PointContents,			// pfnPointContents()

	&mm_MessageBegin,			// pfnMessageBegin()
	&mm_MessageEnd,				// pfnMessageEnd()

	&mm_WriteByte,				// pfnWriteByte()
	&mm_WriteChar,				// pfnWriteChar()
	&mm_WriteShort,				// pfnWriteShort()
	&mm_WriteLong,				// pfnWriteLong()
	&mm_WriteAngle,				// pfnWriteAngle()
	&mm_WriteCoord,				// pfnWriteCoord()
	&mm_WriteString,			// pfnWriteString()
	&mm_WriteEntity,			// pfnWriteEntity()

	&mm_CVarRegister,			// pfnCVarRegister()
	&mm_CVarGetFloat,			// pfnCVarGetFloat()
	&mm_CVarGetString,			// pfnCVarGetString()
	&mm_CVarSetFloat,			// pfnCVarSetFloat()
	&mm_CVarSetString,			// pfnCVarSetString()

	&mm_AlertMessage,			// pfnAlertMessage()
	&mm_EngineFprintf,			// pfnEngineFprintf()

	&mm_PvAllocEntPrivateData,	// pfnPvAllocEntPrivateData()
	&mm_PvEntPrivateData,		// pfnPvEntPrivateData()
	&mm_FreeEntPrivateData,		// pfnFreeEntPrivateData()

	&mm_SzFromIndex,			// pfnSzFromIndex()
	&mm_AllocString,			// pfnAllocString()

	&mm_GetVarsOfEnt, 			// pfnGetVarsOfEnt()
	&mm_PEntityOfEntOffset,		// pfnPEntityOfEntOffset()
	&mm_EntOffsetOfPEntity,		// pfnEntOffsetOfPEntity()
	&mm_IndexOfEdict,			// pfnIndexOfEdict()
	&mm_PEntityOfEntIndex,		// pfnPEntityOfEntIndex()
	&mm_FindEntityByVars,		// pfnFindEntityByVars()
	&mm_GetModelPtr,			// pfnGetModelPtr()

	&mm_RegUserMsg,				// pfnRegUserMsg()

	&mm_AnimationAutomove,		// pfnAnimationAutomove()
	&mm_GetBonePosition,		// pfnGetBonePosition()

	&mm_FunctionFromName,		// pfnFunctionFromName()
	&mm_NameForFunction,		// pfnNameForFunction()

	&mm_ClientPrintf,			// pfnClientPrintf()			//! JOHN: engine callbacks so game DLL can print messages to individual clients
	&mm_ServerPrint,			// pfnServerPrint()

	&mm_Cmd_Args,				// pfnCmd_Args()				//! these 3 added 
	&mm_Cmd_Argv,				// pfnCmd_Argv()				//! so game DLL can easily 
	&mm_Cmd_Argc,				// pfnCmd_Argc()				//! access client 'cmd' strings

	&mm_GetAttachment,			// pfnGetAttachment()

	&mm_CRC32_Init,				// pfnCRC32_Init()
	&mm_CRC32_ProcessBuffer,	// pfnCRC32_ProcessBuffer()
	&mm_CRC32_ProcessByte,		// pfnCRC32_ProcessByte()
	&mm_CRC32_Final,			// pfnCRC32_Final()

	&mm_RandomLong,				// pfnRandomLong()
	&mm_RandomFloat,			// pfnRandomFloat()

	&mm_SetView,				// pfnSetView()
	&mm_Time,					// pfnTime()
	&mm_CrosshairAngle,			// pfnCrosshairAngle()

	&mm_LoadFileForMe,			// pfnLoadFileForMe()
	&mm_FreeFile,				// pfnFreeFile()

	&mm_EndSection,				// pfnEndSection()				//! trigger_endsection
	&mm_CompareFileTime,		// pfnCompareFileTime()
	&mm_GetGameDir,				// pfnGetGameDir()
	&mm_Cvar_RegisterVariable,	// pfnCvar_RegisterVariable()
	&mm_FadeClientVolume,		// pfnFadeClientVolume()
	&mm_SetClientMaxspeed,		// pfnSetClientMaxspeed()
	&mm_CreateFakeClient,		// pfnCreateFakeClient() 		//! returns NULL if fake client can't be created
	&mm_RunPlayerMove,			// pfnRunPlayerMove()
	&mm_NumberOfEntities,		// pfnNumberOfEntities()

	&mm_GetInfoKeyBuffer,		// pfnGetInfoKeyBuffer()		//! passing in NULL gets the serverinfo
	&mm_InfoKeyValue,			// pfnInfoKeyValue()
	&mm_SetKeyValue,			// pfnSetKeyValue()
	&mm_SetClientKeyValue,		// pfnSetClientKeyValue()

	&mm_IsMapValid,				// pfnIsMapValid()
	&mm_StaticDecal,			// pfnStaticDecal()
	&mm_PrecacheGeneric,		// pfnPrecacheGeneric()
	&mm_GetPlayerUserId, 		// pfnGetPlayerUserId()			//! returns the server assigned userid for this player.
	&mm_BuildSoundMsg,			// pfnBuildSoundMsg()
	&mm_IsDedicatedServer,		// pfnIsDedicatedServer()		//! is this a dedicated server?
	&mm_CVarGetPointer,			// pfnCVarGetPointer()
	&mm_GetPlayerWONId,			// pfnGetPlayerWONId()			//! returns the server assigned WONid for this player.

	//! YWB 8/1/99 TFF Physics additions
	&mm_Info_RemoveKey,			// pfnInfo_RemoveKey()
	&mm_GetPhysicsKeyValue,		// pfnGetPhysicsKeyValue()
	&mm_SetPhysicsKeyValue,		// pfnSetPhysicsKeyValue()
	&mm_GetPhysicsInfoString,	// pfnGetPhysicsInfoString()
	&mm_PrecacheEvent,			// pfnPrecacheEvent()
	&mm_PlaybackEvent,			// pfnPlaybackEvent()

	&mm_SetFatPVS,				// pfnSetFatPVS()
	&mm_SetFatPAS,				// pfnSetFatPAS()

	&mm_CheckVisibility,		// pfnCheckVisibility()

	&mm_DeltaSetField,			// pfnDeltaSetField()
	&mm_DeltaUnsetField,		// pfnDeltaUnsetField()
	&mm_DeltaAddEncoder,		// pfnDeltaAddEncoder()
	&mm_GetCurrentPlayer,		// pfnGetCurrentPlayer()
	&mm_CanSkipPlayer,			// pfnCanSkipPlayer()
	&mm_DeltaFindField,			// pfnDeltaFindField()
	&mm_DeltaSetFieldByIndex,	// pfnDeltaSetFieldByIndex()
	&mm_DeltaUnsetFieldByIndex,	// pfnDeltaUnsetFieldByIndex()

	&mm_SetGroupMask,			// pfnSetGroupMask()

	&mm_engCreateInstancedBaseline, // pfnCreateInstancedBaseline()		// D'oh, CreateInstancedBaseline in dllapi too.
	&mm_Cvar_DirectSet,			// pfnCvar_DirectSet()

	&mm_ForceUnmodified,		// pfnForceUnmodified()

	&mm_GetPlayerStats,			// pfnGetPlayerStats()

	&mm_AddServerCommand,		// pfnAddServerCommand()

	// Added in SDK 2l2:
	&mm_Voice_GetClientListening,	// pfnVoice_GetClientListening()
	&mm_Voice_SetClientListening,	// pfnVoice_SetClientListening()

	// Added for HL 1109 (no SDK update):
	&mm_GetPlayerAuthId,			// pfnGetPlayerAuthId()

	// Added 2003-11-10 (no SDK update):
	&mm_SequenceGet,					// pfnSequenceGet()
	&mm_SequencePickSentence,			// pfnSequencePickSentence()
	&mm_GetFileSize,					// pfnGetFileSize()
	&mm_GetApproxWavePlayLen,			// pfnGetApproxWavePlayLen()
	&mm_IsCareerMatch,				// pfnIsCareerMatch()
	&mm_GetLocalizedStringLength,		// pfnGetLocalizedStringLength()
	&mm_RegisterTutorMessageShown,	// pfnRegisterTutorMessageShown()
	&mm_GetTimesTutorMessageShown,	// pfnGetTimesTutorMessageShown()
	&mm_ProcessTutorMessageDecayBuffer,	// pfnProcessTutorMessageDecayBuffer()
	&mm_ConstructTutorMessageDecayBuffer,	// pfnConstructTutorMessageDecayBuffer()
	&mm_ResetTutorMessageDecayData,		// pfnResetTutorMessageDecayData()

	//Added 2005-08-11 (no SDK update)
	&mm_QueryClientCvarValue,		// pfnQueryClientCvarValue()

	//Added 2005-11-22 (no SDK update)
	&mm_QueryClientCvarValue2		// pfnQueryClientCvarValue2()
);


