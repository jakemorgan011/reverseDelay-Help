/*
  ==============================================================================

    tempo_sync_reverse.cpp
    Created: 2 Dec 2024 1:00:48pm
    Author:  Jake Morgan

  ==============================================================================
*/

#include "tempo_sync_reverse.h"

tempo_sync_delay::tempo_sync_delay(){
    smoothedFeedback.setCurrentAndTargetValue(0.5f);
    smoothedDryWet.setCurrentAndTargetValue(0.5f);
    smoothedSyncedWindowSize.setCurrentAndTargetValue(2);
}

tempo_sync_delay::~tempo_sync_delay(){
    
}

void tempo_sync_delay::prepareToPlay(float inSampleRate, double bpm){
    samples_per_beat = 60/bpm * inSampleRate;
    
    windowSizes(inSampleRate, bpm);
    
    sampleRate = inSampleRate;
    circularBuffer.setSize(2, halfNoteWindow*2);
    circularBuffer.clear();
    
    smoothedFeedback.reset(sampleRate, 0.01f);
    smoothedDryWet.reset(sampleRate, 0.01f);
    smoothedSyncedWindowSize.reset(sampleRate, 0.01f);
    smoothedBPM.reset(sampleRate, 0.01f);
}

void tempo_sync_delay::windowSizes(float inSampleRate, double bpm){
    quarterNoteWindow = samples_per_beat;
    eighthNoteWindow = quarterNoteWindow/2;
    sixteenthNoteWindow = eighthNoteWindow/2;
    halfNoteWindow = quarterNoteWindow *2;
    
    syncedTimes[0] = halfNoteWindow;
    syncedTimes[1] = quarterNoteWindow;
    syncedTimes[2] = eighthNoteWindow;
    syncedTimes[3] = sixteenthNoteWindow;
}

float tempo_sync_delay::hannWindow(int sizeInSamples, int currentSample){
    float output = 0.f;
    output = 0.5 * ((1-cos(2*(M_PI))) * currentSample/sizeInSamples);
    return output;
}

void tempo_sync_delay::processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlayig, juce::int64 currentSamplePos){
    calculateModulo(currentSamplePos, smoothedBPM.getNextValue());
    
    int num_samples = inBuffer.getNumSamples();
    
    // not used yet... but maybe in a later patch
    long long int current_pos_in_samples = currentSamplePos;
    
    int oneBarInSamples = halfNoteWindow * 2;
    //
    
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    float* circular_channel_left = circularBuffer.getWritePointer(0);
    float* circular_channel_right = circularBuffer.getWritePointer(1);
    
    int smoothed_window_size = smoothedSyncedWindowSize.getNextValue();
    
    for(int i = 0; i < num_samples; i++){
        float curve = hannWindow(smoothed_window_size, std::abs(revhead-beginhead));
        
        float input_left = in_channel_left[i];
        float input_right = in_channel_right[i];
        
        in_channel_left[i] = 0.f;
        in_channel_right[i] = 0.f;
        
        circular_channel_left[writehead] = input_left + writeFeedbackLeft;
        circular_channel_right[writehead] = input_right + writeFeedbackRight;
        
        float wet_amount = smoothedDryWet.getNextValue();
        float dry_amount = 1.f - wet_amount;
        
        if(windowComplete == false){
            in_channel_left[i] = input_left + feedbackLeft;
            in_channel_right[i] = input_right + feedbackRight;
        }
        
        if(windowCounter == smoothed_window_size){
            beginhead = writehead;
            endhead = writehead - smoothed_window_size;
            if(endhead < 0){
                endhead += circularBuffer.getNumSamples();
            }
            revhead = beginhead;
            windowComplete = true;
        }
        if(windowComplete == true){
            int feedhead = writehead - smoothed_window_size;
            
            float reverse_out_left = circular_channel_left[revhead];
            float reverse_out_right = circular_channel_right[revhead];
            
            float feedback_out_left = circular_channel_left[feedhead];
            float feedback_out_right = circular_channel_right[feedhead];
            
            float feedback_amount = smoothedFeedback.getNextValue();
            
            writeFeedbackLeft = feedback_out_left * feedback_amount;
            writeFeedbackRight = feedback_out_right * feedback_amount;
            
            feedbackLeft = reverse_out_left * feedback_amount;
            feedbackRight = reverse_out_left * feedback_amount;
            
            in_channel_left[i] = (input_left * dry_amount) + (((reverse_out_left * curve) * wet_amount) + feedbackLeft);
            in_channel_right[i] = (input_left * dry_amount) + (((reverse_out_right * curve) * wet_amount) + feedbackRight);
            
            revhead--;
            if(revhead < 0){
                revhead += circularBuffer.getNumSamples();
            }
        }
        writehead++;
        windowCounter++;
        
        if(writehead >= circularBuffer.getNumSamples()){
            writehead = 0;
        }
        if(windowCounter >= smoothed_window_size + 1){
            windowCounter = 0;
        }
        if(revhead <= endhead){
            windowComplete = false;
        }
        
    }
}

void tempo_sync_delay::setParameters(int inSyncedWindowSize, float inFeedbackPercent, float inDryWetPercent){
    
    smoothedSyncedWindowSize.setTargetValue(syncedTimes[inSyncedWindowSize-1]);
    smoothedFeedback.setTargetValue(inFeedbackPercent);
    smoothedDryWet.setTargetValue(inDryWetPercent);
    
    syncnum = inSyncedWindowSize;
}

// not using yet :( this will be able to detect where in arrangement view you are via sample pos
juce::int64 tempo_sync_delay::calculateModulo(juce::int64 currentSamplePos, double bpm){
    int calculatedWindowPosition = currentSamplePos % quarterNoteWindow;
    DBG(calculatedWindowPosition);
    return calculatedWindowPosition;
}
