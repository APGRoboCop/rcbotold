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
	CBits(unsigned iNumBits);
	CBits(const CBits& other);
	CBits& operator=(const CBits& other);
	CBits(CBits&& other) noexcept;
	CBits& operator=(CBits&& other) noexcept;
	~CBits();

	void setBit(unsigned iBit, bool bSet) const;

	bool getBit(unsigned iBit) const;

	void freeMemory();

	void load(std::FILE* bfp);

	void save(std::FILE* bfp) const;

	void setup(unsigned iNumBits);

	unsigned numBits() const { return m_iNumBits; }

	void copy(const CBits& otherBits);

	unsigned char* getBits() const { return m_cBits; }

	void randomize() const;

	void clear() const;

	unsigned size() const;

private:
	unsigned m_iNumBits;
	unsigned char* m_cBits;
	//int m_iSize; // bits' mem size
};

#endif