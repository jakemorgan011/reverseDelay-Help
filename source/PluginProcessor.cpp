/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Static_revAudioProcessor::Static_revAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    _constructValueTreeState();
    syncedWindowSize = ValueTreeState->getRawParameterValue("syncedWindowSize");
    feedback = ValueTreeState->getRawParameterValue("feedback");
    dryWet = ValueTreeState->getRawParameterValue("dryWet");
}

Static_revAudioProcessor::~Static_revAudioProcessor()
{
}

//==============================================================================
const juce::String Static_revAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Static_revAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Static_revAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Static_revAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Static_revAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Static_revAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Static_revAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Static_revAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Static_revAudioProcessor::getProgramName (int index)
{
    return {};
}

void Static_revAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Static_revAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverseDelay.prepareToPlay(sampleRate, playhead.bpm);
}

void Static_revAudioProcessor::releaseResources()
{
    // maybe use this for stopping the loop.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Static_revAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Static_revAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    reverseDelay.processBlock(buffer, playhead.isPlaying, playhead.timeInSamples);
    reverseDelay.setParameters(*syncedWindowSize, *feedback, *dryWet);
}

//==============================================================================
bool Static_revAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Static_revAudioProcessor::createEditor()
{
    return new Static_revAudioProcessorEditor (*this);
}

//==============================================================================
void Static_revAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = ValueTreeState->copyState();
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Static_revAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    ValueTreeState->replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Static_revAudioProcessor();
}

void Static_revAudioProcessor::_constructValueTreeState(){
    ValueTreeState.reset(new juce::AudioProcessorValueTreeState(*this, nullptr,juce::Identifier("reverseDelay"),{
        //
        //NEED TO MAKE THIS WORK
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("syncedWindowSize", 1), "Synced_Window_Size", juce::NormalisableRange<float>(1,4,1), 2),
        
        //
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("feedback", 1), "Feedback", juce::NormalisableRange<float>(0.f,1.0f,0.01f), 0.5f),
        
        //
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("dryWet", 1), "Dry_Wet", juce::NormalisableRange<float>(0.f,1.0f,0.01f), 0.5f),
    }));
}
