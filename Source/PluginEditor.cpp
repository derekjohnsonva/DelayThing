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
    addAndMakeVisible(delayMixSlider);
    addAndMakeVisible(delayRepsSlider);

    for (auto slider : delayGainSliders)
    {
        addAndMakeVisible(*slider);
    }
    // int delayReps = static_cast<int>(*p.delayReps);
    // for (int i = 0; i < delayReps; i++)
    // {
    //     addAndMakeVisible(*delayGainSliders[i]);
    // }

    // Start the timer
    float fps = 24.f; // frames per second
    startTimer(static_cast<int>(1000.f / fps));
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
    const auto height = box.getHeight();
    auto knobsBox = box.removeFromTop(height / 2);
    delayTimeSlider.setBounds(knobsBox.removeFromLeft(width / 3).reduced(10));
    delayMixSlider.setBounds(knobsBox.removeFromLeft(width / 3).reduced(10));
    delayRepsSlider.setBounds(knobsBox.removeFromLeft(width / 3).reduced(10));

    auto slidersBox = box.removeFromTop(height / 2);
    int delayReps = static_cast<int>(*processorRef.delayReps);
    for (int i = 0; i < delayReps; i++)
    {
        (*delayGainSliders[i]).setBounds(slidersBox.removeFromLeft(width / delayReps).reduced(10));
    }
}

void DelayThingEditor::timerCallback()
{
    int delayReps = static_cast<int>(*processorRef.delayReps);
    if (delayReps != lastDelayRepsValue)
    {
        // Flag some of the delayReps as not visible
        for (int i = 0; i < processorRef.maxDelayReps; i++)
        {
            if (i < delayReps)
            {
                (*delayGainSliders[i]).setVisible(true);
            }
            else
            {
                (*delayGainSliders[i]).setVisible(false);
            }
        }
        resized();
        repaint();
    }
}