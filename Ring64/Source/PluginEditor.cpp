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

Ring64AudioProcessorEditor::Ring64AudioProcessorEditor(Ring64AudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(500, 440);
    setResizeLimits(400, 352, 3000, 2640);
    setResizable(true, p.wrapperType != Ring64AudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(5.0f/4.4f);

    getLookAndFeel().setColour(juce::Label::textColourId, customLookAndFeel.textColour);
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    header.setText("Plug64", juce::dontSendNotification);
    header.setColour(juce::Label::textColourId, customLookAndFeel.backgroundColour);
    header.setColour(juce::Label::backgroundColourId, customLookAndFeel.textColour);
    addAndMakeVisible(header);

    title.setText("RING64", juce::dontSendNotification);
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
            std::string ch_str = std::to_string(ch+1);
            std::array<std::string, 4> paramIDs{"chmod" + ch_str, "chfreq" + ch_str, "chmodch" + ch_str, "chwet" + ch_str};

            for (auto paramID : paramIDs)
            {
                if (auto* param = audioProcessor.treeState.getParameter(paramID))
                {
                    param->beginChangeGesture();
                    param->setValueNotifyingHost(param->getDefaultValue());
                    param->endChangeGesture();
                }
            }
        }
    };

    masterLabel.setText("MASTER", juce::dontSendNotification);
    masterLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterLabel);

    masterFreqLabel.setText("FREQUENCY", juce::dontSendNotification);
    masterFreqLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterFreqLabel);

    masterModChLabel.setText("MODCH", juce::dontSendNotification);
    masterModChLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterModChLabel);

    masterWetLabel.setText("WET", juce::dontSendNotification);
    masterWetLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterWetLabel);

    masterModLabel.setText("MOD", juce::dontSendNotification);
    masterModLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterModLabel);

    masterFreqSlider.setLookAndFeel(&customLookAndFeel);
    masterFreqSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.mainMasterSliderColour);
    masterFreqSlider.setSliderStyle(juce::Slider::LinearBar);
    masterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterFreqSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterFreqSlider);
    masterFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterfreq", masterFreqSlider);
    masterFreqSlider.setTextValueSuffix(" Hz");

    masterModChSlider.setLookAndFeel(&customLookAndFeel);
    masterModChSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.otherMasterSliderColour);
    masterModChSlider.setSliderStyle(juce::Slider::LinearBar);
    masterModChSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterModChSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterModChSlider);
    masterModChAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastermodch", masterModChSlider);
    //masterModChSlider.setTextValueSuffix(" %");

    masterWetSlider.setLookAndFeel(&customLookAndFeel);
    masterWetSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.otherMasterSliderColour);
    masterWetSlider.setSliderStyle(juce::Slider::LinearBar);
    masterWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterWetSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterWetSlider);
    masterWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterwet", masterWetSlider);
    masterWetSlider.setTextValueSuffix(" %");

    masterModBox.setLookAndFeel(&customLookAndFeel);
    masterModBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    masterModBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    masterModBox.setScrollWheelEnabled(true);
    addAndMakeVisible(masterModBox);
    masterModBox.addItem("SINE", 1);
    masterModBox.addItem("TRIANGLE", 2);
    masterModBox.addItem("SINE AM", 3);
    masterModBox.addItem("TRI AM", 4);
    masterModBox.addItem("CH INPUT", 5);
    masterModAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, "mastermod", masterModBox);

    chLabel.setText("CHAN", juce::dontSendNotification);
    chLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chLabel);

    chFreqLabel.setText("FREQUENCY", juce::dontSendNotification);
    chFreqLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chFreqLabel);

    chModChLabel.setText("MODCH", juce::dontSendNotification);
    chModChLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chModChLabel);

    chWetLabel.setText("WET", juce::dontSendNotification);
    chWetLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chWetLabel);

    chModLabel.setText("MOD", juce::dontSendNotification);
    chModLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chModLabel);

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
        std::string ch_str = std::to_string(ch+1);
        std::string paramID;

        chFreqSliders[ch].setLookAndFeel(&customLookAndFeel);
        chFreqSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.mainChSliderColour);
        chFreqSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chFreqSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chFreqSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chFreqSliders[ch]);
        paramID = "chfreq" + ch_str;
        chFreqAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chFreqSliders[ch]);
        chFreqSliders[ch].setTextValueSuffix(" Hz");

        chModChSliders[ch].setLookAndFeel(&customLookAndFeel);
        chModChSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.otherChSliderColour);
        chModChSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chModChSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chModChSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chModChSliders[ch]);
        paramID = "chmodch" + ch_str;
        chModChAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chModChSliders[ch]);
        //chModChSliders[ch].setTextValueSuffix(" %");

        chWetSliders[ch].setLookAndFeel(&customLookAndFeel);
        chWetSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.otherChSliderColour);
        chWetSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chWetSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chWetSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chWetSliders[ch]);
        paramID = "chwet" + ch_str;
        chWetAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chWetSliders[ch]);
        chWetSliders[ch].setTextValueSuffix(" %");

        chModBoxes[ch].setLookAndFeel(&customLookAndFeel);
        chModBoxes[ch].setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
        chModBoxes[ch].setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        chModBoxes[ch].setScrollWheelEnabled(true);
        addAndMakeVisible(chModBoxes[ch]);
        chModBoxes[ch].addItem("SINE", 1);
        chModBoxes[ch].addItem("TRIANGLE", 2);
        chModBoxes[ch].addItem("SINE AM", 3);
        chModBoxes[ch].addItem("TRI AM", 4);
        chModBoxes[ch].addItem("CH INPUT", 5);
        paramID = "chmod" + ch_str;
        chModAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, paramID, chModBoxes[ch]);
    }
}

Ring64AudioProcessorEditor::~Ring64AudioProcessorEditor()
{
}

void Ring64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(customLookAndFeel.backgroundColour);
    g.setColour(customLookAndFeel.lineColour);
    auto width = static_cast<float>(getWidth());
    g.drawLine(width * 0.05f, width * 0.2f, width * 0.95f, width * 0.2f, width * 0.004f);
}

void Ring64AudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil((float)getWidth() / 16.0f);
    fontSize = (float)blockUI * 0.75f;
    customFont = customFont.withHeight(fontSize);

    header.setJustificationType(juce::Justification::left);
    header.setBounds(blockUI, blockUI / 2, blockUI * 2, (int)fontSize);
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
    masterLabel.setBounds(blockUI, blockUI * 5, blockUI * 3, blockUI);
    masterLabel.setFont(customFont.withHeight(fontSize));

    masterModLabel.setJustificationType(juce::Justification::bottomLeft);
    masterModLabel.setBounds(blockUI * 5, blockUI * 4, blockUI * 3, blockUI);
    masterModLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterModChLabel.setJustificationType(juce::Justification::bottomLeft);
    masterModChLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 4, blockUI * 3, blockUI);
    masterModChLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterWetLabel.setJustificationType(juce::Justification::bottomLeft);
    masterWetLabel.setBounds(blockUI * 12, blockUI * 4, blockUI * 3, blockUI);
    masterWetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterModBox.setBounds(blockUI * 5, blockUI * 5, blockUI * 3, blockUI);

    masterModChSlider.setBounds((int)((float)blockUI * 8.5f), blockUI * 5, blockUI * 3, blockUI);
    masterModChSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterWetSlider.setBounds(blockUI * 12, blockUI * 5, blockUI * 3, blockUI);
    masterWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterFreqLabel.setJustificationType(juce::Justification::bottomLeft);
    masterFreqLabel.setBounds(blockUI * 5, blockUI * 6, blockUI * 3, blockUI);
    masterFreqLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterFreqSlider.setBounds(blockUI * 5, blockUI * 7, blockUI * 10, blockUI);
    masterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chLabel.setJustificationType(juce::Justification::centredLeft);
    chLabel.setFont(customFont.withHeight(fontSize));
    chLabel.setBounds(blockUI, blockUI * 10, blockUI * 2, blockUI);

    chModLabel.setJustificationType(juce::Justification::bottomLeft);
    chModLabel.setBounds(blockUI * 5, blockUI * 9, blockUI * 3, blockUI);
    chModLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chModChLabel.setJustificationType(juce::Justification::bottomLeft);
    chModChLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 9, blockUI * 4, blockUI);
    chModChLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chWetLabel.setJustificationType(juce::Justification::bottomLeft);
    chWetLabel.setBounds(blockUI * 12, blockUI * 9, blockUI * 3, blockUI);
    chWetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chFreqLabel.setJustificationType(juce::Justification::bottomLeft);
    chFreqLabel.setBounds(blockUI * 5, blockUI * 11, blockUI * 3, blockUI);
    chFreqLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    selectChBox.setBounds((int)((float)blockUI * 2.5f), blockUI * 10, (int)((float)blockUI * 1.5f), blockUI);

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        if ((unsigned int)(selectChBox.getSelectedId()) - 1 == ch)
        {
            chModBoxes[ch].setBounds(blockUI * 5, blockUI * 10, blockUI * 3, blockUI);

            chModChSliders[ch].setBounds((int)((float)blockUI * 8.5f), blockUI * 10, blockUI * 3, blockUI);
            chModChSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chWetSliders[ch].setBounds(blockUI * 12, blockUI * 10, blockUI * 3, blockUI);
            chWetSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chFreqSliders[ch].setBounds(blockUI * 5, blockUI * 12, blockUI * 10, blockUI);
            chFreqSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);
        }
        else
        {
            chModBoxes[ch].setBounds(0, 0, 0, 0);
            chFreqSliders[ch].setBounds(0, 0, 0, 0);
            chModChSliders[ch].setBounds(0, 0, 0, 0);
            chWetSliders[ch].setBounds(0, 0, 0, 0);
        }
    }
}
