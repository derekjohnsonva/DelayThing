#include "DelayBuffer.h"
#include "Utils.h"

DelayBuffer::DelayBuffer()
    : delayBuffer(10, juce::Array<DelaySample>())
{
}

void DelayBuffer::setSize(int numSamples)
{
    // resize the vector holding the juce::Array
    jassert(numSamples >= 0);
    delayBuffer.resize(numSamples);
    writePosition = 0;
}

void DelayBuffer::writeFrom(const juce::AudioBuffer<float> &inputBuffer, int inputChannel)
{
    jassert(inputChannel >= 0);
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
        // update the write-position
        writePosition = (writePosition + 1) % delayBuffer.size();
        DelaySample ds = DelaySample(inputSample);
        delayBufferForSample.add(ds);
    }
}

void DelayBuffer::addTo(juce::AudioBuffer<float> &outputBuffer, int outputChannel, int delayReps, const juce::Array<std::atomic<float> *> repGains, Smoother<float> &delaySizeInSamples)
{
    jassert(outputChannel >= 0);
    // loop through all the samples in the outputBuffer
    for (int sample = 0; sample < outputBuffer.getNumSamples(); ++sample)
    {
        // get the output sample
        float outputSample = outputBuffer.getSample(outputChannel, sample);
        // get the read position
        double readPosition = writePosition + sample - delaySizeInSamples.getVal();
        if (readPosition < 0)
        {
            readPosition += delayBuffer.size();
        }
        readPosition = std::fmod(readPosition, delayBuffer.size());
        // linear interpolation of the read position
        const auto readPositionFloor = std::floor(readPosition);
        auto rPF = static_cast<unsigned long>(readPositionFloor);
        auto rPC = static_cast<unsigned long>(readPositionFloor + 1);
        rPC = rPC % delayBuffer.size();
        auto readPositionFraction = readPosition - readPositionFloor;
        // juce::Array<DelaySample> &delayBufferForSample = delayBuffer[rPF] + readPositionFraction * (delayBuffer[rPC] - delayBuffer[rPF]);
        juce::Array<DelaySample> &delayBufferForSampleF = delayBuffer[rPF];
        juce::Array<DelaySample> &delayBufferForSampleC = delayBuffer[rPC];
        // iterate over the delay buffer for the current sample
        int initialSize = delayBufferForSampleF.size();
        for (int i = 0; i < initialSize; i++)
        {
            auto delaySampleF = delayBufferForSampleF.removeAndReturn(0);
            if (delaySampleF.reps < delayReps)
            {
                float sampleAndGainF = delaySampleF.sample * (*repGains[delaySampleF.reps]);
                outputSample += sampleAndGainF;
                // Get the ceil Delay Sample
                if (i < delayBufferForSampleC.size())
                {
                    auto delaySampleC = delayBufferForSampleC[i];
                    if (delaySampleC.reps < delayReps)
                    {
                        float sampleAndGainC = delaySampleC.sample * (*repGains[delaySampleC.reps]);
                        outputSample += readPositionFraction * (sampleAndGainC - sampleAndGainF);
                    }
                }
                // increment the number of repetitions
                delaySampleF.reps++;
                unsigned long wp = (writePosition + sample) % delayBuffer.size();
                delayBuffer[wp].add(delaySampleF);
            }
        }
        outputBuffer.setSample(outputChannel, sample, outputSample);
    }
}

DelayBuffer::~DelayBuffer() = default;
