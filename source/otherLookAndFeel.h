/*
  ==============================================================================

    otherLookAndFeel.h
    Created: 7 Oct 2024 5:29:21pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class otherLookAndFeel : public juce::LookAndFeel_V4{
public:
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider){
        
        auto sprite = juce::ImageCache::getFromMemory(BinaryData::blackKnob_png, BinaryData::blackKnob_pngSize);
        
        auto image_width = sprite.getWidth();
        auto num_slides = sprite.getHeight() / image_width;
        auto target_slide = num_slides * sliderPos;
//        float sliderHeight = 80;
//        float sliderWidth = 80;
        
        auto start_y = target_slide * image_width;
        
        g.drawImage(sprite, 0, 0, sprite.getWidth(), sprite.getHeight(), 0, start_y, sprite.getWidth(), sprite.getWidth());
        g.drawImage(sprite, 0, 0, sprite.getWidth(), 100, 0, start_y, image_width, sprite.getWidth());
    }
    
};
