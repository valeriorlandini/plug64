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

#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class Ring64AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Ring64AudioProcessorEditor(Ring64AudioProcessor&);
    ~Ring64AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Ring64AudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::Label header;
    juce::Label title;
    juce::Label resetLabel;
    juce::ShapeButton resetButton{"reset", juce::Colour(243, 255, 148), juce::Colour(243, 255, 148), juce::Colour(214, 108, 87)};
    juce::Line<int> headerLine;
    juce::Label masterLabel;
    juce::Label masterModLabel;
    juce::Label masterModChLabel;
    juce::Label masterWetLabel;
    juce::Label masterFreqLabel;
    juce::Slider masterFreqSlider;
    juce::Slider masterModChSlider;
    juce::Slider masterWetSlider;
    juce::ComboBox masterModBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> masterModAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterModChAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterWetAttachment;
    juce::Label chLabel;
    juce::Label chFreqLabel;
    juce::Label chWetLabel;
    juce::Label chModLabel;
    juce::Label chModChLabel;
    juce::ComboBox selectChBox;
    juce::Slider selectChSlider;
    std::array<juce::Slider, MAX_CHANS> chFreqSliders;
    std::array<juce::Slider, MAX_CHANS> chModChSliders;
    std::array<juce::Slider, MAX_CHANS> chWetSliders;
    std::array<juce::ComboBox, MAX_CHANS> chModBoxes;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, MAX_CHANS> chModAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chFreqAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chModChAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chWetAttachments;

    juce::Typeface::Ptr customTypeface;
    juce::Font customFont;
    float fontSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ring64AudioProcessorEditor)
};
