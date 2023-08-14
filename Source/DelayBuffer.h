// make a delay buffer class
// This is similar to a juce::AudioBuffer, but it will store a struct called
// a DelaySample. A DelaySample will be a float and an int. The float will
// be the sample value and the int will be how many more times to apply the delay.
// The DelayBuffer will also be able to keep track of the write position

#pragma once
#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

struct DelaySample
{
    float sample;
    int reps;
};

class DelayBuffer
{
public:
    DelayBuffer();
    // DelayBuffer(int numChannels, int numSamples);
    ~DelayBuffer();
    void setDelayBufferSize(int numChannels, int numSamples);
    void writeToDelayBuffer(const juce::AudioBuffer<float> &inputBuffer, int channel, int writePosition);
    void addFromDelayBuffer(juce::AudioBuffer<float> &outputBuffer, int channel, int readPosition);

private:
    std::vector<std::vector<std::vector<DelaySample>>> delayBuffer;
    int writePosition = 0;
};
