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

/////////////////////////////////////////////
// DIRECTORY ROUTINES...

#ifndef __linux__

// Windows OS Routine...
// MS-DOS directory wildcard routines...

// Call Find Directory with dirspec = the starting directory you want to look in

HANDLE FindDirectory(HANDLE hFile, char *dirname, char *dirspec)
{
   WIN32_FIND_DATA pFindFileData;

   dirname[0] = 0;

   if (hFile == NULL)
   {
      hFile = FindFirstFile(dirspec, &pFindFileData);

      if (hFile == INVALID_HANDLE_VALUE)
	  {
		  // PM : bugfix
         return NULL;
	  }

      while ( (pFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY )
      {
         if (FindNextFile(hFile, &pFindFileData) == 0)
         {
            FindClose(hFile);
            hFile = NULL;
            return hFile;
         }
      }

      strcpy(dirname, pFindFileData.cFileName);

      return hFile;
   }
   else
   {
      if (FindNextFile(hFile, &pFindFileData) == 0)
      {
         FindClose(hFile);
         hFile = NULL;
         return hFile;
      }

	  // BUGGG FIXXX....!!!

	  // Fixes bots only choosing OTIS and HELMET in svencoop with some peoples half-life!
      while ( (pFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY )
      {
         if (FindNextFile(hFile, &pFindFileData) == 0)
         {
            FindClose(hFile);
            hFile = NULL;
            return hFile;
         }	 
      }

      strcpy(dirname, pFindFileData.cFileName);

      return hFile;
   }
}

#else

// Linux directory wildcard routines...

DIR *FindDirectory(DIR *directory, char *dirname, char *dirspec)
{
   char pathname[256];
   struct dirent *dirent;
   struct stat stat_str;

   if (directory == NULL)
   {
      if ((directory = opendir(dirspec)) == NULL)
         return NULL;
   }

   while (1)
   {
      dirent = readdir(directory);

      if (dirent == NULL)  // at end of directory?
      {
         closedir(directory);
         return NULL;
      }

      strcpy(pathname, dirspec);
      strcat(pathname, "/");
      strcat(pathname, dirent->d_name);

      if (stat(pathname, &stat_str) == 0)
      {
         if (stat_str.st_mode & S_IFDIR)
         {
            strcpy(dirname, dirent->d_name);
            return directory;
         }
      }
   }
}

#endif