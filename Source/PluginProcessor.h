/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class QuinnKPFinalCompressionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    QuinnKPFinalCompressionAudioProcessor();
    ~QuinnKPFinalCompressionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    void calcAlgorithmParams();

    // User Parameters
    juce::AudioParameterFloat* mThresholdParam;
    juce::AudioParameterFloat* mRatioParam;
    juce::AudioParameterFloat* mAttackTimeParam;
    juce::AudioParameterFloat* mReleaseTimeParam;
    juce::AudioParameterBool* mUseSidechainParam;

    //Algorithm Parameters
    float mFs;
    bool mUseSidechain;
    float mEnvOut = 0.0;
    float mEnvB0;
    float mEnvTau = 0.010;
    float mThresholdDB;
    float mSlope;
    float mAttackCoeff;
    float mReleaseCoeff;
    float mFinalGainLin;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuinnKPFinalCompressionAudioProcessor)
};
