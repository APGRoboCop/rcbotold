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

#pragma once

#include "gannconst.h"
#include "perceptron.h"

#include <cstdio>
#include <memory>
#include <vector>

class CPerceptron;
class CGA;
class IIndividual;

class CNNTrainSet
{
public:
    std::vector<ga_value> inputs;
    std::vector<ga_value> outputs;
};

class NNLayer
{
public:
    explicit NNLayer(std::FILE* bfp);

    NNLayer(int iNumNeurons, int iNumInputs);

    unsigned numNeurons() const
    {
        return m_Neurons.size();
    }

    CPerceptron* getNeuron(const unsigned iNeuron) {
        return m_Neurons[iNeuron].get();
    }

    const CPerceptron* getNeuron(const unsigned iNeuron) const {
        return m_Neurons[iNeuron].get();
    }

    void save(std::FILE* bfp) const;

    void load(std::FILE* bfp);
private:
    std::vector<std::unique_ptr<CPerceptron>> m_Neurons;
};

class NN
{
public:
    virtual ~NN() = default;

    static float g_learnRate;

    NN()
        : m_iNumInputs(0)
    {
    }

    NN(int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs);

    void setWeights(const std::vector<ga_value>& weights) const;

    void getWeights(std::vector<ga_value>& weights) const;

    void execute(std::vector<ga_value>& outputs, const std::vector<ga_value>& inputs) const;

    void randomize() const;

    void getOutputs(std::vector<ga_value>& outputs) const;
    void trainOutputs(const std::vector<ga_value>& wanted_outputs) const;

    void load(std::FILE* bfp);

    void save(std::FILE* bfp) const;

    static void train(const std::vector<CNNTrainSet>&)
    {
        //return;
    }
private:
    std::vector<std::unique_ptr<NNLayer>> m_Layers;
    int m_iNumInputs;
};

class NNGATrained : public NN
{
public:
    NNGATrained(int iNumHiddenLayers, int iNumInputs, int iNumNeuronsPerHiddenLayer, int iNumOutputs);

    ~NNGATrained() override;

    void train(const std::vector<CNNTrainSet>& trainingsets);
private:
    std::unique_ptr<CGA> m_pGA;
    std::unique_ptr<IIndividual> m_pInd;
};