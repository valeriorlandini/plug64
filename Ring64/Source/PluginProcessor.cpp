/******************************************************************************
This file is part of Plug64.
Copyright 2024-2025 Valerio Orlandini <valeriorlandini@gmail.com>.

Plug64 is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

Plug64 is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
Plug64. If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

Ring64AudioProcessor::Ring64AudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                   .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                   .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ),
#endif
    treeState(*this, nullptr, juce::Identifier("Ring64Parameters"),
              [&]()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("mastermod", "Master Modulator", juce::NormalisableRange<float>(0.0f, 4.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterfreq", "Master Frequency", juce::NormalisableRange<float>(0.0f, 20000.0f, 1.0f), 440.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("mastermodch", "Master Mod Channel", juce::NormalisableRange<float>(1.0f, 64.0f, 1.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterwet", "Master Wet", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        std::string parameterID, parameterName;

        parameterID = "chmod" + ch_str;
        parameterName = "Channel " + ch_str + " Modulator";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName,juce::NormalisableRange<float>(0.0f, 4.0f, 1.0f), 0.0f));

        parameterID = "chfreq" + ch_str;
        parameterName = "Channel " + ch_str + " Frequency";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(0.0f, 20000.0f, 1.0f), 440.0f));

        parameterID = "chmodch" + ch_str;
        parameterName = "Channel " + ch_str + " Mod Channel";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(1.0f, 64.0f, 1.0f), (float)ch+1));

        parameterID = "chwet" + ch_str;
        parameterName = "Channel " + ch_str + " Wet";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));
    }

    return layout;
}
()
         )
{
    if (!treeState.state.hasProperty("selChannel"))
    {
        treeState.state.setProperty("selchannel", 1, nullptr);
    }

    masterModParameter = treeState.getRawParameterValue("mastermod");
    masterFreqParameter = treeState.getRawParameterValue("masterfreq");
    masterModChParameter = treeState.getRawParameterValue("mastermodch");
    masterMixParameter = treeState.getRawParameterValue("masterwet");

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        chModParameters.at(ch) = treeState.getRawParameterValue("chmod" + ch_str);
        chFreqParameters.at(ch) = treeState.getRawParameterValue("chfreq" + ch_str);
        chModChParameters.at(ch) = treeState.getRawParameterValue("chmodch" + ch_str);
        chMixParameters.at(ch) = treeState.getRawParameterValue("chwet" + ch_str);
    }

    updateParams();
}

Ring64AudioProcessor::~Ring64AudioProcessor()
{
}

const juce::String Ring64AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Ring64AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Ring64AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Ring64AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Ring64AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Ring64AudioProcessor::getNumPrograms()
{
    return 1;
}

int Ring64AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Ring64AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String Ring64AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);

    return {};
}

void Ring64AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void Ring64AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    for (unsigned int ch = 0; ch < (unsigned int)std::min(getTotalNumInputChannels(), MAX_CHANS); ++ch)
    {
        chRings.at(ch).set_sample_rate(static_cast<float>(sampleRate));
        masterRings.at(ch).set_sample_rate(static_cast<float>(sampleRate));
    }

    updateParams();
}

void Ring64AudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Ring64AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void Ring64AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::ignoreUnused(midiMessages);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    updateParams();

    juce::AudioBuffer<float> tempBuffer(totalNumOutputChannels, buffer.getNumSamples());

    std::vector<const float*> inputChannelPointers(totalNumInputChannels);
    for (auto ch = 0; ch < totalNumInputChannels; ++ch)
    {
        inputChannelPointers[ch] = buffer.getReadPointer(ch);
    }

    for (unsigned int ch = 0; ch < static_cast<unsigned int>(totalNumInputChannels); ++ch)
    {
        auto* channelData = buffer.getReadPointer(static_cast<int>(ch));
        auto* tempChannelData = tempBuffer.getWritePointer(static_cast<int>(ch));

        if (ch < MAX_CHANS)
        {
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
            {
                // Channel ring modulation
                float chmod = 0.0f;
                if (static_cast<int>(*(chModParameters.at(ch))) == 4)
                {
                    int chModCh = static_cast<int>(*(chModChParameters.at(ch))) - 1;
                    if (chModCh < totalNumInputChannels)
                    {
                        chmod = inputChannelPointers[chModCh][i];
                    }
                }
                const float chRinged = chRings.at(ch).run(channelData[i], chmod);
                auto chSample = chRinged * (*(chMixParameters.at(ch)) * 0.01f) + channelData[i] * (1.0f - (*(chMixParameters.at(ch)) * 0.01f));

                // Master ring modulation
                float mastermod = 0.0f;
                if (static_cast<int>(*masterModParameter) == 4)
                {
                    int masterModCh = static_cast<int>(*masterModChParameter) - 1;
                    if (masterModCh < totalNumInputChannels)
                    {
                        mastermod = inputChannelPointers[masterModCh][i];
                    }
                }
                const float masterRinged = masterRings.at(ch).run(chSample, mastermod);
                tempChannelData[i] = masterRinged * (*masterMixParameter * 0.01f) + chSample * (1.0f - (*masterMixParameter * 0.01f));
            }
        }
        else
        {
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
            {
                tempChannelData[i] = channelData[i];
            }
        }
    }

    for (unsigned int ch = 0; ch < static_cast<unsigned int>(totalNumOutputChannels); ++ch)
    {
        auto* channelData = buffer.getWritePointer(static_cast<int>(ch));
        const auto* tempChannelData = tempBuffer.getReadPointer(static_cast<int>(ch));

        for (auto i = 0; i < buffer.getNumSamples(); ++i)
        {
            channelData[i] = tempChannelData[i];
        }
    }

}

bool Ring64AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Ring64AudioProcessor::createEditor()
{
    return new Ring64AudioProcessorEditor(*this);
}

void Ring64AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Ring64AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(treeState.state.getType()))
        {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));

            if (treeState.state.hasProperty("selchannel"))
            {
                selChannel.referTo(treeState.state.getPropertyAsValue("selchannel", nullptr));
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Ring64AudioProcessor();
}
