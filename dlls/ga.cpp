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

#include <memory>
#include <stdexcept>

const int CGA::g_iDefaultMaxPopSize = 16;
const float CGA::g_fCrossOverRate = 0.7f;
const float CGA::g_fMutateRate = 0.1f;
const float CGA::g_fMaxPerturbation = 0.3f;

////////////////////
// POPULATION
////////////////////

IIndividual* CPopulation::get(const int iIndex) const
{
	return m_theIndividuals[iIndex];
}

IIndividual* CPopulation::getBestIndividual() const
{
	IIndividual* best = nullptr;

	for (IIndividual* curr : m_theIndividuals)
	{
		if (!best || curr->getFitness() > best->getFitness())
			best = curr;
	}

	return best;
}

void CPopulation::add(IIndividual* individual)
{
	m_theIndividuals.emplace_back(individual);
}

void CPopulation::freeMemory()
{
	for (IIndividual*& m_theIndividual : m_theIndividuals)
	{
		m_theIndividual->clear();
		delete m_theIndividual;
		m_theIndividual = nullptr;
	}

	m_theIndividuals.clear();
}

void CPopulation::clear()
{
	m_theIndividuals.clear();
}

ga_value CPopulation::totalFitness() const
{
	float fTotalFitness = 0.0f;

	for (unsigned int i = 0; i < size(); i++)
	{
		fTotalFitness += m_theIndividuals[i]->getFitness();
	}

	return fTotalFitness;
}

void CPopulation::save(std::FILE* bfp) const
{
	const unsigned int iSize = m_theIndividuals.size();

	const CGenericHeader header = CGenericHeader(LEARNTYPE_POPULATION, m_ga->m_iMaxPopSize);

	header.write(bfp);

	std::fwrite(&iSize, sizeof(int), 1, bfp);

	for (unsigned int i = 0; i < iSize; i++)
		m_theIndividuals[i]->save(bfp);
}

void CPopulation::load(std::FILE* bfp, const int chromosize, const int type)
{
	unsigned int iSize;

	if (std::feof(bfp))
		return;

	CGenericHeader header = CGenericHeader(LEARNTYPE_POPULATION, m_ga->m_iMaxPopSize);

	if (!CGenericHeader::read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	std::fread(&iSize, sizeof(int), 1, bfp);

	IIndividual* pVals;

	m_theIndividuals.clear();

	for (unsigned int i = 0; i < iSize; i++)
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
	bool gotBestFitness = false;
	float fBestFitness = 0.0f;

	for (unsigned int i = 0; i < size(); i++)
	{
		const float fFitness = m_theIndividuals[i]->getFitness();

		if (!gotBestFitness || fFitness > fBestFitness)
		{
			fBestFitness = fFitness;
			gotBestFitness = true;
		}
	}

	return fBestFitness;
}

ga_value CPopulation::averageFitness() const
{
	return totalFitness() / static_cast<ga_value>(m_theIndividuals.size());
}

IIndividual* CPopulation::pick()
{
	//IIndividual *to_return = m_theIndividuals[RANDOM_LONG(0,m_theIndividuals.size()-1)];//m_theIndividuals.back();

	IIndividual* to_return = m_theIndividuals.back();

	m_theIndividuals.pop_back();

	return to_return;
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

		if (best && !m_bestIndividual || best && m_bestIndividual->getFitness() < best->getFitness())
		{
			bool set = true;

			if (m_bestIndividual && m_bestIndividual != best)
			{
				delete m_bestIndividual;
				m_bestIndividual = nullptr;
			}
			else if (m_bestIndividual == best)
				set = false;

			if (set)
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
		load(bfp);
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

	while (m_theNewPopulation.size() < m_iMaxPopSize)
	{
		IIndividual* mum = m_theSelectFunction->select(&m_thePopulation);
		IIndividual* dad = m_theSelectFunction->select(&m_thePopulation);

		IIndividual* baby1 = mum->copy();
		IIndividual* baby2 = dad->copy();

		if (RANDOM_FLOAT(0, 1) < g_fCrossOverRate)
			baby1->crossOver(baby2);

		baby1->mutate();
		baby2->mutate();

		m_theNewPopulation.add(baby1);
		m_theNewPopulation.add(baby2);
	}

	m_iNumGenerations++;
	/*
		BotMessage(NULL,0,"-----GENERATION %d------",m_iNumGenerations);
		BotMessage(NULL,0,"Previous fitnesses :");
		BotMessage(NULL,0,"Best fitness : %0.4f",m_thePopulation.bestFitness());
		float fCurAvgFitness = m_thePopulation.averageFitness();
		BotMessage(NULL,0,"Average fitness : %0.4f",fCurAvgFitness);
		BotMessage(NULL,0,"Prev Average fitness = %0.4f",m_fPrevAvgFitness);
		if (fCurAvgFitness > m_fPrevAvgFitness )
			BotMessage(NULL,0,"Getting Better! :)");
		else
			BotMessage(NULL,0,"Getting Worse! :(");
		m_fPrevAvgFitness = fCurAvgFitness;
		BotMessage(NULL,0,"------------------------");*/
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

///////////////
// SELECTION
///////////////

IIndividual* CRouletteSelection::select(CPopulation* population)
{
	const ga_value fFitnessSlice = RANDOM_FLOAT(0, population->totalFitness());
	ga_value fFitnessSoFar = 0.0f;

	for (unsigned int i = 0; i < population->size(); i++)
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
//TODO: To allow the experience data to be saved properly [APG]RoboCop[CL]
std::FILE* RCBOpenFile(const char* file, const char* readtype, const eGASaveType savedtype, const int iId)
{
	char filename[256];
	char tmpfilename[256];

	if (savedtype == SAVETYPE_BOT)
		snprintf(tmpfilename, sizeof(tmpfilename), "%dp%s.rce", iId, file); // iId = profileid
	else if (savedtype == SAVETYPE_TEAM)
		snprintf(tmpfilename, sizeof(tmpfilename), "%dt%s.rce", iId, file); // iId = team id
	else
		throw std::invalid_argument("Invalid save type");

	UTIL_BuildFileName(filename, BOT_PROFILES_FOLDER, tmpfilename);

	std::FILE* bfp = std::fopen(filename, readtype);
	if (!bfp)
		throw std::runtime_error("Failed to open file");

	return bfp;
}