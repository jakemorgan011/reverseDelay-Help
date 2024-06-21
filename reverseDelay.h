/*
  ==============================================================================

    reverseDelay.h
    Created: 22 Apr 2024 1:11:34pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class reverseDelay {
    
public:
    //c
    reverseDelay();
    //dc
    ~reverseDelay();
    
    //
    void prepareToPlay(float inSampleRate);
    
    void processBlock(juce::AudioBuffer<float>& inBuffer);
    
    void setParameters(float inWindowSize, float inFeedbackPercent, float inDryWetPercent);
    
private:
    // the notorius CB
    juce::AudioBuffer<float> circularBuffer;
    
    //playhead
    int writehead = 0;
    int revhead = 0;
    int endhead = 0;
    int beginhead = 0;
    
    float feedbackLeft = 0;
    float feedbackRight = 0;
    
    float sampleRate = 44100.f;
    
    int windowCounter = 0;
    bool windowComplete = false;
    
    //trouble shoot a window size till it works
    const int maxWindowSize = sampleRate / 1.5;
    
    juce::LinearSmoothedValue<float> smoothedWindowSize;
    juce::LinearSmoothedValue<float> smoothedFeedback;
    juce::LinearSmoothedValue<float> smoothedDryWet;
};
