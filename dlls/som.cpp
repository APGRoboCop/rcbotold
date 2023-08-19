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

CSom::CSom(int iW, int iH, int iIn)
{
	m_iW = iW;
	m_iH = iH;

	// neighbourhood size
	m_fNSize = static_cast<ga_value>(iW) / 2;

	for (int i = 0; i < iH; i++)
	{
		for (int j = 0; j < iW; j++)
			m_Neurons.emplace_back(new CSomNeuron(iIn, j, i));
	}

	m_iEpochs = 0;
}

CSom :: ~CSom()
{
	m_Neurons.clear();
}

CSomNeuron* CSom::getBMU(std::vector <ga_value>* inputs) const
{
	CSomNeuron* winner = nullptr;
	ga_value bestdistance = 0;

	for (unsigned int i = 0; i < m_Neurons.size(); i++)
	{
		const ga_value dist = m_Neurons[i]->distance(inputs);

		if (!winner || dist < bestdistance)
		{
			winner = m_Neurons[i];
			bestdistance = dist;
		}
	}

	return winner;
}

void CSom::updateAround(std::vector<ga_value>* inputs, CSomNeuron* bmu) const
{
	ga_value dist;
	const ga_value nsiz = m_fNSize * m_fNSize;

	for (unsigned int i = 0; i < m_Neurons.size(); i++)
	{
		CSomNeuron* current = m_Neurons[i];

		if ((dist = bmu->neighbourDistance(current, nsiz)) <= nsiz)
		{
			bmu->update(inputs, std::exp(-dist / (2 * nsiz)));
		}
	}
}

CSomNeuron* CSom::inputOne(std::vector <ga_value>* inputs)
{
	CSomNeuron* winner = getBMU(inputs);

	updateAround(inputs, winner);

	m_fNSize *= 0.75f;
	m_fLearnRate *= 0.75f;
	m_iEpochs++;

	return winner;
}

void CSom::input(std::vector<std::vector<ga_value>>* inputs, int epochs)//TODO: Experimental [APG]RoboCop[CL]
{
	// Check if the inputs vector is not empty and the number of epochs is valid
	if (inputs == nullptr || inputs->empty() || epochs <= 0) {
		// Handle invalid input and return (e.g., throw an exception, log an error)
		return;
	}

	// Get the number of input vectors and the length of each input vector
	const size_t numInputVectors = inputs->size();
	size_t inputVectorLength = (*inputs)[0].size();

	// Initialize the self-organizing map with random initial weights
	// This step is not shown in the provided code snippet and requires initialization logic

	// Training loop for the specified number of epochs
	for (int epoch = 0; epoch < epochs; ++epoch) {
		// Iterate through each input vector
		for (size_t i = 0; i < numInputVectors; ++i) {
			// Get the current input vector
			std::vector<ga_value>& inputVector = (*inputs)[i];

			// Find the best matching unit (BMU) in the self-organizing map for the current input
			// This involves calculating the distances between the input and each unit in the map

			// Update the weights of the BMU and its neighbors based on a learning rate and neighborhood function
			// This step involves adjusting the weights of the BMU and its neighbors to better match the input

			// Repeat the above steps for all input vectors in the epoch
		}
	}
}

CSomNeuron* CSom::input(std::vector <std::vector <ga_value> >* inputs)
{
	return inputOne(&(*inputs)[RANDOM_LONG(0, static_cast<int>(inputs->size()) - 1)]);
}

void CSom::display() const
{
	//std::printf("\nDisplaying...\n");

	for (unsigned int i = 0; i < m_Neurons.size(); i++)
	{
		//std::printf("%d -- ",i);
		m_Neurons[i]->displayWeights();
		//std::printf("\n");
	}
}

void CSomNeuron::update(std::vector<ga_value>* inputs, ga_value inf)
{
	for (unsigned int i = 0; i < inputs->size(); i++)
	{
		const ga_value change = (*inputs)[i] - fWeights[i];

		fWeights[i] += change * CSom::m_fLearnRate * inf;
	}
}

CSomNeuron :: ~CSomNeuron()
{
	fWeights.clear();
}

CSomNeuron::CSomNeuron()
{
	m_iX = 0;
	m_iY = 0;
	return;
}

CSomNeuron::CSomNeuron(int iInp, ga_value iX, ga_value iY)
{
	m_iX = iX;
	m_iY = iY;

	for (int i = 0; i < iInp; i++)
		fWeights.emplace_back(RANDOM_FLOAT(0, 1));
}

//void CSomNeuron::update(std::vector<ga_value>* inputs, ga_value inf);

ga_value CSomNeuron::distance(std::vector <ga_value>* inputs) const
{
	ga_value dist = 0;

	for (unsigned int i = 0; i < inputs->size(); i++)
	{
		const ga_value comp = fWeights[i] - (*inputs)[i];

		dist += comp * comp;
	}

	return dist;
}

std::vector <ga_value>* CSomNeuron::weights()
{
	return &fWeights;
}

void CSomNeuron::displayWeights() const
{
	for (unsigned int i = 0; i < fWeights.size(); i++)
	{
		std::printf("%0.4f,", fWeights[i]);
	}
}

ga_value CSomNeuron::neighbourDistance(CSomNeuron* other, ga_value fDistance)
{
	const ga_value distx = getX() - other->getX();
	const ga_value disty = getY() - other->getY();

	return distx * distx + disty * disty;
}