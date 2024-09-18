/*
  ==============================================================================

    tempoSyncReverseDelay.h
    Created: 16 Sep 2024 2:17:09pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"

class tempoSyncReverseDelay {
    
public:
    //c
    tempoSyncReverseDelay();
    //dc
    ~tempoSyncReverseDelay();
    
    //
    void prepareToPlay(float inSampleRate, double bpm);
    
    void windowSizes(float inSampleRate, double bpm);
    
    float hannWindow(int lengthInSamples, int currentSample);
    
    void processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlaying);
    
    void setParameters(int inSyncedWindowSize, float inFeedbackPercent, float inDryWetPercent);
    
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
    
    float writeFeedbackLeft = 0;
    float writeFeedbackRight = 0;
    
    float sampleRate = 44100.f;
    
    int current_pos_in_samples = 0;
    int samples_per_beat = 0;
    int quarterNoteWindow = 0;
    int halfNoteWindow = 0;
    int eighthNoteWindow = 0;
    int sixteenthNoteWindow = 0;
    
    int syncedTimes[4];
    
    bool swap = false;
    
    int windowCounter = 0;
    bool windowComplete = false;
    
    //trouble shoot a window size till it works
    //const int maxWindowSize = sampleRate / 1.5;
    
    juce::LinearSmoothedValue<int> smoothedSyncedWindowSize;
    juce::LinearSmoothedValue<float> smoothedFeedback;
    juce::LinearSmoothedValue<float> smoothedDryWet;
    juce::LinearSmoothedValue<float> smoothedVolumeRamp;
};
