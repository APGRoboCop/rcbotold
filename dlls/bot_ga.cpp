// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif
#include "cbase.h" // Need CBASE for classify()
 //#include "player.h"
#include "bot_const.h"
#include "bot.h"
#include "bot_ga.h"

#include "bits.h"

CBotGAValues::CBotGAValues()
{
	CBotGAValues::clear();
	setFitness(0);
}

CBotGAValues::CBotGAValues(const std::vector<ga_value>& values)
{
	CBotGAValues::clear();
	setFitness(0);

	setVector(values);
}

void CBotGAValues::clear()
{
	m_theValues.clear();
}

// crossover with other individual
void CBotGAValues::crossOver(IIndividual* other)
{
	const unsigned int iPoint = RANDOM_LONG(0, m_theValues.size());

	CBotGAValues* vother = static_cast<CBotGAValues*>(other);

	for (unsigned int i = 0; i < iPoint; i++)
	{
		std::swap(m_theValues[i], vother->m_theValues[i]);
	}
}

// mutate some values
void CBotGAValues::mutate()
{
	for (unsigned int i = 0; i < m_theValues.size(); i++)
	{
		if (RANDOM_FLOAT(0, 1) < CGA::g_fMutateRate)
		{
			const float fCurrentVal = get(i);

			set(i, fCurrentVal + fCurrentVal * (-1 + RANDOM_FLOAT(0, 2)) * CGA::g_fMaxPerturbation);
		}
	}
}

float CBotGAValues::get(const unsigned int iIndex) const
{
	return m_theValues[iIndex];
}

void CBotGAValues::set(const unsigned int iIndex, const ga_value fVal)
{
	m_theValues[iIndex] = fVal;
}

//CBotGAValues :: ~CBotGAValues ()
//{/
	//m_theValues.clear();
//}

// get new copy of this
// sub classes return their class with own values
IIndividual* CBotGAValues::copy()
{
	IIndividual* individual = new CBotGAValues(m_theValues);

	individual->setFitness(getFitness());

	return individual;
}

void CBotGAValues::setVector(const std::vector<ga_value>& values)
{
	m_theValues.clear();
	m_theValues.reserve(values.size());
	for (const ga_value& value : values) // Use const reference for efficiency
		m_theValues.emplace_back(value);
}
void CBotGAValues::getVector(std::vector<ga_value>& values) const // Use reference instead of pointer
{
	values.clear();
	values.reserve(m_theValues.size());
	for (const ga_value& m_theValue : m_theValues) // Use const reference for efficiency
		values.emplace_back(m_theValue);
}

void CBotGAValues::freeMemory()
{
	m_theValues.clear();
}
/*
void CBotGAValues :: loadForBot (const char *file, const int iProfile, const int chromosize)
{
	FILE *bfp = RCBOpenFile(file,"rb",SAVETYPE_BOT,iProfile);

	if ( bfp )
	{
		load(bfp, chromosize);
		std::fclose(bfp);
	}
}

void CBotGAValues :: saveForBot (const char *file, const int iProfile)
{
	FILE *bfp = RCBOpenFile(file,"wb",SAVETYPE_BOT,iProfile);

	if ( bfp )
	{
		save(bfp);
		std::fclose(bfp);
	}
}*/

void CBotGAValues::save(std::FILE* bfp)
{
	const unsigned int iSize = m_theValues.size();

	const CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, m_theValues.size());

	header.write(bfp);

	std::fwrite(&iSize, sizeof(unsigned int), 1, bfp);

	for (unsigned int i = 0; i < iSize; i++)
	{
		std::fwrite(&m_theValues[i], sizeof(ga_value), 1, bfp);
	}

	std::fwrite(&m_fFitness, sizeof(ga_value), 1, bfp);
}

void CBotGAValues::load(std::FILE* bfp, const int req_size)
{
	unsigned int iSize;
	ga_value fRead;

	if (std::feof(bfp))
		return;

	CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, req_size);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	m_theValues.clear();

	std::fread(&iSize, sizeof(unsigned int), 1, bfp);

	for (unsigned int i = 0; i < iSize; i++)
	{
		// reliability check
		if (std::feof(bfp))
			return;

		std::fread(&fRead, sizeof(ga_value), 1, bfp);
		m_theValues.emplace_back(fRead);
	}

	std::fread(&m_fFitness, sizeof(ga_value), 1, bfp);
}
//----------------------
// bits
//----------------------

unsigned int CBitsGAValues::size() const
{
	return m_theBits->size();
}

void CBitsGAValues::convert(unsigned int* iBits) const
{
	for (unsigned int i = 0; i < size(); i++)
	{
		if (m_theBits->getBit(i))
			*iBits = *iBits + (1 << i);
	}
}

CBitsGAValues::CBitsGAValues(const unsigned int iNumBits)
{
	m_theBits = new CBits(iNumBits);
}

void CBitsGAValues::load(std::FILE* bfp, int req_size)
{
	// TODO : header
	m_theBits->load(bfp);
}

void CBitsGAValues::save(std::FILE* bfp)
{
	// TODO : header
	m_theBits->save(bfp);
}

//void loadForBot ( char *file, int iProfile );
//void saveForBot ( char *file, int iProfile );

//~CBotGAValues();

CBitsGAValues::CBitsGAValues(CBits* bits)
{
	//m_theBits = nullptr; //TODO: Not required? [APG]RoboCop[CL]
	//delete m_theBits;
	m_theBits = bits;
}

// crossover with other individual
void CBitsGAValues::crossOver(IIndividual* other)
{
	int i;

	const int iNumBits = m_theBits->numBits();
	const int iCrossoverPoint = RANDOM_LONG(0, iNumBits);
	const CBits tempBits(iNumBits);

	const CBitsGAValues* otherBits = static_cast<CBitsGAValues*>(other);

	for (i = 0; i < iCrossoverPoint; i++)
	{
		tempBits.setBit(i, m_theBits->getBit(i));
		m_theBits->setBit(i, otherBits->get(i));
		otherBits->set(i, tempBits.getBit(i));
	}

	for (i = iCrossoverPoint; i < iNumBits; i++)
	{
		tempBits.setBit(i, otherBits->get(i));
		otherBits->set(i, m_theBits->getBit(i));
		m_theBits->setBit(i, tempBits.getBit(i));
	}
}

// mutate some values
void CBitsGAValues::mutate()
{
	for (unsigned int i = 0; i < m_theBits->numBits(); i++)
	{
		if (RANDOM_FLOAT(0.0f, 1.0f) < CGA::g_fMutateRate)
		{
			m_theBits->setBit(i, !m_theBits->getBit(i));
		}
	}
}

// get new copy of this
// sub classes return their class with own values
IIndividual* CBitsGAValues::copy()
{
	CBits* newBits = new CBits(m_theBits);

	return new CBitsGAValues(newBits);
}

//void setBits ( CBits values );
//void getBits ( CBits *values );

bool CBitsGAValues::get(const int iIndex) const
{
	return m_theBits->getBit(iIndex);
}

void CBitsGAValues::set(const int iIndex, const bool bSet) const
{
	m_theBits->setBit(iIndex, bSet);
}

void CBitsGAValues::clear()
{
	m_theBits->clear();
}

inline void CBitsGAValues::random() const
{
	m_theBits->randomize();
}

void CBitsGAValues::freeMemory()
{
	delete m_theBits;
	m_theBits = nullptr;
}