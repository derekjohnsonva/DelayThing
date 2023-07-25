#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class DelayThing : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayThing();
    ~DelayThing() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;
    void writeToDelayBuffer(const juce::AudioBuffer<float> &inputBuffer, int channel, int writePosition);
    void addFromDelayBuffer(juce::AudioBuffer<float> &outputBuffer, int channel, int readPosition);
    int setDelayBufferSizeInSamples(int newDelayBufferSizeInSamples);
    int getDelayBufferSizeInSamples() const;
    juce::AudioBuffer<float> getDelayBuffer() const;
    // setDelayBufferSize using channels and samples
    void setDelayBufferSize(int numChannels, int numSamples);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayThing)
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSizeInSamples = 0;
    int delayBufferWritePosition = 0;
};