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

Delay64AudioProcessor::Delay64AudioProcessor() :
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
    treeState(*this, nullptr, juce::Identifier("Delay64Parameters"),
              [&]()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("mastersync", "Master Sync", juce::NormalisableRange<float>(0.0f, 10.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("mastertime", "Master Time", juce::NormalisableRange<float>(0.0f, 5000.0f, 1.0f), 1000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterfeedback", "Master Feedback", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 25.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("masterwet", "Master Wet", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 25.0f));

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        std::string parameterID, parameterName;

        parameterID = "chsync" + ch_str;
        parameterName = "Channel " + ch_str + " Sync";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName,juce::NormalisableRange<float>(0.0f, 10.0f, 1.0f), 0.0f));

        parameterID = "chtime" + ch_str;
        parameterName = "Channel " + ch_str + " Time";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(0.0f, 5000.0f, 1.0f), 1000.0f));

        parameterID = "chfeedback" + ch_str;
        parameterName = "Channel " + ch_str + " Feedback";
        layout.add(std::make_unique<juce::AudioParameterFloat>(parameterID, parameterName, juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f));

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

    masterSyncParameter = treeState.getRawParameterValue("mastersync");
    masterTimeParameter = treeState.getRawParameterValue("mastertime");
    masterFeedbackParameter = treeState.getRawParameterValue("masterfeedback");
    masterMixParameter = treeState.getRawParameterValue("masterwet");

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        std::string ch_str = std::to_string(ch+1);
        chSyncParameters.at(ch) = treeState.getRawParameterValue("chsync" + ch_str);
        chTimeParameters.at(ch) = treeState.getRawParameterValue("chtime" + ch_str);
        chFeedbackParameters.at(ch) = treeState.getRawParameterValue("chfeedback" + ch_str);
        chMixParameters.at(ch) = treeState.getRawParameterValue("chwet" + ch_str);
    }

    updateParams();
}

Delay64AudioProcessor::~Delay64AudioProcessor()
{
}

const juce::String Delay64AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Delay64AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Delay64AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Delay64AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Delay64AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Delay64AudioProcessor::getNumPrograms()
{
    return 1;
}

int Delay64AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Delay64AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String Delay64AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);

    return {};
}

void Delay64AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void Delay64AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 1};

    for (unsigned int ch = 0; ch < (unsigned int)std::min(getTotalNumInputChannels(), MAX_CHANS); ++ch)
    {
        processorChains.at(ch).prepare(spec);
        processorChains.at(ch).get<0>().setMaximumDelayInSamples(5 * static_cast<int>(sampleRate));
        processorChains.at(ch).get<1>().setMaximumDelayInSamples(5 * static_cast<int>(sampleRate));
        chDelays.at(ch).set_sample_rate(static_cast<float>(sampleRate));
        masterDelays.at(ch).set_sample_rate(static_cast<float>(sampleRate));
        chDelays.at(ch).set_max_time(5000.0f, true);
        masterDelays.at(ch).set_max_time(5000.0f, true);
    }

    updateParams();
}

void Delay64AudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Delay64AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void Delay64AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    for (unsigned int ch = 0; ch < (unsigned int)totalNumInputChannels; ++ch)
    {   
        auto* channelData = buffer.getWritePointer(ch);

        if (ch < MAX_CHANS)
        {
            for (auto i = 0; i < buffer.getNumSamples(); ++i)
            {
                // Channel delay
                const float chDelayed = chDelays.at(ch).run(channelData[i]);
                chDelays.at(ch).set_time(chTimeSmoothers.at(ch).getNextValue());
                /*processorChains.at(ch).get<0>().pushSample(0, channelData[i] + chFeedbackGains.at(ch).processSample(chDelayed));*/
                auto chSample = chDelayed * (*(chMixParameters.at(ch)) * 0.01f) + channelData[i] * (1.0f - (*(chMixParameters.at(ch)) * 0.01f));

                // Master delay
                const float masterDelayed = masterDelays.at(ch).run(chSample);
                masterDelays.at(ch).set_time(masterTimeSmoothers.at(ch).getNextValue());
                /*processorChains.at(ch).get<1>().pushSample(0, chSample + masterFeedbackGain.processSample(masterDelayed));*/
                channelData[i] = masterDelayed * (*masterMixParameter * 0.01f) + chSample * (1.0f - (*masterMixParameter * 0.01f));
            }
        }
    }
}

bool Delay64AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Delay64AudioProcessor::createEditor()
{
    return new Delay64AudioProcessorEditor(*this);
}

void Delay64AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Delay64AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new Delay64AudioProcessor();
}
