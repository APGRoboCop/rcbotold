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
#include "extdll.h"
#include "enginecallback.h"
#ifndef RCBOT_META_BUILD
#include "util.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot.h"
#include "perceptron.h"
#include "bot_ga.h"

ga_value CPerceptron::m_fDefaultLearnRate = 0.5f;
ga_value CPerceptron::m_fDefaultBias = 1.0f;

ga_value CSigmoidTransfer::transfer(ga_value netInput)
{
	return 1 / (1 + exp(-netInput));
}

CPerceptron::CPerceptron(unsigned int iInputs, ITransfer* transferFunction, float fLearnRate)
{
	m_inputs.clear();
	m_iInputs = iInputs;

	// bias weight
	m_weights.emplace_back(RANDOM_FLOAT(0, 0.6) - 0.3);

	for (unsigned int i = 0; i < m_iInputs; i++)
		m_weights.emplace_back(RANDOM_FLOAT(0, 0.6) - 0.3);

	m_transferFunction = transferFunction;

	if (m_transferFunction == nullptr)
		m_transferFunction = new CSigmoidTransfer();

	m_Bias = m_fDefaultBias;

	if (fLearnRate == 0.0f)
		m_LearnRate = m_fDefaultLearnRate;
	else
		m_LearnRate = fLearnRate;

	m_bTrained = FALSE;
	m_output = 0;
}

void CPerceptron::setWeights(CBotGAValues* vals, int iFrom, int iNum)
{
	const float bias = m_weights[0];

	m_weights.clear();

	m_weights.emplace_back(bias);

	for (int i = iFrom; i < iFrom + iNum; i++)
	{
		m_weights.emplace_back(vals->get(i));
	}
}

void CPerceptron::setWeights(std::vector <ga_value> const& weights, int iFrom, int iNum)
{
	m_weights.clear();

	for (int i = iFrom; i < iFrom + iNum; i++)
	{
		m_weights.emplace_back(weights[i]);
	}
}

void CPerceptron::randomize()
{
	for (unsigned int i = 0; i < m_weights.size(); i++)
		m_weights[i] = RANDOM_FLOAT(0, 0.6) - 0.3;
}

void CPerceptron::setWeights(std::vector <ga_value> const& weights)
{
	m_weights.clear();

	for (unsigned int i = 0; i < weights.size(); i++)
		m_weights.emplace_back(weights[i]);
}

void CPerceptron::input(std::vector <ga_value>* inputs)
{
	m_inputs.clear();

	for (unsigned int i = 0; i < inputs->size(); i++)
		m_inputs.emplace_back((*inputs)[i]);
}

ga_value CPerceptron::execute()
{
	// bias weight
	ga_value fNetInput = m_weights[0];

	for (unsigned int i = 0; i < m_inputs.size(); i++)
	{
		fNetInput += m_weights[i + 1] * m_inputs[i];
	}

	m_output = m_transferFunction->transfer(fNetInput);

	return m_output;
}

BOOL CPerceptron::fired()
{
	return m_output >= 0.5f;
}

ga_value CPerceptron::getOutput()
{
	return m_output;
}

void CPerceptron::train(ga_value expectedOutput)
{
	m_bTrained = TRUE;

	// bias
	m_weights[0] = m_weights[0] + m_LearnRate * (expectedOutput - m_output) * m_Bias;

	for (unsigned int i = 1; i < m_weights.size(); i++)
	{
		m_weights[i] = m_weights[i] + m_LearnRate * (expectedOutput - m_output) * m_inputs[i - 1];
	}
}

void CPerceptron::save(FILE* bfp)
{
	unsigned int iTemp;
	unsigned int i;

	CGenericHeader header = CGenericHeader(LEARNTYPE_PERCEPTRON, m_iInputs);

	header.write(bfp);

	fwrite(&m_iInputs, sizeof(unsigned int), 1, bfp);
	fwrite(&m_Bias, sizeof(ga_value), 1, bfp);
	fwrite(&m_LearnRate, sizeof(ga_value), 1, bfp);

	iTemp = m_inputs.size();

	fwrite(&iTemp, sizeof(int), 1, bfp);
	for (i = 0; i < iTemp; i++)
		fwrite(&m_inputs[i], sizeof(ga_value), 1, bfp);

	iTemp = m_weights.size();

	fwrite(&iTemp, sizeof(int), 1, bfp);
	for (i = 0; i < iTemp; i++)
		fwrite(&m_weights[i], sizeof(ga_value), 1, bfp);

	fwrite(&m_output, sizeof(ga_value), 1, bfp);
	fwrite(&m_bTrained, sizeof(BOOL), 1, bfp);
}

void CPerceptron::load(FILE* bfp)
{
	unsigned int iTemp;
	unsigned int i;

	if (feof(bfp))
		return;

	const CGenericHeader header = CGenericHeader(LEARNTYPE_PERCEPTRON, m_iInputs);

	if (!header.read(bfp, header))
	{
		BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
		return;
	}

	fread(&m_iInputs, sizeof(unsigned int), 1, bfp);
	fread(&m_Bias, sizeof(ga_value), 1, bfp);
	fread(&m_LearnRate, sizeof(ga_value), 1, bfp);

	// inputs
	m_inputs.clear();
	fread(&iTemp, sizeof(int), 1, bfp);
	for (i = 0; i < iTemp; i++)
	{
		ga_value fVal;

		// reliability check
		if (feof(bfp))
			return;

		fread(&fVal, sizeof(ga_value), 1, bfp);
		m_inputs.emplace_back(fVal);
	}

	m_weights.clear();
	fread(&iTemp, sizeof(unsigned int), 1, bfp);
	for (i = 0; i < iTemp; i++)
	{
		ga_value fVal;

		// reliability check
		if (feof(bfp))
			return;

		fread(&fVal, sizeof(ga_value), 1, bfp);
		m_weights.emplace_back(fVal);
	}

	fread(&m_output, sizeof(ga_value), 1, bfp);
	fread(&m_bTrained, sizeof(BOOL), 1, bfp);
}

void CPerceptron::load(char* filename, int iProfileId)
{
	FILE* bfp = RCBOpenFile(filename, "rb", SAVETYPE_BOT, iProfileId);

	if (bfp)
	{
		load(bfp);
		fclose(bfp);
	}
}

void CPerceptron::save(char* filename, int iProfileId)
{
	FILE* bfp = RCBOpenFile(filename, "wb", SAVETYPE_BOT, iProfileId);

	if (bfp)
	{
		save(bfp);
		fclose(bfp);
	}
}