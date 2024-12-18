/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Static_revAudioProcessorEditor::Static_revAudioProcessorEditor (Static_revAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&customLookAndFeel);
    
    windowSizeSlider.setRange(0.1f, 1.f);
    windowSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    windowSizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(windowSizeSlider);
    windowSizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"syncedWindowSize", windowSizeSlider));
    
    feedbackSlider.setRange(0.0f, 1.0f);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(feedbackSlider);
    feedbackSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"feedback", feedbackSlider));
    
    dryWetSlider.setRange(0.f,1.f);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(dryWetSlider);
    dryWetSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"dryWet", dryWetSlider));
    
    setSize(300,300);
}

Static_revAudioProcessorEditor::~Static_revAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void Static_revAudioProcessorEditor::paint (juce::Graphics& g)
{
    background = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
}

void Static_revAudioProcessorEditor::resized()
{
    windowSizeSlider.setBounds(0, 50, 100, 100);
    feedbackSlider.setBounds(100, 50, 100, 100);
    dryWetSlider.setBounds(200, 50, 100, 100);
}
