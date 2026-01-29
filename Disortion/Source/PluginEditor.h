/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DisortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DisortionAudioProcessorEditor (DisortionAudioProcessor&);
    ~DisortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DisortionAudioProcessor& audioProcessor;

    // UI Elements
    juce::Slider driveSlider;
    juce::Slider volumeSlider;
    
    juce::Label driveLabel;
    juce::Label volumeLabel;

    // Attachments (Connecting UI to Processor)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisortionAudioProcessorEditor)
};
