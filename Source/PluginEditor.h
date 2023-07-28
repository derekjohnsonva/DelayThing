#pragma once

#include "PluginProcessor.h"

//==============================================================================
class DelayThingEditor : public juce::AudioProcessorEditor
{
public:
    explicit DelayThingEditor(DelayThingAudioProcessor &);
    ~DelayThingEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayThingAudioProcessor &processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayThingEditor)

    juce::Slider delayTimeSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};
    juce::Slider delayMixSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};
    juce::Slider delayRepsSlider{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow};

    juce::AudioProcessorValueTreeState::SliderAttachment delayTimeKnobAttachment{processorRef.getValueTreeState(), processorRef.delayTimeParamName, delayTimeSlider};
    juce::AudioProcessorValueTreeState::SliderAttachment delayMixKnobAttachment{processorRef.getValueTreeState(), processorRef.delayMixParamName, delayMixSlider};
    juce::AudioProcessorValueTreeState::SliderAttachment delayRepsKnobAttachment{processorRef.getValueTreeState(), processorRef.delayRepsParamName, delayRepsSlider};
};