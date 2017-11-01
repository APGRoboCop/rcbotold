/*
 *    This file is part of RCBot.
 *
 *    Copyright (c) 2005 Paul Murphy 'Cheeseh'
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

#ifndef __GANN_CONST_H__
#define __GANN_CONST_H__

#include <string.h>

#define TYPE_BOTGAVALS 0
#define TYPE_INTGAVALS 1

typedef float ga_value;

class ILearnInfo
{
public:
	virtual void load ( FILE *bfp, int req_size ) = 0;
	virtual void save ( FILE *bfp ) = 0;
};

class CGenericHeader
{
public:
	CGenericHeader()
	{
		m_szHeader[0] = 0;
	}

	CGenericHeader ( int iType, int iSize )
	{
		sprintf(m_szHeader,"t-%d-s-%d",iType,iSize);
	}

	const char *getHeader ()
	{
		return m_szHeader;		
	}

	BOOL operator == ( CGenericHeader other )
	{
		return (strcmp(m_szHeader,other.getHeader()) == 0);
	}

	void write ( FILE *bfp )
	{
		fwrite(this,sizeof(CGenericHeader),1,bfp);
	}

	BOOL read ( FILE *bfp, CGenericHeader compare )
	{
		CGenericHeader read;

		fread(&read,sizeof(CGenericHeader),1,bfp);

		return (read == compare);
	}

private:
	char m_szHeader[32];
};

enum
{
	LEARNTYPE_PERCEPTRON = 1,
	LEARNTYPE_POPULATION = 2,
	LEARNTYPE_GAVALUES = 3,
	LEARNTYPE_NN = 4,
	LEARNTYPE_NN_LAYER = 5,
	LEARNTYPE_BITS = 6,
	LEARNTYPE_GA = 7
};
#endif