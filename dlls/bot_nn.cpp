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

NNLayer::NNLayer(const int iNumNeurons, const int iNumInputs)
{
	for (int i = 0; i < iNumNeurons; i++)
		m_Neurons.emplace_back(new CPerceptron(iNumInputs, nullptr, NN::g_learnRate));
}

NN::NN(const int iNumHiddenLayers, const int iNumInputs, const int iNumNeuronsPerHiddenLayer, const int iNumOutputs)
{
	//m_Layers.add(new NNLayer(iNumInputs));

	m_iNumInputs = iNumInputs;
	m_Layers.emplace_back(new NNLayer(iNumNeuronsPerHiddenLayer, iNumInputs));

	for (int i = 0; i < iNumHiddenLayers; i++)
		m_Layers.emplace_back(new NNLayer(iNumNeuronsPerHiddenLayer, iNumNeuronsPerHiddenLayer));
	//output layer
	m_Layers.emplace_back(new NNLayer(iNumOutputs, iNumNeuronsPerHiddenLayer));
}

void NN::setWeights(const std::vector<ga_value>* weights) const
{
	unsigned int w = 0;

	for (NNLayer* const& l : m_Layers)
	{
		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			CPerceptron* n = l->getNeuron(j);

			for (unsigned int k = 0; k < n->numWeights(); k++)
			{
				n->setWeight(k, (*weights)[w++]);
			}
		}
	}
}

void NN::getWeights(std::vector<ga_value>* weights) const
{
	for (NNLayer* const& l : m_Layers)
	{
		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			const CPerceptron* n = l->getNeuron(j);

			for (unsigned int k = 0; k < n->numWeights(); k++)
			{
				weights->emplace_back(n->getWeight(k));
			}
		}
	}
}

void NN::trainOutputs(const std::vector<ga_value>* wanted_outputs) const
{
	unsigned int w = 0;

	for (NNLayer* const& l : m_Layers)
	{
		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			CPerceptron* n = l->getNeuron(j);

			n->train((*wanted_outputs)[w++]);
		}
	}
}

void NN::getOutputs(std::vector<ga_value>* outputs) const
{
	//unsigned short int w = 0;

	outputs->clear();

	for (NNLayer* const& l : m_Layers)
	{
		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			const CPerceptron* n = l->getNeuron(j);

			outputs->emplace_back(n->getOutput());
		}
	}
}

void NN::execute(std::vector<ga_value>* outputs, const std::vector<ga_value>* inputs) const
{
	std::vector<ga_value> newoutputs = *inputs; // Initialize with inputs

	outputs->clear();

	for (NNLayer* const l : m_Layers)
	{
		outputs->clear();

		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			CPerceptron* n = l->getNeuron(j);

			n->input(newoutputs);

			n->execute();
			outputs->emplace_back(n->getOutput());
		}
		// Swap to avoid unnecessary copying
		std::swap(newoutputs, *outputs);
	}
}

void NNLayer::save(std::FILE* bfp) const
{
	if (!bfp) return;

	const unsigned int iTemp = m_Neurons.size();

	std::fwrite(&iTemp, sizeof(unsigned int), 1, bfp);

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

	unsigned int iTemp;

	std::fread(&iTemp, sizeof(unsigned int), 1, bfp);

	const CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN_LAYER, iTemp);

	if (CGenericHeader::read(bfp, header1))
	{
		for (unsigned int i = 0; i < iTemp; i++)
		{
			m_Neurons.emplace_back(std::make_unique<CPerceptron>(bfp));
		}
	}
}

void NN::load(std::FILE* bfp)
{
	if (std::feof(bfp))
		return;

	unsigned int iTemp;

	std::fread(&iTemp, sizeof(unsigned int), 1, bfp);

	const CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN, iTemp);
	//CGenericHeader header2; //header2 unused? [APG]RoboCop[CL]

	if (CGenericHeader::read(bfp, header1))
	{
		for (unsigned int i = 0; i < iTemp; i++)
		{
			m_Layers.emplace_back(new NNLayer(bfp));
		}
	}
}

void NN::save(std::FILE* bfp) const
{
	if (std::feof(bfp))
		return;

	const unsigned int iTemp = m_Layers.size();

	std::fwrite(&iTemp, sizeof(unsigned int), 1, bfp);

	const CGenericHeader header = CGenericHeader(LEARNTYPE_NN, iTemp);

	header.write(bfp);

	for (const NNLayer* m_Layer : m_Layers)
	{
		m_Layer->save(bfp);
	}
}

void NN::randomize() const
{
	for (const NNLayer* l : m_Layers)
	{
		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			l->getNeuron(j)->randomize();
		}
	}
}

void NN::freeMemory()
{
	for (NNLayer*& m_Layer : m_Layers)
	{
		NNLayer* l = m_Layer;

		for (unsigned int j = 0; j < l->numNeurons(); j++)
		{
			l->freeMemory();
		}

		delete l;
		m_Layer = nullptr;
	}

	m_Layers.clear();
}

void NNLayer::freeMemory()
{
	m_Neurons.clear();
}

NNGATrained::NNGATrained(int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs)
{
	//NN::NN(iNumHiddenLayers,iNumInputs,iNumNeuronsPerHiddenLayer,iNumOutputs); //Fails on newer compilers? [APG]RoboCop[CL]

	m_pGA = new CGA(12);
	m_pInd = new CBotGAValues();
}

NNGATrained::~NNGATrained()
{
	if (m_pGA)
	{
		m_pGA->freeGlobalMemory();
		delete m_pGA;
		m_pGA = nullptr;
	}

	if (m_pInd)
	{
		delete m_pInd;
		m_pInd = nullptr;
	}
}

void NNGATrained::train(const std::vector<CNNTrainSet>& trainingsets)
{
	if (m_pGA->canPick())
	{
		m_pInd = m_pGA->pick();

		m_pInd->setFitness(0);
		setWeights(static_cast<CBotGAValues*>(m_pInd)->returnVector());
	}
	else
	{
		std::vector<ga_value> weights;

		m_pInd->clear();
		m_pInd->setFitness(0);

		randomize();

		getWeights(&weights);

		static_cast<CBotGAValues*>(m_pInd)->setVector(weights);
	}

	std::vector<ga_value> outputs;

	ga_value fTotalError = 0;
	ga_value iNum = 0;

	for (const CNNTrainSet& trainingset : trainingsets)
	{
		this->execute(&outputs, &trainingset.inputs);

		for (size_t j = 0; j < outputs.size(); ++j)
		{
			const ga_value fError = outputs[j] - trainingset.outputs[j];

			fTotalError += fError * fError;
			++iNum;
		}
	}

	fTotalError /= iNum;
	m_pInd->setFitness(1 / fTotalError);

	m_pGA->addToPopulation(m_pInd->copy());
}