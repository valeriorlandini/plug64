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

class Delay64AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Delay64AudioProcessorEditor(Delay64AudioProcessor&);
    ~Delay64AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Delay64AudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::Label header;
    juce::Label title;
    juce::Label resetLabel;
    juce::ShapeButton resetButton{"reset", juce::Colour(243, 255, 148), juce::Colour(243, 255, 148), juce::Colour(214, 108, 87)};
    juce::Line<int> headerLine;
    juce::Label masterLabel;
    juce::Label masterSyncLabel;
    juce::Label masterFeedbackLabel;
    juce::Label masterWetLabel;
    juce::Label masterTimeLabel;
    juce::Slider masterTimeSlider;
    juce::Slider masterFeedbackSlider;
    juce::Slider masterWetSlider;
    juce::ComboBox masterSyncBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> masterSyncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterWetAttachment;
    juce::Label chLabel;
    juce::Label chTimeLabel;
    juce::Label chWetLabel;
    juce::Label chSyncLabel;
    juce::Label chFeedbackLabel;
    juce::ComboBox selectChBox;
    juce::Slider selectChSlider;
    std::array<juce::Slider, MAX_CHANS> chTimeSliders;
    std::array<juce::Slider, MAX_CHANS> chFeedbackSliders;
    std::array<juce::Slider, MAX_CHANS> chWetSliders;
    std::array<juce::ComboBox, MAX_CHANS> chSyncBoxes;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, MAX_CHANS> chSyncAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chTimeAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chFeedbackAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, MAX_CHANS> chWetAttachments;

    juce::Typeface::Ptr customTypeface;
    juce::Font customFont;
    float fontSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay64AudioProcessorEditor)
};
