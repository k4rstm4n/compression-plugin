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
class QuinnKPFinalCompressionAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Button::Listener
{
public:
    QuinnKPFinalCompressionAudioProcessorEditor (QuinnKPFinalCompressionAudioProcessor&);
    ~QuinnKPFinalCompressionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    QuinnKPFinalCompressionAudioProcessor& audioProcessor;

    juce::Slider mThresholdSlider;
    juce::Slider mRatioSlider;
    juce::Slider mAttackSlider;
    juce::Slider mReleaseSlider;
    juce::ToggleButton mSidechainButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuinnKPFinalCompressionAudioProcessorEditor)
};
