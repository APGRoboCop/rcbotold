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
 // bot_rep.cpp
 //
 //////////////////////////////////////////////////
 //
 // Bot reputation handling
 //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot_const.h"
#include "bot.h"

extern CBotGlobals gBotGlobals;

////////////////////////////////////////////////////////////
// REPUTATION HANDLING

int BotFunc_GetRepArrayNum(const int iRep)
// Return the array index for chat arrays for the right rep.
// 0 = enemy
// 1 = neutral
// 2 = friendly
{
	if (iRep == -1)
	{
		BugMessage(nullptr, "GetRepArrayNum: Bad rep recieved (%d)", iRep);
		return 1;
	}

	if (iRep < 3)
		return 0;
	else if (iRep >= 3 && iRep <= 6)
		return 1;

	return 2;
}

CBotReputation::CBotReputation(const int iPlayerRepId, const int iRep)
{
	m_iPlayerRepId = iPlayerRepId;

	m_iRep = iRep;
}

CClient* CBotReputations::GetRandomClient(const int iRep) const
// return a random client that conforms to the iRep (reputation)
// -1 will return a random bad client
// 0 : a random neutral client
// 1 : a random friendly client
{
	dataStack <CBotReputation> tempStack = m_RepList;
	CBotReputation* pRep;
	dataUnconstArray<CBotReputation*> iIdList;

	iIdList.Init();

	while (!tempStack.IsEmpty())
	{
		pRep = tempStack.ChoosePointerFromStack();

		const int iGotRep = pRep->CurrentRep();

		switch (iRep)
		{
		case -1: // bad
			if (iGotRep <= 3)
				iIdList.Add(pRep);
			break;
		case 0: // neutral
			if (iGotRep > 3 && iGotRep < 7)
				iIdList.Add(pRep);
			break;
		case 1: // good
			if (iGotRep >= 7)
				iIdList.Add(pRep);
			break;
		}
	}

	tempStack.Init();

	if (iIdList.IsEmpty())
		return nullptr;

	assert(iIdList.Size() > 0);

	pRep = iIdList.Random();

	iIdList.Clear();

	if (pRep == nullptr)
		return nullptr;

	return gBotGlobals.m_Clients.GetClientByRepId(pRep->GetRepId());
}

////////////////////////////////
// Saves ONE rep and removes it
void CBotReputations::RemoveSaveRep(const int iBotProfile, const int iPlayerRepId)
{
	CBotReputation* pRep = this->GetRep(iPlayerRepId);

	if (pRep)
	{
		WriteToFile(iBotProfile, pRep);
	}

	m_RepList.RemoveByPointer(pRep);
}
////////////////////////////////
// Saves ALL rep
void CBotReputations::SaveAllRep(const int iBotProfile)
{
	dataStack <CBotReputation> tempStack = m_RepList;

	while (!tempStack.IsEmpty())
	{
		if (CBotReputation* pRep = tempStack.ChoosePointerFromStack())
		{
			WriteToFile(iBotProfile, pRep);
		}
	}

	return;
}

void CBotReputations::AddLoadRep(const int iBotProfile, const int iPlayerRepId)
{
	char filename[256];
	char repfile[16];

	CBotReputation Rep;

	BOOL bFound = false;

	snprintf(repfile, sizeof(repfile), "%d.rcr", iBotProfile);

	UTIL_BuildFileName(filename, "botprofiles", repfile);

	std::FILE* fp = std::fopen(filename, "rb"); // open the file in ascii read/write mode

	if (fp == nullptr)
	{
		m_RepList.Push(CBotReputation(iPlayerRepId, BOT_MID_REP));

		WriteToFile(iBotProfile, m_RepList.GetHeadInfoPointer());

		return;
	}

	std::fseek(fp, 0, SEEK_END); // move pos to end of file
	const long fPos = std::ftell(fp);  // get length of file

	// do some error checking - verify the file is not corrupt
	if (fPos % sizeof(CBotReputation) != 0) return;

	// get the count of items in the file
	long   count = fPos / sizeof(CBotReputation);

	std::fseek(fp, 0, SEEK_SET); // move pos back to beginning

	while (bFound == false && !std::feof(fp) && count > 0)
	{
		std::fread(&Rep, sizeof(CBotReputation), 1, fp);

		// is for the player input into function (player id)?
		if (Rep.IsForPlayer(iPlayerRepId))
		{
			m_RepList.Push(Rep);
			bFound = true;
		}

		count--;
	}

	std::fclose(fp);

	if (bFound == false)
	{
		m_RepList.Push(CBotReputation(iPlayerRepId, BOT_MID_REP));

		// save new
		WriteToFile(iBotProfile, m_RepList.GetHeadInfoPointer());
	}

	//return;
}

void CBotReputations::WriteToFile(const int iBotProfile, CBotReputation* pRep)
{
	char filename[256];
	char repfile[16];

	CBotReputation Rep;

	BOOL bChanged = false;

	if (pRep == nullptr) // error
		return;

	snprintf(repfile, sizeof(repfile), "%d.rcr", iBotProfile);

	UTIL_BuildFileName(filename, "botprofiles", repfile);

	std::FILE* fp = std::fopen(filename, "rb+"); // open the file in ascii read/write mode

	if (fp == nullptr)
	{
		fp = std::fopen(filename, "wb");

		if (fp == nullptr)
		{
			BotMessage(nullptr, 0, "could not save Bot id %d's rep file", iBotProfile);
			return;
		}

		std::fwrite(pRep, sizeof(CBotReputation), 1, fp);
		std::fclose(fp);

		return;
	}

	std::fseek(fp, 0, SEEK_END); // move pos to end of file
	long   fPos = std::ftell(fp);  // get length of file

	// do some error checking - verify the file is not corrupt
	if (fPos % sizeof(CBotReputation) != 0) return;

	// get the count of items in the file
	long   count = fPos / sizeof(CBotReputation);

	std::fseek(fp, 0, SEEK_SET); // move pos back to beginning

	while (bChanged == false && !std::feof(fp) && count > 0)
	{
		std::fread(&Rep, sizeof(CBotReputation), 1, fp);

		if (Rep == *pRep)
		{
			fPos = std::ftell(fp);   // get the current position
			// move marker back to start of this record
			std::fseek(fp, 0 - sizeof(CBotReputation), SEEK_CUR);
			std::fwrite(pRep, sizeof(CBotReputation), 1, fp);
			fflush(fp);
			// now reset the marker to same as before we
			// deleted this record so that everything continues
			// as normal
			std::fseek(fp, fPos, SEEK_SET);

			bChanged = true;
		}
		count--;
	}
	std::fclose(fp);

	if (bChanged == false)
	{
		fp = std::fopen(filename, "ab"); // append binary

		if (fp == nullptr)
		{
			BotMessage(nullptr, 0, "Can't open reputation file \"%s\" for appending", filename);
			return; // error
		}
		else
		{
			std::fwrite(pRep, sizeof(CBotReputation), 1, fp);

			std::fclose(fp);
		}
	}
}

int GetPlayerEdictRepId(edict_t* pEdict)
// return rep id of pEdict
{
	if (pEdict == nullptr)
		return -1;

	if (const CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pEdict))
		return pClient->GetPlayerRepId();

	return -1;
}

int GetPlayerRepId(const char* szPlayerName)
// each player is uniquely identified by an integer id
// held in the playersid.ini file. theire id is basically
// the line number that their name is on in the .ini file.
{
	if (szPlayerName == nullptr)
		return -1;
	if (*szPlayerName == 0)
		return -1;

	char filename[512];
	UTIL_BuildFileName(filename, BOT_PLAYER_ID_FILE);

	std::FILE* fp = std::fopen(filename, "r");

	if (fp == nullptr)
	{
		fp = std::fopen(filename, "w");

		if (fp != nullptr)
		{
			std::fprintf(fp, "%s", BOT_PLAYER_ID_FILE_HEADER);
			std::fprintf(fp, "\"%s\"\n", szPlayerName);

			std::fclose(fp);

			return 0;
		}
		else
		{
			BugMessage(nullptr, "Cannot open/write to bot player id file");

			return -1;
		}
	}
	else
	{
		char buffer[128];
		char playername[64];
		int iId = 0;

		while (std::fgets(buffer, 127, fp))
		{
			if (buffer[0] == '#') // comment
				continue;

			size_t length = std::strlen(buffer);

			if (length == 0)
				continue; // nothing on this line

			if (buffer[length - 1] == '\n')
			{
				length--;
				buffer[length] = 0;
			}

			unsigned int i = 0;

			while (i < length && buffer[i] != '"')
				i++;

			i++;

			int iPlayerChar = 0;

			while (i < length && buffer[i] != '"' && iPlayerChar < 64)
				playername[iPlayerChar++] = buffer[i++];

			if (iPlayerChar < 64)
				playername[iPlayerChar] = 0;

			//std::sscanf(buffer,"\"%s\"",playername);

			if (std::strcmp(playername, szPlayerName) == 0)
			{
				std::fclose(fp);
				return iId;
			}

			iId++;
		}

		std::fclose(fp);

		fp = std::fopen(filename, "a");

		if (fp != nullptr)
		{
			std::fprintf(fp, "\"%s\"\n", szPlayerName);
			std::fclose(fp);

			return iId;
		}
	}

	// error
	return -1;
}

int CBotReputations::GetClientRep(CClient* pClient) const
{
	if (pClient == nullptr)
	{
		BotMessage(nullptr, 0, "warning: GetClientRep(): Bad pClient recieved (NULL) (returning default)");
		return BOT_MID_REP;
	}

	const int iRepId = pClient->GetPlayerRepId();

	if (iRepId == -1)
		return BOT_MID_REP;

	const CBotReputation* pRep = this->GetRep(iRepId);

	if (pRep == nullptr)
	{
		BotMessage(nullptr, 0, "warning: GetClientRep(): No Rep for pClient (id: %d) (returning default)", pClient->GetPlayerRepId());
		return BOT_MID_REP;
	}

	return pRep->CurrentRep();
}