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
 // waypoint.cpp
 //
 //////////////////////////////////////////////////
 //
 // Waypoint handling for navigation
 //

#ifndef __linux__
#include <io.h>
#endif
#include <fcntl.h>

#include <sys/stat.h>

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#include "enginecallback.h"
#else
#include "h_export_meta.h"
#include "dllapi.h"
#include "meta_api.h"
#endif

#include "bot.h"
#include "waypoint.h"

extern DLL_FUNCTIONS gFunctionTable;

CWaypointLocations WaypointLocations;
CWaypointVisibilityTable WaypointVisibility;

// waypoints with information bits (flags)
WAYPOINTS waypoints;//[MAX_WAYPOINTS];

//dataUnconstArray<int> g_dynamicTeleporterWaypoints;

// number of waypoints currently in use
int num_waypoints;

// declare the array of head pointers to the path structures...
PATH* paths[MAX_WAYPOINTS];

// Global check for checking waypoints to ignore for current bot.
static unsigned char g_iFailedWaypoints[MAX_WAYPOINTS];

extern CBotGlobals gBotGlobals;

void CWaypointLocations::getMaxMins(Vector const& vOrigin, int& mini, int& minj, int& mink, int& maxi, int& maxj, int& maxk)
{
	const int iLoc = std::abs(static_cast<int>(vOrigin.x + 4096.0f) / 256);
	const int jLoc = std::abs(static_cast<int>(vOrigin.y + 4096.0f) / 256);
	const int kLoc = std::abs(static_cast<int>(vOrigin.z + 4096.0f) / 256);

	// get current area
	mini = iLoc - 1;
	maxi = iLoc + 1;

	minj = jLoc - 1;
	maxj = jLoc + 1;

	mink = kLoc - 1;
	maxk = kLoc + 1;

	constexpr int iMaxLoc = MAX_WPT_LOCATIONS - 1;

	if (mini < 0)
		mini = 0;
	if (minj < 0)
		minj = 0;
	if (mink < 0)
		mink = 0;
	if (maxi > iMaxLoc)
		maxi = iMaxLoc;
	if (maxj > iMaxLoc)
		maxj = iMaxLoc;
	if (maxk > iMaxLoc)
		maxk = iMaxLoc;
}

#define FOR_EACH_WPT_LOC_BUCKET(origin)\
	int i,j,k;\
	int iMinLoci,iMinLocj,iMinLock,iMaxLoci,iMaxLocj,iMaxLock;\
\
	getMaxMins(origin,iMinLoci,iMinLocj,iMinLock,iMaxLoci,iMaxLocj,iMaxLock);\
	for ( i = iMinLoci; i <= iMaxLoci; i++ )\
	{\
		for ( j = iMinLocj; j <= iMaxLocj; j++ )\
		{\
			for ( k = iMinLock; k <= iMaxLock; k++ )\
			{\

#define END_EACH_WPT_LOC_BUCKET }\
}\
}\

///////////////
// return nearest waypoint that can be used to cover from vCoverFrom vector
int CWaypointLocations::GetCoverWaypoint(Vector const& vPlayerOrigin, const Vector& vCoverFrom, dataStack<int>* iIgnoreWpts) const
{
	const int iWaypoint = this->NearestWaypoint(vCoverFrom, REACHABLE_RANGE, -1, false, true);

	if (iWaypoint == -1)
		return -1;

	float fNearestDist = 4096.0f;

	int iNearestIndex = -1;

	FOR_EACH_WPT_LOC_BUCKET(vPlayerOrigin)

		// check each area around the current area
		// for closer waypoints
		FindNearestCoverWaypointInBucket(i, j, k, vPlayerOrigin, &fNearestDist, &iNearestIndex, iIgnoreWpts, iWaypoint);

	END_EACH_WPT_LOC_BUCKET

		/*for ( i = iMinLoci; i <= iMaxLoci; i++ )
		{
			for ( j = iMinLocj; j <= iMaxLocj; j++ )
			{
				for ( j = iMinLocj; j <= iMaxLocj; j++ )
				{
				// check each area around the current area
				// for closer waypoints
					FindNearestCoverWaypointInBucket(i,j,k,vPlayerOrigin,&fNearestDist,&iNearestIndex,iIgnoreWpts,iWaypoint);
				}
			}
		}*/

		/*if ( FBitSet(gBotGlobals.m_iDebugLevels,BOT_DEBUG_NAV_LEVEL) )
		{
			if ( !IS_DEDICATED_SERVER() && gBotGlobals.m_pListenServerEdict )
			{
				if ( iNearestIndex != -1 )
					WaypointDrawBeam(gBotGlobals.m_pListenServerEdict,vPlayerOrigin,WaypointOrigin(iNearestIndex),16,1,255,255,0,200,10);
			}
		}*/

		return iNearestIndex;
}

void CWaypointLocations::AddWptLocation(const int iIndex, const float* fOrigin)
{
	// Add a waypoint with index and at origin (for quick insertion in the list)
	//
	const int i = std::abs(static_cast<int>(fOrigin[0] + 4096.0f) / 256);
	const int j = std::abs(static_cast<int>(fOrigin[1] + 4096.0f) / 256);
	const int k = std::abs(static_cast<int>(fOrigin[2] + 4096.0f) / 256);

	m_iLocations[i][j][k].Push(iIndex);
}

void CWaypointLocations::DeleteWptLocation(const int iIndex, const float* fOrigin)
// Delete the waypoint index at the origin (for finding it quickly in the list)
//
{
	const int i = std::abs(static_cast<int>(fOrigin[0] + 4096.0f) / 256);
	const int j = std::abs(static_cast<int>(fOrigin[1] + 4096.0f) / 256);
	const int k = std::abs(static_cast<int>(fOrigin[2] + 4096.0f) / 256);

	m_iLocations[i][j][k].Remove(iIndex);
}

void CWaypointLocations::FindNearestCoverWaypointInBucket(const int i, const int j, const int k, const Vector& vOrigin,
	float* pfMinDist, int* piIndex, dataStack<int>* iIgnoreWpts,
	const int iCoverFromWpt) const
	// Search for the nearest waypoint : I.e.
		// Find the waypoint that is closest to vOrigin from the distance pfMinDist
		// And set the piIndex to the waypoint index if closer.
{
	dataStack <int> tempStack = m_iLocations[i][j][k];

	float fDist;

	std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

	if (iIgnoreWpts)
	{
		dataStack<int> ignoreWptStack = *iIgnoreWpts;
		int iWpt;

		while (!ignoreWptStack.IsEmpty())
		{
			if ((iWpt = ignoreWptStack.ChooseFromStack()) != -1)
				g_iFailedWaypoints[iWpt] = 1;
		}
	}

	while (!tempStack.IsEmpty())
	{
		const int iSelectedIndex = tempStack.ChooseFromStack();

		if (iCoverFromWpt == iSelectedIndex)
			continue;
		if (g_iFailedWaypoints[iSelectedIndex])
			continue;

		const WAYPOINT* curr_wpt = &waypoints[iSelectedIndex];

		if (curr_wpt->flags & W_FL_DELETED)
			continue;
		if (curr_wpt->flags & W_FL_UNREACHABLE)
			continue;
		if (WaypointVisibility.GetVisibilityFromTo(iCoverFromWpt, iSelectedIndex) > 0)
			continue;

		if ((fDist = (curr_wpt->origin - vOrigin).Length()) < *pfMinDist)
		{
			*piIndex = iSelectedIndex;
			*pfMinDist = fDist;
		}
	}
}

void CWaypointLocations::FindNearestInBucket(const int i, const int j, const int k, const Vector& vOrigin,
	float* pfMinDist, int* piIndex, const int iIgnoreWpt,
	const BOOL bGetVisible, const BOOL bGetUnReachable, const BOOL bIsBot,
	const dataStack<int>* iFailedWpts, const BOOL bNearestAimingOnly) const
	// Search for the nearest waypoint : I.e.
		// Find the waypoint that is closest to vOrigin from the distance pfMinDist
		// And set the piIndex to the waypoint index if closer.
{
	dataStack <int> tempStack1 = m_iLocations[i][j][k];

	float fDist;

	TraceResult tr;

	if (!bNearestAimingOnly)
	{
		std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

		if (iFailedWpts)
		{
			dataStack<int> tempStack2 = *iFailedWpts;
			int iWpt;

			while (!tempStack2.IsEmpty())
			{
				if ((iWpt = tempStack2.ChooseFromStack()) != -1)
					g_iFailedWaypoints[iWpt] = 1;
			}
		}
	}

	while (!tempStack1.IsEmpty())
	{
		const int iSelectedIndex = tempStack1.ChooseFromStack();

		if (iSelectedIndex == iIgnoreWpt)
			continue;
		if (g_iFailedWaypoints[iSelectedIndex] == 1)
			continue;

		const WAYPOINT* curr_wpt = &waypoints[iSelectedIndex];

		const int iWptFlags = curr_wpt->flags;

		if (!bGetUnReachable && iWptFlags & W_FL_UNREACHABLE)
			continue;

		if (iWptFlags & W_FL_DELETED)
			continue;

		if (bNearestAimingOnly)
		{
			if (!(iWptFlags & W_FL_AIMING))
				continue;
		}
		else
		{
			// dont make bot choose certain waypoint types
			// in case it can't go there/won't be able to make it
			if (bIsBot)
			{
				if (iWptFlags & W_FL_JUMP)
					continue;
				if (iWptFlags & W_FL_CROUCHJUMP)
					continue;
				if (iWptFlags & W_FL_FLY)
					continue;
				if (!gBotGlobals.IsMod(MOD_TS) && iWptFlags & W_FL_WALL_STICK)
					continue;
			}
		}

		if ((fDist = (curr_wpt->origin - vOrigin).Length()) < *pfMinDist)
		{
			BOOL bAdd;

			if (bGetVisible == false)
				bAdd = true;
			else
			{
				UTIL_TraceLine(vOrigin, curr_wpt->origin, ignore_monsters, dont_ignore_glass, nullptr, &tr);
				bAdd = tr.flFraction >= 1.0f;
			}

			if (bAdd)
			{
				*piIndex = iSelectedIndex;
				*pfMinDist = fDist;
			}
		}
	}
}

void CWaypointLocations::FillWaypointsInBucket(const int i, const int j, const int k, const Vector& vOrigin,
	dataStack<int>* iWaypoints, dataStack<int>* iFailedWpts) const
{
	dataStack <int> tempStack = m_iLocations[i][j][k];

	while (!tempStack.IsEmpty())
	{
		int iWpt = tempStack.ChooseFromStack();

		if (WaypointFlags(iWpt) & W_FL_UNREACHABLE)
			continue;

		iWaypoints->Push(iWpt);
	}
}

void CWaypointLocations::FillNearbyWaypoints(const Vector& vOrigin, dataStack<int>* iWaypoints, dataStack<int>* iFailedWpts) const
{
	FOR_EACH_WPT_LOC_BUCKET(vOrigin)

		FillWaypointsInBucket(i, j, k, vOrigin, iWaypoints, iFailedWpts);

	END_EACH_WPT_LOC_BUCKET
}

/////////////////////////////
// get the nearest waypoint INDEX from an origin
int CWaypointLocations::NearestWaypoint(const Vector& vOrigin, float fNearestDist, const int iIgnoreWpt,
	const BOOL bGetVisible, const BOOL bGetUnReachable, const BOOL bIsBot,
	dataStack<int>* iFailedWpts, const BOOL bNearestAimingOnly) const
{
	int iNearestIndex = -1;

	//	dataStack<int> tempStack;

	FOR_EACH_WPT_LOC_BUCKET(vOrigin)

		FindNearestInBucket(i, j, k, vOrigin, &fNearestDist, &iNearestIndex, iIgnoreWpt, bGetVisible, bGetUnReachable, bIsBot, iFailedWpts, bNearestAimingOnly);

	END_EACH_WPT_LOC_BUCKET

		return iNearestIndex;
}

//////////////////////////////////
// Draw waypoints around a player
void CWaypointLocations::DrawWaypoints(edict_t* pEntity, Vector& vOrigin, float fDist) const
{
	unsigned char* pvs;
	unsigned char* pas;

	Vector vWpt;

	pvs = nullptr;
	pas = nullptr;

	// Setup visibility, for quick vis checking
	(*gFunctionTable.pfnSetupVisibility)(nullptr, pEntity, &pvs, &pas);

	FOR_EACH_WPT_LOC_BUCKET(vOrigin)

		dataStack<int> tempStack = m_iLocations[i][j][k];

	while (!tempStack.IsEmpty())
	{
		// Draw each waypoint in distance

		const int iIndex = tempStack.ChooseFromStack();

		vWpt = WaypointOrigin(iIndex);

		if (std::fabs(vWpt.z - vOrigin.z) <= 256.0f)
		{
			pvs = ENGINE_SET_PVS(reinterpret_cast<float*>(&vWpt));

			if (!ENGINE_CHECK_VISIBILITY(pEntity, pvs))
				continue;

			//if ( WaypointDistance(vOrigin,iIndex) < fDist )
			WaypointDrawIndex(pEntity, iIndex);
		}
	}

	END_EACH_WPT_LOC_BUCKET
}

constexpr int WPT_CONVERT_FROM_HPBBOT = 1;
constexpr int WPT_CONVERT_FROM_WHICHBOT = 2;

BOOL WaypointLoad(edict_t* pEntity)
{
	char filename[512];
	char file[256];

	WAYPOINT_HDR header;
	char msg[80];
	short int num;
	short int path_index;

	int iConvertFrom = 0;

	num_waypoints = 0;

#ifdef __linux__
	snprintf(file, sizeof(file), "waypoints/%s/%s.rcw", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#else
	snprintf(file, sizeof(file), "waypoints\\%s\\%s.rcw", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#endif
	UTIL_BuildFileName(filename, file);

	if (IS_DEDICATED_SERVER())
		std::printf("loading waypoint file: %s\n", filename);

	std::FILE* bfp = std::fopen(filename, "rb");

	// If .rcw file doesnt exist
	if (bfp == nullptr)
	{
		// try loading a .wpt file
		const size_t iLen = std::strlen(filename);

		if (iLen >= 3)
		{
			filename[iLen - 1] = 't';
			filename[iLen - 2] = 'p';
			filename[iLen - 3] = 'w';
		}
		else
		{
			snprintf(msg, sizeof(msg), "%s is not a RCBot compatible waypoint file!\n", filename);
			ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);
		}

		bfp = std::fopen(filename, "rb");

		if (gBotGlobals.IsMod(MOD_NS))
			iConvertFrom = WPT_CONVERT_FROM_WHICHBOT;
		else
			iConvertFrom = WPT_CONVERT_FROM_HPBBOT;
	}

	// if file exists, read the waypoint structure from it
	if (bfp != nullptr)
	{
		std::fread(&header, sizeof header, 1, bfp);

		header.filetype[7] = 0;

		BOOL bWorkOutVisibility = (header.waypoint_file_flags & W_FILE_FL_READ_VISIBILITY) == W_FILE_FL_READ_VISIBILITY;

		if (std::strcmp(header.filetype, "RCBot") == 0 || std::strcmp(header.filetype, "HPB_bot") == 0)
		{
			if (header.waypoint_file_version != WAYPOINT_VERSION)
			{
				if (pEntity)
					ClientPrint(pEntity, HUD_PRINTNOTIFY, "Incompatible RCBot waypoint file version!\nWaypoints not loaded!\n");

				std::fclose(bfp);
				return false;
			}

			header.mapname[31] = 0;

			if (std::strcmp(header.mapname, STRING(gpGlobals->mapname)) == 0)
			{
				int i;
				WaypointInit();  // remove any existing waypoints

				for (i = 0; i < header.number_of_waypoints; i++)
				{
					std::fread(&waypoints[i], sizeof waypoints[0], 1, bfp);

					//TODO: Experimental optimisation using waypoint_origin [APG]RoboCop[CL]
					const Vector& waypoint_origin = waypoints[i].origin;

					if (waypoint_origin.x > 4096.0f || waypoint_origin.y > 4096.0f || waypoint_origin.z > 4096.0f)
					{
						BotMessage(pEntity, 0, "ERROR!!! Invalid waypoint (id: %d) origin outside map !!!", i);
						waypoints[i].flags |= W_FL_DELETED;
						waypoints[i].origin = Vector(0, 0, 0);
						continue;
					}

					WaypointLocations.AddWptLocation(i, waypoints[i].origin);

					if (iConvertFrom == WPT_CONVERT_FROM_HPBBOT)
					{
					}

					num_waypoints++;
				}

				// read and add waypoint paths...
				for (int index = 0; index < num_waypoints; index++)
				{
					// read the number of paths from this node...
					std::fread(&num, sizeof num, 1, bfp);

					for (i = 0; i < num; i++)
					{
						std::fread(&path_index, sizeof path_index, 1, bfp);

						WaypointAddPath(static_cast<short>(index), path_index);
					}
				}

				gBotGlobals.m_bWaypointsHavePaths = true;  // keep track so path can be freed
			}
			else
			{
				{
					snprintf(msg, sizeof(msg), "%s RCBot waypoints are not for this map!\n", filename);
					ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);
				}

				std::fclose(bfp);
				return false;
			}
		}
		else
		{
			if (pEntity)
			{
				snprintf(msg, sizeof(msg), "%s is not a RCBot compatible waypoint file!\n", filename);
				ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);
			}

			std::fclose(bfp);
			return false;
		}

		std::fclose(bfp);

		if (!bWorkOutVisibility)
		{
			bWorkOutVisibility = !WaypointVisibility.ReadFromFile();

			if (!bWorkOutVisibility) // read file okay
				BotMessage(nullptr, 0, "Read Waypoint Visibility File Successfully");
			else
				BotMessage(nullptr, 0, "No Waypoint Visibility File found");
		}

		if (bWorkOutVisibility)
		{
			BotMessage(nullptr, 0, "Waypoint Visibility File out of date");

			BotMessage(nullptr, 0, "Working Out Waypoint Visibility Table, this may take a while... Please wait");

			WaypointVisibility.WorkOutVisibilityTable(num_waypoints);

			BotMessage(nullptr, 0, "Done! Saving To File...");

			if (WaypointVisibility.SaveToFile())
				WaypointSave(true);
		}
	}
	else
	{
		if (!IS_DEDICATED_SERVER() && gBotGlobals.m_pListenServerEdict)
		{
			BotMessage(gBotGlobals.m_pListenServerEdict, 0, "Waypoint file %s does not exist!\n", filename);
		}
		else
		{
			BotMessage(pEntity, 0, "Waypoint file %s does not exist!\n", filename);
		}

		//AutoWaypoint();
		return false;
	}

	return true;
}

//TODO: This may cause WaypointFindPath server crashes in awkward designed maps [APG]RoboCop[CL]
/*
void AutoWaypoint()
{
	//if ( gBotGlobals.IsMod(MOD_SVENCOOP) )
	//{
	edict_t* pNode = nullptr;

		TraceResult tr;

	BotMessage(nullptr, 0, "Autowaypointing using info_nodes...");

	while ((pNode = UTIL_FindEntityByClassname(pNode, "info_node")) != nullptr)
		{
		Vector vOrigin = pNode->v.origin;

			// hit the floor
		UTIL_TraceLine(vOrigin, Vector(0, 0, -4096.0f), ignore_monsters, dont_ignore_glass, nullptr, &tr);

		Vector min = tr.vecEndPos;

			// hit the ceiling
		UTIL_TraceLine(vOrigin, Vector(0, 0, 4096.0f), ignore_monsters, dont_ignore_glass, nullptr, &tr);

		const Vector max = tr.vecEndPos;

			// small area ? might need to be a crouch waypoint
		const float fHeight = max.z - min.z;

		if (fHeight < 72)
			{
			WaypointAddOrigin(min + Vector(0, 0, 18), W_FL_CROUCH, nullptr);
			}
			else
			WaypointAddOrigin(min + Vector(0, 0, 36), 0, nullptr);

		BotMessage(nullptr, 0, "Added waypoint at node.");
		}
	//}
	}
}
*/

std::FILE* CWaypointConversion::openWaypoint() const
{
	char szFilename[512];

#ifndef __linux__
	snprintf(szFilename, sizeof(szFilename), "%s\\%s\\%s.%s", gBotGlobals.m_szModFolder, getFolder(), STRING(gpGlobals->mapname), getExtension());
#else
	snprintf(szFilename, sizeof(szFilename), "%s/%s/%s.%s", gBotGlobals.m_szModFolder, getFolder(), STRING(gpGlobals->mapname), getExtension());
#endif
	return std::fopen(szFilename, "wb");
}

/////////////////////////////
// Saves the waypoints, bVisiblilityMade will be true when a visibility file has been made OK
BOOL WaypointSave(const BOOL bVisibilityMade, CWaypointConversion* theConverter)
{
	char filename[512];
	char file[256];

	WAYPOINT_HDR header;
	int index, i;
	short int num;

	if (theConverter != nullptr)
	{
		std::strcpy(header.filetype, theConverter->getHeader());
		header.waypoint_file_version = theConverter->getVersion();
	}
	else
	{
		std::strcpy(header.filetype, "RCBot");

		header.waypoint_file_version = WAYPOINT_VERSION;

		if (!bVisibilityMade)
			header.waypoint_file_flags = W_FILE_FL_READ_VISIBILITY;
		else
			header.waypoint_file_flags = 0;
	}

	header.number_of_waypoints = num_waypoints;

	std::memset(header.mapname, 0, sizeof header.mapname);
	std::strncpy(header.mapname, STRING(gpGlobals->mapname), 31);
	header.mapname[31] = 0;

	std::FILE* bfp;

	if (theConverter != nullptr)
	{
		bfp = theConverter->openWaypoint();
	}
	else
	{
#ifdef __linux__
		snprintf(file, sizeof(file), "waypoints/%s/%s.rcw", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#else
		snprintf(file, sizeof(file), "waypoints\\%s\\%s.rcw", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#endif
		UTIL_BuildFileName(filename, file);

		bfp = std::fopen(filename, "wb"); // wb = write binary
	}

	if (bfp == nullptr)
	{
		/*	   char folder[512];

			   std::sprintf(folder,"rcbot/waypoints/%s",gBotGlobals.m_szModFolder);

			   if ( !mkdir(folder) )
			   {
				   std::sprintf(folder,"rcbot/waypoints");

				   if ( !mkdir(folder) )
				   {
					   std::sprintf(folder,"rcbot",gBotGlobals.m_szModFolder);

					   if ( !mkdir(folder) )
					   {
						   BotMessage(pEntity,0,"ERROR: RCBot NOT installed to correct directory and CANNOT save waypoint");
					   }
					   else
						   BotMessage(pEntity,0,"WARNING: RCBot not installed to correct directory (<HL>/rcbot), rcbot was made to save waypoint");
				   }
			   }
			   else
			   {
					BotMessage(pEntity,0,"ERROR: Cannot open waypoint file \"%s\" for writing, NOT SAVED (check permissions)",filename);
			   }
		*/
		return false;
	}

	/*bfp = std::fopen(filename, "wb");

	if ( bfp == NULL )
	{
		return false;
	}*/

	// write the waypoint header to the file...
	std::fwrite(&header, sizeof header, 1, bfp);

	if (theConverter != nullptr) // write converted waypoints
	{
		WAYPOINT convertedWpt;

		for (index = 0; index < num_waypoints; index++)
		{
			convertedWpt = theConverter->convert(&waypoints[index]);

			std::fwrite(&convertedWpt, sizeof waypoints[0], 1, bfp);
		}
	}
	else
	{
		// write the waypoint data to the file...
		for (index = 0; index < num_waypoints; index++)
		{
			std::fwrite(&waypoints[index], sizeof waypoints[0], 1, bfp);
		}
	}

	// save the waypoint paths...
	for (index = 0; index < num_waypoints; index++)
	{
		// count the number of paths from this node...

		const PATH* p = paths[index];
		num = 0;

		while (p != nullptr)
		{
			i = 0;

			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] != -1)
					num++;  // count path node if it's used

				i++;
			}

			p = p->next;  // go to next node in linked list
		}

		std::fwrite(&num, sizeof num, 1, bfp);  // write the count

		// now write out each path index...

		p = paths[index];

		while (p != nullptr)
		{
			i = 0;

			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] != -1)  // save path node if it's used
					std::fwrite(&p->index[i], sizeof p->index[0], 1, bfp);

				i++;
			}

			p = p->next;  // go to next node in linked list
		}
	}

	std::fclose(bfp);

	if (theConverter == nullptr)
		return WaypointVisibility.SaveToFile();

	return true;
}

static std::FILE* fp;

void WaypointDebug()
{
	//int y = 1, x = 1;

	fp = std::fopen("bot.txt", "a");
	std::fprintf(fp, "WaypointDebug: LINKED LIST ERROR!!!\n");
	std::fclose(fp);

	//x = x - 1;  // x is zero
	//y = y / x;  // cause an divide by zero exception

	//return;
}

// free the linked list of waypoint path nodes...
void WaypointFree()
{
	for (PATH*& i : paths)
	{
		if (i)
		{
			PATH* p = i;

			while (p)  // free the linked list
			{
				PATH* p_next = p->next;  // save the link to next
				std::free(p);
				p = p_next;

#ifdef _DEBUG
				int count = 0;
				count++;
				if (count > 1000) WaypointDebug();
#endif
			}

			i = nullptr;
		}
	}
}

// initialize the waypoint structures...
void WaypointInit()
{
	// have any waypoint path nodes been allocated yet?
	if (gBotGlobals.m_bWaypointsHavePaths)
		WaypointFree();  // must free previously allocated path memory

	for (int i = 0; i < MAX_WAYPOINTS; i++)
	{
		waypoints[i].flags = 0;
		waypoints[i].origin = Vector(0, 0, 0);

		paths[i] = nullptr;  // no paths allocated yet
	}

	num_waypoints = 0;

	WaypointLocations.Clear(); // free all memory
	WaypointLocations.Init(); // nullify list incase

	WaypointVisibility.ClearVisibilityTable();
}

// add a path from one waypoint (add_index) to another (path_index)...
void WaypointAddPath(const short int add_index, const short int path_index)
{
	PATH* p = paths[add_index];
	PATH* prev = nullptr;

	// find an empty slot for new path_index...
	while (p != nullptr)
	{
		int i = 0;

		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == -1)
			{
				p->index[i] = path_index;

				return;
			}

			i++;
		}

		prev = p;     // save the previous node in linked list
		p = p->next;  // go to next node in linked list

#ifdef _DEBUG
		int count = 0;

		count++;
		if (count > 100) WaypointDebug();
#endif
	}

	p = static_cast<PATH*>(std::malloc(sizeof(PATH)));

	if (p == nullptr)
	{
		char errorMsg[] = "HPB_bot - Error allocating memory for path!";
		ALERT(at_error, errorMsg);
		return; // or throw an exception
	}

	p->index[0] = path_index;
	p->index[1] = -1;
	p->index[2] = -1;
	p->index[3] = -1;
	p->next = nullptr;

	if (prev != nullptr)
		prev->next = p;  // link new node into existing list

	if (paths[add_index] == nullptr)
		paths[add_index] = p;  // save head point if necessary
}

// delete all paths to this waypoint index...
void WaypointDeletePath(const short int del_index)
{
	// search all paths for this index...
	for (int index = 0; index < num_waypoints; index++)
	{
		PATH* p = paths[index];

		// search linked list for del_index...
		while (p != nullptr)
		{
			int i = 0;

			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] == del_index)
				{
					p->index[i] = -1;  // unassign this path
				}

				i++;
			}

			p = p->next;  // go to next node in linked list

#ifdef _DEBUG
			int count = 0;
			count++;
			if (count > 100) WaypointDebug();
#endif
		}
	}
}

// delete a path from a waypoint (path_index) to another waypoint
// (del_index)...
void WaypointDeletePath(const short int path_index, const short int del_index)
{
	PATH* p = paths[path_index];

	// search linked list for del_index...
	while (p != nullptr)
	{
		int i = 0;

		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == del_index)
			{
				p->index[i] = -1;  // unassign this path
			}

			i++;
		}

		p = p->next;  // go to next node in linked list

#ifdef _DEBUG
		int count = 0;
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
}

// find a path from the current waypoint. (pPath MUST be NULL on the
// initial call. subsequent calls will return other paths if they exist.)
int WaypointFindPath(PATH** pPath, int* path_index, const int waypoint_index, const int team)
{
	if (pPath == nullptr || path_index == nullptr) //Required to reduce crashes? [APG]RoboCop[CL]
	{
		// Handle error
		return -1;
	}

	if (*pPath == nullptr)
	{
		if (waypoint_index >= 0 && waypoint_index < std::size(paths))
		{
			*pPath = paths[waypoint_index];
			*path_index = 0;
		}
		else
		{
			// Handle error
			return -1;
		}
	}

	while (*pPath != nullptr)
	{
		while (*path_index < MAX_PATH_INDEX)
		{
			if ((*pPath)->index[*path_index] != -1 && (*pPath)->index[*path_index] < num_waypoints)//TODO: triggers crash? [APG]RoboCopCL]
			{
				// save the return value
				if (const int index = (*pPath)->index[*path_index]; index >= 0 && index < MAX_WAYPOINTS)
				{
					// skip this path if next waypoint is team specific and NOT this team
					if (team != -1 && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
						(waypoints[index].flags & W_FL_TEAM) != team)
					{
						(*path_index)++;
						continue;
					}

					// set up stuff for subsequent calls...
					(*path_index)++;

					return index;
				}
				// Handle error
				return -1;
			}

			(*path_index)++;
		}

		*path_index = 0;

		if (*pPath)
		{
			if (*pPath && (*pPath)->next != nullptr)
			{
				*pPath = (*pPath)->next;
			}
			else
			{
				break;
			}
#ifdef _DEBUG
			int count = 0;
			count++;
			if (count > 100)
				WaypointDebug();
#endif
		}
	}

	return -1;
}

// find the nearest waypoint to the player and return the index
// (-1 if not found)...
int WaypointFindNearest(edict_t* pEntity, float range, int team)
{
	TraceResult tr;

	if (num_waypoints < 1)
		return -1;

	// find the nearest waypoint...

	int min_index = -1;
	float min_distance = 9999.0f;

	for (int i = 0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoints[i].flags & W_FL_TEAM_SPECIFIC &&
			(waypoints[i].flags & W_FL_TEAM) != team)
			continue;

		const float distance = (waypoints[i].origin - pEntity->v.origin).Length();

		if (distance < min_distance && distance < range)
		{
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine(pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters, pEntity->v.pContainingEntity, &tr);

			if (tr.flFraction >= 1.0f)
			{
				min_index = i;
				min_distance = distance;
			}
		}
	}

	return min_index;
}

// find the nearest waypoint to the source postition and return the index
// of that waypoint...
int WaypointFindNearest(const Vector& v_src, edict_t* pEntity, const float range, const int team)
{
	TraceResult tr;

	if (num_waypoints < 1)
		return -1;

	// find the nearest waypoint...

	int min_index = -1;
	float min_distance = 9999.0f;

	for (int index = 0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoints[index].flags & W_FL_TEAM_SPECIFIC &&
			(waypoints[index].flags & W_FL_TEAM) != team)
			continue;

		const float distance = (waypoints[index].origin - v_src).Length();

		if (distance < min_distance && distance < range)
		{
			// if waypoint is visible from source position...
			UTIL_TraceLine(v_src, waypoints[index].origin, ignore_monsters,
				pEntity->v.pContainingEntity, &tr);

			if (tr.flFraction >= 1.0f)
			{
				min_index = index;
				min_distance = distance;
			}
		}
	}

	return min_index;
}

// find the goal nearest to the source position (v_src) matching the "flags"
// bits and return the index of that waypoint...
int WaypointFindNearestGoal(const Vector& v_src, edict_t* pEntity, const float range, const int team, const int flags,
	dataStack<int>* iIgnoreWpts)
{
	//TraceResult tr;

	if (num_waypoints < 1)
		return -1;

	// find the nearest waypoint with the matching flags...

	int min_index = -1;
	float min_distance = 99999.0f;

	// use bytes 0 or 1
	std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

	if (iIgnoreWpts)
	{
		dataStack<int> tempStack = *iIgnoreWpts;
		int iWpt;

		while (!tempStack.IsEmpty())
		{
			if ((iWpt = tempStack.ChooseFromStack()) != -1)
				g_iFailedWaypoints[iWpt] = 1;
		}
	}

	for (int index = 0; index < num_waypoints; index++)
	{
		if (g_iFailedWaypoints[index])
			continue;

		const WAYPOINT* waypoint = &waypoints[index];

		const int wpt_flags = waypoint->flags;

		if (wpt_flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (wpt_flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && wpt_flags & W_FL_TEAM_SPECIFIC &&
			(wpt_flags & W_FL_TEAM) != team)
			continue;

		if ((wpt_flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match

		const float distance = (waypoint->origin - v_src).Length();

		if (distance < range && distance < min_distance)
		{
			//UTIL_TraceLine(v_src,waypoint->origin,ignore_monsters,pEntity,&tr);

			//if ( tr.flFraction >= 1.0f )
			//{
			min_index = index;
			min_distance = distance;
			//}
		}
	}

	return min_index;
}

// find a random goal matching the "flags" bits and return the index of
// that waypoint...
int WaypointFindRandomGoal(edict_t* pEntity, const int team, const int flags, dataStack<int>* iIgnoreWpts)
{
	int index;
	int indexes[50];
	int count = 0;

	if (num_waypoints < 1)
		return -1;

	std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

	if (iIgnoreWpts)
	{
		dataStack<int> tempStack = *iIgnoreWpts;
		int iWpt;

		while (!tempStack.IsEmpty())
		{
			if ((iWpt = tempStack.ChooseFromStack()) != -1)
				g_iFailedWaypoints[iWpt] = 1;
		}
	}

	// find all the waypoints with the matching flags...

	for (index = 0; index < num_waypoints; index++)
	{
		if (g_iFailedWaypoints[index])
			continue;

		const WAYPOINT* waypoint = &waypoints[index];

		if (waypoint->flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoint->flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoint->flags & W_FL_TEAM_SPECIFIC &&
			(waypoint->flags & W_FL_TEAM) != team)
			continue;

		if ((waypoint->flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match

		if (count < 50)
		{
			indexes[count] = index;

			count++;
		}
	}

	if (count == 0)  // no matching waypoints found
		return -1;

	index = RANDOM_LONG(1, count) - 1;

	return indexes[index];
}

// find a random goal matching the "flags" bits and return the index of
// that waypoint...
int WaypointFindRandomGoal(edict_t* pEntity, const int team, dataStack<int>* iIgnoreWpts)
{
	int index;
	int indexes[50];
	int count = 0;

	if (num_waypoints < 1)
		return -1;

	std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

	if (iIgnoreWpts)
	{
		dataStack<int> tempStack = *iIgnoreWpts;
		int iWpt;

		while (!tempStack.IsEmpty())
		{
			if ((iWpt = tempStack.ChooseFromStack()) != -1)
				g_iFailedWaypoints[iWpt] = 1;
		}
	}

	// find all the waypoints with the matching flags...

	for (index = 0; index < num_waypoints; index++)
	{
		if (g_iFailedWaypoints[index])
			continue;
		const WAYPOINT* waypoint = &waypoints[index];

		if (waypoint->flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoint->flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoint->flags & W_FL_TEAM_SPECIFIC &&
			(waypoint->flags & W_FL_TEAM) != team)
			continue;

		if (count < 50)
		{
			indexes[count] = index;

			count++;
		}
	}

	if (count == 0)  // no matching waypoints found
		return -1;

	index = RANDOM_LONG(1, count) - 1;

	return indexes[index];
}

// find a random goal within a range of a position (v_src) matching the
// "flags" bits and return the index of that waypoint...
int WaypointFindRandomGoal(const Vector& v_src, edict_t* pEntity, const float range, const int team, const int flags,
	dataStack<int>* iIgnoreWpts)
{
	int index;
	int indexes[50];
	int count = 0;

	if (num_waypoints < 1)
		return -1;

	// find all the waypoints with the matching flags...

	std::memset(g_iFailedWaypoints, 0, sizeof(unsigned char) * MAX_WAYPOINTS);

	if (iIgnoreWpts)
	{
		dataStack<int> tempStack = *iIgnoreWpts;
		int iWpt;

		while (!tempStack.IsEmpty())
		{
			if ((iWpt = tempStack.ChooseFromStack()) != -1)
				g_iFailedWaypoints[iWpt] = 1;
		}
	}

	for (index = 0; index < num_waypoints; index++)
	{
		if (g_iFailedWaypoints[index])
			continue;

		const WAYPOINT* waypoint = &waypoints[index];

		if (waypoint->flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoint->flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoint->flags & W_FL_TEAM_SPECIFIC &&
			(waypoint->flags & W_FL_TEAM) != team)
			continue;

		if ((waypoint->flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match

		const float distance = (waypoint->origin - v_src).Length();

		if (distance < range && count < 50)
		{
			indexes[count] = index;

			count++;
		}
	}

	if (count == 0)  // no matching waypoints found
		return -1;

	index = RANDOM_LONG(0, count - 1);

	return indexes[index];
}

// find the nearest "special" aiming waypoint (for sniper aiming)...
int WaypointFindNearestAiming(const Vector& v_origin)
{
	int min_index = -1;
	float min_distance = 9999.0f;

	if (num_waypoints < 1)
		return -1;

	// search for nearby aiming waypoint...
	for (int index = 0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if ((waypoints[index].flags & W_FL_AIMING) == 0)
			continue;  // skip any NON aiming waypoints

		if (const float distance = (v_origin - waypoints[index].origin).Length(); distance < min_distance && distance < 40)
		{
			min_index = index;
			min_distance = distance;
		}
	}

	return min_index;
}

void WaypointDrawBeam(edict_t* pEntity, const Vector& start, const Vector& end, int width,
	int noise, int red, int green, int blue, int brightness, int speed)
{
	// PM - Use MSG_ONE_UNRELIABLE
	//    - no overflows!
	MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, nullptr, pEntity);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(start.x);
	WRITE_COORD(start.y);
	WRITE_COORD(start.z);
	WRITE_COORD(end.x);
	WRITE_COORD(end.y);
	WRITE_COORD(end.z);
	WRITE_SHORT(gBotGlobals.m_iWaypointTexture);
	WRITE_BYTE(1); // framestart
	WRITE_BYTE(10); // framerate
	WRITE_BYTE(10); // life in 0.1's
	WRITE_BYTE(width); // width
	WRITE_BYTE(noise);  // noise

	WRITE_BYTE(red);   // r, g, b
	WRITE_BYTE(green);   // r, g, b
	WRITE_BYTE(blue);   // r, g, b

	WRITE_BYTE(brightness);   // brightness
	WRITE_BYTE(speed);    // speed
	MESSAGE_END();
}

int WaypointAddOrigin(Vector const& vOrigin, const int iFlags, edict_t* pEntity,
	const BOOL bDraw, const BOOL bSound, const BOOL bAutoSetFlagsForPlayer)
{
	TraceResult tr;

	int index = 0;

	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;

		index++;
	}

	if (index >= MAX_WAYPOINTS)
	{
		if (pEntity)
			BotMessage(pEntity, 0, "Can't add waypoint, max waypoints already added");
		return -1;
	}

	WAYPOINT* new_waypoint = &waypoints[index];

	if (iFlags)
		new_waypoint->flags = iFlags;
	else
		new_waypoint->flags = 0;

	// store the origin (location) of this waypoint (use entity origin)
	new_waypoint->origin = vOrigin;

	if (pEntity && bAutoSetFlagsForPlayer)
	{
		if ((pEntity->v.flags & FL_DUCKING) == FL_DUCKING)
		{
			new_waypoint->flags |= W_FL_CROUCH;  // crouching waypoint
		}

		if (pEntity->v.movetype == MOVETYPE_FLY)
			new_waypoint->flags |= W_FL_LADDER;  // waypoint on a ladder
	}

	//********************************************************
	// look for lift, ammo, flag, health, armor, etc.
	//********************************************************

	edict_t* pEnt = nullptr;

	while ((pEnt = UTIL_FindEntityInSphere(pEnt, vOrigin, 72.0f)) != nullptr)
	{
		if (pEnt->v.owner != nullptr)
			continue;

		Vector vEntOrigin = EntityOrigin(pEnt);

		UTIL_TraceLine(vOrigin, vEntOrigin, ignore_monsters, pEntity, &tr);

		if (tr.flFraction >= 1.0f || tr.pHit == pEnt)
		{
			const char* szClassname = const_cast<char*>(STRING(pEnt->v.classname));

			if (std::strncmp("ammo_", szClassname, 5) == 0)
				new_waypoint->flags |= W_FL_AMMO;

			else if (std::strncmp("weapon_", szClassname, 7) == 0)
				new_waypoint->flags |= W_FL_WEAPON;

			else if (std::strncmp("item_health", szClassname, 11) == 0 ||
				std::strncmp("item_healthkit", szClassname, 14) == 0 ||
				std::strncmp("func_healthcharger", szClassname, 18) == 0)
				new_waypoint->flags |= W_FL_HEALTH;

			else if (std::strncmp("item_armor", szClassname, 10) == 0 ||
				std::strncmp("item_battery", szClassname, 12) == 0 ||
				std::strncmp("func_recharge", szClassname, 13) == 0)
				new_waypoint->flags |= W_FL_ARMOR;
		}
	}

	if (pEntity && bSound)
	{
		UTIL_PlaySound(pEntity, "weapons/xbow_hit1.wav");
		//UTIL_PlaySound(pEntity,"weapons/xbow_hit1.wav");
		//
	}

	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;

	if (gBotGlobals.m_bAutoPathWaypoint)
	{
		// calculate all the paths to this new waypoint
		for (int i = 0; i < num_waypoints; i++)
		{
			if (i == index)
				continue;  // skip the waypoint that was just added

			const WAYPOINT* temp_waypoint = &waypoints[i];

			if (temp_waypoint->flags & W_FL_AIMING)
				continue;  // skip any aiming waypoints

			UTIL_TraceLine(vOrigin, temp_waypoint->origin, ignore_monsters, ignore_glass, nullptr, &tr);

			WaypointVisibility.SetVisibilityFromTo(index, i, tr.flFraction >= 1.0f);
			WaypointVisibility.SetVisibilityFromTo(i, index, tr.flFraction >= 1.0f);

			// check if the waypoint is reachable from the new one (one-way)
			if (WaypointReachable(vOrigin, temp_waypoint->origin))
			{
				WaypointAddPath(static_cast<short>(index), static_cast<short>(i));
			}

			// check if the new one is reachable from the waypoint (other way)
			if (WaypointReachable(temp_waypoint->origin, vOrigin))
			{
				WaypointAddPath(static_cast<short>(i), static_cast<short>(index));
			}
		}
	}

	if (pEntity && bDraw)
	{
		WaypointDrawIndex(pEntity, index);
	}

	WaypointLocations.AddWptLocation(index, vOrigin);

	return index;
}

void WaypointAdd(CClient* pClient)
{
	edict_t* pEntity = nullptr;

	if (pClient)
		pEntity = pClient->GetPlayer();

	if (pEntity)
	{
		WaypointAddOrigin(pEntity->v.origin, 0, pEntity);
	}

	return;
}

void WaypointAddAiming(edict_t* pEntity)
{
	//   edict_t *pent = NULL;

	if (num_waypoints >= MAX_WAYPOINTS)
		return;

	int index = 0;

	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;

		index++;
	}

	waypoints[index].flags = W_FL_AIMING;  // aiming waypoint

	Vector v_angle = pEntity->v.v_angle;

	v_angle.x = 0;  // reset pitch to horizontal
	v_angle.z = 0;  // reset roll to level

	UTIL_MakeVectors(v_angle);

	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = pEntity->v.origin + gpGlobals->v_forward * 25;

	// set the time that this waypoint was originally displayed...
	WaypointDrawIndex(pEntity, index);

	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;
}

void WaypointDelete(CClient* pClient)
{
	if (num_waypoints < 1)
		return;

	edict_t* pEntity = pClient->GetPlayer();

	pClient->m_iCurrentWaypoint = WaypointLocations.NearestWaypoint(pEntity->v.origin, 50.0, -1, false, true);

	const int index = pClient->m_iCurrentWaypoint;

	if (index == -1)
	{
		UTIL_PlaySound(pEntity, "weapons/wpn_denyselect.wav");

		return;
	}
	/*
	if (waypoints[index].flags & W_FL_CAMP)
	{
	   int i;
	   int min_index = -1;
	   int min_distance = 9999.0f;
	   float distance;

	   // search for nearby aiming waypoint and delete it also...
	   for (i=0; i < num_waypoints; i++)
	   {
		  if (waypoints[i].flags & W_FL_DELETED)
			 continue;  // skip any deleted waypoints

		  if ((waypoints[i].flags & W_FL_AIMING) == 0)
			 continue;  // skip any NON aiming waypoints

		  distance = (waypoints[i].origin - waypoints[index].origin).Length();

		  if ((distance < min_distance) && (distance < 40))
		  {
			 min_index = i;
			 min_distance = distance;
		  }
	   }

	   if (min_index != -1)
	   {
		  waypoints[min_index].flags = W_FL_DELETED;  // not being used
		  waypoints[min_index].origin = Vector(0,0,0);
	   }
	}*/

	// delete any paths that lead to this index...
	WaypointDeletePath(static_cast<short>(index));

	// free the path for this index...

	if (paths[index] != nullptr)
	{
		PATH* p = paths[index];

		while (p)  // free the linked list
		{
			PATH* p_next = p->next;  // save the link to next
			std::free(p);
			p = p_next;

#ifdef _DEBUG
			int count = 0;
			count++;
			if (count > 100) WaypointDebug();
#endif
		}

		paths[index] = nullptr;
	}

	waypoints[index].flags = W_FL_DELETED;  // not being used
	waypoints[index].origin = Vector(0, 0, 0);

	WaypointLocations.DeleteWptLocation(index, reinterpret_cast<float*>(&waypoints[index].origin));

	UTIL_PlaySound(pEntity, "weapons/mine_activate.wav");
}

// allow player to manually create a path from one waypoint to another
void WaypointCreatePath(CClient* pClient, const int cmd)
{
	if (pClient == nullptr)
		return;

	edict_t* pEdict = pClient->GetPlayer();

	if (pEdict == nullptr)
		return;

	if (cmd == 1)  // assign source of path
	{
		pClient->m_iPathWaypointCreateIndex = WaypointLocations.NearestWaypoint(pEdict->v.origin, 50.0f, -1);

		if (pClient->m_iPathWaypointCreateIndex == -1)
		{
			// play "cancelled" sound...
			UTIL_PlaySound(pEdict, "common/wpn_moveselect.wav");

			return;
		}

		// play "start" sound...
		UTIL_PlaySound(pEdict, "common/wpn_hudoff.wav");

		return;
	}

	if (cmd == 2)  // assign dest of path and make path
	{
		const int waypoint1 = pClient->m_iPathWaypointCreateIndex;
		const int waypoint2 = WaypointLocations.NearestWaypoint(pEdict->v.origin, 50.0f, -1);

		BOOL bError = false;

		if (waypoint1 == waypoint2)
		{
			// cant add path to same waypoint
			BotMessage(pEdict, 0, "Error: Waypoint is the same waypoint");
			bError = true;
		}
		else if (waypoint1 == -1 || waypoint2 == -1)
		{
			BotMessage(pEdict, 0, "Error: Waypoint invalid");
			bError = true;
		}
		else if (BotNavigate_FindPathFromTo(waypoint1, waypoint2, -1) != nullptr)
		{
			BotMessage(pEdict, 0, "Error: Waypoint already has a path to this waypoint");
			bError = true;
		}

		if (bError)
		{
			// play "error" sound...
			UTIL_PlaySound(pEdict, "common/wpn_denyselect.wav");

			return;
		}

		WaypointAddPath(static_cast<short>(pClient->m_iPathWaypointCreateIndex), static_cast<short>(waypoint2));

		// play "done" sound...
		UTIL_PlaySound(pEdict, "common/wpn_hudon.wav");
	}
}

// allow player to manually remove a path from one waypoint to another
void WaypointRemovePath(CClient* pClient, const int cmd)
{
	if (pClient == nullptr)
		return;

	edict_t* pEdict = pClient->GetPlayer();

	if (pEdict == nullptr)
		return;

	if (cmd == 1)  // assign source of path
	{
		pClient->m_iPathWaypointRemoveIndex = WaypointLocations.NearestWaypoint(pEdict->v.origin, 50.0f, -1);

		if (pClient->m_iPathWaypointRemoveIndex == -1)
		{
			// play "cancelled" sound...
			UTIL_PlaySound(pEdict, "common/wpn_moveselect.wav");

			return;
		}

		// play "start" sound...
		UTIL_PlaySound(pEdict, "common/wpn_hudoff.wav");

		return;
	}

	if (cmd == 2)  // assign dest of path and make path
	{
		const int waypoint2 = WaypointLocations.NearestWaypoint(pEdict->v.origin, 50.0f, -1);

		if (pClient->m_iPathWaypointRemoveIndex == -1 || waypoint2 == -1)
		{
			// play "error" sound...
			UTIL_PlaySound(pEdict, "common/wpn_denyselect.wav");

			return;
		}

		WaypointDeletePath(static_cast<short>(pClient->m_iPathWaypointRemoveIndex), static_cast<short>(waypoint2));

		// play "done" sound...
		UTIL_PlaySound(pEdict, "common/wpn_hudon.wav");
	}
}

BOOL WaypointReachable(Vector v_src, Vector v_dest, const BOOL bDistCheck)
{
	TraceResult tr;

	float distance = (v_dest - v_src).Length();

	// is the destination close enough?
	if (!bDistCheck || distance < REACHABLE_RANGE)
	{
		// check if this waypoint is "visible"...

		UTIL_TraceLine(v_src, v_dest, ignore_monsters,
			nullptr, &tr);

		// if waypoint is visible from current position (even behind head)...
		if (tr.flFraction >= 1.0f)
		{
			// check for special case of both waypoints being underwater...
			if (POINT_CONTENTS(v_src) == CONTENTS_WATER &&
				POINT_CONTENTS(v_dest) == CONTENTS_WATER)
			{
				return true;
			}

			// check for special case of waypoint being suspended in mid-air...

			// is dest waypoint higher than src? (45 is max jump height)
			if (v_dest.z > v_src.z + MAX_JUMP_HEIGHT)
			{
				const Vector& v_new_src = v_dest;
				Vector v_new_dest = v_dest;

				v_new_dest.z = v_new_dest.z - 50;  // straight down 50 units

				UTIL_TraceLine(v_new_src, v_new_dest, dont_ignore_monsters,
					nullptr, &tr);

				// check if we didn't hit anything, if not then it's in mid-air
				if (tr.flFraction >= 1.0f)
				{
					return false;  // can't reach this one
				}
			}

			// check if distance to ground increases more than jump height
			// at points between source and destination...

			const Vector v_direction = (v_dest - v_src).Normalize();  // 1 unit long
			Vector v_check = v_src;
			Vector v_down = v_src;

			v_down.z = v_down.z - 1000.0f;  // straight down 1000 units

			UTIL_TraceLine(v_check, v_down, ignore_monsters,
				nullptr, &tr);

			float last_height = tr.flFraction * 1000.0f;  // height from ground

			distance = (v_dest - v_check).Length();  // distance from goal

			while (distance > 10.0f)
			{
				// move 10 units closer to the goal...
				v_check = v_check + v_direction * 10.0f;

				v_down = v_check;
				v_down.z = v_down.z - 1000.0f;  // straight down 1000 units

				UTIL_TraceLine(v_check, v_down, ignore_monsters,
					nullptr, &tr);

				const float curr_height = tr.flFraction * 1000.0f;  // height from ground

				// is the difference in the last height and the current height
				// higher that the jump height?
				if (last_height - curr_height > MAX_JUMP_HEIGHT)
				{
					// can't get there from here...
					return false;
				}

				last_height = curr_height;

				distance = (v_dest - v_check).Length();  // distance from goal
			}

			return true;
		}
	}

	return false;
}

// find the nearest reachable waypoint
int WaypointFindReachable(edict_t* pEntity, const float range, const int team)
{
	int min_index = -1;
	TraceResult tr;

	// find the nearest waypoint...

	float min_distance = 9999.0f;

	for (int i = 0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints

		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if (team != -1 && waypoints[i].flags & W_FL_TEAM_SPECIFIC &&
			(waypoints[i].flags & W_FL_TEAM) != team)
			continue;

		const float distance = (waypoints[i].origin - pEntity->v.origin).Length();

		if (distance < min_distance)
		{
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine(pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters, pEntity->v.pContainingEntity, &tr);

			if (tr.flFraction >= 1.0f)
			{
				if (WaypointReachable(pEntity->v.origin, waypoints[i].origin))
				{
					min_index = i;
					min_distance = distance;
				}
			}
		}
	}

	// if not close enough to a waypoint then just return
	if (min_distance > range)
		return -1;

	return min_index;
}

void WaypointPrintInfo(edict_t* pEntity)
{
	char msg[80];

	// find the nearest waypoint...
	const int index = WaypointLocations.NearestWaypoint(pEntity->v.origin, 50.0f, -1, false, true, false, nullptr);

	//BOOL visible = WaypointVisibility.GetVisibilityFromTo(1,4);

	if (index == -1)
		return;

	snprintf(msg, sizeof(msg), "Waypoint %d of %d total\n", index, num_waypoints);
	ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);

	const int flags = waypoints[index].flags;

	if (flags & W_FL_TEAM_SPECIFIC)
	{
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a team only waypoint\n");

		for (int i = 0; i < MAX_TEAMS; i++)
		{
			if ((flags & W_FL_TEAM) == i)
				BotMessage(pEntity, 0, "This is for TEAM %d", i);
		}
	}
	if (flags & W_FL_LIFT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will use a lift button here and wait for a lift\n");
	if (flags & W_FL_LADDER)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is on a ladder\n");
	if (flags & W_FL_DOOR)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a door waypoint\n");
	if (flags & W_FL_HEALTH)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is health near this waypoint\n");
	if (flags & W_FL_ARMOR)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is armor near this waypoint\n");
	if (flags & W_FL_AMMO)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is ammo near this waypoint\n");
	if (flags & W_FL_CHECK_LIFT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will CHECK for a lift here\n");
	if (flags & W_FL_HUMAN_TOWER)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bots will tower-up here\n");
	if (flags & W_FL_WEAPON)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is a weapon near this waypoint\n");
	if (flags & W_FL_IMPORTANT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is an important waypoint\n");
	if (flags & W_FL_UNREACHABLE)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is UNREACHABLE!\n");
	if (flags & W_FL_DEFEND_ZONE)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at a DEFEND point\n");
	if (flags & W_FL_SCIENTIST_POINT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at a SCIENTIST goto point\n");
	//if (flags & W_FL_TFC_FLAG_POINT)
	//	ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at a FLAG point\n");
	//if (flags & W_FL_TFC_CAPTURE_POINT)
	//	ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at a CAPTURE point\n");
	if (flags & W_FL_BARNEY_POINT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at a BARNEY goto point\n");
	if (flags & W_FL_RESCUE)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is near the RESCUE area\n");
	if (flags & W_FL_PUSHABLE)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will put a PUSHABLE object here\n");
	//if (flags & W_FL_TFC_SENTRY)
	//	ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will build a SENTRY here\n");

	/*if (gBotGlobals.IsMod(MOD_TFC))
	{
			#define W_FL_GREN_THROW     (1<<30)
			#define W_FL_TFC_SNIPER     (1<<30)
			#define W_FL_TFC_DETPACK    (1<<30)

		if (flags & W_FL_GREN_THROW)
		{
			ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will snipe here\n");
		}
		else if (flags & W_FL_TFC_DETPACK)
		{
			ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will place detpack here\n");
		}
	}
	else
	{
		if (flags & W_FL_GREN_THROW)
			ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will throw a grenade here\n");
	}*/

	if (flags & W_FL_CROUCHJUMP)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will do a LONG JUMP/CROUCH JUMP here\n");
	if (flags & W_FL_WAIT_FOR_LIFT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will WAIT for a lift before going to this waypoint\n");
	if (flags & W_FL_PAIN)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is a pain/death trigger nearby\n");
	if (flags & W_FL_WALL_STICK)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a wall stick waypoint\n");
	if (flags & W_FL_JUMP)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a jump waypoint\n");
	if (flags & W_FL_TELEPORT)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a teleport waypoint\n");
	if (flags & W_FL_TANK)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a tank waypoint\n");
	if (flags & W_FL_FLY)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This is a fly waypoint\n");
	if (flags & W_FL_STAY_NEAR)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "Bot will stay close to this waypoint\n");
	if (flags & W_FL_ENDLEVEL)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "There is a map objective/end of level here\n");
	if (flags & W_FL_OPENS_LATER)
		ClientPrint(pEntity, HUD_PRINTNOTIFY, "This waypoint is at an area which opens up later in the map\n");
}

void WaypointDrawIndex(edict_t* pEntity, int index)
// display a particular waypoint index
{
	Vector colour;
	Vector start;
	Vector end;

	int waypoint_flags;

	WAYPOINT* waypoint;

	waypoint = &waypoints[index];

	colour = Vector(0.0f, 0.0f, 255.0f);
	start = waypoint->origin - Vector(0, 0, 34);
	end = start + Vector(0, 0, 68);

	waypoint_flags = waypoint->flags;

	if (waypoint_flags & W_FL_CROUCH)
	{
		start = waypoint->origin - Vector(0, 0, 17);
		end = start + Vector(0, 0, 34);
	}

	if (waypoint_flags & W_FL_AIMING)
	{
		start = waypoint->origin - Vector(0, 0, 10);
		end = start + Vector(0, 0, 14);
	}
	else if (waypoint_flags & W_FL_JUMP)
		colour = Vector(0.0f, 255.0f, 255.0f);
	else if (waypoint_flags & W_FL_LADDER)
	{
		// find next ladder waypoint

		PATH* pPath = nullptr;
		int pathindex = 0;

		int inextwpt;

		Vector vnextwpt;

		while ((inextwpt = WaypointFindPath(&pPath, &pathindex, index, -1)) != -1)
		{
			if (WaypointFlags(inextwpt) & W_FL_LADDER && (vnextwpt = WaypointOrigin(inextwpt)).z > waypoint->origin.z)
				break;
		}

		if (inextwpt != -1)
		{
			Vector vComp = vnextwpt - waypoint->origin;

			vComp = vComp.Normalize();

			WaypointDrawBeam(pEntity, waypoint->origin, waypoint->origin + vComp * 128, 32, 2, 0, 0, 255, 200, 1);
		}

		colour = Vector(255.0f, 255.0f, 0.0f);
	}
	else if (waypoint_flags & W_FL_DOOR)
		colour = Vector(0.0f, 0.0f, 255.0f);
	else if (waypoint_flags & W_FL_WALL_STICK)
		colour = Vector(200.0f, 150.0f, 0.0f);
	else if (waypoint_flags & W_FL_LIFT)
		colour = Vector(200.0f, 0.0f, 0.0f);
	else if (waypoint_flags & W_FL_GREN_THROW)
		colour = Vector(10.0f, 127.0f, 10.0f);
	else if (waypoint_flags & W_FL_CROUCH)
		colour = Vector(200.0f, 100.0f, 100.0f);
	else if (waypoint_flags & W_FL_CROUCHJUMP)
		colour = Vector(150.0f, 100.0f, 50.0f);
	else if (waypoint_flags & W_FL_TELEPORT)
		colour = Vector(150.0f, 150.0f, 150.0f);
	else if (waypoint_flags & W_FL_TANK)
		colour = Vector(200.0f, 100.0f, 0.0f);
	else if (waypoint_flags & W_FL_FLY)
		colour = Vector(0.0f, 50.0f, 150.0f);
	else if (waypoint_flags & W_FL_WAIT_FOR_LIFT)
		colour = Vector(100.0f, 150.0f, 100.0f);
	else if (waypoint_flags & W_FL_ENDLEVEL)
		colour = Vector(0.0f, 20.0f, 150.0f);
	else if (waypoint_flags & W_FL_HUMAN_TOWER)
		colour = Vector(200.0f, 100.0f, 200.0f);
	else if (waypoint_flags & W_FL_OPENS_LATER)
		colour = Vector(100.0f, 100.0f, 100.0f);
	else if (waypoint_flags & W_FL_UNREACHABLE)
		colour = Vector(255.0f, 255.0f, 255.0f);
	else if (waypoint_flags & W_FL_IMPORTANT)
		colour = Vector(0.0f, 255.0f, 0.0f);
	else if (waypoint_flags & W_FL_PUSHABLE)
		colour = Vector(255.0f, 127.0f, 0.0f);
	else if (waypoint_flags & W_FL_SCIENTIST_POINT)
		colour = Vector(127.0f, 127.0f, 127.0f);
	else if (waypoint_flags & W_FL_BARNEY_POINT)
		colour = Vector(0.0f, 0.0f, 127.0f);
	else if (waypoint_flags & W_FL_HEALTH)
		colour = Vector(200.0f, 200.0f, 180.0f);
	else if (waypoint_flags & W_FL_ARMOR)
		colour = Vector(220.0f, 180.0f, 180.0f);
	else if (waypoint_flags & W_FL_CHECK_LIFT)
		colour = Vector(213.0f, 234.0f, 129.0f); // blah

	WaypointDrawBeam(pEntity, start, end, 16, 2, static_cast<int>(colour.x), static_cast<int>(colour.y), static_cast<int>(colour.z), 200, 10);
}

// Now done in CClient :: Think ()
/*
void WaypointThink ( void )
// shows waypoints and stuff
{
	int i = 0;
	int n;

	int iCurrentWaypoint;

	CClient *cCurClient;
	edict_t *pPlayer;

	Vector vOrigin;

	for ( i = 0; i < MAX_PLAYERS; i ++ )
	{
		cCurClient = gBotGlobals.m_Clients.GetClientByIndex(i);

		if ( !cCurClient->m_bWaypointOn )// only display the waypoints if turned on...
			continue;

		if ( cCurClient->m_fWaypointDisplayTime > gpGlobals->time )
			continue;

		/////////////////////////////////////////////////////////////////
		// GET ALL INFO NEEDED...
		cCurClient->m_fWaypointDisplayTime = gpGlobals->time + 1.0f;

		pPlayer = cCurClient->GetPlayer();
		vOrigin = pPlayer->v.origin;

		iCurrentWaypoint = cCurClient->m_iCurrentWaypoint;
		/////////////////////////////////////////////////////////////////
		// DRAW WAYPOINTS
		WaypointLocations.DrawWaypoints(pPlayer,vOrigin,600);
		/////////////////////////////////////////////////////////////////

		if ( cCurClient->m_bWaypointPathsOn )
		{
			iCurrentWaypoint = WaypointLocations.NearestWaypoint(vOrigin,BOT_WAYPOINT_TOUCH_DIST,-1);

			if ( iCurrentWaypoint != -1 )
				cCurClient->m_iCurrentWaypoint;

			if ( iCurrentWaypoint != -1 )
			{
				// check if path waypointing is on...

				PATH *p;

				p = paths[iCurrentWaypoint];

				while (p != NULL)
				{
					n = 0;

					while (n < MAX_PATH_INDEX)
					{
						if (p->index[n] != -1)
						{
							// draw a white line to this index's waypoint
							WaypointDrawBeam(pPlayer, waypoints[iCurrentWaypoint].origin, waypoints[p->index[n]].origin, 10, 2, 250, 250, 250, 200, 10);
						}

						n++;
					}

					p = p->next;  // go to next node in linked list
				}
			}
		}
	}
}
*/
float WaypointDistance(const Vector& vOrigin, const int iWaypointIndex)
{
	assert(iWaypointIndex >= 0);

	if (iWaypointIndex < 0)
	{
		BotMessage(nullptr, 0, "Caution: WaypointDistance() received invalid waypoint index!");
		return 0;
	}

	return (waypoints[iWaypointIndex].origin - vOrigin).Length();
}

Vector WaypointOrigin(const int iWaypointIndex)
{
	assert(iWaypointIndex != -1);

	if (iWaypointIndex < 0)
	{
		BotMessage(nullptr, 0, "Caution: WaypointOrigin() received invalid waypoint index!");
		return { 0, 0, 0 };
	}

	return waypoints[iWaypointIndex].origin;
}

int WaypointFlags(const int iWaypointIndex)
{
	//assert ( iWaypointIndex != -1 );

	if (iWaypointIndex < 0 || iWaypointIndex > MAX_WAYPOINTS)
	{
		//BotMessage(NULL,0,"Caution: WaypointFlags() received invalid waypoint index!");
		return 0;
	}

	return waypoints[iWaypointIndex].flags;
}

int NearestWaypointToEdict(edict_t* pEdict, const int iIgnoreWpt, dataStack<int>* iFailedWpts)
{
	return WaypointLocations.NearestWaypoint(EntityOrigin(pEdict), REACHABLE_RANGE, iIgnoreWpt, true, false, false, iFailedWpts);
}

int NearestWaypointToOrigin(const Vector& vOrigin, const int iIgnoreWpt, dataStack<int>* iFailedWpts)
{
	return WaypointLocations.NearestWaypoint(vOrigin, REACHABLE_RANGE, iIgnoreWpt, true, false, false, iFailedWpts);
}

WAYPOINT& WAYPOINTS :: operator [] (const int index)
{
	if (index >= 0 && index < MAX_WAYPOINTS) // Inside array bounds?
		return m_Waypoints[index];  // Return index
	else
	{
		BugMessage(nullptr, "Waypoint Error : Index Value Doesn't Exist");
		return m_Waypoints[0]; // Return the last waypoint?
	}
}

void CWaypointVisibilityTable::WorkOutVisibilityTable(const int iNumWaypoints) const
{
	TraceResult tr;

	ClearVisibilityTable();

	// loop through all waypoint possibilities.
	for (int i = 0; i < iNumWaypoints; i++)
	{
		const WAYPOINT* Waypoint1 = &waypoints[i];

		if (Waypoint1->flags & W_FL_DELETED)
			continue;

		for (int j = 0; j < iNumWaypoints; j++)
		{
			if (i == j)
			{
				SetVisibilityFromTo(i, j, 1);
				continue;
			}

			const WAYPOINT* Waypoint2 = &waypoints[j];

			if (Waypoint2->flags & W_FL_DELETED)
				continue;

			UTIL_TraceLine(Waypoint1->origin, Waypoint2->origin + Vector(0, 0, 16.0f), ignore_monsters, ignore_glass, nullptr, &tr);

			SetVisibilityFromTo(i, j, tr.flFraction >= 1.0f);
		}
	}
}

BOOL CWaypointVisibilityTable::SaveToFile() const
{
	char filename[512];
	char file[256];

#ifdef __linux__
	snprintf(file, sizeof(file), "waypoints/%s/%s.rcv", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#else
	snprintf(file, sizeof(file), "waypoints\\%s\\%s.rcv", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#endif
	UTIL_BuildFileName(filename, file);

	if (IS_DEDICATED_SERVER())
		std::printf("loading waypoint file: %s\n", filename);

	std::FILE* bfp = std::fopen(filename, "wb"); // binary file write

	if (bfp == nullptr)
	{
		BotMessage(nullptr, 0, "Can't open Waypoint Visibility table for writing!");
		return false;
	}

	std::fwrite(m_VisTable, sizeof(unsigned char), Ceiling(static_cast<float>(num_waypoints * num_waypoints) / 8), bfp);

	std::fclose(bfp);

	return true;
}

BOOL CWaypointVisibilityTable::ReadFromFile() const
{
	char filename[512];
	char file[256];
	int iSize;
	int iDesiredSize;

	// get the folder right.
#ifdef __linux__
	snprintf(file, sizeof(file), "waypoints/%s/%s.rcv", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#else
	snprintf(file, sizeof(file), "waypoints\\%s\\%s.rcv", gBotGlobals.m_szModFolder, STRING(gpGlobals->mapname));
#endif
	UTIL_BuildFileName(filename, file);

	if (IS_DEDICATED_SERVER())
		std::printf("loading waypoint file: %s\n", filename);

	std::FILE* bfp = std::fopen(filename, "rb"); // binary file write

	if (bfp == nullptr)
	{
		BotMessage(nullptr, 0, "Can't open Waypoint Visibility table for reading!");
		return false;
	}

	std::fseek(bfp, 0, SEEK_END); // seek at end

	iSize = std::ftell(bfp); // get file size
	iDesiredSize = Ceiling(static_cast<float>(num_waypoints * num_waypoints) / 8);

	// size not right, return false to re workout table
	if (iSize != iDesiredSize)
	{
		std::fclose(bfp);
		return false;
	}

	std::fseek(bfp, 0, SEEK_SET); // seek at start

	// clear table
	std::memset(m_VisTable, 0, sizeof g_iMaxVisibilityByte);

	// read vis table
	std::fread(m_VisTable, sizeof(unsigned char), iDesiredSize, bfp);

	std::fclose(bfp);

	return true;
}

BOOL WaypointFlagsOnLadderOrFly(const int iWaypointFlags)
{
	// return true if waypoint index is on a ladder or a fly waypoint.
	return (iWaypointFlags & (W_FL_FLY | W_FL_LADDER)) > 0;
}

// return player edict pointer nearby origin within range.
edict_t* PlayerNearVector(const Vector& vOrigin, float fRange)
{
	edict_t* pNearest = nullptr;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t* pPlayer = INDEXENT(i);

		if (pPlayer == nullptr)
			continue;

		if (!*STRING(pPlayer->v.netname))
			continue;

		if (!EntityIsAlive(pPlayer))
			continue;

		const float fDist = (EntityOrigin(pPlayer) - vOrigin).Length();

		if (fDist <= fRange)
		{
			fRange = fDist;

			pNearest = pPlayer;
		}
	}

	return pNearest;
}