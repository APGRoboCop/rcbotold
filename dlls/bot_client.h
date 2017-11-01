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
typedef enum
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
	
	CBotNetMessage()
	{
		m_iMessage = 0;
		m_szMessageName = NULL;
		//m_fpMsgFunction = NULL;
		m_bAllowHumans = FALSE;
	}

	virtual BOOL isStateMsg () { return FALSE; }

	BOOL humansAllowed ()
	{
		return m_bAllowHumans;
	}

	CBotNetMessage(/*void (*fpBotFunction)(void *, int),*/const char *szMessageName, int iModId, BOOL bAllowHumans = FALSE );

	BOOL MessageForMod ( int iModId )
	{
		return ((m_iModId == MOD_ANY) || (iModId == m_iModId));
	}

	virtual void execute ( void *p, int iIndex )
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


	const char *getMessageName ()
	{
		return m_szMessageName;
	}

	int MessageNum ( void )
	{
		return m_iMessage;
	}

	BOOL IsMessage ( const int iMessage );

	BOOL IsMessageName ( const char *szMessage )
	{
		return (strcmp(m_szMessageName,szMessage) == 0);
	}
	
	void UpdateMsgId ( int iMessage )
	{
		m_iMessage = iMessage;
	}

	void UpdateMessageSize ( int iSize )
	{
		m_iSize = iSize;
	}

	void setName ( char *szName );

	void setMod ( int iModId )
	{
		m_iModId = iModId;
	}

	void allowHumans ()
	{
		m_bAllowHumans = TRUE;
	}

private:

	int m_iMessage;
	char *m_szMessageName;
	int m_iModId;
	int m_iSize;

	BOOL m_bAllowHumans;

	//void (*m_fpMsgFunction)(void *, int);
};
	

/////////////////////////////////////
class CBotNetMessages
{
public:

	void AddNetMessage (CBotNetMessage *pMessage)
	{
		m_NetMessages.Push(pMessage);
	}

	void execute ( int iMessage, void *p, int i );

	//void *GetFunction ( int iMessage );

	CBotNetMessage *GetMessage ( int iMessage, const char *szName );

	void UpdateMessage ( const char *szMessageName, int iMessage, int iSize )
	{
		CBotNetMessage *l_Msg = GetMessage(iMessage,szMessageName);
		
		if ( l_Msg )
		{
			l_Msg->UpdateMsgId(iMessage);
			l_Msg->UpdateMessageSize(iSize);
		}
	}

	void Destroy ( void )
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

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_DelObj : public CBotNetMessage
{
public:
	BotClient_TS_DelObj()
	{
		setName("DelObj");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};


class BotClient_TS_WStatus : public CBotNetMessage
{
public:
	BotClient_TS_WStatus()
	{
		setName("WStatus");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_WeaponInfo : public CBotNetMessage
{
public:
	BotClient_TS_WeaponInfo()
	{
		setName("WeaponInfo");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_WeaponList : public CBotNetMessage
{
public:
	BotClient_Generic_WeaponList()
	{
		setName("WeaponList");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_CurrentWeapon : public CBotNetMessage
{
public:
	BotClient_Generic_CurrentWeapon()
	{
		setName("CurWeapon");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_AmmoX : public CBotNetMessage
{
public:
	BotClient_Generic_AmmoX()
	{
		setName("AmmoX");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_AmmoPickup : public CBotNetMessage
{
public:
	BotClient_Generic_AmmoPickup()
	{
		setName("AmmoPickup");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_WeaponPickup : public CBotNetMessage
{
public:
	BotClient_Generic_WeaponPickup()
	{
		setName("WeapPickup");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_ItemPickup : public CBotNetMessage
{
public:
	BotClient_Generic_ItemPickup()
	{
		setName("ItemPickup");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_Health : public CBotNetMessage
{
public:
	BotClient_Generic_Health()
	{
		setName("Health");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_Battery : public CBotNetMessage
{
public:
	BotClient_Generic_Battery()
	{
		setName("Battery");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_Damage : public CBotNetMessage
{
public:
	BotClient_Generic_Damage()
	{
		setName("Damage");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_TeamInfo : public CBotNetMessage
{
public:
	BotClient_TS_TeamInfo()
	{
		setName("TeamInfo");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_ScreenFade : public CBotNetMessage
{
public:
	BotClient_Generic_ScreenFade()
	{
		setName("ScreenFade");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};
	// DMC Items
class BotClient_DMC_QuakeItem : public CBotNetMessage
{
public:
	BotClient_DMC_QuakeItem	()
	{
		setName("QItems");
		setMod(MOD_DMC);
	}
	void execute ( void *p, int iIndex); // override base function
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
	void execute ( void *p, int iIndex); // override base function
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
	void execute ( void *p, int iIndex); // override base function
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

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_Health : public CBotNetMessage
{
public:
	BotClient_TS_Health()
	{
		setName("TSHealth");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_State : public CBotNetMessage
{
public:
	BotClient_TS_State()
	{
		setName("TSState");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_Frags : public CBotNetMessage
{
public:
	BotClient_TS_Frags()
	{
		setName("TSFrags");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_Objective : public CBotNetMessage
{
public:
	BotClient_TS_Objective()
	{
		setName("Objective");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_TSMessage : public CBotNetMessage
{
public:
	BotClient_TS_TSMessage()
	{
		setName("TSMessage");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_TS_RoundTime : public CBotNetMessage
{
public:
	BotClient_TS_RoundTime()
	{
		setName("RoundTime");
		setMod(MOD_TS);
	}

	void execute ( void *p, int iIndex); // override base function
};




class BotClient_Generic_TeamScore : public CBotNetMessage
{
public:
	BotClient_Generic_TeamScore()
	{
		setName("TeamScore");
		setMod(MOD_ANY);
	}

	void execute ( void *p, int iIndex); // override base function
};

class BotClient_NS_Commndr : public CBotNetMessage
{
public:
	BotClient_NS_Commndr()
	{
		setName("Commndr");
		setMod(MOD_NS);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_NS_HudText : public CBotNetMessage
{
public:
	BotClient_NS_HudText()
	{
		setName("HudText");
		setMod(MOD_NS);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_NS_SetOrder : public CBotNetMessage
{
public:
	BotClient_NS_SetOrder()
	{
		setName("SetOrder");
		setMod(MOD_NS);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_NS_SetTech : public CBotNetMessage
{
public:
	BotClient_NS_SetTech()
	{
		setName("SetTech");
		setMod(MOD_NS);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_NS_AlienInfo : public CBotNetMessage
{
public:
	BotClient_NS_AlienInfo()
	{
		setName("AlienInfo");
		setMod(MOD_NS);
	}
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_BG_MakeMessage : public CBotNetMessage
{
public:
	BotClient_BG_MakeMessage()
	{
		setName("MakeMessage");
		setMod(MOD_BG);
	}
	void execute ( void *p, int iIndex); // override base function
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
	void execute ( void *p, int iIndex); // override base function
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
	void execute ( void *p, int iIndex); // override base function
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
	void execute ( void *p, int iIndex); // override base function
};

class BotClient_Generic_DeathMessage : public CBotNetMessage
{
public:
	BotClient_Generic_DeathMessage()
	{
		setName("DeathMsg");
		setMod(MOD_ANY);
	}
	void execute ( void *p, int iIndex); // override base function
};	
void SetupNetMessages ( void );

class CBotStatedNetMessage : public CBotNetMessage 
{
public:
	CBotStatedNetMessage();

	void init (int index) { iState = 0; m_pEdict = NULL; if ( index ) { m_pEdict = INDEXENT(index); } msg_init(); }

	virtual void writeChar ( char c ) { write_char(c); iState++; };
	virtual void writeByte ( int b ) { write_byte(b); iState++; };
	virtual void writeShort ( int s ) {write_short(s); iState++;};
	virtual void writeLong ( int l ) {write_long(l); iState++;};
	virtual void writeString ( const char *s ) {write_string(s); iState++;};
	virtual void writeCoord ( float f ) {write_coord(f); iState++;};
	virtual void writeEntity ( edict_t *p ) {write_entity(p); iState++;};
	virtual void writeAngle ( float f ) {write_angle(f); iState++;};

	virtual void messageEnd () { iState = 0; message_end(); };

	BOOL isStateMsg () { return TRUE; }

private:
	virtual void msg_init () {};

	virtual void write_char ( char c ) {};
	virtual void write_byte ( int b ) {};
	virtual void write_short ( int s ) {};
	virtual void write_long ( int l ) {};
	virtual void write_string ( const char *s ) {};
	virtual void write_coord ( float p ) {};
	virtual void write_entity ( edict_t *p ) {};
	virtual void write_angle ( float f ) {};
	virtual void message_end () {};

protected:
	edict_t *m_pEdict; // player receiving message	
	int iState;
};

#include <vector>
using namespace std;

class CSetOrderMessage : public CBotStatedNetMessage
{
public:
	void msg_init ()
	{
		m_ReceivedPlayers.clear();
		m_iOrderType = (AvHOrderType)0;
		m_iOrderTargetType = (AvHOrderTargetType)0;
		iOrderStatus = 0;
		iReceivers = 0;
		iEntityUser3 = 0;
	}

	void write_short ( int s )
	{
		if ( !m_ReceivedPlayers.size() )		
			m_ReceivedPlayers.push_back(INDEXENT(s));
		else if ( !m_iOrderType )		
			m_iOrderType = (AvHOrderType)s;
		//else if ( m_iOrderType == )
	}

	void write_byte ( int b )
	{

	}

	void write_coord ( float p )
	{

	}

	void message_end ()
	{
		
	}
private:
	vector <edict_t *> m_ReceivedPlayers;
	Vector m_vOrderOrigin;
	AvHOrderType m_iOrderType;
	AvHOrderTargetType m_iOrderTargetType;
	int iOrderStatus;
	int iReceivers;
	int	iEntityUser3;
};

#endif
