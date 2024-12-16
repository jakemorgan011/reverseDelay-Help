/*
  ==============================================================================

    tempoSyncReverseDelay.cpp
    Created: 16 Sep 2024 2:17:09pm
    Author:  Jake Morgan

  ==============================================================================
*/

#include "tempoSyncReverseDelay.h"

//====================================
//-----------------------------------
//          NOTES
//
// WHAT IS WORKING:
// - temposync is officially online whoop whoop 9/25
// - THE BUTTON WORKS 9/14
// - HANN WINDOW DOES ITS RAMP PERFECTLY 9/14
// - we have synced reversing. 9/25
//
// WHAT IS NOT WORKING:
// - WHEN SWAPPING BETWEEN SYNC AND NO SYNC ARTIFACTS SEEM TO BUILD UP AND FEEDBACK 9/14
// - Buffer is filling up with nonsense that explodes the speaker 9/25
// - hopefully this is solved by the swap checker since the buffer had leftover garbage inside of it when swapping between synced and not synced 10/26
// - can't use the delay in the middle of a bar.
//
//
//-------------------------------------
//
//  NOTE FROM A MONTH LATER. 10/26
// this is literally just describing how a pointer works lol
// but yea
// pointer
// i think im gonna try to fix the porblem of not perfect tempo sync.
// pushing this to git
//
// THIS IS THE MOST IMPORTANT NOTE EVER:
// in this code we use *float in_channel_left/right
// this allows us to grab the address of each sample and modify.
// we can now modify the sample(variable) without calling it actively.
// we grab the address and basically change the address's value so that the variable is changed remotely
//
// -----------------------------------------------------------
//
// notes from HALLOWEEN:
// okay so i added a modulo function to calculate basically what eighth note we are on.
// the function needs to dynamically change between each window size.
//
//=====================================

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
    circularBuffer.setSize(2, halfNoteWindow *2);
    circularBuffer.clear();
    
    smoothedFeedback.reset(0.01f);
    smoothedDryWet.reset(0.01f);
    smoothedSyncedWindowSize.reset(0.1f);
    smoothedBPM.reset(0.01f);
    
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

void tempoSyncReverseDelay::processBlock(juce::AudioBuffer<float>& inBuffer, bool isPlaying, juce::int64 currentSamplePos){
    
    
    calculateModulo(currentSamplePos, smoothedBPM.getNextValue());
    
    int num_samples = inBuffer.getNumSamples();
    
    long long int current_pos_in_samples = currentSamplePos;

    int oneBarInSamples = halfNoteWindow * 2;
    // not sure if using the circular buffer instead of in buffer will work but oh well
    //int num_samples = circularBuffer.getNumSamples();
    
    // dry
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    //
    float* circular_channel_left = circularBuffer.getWritePointer(0);
    float* circular_channel_right = circularBuffer.getWritePointer(1);
    
    int smoothed_window_size = smoothedSyncedWindowSize.getNextValue();
    
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

void tempoSyncReverseDelay::setParameters(int inSyncedWindowSize, float inFeedbackPercent, float inDryWetPercent){
    
    smoothedSyncedWindowSize.setTargetValue(syncedTimes[inSyncedWindowSize-1]);
    smoothedFeedback.setTargetValue(inFeedbackPercent);
    smoothedDryWet.setTargetValue(inDryWetPercent);
    
    syncnum = inSyncedWindowSize;
    
}

juce::int64 tempoSyncReverseDelay::calculateModulo(juce::int64 currentSamplePos, double bpm){
    // in my head
    // this should return a value 0-3
    int calculatedWindowPosition = currentSamplePos % quarterNoteWindow;
    DBG(calculatedWindowPosition);
    return calculatedWindowPosition;
}

