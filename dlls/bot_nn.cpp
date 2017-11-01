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

float NN :: g_learnRate = 0.9f;

NNLayer :: NNLayer ( int iNumNeurons, int iNumInputs )
{
	for ( int i = 0; i < iNumNeurons; i ++ )
		m_Neurons.push_back(new CPerceptron(iNumInputs,NULL,NN::g_learnRate));
}

NN :: NN ( int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs )
{
	//m_Layers.add(new NNLayer(iNumInputs));

	m_iNumInputs = iNumInputs;
	m_Layers.push_back(new NNLayer(iNumNeuronsPerHiddenLayer,iNumInputs));

	for ( int i = 0; i < iNumHiddenLayers; i ++ )
		m_Layers.push_back(new NNLayer(iNumNeuronsPerHiddenLayer,iNumNeuronsPerHiddenLayer));
//output layer
	m_Layers.push_back(new NNLayer(iNumOutputs,iNumNeuronsPerHiddenLayer));
}

void NN :: setWeights ( vector<ga_value> *weights )
{
	register unsigned short int i;
	register unsigned short int j;
	register unsigned short int k;
	register unsigned short int w = 0;

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			CPerceptron *n = l->getNeuron(j);

			for ( k = 0; k < n->numWeights(); k ++ )
			{
				n->setWeight(k,(*weights)[w++]);
			}
		}
	}
}

void NN :: getWeights ( vector<ga_value> *weights )
{
	register unsigned short int i;
	register unsigned short int j;
	register unsigned short int k;

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			CPerceptron *n = l->getNeuron(j);

			for ( k = 0; k < n->numWeights(); k ++ )
			{
				weights->push_back(n->getWeight(k));
			}
		}
	}
}

void NN :: trainOutputs( vector<ga_value> *wanted_outputs )
{
	register unsigned short int i;
	register unsigned short int j;
	register unsigned short int w = 0;
	
	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			CPerceptron *n = l->getNeuron(j);

			n->train((*wanted_outputs)[w++]);
		}
	}
}

void NN :: getOutputs ( vector<ga_value> *outputs )
{
	register unsigned short int i;
	register unsigned short int j;
	register unsigned short int w = 0;
	
	outputs->clear();

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			CPerceptron *n = l->getNeuron(j);

			outputs->push_back(n->getOutput());
		}
	}
}

void NN :: execute ( vector <ga_value> *outputs, vector <ga_value> *inputs )
{
	register unsigned short int i;
	register unsigned short int j;

	vector<ga_value> newoutputs;

	outputs->clear();

	for ( i = 0; i < inputs->size(); i ++ )
		outputs->push_back((*inputs)[i]);

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		newoutputs.clear();

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			CPerceptron *n = l->getNeuron(j);

			n->input(outputs);
			n->execute();

			newoutputs.push_back(n->getOutput());
		}

		outputs->clear();

		for ( unsigned int k = 0; k < newoutputs.size(); k ++ )
			outputs->push_back(newoutputs[k]);
	}	

}


void NNLayer :: save ( FILE *bfp )
{
	unsigned int iTemp;

	iTemp = m_Neurons.size();

	fwrite(&iTemp,sizeof(unsigned int),1,bfp);

	CGenericHeader header = CGenericHeader(LEARNTYPE_NN_LAYER,(int)iTemp);

	header.write(bfp);

	for ( unsigned int i = 0; i < m_Neurons.size(); i ++ )
	{
		m_Neurons[i]->save(bfp);
	}
}

void NNLayer :: load ( FILE *bfp )
{
	unsigned int iTemp;

	fread(&iTemp,sizeof(unsigned int),1,bfp);

	CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN_LAYER,iTemp);
	CGenericHeader header2;

	if ( header2.read(bfp,header1) )
	{
		for ( unsigned int i = 0; i < m_Neurons.size(); i ++ )
		{
			m_Neurons.push_back(new CPerceptron(bfp));
		}
	}
}

void NN :: load ( FILE *bfp )
{
	if ( feof(bfp) )
		return;

	unsigned int iTemp;

	fread(&iTemp,sizeof(unsigned int),1,bfp);

	CGenericHeader header1 = CGenericHeader(LEARNTYPE_NN,iTemp);
	CGenericHeader header2;

	if ( header2.read(bfp,header1) )
	{
		for ( unsigned int i = 0; i < iTemp; i ++ )
		{			
			m_Layers.push_back(new NNLayer(bfp));
		}
	}
}

void NN :: save ( FILE *bfp )
{
	if ( feof(bfp) )
		return;

	unsigned int iTemp = m_Layers.size();

	fwrite(&iTemp,sizeof(unsigned int),1,bfp);

	CGenericHeader header = CGenericHeader(LEARNTYPE_NN,iTemp);

	header.write(bfp);

	for ( unsigned int i = 0; i < m_Layers.size(); i ++ )
	{
		m_Layers[i]->save(bfp);
	}
}

void NN :: randomize ()
{
	register unsigned short int i;
	register unsigned short int j;

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			l->getNeuron(j)->randomize();
		}
	}
}

void NN :: freeMemory ()
{
	register unsigned short int i;
	register unsigned short int j;

	for ( i = 0; i < m_Layers.size(); i ++ )
	{
		NNLayer *l = m_Layers[i];

		for ( j = 0; j < l->numNeurons(); j ++ )
		{
			l->freeMemory();
		}

		delete l;
		m_Layers[i]=NULL;
	}

	m_Layers.clear();
}

void NNLayer :: freeMemory ()
{
	register unsigned short int i;

	for ( i = 0; i < m_Neurons.size(); i ++ )
	{
		delete m_Neurons[i];
		m_Neurons[i]=NULL;
	}

	m_Neurons.clear();
}

NNGATrained::NNGATrained (int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs )
{
	NN::NN(iNumHiddenLayers,iNumInputs,iNumNeuronsPerHiddenLayer,iNumOutputs);
	
	m_pGA = new CGA(12);
	m_pInd = new CBotGAValues();
	
}

NNGATrained::~NNGATrained ()
{
	if ( m_pGA )
	{
		m_pGA->freeGlobalMemory();
		delete m_pGA;
		m_pGA=NULL;
	}

	if ( m_pInd )
	{	
		delete m_pInd;
		m_pInd=NULL;
	}
}

void NNGATrained :: train ( vector<CNNTrainSet> trainingsets )
{	
	if ( m_pGA->canPick() )
	{
		m_pInd = m_pGA->pick();

		m_pInd->setFitness(0);
		setWeights(((CBotGAValues*)m_pInd)->returnVector());
	}
	else
	{
		vector<ga_value> weights;

		m_pInd->clear();
		m_pInd->setFitness(0);

		randomize();

		getWeights(&weights);

		((CBotGAValues*)m_pInd)->setVector(weights);				
	}

	vector<ga_value> outputs;

	ga_value fTotalError = 0;
	unsigned int iNum = 0;

	for ( unsigned int i = 0; i < trainingsets.size(); i ++ )
	{
		this->execute(&outputs,&(trainingsets[i].inputs));

		for ( unsigned int j = 0; j < outputs.size(); j ++ )
		{
			ga_value fError;

			fError = (outputs[j]-(trainingsets[i].outputs[j]));

			fTotalError += fError*fError;
			iNum ++;
		}
	}

	fTotalError = fTotalError / iNum;
	
	m_pInd->setFitness(1/fTotalError);

	m_pGA->addToPopulation(m_pInd->copy());
}