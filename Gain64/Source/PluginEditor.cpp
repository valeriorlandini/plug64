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

#include "PluginEditor.h"

Gain64AudioProcessorEditor::Gain64AudioProcessorEditor(Gain64AudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(500, 280);
    setResizeLimits(400, 224, 3000, 1680);
    setResizable(true, p.wrapperType != Gain64AudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(5.0f/2.8f);

    getLookAndFeel().setColour(juce::Label::textColourId, customLookAndFeel.textColour);
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    header.setText("Plug64", juce::dontSendNotification);
    addAndMakeVisible(header);
    title.setText("GAIN64", juce::dontSendNotification);
    addAndMakeVisible(title);

    resetLabel.setText("RESET CH PARAMS", juce::dontSendNotification);
    addAndMakeVisible(resetLabel);

    juce::Path resetShape;
    resetShape.addRectangle(juce::Rectangle<int>(0, 0, 20, 20));
    resetButton.setShape(resetShape, false, true, false);
    addAndMakeVisible(resetButton);
    resetButton.onClick = [this]
    {
        for (int ch = 0; ch < MAX_CHANS; ++ch)
        {
            auto paramID = "chgain" + std::to_string(ch+1);
            if (auto* param = audioProcessor.treeState.getParameter(paramID))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost(param->convertTo0to1(0.0f));
                param->endChangeGesture();
            }
        }
    };

    masterLabel.setText("MASTER", juce::dontSendNotification);
    masterLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterLabel);

    masterGainLabel.setText("GAIN", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterGainLabel);

    masterGainSlider.setLookAndFeel(&customLookAndFeel);
    masterGainSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.mainMasterSliderColour);
    masterGainSlider.setSliderStyle(juce::Slider::LinearBar);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterGainSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterGainSlider);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastergain", masterGainSlider);
    masterGainSlider.setTextValueSuffix(" dB");

    chLabel.setText("CHAN", juce::dontSendNotification);
    chLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chLabel);

    chGainLabel.setText("GAIN", juce::dontSendNotification);
    chGainLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chGainLabel);

    selectChBox.setLookAndFeel(&customLookAndFeel);
    selectChBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    selectChBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    selectChBox.setScrollWheelEnabled(true);
    addAndMakeVisible(selectChBox);
    for (auto ch = 1; ch <= MAX_CHANS; ++ch)
    {
        selectChBox.addItem(std::to_string(ch), ch);
    }
    selectChBox.onChange = [this]
    {
        audioProcessor.selChannel = selectChBox.getSelectedId();
        resized();
    };
    selectChBox.setSelectedId(int(audioProcessor.selChannel.getValue()) != 0 ? int(audioProcessor.selChannel.getValue()) : 1);

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        chGainSliders[ch].setLookAndFeel(&customLookAndFeel);
        chGainSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.mainChSliderColour);
        chGainSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chGainSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chGainSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chGainSliders[ch]);
        auto paramID = "chgain" + std::to_string(ch+1);
        chGainAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chGainSliders[ch]);
        chGainSliders[ch].setTextValueSuffix(" dB");
    }
}

Gain64AudioProcessorEditor::~Gain64AudioProcessorEditor()
{
}

void Gain64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(customLookAndFeel.backgroundColour);
    g.setColour(customLookAndFeel.lineColour);
    auto width = static_cast<float>(getWidth());
    g.drawLine(width * 0.05f, width * 0.2f, width * 0.95f, width * 0.2f, width * 0.004f);
}

void Gain64AudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil((float)getWidth() / 16.0f);
    fontSize = (float)blockUI * 0.75f;
    customFont = customFont.withHeight(fontSize);

    header.setJustificationType(juce::Justification::left);
    header.setBounds(blockUI, 0, blockUI * 14, blockUI * 2);
    header.setFont(customFont.withHeight(fontSize));
    title.setJustificationType(juce::Justification::left);
    title.setBounds(blockUI, blockUI, blockUI * 14, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));

    resetLabel.setJustificationType(juce::Justification::centredLeft);
    resetLabel.setBounds(blockUI * 10, blockUI, blockUI * 14, blockUI * 2);
    resetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    resetButton.setSize((int)((float)blockUI * 0.5f), (int)((float)blockUI * 0.5f));
    resetButton.setCentrePosition(resetLabel.getX() - (int)((float)blockUI * 0.55f), resetLabel.getY() + (int)((float)resetLabel.getHeight() * 0.5f));

    masterLabel.setJustificationType(juce::Justification::centredLeft);
    masterLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    masterLabel.setFont(customFont.withHeight(fontSize));

    masterGainLabel.setJustificationType(juce::Justification::bottomLeft);
    masterGainLabel.setBounds(blockUI * 5, blockUI * 3, blockUI * 3, blockUI);
    masterGainLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterGainSlider.setBounds(blockUI * 5, blockUI * 4, blockUI * 10, blockUI);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chLabel.setJustificationType(juce::Justification::centredLeft);
    chLabel.setFont(customFont.withHeight(fontSize));
    chLabel.setBounds(blockUI, blockUI * 7, blockUI * 2, blockUI);

    chGainLabel.setJustificationType(juce::Justification::bottomLeft);
    chGainLabel.setBounds(blockUI * 5, blockUI * 6, blockUI * 3, blockUI);
    chGainLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    selectChBox.setBounds((int)((float)blockUI * 2.5f), blockUI * 7, (int)((float)blockUI * 1.5f), blockUI);

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        if ((unsigned int)(selectChBox.getSelectedId()) - 1 == ch)
        {
            chGainSliders[ch].setBounds(blockUI * 5, blockUI * 7, blockUI * 10, blockUI);
            chGainSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);
        }
        else
        {
            chGainSliders[ch].setBounds(0, 0, 0, 0);
        }
    }
}
