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

#ifndef __RCBOT_NN_H__
#define __RCBOT_NN_H__

#include "gannconst.h"
#include "perceptron.h"
#include <vector>

class CPerceptron;
class CGA;

class CNNTrainSet
{
public:
	std::vector<ga_value> inputs;
	std::vector<ga_value> outputs;
};

class NNLayer
{
public:
	NNLayer(std::FILE* bfp)
	{
		load(bfp);
	}

	NNLayer(int iNumNeurons, int iNumInputs);

	int numNeurons() const { return m_Neurons.size(); }

	CPerceptron* getNeuron(int iNeuron) const { return m_Neurons[iNeuron]; }

	void freeMemory();

	void save(std::FILE* bfp) const;

	void load(std::FILE* bfp);
private:
	std::vector<CPerceptron*> m_Neurons;
};

class NN
{
public:
	virtual ~NN() = default;

	static float g_learnRate;

	NN()
	{
		m_iNumInputs = 0;
	}

	NN(int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs);

	void setWeights(std::vector<ga_value>* weights) const;

	void getWeights(std::vector<ga_value>* weights) const;

	void execute(std::vector <ga_value>* outputs, std::vector <ga_value>* inputs) const;

	void freeMemory();

	void randomize() const;

	void getOutputs(std::vector<ga_value>* outputs) const;
	void trainOutputs(std::vector<ga_value>* wanted_outputs) const;

	void load(std::FILE* bfp);

	void save(std::FILE* bfp) const;

	virtual void train(std::vector<CNNTrainSet> trainingsets)
	{
		//return;
	}
private:
	std::vector<NNLayer*> m_Layers;
	int m_iNumInputs;
};

class NNGATrained : public NN
{
public:
	NNGATrained(int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs);

	~NNGATrained() override;

	void train(std::vector<CNNTrainSet> trainingsets) override;
private:
	CGA* m_pGA;
	IIndividual* m_pInd;
};

#endif