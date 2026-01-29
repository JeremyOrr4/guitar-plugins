/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DisortionAudioProcessorEditor::DisortionAudioProcessorEditor (DisortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);

    // --- DRIVE SLIDER ---
    driveSlider.setSliderStyle(juce::Slider::Rotary);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(driveSlider);
    
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.attachToComponent(&driveSlider, false);
    addAndMakeVisible(driveLabel);

    // Link Drive Slider to APVTS "drive" parameter
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "drive", driveSlider);

    // --- VOLUME SLIDER ---
    volumeSlider.setSliderStyle(juce::Slider::Rotary);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(volumeSlider);
    
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centred);
    volumeLabel.attachToComponent(&volumeSlider, false);
    addAndMakeVisible(volumeLabel);

    // Link Volume Slider to APVTS "volume" parameter
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "volume", volumeSlider);
}

DisortionAudioProcessorEditor::~DisortionAudioProcessorEditor()
{
}

//==============================================================================
void DisortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background Color
    g.fillAll (juce::Colours::darkgrey);
    
    // Header Text
    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawFittedText ("Gemini Drive", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);
}

void DisortionAudioProcessorEditor::resized()
{
    // Layout the sliders
    // x, y, width, height
    driveSlider.setBounds(50, 100, 100, 100);
    volumeSlider.setBounds(250, 100, 100, 100);
}
