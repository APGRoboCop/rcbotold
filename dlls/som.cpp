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
#include <ctime>
#include <memory>
#include <limits>

#include "extdll.h"
#include "enginecallback.h"
#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "som.h"

ga_value CSom::m_fLearnRate = 1.0f;

CSom::CSom(const int iW, const int iH, const int iIn)
	: m_iH(iH), m_iW(iW), m_iEpochs(0)
{
	// neighbourhood size
	m_fNSize = static_cast<ga_value>(iW) / 2;

	m_Neurons.reserve(static_cast<size_t>(iH) * iW);
	for (int i = 0; i < iH; i++)
	{
		for (int j = 0; j < iW; j++)
			m_Neurons.emplace_back(std::make_unique<CSomNeuron>(iIn, static_cast<ga_value>(j), static_cast<ga_value>(i)));
	}
}

CSom :: ~CSom() = default;

CSomNeuron* CSom::getBMU(const std::vector<ga_value>& inputs) const
{
	CSomNeuron* winner = nullptr;
	ga_value bestdistance = std::numeric_limits<ga_value>::max();

	for (const auto& neuron : m_Neurons)
	{
		const ga_value dist = neuron->distance(inputs);

		if (dist < bestdistance)
		{
			winner = neuron.get();
			bestdistance = dist;
		}
	}

	return winner;
}

void CSom::updateAround(const std::vector<ga_value>& inputs, CSomNeuron* bmu) const
{
	const ga_value nsiz = m_fNSize * m_fNSize;

	for (const std::unique_ptr<CSomNeuron>& current : m_Neurons)
	{
		const ga_value dist = bmu->neighbourDistance(current.get());
		if (dist <= nsiz)
		{
			current->update(inputs, std::exp(-dist / (2 * nsiz)));
		}
	}
}

CSomNeuron* CSom::inputOne(const std::vector <ga_value>& inputs)
{
	CSomNeuron* winner = getBMU(inputs);

	updateAround(inputs, winner);

	m_fNSize *= 0.75f;
	m_fLearnRate *= 0.75f;
	m_iEpochs++;

	return winner;
}

void CSom::input(const std::vector<std::vector<ga_value>>& inputs, const int epochs)//TODO: Experimental [APG]RoboCop[CL]
{
	// Check if the inputs vector is not empty and the number of epochs is valid
	if (inputs.empty() || epochs <= 0)
	{
		// Handle invalid input and return (e.g., throw an exception, log an error)
		return;
	}

	// Training loop for the specified number of epochs
	for (int epoch = 0; epoch < epochs; ++epoch)
	{
		// Iterate through each input vector by selecting one randomly
		for (size_t i = 0; i < inputs.size(); ++i)
		{
			// Get a random input vector and train the network with it
			inputOne(inputs[RANDOM_LONG(0, static_cast<int>(inputs.size()) - 1)]);
		}
	}
}

CSomNeuron* CSom::input(const std::vector <std::vector <ga_value> >& inputs)
{
	return inputOne(inputs[RANDOM_LONG(0, static_cast<int>(inputs.size()) - 1)]);
}

void CSom::display() const
{
	//std::printf("\nDisplaying...\n");

	for (const std::unique_ptr<CSomNeuron>& m_Neuron : m_Neurons)
	{
		//std::printf("%d -- ",i);
		m_Neuron->displayWeights();
		//std::printf("\n");
	}
}

void CSomNeuron::update(const std::vector<ga_value>& inputs, const ga_value inf)
{
	const size_t count = std::min(inputs.size(), fWeights.size());
	for (size_t i = 0; i < count; i++)
	{
		const ga_value change = inputs[i] - fWeights[i];
		fWeights[i] += change * CSom::m_fLearnRate * inf;
	}
}

CSomNeuron :: ~CSomNeuron() = default;

CSomNeuron::CSomNeuron() : m_iX(0), m_iY(0)
{
}

CSomNeuron::CSomNeuron(const int iInp, const ga_value iX, const ga_value iY)
	: m_iX(iX), m_iY(iY)
{
	fWeights.reserve(iInp);
	for (int i = 0; i < iInp; i++)
		fWeights.emplace_back(RANDOM_FLOAT(0, 1));
}

//void CSomNeuron::update(std::vector<ga_value>* inputs, ga_value inf);

ga_value CSomNeuron::distance(const std::vector <ga_value>& inputs) const
{
	ga_value dist = 0;

	for (unsigned i = 0; i < inputs.size(); i++)
	{
		const ga_value comp = fWeights[i] - inputs[i];

		dist += comp * comp;
	}

	return dist;
}

const std::vector <ga_value>& CSomNeuron::weights() const
{
	return fWeights;
}

void CSomNeuron::displayWeights() const
{
	for (const float fWeight : fWeights)
	{
		std::printf("%0.4f,", fWeight);
	}
}

ga_value CSomNeuron::neighbourDistance(const CSomNeuron* other) const
{
	const ga_value distx = getX() - other->getX();
	const ga_value disty = getY() - other->getY();

	return distx * distx + disty * disty;
}