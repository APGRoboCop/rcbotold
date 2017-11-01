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

int BotFunc_GetRepArrayNum ( int iRep )
// Return the array index for chat arrays for the right rep.
// 0 = enemy
// 1 = neutral
// 2 = friendly
{
	if ( iRep == -1 )
	{
		BugMessage(NULL,"GetRepArrayNum: Bad rep recieved (%d)",iRep);
		return 1;
	}

	if ( iRep < 3 )
		return 0;
	else if (( iRep >= 3 )&&( iRep <= 6 ))
		return 1;
	
	return 2;
}


CBotReputation :: CBotReputation( int iPlayerRepId, int iRep )
{
	m_iPlayerRepId = iPlayerRepId;
	
	m_iRep = iRep;
}

CClient *CBotReputations :: GetRandomClient ( int iRep )
// return a random client that conforms to the iRep (reputation)
// -1 will return a random bad client
// 0 : a random neutral client
// 1 : a random friendly client
{
	int iGotRep;

	dataStack <CBotReputation> tempStack = m_RepList;
	CBotReputation *pRep;
	dataUnconstArray<CBotReputation*> iIdList;

	iIdList.Init();

	while ( !tempStack.IsEmpty() )
	{
		pRep = tempStack.ChoosePointerFromStack();

		iGotRep = pRep->CurrentRep();
		
		switch ( iRep )
		{
		case -1: // bad
			if ( iGotRep <= 3 )
				iIdList.Add(pRep);
			break;
		case 0: // neutral
			if (( iGotRep > 3 ) && ( iGotRep < 7 ))
				iIdList.Add(pRep);
			break;
		case 1: // good
			if ( iGotRep >= 7 )
				iIdList.Add(pRep);
			break;
		}
	}

	tempStack.Init();

	if ( iIdList.IsEmpty() )
		return NULL;

	assert ( iIdList.Size() > 0 );

	pRep = iIdList.Random();

	iIdList.Clear();

	if ( pRep == NULL )
		return NULL;

	return gBotGlobals.m_Clients.GetClientByRepId(pRep->GetRepId());
}

////////////////////////////////
// Saves ONE rep and removes it
void CBotReputations :: RemoveSaveRep ( int iBotProfile, int iPlayerRepId )
{
	CBotReputation *pRep = this->GetRep(iPlayerRepId);

	if ( pRep )
	{
		WriteToFile(iBotProfile,pRep);
	}

	m_RepList.RemoveByPointer(pRep);
}
////////////////////////////////
// Saves ALL rep
void CBotReputations :: SaveAllRep ( int iBotProfile )
{
	dataStack <CBotReputation> tempStack = m_RepList;
	CBotReputation *pRep;

	while ( !tempStack.IsEmpty() )
	{
		pRep = tempStack.ChoosePointerFromStack();

		if ( pRep )
		{
			WriteToFile(iBotProfile,pRep);
		}
	}

	return;
}

void CBotReputations :: AddLoadRep ( int iBotProfile, int iPlayerRepId )
{
	FILE *fp;

	char filename[256];
	char repfile[16];

	CBotReputation Rep;

	BOOL bFound = FALSE;

	sprintf(repfile,"%d.rcr",iBotProfile);

	UTIL_BuildFileName(filename,"botprofiles",repfile);

	fp = fopen (filename, "rb"); // open the file in ascii read/write mode

	if ( fp == NULL )
	{
		m_RepList.Push(CBotReputation(iPlayerRepId,BOT_MID_REP));

		WriteToFile(iBotProfile,m_RepList.GetHeadInfoPointer());

		return;
	}

	fseek(fp,0,SEEK_END); // move pos to end of file
	long   fPos = ftell(fp);  // get length of file
	
	// do some error checking - verify the file is not corrupt
	if (fPos%sizeof(CBotReputation)!=0) return;
	
	// get the count of items in the file
	long   count = fPos/sizeof(CBotReputation);
	
	fseek(fp,0,SEEK_SET); // move pos back to beginning
	
	while ((bFound == FALSE) && !feof(fp) && count>0)
	{
		fread(&Rep,sizeof(CBotReputation),1,fp);

		// is for the player input into function (player id)?
		if ( Rep.IsForPlayer(iPlayerRepId) )
		{
			m_RepList.Push(Rep);
			bFound = TRUE;
		}
		
		count--;
	}

	fclose(fp);

	if ( bFound == FALSE )
	{
		m_RepList.Push(CBotReputation(iPlayerRepId,BOT_MID_REP));

		// save new
		WriteToFile(iBotProfile,m_RepList.GetHeadInfoPointer());
	}

	return;

}

void CBotReputations :: WriteToFile ( int iBotProfile, CBotReputation *pRep )
{
	FILE *fp;

	char filename[256];
	char repfile[16];

	CBotReputation Rep;

	BOOL bChanged = FALSE;

	if ( pRep == NULL ) // error
		return;

	sprintf(repfile,"%d.rcr",iBotProfile);

	UTIL_BuildFileName(filename,"botprofiles",repfile);

	fp = fopen (filename, "rb+"); // open the file in ascii read/write mode

	if ( fp == NULL )
	{
		fp = fopen (filename, "wb");

		if ( fp == NULL )
		{
			BotMessage(NULL,0,"could not save Bot id %d's rep file",iBotProfile);
			return;
		}
		
		fwrite(pRep,sizeof(CBotReputation),1,fp);
		fclose(fp);
		
		return;
	}	
	
	fseek(fp,0,SEEK_END); // move pos to end of file
	long   fPos = ftell(fp);  // get length of file
	
	// do some error checking - verify the file is not corrupt
	if (fPos%sizeof(CBotReputation)!=0) return;
	
	// get the count of items in the file
	long   count = fPos/sizeof(CBotReputation);
	
	fseek(fp,0,SEEK_SET); // move pos back to beginning
	
	while ((bChanged == FALSE) && !feof(fp) && count>0)
	{
		fread(&Rep,sizeof(CBotReputation),1,fp);

		if ( Rep == *pRep )
		{
			fPos = ftell(fp);   // get the current position
			// move marker back to start of this record
			fseek(fp,0-sizeof(CBotReputation),SEEK_CUR);
			fwrite(pRep,sizeof(CBotReputation),1,fp);
			fflush(fp);
			// now reset the marker to same as before we 
			// deleted this record so that everything continues
			// as normal
			fseek(fp,fPos,SEEK_SET);

			bChanged = TRUE;
		}
		count--;
	}
	fclose(fp);
	
	if ( bChanged == FALSE )
	{
		fp = fopen(filename,"ab"); // append binary

		if ( fp == NULL )
		{
			BotMessage(NULL,0,"Can't open reputation file \"%s\" for appending",filename);
			return; // error
		}
		else
		{
		   fwrite(pRep,sizeof(CBotReputation),1,fp);

		   fclose(fp);
		}
	}
}

int GetPlayerEdictRepId ( edict_t *pEdict )
// return rep id of pEdict
{
	CClient *pClient;
	
	if ( pEdict == NULL )
		return -1;
	
	pClient = gBotGlobals.m_Clients.GetClientByEdict(pEdict);

	if ( pClient )
		return pClient->GetPlayerRepId();

	return -1;
}

int GetPlayerRepId ( const char *szPlayerName )
// each player is uniquely identified by an integer id
// held in the playersid.ini file. theire id is basically
// the line number that their name is on in the .ini file.
{
	FILE *fp;

	if ( szPlayerName == NULL )
		return -1;
	if ( *szPlayerName == 0 )
		return -1;

	char filename[512];
	UTIL_BuildFileName(filename,BOT_PLAYER_ID_FILE);

	fp = fopen(filename,"r");
	
	if ( fp == NULL )
	{
		fp = fopen(filename,"w");
		
		if ( fp != NULL )
		{
			fprintf(fp,"%s",BOT_PLAYER_ID_FILE_HEADER);
			fprintf(fp,"\"%s\"\n",szPlayerName);
			
			fclose(fp);
			
			return 0;
		}
		else
		{
			BugMessage(NULL,"Cannot open/write to bot player id file");
			
			return -1;
		}
	}
	else
	{
		char buffer[128];
		char playername[64];
		int length;
		int iId = 0;
		int i;
		int iPlayerChar;
		
		while ( fgets(buffer,127,fp) )
		{
			if ( buffer[0] == '#' ) // comment
				continue;
			
			length = strlen(buffer);
			
			if ( length == 0 )
				continue; // nothing on this line

			if ( buffer[length-1] == '\n' )
			{
				length--;
				buffer[length] = 0;
			}

			i = 0;

			while ((i < length) && (buffer[i] != '"'))
				i++;

			i++;

			iPlayerChar = 0;

			while ((i < length) && (buffer[i] != '"') && (iPlayerChar < 64))
				playername[iPlayerChar++] = buffer[i++];

			if ( iPlayerChar < 64 )
				playername[iPlayerChar] = 0;

			//sscanf(buffer,"\"%s\"",playername);
			
			if ( strcmp(playername,szPlayerName) == 0 )
			{
				fclose(fp);
				return iId;
			}
			
			iId ++;
		}
		
		fclose(fp);
		
		fp = fopen(filename,"a");
		
		if ( fp != NULL )
		{
			fprintf(fp,"\"%s\"\n",szPlayerName);
			fclose(fp);
			
			return iId;
		}
	}
	
	// error
	return -1;
}

int CBotReputations ::GetClientRep(CClient *pClient)
{
	if ( pClient == NULL )
	{
		BotMessage(NULL,0,"warning: GetClientRep(): Bad pClient recieved (NULL) (returning default)");
		return BOT_MID_REP;
	}
	
	int iRepId = pClient->GetPlayerRepId();

	if ( iRepId == -1 )
		return BOT_MID_REP;

	CBotReputation *pRep = this->GetRep(iRepId);

	if ( pRep == NULL )
	{
		BotMessage(NULL,0,"warning: GetClientRep(): No Rep for pClient (id: %d) (returning default)",pClient->GetPlayerRepId());
		return BOT_MID_REP;
	}

	return pRep->CurrentRep();
}