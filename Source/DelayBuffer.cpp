#include "DelayBuffer.h"

DelayBuffer::DelayBuffer()
    : delayBuffer(2, std::vector<std::vector<DelaySample>>(2, std::vector<DelaySample>(2)))
{
}

void DelayBuffer::setDelayBufferSize(int numChannels, int numSamples)
{
    // unimplmented
}

void DelayBuffer::writeToDelayBuffer(const juce::AudioBuffer<float> &inputBuffer, int channel, int writePosition)
{
    // unimplemented
}

void DelayBuffer::addFromDelayBuffer(juce::AudioBuffer<float> &outputBuffer, int channel, int readPosition)
{
    // unimplemented
}
