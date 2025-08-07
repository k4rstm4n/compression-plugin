/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
void QuinnKPFinalCompressionAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    auto& params = processor.getParameters();

    if (slider == &mThresholdSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = mThresholdSlider.getValue(); // set the AudioParameter
        DBG("mThresholdSlider Slider Changed");
    }
    else if (slider == &mRatioSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = mRatioSlider.getValue(); // set the AudioParameter
        DBG("mRatioSlider Slider Changed");
    }
    else if (slider == &mAttackSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = mAttackSlider.getValue(); // set the AudioParameter
        DBG("mAttackSlider Slider Changed");
    }
    else if (slider == &mReleaseSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = mReleaseSlider.getValue(); // set the AudioParameter
        DBG("mReleaseSlider Slider Changed");
    }
}


void QuinnKPFinalCompressionAudioProcessorEditor::buttonClicked(juce::Button* button) {
    auto& params = processor.getParameters();

    if (button == &mSidechainButton) {
        juce::AudioParameterBool* audioParam = (juce::AudioParameterBool*)params.getUnchecked(4);
        *audioParam = mSidechainButton.getToggleState();
        DBG("mSideChainButton Button Changed");
    }
}

QuinnKPFinalCompressionAudioProcessorEditor::QuinnKPFinalCompressionAudioProcessorEditor (QuinnKPFinalCompressionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    auto& params = processor.getParameters();

    //mThresholdSlider
    juce::AudioParameterFloat* mThresholdParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    mThresholdSlider.setBounds(70 - 50, 20, 100, 260);
    mThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mThresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    mThresholdSlider.setTextValueSuffix(" dB Threshold");
    mThresholdSlider.setRange(mThresholdParam->range.start, mThresholdParam->range.end);
    mThresholdSlider.setValue(*mThresholdParam);

    //mRatioSlider
    juce::AudioParameterFloat* mRatioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    mRatioSlider.setBounds(200-50, 20, 100, 110);
    mRatioSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRatioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    mRatioSlider.setTextValueSuffix(" :1 Ratio");
    mRatioSlider.setRange(mRatioParam->range.start, mRatioParam->range.end);
    mRatioSlider.setValue(*mRatioParam);

    //mAttackSlider
    juce::AudioParameterFloat* mAttackTimeParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    mAttackSlider.setBounds(200 - 50, 170, 100, 110);
    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    mAttackSlider.setTextValueSuffix(" ms Attack");
    mAttackSlider.setRange(mAttackTimeParam->range.start, mAttackTimeParam->range.end);
    mAttackSlider.setValue(*mAttackTimeParam);

    //mReleaseSlider
    juce::AudioParameterFloat* mReleaseTimeParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    mReleaseSlider.setBounds(330 - 50, 170, 100, 110);
    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    mReleaseSlider.setTextValueSuffix(" ms Attack");
    mReleaseSlider.setRange(mReleaseTimeParam->range.start, mReleaseTimeParam->range.end);
    mReleaseSlider.setValue(*mReleaseTimeParam);

    //mSidechainButton
    juce::AudioParameterBool* mUseSidechainParam = (juce::AudioParameterBool*)params.getUnchecked(4);
    mSidechainButton.setSize(100, 150);
    mSidechainButton.setCentrePosition(330, 55);
    mSidechainButton.setButtonText(" Use Sidechain Input");

    
    mThresholdSlider.addListener(this);
    mRatioSlider.addListener(this);
    mAttackSlider.addListener(this);
    mReleaseSlider.addListener(this);
    mSidechainButton.addListener(this);

    addAndMakeVisible(mThresholdSlider);
    addAndMakeVisible(mRatioSlider);
    addAndMakeVisible(mAttackSlider);
    addAndMakeVisible(mReleaseSlider);
    addAndMakeVisible(mSidechainButton);

}

QuinnKPFinalCompressionAudioProcessorEditor::~QuinnKPFinalCompressionAudioProcessorEditor()
{
}

//==============================================================================
void QuinnKPFinalCompressionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void QuinnKPFinalCompressionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
