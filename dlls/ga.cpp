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
#include "enginecallback.h"
#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif
#include "bot.h"

#include "ga.h"

#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

#include <algorithm>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

const int CGA::g_iDefaultMaxPopSize = 16;
const float CGA::g_fCrossOverRate = 0.7f;
const float CGA::g_fMutateRate = 0.1f;
const float CGA::g_fMaxPerturbation = 0.3f;

////////////////////
// POPULATION
////////////////////
CPopulation::~CPopulation()
{
	freeMemory();
}

IIndividual* CPopulation::get(const unsigned iIndex) const
{
	return m_theIndividuals[iIndex];
}

IIndividual* CPopulation::getBestIndividual() const
{
	if (m_theIndividuals.empty())
	{
		return nullptr;
	}

	const std::vector<IIndividual*>::const_iterator best = std::max_element(
		m_theIndividuals.cbegin(), m_theIndividuals.cend(),
		[](const IIndividual* a, const IIndividual* b)
		{
			return a->getFitness() < b->getFitness();
		});

	return *best;
}

void CPopulation::add(IIndividual* individual)
{
	m_theIndividuals.emplace_back(individual);
}

void CPopulation::freeMemory()
{
	for (IIndividual* individual : m_theIndividuals)
	{
		delete individual;
	}

	m_theIndividuals.clear();
}

void CPopulation::clear()
{
	freeMemory();
}

ga_value CPopulation::totalFitness() const
{
	return std::accumulate(m_theIndividuals.cbegin(), m_theIndividuals.cend(), 0.0f,
		[](ga_value sum, const IIndividual* individual) {
			return sum + individual->getFitness();
		});
}

void CPopulation::save(std::FILE* bfp) const
{
	const unsigned iSize = m_theIndividuals.size();

	const CGenericHeader header = CGenericHeader(LEARNTYPE_POPULATION, m_ga->m_iMaxPopSize);

	header.write(bfp);

	std::fwrite(&iSize, sizeof(int), 1, bfp);

	for (IIndividual* const& individual : m_theIndividuals)
		individual->save(bfp);
}

void CPopulation::load(std::FILE* bfp, const int chromosize, const int type)
{
	unsigned iSize;

	if (std::feof(bfp))
		return;

	const CGenericHeader header = CGenericHeader(LEARNTYPE_POPULATION, m_ga->m_iMaxPopSize);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	std::fread(&iSize, sizeof(int), 1, bfp);

	IIndividual* pVals;

	m_theIndividuals.clear();

	for (unsigned i = 0; i < iSize; i++)
	{
		// reliability check
		if (std::feof(bfp))
			return;

		if (type == TYPE_BOTGAVALS)
			pVals = new CBotGAValues();
		else
			pVals = new CIntGAValues();

		pVals->load(bfp, chromosize);

		m_theIndividuals.emplace_back(pVals);
	}
}

ga_value CPopulation::bestFitness() const
{
	if (m_theIndividuals.empty())
	{
		return 0.0f;
	}

	const std::vector<IIndividual*>::const_iterator best = std::max_element(
		m_theIndividuals.cbegin(), m_theIndividuals.cend(),
		[](const IIndividual* a, const IIndividual* b)
		{
			return a->getFitness() < b->getFitness();
		});

	return (*best)->getFitness();
}

ga_value CPopulation::averageFitness() const
{
	if (m_theIndividuals.empty()) return 0.0f;
	return totalFitness() / static_cast<ga_value>(m_theIndividuals.size());
}

IIndividual* CPopulation::pick()
{
	//IIndividual *to_return = m_theIndividuals[RANDOM_LONG(0,m_theIndividuals.size()-1)];//m_theIndividuals.back();

	IIndividual* to_return = m_theIndividuals.back();

	m_theIndividuals.pop_back();

	return to_return;
}

ga_value CPopulation::diversity() const
{
	if (m_theIndividuals.size() < 2)
		return 0.0f;

	const ga_value avgFit = averageFitness();
	const ga_value bestFit = bestFitness();

	// If best fitness is 0 or very small, return high diversity
	if (bestFit < 0.0001f)
		return 1.0f;

	// Calculate coefficient of variation (normalized standard deviation)
	ga_value variance = 0.0f;
	for (const IIndividual* individual : m_theIndividuals)
	{
		const ga_value diff = individual->getFitness() - avgFit;
		variance += diff * diff;
	}
	variance /= static_cast<ga_value>(m_theIndividuals.size());

	// Return diversity as ratio of std dev to best fitness
	// Higher values = more diverse population
	const ga_value stdDev = std::sqrt(variance);
	return std::min(stdDev / bestFit, 1.0f);
}

////////////////////
// GENETIC ALGORITHM
////////////////////

CGA::CGA(ISelection* selectFunction) : m_theSelectFunction(selectFunction)
{
	m_iMaxPopSize = 0;
	m_iNumGenerations = 0;
	m_fPrevAvgFitness = 0.0f;
	m_iPopType = 0;

	m_bestIndividual = nullptr;
	m_thePopulation.clear();
	m_theNewPopulation.clear();
}

void CGA::addToPopulation(IIndividual* individual)
{
	m_thePopulation.add(individual);

	if (m_thePopulation.size() >= m_iMaxPopSize)
	{
		epoch();

		IIndividual* best = m_thePopulation.getBestIndividual();

		if (best && (!m_bestIndividual || m_bestIndividual->getFitness() < best->getFitness()))
		{
			if (m_bestIndividual && m_bestIndividual != best)
			{
				delete m_bestIndividual;
				m_bestIndividual = nullptr;
			}

			m_bestIndividual = best->copy();
		}

		m_thePopulation.freeMemory();

		if (m_bestIndividual)
			m_thePopulation.add(m_bestIndividual->copy());
	}
}

void CGA::loadTeam(const char* szName, const int iTeam, const int chromosize)
{
	std::FILE* bfp = RCBOpenFile(szName, "rb", SAVETYPE_TEAM, iTeam);

	if (bfp)
	{
		load(bfp, chromosize);
		std::fclose(bfp);
	}
}

void CGA::saveTeam(const char* szName, const int iTeam) const
{
	std::FILE* bfp = RCBOpenFile(szName, "wb", SAVETYPE_TEAM, iTeam);

	if (bfp)
	{
		save(bfp);
		std::fclose(bfp);
	}
}
/*
void CGA :: loadBotGA ( char *szName, int iProfileId )
{
	FILE *bfp = RCBOpenFile(szName,"rb",SAVETYPE_BOT,iProfileId);

	if ( bfp )
	{
		load(bfp, chromosize);
		std::fclose(bfp);
	}
}

void CGA :: saveBotGA ( char *szName, int iProfileId )
{
	FILE *bfp = RCBOpenFile(szName,"wb",SAVETYPE_BOT,iProfileId);

	if ( bfp )
	{
		save(bfp);
		std::fclose(bfp);
	}
}
*/

void CGA::save(std::FILE* bfp) const
{
	m_thePopulation.save(bfp);
	m_theNewPopulation.save(bfp);
}

void CGA::load(std::FILE* bfp, const int chromosize)
{
	m_thePopulation.load(bfp, chromosize, m_iPopType);
	m_theNewPopulation.load(bfp, chromosize, m_iPopType);
}

void CGA::epoch()
{
	m_theNewPopulation.freeMemory();

	constexpr int ELITE_COUNT = 2;
	std::vector<IIndividual*> elite;

	for (int i = 0; i < ELITE_COUNT && i < static_cast<int>(m_thePopulation.size()); i++)
	{
		IIndividual* best = m_thePopulation.getBestIndividual();

		if (best)
		{
			elite.push_back(best->copy());
		}
	}

	// Add elite to new population unchanged (no mutation)
	for (IIndividual* e : elite)
	{
		m_theNewPopulation.add(e);
	}

	// Get adaptive mutation rate for this generation
	const float currentMutateRate = getAdaptiveMutateRate();

	while (m_theNewPopulation.size() < m_iMaxPopSize)
	{
		IIndividual* mum = m_theSelectFunction->select(&m_thePopulation);
		IIndividual* dad = m_theSelectFunction->select(&m_thePopulation);

		IIndividual* baby1 = mum->copy();
		IIndividual* baby2 = dad->copy();

		if (RANDOM_FLOAT(0, 1) < g_fCrossOverRate)
			baby1->crossOver(baby2);

		// Use adaptive mutation rate
		baby1->mutateWithRate(currentMutateRate);
		baby2->mutateWithRate(currentMutateRate);

		m_theNewPopulation.add(baby1);
		m_theNewPopulation.add(baby2);
	}

	m_fPrevAvgFitness = m_thePopulation.averageFitness();
	m_iNumGenerations++;

	/*
	BotMessage(NULL,0,"-----GENERATION %d------",m_iNumGenerations);
	BotMessage(NULL,0,"Adaptive Mutation Rate: %0.4f", currentMutateRate);
	BotMessage(NULL,0,"Population Diversity: %0.4f", m_thePopulation.diversity());
	BotMessage(NULL,0,"Best fitness : %0.4f",m_thePopulation.bestFitness());
	BotMessage(NULL,0,"Average fitness : %0.4f",m_fPrevAvgFitness);
	BotMessage(NULL,0,"------------------------");
	*/
}

void CGA::freeLocalMemory()
{
	m_thePopulation.freeMemory();
	m_theNewPopulation.freeMemory();
	m_iNumGenerations = 0;
	//	if (m_bestIndividual)
	delete m_bestIndividual;
	m_bestIndividual = nullptr;
}

void CGA::freeGlobalMemory()
{
	freeLocalMemory();
	//	if (m_theSelectFunction)
	delete m_theSelectFunction;
	m_theSelectFunction = nullptr;
}

bool CGA::canPick() const
{
	return m_theNewPopulation.size() > 0;
}

IIndividual* CGA::pick()
{
	return m_theNewPopulation.pick();
}

float CGA::getAdaptiveMutateRate() const
{
	if (!m_bUseAdaptiveMutation)
		return g_fMutateRate;

	const ga_value avgFitness = m_thePopulation.averageFitness();
	const ga_value bestFitness = m_thePopulation.bestFitness();

	// If no valid fitness data, use default rate
	if (bestFitness <= 0.0f)
		return g_fMutateRate;

	// Calculate population diversity (how spread out the fitness values are)
	const ga_value diversity = m_thePopulation.diversity();

	// When population converges (low diversity), increase mutation to explore more
	// When population is diverse, decrease mutation to exploit good solutions
	// Base rate is g_fMutateRate (0.1), can range from 0.05 to 0.3

	constexpr float MIN_MUTATION = 0.05f;
	constexpr float MAX_MUTATION = 0.30f;

	// Invert diversity: low diversity = high mutation need
	const float mutationNeed = 1.0f - diversity;

	// Scale mutation rate based on diversity
	float adaptiveRate = MIN_MUTATION + (MAX_MUTATION - MIN_MUTATION) * mutationNeed;

	// Also consider generational progress - early generations need more exploration
	if (m_iNumGenerations < 10)
	{
		adaptiveRate = std::max(adaptiveRate, 0.15f);
	}

	return adaptiveRate;
}

///////////////
// SELECTION
///////////////

IIndividual* CRouletteSelection::select(CPopulation* population)
{
	const ga_value fFitnessSlice = RANDOM_FLOAT(0, population->totalFitness());
	ga_value fFitnessSoFar = 0.0f;

	for (unsigned i = 0; i < population->size(); i++)
	{
		IIndividual* individual = population->get(i);

		fFitnessSoFar += individual->getFitness();

		if (fFitnessSoFar >= fFitnessSlice)
			return individual;
	}

	return population->get(0);
}

///////////////
// SAVING
// TODO: not functional and may need to be redesigned to support team and bot saves properly - [APG]RoboCop[CL]
std::FILE* RCBOpenFile(const char* file, const char* readtype, const eGASaveType savedtype, const int iId)
{
	char filename[256];
	char tmpfilename[256];
	char dirpath[256];

	if (savedtype == SAVETYPE_BOT)
		snprintf(tmpfilename, sizeof(tmpfilename), "%dp%s.rce", iId, file); // iId = profileid
	else if (savedtype == SAVETYPE_TEAM)
		snprintf(tmpfilename, sizeof(tmpfilename), "%dt%s.rce", iId, file); // iId = team id
	else
	{
		BotMessage(nullptr, 0, "RCBOpenFile: Invalid save type %d", savedtype);
		return nullptr;
	}

	// Ensure the profiles directory exists before attempting to write
	UTIL_BuildFileName(dirpath, BOT_PROFILES_FOLDER, nullptr);

#ifdef _WIN32
	_mkdir(dirpath);
#else
	mkdir(dirpath, 0755);
#endif

	UTIL_BuildFileName(filename, BOT_PROFILES_FOLDER, tmpfilename);

	std::FILE* bfp = std::fopen(filename, readtype);

	if (!bfp && readtype[0] == 'w')
	{
		BotMessage(nullptr, 0, "RCBOpenFile: Failed to create %s", filename);
	}

	return bfp;
}