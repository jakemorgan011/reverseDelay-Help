/*
  ==============================================================================

    tempo_sync_reverse.h
    Created: 2 Dec 2024 1:00:48pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "math.h"
class tempo_sync_delay{
public:
    tempo_sync_delay();
    ~tempo_sync_delay();
    
    void prepareToPlay(float inSampleRate, double bpm);
    
    void windowSizes(float inSampleRate, double bpm);
    
    float hannWindow(int lengthInSamples, int currentSample);
    
    void processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlaying, juce::int64 currentSamplePos);
    
    void setParameters(int inSyncedWindowSize, float inFeedbackPercent, float inDryWetPercent);
    
    juce::int64 calculateModulo(juce::int64 currentSamplePos, double bpm);
    
private:
    juce::AudioBuffer<float> circularBuffer;
    
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
    
    int syncnum = 0;
    
    
    juce::LinearSmoothedValue<int> smoothedSyncedWindowSize;
    juce::LinearSmoothedValue<float> smoothedFeedback;
    juce::LinearSmoothedValue<float> smoothedDryWet;
    juce::LinearSmoothedValue<float> smoothedVolumeRamp;
    
    juce::LinearSmoothedValue<double> smoothedBPM;
};
