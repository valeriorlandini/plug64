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
#include "soutel/include/soutel/delay.h"

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
    std::array<float, MAX_CHANS> chDelayTimes = {1000.0f};
    std::atomic<float>* masterSyncParameter = nullptr;
    std::atomic<float>* masterTimeParameter = nullptr;
    std::atomic<float>* masterFeedbackParameter = nullptr;
    std::atomic<float>* masterMixParameter = nullptr;
    float masterDelayTime = 1000.0f;

    juce::Value selChannel;

private:
    std::array<soutel::Delay<float>, MAX_CHANS> chDelays;
    std::array<soutel::Delay<float>, MAX_CHANS> masterDelays;
    std::array<juce::SmoothedValue<float>, MAX_CHANS> masterTimeSmoothers;
    std::array<juce::SmoothedValue<float>, MAX_CHANS> chTimeSmoothers;
    float bpm = 0.0f;
    juce::AudioPlayHead::PositionInfo posInfo;

    inline void updateParams(const bool &onlySmoothers = false)
    {
        if (!onlySmoothers)
        {
            auto masterSync = static_cast<int>(*masterSyncParameter);
            if (masterSync == 0 || bpm < 1.0)
            {
                masterDelayTime = *masterTimeParameter;
            }
            else
            {
                masterDelayTime = (60000.0f / (bpm * 4.0f)) * static_cast<float>(masterSync);
            }
        }

        for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
        {
            if (!onlySmoothers)
            {
                auto chSync = static_cast<int>(*(chSyncParameters.at(ch)));
                if (chSync == 0 || bpm < 1.0)
                {
                    chDelayTimes.at(ch) = *(chTimeParameters.at(ch));
                }
                else
                {
                    chDelayTimes.at(ch) = (60000.0f / (bpm * 4.0f)) * static_cast<float>(chSync);
                }

                if (!juce::exactlyEqual(chTimeSmoothers.at(ch).getTargetValue(), chDelayTimes.at(ch)))
                {
                    chTimeSmoothers.at(ch).setTargetValue(chDelayTimes.at(ch));
                }

                chDelays.at(ch).set_feedback(*(chFeedbackParameters.at(ch)) * 0.01f);


                if (!juce::exactlyEqual(masterTimeSmoothers.at(ch).getTargetValue(), masterDelayTime))
                {
                    masterTimeSmoothers.at(ch).setTargetValue(masterDelayTime);
                }

                masterDelays.at(ch).set_feedback(*masterFeedbackParameter * 0.01f);
            }

            masterDelays.at(ch).set_time(masterTimeSmoothers.at(ch).getNextValue());
            chDelays.at(ch).set_time(chTimeSmoothers.at(ch).getNextValue());
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay64AudioProcessor)
};
