/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AwesomePartyAudioProcessor::AwesomePartyAudioProcessor()
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
    windowSize = ValueTreeState->getRawParameterValue("windowSize");
    feedback = ValueTreeState->getRawParameterValue("feedback");
    dryWet = ValueTreeState->getRawParameterValue("dryWet");
}

AwesomePartyAudioProcessor::~AwesomePartyAudioProcessor()
{
}

//==============================================================================
const juce::String AwesomePartyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AwesomePartyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AwesomePartyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AwesomePartyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AwesomePartyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AwesomePartyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AwesomePartyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AwesomePartyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AwesomePartyAudioProcessor::getProgramName (int index)
{
    return {};
}

void AwesomePartyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AwesomePartyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverseDelay.prepareToPlay(sampleRate);
}

void AwesomePartyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AwesomePartyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AwesomePartyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    reverseDelay.setParameters(*windowSize, *feedback, *dryWet);
    
    reverseDelay.processBlock(buffer);
}

//==============================================================================
bool AwesomePartyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AwesomePartyAudioProcessor::createEditor()
{
    return new AwesomePartyAudioProcessorEditor (*this);
}

//==============================================================================
void AwesomePartyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = ValueTreeState->copyState();
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AwesomePartyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    ValueTreeState->replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AwesomePartyAudioProcessor();
}

void AwesomePartyAudioProcessor::_constructValueTreeState(){
    ValueTreeState.reset(new juce::AudioProcessorValueTreeState(*this, nullptr,juce::Identifier("reverseDelay"),{
        
        //
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("windowSize", 1), "Window_Size", juce::NormalisableRange<float>(0.1f,1.0f,0.01f), 0.5f),
        
        //
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("feedback", 1), "Feedback", juce::NormalisableRange<float>(0.f,1.0f,0.01f), 0.5f),
        
        //
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("dryWet", 1), "Dry_Wet", juce::NormalisableRange<float>(0.f,1.0f,0.01f), 0.5f)
    }));
}
