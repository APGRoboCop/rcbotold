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
#include "waypoint.h"

CBits::CBits(const unsigned iNumBits)
{
	setup(iNumBits);
}

CBits::CBits(const CBits& other)
{
	m_cBits = nullptr;
	copy(other);
}

CBits& CBits::operator=(const CBits& other)
{
	if (this != &other)
	{
		freeMemory();
		copy(other);
	}
	return *this;
}

CBits::CBits(CBits&& other) noexcept : m_iNumBits(other.m_iNumBits), m_cBits(other.m_cBits)
{
	other.m_iNumBits = 0;
	other.m_cBits = nullptr;
}

CBits& CBits::operator=(CBits&& other) noexcept
{
	if (this != &other)
	{
		freeMemory();
		m_iNumBits = other.m_iNumBits;
		m_cBits = other.m_cBits;
		other.m_iNumBits = 0;
		other.m_cBits = nullptr;
	}
	return *this;
}

CBits::~CBits()
{
	freeMemory();
}

void CBits::freeMemory()
{
	delete[] m_cBits;
	m_cBits = nullptr;
	m_iNumBits = 0;
}

void CBits::setBit(const unsigned iBit, const bool bSet) const
{
	const unsigned iBitStart = iBit / 8;
	const unsigned iBitOffset = iBit % 8;

	unsigned char* c = &m_cBits[iBitStart];

	if (bSet)
		*c |= 1 << iBitOffset;
	else
		*c &= ~(1 << iBitOffset);
}

bool CBits::getBit(const unsigned iBit) const
{
	const unsigned iBitStart = iBit / 8;
	const unsigned iBitOffset = iBit % 8;

	const unsigned char* c = &m_cBits[iBitStart];

	return (*c & 1 << iBitOffset) == 1 << iBitOffset;
}

void CBits::load(std::FILE* bfp)
{
	const CGenericHeader header = CGenericHeader(LEARNTYPE_BITS, m_iNumBits);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	// Read the number of bits
	unsigned iNumBits;

	std::fread(&iNumBits, sizeof(unsigned), 1, bfp);

	// Update member variables
	m_iNumBits = iNumBits;

	if (m_cBits != nullptr)
	{
		delete[] m_cBits; // Use delete[] for arrays
		m_cBits = nullptr;
	}

	setup(m_iNumBits);

	std::fread(m_cBits, size(), 1, bfp);
}

void CBits::randomize() const
{
	for (int i = 0; i < static_cast<int>(m_iNumBits); i++)
	{
		setBit(i, RANDOM_FLOAT(0.0f, 1.0f) >= 0.5f);
	}
}

void CBits::setup(const unsigned iNumBits)
{
	m_iNumBits = iNumBits;

	const unsigned iSize = size();

	m_cBits = new unsigned char[iSize];

	std::memset(m_cBits, 0, iSize);
}

// memory size
unsigned CBits::size() const
{
	return (m_iNumBits + 7) / 8;
}

void CBits::save(std::FILE* bfp) const
{
	const CGenericHeader checkHeader = CGenericHeader(LEARNTYPE_BITS, m_iNumBits);

	// Write the header
	checkHeader.write(bfp);

	std::fwrite(&m_iNumBits, sizeof(unsigned), 1, bfp);

	std::fwrite(m_cBits, size(), 1, bfp);
}

void CBits::copy(const CBits& otherBits)
{
	setup(otherBits.numBits());
	std::memcpy(m_cBits, otherBits.getBits(), size());
}

void CBits::clear() const
{
	std::memset(m_cBits, 0, size());
}