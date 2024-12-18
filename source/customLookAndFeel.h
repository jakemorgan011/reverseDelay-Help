/*
  ==============================================================================

    customLookAndFeel.h
    Created: 12 Nov 2024 11:37:58pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class customLookAndFeel : public juce::LookAndFeel_V4{
    
public:
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        // variable setup for the slider when it is drawn.
        auto radius = (float)juce::jmin(width / 2, height/2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        //
        g.setColour (juce::Colours::whitesmoke);
        g.fillEllipse(rx, ry, rw, rw);
        
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        
        g.setColour(juce::Colours::black);
        g.fillPath(p);
        
    }
    
private:
};

