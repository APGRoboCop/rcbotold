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
#ifndef __RCBOT_SOM_H__
#define __RCBOT_SOM_H__

#include <cmath>

#include "gannconst.h"

#include <vector>

class CSomNeuron
{
public:
	~CSomNeuron();

	CSomNeuron();

	CSomNeuron(int iInp, ga_value iX, ga_value iY);

	ga_value getX() const
	{
		return m_iX;
	}

	ga_value getY() const
	{
		return m_iY;
	}

	void update(const std::vector<ga_value>& inputs, ga_value inf);

	ga_value distance(const std::vector <ga_value>& inputs) const;

	const std::vector <ga_value>& weights() const;

	void displayWeights() const;

	ga_value neighbourDistance(const CSomNeuron* other, ga_value fDistance) const;

private:
	std::vector<ga_value> fWeights;
	ga_value m_iX;
	ga_value m_iY;
};

class CSom
{
public:
	static ga_value m_fLearnRate;

	CSom(int iW, int iH, int iIn);

	~CSom();

	CSomNeuron* getBMU(const std::vector <ga_value>& inputs) const;

	void updateAround(const std::vector<ga_value>& inputs, CSomNeuron* bmu) const;

	CSomNeuron* input(const std::vector < std::vector <ga_value> >& inputs);

	CSomNeuron* inputOne(const std::vector < ga_value >& inputs);

	void input(const std::vector <std::vector <ga_value> >& inputs, int epochs);

	void display() const;

	unsigned epochs() const
	{
		return m_iEpochs;
	}

private:
	std::vector<std::unique_ptr<CSomNeuron>> m_Neurons;
	int m_iH;
	int m_iW;
	ga_value m_fNSize;
	unsigned m_iEpochs;
};
#endif