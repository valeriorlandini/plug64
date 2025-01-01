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
    //getLookAndFeel().setColour(juce::Label::backgroundColourId, juce::Colours::darkslategrey);
    //getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    title.setText("Gain 64", juce::dontSendNotification);
    addAndMakeVisible(title);

    masterGainLabel.setText("MASTER", juce::dontSendNotification);
    addAndMakeVisible(masterGainLabel);

    masterGainSlider.setLookAndFeel(&customLookAndFeel);
    masterGainSlider.setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    masterGainSlider.setSliderStyle(juce::Slider::LinearBar);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterGainSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterGainSlider);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastergain", masterGainSlider);

    chGainLabel.setJustificationType(juce::Justification::centred);
    chGainLabel.setText("CHANNEL", juce::dontSendNotification);
    addAndMakeVisible(chGainLabel);

    selectChSlider.setLookAndFeel(&customLookAndFeel);
    selectChSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    selectChSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(101, 142, 162));
    selectChSlider.setSliderStyle(juce::Slider::LinearBar);
    selectChSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    selectChSlider.setRange(1, 64, 1);
    selectChSlider.setPopupDisplayEnabled(false, false, this);
    selectChSlider.addListener(this);
    addAndMakeVisible(selectChSlider);

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
    }
}

Gain64AudioProcessorEditor::~Gain64AudioProcessorEditor()
{
}

void Gain64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(44, 48, 52));
}

void Gain64AudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil(getWidth() / 16.0f);
    fontSize = (float)blockUI * 0.75f;
    customFont = customFont.withHeight(fontSize);

    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI, blockUI, blockUI * 14, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));

    masterGainLabel.setJustificationType(juce::Justification::centred);
    masterGainLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    masterGainLabel.setFont(customFont.withHeight(fontSize));
    masterGainSlider.setBounds(blockUI * 5, blockUI * 4, blockUI * 10, blockUI);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chGainLabel.setJustificationType(juce::Justification::centred);
    chGainLabel.setFont(customFont.withHeight(fontSize));
    chGainLabel.setBounds(blockUI, blockUI * 6, blockUI * 3, blockUI);
    selectChSlider.setBounds(blockUI * 5, blockUI * 6, blockUI * 2, blockUI);
    selectChSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 2, blockUI);
    for (unsigned int c = 0; c < 64; c++)
    {
        if ((unsigned int)(selectChSlider.getValue()) - 1 == c)
        {
            chGainSliders[c].setBounds(blockUI * 7, blockUI * 6, blockUI * 8, blockUI);    
            chGainSliders[c].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);
        }
        else
        {
            chGainSliders[c].setBounds(0, 0, 0, 0);
        }
    }
}
