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
#include "soutel/include/soutel/ringmod.h"

class Ring64AudioProcessor : public juce::AudioProcessor
{
public:
    Ring64AudioProcessor();
    ~Ring64AudioProcessor() override;

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

    std::array<std::atomic<float>*, MAX_CHANS> chModParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chFreqParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chModChParameters = {nullptr};
    std::array<std::atomic<float>*, MAX_CHANS> chMixParameters = {nullptr};
    std::atomic<float>* masterModParameter = nullptr;
    std::atomic<float>* masterFreqParameter = nullptr;
    std::atomic<float>* masterModChParameter = nullptr;
    std::atomic<float>* masterMixParameter = nullptr;

    juce::Value selChannel;

private:
    std::array<soutel::RingMod<float>, MAX_CHANS> chRings;
    std::array<soutel::RingMod<float>, MAX_CHANS> masterRings;

    inline void updateParams()
    {
        soutel::RModulators masterModulator = soutel::RModulators::oscillator;
        soutel::BLWaveforms masterWaveform = soutel::BLWaveforms::sine;
        bool masterAm = false;

        int masterMod = static_cast<int>(*(masterModParameter));
        switch (masterMod)
        {
            case 0:
                masterModulator = soutel::RModulators::oscillator;
                masterWaveform = soutel::BLWaveforms::sine;
                break;
            case 1:
                masterModulator = soutel::RModulators::oscillator;
                masterWaveform = soutel::BLWaveforms::triangle;
                break;
            case 2:
                masterModulator = soutel::RModulators::oscillator;
                masterWaveform = soutel::BLWaveforms::sine;
                masterAm = true;
                break;
            case 3:
                masterModulator = soutel::RModulators::oscillator;
                masterWaveform = soutel::BLWaveforms::triangle;
                masterAm = true;
                break;
            case 4:
                masterModulator = soutel::RModulators::input;
                break;
        }

        for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
        {
            soutel::RModulators chModulator = soutel::RModulators::oscillator;
            soutel::BLWaveforms chWaveform = soutel::BLWaveforms::sine;
            bool chAm = false;

            int chMod = static_cast<int>(*(chModParameters.at(ch)));
            switch (chMod)
            {
                case 0:
                    chModulator = soutel::RModulators::oscillator;
                    chWaveform = soutel::BLWaveforms::sine;
                    break;
                case 1:
                    chModulator = soutel::RModulators::oscillator;
                    chWaveform = soutel::BLWaveforms::triangle;
                    break;
                case 2:
                    chModulator = soutel::RModulators::oscillator;
                    chWaveform = soutel::BLWaveforms::sine;
                    chAm = true;
                    break;
                case 3:
                    chModulator = soutel::RModulators::oscillator;
                    chWaveform = soutel::BLWaveforms::triangle;
                    chAm = true;
                    break;
                case 4:
                    chModulator = soutel::RModulators::input;
                    break;
            }

            chRings.at(ch).set_modulator(chModulator);
            chRings.at(ch).set_modulator_wave(chWaveform);
            chRings.at(ch).set_am(chAm);
            chRings.at(ch).set_frequency(*(chFreqParameters.at(ch)));

            masterRings.at(ch).set_modulator(masterModulator);
            masterRings.at(ch).set_modulator_wave(masterWaveform);
            masterRings.at(ch).set_am(masterAm);
            masterRings.at(ch).set_frequency(*masterFreqParameter);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ring64AudioProcessor)
};
