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
 //////////////////////////////////////////////////
 // RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
 //
 // (http://www.rcbot.net)
 //
 // Based on botman's High Ping Bastard bot
 //
 // (http://planethalflife.com/botman/)
 //
 // netmessages.cpp
 //
 //////////////////////////////////////////////////
 //
 // Class for catching network messages
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

#include "bot_client.h"

extern CBotGlobals gBotGlobals;

//////////////////////////////////////////////////////////////////////////////
// NET MESSAGE HANDLING

void CBotNetMessage::setName(const char* szName)
{
	m_szMessageName = gBotGlobals.m_Strings.GetString(szName);
}

bool CBotNetMessage::IsMessage(const int iMessage)
{
#ifdef RCBOT_META_BUILD
	//////////////////////
	// FOR META MOD ONLY
	if (m_iMessage == 0)
	{
		m_iMessage = GET_USER_MSG_ID(PLID, m_szMessageName, &m_iSize);
	}
#endif

	return m_iMessage == iMessage;
}

void CBotNetMessages::execute(const int iMessage, void* p, const int i) const
{
	if (CBotNetMessage* pMsg = GetMessage(iMessage, nullptr))
		pMsg->execute(p, i);
}

CBotNetMessage* CBotNetMessages::GetMessage(const int iMessage, const char* szName) const
{
	dataStack<CBotNetMessage*> l_tempStack = m_NetMessages;

	while (!l_tempStack.IsEmpty())
	{
		// Cheat here, use the stack
		CBotNetMessage* l_currMsg = l_tempStack.ChooseFromStack();

		if (!l_currMsg->MessageForMod(MOD_ANY))
		{
			if (!l_currMsg->MessageForMod(gBotGlobals.m_iCurrentMod))
			{
				continue;
			}
		}

		if (szName == nullptr)
		{
			if (!l_currMsg->IsMessage(iMessage))
			{
				continue;
			}
		}
		else
		{
			if (!l_currMsg->IsMessageName(szName))
			{
				continue;
			}
		}

		l_tempStack.Init(); // Fool the pointer so the program wont free up memory

		return l_currMsg;
	}

	return nullptr;
}

CBotNetMessage::CBotNetMessage(const char* szMessageName, const int iModId, const bool bAllowHumans)
{
	m_iSize = 0;
	m_iMessage = 0;
	//	m_fpMsgFunction = fpBotFunction;
	m_iModId = iModId;

	m_bAllowHumans = bAllowHumans;

	m_szMessageName = gBotGlobals.m_Strings.GetString(szMessageName);
}