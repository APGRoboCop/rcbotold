// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
#include <memory>
#include <numeric>
#include <stdexcept>

ga_value CPerceptron::m_fDefaultLearnRate = 0.5f;
ga_value CPerceptron::m_fDefaultBias = 1.0f;

ga_value CSigmoidTransfer::transfer(const ga_value netInput)
{
    return 1 / (1 + std::exp(-netInput));
}

CPerceptron::CPerceptron(const int iInputs, ITransfer* transferFunction, const float fLearnRate)
    : m_iInputs(iInputs), m_Bias(m_fDefaultBias), m_LearnRate(fLearnRate == 0.0f ? m_fDefaultLearnRate : fLearnRate),
      m_output(0), m_transferFunction(transferFunction ? std::unique_ptr<ITransfer>(transferFunction)
	                         : std::make_unique<CSigmoidTransfer>()), m_bTrained(false)
{
    m_weights.reserve(iInputs + 1);
    m_weights.emplace_back(RANDOM_FLOAT(0, 0.6f) - 0.3f);

    for (int i = 0; i < m_iInputs; i++)
        m_weights.emplace_back(RANDOM_FLOAT(0, 0.6f) - 0.3f);
}

void CPerceptron::setWeights(const CBotGAValues* vals, const int iFrom, const int iNum)
{
    m_weights.clear();
    m_weights.reserve(iNum + 1);
    m_weights.emplace_back(m_Bias); // or vals->get(0) if bias is stored in vals

    for (int i = iFrom; i < iFrom + iNum; i++)
    {
        m_weights.emplace_back(vals->get(i));
    }
}

void CPerceptron::setWeights(const std::vector<ga_value>& weights, const int iFrom, const int iNum)
{
    if (iFrom < 0 || iNum < 0 || iFrom + iNum > static_cast<int>(weights.size())) {
        BotMessage(nullptr, 0, "Invalid range for setWeights");
    }
    m_weights.assign(weights.begin() + iFrom, weights.begin() + iFrom + iNum);
}

void CPerceptron::randomize()
{
    std::generate(m_weights.begin(), m_weights.end(), []() {
        return RANDOM_FLOAT(0, 0.6f) - 0.3f;
        });
}

void CPerceptron::setWeights(const std::vector<ga_value>& weights)
{
    m_weights.assign(weights.begin(), weights.end());
}

void CPerceptron::input(const std::vector<ga_value>& inputs)
{
    m_inputs.assign(inputs.begin(), inputs.end());
}

ga_value CPerceptron::execute()
{
    if (m_weights.size() != m_inputs.size() + 1) {
        BotMessage(nullptr, 0, "Weights and inputs size mismatch");
    }

    // bias weight
    ga_value fNetInput = m_weights[0];

    fNetInput += std::inner_product(m_inputs.begin(), m_inputs.end(), m_weights.begin() + 1, 0.0f);

    m_output = m_transferFunction->transfer(fNetInput);

    return m_output;
}

bool CPerceptron::fired() const
{
    return m_output >= 0.5f;
}

ga_value CPerceptron::getOutput() const
{
    return m_output;
}

void CPerceptron::train(const ga_value expectedOutput)
{
    if (m_inputs.size() != static_cast<size_t>(m_iInputs))
        BotMessage(nullptr, 0, "Cannot train: input size mismatch");
    if (m_weights.size() != m_inputs.size() + 1)
        BotMessage(nullptr, 0, "Cannot train: weights size mismatch");

    m_bTrained = true;

    const ga_value error = expectedOutput - m_output;

    // Update bias weight
    m_weights[0] += m_LearnRate * error * m_Bias;

    // Update weights for inputs
    for (size_t i = 1; i < m_weights.size(); i++)
    {
        m_weights[i] += m_LearnRate * error * m_inputs[i - 1];
    }
}

void CPerceptron::save(std::FILE* bfp) const
{
    if (!bfp)
        BotMessage(nullptr, 0, "File pointer is null");

    const CGenericHeader header = CGenericHeader(LEARNTYPE_PERCEPTRON, m_iInputs);

    // Write header
    header.write(bfp);

    // Helper lambda for fwrite with error checking
    auto safe_write = [](const void* data, const size_t size, const size_t count, std::FILE* file) {
        if (std::fwrite(data, size, count, file) != count) {
            BotMessage(nullptr, 0, "Error writing to file");
        }
    };

    // Write perceptron data
    safe_write(&m_iInputs, sizeof(decltype(m_iInputs)), 1, bfp);
    safe_write(&m_Bias, sizeof(decltype(m_Bias)), 1, bfp);
    safe_write(&m_LearnRate, sizeof(decltype(m_LearnRate)), 1, bfp);

    // Write inputs
    const unsigned inputCount = m_inputs.size();
    safe_write(&inputCount, sizeof(unsigned), 1, bfp);
    safe_write(m_inputs.data(), sizeof(ga_value), inputCount, bfp);

    // Write weights
    const unsigned weightCount = m_weights.size();
    safe_write(&weightCount, sizeof(unsigned), 1, bfp);
    safe_write(m_weights.data(), sizeof(ga_value), weightCount, bfp);

    // Write remaining data
    safe_write(&m_output, sizeof(decltype(m_output)), 1, bfp);
    safe_write(&m_bTrained, sizeof(decltype(m_bTrained)), 1, bfp);
}

void CPerceptron::load(std::FILE* bfp)
{
    if (!bfp || std::feof(bfp))
        return;

    const CGenericHeader header = CGenericHeader(LEARNTYPE_PERCEPTRON, m_iInputs);

    if (!CGenericHeader::read(bfp, header))
    {
        BotMessage(nullptr, 0, "Learn data version mismatch - wiping");
        return;
    }

    // Helper lambda for fread with error checking
    auto safe_read = [](void* data, const size_t size, const size_t count, std::FILE* file) {
        if (std::fread(data, size, count, file) != count) {
            BotMessage(nullptr, 0, "Error reading from file");
            return false;
        }
        return true;
    };

    // Read perceptron data
    if (!safe_read(&m_iInputs, sizeof(decltype(m_iInputs)), 1, bfp) ||
        !safe_read(&m_Bias, sizeof(decltype(m_Bias)), 1, bfp) ||
        !safe_read(&m_LearnRate, sizeof(decltype(m_LearnRate)), 1, bfp))
    {
        return;
    }

    // Read inputs
    unsigned inputCount = 0;
    if (!safe_read(&inputCount, sizeof(unsigned), 1, bfp)) return;
    m_inputs.resize(inputCount);
    if (!safe_read(m_inputs.data(), sizeof(ga_value), inputCount, bfp)) return;

    // Read weights
    unsigned weightCount = 0;
    if (!safe_read(&weightCount, sizeof(unsigned), 1, bfp)) return;
    m_weights.resize(weightCount);
    if (!safe_read(m_weights.data(), sizeof(ga_value), weightCount, bfp)) return;

    // Read remaining data
    if (!safe_read(&m_output, sizeof(decltype(m_output)), 1, bfp) ||
        !safe_read(&m_bTrained, sizeof(decltype(m_bTrained)), 1, bfp))
    {
        // Error reading remaining data
    }
}

void CPerceptron::load(const char* filename, const int iProfileId)
{
    if (std::FILE* bfp = RCBOpenFile(filename, "rb", SAVETYPE_BOT, iProfileId))
    {
        load(bfp);
        std::fclose(bfp);
    }
    else
    {
        BotMessage(nullptr, 0, "Failed to open file for reading");
    }
}

void CPerceptron::save(const char* filename, const int iProfileId) const
{
    if (std::FILE* bfp = RCBOpenFile(filename, "wb", SAVETYPE_BOT, iProfileId))
    {
        save(bfp);
        std::fclose(bfp);
    }
    else
    {
        BotMessage(nullptr, 0, "Failed to open file for writing");
    }
}