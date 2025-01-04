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

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    comboFontSize = box.getHeight() * 0.75f;

    return customFont.withHeight(comboFontSize);
}

juce::Font CustomLookAndFeel::getPopupMenuFont()
{
    return customFont.withHeight(comboFontSize);
}

void CustomLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int, int)
{
    g.fillAll(juce::Colour(44, 48, 52));
}

juce::Path CustomLookAndFeel::getTickShape(float)
{
    juce::Path emptyPath;

    return emptyPath;
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                  int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat(), cornerSize, 0.0f);
}

void CustomLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(0, 0, box.getWidth(), box.getHeight());
    label.setFont(getComboBoxFont(box));
    label.setJustificationType(juce::Justification::centred);
}

juce::PopupMenu::Options CustomLookAndFeel::getOptionsForComboBoxPopupMenu (juce::ComboBox& box, juce::Label&)
{
    juce::PopupMenu::Options options;
    juce::Rectangle<int> bounds = box.getScreenBounds();
    return options.withTargetScreenArea(bounds);
}