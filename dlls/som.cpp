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
#include <time.h>

#include "extdll.h"
#include "enginecallback.h"
#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "som.h"

ga_value CSom :: m_fLearnRate = 1.0;

CSom :: CSom ( int iW, int iH, int iIn )
{       
	m_iW = iW;
	m_iH = iH;

	// neighbourhood size
	m_fNSize = (int)((ga_value)iW/2);

	for ( int i = 0; i < iH; i ++ )
	{
		for ( int j = 0; j < iW; j ++ )
			m_Neurons.push_back(new CSomNeuron(iIn,j,i));
	}

	m_iEpochs = 0;
}

CSom :: ~CSom ()
{
	m_Neurons.clear();
}

CSomNeuron *CSom :: getBMU ( vector <ga_value> *inputs )
{       
	CSomNeuron *winner = NULL;
	ga_value bestdistance;
	ga_value dist;

	for ( unsigned int i = 0; i < m_Neurons.size(); i ++ )
	{       
		dist = m_Neurons[i]->distance(inputs);

		if ( !winner || (dist < bestdistance) )
		{
			winner = m_Neurons[i];
			bestdistance = dist;
		}
	}

	return winner;
}



void CSom :: updateAround ( vector<ga_value> *inputs, CSomNeuron *bmu )
{
	ga_value dist;
	ga_value nsiz = (m_fNSize*m_fNSize);

	for ( unsigned int i = 0; i < m_Neurons.size(); i ++ )
	{
		CSomNeuron *current = m_Neurons[i];

		if ( (dist = bmu->neighbourDistance(current,nsiz)) <= nsiz )
		{
			bmu->update(inputs,exp(-(dist) / (2*nsiz)));    
		}           
	}
}

CSomNeuron *CSom :: inputOne ( vector <ga_value> *inputs )
{
	CSomNeuron *winner = getBMU(inputs);

	updateAround(inputs,winner);

	m_fNSize *= 0.75;
	m_fLearnRate *= 0.75;
	m_iEpochs++;

	return winner;
}

CSomNeuron *CSom :: input ( vector < vector <ga_value> > *inputs )
{
	return inputOne(&((*inputs)[RANDOM_LONG(0,(int)inputs->size()-1)]));
}

void CSom :: display ()
{
	//printf("\nDisplaying...\n");

	for ( unsigned int i = 0; i < m_Neurons.size(); i ++ )
	{
		//printf("%d -- ",i);
		m_Neurons[i]->displayWeights();
		//printf("\n");
	}
}

void CSomNeuron :: update ( vector<ga_value> *inputs, ga_value inf )
{
	ga_value change;

	for ( unsigned int i = 0; i < inputs->size(); i ++ )
	{
		change = ((*inputs)[i] - fWeights[i]);

		fWeights[i] += (change*CSom::m_fLearnRate*inf);
	}
}

CSomNeuron :: ~CSomNeuron ()
{
	fWeights.clear();
}

CSomNeuron :: CSomNeuron ()
{
	return;
}

CSomNeuron :: CSomNeuron ( int iInp, int iX, int iY )
{				
	m_iX = iX;
	m_iY = iY;
	
	for ( int i = 0; i < iInp; i ++ )
		fWeights.push_back(RANDOM_FLOAT(0,1));
}

//void CSomNeuron :: update ( vector<ga_value> *inputs, ga_value inf );

ga_value CSomNeuron :: distance ( vector <ga_value> *inputs )
{
	ga_value dist = 0;
	ga_value comp;
	
	for ( unsigned int i = 0; i < inputs->size(); i ++ )
	{
		comp = fWeights[i] - (*inputs)[i];
		
		dist += (comp*comp);
	}
	
	return dist;
}

vector <ga_value> *CSomNeuron :: weights ()
{
	return &fWeights;
}

void CSomNeuron :: displayWeights ()
{
	for ( unsigned int i = 0; i < fWeights.size(); i ++ )
	{
		printf("%0.4f,",fWeights[i]);
	}
}

ga_value CSomNeuron :: neighbourDistance ( CSomNeuron *other, ga_value fDistance )
{
	ga_value distx = getX()-other->getX();
	ga_value disty = getY()-other->getY();
	
	return (distx*distx)+(disty*disty);
}