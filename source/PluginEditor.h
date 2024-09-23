/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "lookAndFeel.h"

//==============================================================================
/**
*/
class AwesomePartyAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AwesomePartyAudioProcessorEditor (AwesomePartyAudioProcessor&);
    ~AwesomePartyAudioProcessorEditor() override;
    
//    void _updateTempoSync();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //custom look and feel
    customLookAndFeel customLookAndfeel;
    
    //button
    juce::ToggleButton tempoSyncButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncButtonAttachment;
    
    //slider
    //
    juce::Slider windowSizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> windowSizeSliderAttachment;
    
    //
    juce::Slider feedbackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;
    
    //
    juce::Slider dryWetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetSliderAttachment;
    
    juce::Image background;
    
    
    void _updateTempoSync();
    
    void _buttonWasPressed();
    
    //maybe add more parameters later
    
    
    AwesomePartyAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AwesomePartyAudioProcessorEditor)
};
