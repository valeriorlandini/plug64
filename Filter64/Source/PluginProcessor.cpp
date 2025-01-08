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

Filter64AudioProcessor::Filter64AudioProcessor() :
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
    treeState(*this, nullptr, juce::Identifier("Filter64Parameters"),
              [&]()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("mastertype", "Master Filter", juce::NormalisableRange<float>(0.0f, 6.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("mastercutoff", "Master Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.4f, false), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterresonance", "Master Resonance", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterdrive", "Master Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        std::string parameterID, parameterName;

        parameterID = "chtype" + ch_str;
        parameterName = "Channel " + ch_str + " Filter";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName,juce::NormalisableRange<float>(0.0f, 6.0f, 1.0f), 0.0f));

        parameterID = "chcutoff" + ch_str;
        parameterName = "Channel " + ch_str + " Cutoff";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.4f, false), 20000.0f));

        parameterID = "chresonance" + ch_str;
        parameterName = "Channel " + ch_str + " Resonance";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 5.0f));

        parameterID = "chdrive" + ch_str;
        parameterName = "Channel " + ch_str + " Drive";
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

    masterTypeParameter = treeState.getRawParameterValue("mastertype");
    masterCutoffParameter = treeState.getRawParameterValue("mastercutoff");
    masterResonanceParameter = treeState.getRawParameterValue("masterresonance");
    masterDriveParameter = treeState.getRawParameterValue("masterdrive");

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        chTypeParameters.at(ch) = treeState.getRawParameterValue("chtype" + ch_str);
        chCutoffParameters.at(ch) = treeState.getRawParameterValue("chcutoff" + ch_str);
        chResonanceParameters.at(ch) = treeState.getRawParameterValue("chresonance" + ch_str);
        chDriveParameters.at(ch) = treeState.getRawParameterValue("chdrive" + ch_str);
    }
}

Filter64AudioProcessor::~Filter64AudioProcessor()
{
}

const juce::String Filter64AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Filter64AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Filter64AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Filter64AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Filter64AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Filter64AudioProcessor::getNumPrograms()
{
    return 1;
}

int Filter64AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Filter64AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String Filter64AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);

    return {};
}

void Filter64AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void Filter64AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 1};

    for (unsigned int ch = 0; ch < (unsigned int)std::min(getTotalNumInputChannels(), MAX_CHANS); ++ch)
    {
        processorChains.at(ch).prepare(spec);

        updateParams();
    }
}

void Filter64AudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Filter64AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void Filter64AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::ignoreUnused(midiMessages);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    juce::dsp::AudioBlock<float> block(buffer);

    for (unsigned int ch = 0; ch < (unsigned int)totalNumInputChannels; ++ch)
    {
        if (ch < MAX_CHANS)
        {
            updateParams();
            auto channelBlock = block.getSingleChannelBlock(ch);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            processorChains.at(ch).process(context);
        }
    }
}

bool Filter64AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Filter64AudioProcessor::createEditor()
{
    return new Filter64AudioProcessorEditor(*this);
}

void Filter64AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Filter64AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new Filter64AudioProcessor();
}
