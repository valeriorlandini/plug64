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

Filter64AudioProcessorEditor::Filter64AudioProcessorEditor(Filter64AudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(500, 400);
    setResizeLimits(400, 200, 3000, 1500); /// CHECK!
    setResizable(true, p.wrapperType != Filter64AudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(1.25f);

    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    header.setText("Plug64", juce::dontSendNotification);
    addAndMakeVisible(header);
    title.setText("FILTER64", juce::dontSendNotification);
    addAndMakeVisible(title);

    resetLabel.setText("RESET CH PARAMS", juce::dontSendNotification);
    addAndMakeVisible(resetLabel);

    juce::Path resetShape;
    resetShape.addRectangle(juce::Rectangle<int>(0, 0, 20, 20));
    resetButton.setShape(resetShape, false, true, false);
    addAndMakeVisible(resetButton);
    resetButton.onClick = [this]
    {
        for (int ch = 0; ch < 64; ++ch)
        {        
            std::string ch_str = std::to_string(ch+1);
            std::array<std::string, 4> paramIDs{"chtype" + ch_str, "chcutoff" + ch_str, "chresonance" + ch_str, "chdrive" + ch_str};

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

    masterCutoffLabel.setText("CUT", juce::dontSendNotification);
    masterCutoffLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterCutoffLabel);

    masterResonanceLabel.setText("RESO", juce::dontSendNotification);
    masterResonanceLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterResonanceLabel);

    masterDriveLabel.setText("DRIVE", juce::dontSendNotification);
    masterDriveLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterDriveLabel);

    masterTypeLabel.setText("TYPE", juce::dontSendNotification);
    masterTypeLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(masterTypeLabel);

    masterCutoffSlider.setLookAndFeel(&customLookAndFeel);
    masterCutoffSlider.setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    masterCutoffSlider.setSliderStyle(juce::Slider::LinearBar);
    masterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterCutoffSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterCutoffSlider);
    masterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "mastercutoff", masterCutoffSlider);
    masterCutoffSlider.setTextValueSuffix(" Hz");

    masterResonanceSlider.setLookAndFeel(&customLookAndFeel);
    masterResonanceSlider.setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    masterResonanceSlider.setSliderStyle(juce::Slider::LinearBar);
    masterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterResonanceSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterResonanceSlider);
    masterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterresonance", masterResonanceSlider);
    masterResonanceSlider.setTextValueSuffix(" %");

    masterDriveSlider.setLookAndFeel(&customLookAndFeel);
    masterDriveSlider.setColour(juce::Slider::trackColourId, juce::Colour(101, 142, 162));
    masterDriveSlider.setSliderStyle(juce::Slider::LinearBar);
    masterDriveSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    masterDriveSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(masterDriveSlider);
    masterDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "masterdrive", masterDriveSlider);
    masterDriveSlider.setTextValueSuffix(" %");

    masterFilterBox.setLookAndFeel(&customLookAndFeel);
    masterFilterBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    masterFilterBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    masterFilterBox.setScrollWheelEnabled(true);
    addAndMakeVisible(masterFilterBox); 	
    masterFilterBox.addItem("NONE", 1);
    masterFilterBox.addItem("LPF12", 2);
    masterFilterBox.addItem("HPF12", 3);
    masterFilterBox.addItem("BPF12", 4);
    masterFilterBox.addItem("LPF24", 5);
    masterFilterBox.addItem("HPF24", 6);
    masterFilterBox.addItem("BPF24", 7);
    masterFilterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, "mastertype", masterFilterBox);

    chLabel.setText("CHAN", juce::dontSendNotification);
    chLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chLabel);
    
    chCutoffLabel.setText("CUT", juce::dontSendNotification);
    chCutoffLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chCutoffLabel);

    chResonanceLabel.setText("RESO", juce::dontSendNotification);
    chResonanceLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chResonanceLabel);

    chDriveLabel.setText("DRIVE", juce::dontSendNotification);
    chDriveLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chDriveLabel);

    chTypeLabel.setText("TYPE", juce::dontSendNotification);
    chTypeLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chTypeLabel);

    selectChBox.setLookAndFeel(&customLookAndFeel);
    selectChBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    selectChBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    selectChBox.setScrollWheelEnabled(true);
    addAndMakeVisible(selectChBox);
    for (auto ch = 1; ch <= 64; ch++)
    {
        selectChBox.addItem(std::to_string(ch), ch);
    }
    selectChBox.onChange = [this]
    {
        audioProcessor.selChannel = selectChBox.getSelectedId();
        resized();
    };
    selectChBox.setSelectedId(int(audioProcessor.selChannel.getValue()) != 0 ? int(audioProcessor.selChannel.getValue()) : 1);

    for (unsigned int i = 0; i < 64; i++)
    {
        std::string ch_str = std::to_string(i+1);
        std::string paramID;

        chCutoffSliders[i].setLookAndFeel(&customLookAndFeel);
        chCutoffSliders[i].setColour(juce::Slider::trackColourId, juce::Colour(153, 99, 134));
        chCutoffSliders[i].setSliderStyle(juce::Slider::LinearBar);
        chCutoffSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chCutoffSliders[i].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chCutoffSliders[i]);
        paramID = "chcutoff" + ch_str;
        chCutoffAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chCutoffSliders[i]);
        chCutoffSliders[i].setTextValueSuffix(" Hz");

        chResonanceSliders[i].setLookAndFeel(&customLookAndFeel);
        chResonanceSliders[i].setColour(juce::Slider::trackColourId, juce::Colour(153, 99, 134));
        chResonanceSliders[i].setSliderStyle(juce::Slider::LinearBar);
        chResonanceSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chResonanceSliders[i].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chResonanceSliders[i]);
        paramID = "chresonance" + ch_str;
        chResonanceAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chResonanceSliders[i]);
        chResonanceSliders[i].setTextValueSuffix(" %");

        chDriveSliders[i].setLookAndFeel(&customLookAndFeel);
        chDriveSliders[i].setColour(juce::Slider::trackColourId, juce::Colour(153, 99, 134));
        chDriveSliders[i].setSliderStyle(juce::Slider::LinearBar);
        chDriveSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        chDriveSliders[i].setPopupDisplayEnabled(false, false, this);
        addAndMakeVisible(chDriveSliders[i]);
        paramID = "chdrive" + ch_str;
        chDriveAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, paramID, chDriveSliders[i]);
        chDriveSliders[i].setTextValueSuffix(" %");

        chFilterBoxes[i].setLookAndFeel(&customLookAndFeel);
        chFilterBoxes[i].setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
        chFilterBoxes[i].setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        chFilterBoxes[i].setScrollWheelEnabled(true);
        addAndMakeVisible(chFilterBoxes[i]); 	
        chFilterBoxes[i].addItem("NONE", 1);
        chFilterBoxes[i].addItem("LPF12", 2);
        chFilterBoxes[i].addItem("HPF12", 3);
        chFilterBoxes[i].addItem("BPF12", 4);
        chFilterBoxes[i].addItem("LPF24", 5);
        chFilterBoxes[i].addItem("HPF24", 6);
        chFilterBoxes[i].addItem("BPF24", 7);
        paramID = "chtype" + ch_str;
        chFilterAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.treeState, paramID, chFilterBoxes[i]);
    }
}

Filter64AudioProcessorEditor::~Filter64AudioProcessorEditor()
{
}

void Filter64AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(44, 48, 52));
    resized();
}

void Filter64AudioProcessorEditor::resized()
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

    resetLabel.setJustificationType(juce::Justification::centredLeft);
    resetLabel.setBounds(blockUI * 10, blockUI, blockUI * 14, blockUI * 2);
    resetLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    resetButton.setSize((int)((float)blockUI * 0.5f), (int)((float)blockUI * 0.5f));
    resetButton.setCentrePosition(resetLabel.getX() - (int)((float)blockUI * 0.55f), resetLabel.getY() + (int)((float)resetLabel.getHeight() * 0.5f));

    masterLabel.setJustificationType(juce::Justification::centredLeft);
    masterLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    masterLabel.setFont(customFont.withHeight(fontSize));

    masterTypeLabel.setJustificationType(juce::Justification::bottomLeft);
    masterTypeLabel.setBounds(blockUI * 5, blockUI * 3, blockUI * 3, blockUI);
    masterTypeLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterResonanceLabel.setJustificationType(juce::Justification::bottomLeft);
    masterResonanceLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 3, blockUI * 3, blockUI);
    masterResonanceLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterDriveLabel.setJustificationType(juce::Justification::bottomLeft);
    masterDriveLabel.setBounds(blockUI * 12, blockUI * 3, blockUI * 3, blockUI);
    masterDriveLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterFilterBox.setBounds(blockUI * 5, blockUI * 4, blockUI * 3, blockUI);

    masterResonanceSlider.setBounds((int)((float)blockUI * 8.5f), blockUI * 4, blockUI * 3, blockUI);
    masterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterDriveSlider.setBounds(blockUI * 12, blockUI * 4, blockUI * 3, blockUI);
    masterDriveSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

    masterCutoffLabel.setJustificationType(juce::Justification::bottomLeft);
    masterCutoffLabel.setBounds(blockUI * 5, blockUI * 5, blockUI * 3, blockUI);
    masterCutoffLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    masterCutoffSlider.setBounds(blockUI * 5, blockUI * 6, blockUI * 10, blockUI);
    masterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);

    chLabel.setJustificationType(juce::Justification::centredLeft);
    chLabel.setFont(customFont.withHeight(fontSize));
    chLabel.setBounds(blockUI, blockUI * 8, blockUI * 2, blockUI);

    chTypeLabel.setJustificationType(juce::Justification::bottomLeft);
    chTypeLabel.setBounds(blockUI * 5, blockUI * 7, blockUI * 3, blockUI);
    chTypeLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chResonanceLabel.setJustificationType(juce::Justification::bottomLeft);
    chResonanceLabel.setBounds((int)((float)blockUI * 8.5f), blockUI * 7, blockUI * 4, blockUI);
    chResonanceLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chDriveLabel.setJustificationType(juce::Justification::bottomLeft);
    chDriveLabel.setBounds(blockUI * 12, blockUI * 7, blockUI * 3, blockUI);
    chDriveLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    chCutoffLabel.setJustificationType(juce::Justification::bottomLeft);
    chCutoffLabel.setBounds(blockUI * 5, blockUI * 9, blockUI * 3, blockUI);
    chCutoffLabel.setFont(customFont.withHeight(fontSize * 0.75f));

    selectChBox.setBounds((int)((float)blockUI * 2.5f), blockUI * 8, (int)((float)blockUI * 1.5f), blockUI);

    for (unsigned int c = 0; c < 64; c++)
    {
        if ((unsigned int)(selectChBox.getSelectedId()) - 1 == c)
        {
            chFilterBoxes[c].setBounds(blockUI * 5, blockUI * 8, blockUI * 3, blockUI);

            chResonanceSliders[c].setBounds((int)((float)blockUI * 8.5f), blockUI * 8, blockUI * 3, blockUI);
            chResonanceSliders[c].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chDriveSliders[c].setBounds(blockUI * 12, blockUI * 8, blockUI * 3, blockUI);
            chDriveSliders[c].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 3, blockUI);

            chCutoffSliders[c].setBounds(blockUI * 5, blockUI * 10, blockUI * 10, blockUI);    
            chCutoffSliders[c].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 10, blockUI);
        }
        else
        {
            chFilterBoxes[c].setBounds(0, 0, 0, 0);
            chCutoffSliders[c].setBounds(0, 0, 0, 0);
            chResonanceSliders[c].setBounds(0, 0, 0, 0);
            chDriveSliders[c].setBounds(0, 0, 0, 0);
        }
    }
}
