#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayThing::DelayThing()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
{
}

DelayThing::~DelayThing() = default;

//==============================================================================
const juce::String DelayThing::getName() const
{
    return JucePlugin_Name;
}

bool DelayThing::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayThing::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DelayThing::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DelayThing::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayThing::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DelayThing::getCurrentProgram()
{
    return 0;
}

void DelayThing::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String DelayThing::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void DelayThing::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void DelayThing::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    // we will need our buffer to be able to hold 2 seconds of audio data
    // First, get the number of samples in 2 seconds of audio
    int numSamples = (int)(2.0 * sampleRate);
    delayBuffer.setSize(getTotalNumOutputChannels(), numSamples);

    // TODO: make it so that the delay length can be changed
    // For now, set delay to be 0.25 seconds
    delayBufferSizeInSamples = (int)(0.25 * sampleRate);
}

void DelayThing::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool DelayThing::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void DelayThing::processBlock(juce::AudioBuffer<float> &buffer,
                              juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // add the channel data to the delay buffer
        writeToDelayBuffer(buffer, channel, delayBufferWritePosition);
        delayBufferWritePosition += buffer.getNumSamples();
        // calculate the read position in the delay buffer
        int readPosition = delayBufferWritePosition - delayBufferSizeInSamples;
        // if the read position is negative, wrap it around to the end of the buffer
        if (readPosition < 0)
        {
            readPosition += delayBufferSizeInSamples;
        }
        // read from the delay buffer
        addFromDelayBuffer(buffer, channel, readPosition);
    }
}

void DelayThing::writeToDelayBuffer(const juce::AudioBuffer<float> &inputBuffer, int channel, int writePosition)
{
    // check to see if when we write to the delay buffer we will be writing past the end
    // of the buffer
    // if we are, we need to wrap around to the beginning of the buffer
    if (writePosition + inputBuffer.getNumSamples() >= delayBufferSizeInSamples)
    {
        // calculate the number of samples we will write to the end of the buffer
        int numSamplesToEnd = delayBufferSizeInSamples - writePosition;
        // write the samples to the end of the buffer
        delayBuffer.copyFrom(channel, writePosition, inputBuffer, channel, 0, numSamplesToEnd);
        // calculate the number of samples we will write to the beginning of the buffer
        int numSamplesFromStart = inputBuffer.getNumSamples() - numSamplesToEnd;
        // write the samples to the beginning of the buffer
        delayBuffer.copyFrom(channel, 0, inputBuffer, channel, numSamplesToEnd, numSamplesFromStart);
    }
    else
    {
        // if we are not writing past the end of the buffer, just write the samples
        // to the buffer
        delayBuffer.copyFrom(channel, writePosition, inputBuffer, channel, 0, inputBuffer.getNumSamples());
    }
}

void DelayThing::addFromDelayBuffer(juce::AudioBuffer<float> &outputBuffer, int channel, int readPosition)
{
    // check to see if when we read from the delay buffer we will be reading past the end
    // of the buffer
    // if we are, we need to wrap around to the beginning of the buffer
    if (readPosition + outputBuffer.getNumSamples() >= delayBufferSizeInSamples)
    {
        // calculate the number of samples we will read from the end of the buffer
        int numSamplesToEnd = delayBufferSizeInSamples - readPosition;
        // read the samples from the end of the buffer
        outputBuffer.addFrom(channel, 0, delayBuffer, channel, readPosition, numSamplesToEnd);
        // calculate the number of samples we will read from the beginning of the buffer
        int numSamplesFromStart = outputBuffer.getNumSamples() - numSamplesToEnd;
        // read the samples from the beginning of the buffer
        outputBuffer.addFrom(channel, numSamplesToEnd, delayBuffer, channel, 0, numSamplesFromStart);
    }
    else
    {
        // if we are not reading past the end of the buffer, just read the samples
        // from the buffer
        outputBuffer.addFrom(channel, 0, delayBuffer, channel, readPosition, outputBuffer.getNumSamples());
    }
}

//==============================================================================
bool DelayThing::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *DelayThing::createEditor()
{
    return new DelayThingEditor(*this);
}

//==============================================================================
void DelayThing::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void DelayThing::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new DelayThing();
}
