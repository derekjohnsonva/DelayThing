#include "DelayBuffer.h"

DelayBuffer::DelayBuffer()
    : delayBuffer(10, juce::Array<DelaySample>())
{
}

void DelayBuffer::setSize(int numSamples)
{
    // resize the vector holding the juce::Array
    delayBuffer.resize(numSamples);
    writePosition = 0;
}

void DelayBuffer::writeFrom(const juce::AudioBuffer<float> &inputBuffer, int inputChannel, int delaysPerSample)
{
    jassert(inputChannel >= 0 );
    // The way this works, is that we are writing to the delay buffer, which is a vector of juce::Arrays.
    // Each juce::Array represents the output for a sample.
    // Each entry in a juce::Array is a delayed sample with an associated delay number.
    // Thus, for each sample in the inputBuffer, we will go to the corresponding juce::Array in the delayBuffer
    // and add the sample to the juce::Array with the appropriate delay number.
    jassert(inputBuffer.getNumChannels() > inputChannel);
    // loop through all the samples in the inputBuffer
    for (int sample = 0; sample < inputBuffer.getNumSamples(); ++sample)
    {
        // get the input sample
        float inputSample = inputBuffer.getSample(inputChannel, sample);
        // get the delay buffer for the current sample
        juce::Array<DelaySample> &delayBufferForSample = delayBuffer[writePosition];
        // update the write position
        writePosition = (writePosition + 1) % delayBuffer.size();
        delayBufferForSample.add(DelaySample(inputSample, delaysPerSample));
    }
}

void DelayBuffer::addTo(juce::AudioBuffer<float> &outputBuffer, int outputChannel, const  juce::Array<float>& repGains, int delaySizeInSamples)
{
    jassert(outputChannel >= 0 );
    // loop through all the samples in the outputBuffer
    for (int sample = 0; sample < outputBuffer.getNumSamples(); ++sample)
    {
        // get the output sample
        float outputSample = outputBuffer.getSample(outputChannel, sample);
        // get the read position
        int readPosition = writePosition + sample - delaySizeInSamples;
        if (readPosition < 0)
        {
            readPosition += delayBuffer.size();
        }
        // get the delay buffer for the current sample
        juce::Array<DelaySample> &delayBufferForSample = delayBuffer[readPosition];
        // iterate over the delay buffer for the current sample
        for (DelaySample &delaySample : delayBufferForSample) {
            outputSample += delaySample.sample * 1;
            delaySample.reps--;
        }
        delayBufferForSample.removeIf([](DelaySample &delaySample)
                                      { return delaySample.reps <= 0; });
        outputBuffer.setSample(outputChannel, sample, outputSample);
    }
}

DelayBuffer::~DelayBuffer()
= default;
