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

#pragma once

 //#include "ga.h"
#include "gannconst.h"
#include <vector>
#include <memory>

typedef float ga_value;

class CBotGAValues;

class ITransfer
{
public:
	virtual ~ITransfer() = default;
	virtual ga_value transfer(ga_value netInput) = 0;
};

class CSigmoidTransfer : public ITransfer
{
public:
	ga_value transfer(ga_value netInput) override;
};

class CPerceptron //: public IIndividual
{
public:

	static ga_value m_fDefaultLearnRate;// = 0.5f;
	static ga_value m_fDefaultBias;// = 1.0f;
	/*
		void load ( FILE *bfp, int req_size ) = 0;
		//void save ( FILE *bfp ) = 0;

		// crossover with other individual
		virtual void crossOver ( IIndividual *other )
		{
			std::memset(other,this,sizeof(CPerceptron));
			other->clear();
		}

		// mutate some values
		virtual void mutate () = 0;

		virtual void clear () = 0;

		// get new copy of this
		// sub classes return their class with own values
		virtual IIndividual *copy () = 0;
	*/

	CPerceptron(int iInputs, ITransfer* transferFunction = nullptr, float fLearnRate = 0.0f);

	CPerceptron(std::FILE* bfp)
	{
		load(bfp);
	}

	void setWeights(const std::vector<ga_value>& weights);

	void setWeights(const std::vector<ga_value>& weights, int iFrom, int iNum);

	void setWeights(const CBotGAValues* vals, int iFrom, int iNum);

	unsigned numWeights() const { return m_weights.size(); }

	void setWeight(const unsigned iWeight, ga_value fVal) { m_weights[iWeight] = fVal; }

	ga_value getWeight(const unsigned iWeight) const { return m_weights[iWeight]; }

	void input(const std::vector<ga_value>& inputs);

	ga_value execute();

	bool fired() const;

	ga_value getOutput() const;

	void train(ga_value expectedOutput);

	void randomize();

	~CPerceptron() = default;

	bool trained() const { return m_bTrained; }

	void setTrained() { m_bTrained = true; }

	void save(std::FILE* bfp) const;
	void load(std::FILE* bfp);

	void load(const char* filename, int iProfileId);
	void save(const char* filename, int iProfileId) const;

private:
	int m_iInputs;
	ga_value m_Bias;
	ga_value m_LearnRate;
	ga_value m_output;
	std::unique_ptr<ITransfer> m_transferFunction;
	bool m_bTrained;
	std::vector<ga_value> m_weights;
	std::vector<ga_value> m_inputs;
};