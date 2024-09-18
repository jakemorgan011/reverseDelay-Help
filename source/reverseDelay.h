/*
  ==============================================================================

    reverseDelay.h
    Created: 22 Apr 2024 1:11:34pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "tempoSyncReverseDelay.h"
#include <math.h>

class reverseDelay {
    
public:
    //c
    reverseDelay();
    //dc
    ~reverseDelay();
    
    //
    void prepareToPlay(float inSampleRate);
    
    float hannWindow(int lengthInSamples, int currentSample);
    
    void processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlaying);
    
    void setParameters(float inWindowSize, float inFeedbackPercent, float inDryWetPercent, bool timeSync);
    
private:
    // the notorius CB
    juce::AudioBuffer<float> circularBuffer;
    
    tempoSyncReverseDelay tempoSyncReverseDelay;
    
    //playhead
    int writehead = 0;
    int revhead = 0;
    int endhead = 0;
    int beginhead = 0;
    
    float feedbackLeft = 0;
    float feedbackRight = 0;
    
    float writeFeedbackLeft = 0;
    float writeFeedbackRight = 0;
    
    float sampleRate = 44100.f;
    
    int windowCounter = 0;
    bool windowComplete = false;
    
    bool sync = false;
    
    //trouble shoot a window size till it works
    const int maxWindowSize = sampleRate / 1.5;
    
    juce::LinearSmoothedValue<float> smoothedWindowSize;
    juce::LinearSmoothedValue<float> smoothedFeedback;
    juce::LinearSmoothedValue<float> smoothedDryWet;
    juce::LinearSmoothedValue<float> smoothedVolumeRamp;
};
