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

#pragma once

#include <array>
#include <atomic>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"
#include "delay.h"

class Delay64AudioProcessor : public juce::AudioProcessor
{
public:
    Delay64AudioProcessor();
    ~Delay64AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState treeState;

    std::array<std::atomic<float>*, MAX_CHANS> chSyncParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chTimeParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chFeedbackParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chMixParameters = {nullptr};
    std::array<float, MAX_CHANS> chDelaySamples = {1.0f};
    std::atomic<float>* masterSyncParameter = nullptr;
    std::atomic<float>* masterTimeParameter = nullptr;
    std::atomic<float>* masterFeedbackParameter = nullptr;
    std::atomic<float>* masterMixParameter = nullptr;
    float masterDelaySamples = 1.0f;

    juce::Value selChannel;

private:
    std::array<soutel::Delay<float>, MAX_CHANS> chDelays;
    std::array<soutel::Delay<float>, MAX_CHANS> masterDelays;
    std::array<juce::dsp::ProcessorChain<juce::dsp::DelayLine<float>, juce::dsp::DelayLine<float>>, MAX_CHANS> processorChains;
    std::array<juce::dsp::Gain<float>, MAX_CHANS> chFeedbackGains;
    juce::dsp::Gain<float> masterFeedbackGain;
    std::array<juce::SmoothedValue<float>, MAX_CHANS> masterTimeSmoothers;
    std::array<juce::SmoothedValue<float>, MAX_CHANS> chTimeSmoothers;
    juce::AudioPlayHead::PositionInfo currPosInfo;

    inline void updateParams()
    {
        for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
        {
            auto chSync = static_cast<int>(*(chSyncParameters.at(ch)));
            auto bpm = currPosInfo.getBpm();
            if (chSync == 0 || !bpm.hasValue())
            {
                chDelaySamples.at(ch) = *(chTimeParameters.at(ch)) * static_cast<float>(getSampleRate()) * 0.001f;
            }
            else
            {
                chDelaySamples.at(ch) = 60000.0f / static_cast<float>(*bpm) * static_cast<float>(chSync) * static_cast<float>(getSampleRate()) * 0.001f;          
            }
            chTimeSmoothers.at(ch).setTargetValue(*(chTimeParameters.at(ch)));
            
            processorChains.at(ch).get<0>().setDelay(chDelaySamples.at(ch));
            chDelays.at(ch).set_feedback(*(chFeedbackParameters.at(ch)) * 0.01f);
            chFeedbackGains.at(ch).setGainLinear(*(chFeedbackParameters.at(ch)) * 0.01f);
            
            auto masterSync = static_cast<int>(*masterSyncParameter);
            if (masterSync == 0 || !bpm.hasValue())
            {
                masterDelaySamples = *masterTimeParameter * static_cast<float>(getSampleRate()) * 0.001f;
            }
            else
            {
                masterDelaySamples = 60000.0f / static_cast<float>(*bpm) * static_cast<float>(masterSync) * static_cast<float>(getSampleRate()) * 0.001f;
            }
            masterTimeSmoothers.at(ch).setTargetValue(*masterTimeParameter);
            
            processorChains.at(ch).get<1>().setDelay(masterDelaySamples);
            masterDelays.at(ch).set_feedback(*masterFeedbackParameter * 0.01f);
            masterFeedbackGain.setGainLinear(*masterFeedbackParameter * 0.01f);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay64AudioProcessor)
};
