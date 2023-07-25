#include <catch2/catch_test_macros.hpp>

// #include "catch.hpp" // You need to install Catch2 library to use this header

#include "PluginProcessor.h"

TEST_CASE("writeToDelayBuffer writes samples to delay buffer", "[DelayThing]")
{
    DelayThing delayThing;
    const int channel = 0;
    const int writePosition = 0;
    const int numSamples = 10;
    const int delayBufferSizeInSamples = 15;
    delayThing.setDelayBufferSize(1, delayBufferSizeInSamples * 2);
    delayThing.setDelayBufferSizeInSamples(delayBufferSizeInSamples);
    // make an input buffer with with all ones
    juce::AudioBuffer<float> inputBuffer(1, numSamples);
    inputBuffer.clear();
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer.setSample(channel, i, 1); // Set the sample value in the buffer
    }
    delayThing.writeToDelayBuffer(inputBuffer, channel, writePosition);
    // write another input buffer with all twos
    juce::AudioBuffer<float> inputBuffer2(1, numSamples);
    inputBuffer2.clear();
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer2.setSample(channel, i, 2); // Set the sample value in the buffer
    }
    delayThing.writeToDelayBuffer(inputBuffer2, channel, writePosition + numSamples);
    // check to make sure that the buffer has the correct values
    for (int i = 0; i < delayBufferSizeInSamples; ++i)
    {
        int value = i < 10 && i > 4 ? 1 : 2;
        REQUIRE(delayThing.getDelayBuffer().getSample(channel, i) == value);
    }
}

TEST_CASE("addFromDelayBuffer reads samples from delay buffer", "[DelayThing]")
{
    DelayThing delayThing;
    const int channel = 0;
    const int numSamples = 10;
    const int delayBufferSizeInSamples = 15;
    delayThing.setDelayBufferSize(1, delayBufferSizeInSamples * 2);
    delayThing.getDelayBuffer().clear();
    delayThing.setDelayBufferSizeInSamples(delayBufferSizeInSamples);
    // make an output buffer with with all ones
    juce::AudioBuffer<float> outputBuffer(1, numSamples);
    outputBuffer.clear();
    for (int i = 0; i < numSamples; ++i)
    {
        outputBuffer.setSample(channel, i, 1); // Set the sample value in the buffer
    }
    // make the delayBuffer have all twos
    for (int i = 0; i < delayBufferSizeInSamples; ++i)
    {
        delayThing.getDelayBuffer().setSample(channel, i, 2);
    }
    delayThing.addFromDelayBuffer(outputBuffer, channel, 0);
    // check to make sure that the buffer has the correct values
    for (int i = 0; i < numSamples; ++i)
    {
        // print the value of the sample
        std::cout << outputBuffer.getSample(channel, i) << std::endl;
        REQUIRE(outputBuffer.getSample(channel, i) == 3);
    }
}
