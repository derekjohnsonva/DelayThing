// make a delay buffer class
// This is similar to a juce::AudioBuffer, but it will store a struct called
// a DelaySample. A DelaySample will be a float and an int. The float will
// be the sample value and the int will be how many more times to apply the delay.
// The DelayBuffer will also be able to keep track of the write-position

#pragma once
#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Utils.h"

struct DelaySample
{
    float sample;
    int reps;
    DelaySample() : sample(0), reps(0) {}
    DelaySample(float sampleVal) : sample(sampleVal), reps(0) {}
    DelaySample(const DelaySample &other) = default;
    DelaySample &operator=(const DelaySample &other)
    {
        if (this != &other)
        {
            sample = other.sample;
            reps = other.reps;
        }
        return *this;
    }
};
class DelayBuffer
{
public:
    DelayBuffer();
    ~DelayBuffer();
    void setSize(int numSamples);
    void writeFrom(const juce::AudioBuffer<float> &inputBuffer, int inputChannel);
    void addTo(juce::AudioBuffer<float> &outputBuffer, int outputChannel, int delayReps, const juce::Array<std::atomic<float> *> repGains, Smoother<float> &delaySizeInSamples);

private:
    std::vector<juce::Array<DelaySample>> delayBuffer;
    int writePosition = 0;
};
