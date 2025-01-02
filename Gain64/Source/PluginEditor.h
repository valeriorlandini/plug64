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

#pragma once

#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class Gain64AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Gain64AudioProcessorEditor(Gain64AudioProcessor&);
    ~Gain64AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Gain64AudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::Rectangle<float> logoBounds;
    juce::Label title;
    juce::Label masterGainLabel;
    juce::Slider masterGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAttachment;
    juce::Label chGainLabel;
    juce::ComboBox selectChBox;
    juce::Slider selectChSlider;
    std::array<juce::Slider, 64> chGainSliders;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 64> chGainAttachments;

    juce::Typeface::Ptr customTypeface;
    juce::Font customFont;
    float fontSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Gain64AudioProcessorEditor)
};
