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

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    juce::Font customFont;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};