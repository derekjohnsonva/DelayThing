#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class DelayThingAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    DelayThingAudioProcessor();
    ~DelayThingAudioProcessor() override;

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
    void setDelayBufferSample(int channel, int sample, float value);
    juce::AudioBuffer<float> getDelayBuffer() const;
    // setDelayBufferSize using channels and samples
    void setDelayBufferSize(int numChannels, int numSamples);

    // Listener for the parameters
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState &getValueTreeState();
    void updateDelayBufferSizeInSamples(float delaySizeInMS);

    // The parameter name constants
    const juce::String delayTimeParamName = "delayTime";
    const juce::String delayMixParamName = "delayMix";
    const juce::String delayRepsParamName = "delayReps";

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayThingAudioProcessor)
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSizeInSamples = 0;
    // a vector of the last delay times in samples
    std::vector<int> delayBufferWritePositions;

    juce::UndoManager undoManager;

    // Parameters for the plugin
    juce::AudioProcessorValueTreeState parameters;
    // The delay time in milliseconds
    std::atomic<float> *delayTime = nullptr;
    // The delay mix
    std::atomic<float> *delayMix = nullptr;
    // The number of times each delay is repeated
    std::atomic<float> *delayReps = nullptr;
};