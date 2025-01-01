/******************************************************************************
This file is part of Plug64.
Copyright 2024 Valerio Orlandini <valeriorlandini@gmail.com>.

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

Gain64AudioProcessor::Gain64AudioProcessor() :
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
    treeState(*this, nullptr, juce::Identifier("Gain64Parameters"),
              [&]() {
                  juce::AudioProcessorValueTreeState::ParameterLayout layout;

                  layout.add(std::make_unique<juce::AudioParameterFloat>("mastergain", "Master Gain", -70.0f, 12.0f, 0.0f));

                  for (unsigned int i = 0; i < 64; i++)
                  {
                      auto parameterID = "chgain" + std::to_string(i+1);
                      auto parameterName = "Channel Gain " + std::to_string(i+1);
                      layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, -70.0f, 12.0f, 0.0f));
                  }

                  return layout;
              }()
    )
{
    masterGainParameter = treeState.getRawParameterValue("mastergain");

    for (unsigned int ch = 0; ch < 64; ++ch)
    {
        chGainParameters.at(ch) = treeState.getRawParameterValue("chgain" + std::to_string(ch+1));
        processorChains.at(ch).get<0>().setRampDurationSeconds(0.05);
        processorChains.at(ch).get<1>().setRampDurationSeconds(0.05);
    }
}

Gain64AudioProcessor::~Gain64AudioProcessor()
{
}

const juce::String Gain64AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Gain64AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Gain64AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Gain64AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Gain64AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Gain64AudioProcessor::getNumPrograms()
{
    return 1;
}

int Gain64AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Gain64AudioProcessor::setCurrentProgram(int index)
{
    index = 0;

    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
}

const juce::String Gain64AudioProcessor::getProgramName(int index)
{
    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
    return {};
}

void Gain64AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
    auto dummy = newName;
}

void Gain64AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 1};

    for (unsigned int ch = 0; ch < (unsigned int)std::min(getTotalNumInputChannels(), 64); ch++)
    {
        processorChains.at(ch).get<0>().setGainDecibels(*masterGainParameter);
        processorChains.at(ch).get<1>().setGainDecibels(*(chGainParameters.at(ch)));
        processorChains.at(ch).prepare(spec);
    }
}

void Gain64AudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Gain64AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void Gain64AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (midiMessages.isEmpty())
    {
        // Dummy, to avoid warnings from some compilers
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    juce::dsp::AudioBlock<float> block(buffer);

    for (unsigned int ch = 0; ch < (unsigned int)totalNumInputChannels; ++ch)
    {
        if (ch < 64)
        {
            processorChains.at(ch).get<0>().setGainDecibels(*masterGainParameter);
            processorChains.at(ch).get<1>().setGainDecibels(*(chGainParameters.at(ch)));
            auto channelBlock = block.getSingleChannelBlock(ch);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            processorChains.at(ch).process(context);
        }
    }
}

bool Gain64AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Gain64AudioProcessor::createEditor()
{
    return new Gain64AudioProcessorEditor(*this);
}

void Gain64AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Gain64AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(treeState.state.getType()))
        {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Gain64AudioProcessor();
}