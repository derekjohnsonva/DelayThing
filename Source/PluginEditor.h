#pragma once

#include "PluginProcessor.h"

//==============================================================================
class DelayThingEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit DelayThingEditor(DelayThingAudioProcessor &);
    ~DelayThingEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayThingAudioProcessor &processorRef;
    int lastDelayRepsValue{0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayThingEditor)

    juce::Slider delayTimeSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};
    juce::Slider delayMixSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};
    juce::Slider delayRepsSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};

    // Sliders for individual delay volume
    juce::Slider delayGainSlider1{juce::Slider::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider delayGainSlider2{juce::Slider::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider delayGainSlider3{juce::Slider::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider delayGainSlider4{juce::Slider::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider delayGainSlider5{juce::Slider::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Array<juce::Slider *> delayGainSliders{&delayGainSlider1, &delayGainSlider2, &delayGainSlider3, &delayGainSlider4, &delayGainSlider5};

    juce::AudioProcessorValueTreeState::SliderAttachment delayTimeKnobAttachment{processorRef.getValueTreeState(), processorRef.delayTimeParamName, delayTimeSlider};
    juce::AudioProcessorValueTreeState::SliderAttachment delayMixKnobAttachment{processorRef.getValueTreeState(), processorRef.delayMixParamName, delayMixSlider};
    juce::AudioProcessorValueTreeState::SliderAttachment delayRepsKnobAttachment{processorRef.getValueTreeState(), processorRef.delayRepsParamName, delayRepsSlider};

    juce::AudioProcessorValueTreeState::SliderAttachment delayGainKnobAttachment1{processorRef.getValueTreeState(), processorRef.delayRepGain1ParamName, delayGainSlider1};
    juce::AudioProcessorValueTreeState::SliderAttachment delayGainKnobAttachment2{processorRef.getValueTreeState(), processorRef.delayRepGain2ParamName, delayGainSlider2};
    juce::AudioProcessorValueTreeState::SliderAttachment delayGainKnobAttachment3{processorRef.getValueTreeState(), processorRef.delayRepGain3ParamName, delayGainSlider3};
    juce::AudioProcessorValueTreeState::SliderAttachment delayGainKnobAttachment4{processorRef.getValueTreeState(), processorRef.delayRepGain4ParamName, delayGainSlider4};
    juce::AudioProcessorValueTreeState::SliderAttachment delayGainKnobAttachment5{processorRef.getValueTreeState(), processorRef.delayRepGain5ParamName, delayGainSlider5};
};