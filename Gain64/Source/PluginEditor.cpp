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

#include "PluginEditor.h"

Gain64AudioProcessorEditor::Gain64AudioProcessorEditor(Gain64AudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(600, 300);
    setResizeLimits(400, 200, 3000, 1500);
    setResizable(true, p.wrapperType != Gain64AudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(2.0f);

    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    header.setText("Plug64", juce::dontSendNotification);
    addAndMakeVisible(header);
    title.setText("GAIN64", juce::dontSendNotification);
    addAndMakeVisible(title);

    masterGainLabel.setText("MASTER", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterGainLabel);

    masterGainSlider.setLookAndFeel(&customLookAndFeel);
    masterGainSlider.setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    masterGainSlider.setSliderStyle(juce::Slider::LinearBar);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterGainSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterGainSlider);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastergain", masterGainSlider);
    masterGainSlider.setSkewFactor(1.5);
    masterGainSlider.setTextValueSuffix(" dB");

    chGainLabel.setText("CHAN", juce::dontSendNotification);
    chGainLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chGainLabel);

    selectChBox.setLookAndFeel(&customLookAndFeel);
    selectChBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    selectChBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    selectChBox.setScrollWheelEnabled(true);
    addAndMakeVisible(selectChBox);
    for (auto ch = 1; ch <= 64; ch++)
    {
        selectChBox.addItem(std::to_string(ch), ch);
    }
    selectChBox.onChange = [this] { resized(); };
    selectChBox.setSelectedId(1);


    for (unsigned int i = 0; i < 64; i++)
    {
        chGainSliders[i].setLookAndFeel(&customLookAndFeel);
        chGainSliders[i].setColour(juce::Slider::trackColourId, juce::Colour(153, 99, 134));
        chGainSliders[i].setSliderStyle(juce::Slider::LinearBar);
        chGainSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chGainSliders[i].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chGainSliders[i]);
        auto paramID = "chgain" + std::to_string(i+1);
        chGainAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chGainSliders[i]);
        chGainSliders[i].setTextValueSuffix(" dB");
    }
}

Gain64AudioProcessorEditor::~Gain64AudioProcessorEditor()
{
}

void Gain64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(44, 48, 52));
    resized();
}

void Gain64AudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil(getWidth() / 16.0f);
    fontSize = (float)blockUI * 0.75f;
    customFont = customFont.withHeight(fontSize);

    header.setJustificationType(juce::Justification::left);
    header.setBounds(blockUI, 0, blockUI * 14, blockUI * 2);
    header.setFont(customFont.withHeight(fontSize));
    title.setJustificationType(juce::Justification::left);
    title.setBounds(blockUI, blockUI, blockUI * 14, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));

    masterGainLabel.setJustificationType(juce::Justification::centredLeft);
    masterGainLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    masterGainLabel.setFont(customFont.withHeight(fontSize));
    masterGainSlider.setBounds(blockUI * 5, blockUI * 4, blockUI * 10, blockUI);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chGainLabel.setJustificationType(juce::Justification::centredLeft);
    chGainLabel.setFont(customFont.withHeight(fontSize));
    chGainLabel.setBounds(blockUI, blockUI * 6, blockUI * 2, blockUI);
    selectChBox.setBounds((int)((float)blockUI * 2.5f), blockUI * 6, (int)((float)blockUI * 1.5f), blockUI);
    for (unsigned int c = 0; c < 64; c++)
    {
        if ((unsigned int)(selectChBox.getSelectedId()) == c)
        {
            chGainSliders[c].setBounds(blockUI * 5, blockUI * 6, blockUI * 10, blockUI);    
            chGainSliders[c].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);
        }
        else
        {
            chGainSliders[c].setBounds(0, 0, 0, 0);
        }
    }
}
