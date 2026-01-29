/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DisortionAudioProcessor::DisortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       // INITIALIZE APVTS HERE
       apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

DisortionAudioProcessor::~DisortionAudioProcessor()
{
}

// THIS DEFINES THE PARAMETERS (KNOBS)
juce::AudioProcessorValueTreeState::ParameterLayout DisortionAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ID, Name, Min, Max, Default
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("volume", "Volume", 0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String DisortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DisortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DisortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DisortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DisortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DisortionAudioProcessor::getNumPrograms()
{
    return 1;
}

int DisortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DisortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DisortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DisortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DisortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void DisortionAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DisortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// DSP LOGIC GOES HERE
void DisortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // 1. Get parameter values
    float currentDrive = *apvts.getRawParameterValue("drive");
    float currentVol   = *apvts.getRawParameterValue("volume");

    // Make the drive range more useful (1x gain to 50x gain)
    float inputGain = 1.0f + (currentDrive * 50.0f);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float in = channelData[sample];

            // 2. Apply Drive
            in *= inputGain;

            // 3. Apply Soft Clipping (Distortion)
            // std::tanh "squashes" the wave as it gets loud
            float processed = std::tanh(in);

            // 4. Apply Output Volume and store
            channelData[sample] = processed * currentVol;
        }
    }
}

//==============================================================================
bool DisortionAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DisortionAudioProcessor::createEditor()
{
    return new DisortionAudioProcessorEditor (*this);
}

//==============================================================================
void DisortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save user parameters
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DisortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Load user parameters
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DisortionAudioProcessor();
}
