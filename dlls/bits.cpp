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
// bits.cpp
//
//////////////////////////////////////////////////
//
// Bot definitions header
//
#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot.h"

#include "bits.h"
#include "gannconst.h"

CBits :: CBits ( unsigned int iNumBits )
{
	setup(iNumBits);
}

CBits :: CBits ( CBits *copyBits )
{
	copy(copyBits);
}

void CBits :: freeMemory ()
{
	delete m_cBits;
	m_cBits = NULL;
	m_iNumBits = 0;
}

void CBits :: setBit ( int iBit, BOOL bSet )
{
	int iBitStart = iBit/8;
	int iBitOffset = iBit%8;

	unsigned char *c = &m_cBits[iBitStart];

	if ( bSet )
		*c |= (1<<iBitOffset);
	else
		*c &= ~(1<<iBitOffset);
}

BOOL CBits :: getBit ( int iBit )
{
	int iBitStart = iBit/8;
	int iBitOffset = iBit%8;

	unsigned char *c = &m_cBits[iBitStart];

	return ((*c & (1<<iBitOffset)) == (1<<iBitOffset));
}

void CBits :: load ( FILE *bfp )
{
	CGenericHeader header = CGenericHeader(LEARNTYPE_BITS,(int)m_iNumBits);

	if ( !header.read(bfp,header) )
	{
		BotMessage(NULL,0,"Learn data version mismatch - wiping");
		return;
	}

	unsigned int iNumBits;

	fread(&iNumBits,sizeof(unsigned int),1,bfp);

	m_iNumBits = iNumBits;

	if ( m_cBits != NULL )
	{
		delete m_cBits;
		m_cBits=NULL;
	}

	setup(m_iNumBits);

	fread(&m_cBits,size(),1,bfp);
}

void CBits :: randomize ()
{
	for ( unsigned int i = 0; i < m_iNumBits; i ++ )
	{
		setBit(i,RANDOM_FLOAT(0.0,1.0)>=0.5);
	}
}

void CBits :: setup ( int iNumBits )
{
	int iSize; 

	m_iNumBits = iNumBits;

	iSize = size();

	m_cBits = new unsigned char [iSize];

	memset(m_cBits,0,iSize);
}

// memory size
int CBits :: size ()
{
	return Ceiling(((float)m_iNumBits)/8);
}

void CBits :: save ( FILE *bfp )
{
	CGenericHeader checkHeader = CGenericHeader(LEARNTYPE_BITS,(int)m_iNumBits);

	checkHeader.write(bfp);

	fwrite(&m_iNumBits,sizeof(unsigned int),1,bfp);
	
	fwrite(&m_cBits,size(),1,bfp);
}

void CBits :: copy ( CBits *otherBits )
{
	m_iNumBits = numBits();
	memcpy(m_cBits,otherBits->getBits(),size());
}

void CBits :: clear ()
{
	memset(m_cBits,0,size());
}