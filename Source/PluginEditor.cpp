#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayThingEditor::DelayThingEditor(DelayThingAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Create the delay time knob

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(delayTimeSlider);
    setSize(400, 300);
}

DelayThingEditor::~DelayThingEditor()
{
}

//==============================================================================
void DelayThingEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DelayThingEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto box = getLocalBounds().reduced(20);
    box.removeFromBottom(40);

    const auto width = box.getWidth();
    delayTimeSlider.setBounds(box.removeFromLeft(width / 2).reduced(10));
}