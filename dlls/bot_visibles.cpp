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
 // bot_visible.cpp
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

#include "bot_visibles.h"
#include "bits.h"

CBotVisibles::CBotVisibles()
{
	m_iVisibles = new CBits(gpGlobals->maxEntities + 1);
	m_iIter = 0;
	m_iVisibleList.Clear();
}

bool CBotVisibles::isVisible(const int iIndex) const
{
	return m_iVisibles->getBit(iIndex);
}

void CBotVisibles::setVisible(const int iIndex, const bool bVisible)
{
	const bool bCurrentlyVisible = isVisible(iIndex);

	if (bCurrentlyVisible && !bVisible)
		m_iVisibleList.Remove(iIndex);
	else if (!bCurrentlyVisible && bVisible)
		m_iVisibleList.Add(iIndex);

	m_iVisibles->setBit(iIndex, bVisible);
}

void CBotVisibles::resetIter()
{
	m_iIter = 0;
}

void CBotVisibles::freeMemory()
{
	m_iVisibleList.Destroy();
	m_iVisibleList.Clear();
	m_iVisibles->freeMemory();
	delete m_iVisibles;
	m_iVisibles = nullptr;
}

edict_t* CBotVisibles::nextVisible()
{
	if (m_iIter < m_iVisibleList.Size())
	{
		const int iEntityIndex = m_iVisibleList.ReturnValueFromIndex(m_iIter);

		m_iIter++;

		// reliability checks
		if (!iEntityIndex || iEntityIndex > gpGlobals->maxEntities)
			m_iVisibleList.Clear();
		else
			return INDEXENT(iEntityIndex);
	}

	resetIter();

	return nullptr;
}