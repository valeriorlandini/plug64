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

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;
    void drawPopupMenuBackground(juce::Graphics& g, int, int) override;
    juce::Path getTickShape(float) override;
    void drawPopupMenuUpDownArrow(juce::Graphics&, int, int, bool) override {}
    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                      int, int, int, int, juce::ComboBox& box) override;
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label&) override;
    juce::Font customFont;

    const juce::Colour textColour = juce::Colours::whitesmoke;
    const juce::Colour backgroundColour = juce::Colour(44, 48, 52);
    const juce::Colour lineColour = juce::Colour(224, 224, 224);
    const juce::Colour mainMasterSliderColour = juce::Colour(66, 90, 166);
    const juce::Colour mainChSliderColour = juce::Colour(145, 164, 228);
    const juce::Colour otherMasterSliderColour = juce::Colour(110, 82, 109);
    const juce::Colour otherChSliderColour = juce::Colour(174, 140, 173);

private:
    float comboFontSize = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};
