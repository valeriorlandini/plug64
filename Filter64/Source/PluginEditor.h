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

class Filter64AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Filter64AudioProcessorEditor(Filter64AudioProcessor&);
    ~Filter64AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Filter64AudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::Label header;
    juce::Label title;
    juce::Label resetLabel;
    juce::ShapeButton resetButton{"reset", juce::Colour(243, 255, 148), juce::Colour(243, 255, 148), juce::Colour(214, 108, 87)};
    juce::Line<int> headerLine;
    juce::Label masterLabel;
    juce::Label masterTypeLabel;
    juce::Label masterResonanceLabel;
    juce::Label masterDriveLabel;
    juce::Label masterCutoffLabel;
    juce::Slider masterCutoffSlider;
    juce::Slider masterResonanceSlider;
    juce::Slider masterDriveSlider;
    juce::ComboBox masterFilterBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> masterFilterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterDriveAttachment;
    juce::Label chLabel;
    juce::Label chCutoffLabel;
    juce::Label chDriveLabel;
    juce::Label chTypeLabel;
    juce::Label chResonanceLabel;
    juce::ComboBox selectChBox;
    juce::Slider selectChSlider;
    std::array<juce::Slider, 64> chCutoffSliders;
    std::array<juce::Slider, 64> chResonanceSliders;
    std::array<juce::Slider, 64> chDriveSliders;
    std::array<juce::ComboBox, 64> chFilterBoxes;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 64> chFilterAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 64> chCutoffAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 64> chResonanceAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 64> chDriveAttachments;

    juce::Typeface::Ptr customTypeface;
    juce::Font customFont;
    float fontSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filter64AudioProcessorEditor)
};
