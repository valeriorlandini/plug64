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

class Filter64AudioProcessor : public juce::AudioProcessor
{
public:
    Filter64AudioProcessor();
    ~Filter64AudioProcessor() override;

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

    std::array<std::atomic<float>*, MAX_CHANS> chTypeParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chCutoffParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chResonanceParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chDriveParameters = {nullptr};
    std::atomic<float>* masterTypeParameter = nullptr;
    std::atomic<float>* masterCutoffParameter = nullptr;
    std::atomic<float>* masterResonanceParameter = nullptr;
    std::atomic<float>* masterDriveParameter = nullptr;

    juce::Value selChannel;

private:
    std::array<juce::dsp::ProcessorChain<juce::dsp::LadderFilter<float>, juce::dsp::LadderFilter<float>>, MAX_CHANS> processorChains;

    inline void updateParams()
    {
        for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
        {
            processorChains.at(ch).get<0>().setCutoffFrequencyHz(*(chCutoffParameters.at(ch)));
            processorChains.at(ch).get<0>().setResonance(*(chResonanceParameters.at(ch)) * 0.01f);
            float chDrive = *(chDriveParameters.at(ch));
            processorChains.at(ch).get<0>().setDrive(juce::jmap(chDrive, 0.0f, 100.0f, 1.0f, 10.0f));
            auto chFilterType = static_cast<int>(*(chTypeParameters.at(ch)));
            processorChains.at(ch).get<0>().setEnabled(chFilterType != 0);
            if (chFilterType > 0)
            {
                processorChains.at(ch).get<0>().setMode(static_cast<juce::dsp::LadderFilterMode>(chFilterType - 1));
            }

            processorChains.at(ch).get<1>().setCutoffFrequencyHz(*masterCutoffParameter);
            processorChains.at(ch).get<1>().setResonance(*masterResonanceParameter * 0.01f);
            float masterDrive = *masterDriveParameter;
            processorChains.at(ch).get<1>().setDrive(juce::jmap(masterDrive, 0.0f, 100.0f, 1.0f, 10.0f));
            auto masterFilterType = static_cast<int>(*masterTypeParameter);
            processorChains.at(ch).get<1>().setEnabled(masterFilterType != 0);
            if (masterFilterType > 0)
            {
                processorChains.at(ch).get<1>().setMode(static_cast<juce::dsp::LadderFilterMode>(masterFilterType - 1));
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filter64AudioProcessor)
};
