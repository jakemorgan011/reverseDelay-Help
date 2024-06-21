/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "reverseDelay.h"


//==============================================================================
/**
*/
class AwesomePartyAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AwesomePartyAudioProcessor();
    ~AwesomePartyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // make the VTS activate and expose to the procesor
    juce::AudioProcessorValueTreeState& getVTS(){
        return *ValueTreeState;
    }

private:
    
    reverseDelay reverseDelay;
    
    std::atomic<float>* windowSize = nullptr;
    std::atomic<float>* feedback = nullptr;
    std::atomic<float>* dryWet = nullptr;
    
    juce::LinearSmoothedValue<float> smoothedWindowSize;
    juce::LinearSmoothedValue<float> smoothedFeedback;
    juce::LinearSmoothedValue<float> smoothedDryWet;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState> ValueTreeState;
    
    void _constructValueTreeState();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AwesomePartyAudioProcessor)
};
