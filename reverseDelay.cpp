/*
  ==============================================================================

    reverseDelay.cpp
    Created: 22 Apr 2024 1:11:34pm
    Author:  Jake Morgan

  ==============================================================================
*/

#include "reverseDelay.h"

reverseDelay::reverseDelay(){
    
    smoothedFeedback.setCurrentAndTargetValue(0.5f);
    smoothedDryWet.setCurrentAndTargetValue(0.5f);
    smoothedWindowSize.setCurrentAndTargetValue(1.f);
    
}

reverseDelay::~reverseDelay(){
}

void reverseDelay::prepareToPlay(float inSampleRate){
    
    sampleRate = inSampleRate;
    circularBuffer.setSize(2, inSampleRate * 4);
    
    smoothedFeedback.reset(inSampleRate, 0.01f);
    smoothedWindowSize.reset(inSampleRate, 0.001f);
    smoothedDryWet.reset(0.01f);
    
}

void reverseDelay::processBlock(juce::AudioBuffer<float>& inBuffer){
    
    
    int num_samples = inBuffer.getNumSamples();
    // not sure if using the circular buffer instead of in buffer will work but oh well
    //int num_samples = circularBuffer.getNumSamples();
    
    // dry
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    float* circular_channel_left = circularBuffer.getWritePointer(0);
    float* circular_channel_right = circularBuffer.getWritePointer(1);
    
    int smoothed_window_size = maxWindowSize * smoothedWindowSize.getNextValue();
    
    for(int i = 0; i < num_samples; i++){
        
        //
        float input_left = in_channel_left[i];
        float input_right = in_channel_right[i];
        
        //
        in_channel_left[i] = 0.f;
        in_channel_right[i] = 0.f;
        
        //
        // this is writing the reverse to be read in reverse ie forwards
        // needs to be fixed
        circular_channel_left[writehead] = input_left + (feedbackLeft * smoothedFeedback.getNextValue());
        circular_channel_right[writehead] = input_right + (feedbackRight * smoothedFeedback.getNextValue());
        
        //
        float wet_amount = smoothedDryWet.getNextValue();
        float dry_amount = 1.f - wet_amount;
        
        // this will cut off the delay when window is complete so maybe fix this?
        if(windowComplete == false){
            //in_channel_left[i] = input_left;
            //in_channel_right[i] = input_right;
            
            // see if this doesn't cut of the delay signal
            in_channel_left[i] = input_left + (feedbackLeft * smoothedFeedback.getNextValue());
            in_channel_right[i] = input_right + (feedbackRight * smoothedFeedback.getNextValue());
        }
        // after the buffer gets initialized for the windowSize
        // create an if statement that uses the markerhead and reverse to that point.
        
        // once the window has been filled up by the writehead
        // initializes the sequence to start reversing what was written
        if(windowCounter >= smoothed_window_size){
            beginhead = writehead;
            endhead = writehead - smoothed_window_size;
            if(endhead < 0){
                endhead += circularBuffer.getNumSamples();
            }
            revhead = beginhead;
            windowComplete = true;
        }
        //intializes the reverse window
        if(windowComplete == true){
            //
            float reverse_out_left = circular_channel_left[revhead];
            float reverse_out_right = circular_channel_right[revhead];
            
            float feedback_amount = smoothedFeedback.getNextValue();
            
            //
            feedbackLeft = reverse_out_left * feedback_amount;
            feedbackRight = reverse_out_right * feedback_amount;
            
            //
            in_channel_left[i] = (input_left * dry_amount) + (reverse_out_left * wet_amount);
            in_channel_right[i] = (input_right * dry_amount) + (reverse_out_left * wet_amount);
            
            //
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

void reverseDelay::setParameters(float inWindowSize, float inFeedbackPercent, float inDryWetPercent){
    
    smoothedWindowSize.setTargetValue(inWindowSize);
    smoothedFeedback.setTargetValue(inFeedbackPercent);
    smoothedDryWet.setTargetValue(inDryWetPercent);
    
}
