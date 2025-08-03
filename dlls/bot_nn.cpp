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
#include "nn.h"

#include "perceptron.h"
#include "gannconst.h"

float NN::g_learnRate = 0.9f;

NNLayer::NNLayer(std::FILE* bfp)
{
	load(bfp);
}

NNLayer::NNLayer(const int iNumNeurons, const int iNumInputs)
{
	m_Neurons.reserve(iNumNeurons);
	for (int i = 0; i < iNumNeurons; i++)
		m_Neurons.emplace_back(std::make_unique<CPerceptron>(iNumInputs, nullptr, NN::g_learnRate));
}

NN::NN(const int iNumHiddenLayers, const int iNumInputs, const int iNumNeuronsPerHiddenLayer, const int iNumOutputs)
{
	m_iNumInputs = iNumInputs;
	m_Layers.reserve(iNumHiddenLayers + 2);
	m_Layers.emplace_back(std::make_unique<NNLayer>(iNumNeuronsPerHiddenLayer, iNumInputs));

	for (int i = 0; i < iNumHiddenLayers; i++)
		m_Layers.emplace_back(std::make_unique<NNLayer>(iNumNeuronsPerHiddenLayer, iNumNeuronsPerHiddenLayer));
	//output layer
	m_Layers.emplace_back(std::make_unique<NNLayer>(iNumOutputs, iNumNeuronsPerHiddenLayer));
}

void NN::setWeights(const std::vector<ga_value>& weights)
{
	unsigned w = 0;

	for (const std::unique_ptr<NNLayer>& l : m_Layers)
	{
		for (unsigned j = 0; j < l->numNeurons(); j++)
		{
			CPerceptron* n = l->getNeuron(j);

			for (unsigned k = 0; k < n->numWeights(); k++)
			{
				if (w < weights.size())
				{
					n->setWeight(k, weights[w++]);
				}
			}
		}
	}
}

void NN::getWeights(std::vector<ga_value>& weights) const
{
	weights.clear();
	for (const std::unique_ptr<NNLayer>& l : m_Layers)
	{
		for (unsigned j = 0; j < l->numNeurons(); j++)
		{
			const CPerceptron* n = l->getNeuron(j);

			for (unsigned k = 0; k < n->numWeights(); k++)
			{
				weights.emplace_back(n->getWeight(k));
			}
		}
	}
}

void NN::trainOutputs(const std::vector<ga_value>& wanted_outputs)
{
	unsigned w = 0;

	for (const std::unique_ptr<NNLayer>& l : m_Layers)
	{
		for (unsigned j = 0; j < l->numNeurons(); j++)
		{
			if (w < wanted_outputs.size())
			{
				CPerceptron* n = l->getNeuron(j);
				n->train(wanted_outputs[w++]);
			}
		}
	}
}

void NN::getOutputs(std::vector<ga_value>& outputs) const
{
	outputs.clear();

	if (m_Layers.empty()) {
		return;
	}

	const std::unique_ptr<NNLayer>& last_layer = m_Layers.back();
	outputs.reserve(last_layer->numNeurons());

	for (unsigned j = 0; j < last_layer->numNeurons(); j++)
	{
		const CPerceptron* n = last_layer->getNeuron(j);
		outputs.emplace_back(n->getOutput());
	}
}

void NN::execute(std::vector<ga_value>& outputs, const std::vector<ga_value>& inputs) const
{
	std::vector<ga_value> current_outputs;
	std::vector<ga_value> current_inputs = inputs;

	for (const std::unique_ptr<NNLayer>& l : m_Layers)
	{
		current_outputs.clear();
		current_outputs.reserve(l->numNeurons());

		for (unsigned j = 0; j < l->numNeurons(); j++)
		{
			CPerceptron* n = l->getNeuron(j);

			n->input(current_inputs);

			n->execute();
			current_outputs.emplace_back(n->getOutput());
		}
		current_inputs.swap(current_outputs);
	}
	outputs.swap(current_inputs);
}

void NNLayer::save(std::FILE* bfp) const
{
	if (!bfp) return;

	const unsigned iTemp = m_Neurons.size();

	std::fwrite(&iTemp, sizeof(unsigned), 1, bfp);

	const CGenericHeader header = CGenericHeader(LEARNTYPE_NN_LAYER, static_cast<int>(iTemp));

	header.write(bfp);

	for (const std::unique_ptr<CPerceptron>& m_Neuron : m_Neurons)
	{
		m_Neuron->save(bfp);
	}
}

void NNLayer::load(std::FILE* bfp)
{
	if (!bfp) return;

	unsigned iTemp;

	std::fread(&iTemp, sizeof(unsigned), 1, bfp);

	const CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN_LAYER, iTemp);

	if (CGenericHeader::read(bfp, header1))
	{
		m_Neurons.reserve(iTemp);
		for (unsigned i = 0; i < iTemp; i++)
		{
			m_Neurons.emplace_back(std::make_unique<CPerceptron>(bfp));
		}
	}
}

void NN::load(std::FILE* bfp)
{
	if (std::feof(bfp))
		return;

	unsigned iTemp;

	std::fread(&iTemp, sizeof(unsigned), 1, bfp);

	const CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN, iTemp);

	if (CGenericHeader::read(bfp, header1))
	{
		m_Layers.reserve(iTemp);
		for (unsigned i = 0; i < iTemp; i++)
		{
			m_Layers.emplace_back(std::make_unique<NNLayer>(bfp));
		}
	}
}

void NN::save(std::FILE* bfp) const
{
	if (std::feof(bfp))
		return;

	const unsigned iTemp = m_Layers.size();

	std::fwrite(&iTemp, sizeof(unsigned), 1, bfp);

	const CGenericHeader header = CGenericHeader(LEARNTYPE_NN, iTemp);

	header.write(bfp);

	for (const std::unique_ptr<NNLayer>& m_Layer : m_Layers)
	{
		m_Layer->save(bfp);
	}
}

void NN::randomize() const
{
	for (const std::unique_ptr<NNLayer>& l : m_Layers)
	{
		for (unsigned j = 0; j < l->numNeurons(); j++)
		{
			l->getNeuron(j)->randomize();
		}
	}
}

NNGATrained::NNGATrained(const int iNumHiddenLayers, const int iNumInputs, const int iNumNeuronsPerHiddenLayer, const int iNumOutputs)
	: NN(iNumHiddenLayers, iNumInputs, iNumNeuronsPerHiddenLayer, iNumOutputs)
{
	m_pGA = std::make_unique<CGA>(12);
	m_pInd = std::make_unique<CBotGAValues>();
}

NNGATrained::~NNGATrained()
{
	if (m_pGA)
	{
		m_pGA->freeGlobalMemory();
	}
}

void NNGATrained::train(const std::vector<CNNTrainSet>& trainingsets)
{
	if (m_pGA->canPick())
	{
		m_pInd.reset(m_pGA->pick());

		m_pInd->setFitness(0);
		std::vector<ga_value> weights;
		static_cast<CBotGAValues*>(m_pInd.get())->getVector(weights);
		setWeights(weights);
	}
	else
	{
		std::vector<ga_value> weights;

		m_pInd->clear();
		m_pInd->setFitness(0);

		randomize();

		getWeights(weights);

		static_cast<CBotGAValues*>(m_pInd.get())->setVector(weights);
	}

	std::vector<ga_value> outputs;

	ga_value fTotalError = 0;
	ga_value iNum = 0;

	for (const CNNTrainSet& trainingset : trainingsets)
	{
		this->execute(outputs, trainingset.inputs);

		for (size_t j = 0; j < outputs.size(); ++j)
		{
			if (j < trainingset.outputs.size())
			{
				const ga_value fError = outputs[j] - trainingset.outputs[j];
				fTotalError += fError * fError;
				++iNum;
			}
		}
	}

	if (iNum > 0)
	{
		fTotalError /= iNum;
		if (fTotalError > std::numeric_limits<ga_value>::epsilon())
		{
			m_pInd->setFitness(1 / fTotalError);
		}
	}


	m_pGA->addToPopulation(m_pInd->copy());
}