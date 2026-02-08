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
#include <algorithm>

class CPerceptron;
class CGA;
class IIndividual;

class CNNTrainSet
{
public:
    std::vector<ga_value> inputs;
    std::vector<ga_value> outputs;
};

///////////////////////////////////////////////
// Enhanced Combat NN Input Structure
// Provides normalized inputs for combat decision neural networks
///////////////////////////////////////////////
constexpr int COMBAT_NN_NUM_INPUTS = 8;
constexpr int COMBAT_NN_NUM_OUTPUTS = 4;  // attack, retreat, strafe, jump
constexpr float COMBAT_MAX_DISTANCE = 2048.0f;
constexpr float COMBAT_MAX_HEALTH = 100.0f;
constexpr float COMBAT_MAX_TEAMMATES = 5.0f;

class CCombatNNInputs
{
public:
    ga_value enemyDistance;      // Normalized 0-1 (distance / max distance)
    ga_value enemyHealth;        // Normalized 0-1 (health / max health)
    ga_value selfHealth;         // Normalized 0-1 (health / max health)
    ga_value selfAmmo;           // Normalized 0-1 (current / max ammo)
    ga_value enemyVisible;       // 0 or 1
    ga_value coverNearby;        // 0 or 1 (from waypoint flags)
    ga_value heightAdvantage;    // -1 to 1 (positive = bot is higher)
    ga_value teammatesNearby;    // Normalized 0-1 (count / max count)

    CCombatNNInputs()
        : enemyDistance(0.5f)
        , enemyHealth(1.0f)
        , selfHealth(1.0f)
        , selfAmmo(1.0f)
        , enemyVisible(0.0f)
        , coverNearby(0.0f)
        , heightAdvantage(0.0f)
        , teammatesNearby(0.0f)
    {
    }

    // Set enemy distance (automatically normalizes)
    void setEnemyDistance(float distance)
    {
        enemyDistance = std::clamp(distance / COMBAT_MAX_DISTANCE, 0.0f, 1.0f);
    }

    // Set enemy health (automatically normalizes)
    void setEnemyHealth(float health, float maxHealth = COMBAT_MAX_HEALTH)
    {
        enemyHealth = (maxHealth > 0.0f) ? std::clamp(health / maxHealth, 0.0f, 1.0f) : 0.0f;
    }

    // Set self health (automatically normalizes)
    void setSelfHealth(float health, float maxHealth = COMBAT_MAX_HEALTH)
    {
        selfHealth = (maxHealth > 0.0f) ? std::clamp(health / maxHealth, 0.0f, 1.0f) : 0.0f;
    }

    // Set self ammo (automatically normalizes)
    void setSelfAmmo(int currentAmmo, int maxAmmo)
    {
        selfAmmo = (maxAmmo > 0) ? std::clamp(static_cast<float>(currentAmmo) / static_cast<float>(maxAmmo), 0.0f, 1.0f) : 0.0f;
    }

    // Set enemy visibility
    void setEnemyVisible(bool visible)
    {
        enemyVisible = visible ? 1.0f : 0.0f;
    }

    // Set cover availability
    void setCoverNearby(bool hasCover)
    {
        coverNearby = hasCover ? 1.0f : 0.0f;
    }

    // Set height advantage from Z difference (positive = bot is higher)
    void setHeightAdvantage(float botZ, float enemyZ)
    {
        constexpr float MAX_HEIGHT_DIFF = 200.0f;
        const float diff = botZ - enemyZ;
        heightAdvantage = std::clamp(diff / MAX_HEIGHT_DIFF, -1.0f, 1.0f);
    }

    // Set teammates nearby count (automatically normalizes)
    void setTeammatesNearby(int count)
    {
        teammatesNearby = std::clamp(static_cast<float>(count) / COMBAT_MAX_TEAMMATES, 0.0f, 1.0f);
    }

    // Convert to vector for NN input
    void toVector(std::vector<ga_value>& inputs) const
    {
        inputs.clear();
        inputs.reserve(COMBAT_NN_NUM_INPUTS);
        inputs.emplace_back(enemyDistance);
        inputs.emplace_back(enemyHealth);
        inputs.emplace_back(selfHealth);
        inputs.emplace_back(selfAmmo);
        inputs.emplace_back(enemyVisible);
        inputs.emplace_back(coverNearby);
        inputs.emplace_back(heightAdvantage);
        inputs.emplace_back(teammatesNearby);
    }

    // Get number of inputs
    static constexpr int numInputs() { return COMBAT_NN_NUM_INPUTS; }
};

///////////////////////////////////////////////
// Combat NN Output Interpretation
///////////////////////////////////////////////
enum eCombatAction : std::uint8_t
{
    COMBAT_ACTION_ATTACK = 0,
    COMBAT_ACTION_RETREAT = 1,
    COMBAT_ACTION_STRAFE = 2,
    COMBAT_ACTION_JUMP = 3
};

class CCombatNNOutputs
{
public:
    ga_value attackWeight;
    ga_value retreatWeight;
    ga_value strafeWeight;
    ga_value jumpWeight;

    CCombatNNOutputs()
        : attackWeight(0.0f)
        , retreatWeight(0.0f)
        , strafeWeight(0.0f)
        , jumpWeight(0.0f)
    {
    }

    // Parse outputs from NN result vector
    void fromVector(const std::vector<ga_value>& outputs)
    {
        if (outputs.size() >= COMBAT_NN_NUM_OUTPUTS)
        {
            attackWeight = outputs[COMBAT_ACTION_ATTACK];
            retreatWeight = outputs[COMBAT_ACTION_RETREAT];
            strafeWeight = outputs[COMBAT_ACTION_STRAFE];
            jumpWeight = outputs[COMBAT_ACTION_JUMP];
        }
    }

    // Get the best action based on highest weight
    eCombatAction getBestAction() const
    {
        ga_value maxWeight = attackWeight;
        eCombatAction bestAction = COMBAT_ACTION_ATTACK;

        if (retreatWeight > maxWeight)
        {
            maxWeight = retreatWeight;
            bestAction = COMBAT_ACTION_RETREAT;
        }
        if (strafeWeight > maxWeight)
        {
            maxWeight = strafeWeight;
            bestAction = COMBAT_ACTION_STRAFE;
        }
        if (jumpWeight > maxWeight)
        {
            bestAction = COMBAT_ACTION_JUMP;
        }

        return bestAction;
    }

    // Check if a specific action should be taken (threshold-based)
    bool shouldAttack(ga_value threshold = 0.5f) const { return attackWeight > threshold; }
    bool shouldRetreat(ga_value threshold = 0.5f) const { return retreatWeight > threshold; }
    bool shouldStrafe(ga_value threshold = 0.3f) const { return strafeWeight > threshold; }
    bool shouldJump(ga_value threshold = 0.4f) const { return jumpWeight > threshold; }

    // Get number of outputs
    static constexpr int numOutputs() { return COMBAT_NN_NUM_OUTPUTS; }
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

    void setWeights(const std::vector<ga_value>& weights);

    void getWeights(std::vector<ga_value>& weights) const;

    void execute(std::vector<ga_value>& outputs, const std::vector<ga_value>& inputs) const;

    void randomize() const;

    void getOutputs(std::vector<ga_value>& outputs) const;
    void trainOutputs(const std::vector<ga_value>& wanted_outputs);

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