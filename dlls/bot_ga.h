/*
 *    This file is part of RCBot.
 *
 *    Copyright (c) 2006 Paul Murphy 'Cheeseh'
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

#ifndef __BOT_GA_H__
#define __BOT_GA_H__

#include "ga.h"
#include "gannconst.h"

class CBits;

class CBotGAValues : public IIndividual
{
public:
	CBotGAValues();

	void load ( FILE *bfp, int req_size );
	void save ( FILE *bfp );

	//void loadForBot ( char *file, int iProfile );
	//void saveForBot ( char *file, int iProfile );

	//~CBotGAValues();

	CBotGAValues( vector<ga_value> values );

	// crossover with other individual
	void crossOver ( IIndividual *other );

	// mutate some values
	void mutate ();

	// get new copy of this
	// sub classes return their class with own values
	IIndividual *copy ();

	void setVector ( vector<ga_value> values );
	void getVector ( vector<ga_value> *values );

	float get ( int iIndex );

	void set ( int iIndex, ga_value fVal );

	void clear ();

	inline void add ( ga_value val ) { m_theValues.push_back(val); }

	inline vector<ga_value> *returnVector () { return &m_theValues; }

	void freeMemory ();

private:
	vector<ga_value> m_theValues;
};

/////////////////////////////////////////////////

class CIntGAValues : public IIndividual
{
public:

	CIntGAValues ( int val = 0 )
	{
		m_Value = val;
	}

	void load ( FILE *bfp, int req_size )
	{
		int check;

		fread(&check,sizeof(int),1,bfp);

		if ( check == 1 )
			fread(&m_Value,sizeof(int),1,bfp);
		else
		{
			fread(&check,sizeof(int),1,bfp);
			m_Value = RANDOM_LONG(0,4294967295);
		}
	}

	void save ( FILE *bfp )
	{
		int iSiz = 1;
		fwrite(&iSiz,sizeof(int),1,bfp);
		fwrite(&m_Value,sizeof(int),1,bfp);
	}

	// crossover with other individual
	void crossOver ( IIndividual *other )
	{
		CIntGAValues *p = (CIntGAValues*)other;

		int iOther = p->get();

		// unform/mixed crossover

		for ( unsigned i = 0; i < 32; i ++ )
		{
			if ( RANDOM_FLOAT(0,1) >= 0.5f )
			{
				m_Value |= (iOther & (1<<i));
				p->set(iOther |= (m_Value&(1<<i)));
			}
		}
	}

	// mutate some values
	void mutate () 
	{ 
		for ( unsigned i = 0; i < 32; i ++ )
		{
			if ( RANDOM_FLOAT(0,1) <= 0.1f )	
			{
				if ( m_Value & (1<<i) )
					m_Value &= ~(1<<i);
				else
					m_Value |= (i<<i);
			}
		}
	}

	void clear () 
	{ 
		m_Value = 0;
	};

	// get new copy of this
	// sub classes return their class with own values
	IIndividual *copy ()
	{
		IIndividual *p = new CIntGAValues(m_Value);
		p->setFitness(getFitness());
		return p;
	}

	inline int get () { return m_Value;}
	inline void set ( int value ) { m_Value = value; }
private:
	int m_Value;
};

class CBitsGAValues : public IIndividual
{
public:
	CBitsGAValues ( unsigned int iNumBits );

	void load ( FILE *bfp, int req_size );
	void save ( FILE *bfp );

	//void loadForBot ( char *file, int iProfile );
	//void saveForBot ( char *file, int iProfile );

	//~CBotGAValues();
	void convert ( int *iBits );
	CBitsGAValues( CBits *bits );

	// crossover with other individual
	void crossOver ( IIndividual *other );

	// mutate some values
	void mutate ();

	// get new copy of this
	// sub classes return their class with own values
	IIndividual *copy ();

	unsigned int size ();

	//void setBits ( CBits values );
	//void getBits ( CBits *values );

	BOOL get ( int iIndex );

	void set ( int iIndex, BOOL bSet );

	void clear ();

	inline void random ();

	inline CBits *returnBits () { return m_theBits; }

	void freeMemory ();

private:
	CBits *m_theBits;
};

#endif