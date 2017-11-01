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
// bits.h
//
//////////////////////////////////////////////////
//
// Bot definitions header
//
#ifndef __RCBOT_BITS_H__
#define __RCBOT_BITS_H__

#include "gannconst.h"

class CBits
{
public:
	CBits ( unsigned int iNumBits );
	CBits ( CBits *copyBits );

	void setBit ( int iBit, BOOL bSet );

	BOOL getBit ( int iBit );

	void freeMemory ();

	void load ( FILE *bfp );

	void save ( FILE *bfp );

	void setup ( int iNumBits );

	inline unsigned int numBits () { return m_iNumBits; }

	void copy ( CBits *otherBits );

	inline unsigned char *getBits () { return m_cBits; }

	void randomize ();

	void clear ();

	int size ();

private:
	unsigned int m_iNumBits;
	unsigned char *m_cBits;
	int m_iSize; // bits' mem size
};

#endif