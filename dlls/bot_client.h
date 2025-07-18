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
 // bot_client.h
 //
 //////////////////////////////////////////////////
 //
 // Prototypes for network message functions
 //

#ifndef __BOT_CLIENT_H__
#define __BOT_CLIENT_H__

#include "generic_class.h"
#include "bot_const.h"

// net message write types
typedef enum : std::uint8_t
{
	NET_MSG_TYPE_NONE = 0,
	NET_MSG_TYPE_COORD,
	NET_MSG_TYPE_BYTE,
	NET_MSG_TYPE_STRING,
	NET_MSG_TYPE_ENTITY,
	NET_MSG_TYPE_LONG,
	NET_MSG_TYPE_CHAR,
	NET_MSG_TYPE_SHORT,
	NET_MSG_TYPE_ANGLE
}eNetMessageTypes;

// make functions with same paramaters to make more client message functions

/////////////////////////////////////////////////////
// NETWORK MESSAGES

// collection of valid network messages
// catchable by the bot

class CBotNetMessage
{
public:
	virtual ~CBotNetMessage() = default;

	CBotNetMessage()
	{
		m_iModId = 0;
		m_iSize = 0;
		m_iMessage = 0;
		m_szMessageName = nullptr;
		//m_fpMsgFunction = NULL;
		m_bAllowHumans = false;
	}

	virtual bool isStateMsg() { return false; }

	bool humansAllowed() const
	{
		return m_bAllowHumans;
	}

	CBotNetMessage(/*void (*fpBotFunction)(void *, int),*/const char* szMessageName, int iModId, bool bAllowHumans = false);

	bool MessageForMod(const int iModId) const
	{
		return m_iModId == MOD_ANY || iModId == m_iModId;
	}

	virtual void execute(void* p, int iIndex)
	{
		return;
	}

	/*void *GetFunction ( void )
	{
		return ( (void*)m_fpMsgFunction );
	}*/

	/*void CallFunction ( void *p, int i )
	{
		//assert(m_fpMsgFunction != NULL);

		// may allow null functions for now
		if ( m_fpMsgFunction )
			(*m_fpMsgFunction)(p,i);
	}*/

	const char* getMessageName() const
	{
		return m_szMessageName;
	}

	int MessageNum() const
	{
		return m_iMessage;
	}

	bool IsMessage(int iMessage);

	bool IsMessageName(const char* szMessage) const
	{
		return std::strcmp(m_szMessageName, szMessage) == 0;
	}

	void UpdateMsgId(const int iMessage)
	{
		m_iMessage = iMessage;
	}

	void UpdateMessageSize(const int iSize)
	{
		m_iSize = iSize;
	}

	void setName(const char* szName);

	void setMod(const int iModId)
	{
		m_iModId = iModId;
	}

	void allowHumans()
	{
		m_bAllowHumans = true;
	}

private:

	int m_iMessage;
	char* m_szMessageName;
	int m_iModId;
	int m_iSize;

	bool m_bAllowHumans;

	//void (*m_fpMsgFunction)(void *, int);
};

/////////////////////////////////////
class CBotNetMessages
{
public:

	void AddNetMessage(CBotNetMessage* pMessage)
	{
		m_NetMessages.Push(pMessage);
	}

	void execute(int iMessage, void* p, int i) const;

	//void *GetFunction ( int iMessage );

	CBotNetMessage* GetMessage(int iMessage, const char* szName) const;

	void UpdateMessage(const char* szMessageName, const int iMessage, const int iSize) const
	{
		if (CBotNetMessage* l_Msg = GetMessage(iMessage, szMessageName))
		{
			l_Msg->UpdateMsgId(iMessage);
			l_Msg->UpdateMessageSize(iSize);
		}
	}

	void Destroy()
	{
		m_NetMessages.Destroy();
	}

private:
	dataStack<CBotNetMessage*> m_NetMessages;
};

///////////////////////
class BotClient_TS_ClipInfo : public CBotNetMessage
{
public:
	BotClient_TS_ClipInfo()
	{
		setName("ClipInfo");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_DelObj : public CBotNetMessage
{
public:
	BotClient_TS_DelObj()
	{
		setName("DelObj");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_WStatus : public CBotNetMessage
{
public:
	BotClient_TS_WStatus()
	{
		setName("WStatus");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_WeaponInfo : public CBotNetMessage
{
public:
	BotClient_TS_WeaponInfo()
	{
		setName("WeaponInfo");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_WeaponList : public CBotNetMessage
{
public:
	BotClient_Generic_WeaponList()
	{
		setName("WeaponList");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_CurrentWeapon : public CBotNetMessage
{
public:
	BotClient_Generic_CurrentWeapon()
	{
		setName("CurWeapon");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_AmmoX : public CBotNetMessage
{
public:
	BotClient_Generic_AmmoX()
	{
		setName("AmmoX");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_AmmoPickup : public CBotNetMessage
{
public:
	BotClient_Generic_AmmoPickup()
	{
		setName("AmmoPickup");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_WeaponPickup : public CBotNetMessage
{
public:
	BotClient_Generic_WeaponPickup()
	{
		setName("WeapPickup");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_ItemPickup : public CBotNetMessage
{
public:
	BotClient_Generic_ItemPickup()
	{
		setName("ItemPickup");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_Health : public CBotNetMessage
{
public:
	BotClient_Generic_Health()
	{
		setName("Health");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_Battery : public CBotNetMessage
{
public:
	BotClient_Generic_Battery()
	{
		setName("Battery");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_Damage : public CBotNetMessage
{
public:
	BotClient_Generic_Damage()
	{
		setName("Damage");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_TeamInfo : public CBotNetMessage
{
public:
	BotClient_TS_TeamInfo()
	{
		setName("TeamInfo");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_ScreenFade : public CBotNetMessage
{
public:
	BotClient_Generic_ScreenFade()
	{
		setName("ScreenFade");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};
// DMC Items
class BotClient_DMC_QuakeItem : public CBotNetMessage
{
public:
	BotClient_DMC_QuakeItem()
	{
		setName("QItems");
		setMod(MOD_DMC);
	}
	void execute(void* p, int iIndex) override; // override base function
};
// get vgui message
class BotClient_Generic_VGUIMenu : public CBotNetMessage
{
public:
	BotClient_Generic_VGUIMenu()
	{
		setName("VGUIMenu");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};
// ns 3.0 tech upgrades??
class BotClient_NS_TechSlots : public CBotNetMessage
{
public:
	BotClient_NS_TechSlots()
	{
		setName("TechSlots");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};
// need to catch message to show a menu
class BotClient_Generic_ShowMenu : public CBotNetMessage
{
public:
	BotClient_Generic_ShowMenu()
	{
		setName("ShowMenu");
		setMod(MOD_ANY);
	}
	//void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_PTakeDam : public CBotNetMessage
{
public:
	BotClient_TS_PTakeDam()
	{
		setName("PTakeDam");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_Health : public CBotNetMessage
{
public:
	BotClient_TS_Health()
	{
		setName("TSHealth");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_State : public CBotNetMessage
{
public:
	BotClient_TS_State()
	{
		setName("TSState");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_Frags : public CBotNetMessage
{
public:
	BotClient_TS_Frags()
	{
		setName("TSFrags");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_Objective : public CBotNetMessage
{
public:
	BotClient_TS_Objective()
	{
		setName("Objective");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_TSMessage : public CBotNetMessage
{
public:
	BotClient_TS_TSMessage()
	{
		setName("TSMessage");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_TS_RoundTime : public CBotNetMessage
{
public:
	BotClient_TS_RoundTime()
	{
		setName("RoundTime");
		setMod(MOD_TS);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_TeamScore : public CBotNetMessage
{
public:
	BotClient_Generic_TeamScore()
	{
		setName("TeamScore");
		setMod(MOD_ANY);
	}

	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_NS_Commndr : public CBotNetMessage
{
public:
	BotClient_NS_Commndr()
	{
		setName("Commndr");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_NS_HudText : public CBotNetMessage
{
public:
	BotClient_NS_HudText()
	{
		setName("HudText");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_NS_SetOrder : public CBotNetMessage
{
public:
	BotClient_NS_SetOrder()
	{
		setName("SetOrder");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_NS_SetTech : public CBotNetMessage
{
public:
	BotClient_NS_SetTech()
	{
		setName("SetTech");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_NS_AlienInfo : public CBotNetMessage
{
public:
	BotClient_NS_AlienInfo()
	{
		setName("AlienInfo");
		setMod(MOD_NS);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_BG_MakeMessage : public CBotNetMessage
{
public:
	BotClient_BG_MakeMessage()
	{
		setName("MakeMessage");
		setMod(MOD_BG);
	}
	void execute(void* p, int iIndex) override; // override base function
};

//TODO: Science and Industry - Experimental [APG]RoboCop[CL]
class BotClient_SI_TeamCash : public CBotNetMessage
{
public:
	BotClient_SI_TeamCash()
	{
		setName("TeamCash");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_SI_SciCount : public CBotNetMessage
{
public:
	BotClient_SI_SciCount()
	{
		setName("SciCount");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_SI_CarryInfo : public CBotNetMessage
{
public:
	BotClient_SI_CarryInfo()
	{
		setName("CarryInfo");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_SI_Goal : public CBotNetMessage
{
public:
	BotClient_SI_Goal()
	{
		setName("Goal");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_SI_Notice : public CBotNetMessage
{
public:
	BotClient_SI_Notice()
	{
		setName("Notice");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_SI_VoteInfo : public CBotNetMessage
{
public:
	BotClient_SI_VoteInfo()
	{
		setName("VoteInfo");
		setMod(MOD_SI);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_ScreenShake : public CBotNetMessage
{
public:
	BotClient_Generic_ScreenShake()
	{
		setName("ScreenShake");
		setMod(MOD_ANY);
	}
	//void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_TextMessage : public CBotNetMessage
{
public:
	BotClient_Generic_TextMessage()
	{
		setName("TextMsg");
		setMod(MOD_ANY);
		allowHumans();
	}
	void execute(void* p, int iIndex) override; // override base function
};
// say to team etc.?
class BotClient_Generic_SayText : public CBotNetMessage
{
public:
	BotClient_Generic_SayText()
	{
		setName("SayText");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

//////////////////////////
// MESSAGES to all players
class BotClient_All_Generic_SayText : public CBotNetMessage
{
public:
	BotClient_All_Generic_SayText()
	{
		setName("SayText");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};

class BotClient_Generic_DeathMessage : public CBotNetMessage
{
public:
	BotClient_Generic_DeathMessage()
	{
		setName("DeathMsg");
		setMod(MOD_ANY);
	}
	void execute(void* p, int iIndex) override; // override base function
};
void SetupNetMessages();

class CBotStatedNetMessage : public CBotNetMessage
{
public:
	CBotStatedNetMessage();

	void init(const int index) { iState = 0; m_pEdict = nullptr; if (index) { m_pEdict = INDEXENT(index); } msg_init(); }

	virtual void writeChar(const char c) { write_char(c); iState++; }
	virtual void writeByte(const int b) { write_byte(b); iState++; }
	virtual void writeShort(const int s) { write_short(s); iState++; }
	virtual void writeLong(const int l) { write_long(l); iState++; }
	virtual void writeString(const char* s) { write_string(s); iState++; }
	virtual void writeCoord(const float f) { write_coord(f); iState++; }
	virtual void writeEntity(edict_t* p) { write_entity(p); iState++; }
	virtual void writeAngle(const float f) { write_angle(f); iState++; }

	virtual void messageEnd() { iState = 0; message_end(); }

	bool isStateMsg() override { return true; }

private:
	virtual void msg_init() {}

	virtual void write_char(char c) {}
	virtual void write_byte(int b) {}
	virtual void write_short(int s) {}
	virtual void write_long(int l) {}
	virtual void write_string(const char* s) {}
	virtual void write_coord(float p) {}
	virtual void write_entity(edict_t* p) {}
	virtual void write_angle(float f) {}
	virtual void message_end() {}

protected:
	edict_t* m_pEdict; // player receiving message
	int iState;
};

#include <vector>

class CSetOrderMessage : public CBotStatedNetMessage
{
public:
	void msg_init() override
	{
		m_ReceivedPlayers.clear();
		m_iOrderType = static_cast<AvHOrderType>(0);
		m_iOrderTargetType = static_cast<AvHOrderTargetType>(0);
		iOrderStatus = 0;
		iReceivers = 0;
		iEntityUser3 = 0;
	}

	void write_short(int s) override
	{
		if (m_ReceivedPlayers.empty())
			m_ReceivedPlayers.emplace_back(INDEXENT(s));
		else if (!m_iOrderType)
			m_iOrderType = static_cast<AvHOrderType>(s);
		//else if ( m_iOrderType == )
	}

	void write_byte(int b) override
	{
	}

	void write_coord(float p) override
	{
	}

	void message_end() override
	{
	}
private:
	std::vector<edict_t*> m_ReceivedPlayers;
	Vector m_vOrderOrigin;
	AvHOrderType m_iOrderType;
	AvHOrderTargetType m_iOrderTargetType;
	int iOrderStatus = 0;
	int iReceivers = 0;
	int iEntityUser3 = 0;
};

#endif
