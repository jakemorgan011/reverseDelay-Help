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
    
    //setLookAndFeel(&customLookAndfeel);
    
    //
//    if(tempoSyncButton.getState() == true){
//        windowSizeSlider.setRange(1,4);
//    }
//    if(tempoSyncButton.getState() == false){
//        windowSizeSlider.setRange(0.1, 1.0f);
//    }
    windowSizeSlider.setRange(0.1, 1.0f);
    //FIX
//    windowSizeSlider.setRange(1,4);
//    windowSizeSlider.set
    windowSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    windowSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(windowSizeSlider);
//    if(tempoSyncButton.getState() == true){
//        windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"syncedWindowSize",windowSizeSlider));
//    }
//    if(tempoSyncButton.getState() == false){
//        windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"windowSize",windowSizeSlider));
//    }
    
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
    
    //
    tempoSyncButton.setToggleable(true);
    tempoSyncButton.setButtonText("tempoSync");
    tempoSyncButton.setColour(0, juce::Colours::black);
    tempoSyncButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.getVTS(), "tempoSync", tempoSyncButton));
    addAndMakeVisible(tempoSyncButton);
    _updateTempoSync();
    
    

    setSize (300, 200);
}

AwesomePartyAudioProcessorEditor::~AwesomePartyAudioProcessorEditor()
{
    //setLookAndFeel(nullptr);
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
    tempoSyncButton.setBounds(100,100,100,100);
}

void AwesomePartyAudioProcessorEditor::_updateTempoSync(){
    
    if(tempoSyncButton.getToggleState()){
        windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"syncedWindowSize",windowSizeSlider));
    }else{
        windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"windowSize",windowSizeSlider));
    }
    
}

void AwesomePartyAudioProcessorEditor::_buttonWasPressed(){
}
