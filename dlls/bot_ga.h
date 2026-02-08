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

#pragma once

#include "ga.h"
#include "gannconst.h"
#include "nn.h"

#include <algorithm>

class CBits;

///////////////////////////////////////////////
// Combat Fitness Metrics
// Multi-objective fitness calculation for combat performance
///////////////////////////////////////////////
class CCombatFitnessMetrics
{
public:
	float damageDealt = 0.0f;
	float damageTaken = 0.0f;
	float accuracy = 0.0f;       // hits / shots fired (0-1)
	float survivalTime = 0.0f;   // seconds survived
	int kills = 0;
	int deaths = 0;
	int assists = 0;
	int objectivesCompleted = 0; // flags captured, objectives done, etc.

	// Weights for fitness calculation (can be tuned)
	static constexpr float WEIGHT_DAMAGE_RATIO = 0.25f;
	static constexpr float WEIGHT_KD_RATIO = 0.25f;
	static constexpr float WEIGHT_ACCURACY = 0.15f;
	static constexpr float WEIGHT_SURVIVAL = 0.15f;
	static constexpr float WEIGHT_OBJECTIVES = 0.20f;

	CCombatFitnessMetrics() = default;

	void reset()
	{
		damageDealt = 0.0f;
		damageTaken = 0.0f;
		accuracy = 0.0f;
		survivalTime = 0.0f;
		kills = 0;
		deaths = 0;
		assists = 0;
		objectivesCompleted = 0;
	}

	// Record a hit on enemy
	void recordHit(float damage)
	{
		damageDealt += damage;
	}

	// Record damage taken
	void recordDamageTaken(float damage)
	{
		damageTaken += damage;
	}

	// Record accuracy from shots
	void recordAccuracy(int hits, int shotsFired)
	{
		if (shotsFired > 0)
		{
			accuracy = static_cast<float>(hits) / static_cast<float>(shotsFired);
		}
	}

	// Record a kill
	void recordKill()
	{
		kills++;
	}

	// Record a death
	void recordDeath()
	{
		deaths++;
	}

	// Record an assist
	void recordAssist()
	{
		assists++;
	}

	// Record objective completion
	void recordObjective()
	{
		objectivesCompleted++;
	}

	// Update survival time
	void updateSurvivalTime(float deltaTime)
	{
		survivalTime += deltaTime;
	}

	// Calculate weighted multi-objective fitness score
	ga_value calculateFitness() const
	{
		// Damage ratio: how much damage dealt vs taken
		// Avoid division by zero, reward dealing damage even if none taken
		const ga_value damageRatio = (damageTaken > 0.0f)
			? std::min(damageDealt / damageTaken, 10.0f) / 10.0f  // Normalize to 0-1
			: (damageDealt > 0.0f ? 1.0f : 0.0f);

		// K/D ratio: kills vs deaths, with assists counting as partial kills
		const float effectiveKills = static_cast<float>(kills) + static_cast<float>(assists) * 0.5f;
		const ga_value kdRatio = (deaths > 0)
			? std::min(effectiveKills / static_cast<float>(deaths), 5.0f) / 5.0f  // Normalize to 0-1
			: (effectiveKills > 0.0f ? 1.0f : 0.0f);

		// Accuracy: already 0-1
		const ga_value accuracyScore = std::clamp(accuracy, 0.0f, 1.0f);

		// Survival time: normalize to 0-1 (assuming 300 seconds = 5 min is excellent)
		constexpr float MAX_SURVIVAL_TIME = 300.0f;
		const ga_value survivalScore = std::min(survivalTime / MAX_SURVIVAL_TIME, 1.0f);

		// Objectives: normalize (assuming 5 objectives is excellent)
		constexpr float MAX_OBJECTIVES = 5.0f;
		const ga_value objectiveScore = std::min(static_cast<float>(objectivesCompleted) / MAX_OBJECTIVES, 1.0f);

		// Calculate weighted fitness
		const ga_value fitness =
			(damageRatio * WEIGHT_DAMAGE_RATIO) +
			(kdRatio * WEIGHT_KD_RATIO) +
			(accuracyScore * WEIGHT_ACCURACY) +
			(survivalScore * WEIGHT_SURVIVAL) +
			(objectiveScore * WEIGHT_OBJECTIVES);

		return fitness;
	}

	// Get individual component scores for debugging/analysis
	void getComponentScores(ga_value& outDamage, ga_value& outKD, ga_value& outAcc,
		ga_value& outSurvival, ga_value& outObjective) const
	{
		outDamage = (damageTaken > 0.0f)
			? std::min(damageDealt / damageTaken, 10.0f) / 10.0f
			: (damageDealt > 0.0f ? 1.0f : 0.0f);

		const float effectiveKills = static_cast<float>(kills) + static_cast<float>(assists) * 0.5f;
		outKD = (deaths > 0)
			? std::min(effectiveKills / static_cast<float>(deaths), 5.0f) / 5.0f
			: (effectiveKills > 0.0f ? 1.0f : 0.0f);

		outAcc = std::clamp(accuracy, 0.0f, 1.0f);
		outSurvival = std::min(survivalTime / 300.0f, 1.0f);
		outObjective = std::min(static_cast<float>(objectivesCompleted) / 5.0f, 1.0f);
	}
};

class CBotGAValues : public IIndividual
{
public:
	CBotGAValues();

	void load(std::FILE* bfp, int req_size) override;
	void save(std::FILE* bfp) override;

	//void loadForBot ( char *file, int iProfile );
	//void saveForBot ( char *file, int iProfile );

	//~CBotGAValues();

	CBotGAValues(const std::vector<ga_value>& values);

	// crossover with other individual
	void crossOver(IIndividual* other) override;

	// mutate some values
	void mutate() override;

	// mutate with custom rate (for adaptive mutation)
	void mutateWithRate(float rate) override;

	// get new copy of this
	// sub classes return their class with own values
	IIndividual* copy() override;

	void setVector(const std::vector<ga_value>& values);
	void getVector(std::vector<ga_value>& values) const;

	float get(unsigned iIndex) const;
	void set(unsigned iIndex, ga_value fVal);

	void clear() override;
	void add(ga_value val) { m_theValues.emplace_back(val); }

	std::vector<ga_value>* returnVector() { return &m_theValues; }
	void freeMemory();

	//void loadForBot(const char* file, int iProfile, int chromosize);
	//void saveForBot(const char* file, int iProfile);

private:
	std::vector<ga_value> m_theValues;
};

/////////////////////////////////////////////////

class CIntGAValues : public IIndividual
{
public:

	CIntGAValues(int val = 0)
	{
		m_Value = val;
	}

	void load(std::FILE* bfp, int req_size) override
	{
		int check;

		std::fread(&check, sizeof(int), 1, bfp);

		if (check == req_size)
		{
			std::fread(&m_Value, sizeof(int), 1, bfp);
			std::fread(&m_fFitness, sizeof(ga_value), 1, bfp);
		}
	}

	void save(std::FILE* bfp) override
	{
		constexpr int check = sizeof(int);
		std::fwrite(&check, sizeof(int), 1, bfp);
		std::fwrite(&m_Value, sizeof(int), 1, bfp);
		std::fwrite(&m_fFitness, sizeof(ga_value), 1, bfp);
	}

	void crossOver(IIndividual* other) override
	{
		CIntGAValues* pOther = static_cast<CIntGAValues*>(other);
		const int iMask = RANDOM_LONG(0, 32);
		const int iTemp = m_Value;

		m_Value = (m_Value & iMask) | (pOther->m_Value & ~iMask);
		pOther->m_Value = (pOther->m_Value & iMask) | (iTemp & ~iMask);
	}

	void mutate() override
	{
		mutateWithRate(CGA::g_fMutateRate);
	}

	void mutateWithRate(const float rate) override
	{
		if (RANDOM_FLOAT(0, 1) < rate)
		{
			const int iBit = RANDOM_LONG(0, 31);
			m_Value ^= (1 << iBit);  // Flip a random bit
		}
	}

	void clear() override
	{
		m_Value = 0;
	}

	// get new copy of this
	// sub classes return their class with own values
	IIndividual* copy() override
	{
		CIntGAValues* pCopy = new CIntGAValues(m_Value);
		pCopy->setFitness(m_fFitness);
		return pCopy;
	}

	int get() const { return m_Value; }
	void set(int val) { m_Value = val; }

private:
	int m_Value;
};

/////////////////////////////////////////////////

class CBitsGAValues : public IIndividual
{
public:
	explicit CBitsGAValues(unsigned iNumBits);
	explicit CBitsGAValues(CBits* bits);  // ADD THIS MISSING CONSTRUCTOR

	void clear() override;

	void load(std::FILE* bfp, int req_size) override;
	void save(std::FILE* bfp) override;

	//~CBotGAValues();

	// crossover with other individual
	void crossOver(IIndividual* other) override;

	// mutate some values
	void mutate() override;

	// get new copy of this
	// sub classes return their class with own values
	IIndividual* copy() override;

	unsigned size() const;
	void convert(unsigned* iBits) const;

	// ADD THESE MISSING DECLARATIONS
	bool get(int iIndex) const;
	void set(int iIndex, bool bSet) const;
	void random() const;
	void freeMemory();

private:
	CBits* m_theBits;
};