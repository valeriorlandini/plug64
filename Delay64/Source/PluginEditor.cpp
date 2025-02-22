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

Delay64AudioProcessorEditor::Delay64AudioProcessorEditor(Delay64AudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(500, 440);
    setResizeLimits(400, 352, 3000, 2640);
    setResizable(true, p.wrapperType != Delay64AudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(5.0f/4.4f);

    getLookAndFeel().setColour(juce::Label::textColourId, customLookAndFeel.textColour);
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    header.setText("Plug64", juce::dontSendNotification);
    header.setColour(juce::Label::textColourId, customLookAndFeel.backgroundColour);
    header.setColour(juce::Label::backgroundColourId, customLookAndFeel.textColour);
    addAndMakeVisible(header);

    title.setText("DELAY64", juce::dontSendNotification);
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
            std::array<std::string, 4> paramIDs{"chsync" + ch_str, "chtime" + ch_str, "chfeedback" + ch_str, "chwet" + ch_str};

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

    masterTimeLabel.setText("TIME", juce::dontSendNotification);
    masterTimeLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterTimeLabel);

    masterFeedbackLabel.setText("FBACK", juce::dontSendNotification);
    masterFeedbackLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterFeedbackLabel);

    masterWetLabel.setText("WET", juce::dontSendNotification);
    masterWetLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterWetLabel);

    masterSyncLabel.setText("SYNC", juce::dontSendNotification);
    masterSyncLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterSyncLabel);

    masterTimeSlider.setLookAndFeel(&customLookAndFeel);
    masterTimeSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.mainMasterSliderColour);
    masterTimeSlider.setSliderStyle(juce::Slider::LinearBar);
    masterTimeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterTimeSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterTimeSlider);
    masterTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastertime", masterTimeSlider);
    masterTimeSlider.setTextValueSuffix(" ms");

    masterFeedbackSlider.setLookAndFeel(&customLookAndFeel);
    masterFeedbackSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.otherMasterSliderColour);
    masterFeedbackSlider.setSliderStyle(juce::Slider::LinearBar);
    masterFeedbackSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterFeedbackSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterFeedbackSlider);
    masterFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterfeedback", masterFeedbackSlider);
    masterFeedbackSlider.setTextValueSuffix(" %");

    masterWetSlider.setLookAndFeel(&customLookAndFeel);
    masterWetSlider.setColour(juce::Slider::trackColourId, customLookAndFeel.otherMasterSliderColour);
    masterWetSlider.setSliderStyle(juce::Slider::LinearBar);
    masterWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterWetSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterWetSlider);
    masterWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterwet", masterWetSlider);
    masterWetSlider.setTextValueSuffix(" %");

    masterSyncBox.setLookAndFeel(&customLookAndFeel);
    masterSyncBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    masterSyncBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    masterSyncBox.setScrollWheelEnabled(true);
    addAndMakeVisible(masterSyncBox);
    masterSyncBox.addItem("NONE", 1);
    for (auto i = 1; i <= 16; ++i)
    {
        masterSyncBox.addItem(std::to_string(i) + "/16", i+1);
    }
    masterSyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, "mastersync", masterSyncBox);

    chLabel.setText("CHAN", juce::dontSendNotification);
    chLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chLabel);

    chTimeLabel.setText("TIME", juce::dontSendNotification);
    chTimeLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chTimeLabel);

    chFeedbackLabel.setText("FBACK", juce::dontSendNotification);
    chFeedbackLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chFeedbackLabel);

    chWetLabel.setText("WET", juce::dontSendNotification);
    chWetLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chWetLabel);

    chSyncLabel.setText("SYNC", juce::dontSendNotification);
    chSyncLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chSyncLabel);

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

        chTimeSliders[ch].setLookAndFeel(&customLookAndFeel);
        chTimeSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.mainChSliderColour);
        chTimeSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chTimeSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chTimeSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chTimeSliders[ch]);
        paramID = "chtime" + ch_str;
        chTimeAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chTimeSliders[ch]);
        chTimeSliders[ch].setTextValueSuffix(" ms");

        chFeedbackSliders[ch].setLookAndFeel(&customLookAndFeel);
        chFeedbackSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.otherChSliderColour);
        chFeedbackSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chFeedbackSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chFeedbackSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chFeedbackSliders[ch]);
        paramID = "chfeedback" + ch_str;
        chFeedbackAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chFeedbackSliders[ch]);
        chFeedbackSliders[ch].setTextValueSuffix(" %");

        chWetSliders[ch].setLookAndFeel(&customLookAndFeel);
        chWetSliders[ch].setColour(juce::Slider::trackColourId, customLookAndFeel.otherChSliderColour);
        chWetSliders[ch].setSliderStyle(juce::Slider::LinearBar);
        chWetSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chWetSliders[ch].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chWetSliders[ch]);
        paramID = "chwet" + ch_str;
        chWetAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chWetSliders[ch]);
        chWetSliders[ch].setTextValueSuffix(" %");

        chSyncBoxes[ch].setLookAndFeel(&customLookAndFeel);
        chSyncBoxes[ch].setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
        chSyncBoxes[ch].setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        chSyncBoxes[ch].setScrollWheelEnabled(true);
        addAndMakeVisible(chSyncBoxes[ch]);
        chSyncBoxes[ch].addItem("NONE", 1);
        for (auto i = 1; i <= 16; ++i)
        {
            chSyncBoxes[ch].addItem(std::to_string(i) + "/16", i+1);
        }
        paramID = "chsync" + ch_str;
        chSyncAttachments[ch] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, paramID, chSyncBoxes[ch]);
    }
}

Delay64AudioProcessorEditor::~Delay64AudioProcessorEditor()
{
}

void Delay64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(customLookAndFeel.backgroundColour);
    g.setColour(customLookAndFeel.lineColour);
    auto width = static_cast<float>(getWidth());
    g.drawLine(width * 0.05f, width * 0.2f, width * 0.95f, width * 0.2f, width * 0.004f);
}

void Delay64AudioProcessorEditor::resized()
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

    masterSyncLabel.setJustificationType(juce::Justification::bottomLeft);
    masterSyncLabel.setBounds(blockUI * 5, blockUI * 4, blockUI * 3, blockUI);
    masterSyncLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterFeedbackLabel.setJustificationType(juce::Justification::bottomLeft);
    masterFeedbackLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 4, blockUI * 3, blockUI);
    masterFeedbackLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterWetLabel.setJustificationType(juce::Justification::bottomLeft);
    masterWetLabel.setBounds(blockUI * 12, blockUI * 4, blockUI * 3, blockUI);
    masterWetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterSyncBox.setBounds(blockUI * 5, blockUI * 5, blockUI * 3, blockUI);

    masterFeedbackSlider.setBounds((int)((float)blockUI * 8.5f), blockUI * 5, blockUI * 3, blockUI);
    masterFeedbackSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterWetSlider.setBounds(blockUI * 12, blockUI * 5, blockUI * 3, blockUI);
    masterWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterTimeLabel.setJustificationType(juce::Justification::bottomLeft);
    masterTimeLabel.setBounds(blockUI * 5, blockUI * 6, blockUI * 3, blockUI);
    masterTimeLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterTimeSlider.setBounds(blockUI * 5, blockUI * 7, blockUI * 10, blockUI);
    masterTimeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chLabel.setJustificationType(juce::Justification::centredLeft);
    chLabel.setFont(customFont.withHeight(fontSize));
    chLabel.setBounds(blockUI, blockUI * 10, blockUI * 2, blockUI);

    chSyncLabel.setJustificationType(juce::Justification::bottomLeft);
    chSyncLabel.setBounds(blockUI * 5, blockUI * 9, blockUI * 3, blockUI);
    chSyncLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chFeedbackLabel.setJustificationType(juce::Justification::bottomLeft);
    chFeedbackLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 9, blockUI * 4, blockUI);
    chFeedbackLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chWetLabel.setJustificationType(juce::Justification::bottomLeft);
    chWetLabel.setBounds(blockUI * 12, blockUI * 9, blockUI * 3, blockUI);
    chWetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chTimeLabel.setJustificationType(juce::Justification::bottomLeft);
    chTimeLabel.setBounds(blockUI * 5, blockUI * 11, blockUI * 3, blockUI);
    chTimeLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    selectChBox.setBounds((int)((float)blockUI * 2.5f), blockUI * 10, (int)((float)blockUI * 1.5f), blockUI);

    for (unsigned int ch = 0; ch < MAX_CHANS; ++ch)
    {
        if ((unsigned int)(selectChBox.getSelectedId()) - 1 == ch)
        {
            chSyncBoxes[ch].setBounds(blockUI * 5, blockUI * 10, blockUI * 3, blockUI);

            chFeedbackSliders[ch].setBounds((int)((float)blockUI * 8.5f), blockUI * 10, blockUI * 3, blockUI);
            chFeedbackSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chWetSliders[ch].setBounds(blockUI * 12, blockUI * 10, blockUI * 3, blockUI);
            chWetSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chTimeSliders[ch].setBounds(blockUI * 5, blockUI * 12, blockUI * 10, blockUI);
            chTimeSliders[ch].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);
        }
        else
        {
            chSyncBoxes[ch].setBounds(0, 0, 0, 0);
            chTimeSliders[ch].setBounds(0, 0, 0, 0);
            chFeedbackSliders[ch].setBounds(0, 0, 0, 0);
            chWetSliders[ch].setBounds(0, 0, 0, 0);
        }
    }
}
