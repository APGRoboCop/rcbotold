// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
 //////////////////////
 // Directory code
 //
 // from botman's HPB_Bot
 //
#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "dir.h"
#include <cstring>

/////////////////////////////////////////////
// DIRECTORY ROUTINES...

#ifndef __linux__

// Windows OS Routine...
// MS-DOS directory wildcard routines...

// Helper function to find next directory entry
static bool FindNextDirectoryEntry(HANDLE hFile, WIN32_FIND_DATA* pFindFileData)
{
	while (FindNextFile(hFile, pFindFileData) != 0)
	{
		if ((pFindFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}
	return false;
}

// Call Find Directory with dirspec = the starting directory you want to look in
HANDLE FindDirectory(HANDLE hFile, char* dirname, const char* dirspec)
{
	WIN32_FIND_DATA FindFileData;

	dirname[0] = '\0';

	if (hFile == nullptr)
	{
		hFile = FindFirstFile(dirspec, &FindFileData);

		if (hFile == INVALID_HANDLE_VALUE)
			return nullptr;

		// Check if the first entry is a directory
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!FindNextDirectoryEntry(hFile, &FindFileData))
			{
				FindClose(hFile);
				return nullptr;
			}
		}
	}
	else
	{
		if (!FindNextDirectoryEntry(hFile, &FindFileData))
		{
			FindClose(hFile);
			return nullptr;
		}
	}

	strncpy_s(dirname, MAX_PATH, FindFileData.cFileName, _TRUNCATE);
	return hFile;
}

#else

// Linux directory wildcard routines...

constexpr size_t MAX_PATHNAME_LENGTH = 256;

DIR* FindDirectory(DIR* directory, char* dirname, const char* dirspec)
{
	char pathname[MAX_PATHNAME_LENGTH];
	struct stat stat_str;

	if (directory == nullptr)
	{
		directory = opendir(dirspec);
		if (directory == nullptr)
			return nullptr;
	}

	while (true)
	{
		struct dirent* entry = readdir(directory);

		if (entry == nullptr)
		{
			closedir(directory);
			return nullptr;
		}

		// Build full pathname safely
		const size_t dirspec_len = std::strlen(dirspec);
		const size_t name_len = std::strlen(entry->d_name);
		
		if (dirspec_len + 1 + name_len >= MAX_PATHNAME_LENGTH)
			continue; // Skip entries that would overflow

		std::strcpy(pathname, dirspec);
		std::strcat(pathname, "/");
		std::strcat(pathname, entry->d_name);

		if (stat(pathname, &stat_str) == 0 && S_ISDIR(stat_str.st_mode))
		{
			std::strcpy(dirname, entry->d_name);
			return directory;
		}
	}
}

#endif

/// <summary>
/// Returns true if the path has any sub-directories
/// </summary>
bool HasSubDirectories(const char* path)
{
#ifndef __linux__
	char search_path[MAX_PATH];
	char dirname[MAX_PATH];
	HANDLE directory = nullptr;

	strncpy_s(search_path, MAX_PATH, path, _TRUNCATE);
	strncat_s(search_path, MAX_PATH, "/*", _TRUNCATE);
#else
	char search_path[MAX_PATHNAME_LENGTH];
	char dirname[MAX_PATHNAME_LENGTH];
	DIR* directory = nullptr;

	std::strncpy(search_path, path, MAX_PATHNAME_LENGTH - 1);
	search_path[MAX_PATHNAME_LENGTH - 1] = '\0';
#endif

	while ((directory = FindDirectory(directory, dirname, search_path)) != nullptr)
	{
		// Skip current and parent directory entries
		if (dirname[0] == '.' && (dirname[1] == '\0' || (dirname[1] == '.' && dirname[2] == '\0')))
			continue;

		// Found a valid subdirectory - clean up and return
#ifndef __linux__
		FindClose(directory);
#else
		closedir(directory);
#endif
		return true;
	}

	return false;
}