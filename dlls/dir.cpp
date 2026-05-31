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
/// Returns true if the path has at least one sub-directory.
///
/// Self-contained on purpose: it does NOT reuse the stateful FindDirectory()
/// iterator. The previous version walked FindDirectory() and bailed out early,
/// which on dedicated servers conflated "directory I couldn't read" with
/// "directory is empty" and risked leaving shared iteration state dangling.
/// Here we own the opendir/closedir (or FindFirstFile/FindClose) pair, always
/// close it, and a directory we can't open simply reports "no subdirs". The
/// real safety net for an empty result is the fallback + empty-list guard in
/// CBotGlobals::LoadBotModels(). [APG]RoboCop[CL]
/// </summary>
bool HasSubDirectories(const char* path)
{
#ifndef __linux__
	char search_path[MAX_PATH];

	strncpy_s(search_path, MAX_PATH, path, _TRUNCATE);
	strncat_s(search_path, MAX_PATH, "\\*", _TRUNCATE);

	WIN32_FIND_DATA find_data;
	HANDLE directory = FindFirstFile(search_path, &find_data);

	if (directory == INVALID_HANDLE_VALUE)
		return false;

	bool found = false;

	do
	{
		if (std::strcmp(find_data.cFileName, ".") == 0 || std::strcmp(find_data.cFileName, "..") == 0)
			continue;

		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			found = true;
			break;
		}
	} while (FindNextFile(directory, &find_data) != 0);

	FindClose(directory);
	return found;
#else
	DIR* directory = opendir(path);

	if (directory == nullptr)
		return false;

	bool found = false;
	struct dirent* entry;

	while ((entry = readdir(directory)) != nullptr)
	{
		if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
			continue;

		// Confirm it's actually a sub-directory. Some filesystems don't populate
		// dirent::d_type, so stat() the full path rather than trusting d_type.
		if (std::strlen(path) + 1 + std::strlen(entry->d_name) >= MAX_PATHNAME_LENGTH)
			continue;

		char pathname[MAX_PATHNAME_LENGTH];

		std::strcpy(pathname, path);
		std::strcat(pathname, "/");
		std::strcat(pathname, entry->d_name);

		struct stat stat_str;

		if (stat(pathname, &stat_str) == 0 && S_ISDIR(stat_str.st_mode))
		{
			found = true;
			break;
		}
	}

	closedir(directory);
	return found;
#endif
}