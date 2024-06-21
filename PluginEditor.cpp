/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AwesomePartyAudioProcessorEditor::AwesomePartyAudioProcessorEditor (AwesomePartyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    setLookAndFeel(&customLookAndfeel);
    
    //
    windowSizeSlider.setRange(0.1, 1.0f);
    windowSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    windowSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(windowSizeSlider);
    windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"windowSize",windowSizeSlider));
    //add label later maybe
    
    //
    feedbackSlider.setRange(0, 1.0f);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(feedbackSlider);
    feedbackSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"feedback",feedbackSlider));
    
    //
    dryWetSlider.setRange(0, 1.0f);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(dryWetSlider);
    dryWetSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"dryWet",dryWetSlider));
    
    
    

    setSize (300, 200);
}

AwesomePartyAudioProcessorEditor::~AwesomePartyAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AwesomePartyAudioProcessorEditor::paint (juce::Graphics& g)
{
    background = juce::ImageCache::getFromMemory(BinaryData::frutigerScape_png,BinaryData::frutigerScape_pngSize);
        g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
}

void AwesomePartyAudioProcessorEditor::resized()
{
    windowSizeSlider.setBounds(0, 0, 80, 80);
    feedbackSlider.setBounds(100, 0, 80, 80);
    dryWetSlider.setBounds(200, 0, 80, 80);
}
