/*
  ==============================================================================

    tempoSyncReverseDelay.cpp
    Created: 16 Sep 2024 2:17:09pm
    Author:  Jake Morgan

  ==============================================================================
*/

#include "tempoSyncReverseDelay.h"


tempoSyncReverseDelay::tempoSyncReverseDelay(){
    
    smoothedFeedback.setCurrentAndTargetValue(0.5f);
    smoothedDryWet.setCurrentAndTargetValue(0.5f);
    smoothedSyncedWindowSize.setCurrentAndTargetValue(2);
    
}

tempoSyncReverseDelay::~tempoSyncReverseDelay(){
    
}

void tempoSyncReverseDelay::prepareToPlay(float inSampleRate, double bpm){
    
    samples_per_beat = 60/bpm * inSampleRate;
    
    windowSizes(inSampleRate, bpm);
    
    smoothedFeedback.reset(0.01f);
    smoothedDryWet.reset(0.01f);
    smoothedSyncedWindowSize.reset(0.1f);
    
}

void tempoSyncReverseDelay::windowSizes(float inSampleRate, double bpm){
    
    quarterNoteWindow = samples_per_beat;
    eighthNoteWindow = quarterNoteWindow/2;
    sixteenthNoteWindow = eighthNoteWindow/2;
    halfNoteWindow = quarterNoteWindow * 2;
    
    // this is hard coded for the array size
    // very bad do not do this ever
    // fix later
    syncedTimes[0] = halfNoteWindow;
    syncedTimes[1] = quarterNoteWindow;
    syncedTimes[2] = eighthNoteWindow;
    syncedTimes[3] = sixteenthNoteWindow;
    
}

float tempoSyncReverseDelay::hannWindow(int sizeInSamples, int currentSample){
    
    float output = 0.f;
    
    output = 0.5 * (1-cos((2*(M_PI)) * currentSample/sizeInSamples));
    
    return output;

}

void tempoSyncReverseDelay::processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlaying){
    
    int num_samples = inBuffer.getNumSamples();
    // not sure if using the circular buffer instead of in buffer will work but oh well
    //int num_samples = circularBuffer.getNumSamples();
    
    // dry
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    //
    float* circular_channel_left = circularBuffer.getWritePointer(0);
    float* circular_channel_right = circularBuffer.getWritePointer(1);
    
    int smoothed_window_size = smoothedSyncedWindowSize.getNextValue();
    
    //WHAT TO DO HERE:
    // WE NEED A STATEMNT THAT SWITCHES BETWEEN ON AND OFF WHEN THE ISPLAYING IS TRUE
    // THEN WE NEED TO GRAB THE CURRENT POSITION/ BEATS TO SYNC THE WINDOWS FOR EACH DELAY
    
    if(isPlaying == false){
        if(swap == true){
            for(int i = 0; i < circularBuffer.getNumSamples(); i++){
                circular_channel_left[i] = 0.f;
                circular_channel_right[i] = 0.f;
            }
            swap = false;
            
            for(int i = 0; i < num_samples; i++){
                
                // so beast
                float curve = hannWindow(smoothed_window_size, (revhead - beginhead));
                
                //
                float input_left = in_channel_left[i];
                float input_right = in_channel_right[i];
                
                //
                in_channel_left[i] = 0.f;
                in_channel_right[i] = 0.f;
                
                //
                // this is writing the reverse to be read in reverse ie forwards
                // needs to be fixed
                circular_channel_left[writehead] = input_left + (writeFeedbackLeft);
                circular_channel_right[writehead] = input_right + (writeFeedbackRight);
                
                //
                float wet_amount = smoothedDryWet.getNextValue();
                float dry_amount = 1.f - wet_amount;
                
                // this will cut off the delay when window is complete so maybe fix this?
                if(windowComplete == false){
                    //in_channel_left[i] = input_left;
                    //in_channel_right[i] = input_right;
                    
                    // see if this doesn't cut of the delay signal
                    in_channel_left[i] = input_left + (feedbackLeft);
                    in_channel_right[i] = input_right + (feedbackRight);
                }
                // after the buffer gets initialized for the windowSize
                // create an if statement that uses the markerhead and reverse to that point.
                
                // once the window has been filled up by the writehead
                // initializes the sequence to start reversing what was written
                if(windowCounter == smoothed_window_size){
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
                    
                    
                    int feedhead = writehead-smoothed_window_size;
                    //
                    float reverse_out_left = circular_channel_left[revhead];
                    float reverse_out_right = circular_channel_right[revhead];
                    
                    float feedback_out_left = circular_channel_left[feedhead];
                    float feedback_out_right = circular_channel_right[feedhead];
                    
                    float feedback_amount = smoothedFeedback.getNextValue();
                    
                    // this feedback is for writing
                    writeFeedbackLeft = feedback_out_left * feedback_amount;
                    writeFeedbackRight = feedback_out_right * feedback_amount;
                    
                    // this is for plauing while the delay is active
                    feedbackLeft = reverse_out_left * feedback_amount;
                    feedbackRight = reverse_out_right * feedback_amount;
                    
                    //
                    in_channel_left[i] = ((input_left * dry_amount) + ((reverse_out_left * curve) * wet_amount) + feedbackLeft);
                    in_channel_right[i] = ((input_right * dry_amount) + ((reverse_out_right * curve) * wet_amount) + feedbackRight);
                    
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
        
        if(isPlaying == true){
            if(swap == true){
                for(int i = 0; i < circularBuffer.getNumSamples(); i++){
                    circular_channel_left[i] = 0.f;
                    circular_channel_right[i] = 0.f;
                }
                swap = false;
            }
            
            for(int i = 0; i < num_samples; i++){
                
                // THIS NEEDS TO BE CHANGED SO THAT IT CAN UPDATE WINDOW SIZE LIVE.
                // FIND A WAY TO KEEP THE WINDOW SYNCED BETTER
                
                //
                float input_left = in_channel_left[i];
                float input_right = in_channel_right[i];
                
                writehead = 0;
                windowComplete = false;
                windowCounter = 0;
                
                
                //
                in_channel_left[i] = 0.f;
                in_channel_right[i] = 0.f;
                
                //
                // this is writing the reverse to be read in reverse ie forwards
                // needs to be fixed
                circular_channel_left[writehead] = input_left + (writeFeedbackLeft);
                circular_channel_right[writehead] = input_right + (writeFeedbackRight);
                
                //
                float wet_amount = smoothedDryWet.getNextValue();
                float dry_amount = 1.f - wet_amount;
                
                // this will cut off the delay when window is complete so maybe fix this?
                if(windowComplete == false){
                    //in_channel_left[i] = input_left;
                    //in_channel_right[i] = input_right;
                    
                    // see if this doesn't cut of the delay signal
                    in_channel_left[i] = input_left + (feedbackLeft);
                    in_channel_right[i] = input_right + (feedbackRight);
                }
                // after the buffer gets initialized for the windowSize
                // create an if statement that uses the markerhead and reverse to that point.
                
                // once the window has been filled up by the writehead
                // initializes the sequence to start reversing what was written
                if(windowCounter == smoothed_window_size){
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
                    
                    // so beast
                    float curve = hannWindow(smoothed_window_size, (revhead - beginhead));
                    
                    
                    int feedhead = writehead-smoothed_window_size;
                    //
                    float reverse_out_left = circular_channel_left[revhead];
                    float reverse_out_right = circular_channel_right[revhead];
                    
                    float feedback_out_left = circular_channel_left[feedhead];
                    float feedback_out_right = circular_channel_right[feedhead];
                    
                    float feedback_amount = smoothedFeedback.getNextValue();
                    
                    // this feedback is for writing
                    writeFeedbackLeft = feedback_out_left * feedback_amount;
                    writeFeedbackRight = feedback_out_right * feedback_amount;
                    
                    // this is for plauing while the delay is active
                    feedbackLeft = reverse_out_left * feedback_amount;
                    feedbackRight = reverse_out_right * feedback_amount;
                    
                    //
                    in_channel_left[i] = ((input_left * dry_amount) + ((reverse_out_left * curve) * wet_amount) + feedbackLeft);
                    in_channel_right[i] = ((input_right * dry_amount) + ((reverse_out_right * curve) * wet_amount) + feedbackRight);
                    
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
        
    }
}

void tempoSyncReverseDelay::setParameters(int inSyncedWindowSize, float inFeedbackPercent, float inDryWetPercent){
    
    smoothedSyncedWindowSize.setTargetValue(syncedTimes[inSyncedWindowSize-1]);
    smoothedFeedback.setTargetValue(inFeedbackPercent);
    smoothedDryWet.setTargetValue(inDryWetPercent);
    
}
