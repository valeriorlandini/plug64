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

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel() :
    customFont(juce::Font(juce::FontOptions().withTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize))))
{
}

CustomLookAndFeel::~CustomLookAndFeel()
{
}

juce::Label* CustomLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    // Create a custom label for the slider's text box
    auto* label = new juce::Label();
    label->setJustificationType(juce::Justification::centred);

    float fontSize = slider.getHeight() * 0.75f;
    label->setFont(customFont.withHeight(fontSize));

    label->setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    label->setColour(juce::Label::textWhenEditingColourId, juce::Colours::whitesmoke);

    return label;
}
