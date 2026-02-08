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
	if (m_theValues.empty())
	{
		return;
	}

	const unsigned iPoint = RANDOM_LONG(0, m_theValues.size() - 1);

	CBotGAValues* vother = static_cast<CBotGAValues*>(other);

	for (unsigned i = 0; i < iPoint; i++)
	{
		std::swap(m_theValues[i], vother->m_theValues[i]);
	}
}

// mutate some values
void CBotGAValues::mutate()
{
	for (unsigned i = 0; i < m_theValues.size(); i++)
	{
		if (RANDOM_FLOAT(0, 1) < CGA::g_fMutateRate)
		{
			const float fCurrentVal = get(i);

			set(i, fCurrentVal + fCurrentVal * (-1 + RANDOM_FLOAT(0, 2)) * CGA::g_fMaxPerturbation);
		}
	}
}

// Replace the inline stub with a proper implementation
void CBotGAValues::mutateWithRate(const float rate)
{
	for (unsigned i = 0; i < m_theValues.size(); i++)
	{
		if (RANDOM_FLOAT(0, 1) < rate)
		{
			const float fCurrentVal = get(i);

			const float perturbScale = rate / CGA::g_fMutateRate;
			const float perturbation = CGA::g_fMaxPerturbation * perturbScale;

			set(i, fCurrentVal + fCurrentVal * (-1 + RANDOM_FLOAT(0, 2)) * perturbation);
		}
	}
}

float CBotGAValues::get(const unsigned iIndex) const
{
	assert(iIndex < m_theValues.size());
	return m_theValues[iIndex];
}

void CBotGAValues::set(const unsigned iIndex, const ga_value fVal)
{
	assert(iIndex < m_theValues.size());
	m_theValues[iIndex] = fVal;
}

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
	m_theValues = values;
}

void CBotGAValues::getVector(std::vector<ga_value>& values) const // Use reference instead of pointer
{
	values = m_theValues;
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
	const unsigned iSize = m_theValues.size();

	const CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, m_theValues.size());

	header.write(bfp);

	std::fwrite(&iSize, sizeof(unsigned), 1, bfp);

	if (iSize > 0)
	{
		std::fwrite(m_theValues.data(), sizeof(ga_value), iSize, bfp);
	}

	std::fwrite(&m_fFitness, sizeof(ga_value), 1, bfp);
}

void CBotGAValues::load(std::FILE* bfp, const int req_size)
{
	unsigned iSize;

	if (std::feof(bfp))
		return;

	const CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, req_size);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	m_theValues.clear();

	if (std::fread(&iSize, sizeof(unsigned), 1, bfp) != 1)
	{
		BotMessage(nullptr, 0, "Learn data error - could not read size");
		return;
	}


	if (iSize > 0)
	{
		m_theValues.resize(iSize);
		if (std::fread(m_theValues.data(), sizeof(ga_value), iSize, bfp) != iSize)
		{
			BotMessage(nullptr, 0, "Learn data error - unexpected end of file");
			m_theValues.clear();
			return;
		}
	}

	if (std::fread(&m_fFitness, sizeof(ga_value), 1, bfp) != 1)
	{
		BotMessage(nullptr, 0, "Learn data error - could not read fitness");
	}
}
//----------------------
// bits
//----------------------

unsigned CBitsGAValues::size() const
{
	return m_theBits->numBits();
}

void CBitsGAValues::convert(unsigned* iBits) const
{
	for (unsigned i = 0; i < size(); i++)
	{
		if (m_theBits->getBit(i))
			*iBits = *iBits + (1 << i);
	}
}

CBitsGAValues::CBitsGAValues(const unsigned iNumBits)
{
	m_theBits = new CBits(iNumBits);
}

void CBitsGAValues::load(std::FILE* bfp, const int req_size)
{
	// TODO : header
	const CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, req_size);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}
	m_theBits->load(bfp);
	if (std::fread(&m_fFitness, sizeof(ga_value), 1, bfp) != 1)
	{
		BotMessage(nullptr, 0, "Learn data error - could not read fitness");
	}
}

void CBitsGAValues::save(std::FILE* bfp)
{
	// TODO : header
	const CGenericHeader header = CGenericHeader(LEARNTYPE_GAVALUES, m_theBits->numBits());
	header.write(bfp);
	m_theBits->save(bfp);
	std::fwrite(&m_fFitness, sizeof(ga_value), 1, bfp);
}

//void loadForBot ( char *file, int iProfile );
//void saveForBot ( char *file, int iProfile );

CBitsGAValues::CBitsGAValues(CBits* bits)
{
	m_theBits = bits;
}

// crossover with other individual
void CBitsGAValues::crossOver(IIndividual* other)
{
	CBitsGAValues* otherBits = static_cast<CBitsGAValues*>(other);

	const int iNumBits = m_theBits->numBits();
	if (iNumBits == 0)
	{
		return;
	}
	const int iCrossoverPoint = RANDOM_LONG(0, iNumBits - 1);

	// For performance, we can swap raw bytes up to the crossover point's byte boundary,
	// then handle the remaining bits in the crossover byte, and finally swap the rest of the bytes [APG]RoboCop[CL]
	const int cross_byte = iCrossoverPoint / 8;
	const int cross_bit = iCrossoverPoint % 8;

	unsigned char* p1 = m_theBits->getBits();
	unsigned char* p2 = otherBits->m_theBits->getBits();

	// Swap whole bytes before the crossover point
	for (int i = 0; i < cross_byte; i++)
	{
		std::swap(p1[i], p2[i]);
	}

	// Swap the bits within the crossover byte
	if (cross_bit > 0)
	{
		const unsigned char mask1 = (1 << cross_bit) - 1;
		const unsigned char mask2 = ~mask1;
		const unsigned char b1 = p1[cross_byte];
		const unsigned char b2 = p2[cross_byte];
		p1[cross_byte] = (b1 & mask2) | (b2 & mask1);
		p2[cross_byte] = (b2 & mask2) | (b1 & mask1);
	}

	// Swap the remaining whole bytes
	for (unsigned i = cross_byte + 1; i < m_theBits->size(); i++)
	{
		std::swap(p1[i], p2[i]);
	}
}

// mutate some values
void CBitsGAValues::mutate()
{
	for (unsigned i = 0; i < m_theBits->numBits(); i++)
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
	CBits* newBits = new CBits(*m_theBits);
	IIndividual* individual = new CBitsGAValues(newBits);
	individual->setFitness(getFitness());
	return individual;
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