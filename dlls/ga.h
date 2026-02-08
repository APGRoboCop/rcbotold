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
#ifndef __GA_H__
#define __GA_H__

#include "gannconst.h"
#include <memory>
#include <vector>

typedef enum : std::uint8_t
{
	SAVETYPE_BOT,
	SAVETYPE_TEAM,
	SAVETYPE_MAX
}eGASaveType;

class IIndividual
{
public:
	virtual ~IIndividual() = default;
	// get fitness for this individual
	ga_value getFitness() const { return m_fFitness; }
	void setFitness(const float fVal) { m_fFitness = fVal; }

	virtual void load(std::FILE* bfp, int req_size) = 0;
	virtual void save(std::FILE* bfp) = 0;

	// crossover with other individual
	virtual void crossOver(IIndividual* other) = 0;

	// mutate some values
	virtual void mutate() = 0;

	// mutate with adaptive rate
	virtual void mutateWithRate(float rate) { mutate(); }  // Default falls back to standard mutate

	virtual void clear() = 0;

	// get new copy of this
	// sub classes return their class with own values
	virtual IIndividual* copy() = 0;
protected:
	ga_value m_fFitness = 0.0f;
};

class CGA;

class CPopulation
{
public:
	~CPopulation();

	void freeMemory();

	void setGA(CGA* ga) { m_ga = ga; }
	// size of population
	unsigned size() const { return m_theIndividuals.size(); }

	// get from population index
	IIndividual* get(unsigned iIndex) const;

	// add individual to population
	void add(IIndividual* individual);

	void clear();

	IIndividual* getBestIndividual() const;

	ga_value totalFitness() const;

	ga_value bestFitness() const;

	ga_value averageFitness() const;

	// Calculate population diversity (0 = converged, 1 = diverse)
	ga_value diversity() const;

	void load(std::FILE* bfp, int chromosize, int type = TYPE_BOTGAVALS);

	void save(std::FILE* bfp) const;

	// returns individual
	IIndividual* pick();

private:
	std::vector<IIndividual*> m_theIndividuals;
	CGA* m_ga = nullptr;
};

// selection function interface
class ISelection
{
public:
	virtual ~ISelection() = default;
	virtual IIndividual* select(CPopulation* population) = 0;
};

class CRouletteSelection : public ISelection
{
	IIndividual* select(CPopulation* population) override;
};

class CTournamentSelection : public ISelection
{
public:
	explicit CTournamentSelection(const int tournamentSize = 3)
		: m_iTournamentSize(tournamentSize) {
	}

	IIndividual* select(CPopulation* population) override
	{
		IIndividual* best = nullptr;

		for (int i = 0; i < m_iTournamentSize; i++)
		{
			const unsigned idx = RANDOM_LONG(0, population->size() - 1);
			IIndividual* candidate = population->get(idx);

			if (!best || candidate->getFitness() > best->getFitness())
				best = candidate;
		}
		return best;
	}
private:
	int m_iTournamentSize;
};

class CGA
{
public:

	CGA(const int iMaxPopSize = 0)
	{
		m_theSelectFunction = new CRouletteSelection();

		m_thePopulation.setGA(this);
		m_theNewPopulation.setGA(this);

		m_iNumGenerations = 0;
		m_fPrevAvgFitness = 0.0f;
		m_bUseAdaptiveMutation = true;  // Enable by default

		m_iMaxPopSize = iMaxPopSize;
		m_bestIndividual = nullptr;
		m_iPopType = 0;

		if (m_iMaxPopSize == 0)
			m_iMaxPopSize = g_iDefaultMaxPopSize;
	}

	// give GA a custom selection function
	CGA(ISelection* selectFunction);

	void freeLocalMemory();
	void freeGlobalMemory();

	// make new generation
	void epoch();

	void setSize(const int iSize) { m_iMaxPopSize = iSize; }

	void addToPopulation(IIndividual* individual);

	// can get an individual off new population
	bool canPick() const;

	IIndividual* pick();

	void load(std::FILE* bfp, int chromosize);

	void save(std::FILE* bfp) const;

	void loadTeam(const char* szName, int iTeam, int chromosize);
	void saveTeam(const char* szName, int iTeam) const;

	//void loadBotGA ( char *szName, int iProfileId );
	//void saveBotGA ( char *szName, int iProfileId );

	void setPopType(const int t) { m_iPopType = t; }

	// Adaptive mutation rate control
	void setAdaptiveMutation(bool enable) { m_bUseAdaptiveMutation = enable; }
	bool isAdaptiveMutationEnabled() const { return m_bUseAdaptiveMutation; }

	// Get the current adaptive mutation rate based on population state
	float getAdaptiveMutateRate() const;

	// Get generation count
	unsigned getNumGenerations() const { return m_iNumGenerations; }

	unsigned m_iMaxPopSize;
	static const int g_iDefaultMaxPopSize;
	static const float g_fCrossOverRate;
	static const float g_fMutateRate;
	static const float g_fMaxPerturbation;

private:

	CPopulation m_thePopulation;
	CPopulation m_theNewPopulation;

	unsigned m_iNumGenerations;
	float m_fPrevAvgFitness;
	int m_iPopType;
	bool m_bUseAdaptiveMutation;

	ISelection* m_theSelectFunction;

	// keep bst individual in population
	IIndividual* m_bestIndividual;
};

std::FILE* RCBOpenFile(const char* file, const char* readtype, eGASaveType savedtype, int iId);

#endif