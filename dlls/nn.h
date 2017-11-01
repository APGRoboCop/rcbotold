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
using namespace std;

class CPerceptron;
class CGA;

class CNNTrainSet
{
public:
	vector<ga_value> inputs;
	vector<ga_value> outputs;
};

class NNLayer
{
public:
	NNLayer( FILE *bfp )
	{
		load(bfp);
	}

	NNLayer( int iNumNeurons, int iNumInputs );

	inline int numNeurons () { return m_Neurons.size(); }

	CPerceptron *getNeuron ( int iNeuron ) { return m_Neurons[iNeuron]; }

	void freeMemory ();

	void save ( FILE *bfp );

	void load ( FILE *bfp );
private:
    vector<CPerceptron*> m_Neurons;
};

class NN
{
public:

	static float g_learnRate;

	NN () 
	{ 
		m_iNumInputs = 0;
	};

	NN ( int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs );

	void setWeights ( vector<ga_value> *weights );

	void getWeights ( vector<ga_value> *weights );

	void execute ( vector <ga_value> *outputs, vector <ga_value> *inputs );

	void freeMemory ();

	void randomize ();

	void getOutputs ( vector<ga_value> *outputs );
	void trainOutputs ( vector<ga_value> *wanted_outputs );

	void load ( FILE *bfp );

	void save ( FILE *bfp );

	virtual void train ( vector<CNNTrainSet> trainingsets )
	{
		return;
	}
private:
	vector<NNLayer*> m_Layers;
	int m_iNumInputs;
};

class NNGATrained : public NN
{
public:
	NNGATrained (int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs );

	~NNGATrained ();

	virtual void train ( vector<CNNTrainSet> trainingsets );
private:
	CGA *m_pGA;
	IIndividual *m_pInd;
};

#endif