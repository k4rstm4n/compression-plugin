/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QuinnKPFinalCompressionAudioProcessor::QuinnKPFinalCompressionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       .withInput ("Sidechain", juce::AudioChannelSet::stereo(), true)
                       )
#endif
{
    addParameter(mThresholdParam = new juce::AudioParameterFloat("Threshold (dB)",
        "thresh",
        -100.0,
        20.0,
        -30.0));
    addParameter(mRatioParam = new juce::AudioParameterFloat("Ratio (__:1)",
        "ratio",
        1.0,
        50.0,
        10.0));
    addParameter(mAttackTimeParam = new juce::AudioParameterFloat("Attack (ms)",
        "attack",
        0.0,
        250.0,
        10.0));
    addParameter(mReleaseTimeParam = new juce::AudioParameterFloat("Release (ms)",
        "release",
        0.0,
        2000.0,
        100.0));
    addParameter(mUseSidechainParam = new juce::AudioParameterBool("Sidechain On",
        "sidechain",
        false));
}

QuinnKPFinalCompressionAudioProcessor::~QuinnKPFinalCompressionAudioProcessor()
{
}

//==============================================================================
const juce::String QuinnKPFinalCompressionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool QuinnKPFinalCompressionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool QuinnKPFinalCompressionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool QuinnKPFinalCompressionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double QuinnKPFinalCompressionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int QuinnKPFinalCompressionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int QuinnKPFinalCompressionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void QuinnKPFinalCompressionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String QuinnKPFinalCompressionAudioProcessor::getProgramName (int index)
{
    return {};
}

void QuinnKPFinalCompressionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void QuinnKPFinalCompressionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mFs = sampleRate;

    // Calculate the filter coefficient for RMS envelope follower
    mEnvB0 = 1 - exp(-1 / (mEnvTau * mFs));
}

void QuinnKPFinalCompressionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QuinnKPFinalCompressionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void QuinnKPFinalCompressionAudioProcessor::calcAlgorithmParams()
{
    mSlope = 1 / mRatioParam->get();
    mThresholdDB = mThresholdParam->get();
    float tauAttack = mAttackTimeParam->get() / 1000;
    float tauRelease = mReleaseTimeParam->get() / 1000;
    mAttackCoeff = 1.0 - exp(-1.0 / (tauAttack * mFs));
    mReleaseCoeff = 1.0 - exp(-1.0 / (tauRelease * mFs));
    mUseSidechain = mUseSidechainParam->get();

}

void QuinnKPFinalCompressionAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //auto mainBuffer = getBusBuffer(buffer, true, 0);
    //auto sideChainBuffer = getBusBuffer(buffer, true, 1);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    calcAlgorithmParams();
     
    // Process each audio sample
    auto* outputLeft = buffer.getWritePointer(0);
    auto* outputRight = buffer.getWritePointer(1);
    auto* inputLeft = buffer.getReadPointer(0);
    auto* inputRight = buffer.getReadPointer(1);
    if (mUseSidechain) {
        inputLeft = buffer.getReadPointer(2);
        inputRight = buffer.getReadPointer(3);
    }


    float squareL, squareR, mixedSquares, root, levelDB, dynamicsCoeff, compressionGainDB, compressionGainLin, temp2;

    for (int samp = 0; samp < buffer.getNumSamples(); samp++)
    {
        // Level detector
        squareL = inputLeft[samp] * inputLeft[samp];
        squareR = inputRight[samp] * inputRight[samp];
        mixedSquares = 0.5 * (squareL + squareR);
        mEnvOut += mEnvB0 * (mixedSquares - mEnvOut);
        root = sqrt(mEnvOut);

        // Convert to dB
        levelDB = 20 * log10(root);

        // Calculate gain
        if (levelDB < mThresholdDB) {
            compressionGainLin = 1.0;
        }
        else {
            compressionGainDB = (mSlope - 1) * (levelDB - mThresholdDB);

            compressionGainLin = powf(10.0, compressionGainDB / 20.0);
        }

        // Find coefficient for dynamics
        if (compressionGainLin < mFinalGainLin) {
            dynamicsCoeff = mAttackCoeff;
        }
        else {
            dynamicsCoeff = mReleaseCoeff;
        }

        // Smoothe gain using coefficient
        mFinalGainLin += dynamicsCoeff * (compressionGainLin - mFinalGainLin);

        outputLeft[samp] = mFinalGainLin * outputLeft[samp];
        outputRight[samp] = mFinalGainLin * outputRight[samp];

        //mainIOLeft[samp] = 1 * mainIOLeft[samp];

        //mainIORight[samp] = root; // DEBUG: output of RMS detectorw
        //mainIORight[samp] = compressionGainLin; // DEBUG: Gain Computer Output
        //mainIORight[samp] = mFinalGainLin; // DEBUG: Gain Dynamics Output
        


    }
}

//==============================================================================
bool QuinnKPFinalCompressionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* QuinnKPFinalCompressionAudioProcessor::createEditor()
{
    return new QuinnKPFinalCompressionAudioProcessorEditor (*this);
}

//==============================================================================
void QuinnKPFinalCompressionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void QuinnKPFinalCompressionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QuinnKPFinalCompressionAudioProcessor();
}
