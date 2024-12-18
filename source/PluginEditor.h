/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "customLookAndFeel.h"

//==============================================================================
/**
*/
class Static_revAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Static_revAudioProcessorEditor (Static_revAudioProcessor&);
    ~Static_revAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    customLookAndFeel customLookAndFeel;
    
    juce::Slider windowSizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> windowSizeSliderAttachment;
    
    juce::Slider feedbackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;
    
    juce::Slider dryWetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetSliderAttachment;
    
    juce::Image background;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Static_revAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Static_revAudioProcessorEditor)
};
