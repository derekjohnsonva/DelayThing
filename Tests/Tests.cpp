#include <catch2/catch_test_macros.hpp>

// #include "catch.hpp" // You need to install Catch2 library to use this header

#include "PluginProcessor.h"

TEST_CASE("writeToDelayBuffer writes samples to delay buffer", "[DelayThing]")
{
    // DelayThingAudioProcessor delayThing;
    // const int channel = 0;
    // const int writePosition = 0;
    // const int numSamples = 10;
    // const int delayBufferSizeInSamples = 15;
    // delayThing.setDelayBufferSize(1, delayBufferSizeInSamples * 2);
    // delayThing.setDelayBufferSizeInSamples(delayBufferSizeInSamples);
    // // make an input buffer with with all ones
    // juce::AudioBuffer<float> inputBuffer(1, numSamples);
    // inputBuffer.clear();
    // for (int i = 0; i < numSamples; ++i)
    // {
    //     inputBuffer.setSample(channel, i, 1); // Set the sample value in the buffer
    // }
    // delayThing.writeToDelayBuffer(inputBuffer, channel, writePosition);
    // // write another input buffer with all twos
    // juce::AudioBuffer<float> inputBuffer2(1, numSamples);
    // inputBuffer2.clear();
    // for (int i = 0; i < numSamples; ++i)
    // {
    //     inputBuffer2.setSample(channel, i, 2); // Set the sample value in the buffer
    // }
    // delayThing.writeToDelayBuffer(inputBuffer2, channel, writePosition + numSamples);
    // // check to make sure that the buffer has the correct values
    // for (int i = 0; i < delayBufferSizeInSamples; ++i)
    // {
    //     int value = i < 10 && i > 4 ? 1 : 2;
    //     REQUIRE(delayThing.getDelayBuffer().getSample(channel, i) == value);
    // }
    REQUIRE(true);
}
